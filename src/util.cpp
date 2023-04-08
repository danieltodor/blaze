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
    // TODO: For certain emojis, the returned length is not correct.
    // Maybe counting grapheme clusters would be a better approach.
    std::size_t length = 0;
    for (const std::string &string : strings)
    {
        length += std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(string).size();
    }
    return length;
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

std::string execute_command(const std::string &command)
{
    std::string result = "";
    const std::string env = "LC_ALL=C";
    char buffer[128];
    FILE *pipe = popen((env + "; " + command).c_str(), "r");
    if (!pipe)
    {
        return result;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

bool check_git_repository()
{
    return execute_command("git rev-parse 2>&1").empty() ? true : false;
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

TEST_CASE("get_length")
{
    CHECK(get_length({"123"}) == 3);
    CHECK(get_length({"abc"}) == 3);
    CHECK(get_length({"abc", "def"}) == 6);
    CHECK(get_length({"abc", "def", "123"}) == 9);
    CHECK(get_length({""}) == 1);
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
}

TEST_CASE("check_git_repository")
{
    CHECK(check_git_repository() == true);
}

TEST_CASE("get_env")
{
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
