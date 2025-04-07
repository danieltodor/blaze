#include <iostream>

#include "src/print.hpp"
#include "src/color.hpp"
#include "src/module.hpp"
#include "src/util.hpp"
#include "src/config.hpp"
#include "src/pool.hpp"

// Virtual modules
#define FIXED_MODULE "fixed"
#define SEPARATOR_MODULE "separator"

// Padding before/after the module
std::string get_padding(const Config &config, const Module *current_module)
{
    // For these modules the padding in omitted by default
    if ((current_module->name == FIXED_MODULE || current_module->name == SEPARATOR_MODULE) && current_module->padding == CONTROL_CHAR)
    {
        return "";
    }
    return current_module->padding != CONTROL_CHAR ? current_module->padding : config.global.padding;
}

// Escape sequences, prefixes before the module content
std::string pre(const Context &context, const Module *current_module, const Module *previous_module, const bool display_connector)
{
    std::string result = "";
    result += reset_all(context);
    if (current_module->dim)
    {
        result += set_text_mode(DIM, context);
    }
    result += set_foreground(current_module->background, context);
    if (previous_module != nullptr && previous_module->outer_suffix.empty())
    {
        result += set_background(previous_module->background, context);
    }
    else if (display_connector)
    {
        result += set_background(context.config.connector.background, context);
    }
    result += current_module->outer_prefix;
    result += set_foreground(current_module->foreground, context);
    result += set_background(current_module->background, context);
    result += get_padding(context.config, current_module);
    if (current_module->bold)
    {
        result += set_text_mode(BOLD, context);
    }
    if (current_module->italic)
    {
        result += set_text_mode(ITALIC, context);
    }
    if (current_module->underline)
    {
        result += set_text_mode(UNDERLINE, context);
    }
    result += current_module->inner_prefix;
    return result;
}

// Escape sequences, suffixes after the module content
std::string post(const Context &context, const Module *current_module, const Module *next_module, const bool display_connector)
{
    std::string result = "";
    result += reset_all(context);
    if (current_module->bold)
    {
        result += set_text_mode(BOLD, context);
    }
    if (current_module->dim)
    {
        result += set_text_mode(DIM, context);
    }
    if (current_module->italic)
    {
        result += set_text_mode(ITALIC, context);
    }
    if (current_module->underline)
    {
        result += set_text_mode(UNDERLINE, context);
    }
    result += set_foreground(current_module->foreground, context);
    result += set_background(current_module->background, context);
    result += current_module->inner_suffix;
    result += reset_text_mode(BOLD, context);
    result += reset_text_mode(ITALIC, context);
    result += reset_text_mode(UNDERLINE, context);
    if (current_module->dim)
    {
        result += set_text_mode(DIM, context);
    }
    result += get_padding(context.config, current_module);
    result += set_foreground(current_module->background, context);
    result += reset_background(context);
    if (next_module != nullptr && next_module->outer_prefix.empty())
    {
        result += set_background(next_module->background, context);
    }
    else if (display_connector)
    {
        result += set_background(context.config.connector.background, context);
    }
    result += current_module->outer_suffix;
    result += reset_all(context);
    return result;
}

// Connects the left/right side modules
std::string connector(const Context &context, const int length)
{
    std::string result = "";
    result += reset_all(context);
    if (context.config.connector.dim)
    {
        result += set_text_mode(DIM, context);
    }
    result += set_foreground(context.config.connector.foreground, context);
    result += set_background(context.config.connector.background, context);
    result += multiply_string(length, context.config.connector.character);
    result += reset_all(context);
    return result;
}

// Prompt string
std::string prompt(const Context &context)
{
    std::string result = "";
    result += reset_all(context);
    if (context.args.status != 0 && !context.config.prompt.error_foreground.empty())
    {
        result += set_foreground(context.config.prompt.error_foreground, context);
    }
    else
    {
        result += set_foreground(context.config.prompt.foreground, context);
    }
    result += context.config.prompt.string;
    result += reset_all(context);
    return result;
}

// Checks if the next module is on the next level
bool level_changes(const Config &config, const std::size_t index)
{
    if (index < config.modules.size() - 1 && config.modules[index + 1].level > config.modules[index].level)
    {
        return true;
    }
    return false;
}

// Checks if the current module is the last
bool end_reached(const Config &config, const std::size_t index)
{
    if (index == config.modules.size() - 1)
    {
        return true;
    }
    return false;
}

