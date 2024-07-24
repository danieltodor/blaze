#include <algorithm>
#include <unordered_map>

#include "src/config.hpp"
#include "src/util.hpp"

// Read the user`s config file
toml::value read_data()
{
    const std::string paths[] = {
        get_env("BLAZE_CONFIG"),
        get_env("HOME") + "/.config/blaze/config.toml"
    };
    toml::value data;
    for (std::size_t i = 0; i < std::size(paths); i++)
    {
        try
        {
            data = toml::parse(paths[i]);
            break;
        }
        catch (const std::runtime_error &err)
        {
        }
    }
    return data;
}

// Set config values
template<typename T, typename... Keys>
void set_value(const toml::value &data, T &target, Keys&&... keys)
{
    try
    {
        target = toml::find<T>(data, keys...);
    }
    catch (const std::out_of_range &err)
    {
    }
}

// Load the values from the config file
void load_values(toml::value &data, Config &config)
{
    set_value(data, config.global.new_line, "global", "new_line");
    set_value(data, config.global.padding, "global", "padding");

    set_value(data, config.prompt.string, "prompt", "string");
    set_value(data, config.prompt.foreground, "prompt", "foreground");
    set_value(data, config.prompt.error_foreground, "prompt", "error_foreground");

    set_value(data, config.connector.character, "connector", "character");
    set_value(data, config.connector.foreground, "connector", "foreground");
    set_value(data, config.connector.background, "connector", "background");
    set_value(data, config.connector.dim, "connector", "dim");

    try
    {
        const toml::array module_array = toml::find(data, "module").as_array();
        config.modules.clear();
        try
        {
            for (int i = 0;; i++)
            {
                const toml::value &module_data = module_array.at(i);
                Module current;
                set_value(module_data, current.name, "name");
                set_value(module_data, current.execute, "execute");
                set_value(module_data, current.level, "level");
                set_value(module_data, current.position, "position");
                set_value(module_data, current.align, "align");
                set_value(module_data, current.padding, "padding");
                set_value(module_data, current.inner_prefix, "inner_prefix");
                set_value(module_data, current.inner_suffix, "inner_suffix");
                set_value(module_data, current.outer_prefix, "outer_prefix");
                set_value(module_data, current.outer_suffix, "outer_suffix");
                set_value(module_data, current.foreground, "foreground");
                set_value(module_data, current.background, "background");
                set_value(module_data, current.bold, "bold");
                set_value(module_data, current.dim, "dim");
                set_value(module_data, current.italic, "italic");
                set_value(module_data, current.underline, "underline");
                config.modules.push_back(current);
            }
        }
        catch (const std::out_of_range &err)
        {
        }
    }
    catch (const std::out_of_range &err)
    {
    }

    set_value(data, config.directory.length, "directory", "length");
    set_value(data, config.directory.basename_only, "directory", "basename_only");

    set_value(data, config.execution_time.precision, "execution_time", "precision");
    set_value(data, config.execution_time.display_from, "execution_time", "display_from");
    set_value(data, config.execution_time.display_fractional_until, "execution_time", "display_fractional_until");

    set_value(data, config.git_branch.ignore, "git_branch", "ignore");

    set_value(data, config.git_commit.length, "git_commit", "length");

    set_value(data, config.git_status.count, "git_status", "count");
    set_value(data, config.git_status.spacing, "git_status", "spacing");
    set_value(data, config.git_status.clean, "git_status", "clean");
    set_value(data, config.git_status.ahead, "git_status", "ahead");
    set_value(data, config.git_status.behind, "git_status", "behind");
    set_value(data, config.git_status.stashed, "git_status", "stashed");
    set_value(data, config.git_status.untracked, "git_status", "untracked");
    set_value(data, config.git_status.conflicted, "git_status", "conflicted");
    set_value(data, config.git_status.modified, "git_status", "modified");
    set_value(data, config.git_status.staged, "git_status", "staged");
    set_value(data, config.git_status.renamed, "git_status", "renamed");
    set_value(data, config.git_status.deleted, "git_status", "deleted");

    set_value(data, config.date.format, "date", "format");

    set_value(data, config.time.format, "time", "format");

    set_value(data, config.exit_status.non_zero_only, "exit_status", "non_zero_only");
}

