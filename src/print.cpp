#include <iostream>

#include "src/print.hpp"
#include "src/color.hpp"
#include "src/module.hpp"
#include "src/util.hpp"
#include "src/config.hpp"

// Padding before/after the module
std::string get_padding(const Config &config, const Module *current_module)
{
    // For these modules the padding in omitted by default
    if ((current_module->name == "separator" || current_module->name == "fixed") && current_module->padding == control_char)
    {
        return "";
    }
    return current_module->padding != control_char ? current_module->padding : config.global.padding;
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
    if (previous_module != NULL && previous_module->outer_suffix.empty())
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
    if (next_module != NULL && next_module->outer_prefix.empty())
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
    auto multiple = [](const int n, const std::string &c)
    {
        std::string result = "";
        for (int i = 0; i < n; i++)
        {
            result += c;
        }
        return result;
    };
    std::string result = "";
    result += reset_all(context);
    if (context.config.connector.dim)
    {
        result += set_text_mode(DIM, context);
    }
    result += set_foreground(context.config.connector.foreground, context);
    result += set_background(context.config.connector.background, context);
    result += multiple(length, context.config.connector.character);
    result += reset_all(context);
    return result;
}

// Basically the "PS1"
std::string prompt(const Context &context)
{
    std::string result = "";
    result += reset_all(context);
    if (context.args.exit_status != "0" && !context.config.prompt.error_foreground.empty())
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
    else
    {
        return false;
    }
}

