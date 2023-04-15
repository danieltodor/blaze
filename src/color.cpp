#include <unordered_map>

#include "src/color.hpp"
#include "src/util.hpp"

const std::string csi = "\033["; // Control Sequence Introducer
const std::string foreground_rgb_prefix = "38;2;";
const std::string background_rgb_prefix = "48;2;";
const std::string foreground_id_prefix = "38;5;";
const std::string background_id_prefix = "48;5;";
const std::unordered_map<std::string, std::string> sequence_begin = {
    {"bash", "\001"}, // \[
    {"fish", "\001"}, // \[
    {"zsh", "%{"}
};
const std::unordered_map<std::string, std::string> sequence_end = {
    {"bash", "\002"}, // \]
    {"fish", "\002"}, // \]
    {"zsh", "%}"}
};
const std::unordered_map<std::string, int> foreground_color_map = {
    {"black", BLACK},
    {"red", RED},
    {"green", GREEN},
    {"yellow", YELLOW},
    {"blue", BLUE},
    {"magenta", MAGENTA},
    {"cyan", CYAN},
    {"white", WHITE},
    {"bright_black", BRIGHT_BLACK},
    {"bright_red", BRIGHT_RED},
    {"bright_green", BRIGHT_GREEN},
    {"bright_yellow", BRIGHT_YELLOW},
    {"bright_blue", BRIGHT_BLUE},
    {"bright_magenta", BRIGHT_MAGENTA},
    {"bright_cyan", BRIGHT_CYAN},
    {"bright_white", BRIGHT_WHITE}
};

// Create escape sequence
std::string create_sequence(const std::string &code, const Context &context)
{
    std::string result = "";
    result += sequence_begin.at(context.args.shell);
    result += csi;
    result += code;
    result += 'm';
    result += sequence_end.at(context.args.shell);
    return result;
}

// Convert color to escape sequence
std::string to_color_code(const std::string &color, const int offset, const Context &context)
{
    if (color.empty())
    {
        return "";
    }
    else if (foreground_color_map.find(color) != foreground_color_map.end())
    {
        const int code = foreground_color_map.at(color);
        return create_sequence(std::to_string(code + offset), context);
    }
    else if (is_number(color))
    {
        const std::string id_prefix = offset == 0 ? foreground_id_prefix : background_id_prefix;
        return create_sequence(id_prefix + color, context);
    }
    else
    {
        const std::string rgb_prefix = offset == 0 ? foreground_rgb_prefix : background_rgb_prefix;
        std::string rgb_color = color;
        if (color == "default")
        {
            rgb_color = context.args.background;
        }
        return create_sequence(rgb_prefix + rgb_color, context);
    }
}

std::string set_text_mode(const int code, const Context &context)
{
    return create_sequence(std::to_string(code), context);
}

std::string set_foreground(const std::string &color, const Context &context)
{
    return to_color_code(color, 0, context);
}

std::string set_background(const std::string &color, const Context &context)
{
    return to_color_code(color, BACKGROUND_COLOR_OFFSET, context);
}

std::string reset_all(const Context &context)
{
    return create_sequence(std::to_string(RESET), context);
}

std::string reset_text_mode(const int code, const Context &context)
{
    int special_offset = 0;
    if (code == BOLD)
    {
        special_offset = 1;
    }
    return create_sequence(std::to_string(code + TEXT_TYPE_RESET_OFFSET + special_offset), context);
}

std::string reset_foreground(const Context &context)
{
    return create_sequence(std::to_string(DEFAULT_COLOR), context);
}

std::string reset_background(const Context &context)
{
    return create_sequence(std::to_string(DEFAULT_COLOR + BACKGROUND_COLOR_OFFSET), context);
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("create_sequence")
{
    Context context;
    SUBCASE("bash")
    {
        context.args.shell = "bash";
        const std::string result = create_sequence("32", context);
        CHECK(result.find("\001\033[32m\002") != std::string::npos);
    }
    SUBCASE("fish")
    {
        context.args.shell = "fish";
        const std::string result = create_sequence("32", context);
        CHECK(result.find("\001\033[32m\002") != std::string::npos);
    }
    SUBCASE("zsh")
    {
        context.args.shell = "zsh";
        const std::string result = create_sequence("32", context);
        CHECK(result.find("%{\033[32m%}") != std::string::npos);
    }
}

TEST_CASE("to_color_code")
{
    Context context;
    context.args.shell = "bash";
    SUBCASE("empty")
    {
        const std::string result = to_color_code("", 0, context);
        CHECK(result == "");
    }
    SUBCASE("foreground named")
    {
        const std::string result = to_color_code("green", 0, context);
        CHECK(result.find("\001\033[32m\002") != std::string::npos);
    }
    SUBCASE("background named")
    {
        const std::string result = to_color_code("green", 10, context);
        CHECK(result.find("\001\033[42m\002") != std::string::npos);
    }
    SUBCASE("foreground color id")
    {
        const std::string result = to_color_code("128", 0, context);
        CHECK(result.find("\001\033[38;5;128m\002") != std::string::npos);
    }
    SUBCASE("background color id")
    {
        const std::string result = to_color_code("128", 10, context);
        CHECK(result.find("\001\033[48;5;128m\002") != std::string::npos);
    }
    SUBCASE("foreground rgb")
    {
        const std::string result = to_color_code("30;40;50", 0, context);
        CHECK(result.find("\001\033[38;2;30;40;50m\002") != std::string::npos);
    }
    SUBCASE("background rgb")
    {
        const std::string result = to_color_code("30;40;50", 10, context);
        CHECK(result.find("\001\033[48;2;30;40;50m\002") != std::string::npos);
    }
}

TEST_CASE("set_text_mode")
{
    Context context;
    context.args.shell = "bash";
    const std::string result = set_text_mode(1, context);
    CHECK(result.find("\001\033[1m\002") != std::string::npos);
}

TEST_CASE("set_foreground")
{
    Context context;
    context.args.shell = "bash";
    const std::string result = set_foreground("green", context);
    CHECK(result.find("\001\033[32m\002") != std::string::npos);
}

TEST_CASE("set_background")
{
    Context context;
    context.args.shell = "bash";
    const std::string result = set_background("green", context);
    CHECK(result.find("\001\033[42m\002") != std::string::npos);
}

TEST_CASE("reset_all")
{
    Context context;
    context.args.shell = "bash";
    const std::string result = reset_all(context);
    CHECK(result.find("\001\033[0m\002") != std::string::npos);
}

TEST_CASE("reset_text_mode")
{
    Context context;
    context.args.shell = "bash";
    SUBCASE("bold")
    {
        const std::string result = reset_text_mode(1, context);
        CHECK(result.find("\001\033[22m\002") != std::string::npos);
    }
    SUBCASE("dim")
    {
        const std::string result = reset_text_mode(2, context);
        CHECK(result.find("\001\033[22m\002") != std::string::npos);
    }
    SUBCASE("italic")
    {
        const std::string result = reset_text_mode(3, context);
        CHECK(result.find("\001\033[23m\002") != std::string::npos);
    }
}

TEST_CASE("reset_foreground")
{
    Context context;
    context.args.shell = "bash";
    const std::string result = reset_foreground(context);
    CHECK(result.find("\001\033[39m\002") != std::string::npos);
}

TEST_CASE("reset_background")
{
    Context context;
    context.args.shell = "bash";
    const std::string result = reset_background(context);
    CHECK(result.find("\001\033[49m\002") != std::string::npos);
}

#endif
