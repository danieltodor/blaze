#include <regex>
#include <sys/ioctl.h>
#include <locale>
#include <codecvt>

#include "util.hpp"

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
    return result;
}

std::string join(const std::vector<std::string> &strings, const std::string &delimiter)
{
    std::string result;
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
    std::vector<std::string> lines = split(string, "\n");
    for (std::string &line : lines)
    {
        for (const std::string &pattern : patterns)
        {
            line = std::regex_replace(line, std::regex(pattern), replacement);
        }
    }
    string = join(lines, "\n");
}

bool regex_search(const std::string &string, const std::vector<std::string> &patterns)
{
    std::vector<std::string> lines = split(string, "\n");
    for (const std::string &line : lines)
    {
        for (const std::string &pattern : patterns)
        {
            if (std::regex_search(line, std::regex(pattern)))
            {
                return true;
            }
        }
    }
    return false;
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
