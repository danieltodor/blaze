#include <iostream>

#include "print.hpp"
#include "color.hpp"
#include "module.hpp"
#include "util.hpp"
#include "config.hpp"

std::string get_padding(const Config &config, const Module *current_module)
{
    if (is_separator(*current_module) && current_module->padding == control_char)
    {
        return "";
    }
    return current_module->padding != control_char ? current_module->padding : config.global.padding;
}

std::string pre(const Config &config, const Module *current_module, const Module *previous_module, const bool display_connector)
{
    std::string result = "";
    result += reset_all();
    if (current_module->dim)
    {
        result += set_text_mode(DIM);
    }
    result += set_foreground(current_module->background);
    if (previous_module != NULL && previous_module->outer_suffix.empty())
    {
        result += set_background(previous_module->background);
    }
    else if (display_connector)
    {
        result += set_background(config.connector.background);
    }
    result += current_module->outer_prefix;
    result += set_foreground(current_module->foreground);
    result += set_background(current_module->background);
    result += get_padding(config, current_module);
    if (current_module->bold)
    {
        result += set_text_mode(BOLD);
    }
    if (current_module->italic)
    {
        result += set_text_mode(ITALIC);
    }
    if (current_module->underline)
    {
        result += set_text_mode(UNDERLINE);
    }
    result += current_module->inner_prefix;
    return result;
}

std::string middle(const Config &config, const int length)
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
    result += reset_all();
    if (config.connector.dim)
    {
        result += set_text_mode(DIM);
    }
    result += set_foreground(config.connector.foreground);
    result += set_background(config.connector.background);
    result += multiple(length, config.connector.character);
    result += reset_all();
    return result;
}

std::string post(const Config &config, const Module *current_module, const Module *next_module, const bool display_connector)
{
    std::string result = "";
    result += reset_all();
    if (current_module->bold)
    {
        result += set_text_mode(BOLD);
    }
    if (current_module->dim)
    {
        result += set_text_mode(DIM);
    }
    if (current_module->italic)
    {
        result += set_text_mode(ITALIC);
    }
    if (current_module->underline)
    {
        result += set_text_mode(UNDERLINE);
    }
    result += set_foreground(current_module->foreground);
    result += set_background(current_module->background);
    result += current_module->inner_suffix;
    result += reset_text_mode(BOLD);
    result += reset_text_mode(ITALIC);
    result += reset_text_mode(UNDERLINE);
    if (current_module->dim)
    {
        result += set_text_mode(DIM);
    }
    result += get_padding(config, current_module);
    result += set_foreground(current_module->background);
    result += reset_background();
    if (next_module != NULL && next_module->outer_prefix.empty())
    {
        result += set_background(next_module->background);
    }
    else if (display_connector)
    {
        result += set_background(config.connector.background);
    }
    result += current_module->outer_suffix;
    result += reset_all();
    return result;
}

std::string prompt(const Config &config)
{
    std::string result = "";
    result += reset_all();
    result += set_foreground(config.prompt.foreground);
    result += config.prompt.string;
    result += reset_all();
    return result;
}

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

void evaluate_content(Context &context)
{
    Config &config = context.config;
    for (Module &module : config.modules)
    {
        if (!module.name.empty())
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

void remove_surplus(Context &context)
{
    Config &config = context.config;
    Module *current_module;
    Module *previous_module;
    Module *next_module;
    for (auto iterator = config.modules.begin(); iterator != config.modules.end(); iterator++)
    {
        current_module = iterator.base();
        if (current_module->content.empty() && !is_separator(*current_module))
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
        if (is_separator(*current_module) && next_module != NULL && is_separator(*next_module))
        {
            config.modules.erase(iterator--);
        }
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

void process_modules(Context &context)
{
    evaluate_content(context);
    remove_surplus(context);
}

void print_all(Context &context)
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
    process_modules(context);
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
        temp.insert(0, pre(config, current_module, previous_module, display_connector));
        temp.insert(temp.length(), post(config, current_module, next_module, display_connector));
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
                result += middle(config, get_column() - length);
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
    result += prompt(config);
    std::cout << result;
}
