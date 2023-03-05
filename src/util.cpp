#include <regex>
#include <sys/ioctl.h>
#include <locale>
#include <codecvt>

#include "util.hpp"
#include "external/boost/regex.hpp"

unsigned short get_column()
{
    winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_col;
}

std::size_t get_length(const std::vector<std::string> &strings)
{
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
    char buffer[128];
    FILE *pipe = popen(command.c_str(), "r");
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
    std::string env;
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
