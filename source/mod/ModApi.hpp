#ifndef MOD_MODAPI_HPP
#define MOD_MODAPI_HPP

#include "mod/ModApiIds_generated.hpp"
#include "mod/ModValue.hpp"

namespace mod
{
namespace api
{
struct ArgumentInfo
{
    const char* name;
    Value::EType type;
    unsigned int handle;
    bool mutableArgument;
};

struct FunctionInfo
{
    const char* module;
    const char* name;
    const ArgumentInfo* arguments;
    int argumentCount;
    Value::EType result;
    unsigned int resultHandle;
    bool isStatic;
};

struct HookInfo
{
    const char* name;
    const ArgumentInfo* arguments;
    int argumentCount;
    bool cancel;
};

struct HandleBase
{
    unsigned int derived;
    unsigned int base;
};

extern const char* const kHandleNames[];
extern const HandleBase kHandleBases[];
extern const int kHandleBaseCount;
// whether a handle of kind actual may be used where kind expected is wanted, because its class derives from the other's
bool HandleAccepts(unsigned int expected, unsigned int actual);
extern const FunctionInfo kFunctions[];
extern const HookInfo kHooks[];

int FindHook(const char* name);
}
}

#endif
