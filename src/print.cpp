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

std::string pre(const Config &config, const Module *current_module, const Module *previous_module)
{
    std::string result = "";
    result += reset();
    if (current_module->dim)
    {
        result += text_mode(DIM);
    }
    if (previous_module != NULL && previous_module->display && previous_module->outer_suffix.empty())
    {
        result += background(previous_module->background);
    }
    else if (config.connector.display)
    {
        result += background(config.connector.background);
    }
    result += foreground(current_module->background);
    result += current_module->outer_prefix;
    result += foreground(current_module->foreground);
    result += background(current_module->background);
    result += get_padding(config, current_module);
    result += current_module->inner_prefix;
    if (current_module->bold)
    {
        result += text_mode(BOLD);
    }
    if (current_module->italic)
    {
        result += text_mode(ITALIC);
    }
    if (current_module->underline)
    {
        result += text_mode(UNDERLINE);
    }
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
    result += reset();
    if (config.connector.dim)
    {
        result += text_mode(DIM);
    }
    result += foreground(config.connector.foreground);
    result += background(config.connector.background);
    result += multiple(length, config.connector.character);
    result += reset();
    return result;
}

std::string post(const Config &config, const Module *current_module, const Module *next_module)
{
    std::string result = "";
    result += reset();
    if (current_module->dim)
    {
        result += text_mode(DIM);
    }
    result += foreground(current_module->foreground);
    result += background(current_module->background);
    result += current_module->inner_suffix;
    result += get_padding(config, current_module);
    result += reset();
    result += foreground(current_module->background);
    if (next_module != NULL && next_module->display && next_module->outer_prefix.empty())
    {
        result += background(next_module->background);
    }
    else if (config.connector.display)
    {
        result += background(config.connector.background);
    }
    result += current_module->outer_suffix;
    result += reset();
    return result;
}

std::string prompt(const Config &config)
{
    std::string result = "";
    result += reset();
    result += foreground(config.prompt.foreground);
    result += config.prompt.string;
    result += reset();
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

void preprocess_modules(Context &context)
{
    Config &config = context.config;
    Module *current_module;
    Module *previous_module;
    Module *next_module;
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
    for (std::size_t i = 0; i < config.modules.size(); i++)
    {
        current_module = &config.modules[i];
        previous_module = get_previous_module_in_group(config.modules, i);
        next_module = get_next_module_in_group(config.modules, i);
        if (!current_module->content.empty())
        {
            current_module->display = true;
        }
        else if (is_separator(*current_module) && !previous_module->content.empty() && !next_module->content.empty())
        {
            current_module->display = true;
        }
    }
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
    preprocess_modules(context);
    config.connector.display = content_on_right(config.modules, 1);
    for (std::size_t i = 0; i < config.modules.size(); i++)
    {
        current_module = &config.modules[i];
        previous_module = get_previous_module_in_group(config.modules, i);
        next_module = get_next_module_in_group(config.modules, i);
        temp = current_module->content;
        if (current_module->display)
        {
            length += get_length({
                temp,
                get_padding(config, current_module),
                get_padding(config, current_module),
                current_module->inner_prefix,
                current_module->inner_suffix,
                current_module->outer_prefix,
                current_module->outer_suffix
            });
            temp.insert(0, pre(config, current_module, previous_module));
            temp.insert(temp.length(), post(config, current_module, next_module));
        }
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
            config.connector.display = content_on_right(config.modules, i + 1);
            result += '\n';
        }
    }
    result += prompt(config);
    std::cout << result;
}
