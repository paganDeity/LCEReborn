#include "mod/ModApi.hpp"

#include <string.h>

namespace mod
{
namespace api
{
int FindHook(const char* name)
{
    for (int index = 0; index < eHook_Count; ++index)
        if (strcmp(kHooks[index].name, name) == 0)
            return index;
    return -1;
}
}
}