// Checks if the current module is the last
bool end_reached(const Config &config, const std::size_t index)
{
    if (index == config.modules.size() - 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Evaluate the content of each module
void evaluate_content(Context &context)
{
    Config &config = context.config;
    for (Module &module : config.modules)
    {
        // Skip if prompt is displayed, and the module is not part of it
        if (context.args.prompt && !(module.align == "left" || module.align == "right"))
        {
            continue;
        }
        // Skip if right prompt is displayed, and the module is not part of it
        else if (context.args.right_prompt && module.align != "right_prompt")
        {
            continue;
        }
        else if (!module.name.empty())
        {
            module.content = call_module(module.name, context);
        }
        else if (!module.execute.empty())
        {
            module.content = execute_command(module.execute);
            strip(module.content);
        }
    }
}

// Remove modules that shouldn`t be displayed
void remove_surplus(Context &context)
{
    Config &config = context.config;
    Module *current_module;
    Module *previous_module;
    Module *next_module;
    for (auto iterator = config.modules.begin(); iterator != config.modules.end(); iterator++)
    {
        current_module = iterator.base();
        // Separators are processed later
        if (current_module->name == "separator")
        {
            continue;
        }
        // Remove module if it has nothing to show
        else if (current_module->content.empty() && current_module->name != "fixed")
        {
            config.modules.erase(iterator--);
        }
        // Remove module if prompt is displayed, and it isn`t part of it
        else if (context.args.prompt && !(current_module->align == "left" || current_module->align == "right"))
        {
            config.modules.erase(iterator--);
        }
        // Remove module if right prompt is displayed, and it isn`t part of it
        else if (context.args.right_prompt && current_module->align != "right_prompt")
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
        if (current_module->name == "separator" && next_module != NULL && next_module->name == "separator")
        {
            config.modules.erase(iterator--);
        }
        // Remove separator from the beginning/end of group
        else if (current_module->name == "separator" && (next_module == NULL || previous_module == NULL))
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
    Module *current_module;
    Module *previous_module;
    Module *next_module;
    std::string result;
    std::string temp;
    std::string left;
    std::string right;
    std::size_t length = 0;
    if (config.global.new_line)
    {
        result += "\n";
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
        if (current_module->align == "right")
        {
            right += temp;
        }
        else
        {
            left += temp;
        }
        if (level_changes(config, i) || end_reached(config, i))
        {
            result += left;
            left = "";
            if (!right.empty())
            {
                result += connector(context, get_columns() - length);
                result += right;
                right = "";
            }
            length = 0;
        }
        if (level_changes(config, i))
        {
            display_connector = contains_content_on_right(config.modules, i + 1);
            result += '\n';
        }
    }
    result += prompt(context);
    return result;
}

// Prepare right prompt string
std::string prepare_right_prompt(Context &context)
{
    Config &config = context.config;
    Module *current_module;
    Module *previous_module;
    Module *next_module;
    std::string result;
    std::string temp;
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

void print_prompt(Context &context)
{
    std::cout << prepare_prompt(context);
}

void print_right_prompt(Context &context)
{
    std::cout << prepare_right_prompt(context);
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("get_padding")
{
    Config config;
    config.global.padding = "a";
    Module current_module;
    current_module.name = "module";
    current_module.padding = control_char;
    SUBCASE("separator")
    {
        current_module.name = "separator";
        const std::string result = get_padding(config, &current_module);
        CHECK(result == "");
    }
    SUBCASE("fixed")
    {
        current_module.name = "fixed";
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
        const std::string result = pre(context, &current_module, NULL, display_connector);
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
        const std::string result = post(context, &current_module, NULL, display_connector);
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
        context.config.connector.character = "-";
        const std::string result = connector(context, length);
        CHECK(result.find("-----") != std::string::npos);
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
        context.args.exit_status = "1";
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
        context.config.prompt.string = "abc";
        context.config.prompt.foreground = "green";
        context.config.prompt.error_foreground = "red";
        const std::string test = "\001\033[0m\002\001\033[31m\002abc\001\033[0m\002";
        const std::string result = prompt(context);
        CHECK(result.find(test) != std::string::npos);
    }
}

TEST_CASE("level_changes")
{
    Config config;
    Module directory;
    config.modules.push_back(directory);
    Module execution_time;
    config.modules.push_back(execution_time);
    Module exit_status;
    config.modules.push_back(exit_status);
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
    Module execution_time;
    config.modules.push_back(execution_time);
    Module exit_status;
    config.modules.push_back(exit_status);
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
    context.args.prompt = true;
    context.args.right_prompt = false;
    context.args.start_time = "0";
    context.args.finish_time = "10";
    Module directory;
    directory.name = "directory";
    context.config.modules.push_back(directory);
    Module execution_time;
    execution_time.name = "execution_time";
    context.config.modules.push_back(execution_time);
    SUBCASE("1")
    {
        evaluate_content(context);
        CHECK(context.config.modules[0].content.find("/PWD") != std::string::npos);
        CHECK(context.config.modules[1].content.find("10s") != std::string::npos);
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
    Module execution_time;
    execution_time.name = "execution_time";
    execution_time.content = "10s";
    context.config.modules.push_back(execution_time);
    SUBCASE("don`t remove anything")
    {
        remove_surplus(context);
        CHECK(context.config.modules.size() == 2);
    }
    SUBCASE("remove empty")
    {
        context.config.modules[0].content = "";
        remove_surplus(context);
        CHECK(context.config.modules.size() == 1);
        CHECK(context.config.modules[0].name == "execution_time");
    }
    SUBCASE("remove prompt")
    {
        context.args.prompt = false;
        context.args.right_prompt = true;
        context.config.modules[1].align = "right_prompt";
        remove_surplus(context);
        CHECK(context.config.modules.size() == 1);
        CHECK(context.config.modules[0].name == "execution_time");
    }
    SUBCASE("remove right prompt")
    {
        context.config.modules[1].align = "right_prompt";
        remove_surplus(context);
        CHECK(context.config.modules.size() == 1);
        CHECK(context.config.modules[0].name == "directory");
    }
    SUBCASE("remove separator from beginning")
    {
        Module separator;
        separator.name = "separator";
        context.config.modules.insert(context.config.modules.begin(), separator);
        remove_surplus(context);
        CHECK(context.config.modules.size() == 2);
        CHECK(context.config.modules[0].name == "directory");
        CHECK(context.config.modules[1].name == "execution_time");
    }
    SUBCASE("remove separator from end")
    {
        Module separator;
        separator.name = "separator";
        context.config.modules.push_back(separator);
        remove_surplus(context);
        CHECK(context.config.modules.size() == 2);
        CHECK(context.config.modules[0].name == "directory");
        CHECK(context.config.modules[1].name == "execution_time");
    }
    SUBCASE("don`t remove separator")
    {
        Module separator;
        separator.name = "separator";
        context.config.modules.insert(context.config.modules.begin() + 1, separator);
        remove_surplus(context);
        CHECK(context.config.modules.size() == 3);
        CHECK(context.config.modules[0].name == "directory");
        CHECK(context.config.modules[1].name == "separator");
        CHECK(context.config.modules[2].name == "execution_time");
    }
    SUBCASE("remove duplicate separator")
    {
        Module separator;
        separator.name = "separator";
        context.config.modules.insert(context.config.modules.begin() + 1, separator);
        context.config.modules.insert(context.config.modules.begin() + 1, separator);
        remove_surplus(context);
        CHECK(context.config.modules.size() == 3);
        CHECK(context.config.modules[0].name == "directory");
        CHECK(context.config.modules[1].name == "separator");
        CHECK(context.config.modules[2].name == "execution_time");
    }
}

TEST_CASE("prepare_prompt")
{
    Context context;
    context.args.shell = "bash";
    context.args.start_time = "0";
    context.args.finish_time = "10";
    context.args.prompt = true;
    context.args.right_prompt = false;
    context.PWD = "/PWD";
    Module directory;
    directory.name = "directory";
    context.config.modules.push_back(directory);
    Module execution_time;
    execution_time.name = "execution_time";
    execution_time.align = "right";
    context.config.modules.push_back(execution_time);
    SUBCASE("module content")
    {
        std::string result = prepare_prompt(context);
        CHECK(result.find("/PWD") != std::string::npos);
        CHECK(result.find("10s") != std::string::npos);
    }
    SUBCASE("result")
    {
        context.config.global.padding = "abc";
        context.config.global.new_line = true;

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

        const std::string test = "\n\001\033[0m\002\001\033[2m\002\001\033[34m\002c\001\033[30m\002\001\033[44m\002-\001\033[1m\002\001\033[3m\002\001\033[4m\002a/PWD\001\033[0m\002\001\033[1m\002\001\033[2m\002\001\033[3m\002\001\033[4m\002\001\033[30m\002\001\033[44m\002b\001\033[22m\002\001\033[23m\002\001\033[24m\002\001\033[2m\002-\001\033[34m\002\001\033[49m\002d\001\033[0m\002\001\033[0m\002                                                                                                                                                 \001\033[0m\002\001\033[0m\002\001\033[31m\002c\001\033[37m\002\001\033[41m\002-a10s\001\033[0m\002\001\033[37m\002\001\033[41m\002b\001\033[22m\002\001\033[23m\002\001\033[24m\002-\001\033[31m\002\001\033[49m\002d\001\033[0m\002\001\033[0m\002 \001\033[0m\002";
        std::string result = prepare_prompt(context);
        CHECK(result.find(test) != std::string::npos);
    }
}

TEST_CASE("prepare_right_prompt")
{
    Context context;
    context.args.shell = "bash";
    context.args.start_time = "0";
    context.args.finish_time = "10";
    context.args.prompt = false;
    context.args.right_prompt = true;
    context.PWD = "/PWD";
    Module directory;
    directory.name = "directory";
    context.config.modules.push_back(directory);
    Module execution_time;
    execution_time.name = "execution_time";
    execution_time.align = "right_prompt";
    context.config.modules.push_back(execution_time);
    SUBCASE("module content")
    {
        std::string result = prepare_right_prompt(context);
        CHECK(result.find("/PWD") == std::string::npos);
        CHECK(result.find("10s") != std::string::npos);
    }
    SUBCASE("result")
    {
        context.config.global.padding = "abc";
        context.config.global.new_line = true;

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

#endif