// Evaluate the content of each module
void evaluate_content(Context &context)
{
    Config &config = context.config;
    for (Module &module : config.modules)
    {
        // Skip if prompt is displayed, and the module is not part of it
        if (context.args.prompt && !(module.align == LEFT_ALIGNMENT || module.align == RIGHT_ALIGNMENT))
        {
            continue;
        }
        // Skip if right prompt is displayed, and the module is not part of it
        else if (context.args.right_prompt && module.align != RP_ALIGNMENT)
        {
            continue;
        }
        // Skip if module has predefined content
        else if (module.name == FIXED_MODULE || module.name == SEPARATOR_MODULE)
        {
            continue;
        }
        else if (!module.name.empty())
        {
            thread_pool.detach_task(
                [&module, &context]
                {
                    module.content = call_module(module.name, context);
                }
            );
        }
        else if (!module.execute.empty())
        {
            thread_pool.detach_task(
                [&module]
                {
                    module.content = execute_command(module.execute);
                    strip(module.content);
                }
            );
        }
    }
    thread_pool.wait();
}

// Remove modules that shouldn't be displayed
void remove_surplus(Context &context)
{
    Config &config = context.config;
    Module *current_module = nullptr;
    Module *previous_module = nullptr;
    Module *next_module = nullptr;
    for (auto iterator = config.modules.begin(); iterator != config.modules.end(); iterator++)
    {
        current_module = iterator.base();
        // Separators are processed later
        if (current_module->name == SEPARATOR_MODULE)
        {
            continue;
        }
        // Remove module if it has nothing to show
        else if (current_module->content.empty() && current_module->name != FIXED_MODULE)
        {
            config.modules.erase(iterator--);
        }
        // Remove module if prompt is displayed, and it isn't part of it
        else if (context.args.prompt && !(current_module->align == LEFT_ALIGNMENT || current_module->align == RIGHT_ALIGNMENT))
        {
            config.modules.erase(iterator--);
        }
        // Remove module if right prompt is displayed, and it isn't part of it
        else if (context.args.right_prompt && current_module->align != RP_ALIGNMENT)
        {
            config.modules.erase(iterator--);
        }
    }
    int i = 0;
    for (auto iterator = config.modules.begin(); iterator != config.modules.end(); iterator++)
    {
        current_module = iterator.base();
        previous_module = get_previous_module_in_group(config.modules, i);
        next_module = get_next_module_in_group(config.modules, i);
        // Remove separator if the next one is also a separator
        if (current_module->name == SEPARATOR_MODULE && next_module != nullptr && next_module->name == SEPARATOR_MODULE)
        {
            config.modules.erase(iterator--);
        }
        // Remove separator from the beginning/end of group
        else if (current_module->name == SEPARATOR_MODULE && (next_module == nullptr || previous_module == nullptr))
        {
            config.modules.erase(iterator--);
        }
        else
        {
            i++;
        }
    }
}

// Preprocess modules before printing them
void preprocess_modules(Context &context)
{
    evaluate_content(context);
    remove_surplus(context);
}

// Prepare prompt string
std::string prepare_prompt(Context &context)
{
    Config &config = context.config;
    Module *current_module = nullptr;
    Module *previous_module = nullptr;
    Module *next_module = nullptr;
    std::string result = "";
    std::string temp = "";
    std::string left = "";
    std::string right = "";
    std::size_t length = 0;
    winsize window = get_winsize();
    if (!config.prompt.separator.empty() && !context.args.first_print)
    {
        if (config.prompt.new_line)
        {
            result += '\n';
        }
        result += multiply_string(window.ws_col, config.prompt.separator);
        if (config.prompt.new_line)
        {
            result += "\n\n";
        }
    }
    else if (config.prompt.new_line && !context.args.first_print)
    {
        result += '\n';
    }
    else if (config.prompt.bottom && context.args.first_print)
    {
        result += std::string(window.ws_row - vertical_size(config), '\n');
    }
    preprocess_modules(context);
    bool display_connector = contains_content_on_right(config.modules, 1);
    for (std::size_t i = 0; i < config.modules.size(); i++)
    {
        current_module = &config.modules[i];
        previous_module = get_previous_module_in_group(config.modules, i);
        next_module = get_next_module_in_group(config.modules, i);
        temp = current_module->content;
        length += get_length({
            temp,
            get_padding(config, current_module),
            get_padding(config, current_module),
            current_module->inner_prefix,
            current_module->inner_suffix,
            current_module->outer_prefix,
            current_module->outer_suffix
        });
        temp.insert(0, pre(context, current_module, previous_module, display_connector));
        temp.insert(temp.length(), post(context, current_module, next_module, display_connector));
        if (current_module->align == RIGHT_ALIGNMENT)
        {
            right += temp;
        }
        else
        {
            left += temp;
        }
        const bool level_change = level_changes(config, i);
        if (level_change || end_reached(config, i))
        {
            result += left;
            left = "";
            if (!right.empty())
            {
                result += connector(context, window.ws_col - length);
                result += right;
                right = "";
            }
            length = 0;
            if (level_change)
            {
                display_connector = contains_content_on_right(config.modules, i + 1);
                result += '\n';
            }
        }
    }
    result += prompt(context);
    return result;
}

