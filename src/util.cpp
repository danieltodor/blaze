#include <regex>
#include <sys/ioctl.h>
#include <locale>
#include <codecvt>
#include <cstring>

#include "util.hpp"

unsigned short get_column()
{
    winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_col;
}

std::size_t get_length(std::vector<std::string> strings)
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
    char temp[get_length({string})];
    string.copy(temp, string.length());
    char *ptr;
    ptr = std::strtok(temp, delimiter.c_str());
    while (ptr != NULL)
    {
        result.push_back(ptr);
        ptr = std::strtok(NULL, delimiter.c_str());
    }
    return result;
}

void regex_replace(std::string &string, const std::vector<std::string> patterns, const std::string &replacement)
{
    std::string temp;
    std::vector<std::string> lines = split(string, "\n");
    for (const std::string &line : lines)
    {
        for (const std::string &pattern : patterns)
        {
            temp = std::regex_replace(line, std::regex(pattern), replacement);
        }
    }
    string = temp;
}

bool regex_search(std::string &string, const std::vector<std::string> patterns)
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

std::string execute_command(std::string command)
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

bool git_repository_detected()
{
    return execute_command("git rev-parse 2>&1").empty() ? true : false;
}
