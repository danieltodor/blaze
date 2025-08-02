#include "boost/regex.hpp"
#include "util.hpp"

winsize get_winsize()
{
    winsize win;
    ioctl(0, TIOCGWINSZ, &win);
    return win;
}

std::size_t get_length(const StringVector &strings)
{
    // Naive approach of counting grapheme clusters in emojis
    // Might return incorrect length on some
    #define ZWJ 0x200D
    const auto is_combining_mark = [](const unsigned int codepoint)
    {
        return (codepoint >= 0x0300 && codepoint <= 0x036F) ||
               (codepoint >= 0x1AB0 && codepoint <= 0x1AFF) ||
               (codepoint >= 0x1DC0 && codepoint <= 0x1DFF) ||
               (codepoint >= 0x20D0 && codepoint <= 0x20FF) ||
               (codepoint >= 0xFE20 && codepoint <= 0xFE2F);
    };
    const auto next_codepoint = [](const std::string &string, std::size_t &index)
    {
        unsigned char character = string[index];
        unsigned int result;
        if (character < 0x80)
        {
            result = string[index++];
        }
        else if ((character >> 5) == 0x6)
        {
            result = ((string[index] & 0x1F) << 6) | (string[index + 1] & 0x3F);
            index += 2;
        }
        else if ((character >> 4) == 0xE)
        {
            result = ((string[index] & 0x0F) << 12) | ((string[index + 1] & 0x3F) << 6) | (string[index + 2] & 0x3F);
            index += 3;
        }
        else if ((character >> 3) == 0x1E)
        {
            result = ((string[index] & 0x07) << 18) | ((string[index + 1] & 0x3F) << 12) |
                     ((string[index + 2] & 0x3F) << 6) | (string[index + 3] & 0x3F);
            index += 4;
        }
        else
        {
            // Invalid UTF-8, skip one byte
            result = string[index++];
        }
        return result;
    };
    std::size_t length = 0;
    for (const std::string &string : strings)
    {
        std::size_t index = 0;
        bool in_zero_width_joiner_sequence = false;
        while (index < string.length())
        {
            unsigned int codepoint = next_codepoint(string, index);
            if (is_combining_mark(codepoint))
            {
                continue;
            }
            else if (!in_zero_width_joiner_sequence)
            {
                ++length;
            }
            if (index < string.length())
            {
                std::size_t lookahead_index = index;
                codepoint = next_codepoint(string, lookahead_index);
                if (codepoint == ZWJ)
                {
                    in_zero_width_joiner_sequence = true;
                    index = lookahead_index;
                }
                else
                {
                    in_zero_width_joiner_sequence = false;
                }
            }
            else
            {
                in_zero_width_joiner_sequence = false;
            }
        }
    }
    return length;
}

std::size_t find_nth_occurrence(const std::string &string, const std::string &substring, const std::size_t n, const bool reverse)
{
    if (string.empty() || substring.empty() || n == 0)
    {
        return std::string::npos;
    }
    std::size_t index = reverse ? string.length() - 1 : 0;
    const std::size_t end = reverse ? 0 : string.length() - 1;
    const short increment = reverse ? -1 : 1;
    std::size_t count = 0;
    std::size_t substring_size = substring.length();
    while (count < n && index != end)
    {
        const std::string s = string.substr(index, substring_size);
        if (s == substring)
        {
            count++;
        }
        index += increment;
    }
    if (count == n)
    {
        return index - increment;
    }
    else
    {
        return std::string::npos;
    }
}

StringVector split(const std::string &string, const std::string &delimiter)
{
    StringVector result;
    std::size_t from = 0;
    std::size_t to;
    const std::size_t delimiter_length = delimiter.length();
    while ((to = string.find(delimiter, from)) != std::string::npos)
    {
        result.push_back(string.substr(from, to - from));
        from = to + delimiter_length;
    }
    // Delimiter not found
    if (from == 0 && to == std::string::npos)
    {
        result.push_back(string);
    }
    else
    {
        result.push_back(string.substr(from, to - from));
    }
    return result;
}

std::string join(const StringVector &strings, const std::string &delimiter)
{
    if (strings.empty())
    {
        return "";
    }
    std::string result = "";
    for (std::size_t i = 0; i < strings.size() - 1; i++)
    {
        result += strings.at(i);
        result += delimiter;
    }
    result += strings.at(strings.size() - 1);
    return result;
}

std::string multiply_string(const int number, const std::string &string)
{
    std::string result = "";
    for (int i = 0; i < number; i++)
    {
        result += string;
    }
    return result;
}

void regex_replace(std::string &string, const StringVector &patterns, const std::string &replacement)
{
    const std::string pattern = join(patterns, "|");
    string = boost::regex_replace(string, boost::regex(pattern), replacement);
}