// Prepare right prompt string
std::string prepare_right_prompt(Context &context)
{
    Config &config = context.config;
    Module *current_module = nullptr;
    Module *previous_module = nullptr;
    Module *next_module = nullptr;
    std::string result = "";
    std::string temp = "";
    preprocess_modules(context);
    for (std::size_t i = 0; i < config.modules.size(); i++)
    {
        current_module = &config.modules[i];
        previous_module = get_previous_module_in_group(config.modules, i);
        next_module = get_next_module_in_group(config.modules, i);
        temp = current_module->content;
        temp.insert(0, pre(context, current_module, previous_module, false));
        temp.insert(temp.length(), post(context, current_module, next_module, false));
        result += temp;
    }
    return result;
}

// Prepare transient prompt string
std::string prepare_transient_prompt(Context &context)
{
    Config &config = context.config;
    if (!config.prompt.transient)
    {
        return "";
    }
    std::string result = "";
    result += move_cursor_up(vertical_size(config), context);
    result += erase_until_end_of_screen(context);
    // Carriage returns are needed for bash
    result += '\r';
    // Remove whitespace at beginning of prompt
    regex_replace(config.prompt.string, {"^\\s*"}, "");
    result += prompt(context);
    result += context.args.previous_command;
    result += "\n\r";
    // Sequence characters cannot be used when cursor position is changed
    regex_replace(result, get_sequence_characters(context), "");
    return result;
}

void print_prompt(Context &context)
{
    std::cout << prepare_prompt(context);
}

void print_right_prompt(Context &context)
{
    std::cout << prepare_right_prompt(context);
}

void print_transient_prompt(Context &context)
{
    std::cout << prepare_transient_prompt(context);
}

// ----------------------------------- TESTS -----------------------------------
#ifdef TEST
#include "src/test.hpp"

TEST_CASE("get_padding")
{
    Config config;
    config.global.padding = "a";
    Module current_module;
    current_module.name = "module";
    current_module.padding = CONTROL_CHAR;
    SUBCASE("separator")
    {
        current_module.name = SEPARATOR_MODULE;
        const std::string result = get_padding(config, &current_module);
        CHECK(result == "");
    }
    SUBCASE("fixed")
    {
        current_module.name = FIXED_MODULE;
        const std::string result = get_padding(config, &current_module);
        CHECK(result == "");
    }
    SUBCASE("module")
    {
        current_module.padding = "b";
        const std::string result = get_padding(config, &current_module);
        CHECK(result == "b");
    }
    SUBCASE("global")
    {
        const std::string result = get_padding(config, &current_module);
        CHECK(result == "a");
    }
}