// Sort modules in ascending order
void sort_modules(std::vector<Module> &modules)
{
    std::unordered_map<std::string, int> sides;
    sides["left"] = 1;
    sides["right"] = 2;
    sides["right_prompt"] = 3;
    auto compare = [&sides](const Module &a, const Module &b)
    {
        std::string a_value = "", b_value = "";
        a_value += std::to_string(a.level);
        a_value += std::to_string(sides[a.align]);
        a_value += std::to_string(a.position);
        b_value += std::to_string(b.level);
        b_value += std::to_string(sides[b.align]);
        b_value += std::to_string(b.position);
        return a_value < b_value;
    };
    std::sort(modules.begin(), modules.end(), compare);
}

// Set the default values, in case there is no config file
void set_default_values(Config &config)
{
    config.global.padding = "";
    config.execution_time.display_from = 2;
    config.exit_status.non_zero_only = true;

    Prompt prompt;
    prompt.string = " ❯ ";
    prompt.foreground = "green";
    prompt.error_foreground = "red";
    config.prompt = prompt;

    Module directory;
    directory.name = "directory";
    directory.align = "left";
    directory.foreground = "blue";
    directory.bold = true;
    config.modules.push_back(directory);

    Module execution_time;
    execution_time.name = "execution_time";
    execution_time.align = "left";
    execution_time.inner_prefix = "⌛︎";
    execution_time.outer_prefix = " ";
    execution_time.foreground = "yellow";
    config.modules.push_back(execution_time);

    Module exit_status;
    exit_status.name = "exit_status";
    exit_status.align = "left";
    exit_status.inner_prefix = "⚠ ";
    exit_status.outer_prefix = " ";
    exit_status.foreground = "red";
    config.modules.push_back(exit_status);
}

Config get_config()
{
    Config config;
    toml::value data = read_data();
    if (data.is_uninitialized())
    {
        set_default_values(config);
    }
    else
    {
        load_values(data, config);
    }
    sort_modules(config.modules);
    return config;
}

Module *get_previous_module_in_group(std::vector<Module> &modules, const std::size_t index)
{
    Module *previous = NULL;
    if (index == 0)
    {
        return previous;
    }
    Module &tmp = modules[index - 1];
    if (tmp.level == modules[index].level && tmp.align == modules[index].align)
    {
        previous = &tmp;
    }
    return previous;
}

Module *get_next_module_in_group(std::vector<Module> &modules, const std::size_t index)
{
    Module *next = NULL;
    if (index == modules.size() - 1)
    {
        return next;
    }
    Module &tmp = modules[index + 1];
    if (tmp.level == modules[index].level && tmp.align == modules[index].align)
    {
        next = &tmp;
    }
    return next;
}

std::vector<Module *> get_modules_on_level(std::vector<Module> &modules, const int level)
{
    std::vector<Module *> result;
    for (Module &module : modules)
    {
        if (module.level < level)
        {
            continue;
        }
        else if (module.level == level)
        {
            result.push_back(&module);
        }
        else if (module.level > level)
        {
            break;
        }
    }
    return result;
}

bool contains_content_on_right(std::vector<Module> &modules, const int level)
{
    std::vector<Module *> modules_on_level = get_modules_on_level(modules, level);
    for (const Module *module : modules_on_level)
    {
        if (module->align == "right")
        {
            return true;
        }
    }
    return false;
}

// ----------------------------------- TESTS -----------------------------------
#include "src/test.hpp"
#ifdef TEST

TEST_CASE("read_data")
{
    const toml::value data = read_data();
    // Read user config file
    CHECK(data.is_uninitialized() == false);
}