StringVector regex_find_all(const std::string &string, const StringVector &patterns)
{
    if (string.empty() || patterns.empty())
    {
        return {};
    }
    StringVector result;
    const std::string pattern = join(patterns, "|");
    boost::sregex_token_iterator current(string.begin(), string.end(), boost::regex(pattern));
    const boost::sregex_token_iterator end;
    while (current != end)
    {
        result.push_back(*current);
        current++;
    }
    return result;
}

void strip(std::string &string)
{
    regex_replace(
        string,
        {
            "^\\s*",
            "\\s*$"
        },
        ""
    );
}

std::string execute_command(const std::string &command, int *status)
{
    const std::string prefix = "LC_ALL=C; ";
    const std::string suffix = " 2>/dev/null";
    FILE *pipe = popen((prefix + command + suffix).c_str(), "r");
    if (!pipe)
    {
        return "";
    }
    std::string result = "";
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }
    const int child_status = pclose(pipe);
    const int exit_status = WEXITSTATUS(child_status);
    if (status != nullptr)
    {
        *status = exit_status;
    }
    return result;
}

void check_git_repository(bool &detected, bool &detached)
{
    int status = 0;
    std::string result = execute_command("git branch --show-current", &status);
    strip(result);
    detected = status == 0 ? true : false;
    detached = detected && result.empty() ? true : false;
}

std::string get_env(const std::string &name)
{
    const char *result = std::getenv(name.c_str());
    if (result == nullptr)
    {
        return "";
    }
    return result;
}

std::tm get_current_time()
{
    std::time_t now = std::time(nullptr);
    return *std::localtime(&now);
}

std::string format_time(const std::tm &time_structure, const std::string &format)
{
    char buffer[128];
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-nonliteral"
    std::strftime(buffer, sizeof(buffer), format.c_str(), &time_structure);
    #pragma GCC diagnostic pop
    return buffer;
}

bool is_number(const std::string &string)
{
    if (string.empty())
    {
        return false;
    }
    for (const char c : string)
    {
        if (!std::isdigit(c))
        {
            return false;
        }
    }
    return true;
}

// ----------------------------------- TESTS -----------------------------------
#ifdef TEST
#include "test.hpp"

TEST_CASE("get_winsize")
{
    // It can fail if the terminal window is small
    CHECK(get_winsize().ws_col > 10);
    CHECK(get_winsize().ws_row > 10);
}

TEST_CASE("get_length")
{
    CHECK(get_length({"123"}) == 3);
    CHECK(get_length({"abc"}) == 3);
    CHECK(get_length({"abc", "def"}) == 6);
    CHECK(get_length({"abc", "def", "123"}) == 9);
    CHECK(get_length({"abcéáőúűóü,./;'[]|()_+="}) == 23);
    CHECK(get_length({"éáő", "űóü", "ú"}) == 7);
    CHECK(get_length({""}) == 1);
    CHECK(get_length({""}) == 1);
    CHECK(get_length({"⌛︎"}) == 2); // Still acceptable for now
    CHECK(get_length({"⚠"}) == 1);
    CHECK(get_length({""}) == 1);
    CHECK(get_length({""}) == 1);
    CHECK(get_length({"👩‍👩‍👦‍👦"}) == 1);
    CHECK(get_length({"👩‍👩‍👦"}) == 1);
    CHECK(get_length({"👩"}) == 1);
}

TEST_CASE("find_nth_occurrence")
{
    SUBCASE("empty")
    {
        CHECK(find_nth_occurrence("", "", 0) == std::string::npos);
        CHECK(find_nth_occurrence("a", "", 0) == std::string::npos);
        CHECK(find_nth_occurrence("", "a", 0) == std::string::npos);
        CHECK(find_nth_occurrence("", "", 0, true) == std::string::npos);
        CHECK(find_nth_occurrence("a", "", 0, true) == std::string::npos);
        CHECK(find_nth_occurrence("", "a", 0, true) == std::string::npos);
    }
    SUBCASE("single character")
    {
        CHECK(find_nth_occurrence("abcdefg", "h", 1) == std::string::npos);
        CHECK(find_nth_occurrence("abcdefgh", "h", 2) == std::string::npos);
        CHECK(find_nth_occurrence("abcdefg", "h", 1, true) == std::string::npos);
        CHECK(find_nth_occurrence("abcdefgh", "h", 2, true) == std::string::npos);
        CHECK(find_nth_occurrence("abcdefg", "c", 1) == 2);
        CHECK(find_nth_occurrence("abcdefg", "c", 1, true) == 2);
        CHECK(find_nth_occurrence("abcdefcg", "c", 1, true) == 6);
        CHECK(find_nth_occurrence("abcdefcg", "c", 2, true) == 2);
        CHECK(find_nth_occurrence("abcdefcg", "c", 2) == 6);
    }
    SUBCASE("multiple characters")
    {
        CHECK(find_nth_occurrence("aaabbbcccdddeeefffggg", "asd", 1) == std::string::npos);
        CHECK(find_nth_occurrence("aaabbbasdcccdddeeefffggg", "asd", 2) == std::string::npos);
        CHECK(find_nth_occurrence("aaabbbcccdddeeefffggg", "asd", 1, true) == std::string::npos);
        CHECK(find_nth_occurrence("aaabbbasdcccdddeeefffggg", "asd", 2, true) == std::string::npos);
        CHECK(find_nth_occurrence("aaabbbcccdddeeeasdfffggg", "asd", 1) == 15);
        CHECK(find_nth_occurrence("aaabbbcccdddeeeasdfffggg", "asd", 1, true) == 15);
        CHECK(find_nth_occurrence("aaabbbasdcccdddeeeasdfffggg", "asd", 1, true) == 18);
        CHECK(find_nth_occurrence("aaabbbcccdddeeeasdfffgggasd", "asd", 2, true) == 15);
        CHECK(find_nth_occurrence("aaabbbcccdddeeeasdfffgggasd", "asd", 2) == 24);
    }
}