TEST_CASE("pre")
{
    Context context;
    context.args.shell = "bash";
    Module current_module;
    Module previous_module;
    bool display_connector = false;
    SUBCASE("reset")
    {
        const std::string result = pre(context, &current_module, &previous_module, display_connector);
        CHECK(result.find("\033[0m") != std::string::npos);
    }
    SUBCASE("bold")
    {
        current_module.bold = true;
        const std::string result = pre(context, &current_module, &previous_module, display_connector);
        CHECK(result.find("\033[1m") != std::string::npos);
    }
    SUBCASE("dim")
    {
        current_module.dim = true;
        const std::string result = pre(context, &current_module, &previous_module, display_connector);
        CHECK(result.find("\033[2m") != std::string::npos);
    }
    SUBCASE("italic")
    {
        current_module.italic = true;
        const std::string result = pre(context, &current_module, &previous_module, display_connector);
        CHECK(result.find("\033[3m") != std::string::npos);
    }
    SUBCASE("underline")
    {
        current_module.underline = true;
        const std::string result = pre(context, &current_module, &previous_module, display_connector);
        CHECK(result.find("\033[4m") != std::string::npos);
    }
    SUBCASE("foreground")
    {
        current_module.foreground = "green";
        const std::string result = pre(context, &current_module, &previous_module, display_connector);
        CHECK(result.find("\033[32m") != std::string::npos);
    }
    SUBCASE("background")
    {
        current_module.background = "green";
        const std::string result = pre(context, &current_module, &previous_module, display_connector);
        CHECK(result.find("\033[42m") != std::string::npos);
    }
    SUBCASE("previous module background")
    {
        previous_module.background = "red";
        const std::string result = pre(context, &current_module, &previous_module, display_connector);
        CHECK(result.find("\033[41m") != std::string::npos);
    }
    SUBCASE("connector background")
    {
        context.config.connector.background = "yellow";
        display_connector = true;
        const std::string result = pre(context, &current_module, nullptr, display_connector);
        CHECK(result.find("\033[43m") != std::string::npos);
    }
    SUBCASE("inner prefix")
    {
        current_module.inner_prefix = "abc";
        const std::string result = pre(context, &current_module, &previous_module, display_connector);
        CHECK(result.find("abc") != std::string::npos);
    }
    SUBCASE("outer prefix")
    {
        current_module.outer_prefix = "abc";
        const std::string result = pre(context, &current_module, &previous_module, display_connector);
        CHECK(result.find("abc") != std::string::npos);
    }
    SUBCASE("background as foreground")
    {
        current_module.background = "green";
        const std::string result = pre(context, &current_module, &previous_module, display_connector);
        CHECK(result.find("\033[32m") != std::string::npos);
    }
    SUBCASE("padding")
    {
        context.config.global.padding = "abc";
        const std::string result = pre(context, &current_module, &previous_module, display_connector);
        CHECK(result.find("abc") != std::string::npos);
    }
    SUBCASE("result")
    {
        context.config.global.padding = "abc";
        current_module.bold = true;
        current_module.dim = true;
        current_module.italic = true;
        current_module.underline = true;
        current_module.foreground = "yellow";
        current_module.background = "blue";
        current_module.inner_prefix = "def";
        current_module.outer_prefix = "ghi";
        previous_module.background = "red";
        const std::string test = "\001\033[0m\002\001\033[2m\002\001\033[34m\002\001\033[41m\002ghi\001\033[33m\002\001\033[44m\002abc\001\033[1m\002\001\033[3m\002\001\033[4m\002def";
        const std::string result = pre(context, &current_module, &previous_module, display_connector);
        CHECK(result.find(test) != std::string::npos);
    }
}

