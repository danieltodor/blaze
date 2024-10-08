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
std::string create_sequence(const std::string &code, const Context &context, const bool graphics_mode=true)
{
    std::string result = "";
    result += sequence_begin.at(context.args.shell);
    result += csi;
    result += code;
    if (graphics_mode)
    {
        result += 'm';
    }
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
    std::string code = "";
    // Hexadecimal colors (#3dff27)
    if (color.at(0) == '#')
    {
        const std::string red_s = color.substr(1, 2), green_s = color.substr(3, 2), blue_s = color.substr(5, 2);
        const int red_i = stoi(red_s, nullptr, 16), green_i = stoi(green_s, nullptr, 16), blue_i = stoi(blue_s, nullptr, 16);
        const std::string rgb_prefix = offset == 0 ? foreground_rgb_prefix : background_rgb_prefix;
        code = rgb_prefix + join({std::to_string(red_i), std::to_string(green_i), std::to_string(blue_i)}, ";");
    }
    // Named colors (black, bright_blue, ...)
    else if (foreground_color_map.find(color) != foreground_color_map.end())
    {
        code = std::to_string(foreground_color_map.at(color) + offset);
    }
    // Numbered colors (0, ..., 255)
    else if (is_number(color))
    {
        const std::string id_prefix = offset == 0 ? foreground_id_prefix : background_id_prefix;
        code = id_prefix + color;
    }
    // RGB colors (24;65;255) (43,65,0)
    else
    {
        const std::string rgb_prefix = offset == 0 ? foreground_rgb_prefix : background_rgb_prefix;
        std::string rgb_color = color;
        if (color == "default")
        {
            rgb_color = context.args.background;
        }
        regex_replace(rgb_color, {" "}, "");
        regex_replace(rgb_color, {","}, ";");
        code = rgb_prefix + rgb_color;
    }
    return create_sequence(code, context);
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

std::string move_cursor_up(const int lines, const Context &context)
{
    return create_sequence(std::to_string(lines) + 'A', context, false);
}

std::string erase_until_end_of_screen(const Context &context)
{
    return create_sequence("0J", context, false);
}

std::vector<std::string> get_sequence_characters(const Context &context)
{
    return {sequence_begin.at(context.args.shell), sequence_end.at(context.args.shell)};
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
        CHECK(result == "\001\033[32m\002");
    }
    SUBCASE("fish")
    {
        context.args.shell = "fish";
        const std::string result = create_sequence("32", context);
        CHECK(result == "\001\033[32m\002");
    }
    SUBCASE("zsh")
    {
        context.args.shell = "zsh";
        const std::string result = create_sequence("32", context);
        CHECK(result == "%{\033[32m%}");
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
        CHECK(result == "\001\033[32m\002");
    }
    SUBCASE("background named")
    {
        const std::string result = to_color_code("green", 10, context);
        CHECK(result == "\001\033[42m\002");
    }
    SUBCASE("foreground color id")
    {
        const std::string result = to_color_code("128", 0, context);
        CHECK(result == "\001\033[38;5;128m\002");
    }
    SUBCASE("background color id")
    {
        const std::string result = to_color_code("128", 10, context);
        CHECK(result == "\001\033[48;5;128m\002");
    }
    SUBCASE("foreground rgb")
    {
        const std::string result = to_color_code("30;40;50", 0, context);
        CHECK(result == "\001\033[38;2;30;40;50m\002");
    }
    SUBCASE("background rgb")
    {
        const std::string result = to_color_code("30;40;50", 10, context);
        CHECK(result == "\001\033[48;2;30;40;50m\002");
    }
    SUBCASE("rgb with comma and space")
    {
        const std::string result = to_color_code("30, 40, 50", 0, context);
        CHECK(result == "\001\033[38;2;30;40;50m\002");
    }
    SUBCASE("foreground hex")
    {
        const std::string result = to_color_code("#3f7fb7", 0, context);
        CHECK(result == "\001\033[38;2;63;127;183m\002");
    }
    SUBCASE("background hex")
    {
        const std::string result = to_color_code("#3f7fb7", 10, context);
        CHECK(result == "\001\033[48;2;63;127;183m\002");
    }
}

TEST_CASE("set_text_mode")
{
    Context context;
    context.args.shell = "bash";
    const std::string result = set_text_mode(1, context);
    CHECK(result == "\001\033[1m\002");
}

TEST_CASE("set_foreground")
{
    Context context;
    context.args.shell = "bash";
    const std::string result = set_foreground("green", context);
    CHECK(result == "\001\033[32m\002");
}

TEST_CASE("set_background")
{
    Context context;
    context.args.shell = "bash";
    const std::string result = set_background("green", context);
    CHECK(result == "\001\033[42m\002");
}

TEST_CASE("reset_all")
{
    Context context;
    context.args.shell = "bash";
    const std::string result = reset_all(context);
    CHECK(result == "\001\033[0m\002");
}

TEST_CASE("reset_text_mode")
{
    Context context;
    context.args.shell = "bash";
    SUBCASE("bold")
    {
        const std::string result = reset_text_mode(1, context);
        CHECK(result == "\001\033[22m\002");
    }
    SUBCASE("dim")
    {
        const std::string result = reset_text_mode(2, context);
        CHECK(result == "\001\033[22m\002");
    }
    SUBCASE("italic")
    {
        const std::string result = reset_text_mode(3, context);
        CHECK(result == "\001\033[23m\002");
    }
}

TEST_CASE("reset_foreground")
{
    Context context;
    context.args.shell = "bash";
    const std::string result = reset_foreground(context);
    CHECK(result == "\001\033[39m\002");
}

TEST_CASE("reset_background")
{
    Context context;
    context.args.shell = "bash";
    const std::string result = reset_background(context);
    CHECK(result == "\001\033[49m\002");
}

TEST_CASE("move_cursor_up")
{
    Context context;
    SUBCASE("1 line up in bash")
    {
        context.args.shell = "bash";
        const std::string result = move_cursor_up(1, context);
        CHECK(result == "\001\033[1A\002");
    }
    SUBCASE("5 line up in zsh")
    {
        context.args.shell = "zsh";
        const std::string result = move_cursor_up(5, context);
        CHECK(result == "%{\033[5A%}");
    }
}

TEST_CASE("erase_until_end_of_screen")
{
    Context context;
    context.args.shell = "bash";
    const std::string result = erase_until_end_of_screen(context);
    CHECK(result == "\001\033[0J\002");
}

TEST_CASE("get_sequence_characters")
{
    Context context;
    SUBCASE("bash")
    {
        context.args.shell = "bash";
        const std::vector<std::string> result = get_sequence_characters(context);
        CHECK(result.size() == 2);
        CHECK(result.at(0) == "\001");
        CHECK(result.at(1) == "\002");
    }
    SUBCASE("zsh")
    {
        context.args.shell = "zsh";
        const std::vector<std::string> result = get_sequence_characters(context);
        CHECK(result.size() == 2);
        CHECK(result.at(0) == "%{");
        CHECK(result.at(1) == "%}");
    }
}

#endif