TEST_CASE("split")
{
    SUBCASE("slash")
    {
        const StringVector result = split("a/s/d", "/");
        CHECK(result.size() == 3);
        CHECK(result[0] == "a");
        CHECK(result[1] == "s");
        CHECK(result[2] == "d");
    }
    SUBCASE("newline")
    {
        const StringVector result = split("\na\ns\nd\n", "\n");
        CHECK(result.size() == 5);
        CHECK(result[0] == "");
        CHECK(result[1] == "a");
        CHECK(result[2] == "s");
        CHECK(result[3] == "d");
        CHECK(result[4] == "");
    }
}

TEST_CASE("join")
{
    CHECK(join({"abc", "def"}, "") == "abcdef");
    CHECK(join({"abc", "def"}, "\n") == "abc\ndef");
    CHECK(join({"abc", "def\n"}, "\n") == "abc\ndef\n");
    CHECK(join({"abc", "def", "\n"}, "\n") == "abc\ndef\n\n");
}

TEST_CASE("multiply_string")
{
    CHECK(multiply_string(0, "a") == "");
    CHECK(multiply_string(3, "a") == "aaa");
    CHECK(multiply_string(3, "ab") == "ababab");
}

TEST_CASE("regex_replace")
{
    SUBCASE("1")
    {
        std::string string = "abc";
        regex_replace(string, {"b"}, "");
        CHECK(string == "ac");
    }
    SUBCASE("2")
    {
        std::string string = "a,b,c";
        regex_replace(string, {","}, ".");
        CHECK(string == "a.b.c");
    }
    SUBCASE("3")
    {
        std::string string = "abc/123..def";
        regex_replace(string, {"/", "\\.\\."}, "-");
        CHECK(string == "abc-123-def");
    }
}

TEST_CASE("regex_find_all")
{
    SUBCASE("empty string")
    {
        StringVector result = regex_find_all("", {"123"});
        CHECK(result.size() == 0);
    }
    SUBCASE("empty pattern")
    {
        StringVector result = regex_find_all("abc", {});
        CHECK(result.size() == 0);
    }
    SUBCASE("1")
    {
        StringVector result = regex_find_all("abc123def", {"123"});
        CHECK(result.size() == 1);
        CHECK(result[0] == "123");
    }
    SUBCASE("2")
    {
        StringVector result = regex_find_all("abc123def", {"\\d"});
        CHECK(result.size() == 3);
        CHECK(result[0] == "1");
        CHECK(result[1] == "2");
        CHECK(result[2] == "3");
    }
}

TEST_CASE("strip")
{
    SUBCASE("spaces and newlines")
    {
        std::string string = "\n  a bc\n\n";
        strip(string);
        CHECK(string == "a bc");
    }
}

TEST_CASE("execute_command")
{
    SUBCASE("good command without status")
    {
        CHECK(execute_command("echo abc") == "abc\n");
    }
    SUBCASE("good command with status")
    {
        int status = 0;
        CHECK(execute_command("echo abc") == "abc\n");
        CHECK(status == 0);
    }
    SUBCASE("bad command with status")
    {
        int status = 0;
        CHECK(execute_command("non_existing_command", &status) == "");
        CHECK(status == 127);
    }
}

TEST_CASE("check_git_repository")
{
    bool detected = false;
    bool detached = false;
    check_git_repository(detected, detached);
    CHECK(detected == true);
    CHECK(detached == false);
}

TEST_CASE("get_env")
{
    CHECK(get_env("ASDF") == "");
    CHECK(get_env("HOME").find("/home/") != std::string::npos);
}

TEST_CASE("get_current_time")
{
    std::tm time = get_current_time();
    CHECK(time.tm_year > 120);
}

TEST_CASE("format_time")
{
    CHECK(format_time(get_current_time(), "%F").find("-") != std::string::npos);
    CHECK(format_time(get_current_time(), "%T").find(":") != std::string::npos);
}

TEST_CASE("is_number")
{
    CHECK(is_number("") == false);
    CHECK(is_number("abc") == false);
    CHECK(is_number("123a") == false);
    CHECK(is_number("a123") == false);
    CHECK(is_number("123") == true);
}

#endif