TEST_CASE("post")
{
    Context context;
    context.args.shell = "bash";
    Module current_module;
    Module next_module;
    bool display_connector = false;
    SUBCASE("reset")
    {
        const std::string result = post(context, &current_module, &next_module, display_connector);
        CHECK(result.find("\033[0m") != std::string::npos);
    }
    SUBCASE("bold")
    {
        current_module.bold = true;
        const std::string result = post(context, &current_module, &next_module, display_connector);
        CHECK(result.find("\033[1m") != std::string::npos);
    }
    SUBCASE("dim")
    {
        current_module.dim = true;
        const std::string result = post(context, &current_module, &next_module, display_connector);
        CHECK(result.find("\033[2m") != std::string::npos);
    }
    SUBCASE("italic")
    {
        current_module.italic = true;
        const std::string result = post(context, &current_module, &next_module, display_connector);
        CHECK(result.find("\033[3m") != std::string::npos);
    }
    SUBCASE("underline")
    {
        current_module.underline = true;
        const std::string result = post(context, &current_module, &next_module, display_connector);
        CHECK(result.find("\033[4m") != std::string::npos);
    }
    SUBCASE("foreground")
    {
        current_module.foreground = "green";
        const std::string result = post(context, &current_module, &next_module, display_connector);
        CHECK(result.find("\033[32m") != std::string::npos);
    }
    SUBCASE("background")
    {
        current_module.background = "green";
        const std::string result = post(context, &current_module, &next_module, display_connector);
        CHECK(result.find("\033[42m") != std::string::npos);
    }
    SUBCASE("previous module background")
    {
        next_module.background = "red";
        const std::string result = post(context, &current_module, &next_module, display_connector);
        CHECK(result.find("\033[41m") != std::string::npos);
    }
    SUBCASE("connector background")
    {
        context.config.connector.background = "yellow";
        display_connector = true;
        const std::string result = post(context, &current_module, nullptr, display_connector);
        CHECK(result.find("\033[43m") != std::string::npos);
    }
    SUBCASE("inner suffix")
    {
        current_module.inner_suffix = "abc";
        const std::string result = post(context, &current_module, &next_module, display_connector);
        CHECK(result.find("abc") != std::string::npos);
    }
    SUBCASE("outer suffix")
    {
        current_module.outer_suffix = "abc";
        const std::string result = post(context, &current_module, &next_module, display_connector);
        CHECK(result.find("abc") != std::string::npos);
    }
    SUBCASE("background as foreground")
    {
        current_module.background = "green";
        const std::string result = post(context, &current_module, &next_module, display_connector);
        CHECK(result.find("\033[32m") != std::string::npos);
    }
    SUBCASE("padding")
    {
        context.config.global.padding = "abc";
        const std::string result = post(context, &current_module, &next_module, display_connector);
        CHECK(result.find("abc") != std::string::npos);
    }
    SUBCASE("result")
    {
        context.config.global.padding = "abc";
        current_module.bold = true;
        current_module.dim = true;
        current_module.italic = true;
        current_module.underline = true;
        current_module.foreground = "yellow";
        current_module.background = "blue";
        current_module.inner_suffix = "def";
        current_module.outer_suffix = "ghi";
        next_module.background = "red";
        const std::string test = "\001\033[0m\002\001\033[1m\002\001\033[2m\002\001\033[3m\002\001\033[4m\002\001\033[33m\002\001\033[44m\002def\001\033[22m\002\001\033[23m\002\001\033[24m\002\001\033[2m\002abc\001\033[34m\002\001\033[49m\002\001\033[41m\002ghi\001\033[0m\002";
        const std::string result = post(context, &current_module, &next_module, display_connector);
        CHECK(result.find(test) != std::string::npos);
    }
}

TEST_CASE("connector")
{
    Context context;
    context.args.shell = "bash";
    int length = 5;
    SUBCASE("reset")
    {
        const std::string result = connector(context, length);
        CHECK(result.find("\033[0m") != std::string::npos);
    }
    SUBCASE("dim")
    {
        context.config.connector.dim = true;
        const std::string result = connector(context, length);
        CHECK(result.find("\033[2m") != std::string::npos);
    }
    SUBCASE("foreground")
    {
        context.config.connector.foreground = "green";
        const std::string result = connector(context, length);
        CHECK(result.find("\033[32m") != std::string::npos);
    }
    SUBCASE("background")
    {
        context.config.connector.background = "green";
        const std::string result = connector(context, length);
        CHECK(result.find("\033[42m") != std::string::npos);
    }
    SUBCASE("connection")
    {
        context.config.connector.character = "─";
        const std::string result = connector(context, length);
        CHECK(result.find("─────") != std::string::npos);
    }
}

TEST_CASE("prompt")
{
    Context context;
    context.args.shell = "bash";
    SUBCASE("reset")
    {
        const std::string result = prompt(context);
        CHECK(result.find("\033[0m") != std::string::npos);
    }
    SUBCASE("foreground")
    {
        context.config.prompt.foreground = "green";
        const std::string result = prompt(context);
        CHECK(result.find("\033[32m") != std::string::npos);
    }
    SUBCASE("error foreground")
    {
        context.args.status = 1;
        context.config.prompt.error_foreground = "red";
        const std::string result = prompt(context);
        CHECK(result.find("\033[31m") != std::string::npos);
    }
    SUBCASE("string")
    {
        context.config.prompt.string = "abc";
        const std::string result = prompt(context);
        CHECK(result.find("abc") != std::string::npos);
    }
    SUBCASE("result")
    {
        context.args.status = 0;
        context.config.prompt.string = "abc";
        context.config.prompt.foreground = "green";
        context.config.prompt.error_foreground = "red";
        const std::string test = "\001\033[0m\002\001\033[32m\002abc\001\033[0m\002";
        const std::string result = prompt(context);
        CHECK(result.find(test) != std::string::npos);
    }
}

