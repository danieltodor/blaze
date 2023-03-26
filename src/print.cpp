#include <iostream>

#include "src/print.hpp"
#include "src/color.hpp"
#include "src/module.hpp"
#include "src/util.hpp"
#include "src/config.hpp"

// Padding before/after the module
std::string get_padding(const Config &config, const Module *current_module)
{
    // For separators the padding in omitted by default
    if (is_separator(*current_module) && current_module->padding == control_char)
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
    if (context.exit_status != "0" && !context.config.prompt.error_foreground.empty())
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
        if (context.prompt && !(module.align == "left" || module.align == "right"))
        {
            continue;
        }
        // Skip if right prompt is displayed, and the module is not part of it
        else if (context.right_prompt && module.align != "right_prompt")
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
void remove_surplus(Config &config)
{
    Module *current_module;
    Module *previous_module;
    Module *next_module;
    for (auto iterator = config.modules.begin(); iterator != config.modules.end(); iterator++)
    {
        current_module = iterator.base();
        // Remove module if it`s content is empty
        if (current_module->content.empty() && !is_separator(*current_module))
        {
            config.modules.erase(iterator--);
        }
    }
    // TODO: Maybe this can be done in the first pass, when a module is removed
    int i = 0;
    for (auto iterator = config.modules.begin(); iterator != config.modules.end(); iterator++)
    {
        current_module = iterator.base();
        previous_module = get_previous_module_in_group(config.modules, i);
        next_module = get_next_module_in_group(config.modules, i);
        // Remove separator if the next one is also a separator
        if (is_separator(*current_module) && next_module != NULL && is_separator(*next_module))
        {
            config.modules.erase(iterator--);
        }
        // Remove separator from the beginning/end of group
        else if (is_separator(*current_module) && (next_module == NULL || previous_module == NULL))
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
    remove_surplus(context.config);
}

void print_prompt(Context &context)
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
    std::cout << result;
}

void print_right_prompt(Context &context)
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
    std::cout << result;
}
