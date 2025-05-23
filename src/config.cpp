#include <algorithm>
#include <unordered_map>

#include "toml/toml.hpp"
#include "config.hpp"
#include "util.hpp"

// Read the user's config file
toml::value read_data()
{
    const std::string paths[] = {
        get_env("BLAZE_CONFIG"),
        get_env("HOME") + "/.config/blaze/config.toml"
    };
    toml::value data;
    for (const std::string &path : paths)
    {
        if (path.empty())
        {
            continue;
        }
        try
        {
            data = toml::parse(path);
            break;
        }
        catch (...)
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
    catch (...)
    {
    }
}

// Load the values from the config file
void load_values(const toml::value &data, Config &config)
{
    set_value(data, config.global.padding, "global", "padding");

    set_value(data, config.prompt.string, "prompt", "string");
    set_value(data, config.prompt.foreground, "prompt", "foreground");
    set_value(data, config.prompt.error_foreground, "prompt", "error_foreground");
    set_value(data, config.prompt.new_line, "prompt", "new_line");
    set_value(data, config.prompt.separator, "prompt", "separator");
    set_value(data, config.prompt.transient, "prompt", "transient");
    set_value(data, config.prompt.bottom, "prompt", "bottom");

    set_value(data, config.connector.character, "connector", "character");
    set_value(data, config.connector.foreground, "connector", "foreground");
    set_value(data, config.connector.background, "connector", "background");
    set_value(data, config.connector.dim, "connector", "dim");

    set_value(data, config.directory.limit, "directory", "limit");
    set_value(data, config.directory.from_repository, "directory", "from_repository");

    set_value(data, config.duration.precision, "duration", "precision");
    set_value(data, config.duration.threshold, "duration", "threshold");
    set_value(data, config.duration.fractional_until, "duration", "fractional_until");

    set_value(data, config.git_branch.show_when_detached, "git_branch", "show_when_detached");
    set_value(data, config.git_branch.ignore, "git_branch", "ignore");

    set_value(data, config.git_commit.show_when_attached, "git_commit", "show_when_attached");
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

    set_value(data, config.status.show_zero, "status", "show_zero");

    try
    {
        const toml::array module_array = toml::find(data, "module").as_array();
        config.modules.clear();
        config.modules.reserve(module_array.size());
        try
        {
            for (const toml::value &module_data : module_array)
            {
                Module current;
                set_value(module_data, current.name, "name");
                set_value(module_data, current.execute, "execute");
                set_value(module_data, current.content, "content");
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
        catch (...)
        {
        }
    }
    catch (...)
    {
    }
}

// Sort modules in ascending order
void sort_modules(ModuleVector &modules)
{
    std::unordered_map<std::string, int> sides = {
        {LEFT_ALIGNMENT, 1},
        {RIGHT_ALIGNMENT, 2},
        {RP_ALIGNMENT, 3}
    };
    auto compare = [&sides](const Module &a, const Module &b)
    {
        std::string a_value = "", b_value = "";
        a_value += std::to_string(a.level) + std::to_string(sides[a.align]) + std::to_string(a.position);
        b_value += std::to_string(b.level) + std::to_string(sides[b.align]) + std::to_string(b.position);
        return a_value < b_value;
    };
    std::sort(modules.begin(), modules.end(), compare);
}

// Set the default values, in case there is no config file
void set_default_values(Config &config)
{
    // Add modules only. Don't change other values.
    Module directory;
    directory.name = "directory";
    config.modules.push_back(directory);
}

Config get_config()
{
    Config config;
    toml::value data = read_data();
    set_default_values(config);
    if (!data.is_empty())
    {
        load_values(data, config);
    }
    sort_modules(config.modules);
    return config;
}

Module *get_previous_module_in_group(ModuleVector &modules, const std::size_t index)
{
    Module *previous = nullptr;
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

Module *get_next_module_in_group(ModuleVector &modules, const std::size_t index)
{
    Module *next = nullptr;
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

ModulePtrVector get_modules_on_level(ModuleVector &modules, const int level)
{
    ModulePtrVector result;
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

bool contains_content_on_right(ModuleVector &modules, const int level)
{
    ModulePtrVector modules_on_level = get_modules_on_level(modules, level);
    for (const Module *module : modules_on_level)
    {
        if (module->align == RIGHT_ALIGNMENT)
        {
            return true;
        }
    }
    return false;
}

int vertical_size(const Config &config)
{
    int size = 0;
    // Vertical space by prompt
    size += split(config.prompt.string, "\n").size() - 1;
    // Vertical space by modules (assuming they are sorted)
    size += config.modules.back().level;
    return size;
}

// ----------------------------------- TESTS -----------------------------------
#ifdef TEST
#include "test.hpp"

TEST_CASE("read_data")
{
    // Read users config file
    const toml::value data = read_data();
    CHECK(data.is_empty() == false);
}

TEST_CASE("sort_modules")
{
    ModuleVector modules;

    Module a;
    a.name = "a";
    a.align = RIGHT_ALIGNMENT;
    a.level = 1;
    a.position = 1;
    modules.push_back(a);

    Module b;
    b.name = "b";
    b.align = LEFT_ALIGNMENT;
    b.level = 1;
    b.position = 2;
    modules.push_back(b);

    Module c;
    c.name = "c";
    c.align = RIGHT_ALIGNMENT;
    c.level = 2;
    c.position = 1;
    modules.push_back(c);

    Module d;
    d.name = "d";
    d.align = LEFT_ALIGNMENT;
    d.level = 2;
    d.position = 2;
    modules.push_back(d);

    Module e;
    e.name = "e";
    e.align = LEFT_ALIGNMENT;
    e.level = 2;
    e.position = 1;
    modules.push_back(e);

    Module f;
    f.name = "f";
    f.align = LEFT_ALIGNMENT;
    f.level = 2;
    f.position = 1;
    modules.push_back(f);

    sort_modules(modules);
    CHECK(modules.size() == 6);
    const std::string order[] = {"b", "a", "e", "f", "d", "c"};
    for (std::size_t i = 0; i < modules.size(); i++)
    {
        CHECK(modules[i].name == order[i]);
    }
}

TEST_CASE("set_default_values")
{
    Config config;
    set_default_values(config);
    CHECK(config.prompt.string == " ❯ ");
    CHECK(config.modules.size() == 1);
}

TEST_CASE("get_config")
{
    const Config config = get_config();
    // Make sure the config file is loaded with more modules than default
    CHECK(config.modules.size() > 5);
    int count = 0;
    for (const Module &module : config.modules)
    {
        if (module.name == "directory")
        {
            count++;
        }
    }
    // Check if modules were cleared when reading the config file
    // Only one directory module should be present
    CHECK(count == 1);
}

TEST_CASE("get_previous_module_in_group")
{
    ModuleVector modules;

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
        CHECK(module == nullptr);
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
    ModuleVector modules;

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
        CHECK(module == nullptr);
    }
}

TEST_CASE("get_modules_on_level")
{
    ModuleVector modules;

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
        const ModulePtrVector result = get_modules_on_level(modules, 1);
        CHECK(result.size() == 1);
        CHECK(result[0]->name == "a");
    }
    SUBCASE("2")
    {
        const ModulePtrVector result = get_modules_on_level(modules, 2);
        CHECK(result.size() == 2);
        CHECK(result[0]->name == "b");
        CHECK(result[1]->name == "c");
    }
}

TEST_CASE("contains_content_on_right")
{
    ModuleVector modules;

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
    c.align = RIGHT_ALIGNMENT;
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

TEST_CASE("vertical_size")
{
    Config config;

    Module a;
    a.name = "a";
    config.modules.push_back(a);

    Module b;
    b.name = "b";
    config.modules.push_back(b);

    Module c;
    c.name = "c";
    config.modules.push_back(c);

    SUBCASE("all modules on level 1 and no newlines in prompt")
    {
        CHECK(vertical_size(config) == 1);
    }
    SUBCASE("all modules on level 1 and 2 newlines in prompt")
    {
        config.prompt.string = "\n\n";
        CHECK(vertical_size(config) == 3);
    }
    SUBCASE("one module on level 2 and no newlines in prompt")
    {
        config.modules.back().level = 2;
        config.prompt.string = "";
        CHECK(vertical_size(config) == 2);
    }
    SUBCASE("one module on level 2 and 1 newline in prompt")
    {
        config.modules.back().level = 2;
        config.prompt.string = "\n";
        CHECK(vertical_size(config) == 3);
    }
}

#endif