TEST_CASE("level_changes")
{
    Config config;
    Module directory;
    config.modules.push_back(directory);
    Module duration;
    config.modules.push_back(duration);
    Module status;
    config.modules.push_back(status);
    SUBCASE("all modules are on the same level")
    {
        CHECK(level_changes(config, 0) == false);
    }
    SUBCASE("next module is on the next level")
    {
        config.modules[1].level = 2;
        CHECK(level_changes(config, 0) == true);
    }
    SUBCASE("index is too big")
    {
        CHECK(level_changes(config, 3) == false);
    }
}

TEST_CASE("end_reached")
{
    Config config;
    Module directory;
    config.modules.push_back(directory);
    Module duration;
    config.modules.push_back(duration);
    Module status;
    config.modules.push_back(status);
    SUBCASE("1")
    {
        CHECK(end_reached(config, 0) == false);
    }
    SUBCASE("2")
    {
        CHECK(end_reached(config, 1) == false);
    }
    SUBCASE("3")
    {
        CHECK(end_reached(config, 2) == true);
    }
}

TEST_CASE("evaluate_content")
{
    Context context;
    context.PWD = "/PWD";
    context.HOME = "/a";
    context.args.prompt = true;
    context.args.right_prompt = false;
    context.args.start_time = 1;
    context.args.finish_time = 11;
    context.git_repository_detected = false;
    context.git_repository_detached = false;
    Module directory;
    directory.name = "directory";
    context.config.modules.push_back(directory);
    Module duration;
    duration.name = "duration";
    context.config.modules.push_back(duration);
    Module fixed;
    fixed.name = FIXED_MODULE;
    fixed.content = "a";
    context.config.modules.push_back(fixed);
    Module separator;
    separator.name = SEPARATOR_MODULE;
    separator.content = "b";
    context.config.modules.push_back(separator);
    SUBCASE("1")
    {
        evaluate_content(context);
        CHECK(context.config.modules[0].content == "/PWD");
        CHECK(context.config.modules[1].content == "10s");
        CHECK(context.config.modules[2].content == "a");
        CHECK(context.config.modules[3].content == "b");
    }
}

TEST_CASE("remove_surplus")
{
    Context context;
    context.args.prompt = true;
    context.args.right_prompt = false;
    Module directory;
    directory.name = "directory";
    directory.content = "/home";
    context.config.modules.push_back(directory);
    Module duration;
    duration.name = "duration";
    duration.content = "10s";
    context.config.modules.push_back(duration);
    SUBCASE("don't remove anything")
    {
        remove_surplus(context);
        CHECK(context.config.modules.size() == 2);
    }
    SUBCASE("remove empty")
    {
        context.config.modules[0].content = "";
        remove_surplus(context);
        CHECK(context.config.modules.size() == 1);
        CHECK(context.config.modules[0].name == "duration");
    }
    SUBCASE("remove prompt")
    {
        context.args.prompt = false;
        context.args.right_prompt = true;
        context.config.modules[1].align = RP_ALIGNMENT;
        remove_surplus(context);
        CHECK(context.config.modules.size() == 1);
        CHECK(context.config.modules[0].name == "duration");
    }
    SUBCASE("remove right prompt")
    {
        context.config.modules[1].align = RP_ALIGNMENT;
        remove_surplus(context);
        CHECK(context.config.modules.size() == 1);
        CHECK(context.config.modules[0].name == "directory");
    }
    SUBCASE("remove separator from beginning")
    {
        Module separator;
        separator.name = SEPARATOR_MODULE;
        context.config.modules.insert(context.config.modules.begin(), separator);
        remove_surplus(context);
        CHECK(context.config.modules.size() == 2);
        CHECK(context.config.modules[0].name == "directory");
        CHECK(context.config.modules[1].name == "duration");
    }
    SUBCASE("remove separator from end")
    {
        Module separator;
        separator.name = SEPARATOR_MODULE;
        context.config.modules.push_back(separator);
        remove_surplus(context);
        CHECK(context.config.modules.size() == 2);
        CHECK(context.config.modules[0].name == "directory");
        CHECK(context.config.modules[1].name == "duration");
    }
    SUBCASE("don't remove separator")
    {
        Module separator;
        separator.name = SEPARATOR_MODULE;
        context.config.modules.insert(context.config.modules.begin() + 1, separator);
        remove_surplus(context);
        CHECK(context.config.modules.size() == 3);
        CHECK(context.config.modules[0].name == "directory");
        CHECK(context.config.modules[1].name == "separator");
        CHECK(context.config.modules[2].name == "duration");
    }
    SUBCASE("remove duplicate separator")
    {
        Module separator;
        separator.name = SEPARATOR_MODULE;
        context.config.modules.insert(context.config.modules.begin() + 1, separator);
        context.config.modules.insert(context.config.modules.begin() + 1, separator);
        remove_surplus(context);
        CHECK(context.config.modules.size() == 3);
        CHECK(context.config.modules[0].name == "directory");
        CHECK(context.config.modules[1].name == "separator");
        CHECK(context.config.modules[2].name == "duration");
    }
}

