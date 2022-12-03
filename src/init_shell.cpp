#include <string>
#include <unordered_map>

#include "init/init_bash.hpp"

void init_shell(std::string shell)
{
    std::unordered_map<std::string, void (*)()> shells;
    shells["bash"] = &init_bash;
    shells[shell]();
}