TEST_CASE("sort_modules")
{
    std::vector<Module> modules;

    Module a;
    a.name = "a";
    a.align = "right";
    a.level = 1;
    a.position = 1;
    modules.push_back(a);

    Module b;
    b.name = "b";
    b.align = "left";
    b.level = 1;
    b.position = 2;
    modules.push_back(b);

    Module c;
    c.name = "c";
    c.align = "left";
    c.level = 2;
    c.position = 1;
    modules.push_back(c);

    sort_modules(modules);
    CHECK(modules.size() == 3);
    CHECK(modules[0].name == "b");
    CHECK(modules[1].name == "a");
    CHECK(modules[2].name == "c");
}

TEST_CASE("set_default_values")
{
    Config config;
    set_default_values(config);
    CHECK(config.modules.size() == 3);
}

TEST_CASE("get_config")
{
    const Config config = get_config();
    // User config or default values will be loaded
    CHECK(config.modules.size() > 1);
}

TEST_CASE("get_previous_module_in_group")
{
    std::vector<Module> modules;

    Module a;
    a.name = "a";
    modules.push_back(a);

    Module b;
    b.name = "b";
    modules.push_back(b);

    Module c;
    c.name = "c";
    modules.push_back(c);

    SUBCASE("0")
    {
        const Module *module = get_previous_module_in_group(modules, 0);
        CHECK(module == NULL);
    }
    SUBCASE("1")
    {
        const Module *module = get_previous_module_in_group(modules, 1);
        CHECK(module->name == "a");
    }
    SUBCASE("2")
    {
        const Module *module = get_previous_module_in_group(modules, 2);
        CHECK(module->name == "b");
    }
}

TEST_CASE("get_next_module_in_group")
{
    std::vector<Module> modules;

    Module a;
    a.name = "a";
    modules.push_back(a);

    Module b;
    b.name = "b";
    modules.push_back(b);

    Module c;
    c.name = "c";
    modules.push_back(c);

    SUBCASE("0")
    {
        const Module *module = get_next_module_in_group(modules, 0);
        CHECK(module->name == "b");
    }
    SUBCASE("1")
    {
        const Module *module = get_next_module_in_group(modules, 1);
        CHECK(module->name == "c");
    }
    SUBCASE("2")
    {
        const Module *module = get_next_module_in_group(modules, 2);
        CHECK(module == NULL);
    }
}

TEST_CASE("get_modules_on_level")
{
    std::vector<Module> modules;

    Module a;
    a.name = "a";
    a.level = 1;
    modules.push_back(a);

    Module b;
    b.name = "b";
    b.level = 2;
    modules.push_back(b);

    Module c;
    c.name = "c";
    c.level = 2;
    modules.push_back(c);

    SUBCASE("1")
    {
        const std::vector<Module *> result = get_modules_on_level(modules, 1);
        CHECK(result.size() == 1);
        CHECK(result[0]->name == "a");
    }
    SUBCASE("2")
    {
        const std::vector<Module *> result = get_modules_on_level(modules, 2);
        CHECK(result.size() == 2);
        CHECK(result[0]->name == "b");
        CHECK(result[1]->name == "c");
    }
}

TEST_CASE("contains_content_on_right")
{
    std::vector<Module> modules;

    Module a;
    a.name = "a";
    a.content = "a";
    a.level = 1;
    modules.push_back(a);

    Module b;
    b.name = "b";
    b.content = "b";
    b.level = 2;
    modules.push_back(b);

    Module c;
    c.name = "c";
    c.content = "c";
    c.level = 2;
    c.align = "right";
    modules.push_back(c);

    SUBCASE("1")
    {
        const bool result = contains_content_on_right(modules, 1);
        CHECK(result == false);
    }
    SUBCASE("2")
    {
        const bool result = contains_content_on_right(modules, 2);
        CHECK(result == true);
    }
}

#endif