TEST_CASE("prepare_prompt")
{
    Context context;
    context.args.shell = "bash";
    context.args.start_time = 1;
    context.args.finish_time = 11;
    context.args.prompt = true;
    context.args.right_prompt = false;
    context.args.first_print = false;
    context.config.directory.from_repository = false;
    context.HOME = "/X";
    context.PWD = "/PWD";
    Module directory;
    directory.name = "directory";
    context.config.modules.push_back(directory);
    Module duration;
    duration.name = "duration";
    duration.align = RIGHT_ALIGNMENT;
    context.config.modules.push_back(duration);
    SUBCASE("module content")
    {
        std::string result = prepare_prompt(context);
        CHECK(result.find("/PWD") != std::string::npos);
        CHECK(result.find("10s") != std::string::npos);
    }
    SUBCASE("separator first print")
    {
        context.args.first_print = true;
        context.config.prompt.separator = "─";
        std::string result = prepare_prompt(context);
        CHECK(result.find("─") == std::string::npos);
    }
    SUBCASE("separator without new line")
    {
        context.args.first_print = false;
        context.config.prompt.separator = "-";
        std::string result = prepare_prompt(context);
        CHECK(result.at(0) == '-');
    }
    SUBCASE("separator with new line")
    {
        context.args.first_print = false;
        context.config.prompt.new_line = true;
        context.config.prompt.separator = "─";
        std::string result = prepare_prompt(context);
        CHECK(result.find("\n───────") != std::string::npos);
        CHECK(result.find("───────\n\n") != std::string::npos);
    }
    SUBCASE("new line first print")
    {
        context.config.prompt.new_line = true;
        context.args.first_print = true;
        std::string result = prepare_prompt(context);
        CHECK(result.at(0) != '\n');
    }
    SUBCASE("new line after first print")
    {
        context.config.prompt.new_line = true;
        context.args.first_print = false;
        std::string result = prepare_prompt(context);
        CHECK(result.at(0) == '\n');
    }
    SUBCASE("bottom first print")
    {
        context.config.prompt.bottom = true;
        context.args.first_print = true;
        std::string result = prepare_prompt(context);
        CHECK(result.at(0) == '\n');
        CHECK(result.at(5) == '\n');
    }
    SUBCASE("bottom after first print")
    {
        context.config.prompt.bottom = true;
        context.args.first_print = false;
        std::string result = prepare_prompt(context);
        CHECK(result.at(0) != '\n');
    }
    SUBCASE("result")
    {
        context.config.global.padding = "abc";
        context.config.prompt.new_line = true;

        context.config.modules[0].background = "blue";
        context.config.modules[0].foreground = "black";
        context.config.modules[0].bold = true;
        context.config.modules[0].dim = true;
        context.config.modules[0].italic = true;
        context.config.modules[0].underline = true;
        context.config.modules[0].inner_prefix = "a";
        context.config.modules[0].inner_suffix = "b";
        context.config.modules[0].outer_prefix = "c";
        context.config.modules[0].outer_suffix = "d";
        context.config.modules[0].padding = "-";

        context.config.modules[1].background = "red";
        context.config.modules[1].foreground = "white";
        context.config.modules[1].bold = false;
        context.config.modules[1].dim = false;
        context.config.modules[1].italic = false;
        context.config.modules[1].underline = false;
        context.config.modules[1].inner_prefix = "a";
        context.config.modules[1].inner_suffix = "b";
        context.config.modules[1].outer_prefix = "c";
        context.config.modules[1].outer_suffix = "d";
        context.config.modules[1].padding = "-";

        // Left and right side needs to be tested separately, as the number of spaces depends on window size when running the test
        const std::string test_left = "\n\001\033[0m\002\001\033[2m\002\001\033[34m\002c\001\033[30m\002\001\033[44m\002-\001\033[1m\002\001\033[3m\002\001\033[4m\002a/PWD\001\033[0m\002\001\033[1m\002\001\033[2m\002\001\033[3m\002\001\033[4m\002\001\033[30m\002\001\033[44m\002b\001\033[22m\002\001\033[23m\002\001\033[24m\002\001\033[2m\002-\001\033[34m\002\001\033[49m\002d\001\033[0m\002\001\033[0m\002";
        const std::string test_middle = "                                               ";
        const std::string test_right = "\001\033[0m\002\001\033[0m\002\001\033[31m\002c\001\033[37m\002\001\033[41m\002-a10s\001\033[0m\002\001\033[37m\002\001\033[41m\002b\001\033[22m\002\001\033[23m\002\001\033[24m\002-\001\033[31m\002\001\033[49m\002d\001\033[0m\002\001\033[0m\002 ❯ \001\033[0m\002";
        std::string result = prepare_prompt(context);
        CHECK(result.find(test_left) != std::string::npos);
        CHECK(result.find(test_middle) != std::string::npos);
        CHECK(result.find(test_right) != std::string::npos);
    }
}

