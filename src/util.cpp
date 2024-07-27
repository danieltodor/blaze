#include <regex>
#include <sys/ioctl.h>
#include <locale>
#include <codecvt>

#include "external/boost/regex.hpp"
#include "src/util.hpp"

unsigned short get_columns()
{
    winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_col;
}

std::size_t get_length(const std::vector<std::string> &strings)
{
    // TODO: For certain emojis/symbols, the returned length is not correct.
    // Maybe counting grapheme clusters would be a better approach.
    std::size_t length = 0;
    for (const std::string &string : strings)
    {
        length += std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(string).length();
    }
    return length;
}

std::size_t find_nth_occurrence(const std::string &string, const std::string &substring, const std::size_t n, const bool reverse)
{
    std::size_t index = reverse ? string.length() - 1 : 0;
    const std::size_t end = reverse ? 0 : string.length() - 1;
    const short sign = reverse ? -1 : 1;
    std::size_t count = 0;
    std::size_t substring_size = substring.length();
    while (count < n && index != end)
    {
        const std::string s = string.substr(index, substring_size);
        if (s == substring)
        {
            count++;
        }
        index += sign * 1;
    }
    if (count == n)
    {
        return index - sign;
    }
    else
    {
        return std::string::npos;
    }
}

std::vector<std::string> split(const std::string &string, const std::string &delimiter)
{
    std::vector<std::string> result;
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

std::string join(const std::vector<std::string> &strings, const std::string &delimiter)
{
    std::string result = "";
    if (strings.empty())
    {
        return result;
    }
    for (std::size_t i = 0; i < strings.size() - 1; i++)
    {
        result += strings.at(i);
        result += delimiter;
    }
    result += strings.at(strings.size() - 1);
    return result;
}

void regex_replace(std::string &string, const std::vector<std::string> &patterns, const std::string &replacement)
{
    const std::string pattern = join(patterns, "|");
    string = boost::regex_replace(string, boost::regex(pattern), replacement);
}

std::vector<std::string> regex_find_all(const std::string &string, const std::vector<std::string> &patterns)
{
    std::vector<std::string> result;
    const std::string pattern = join(patterns, "|");
    if (pattern.empty())
    {
        return result;
    }
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

std::string execute_command(const std::string &command, int *exit_status)
{
    std::string result = "";
    const std::string prefix = "LC_ALL=C; ";
    const std::string suffix = " 2>/dev/null";
    char buffer[128];
    FILE *pipe = popen((prefix + command + suffix).c_str(), "r");
    if (!pipe)
    {
        return result;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        result += buffer;
    }
    const int status = WEXITSTATUS(pclose(pipe));
    if (exit_status != NULL)
    {
        *exit_status = status;
    }
    return result;
}

bool check_git_repository()
{
    int exit_status = 0;
    execute_command("git rev-parse", &exit_status);
    return exit_status == 0 ? true : false;
}

std::string get_env(const std::string &name)
{
    std::string env = "";
    const char *result = std::getenv(name.c_str());
    if (result != NULL)
    {
        env = result;
    }
    return env;
}

std::tm get_current_time()
{
    std::time_t now = std::time(NULL);
    return *std::localtime(&now);
}

std::string format_time(const std::tm &time_structure, const std::string &format)
{
    std::string result = "";
    char buffer[128];
    std::strftime(buffer, sizeof(buffer), format.c_str(), &time_structure);
    result += buffer;
    return result;
}

bool is_number(const std::string &string)
{
    if (string.empty())
    {
        return false;
    }
    for (const unsigned char c : string)
    {
        if (!std::isdigit(c))
        {
            return false;
        }
    }
    return true;
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("get_length")
{
    CHECK(get_length({"123"}) == 3);
    CHECK(get_length({"abc"}) == 3);
    CHECK(get_length({"abc", "def"}) == 6);
    CHECK(get_length({"abc", "def", "123"}) == 9);
    CHECK(get_length({""}) == 1);
}

TEST_CASE("find_nth_occurrence")
{
    SUBCASE("single character")
    {
        CHECK(find_nth_occurrence("abcdefg", "h", 1) == std::string::npos);
        CHECK(find_nth_occurrence("abcdefgh", "h", 2) == std::string::npos);
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
        const std::vector<std::string> result = split("a/s/d", "/");
        CHECK(result.size() == 3);
        CHECK(result[0] == "a");
        CHECK(result[1] == "s");
        CHECK(result[2] == "d");
    }
    SUBCASE("newline")
    {
        const std::vector<std::string> result = split("\na\ns\nd\n", "\n");
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
    SUBCASE("1")
    {
        std::vector<std::string> result = regex_find_all("abc123def", {"123"});
        CHECK(result.size() == 1);
        CHECK(result[0] == "123");
    }
    SUBCASE("2")
    {
        std::vector<std::string> result = regex_find_all("abc123def", {"\\d"});
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
        std::string string = "\n  abc\n\n";
        strip(string);
        CHECK(string == "abc");
    }
}

TEST_CASE("execute_command")
{
    CHECK(execute_command("echo abc") == "abc\n");
    SUBCASE("exit status")
    {
        int exit_status = 0;
        CHECK(execute_command("non_existing_command", &exit_status) == "");
        CHECK(exit_status == 127);
    }
}

TEST_CASE("check_git_repository")
{
    CHECK(check_git_repository() == true);
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