TEST_CASE("prepare_right_prompt")
{
    Context context;
    context.args.shell = "bash";
    context.args.start_time = 1;
    context.args.finish_time = 11;
    context.args.prompt = false;
    context.args.right_prompt = true;
    context.PWD = "/PWD";
    Module directory;
    directory.name = "directory";
    context.config.modules.push_back(directory);
    Module duration;
    duration.name = "duration";
    duration.align = RP_ALIGNMENT;
    context.config.modules.push_back(duration);
    SUBCASE("module content")
    {
        std::string result = prepare_right_prompt(context);
        CHECK(result.find("/PWD") == std::string::npos);
        CHECK(result.find("10s") != std::string::npos);
    }
    SUBCASE("result")
    {
        context.config.global.padding = "abc";
        context.config.prompt.new_line = true;

        context.config.modules[0].background = "blue";
        context.config.modules[0].foreground = "black";
        context.config.modules[0].bold = true;
        context.config.modules[0].dim = true;
        context.config.modules[0].italic = true;
        context.config.modules[0].underline = true;
        context.config.modules[0].inner_prefix = "a";
        context.config.modules[0].inner_suffix = "b";
        context.config.modules[0].outer_prefix = "c";
        context.config.modules[0].outer_suffix = "d";
        context.config.modules[0].padding = "-";

        context.config.modules[1].background = "red";
        context.config.modules[1].foreground = "white";
        context.config.modules[1].bold = false;
        context.config.modules[1].dim = false;
        context.config.modules[1].italic = false;
        context.config.modules[1].underline = false;
        context.config.modules[1].inner_prefix = "a";
        context.config.modules[1].inner_suffix = "b";
        context.config.modules[1].outer_prefix = "c";
        context.config.modules[1].outer_suffix = "d";
        context.config.modules[1].padding = "-";

        const std::string test = "\001\033[0m\002\001\033[31m\002c\001\033[37m\002\001\033[41m\002-a10s\001\033[0m\002\001\033[37m\002\001\033[41m\002b\001\033[22m\002\001\033[23m\002\001\033[24m\002-\001\033[31m\002\001\033[49m\002d\001\033[0m\002";
        std::string result = prepare_right_prompt(context);
        CHECK(result.find(test) != std::string::npos);
    }
}

TEST_CASE("prepare_transient_prompt")
{
    Context context;
    context.args.shell = "bash";
    context.args.transient_prompt = true;
    context.args.previous_command = "asd";
    context.args.status = 1;
    context.config.prompt.string = "\n > ";
    Module directory;
    directory.name = "directory";
    context.config.modules.push_back(directory);
    SUBCASE("transient prompt disabled")
    {
        std::string result = prepare_transient_prompt(context);
        CHECK(result == "");
    }
    SUBCASE("transient prompt enabled")
    {
        context.config.prompt.transient = true;
        std::string result = prepare_transient_prompt(context);
        CHECK(result == "\033[2A\033[0J\r\033[0m> \033[0masd\n\r");
    }
}

#endif
