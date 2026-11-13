#include "mod/ModLua.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <vector>

#include "lua.hpp"
#include "mod/ModApi.hpp"

namespace mod
{
namespace
{
const char kCommandsKey[] = "lce.commands";
const char kLoadedKey[] = "lce.loaded";
const char kHookHandlersKey[] = "lce.hookhandlers";
const char kListenersKey[] = "lce.listeners";
const char kMetaKey[] = "lce.meta";

void* Allocate(void* userdata, void* pointer, size_t oldSize, size_t newSize)
{
    LuaInstance* instance = static_cast<LuaInstance*>(userdata);
    if (pointer == NULL)
        oldSize = 0;
    if (newSize == 0)
    {
        instance->m_used -= oldSize;
        free(pointer);
        return NULL;
    }
    if (instance->m_used - oldSize + newSize > instance->MemoryLimit())
        return NULL;
    void* result = realloc(pointer, newSize);
    if (result != NULL)
        instance->m_used = instance->m_used - oldSize + newSize;
    return result;
}

LuaInstance* Owner(lua_State* state)
{
    lua_getfield(state, LUA_REGISTRYINDEX, "lce.owner");
    LuaInstance* instance = static_cast<LuaInstance*>(lua_touserdata(state, -1));
    lua_pop(state, 1);
    return instance;
}

void CountHook(lua_State* state, lua_Debug*)
{
    LuaInstance* instance = Owner(state);
    instance->SetBudget(instance->Budget() - 10000);
    if (instance->Budget() <= 0)
        luaL_error(state, "the mod used up its time for one call");
}

int LuaLog(lua_State* state)
{
    LuaInstance* instance = Owner(state);
    instance->GetServices()->Log(instance->Id(), luaL_checkstring(state, 1));
    return 0;
}

int LuaSay(lua_State* state)
{
    Owner(state)->GetServices()->Say(luaL_checkstring(state, 1));
    return 0;
}

int LuaRun(lua_State* state)
{
    std::string reply;
    const bool ok = Owner(state)->GetServices()->Run(luaL_checkstring(state, 1), &reply);
    lua_pushboolean(state, ok);
    lua_pushlstring(state, reply.data(), reply.size());
    return 2;
}

int LuaPlayers(lua_State* state)
{
    const std::vector<std::string> names = Owner(state)->GetServices()->Players();
    lua_createtable(state, static_cast<int>(names.size()), 0);
    for (size_t index = 0; index < names.size(); ++index)
    {
        lua_pushlstring(state, names[index].data(), names[index].size());
        lua_rawseti(state, -2, static_cast<lua_Integer>(index + 1));
    }
    return 1;
}

int LuaTime(lua_State* state)
{
    lua_pushinteger(state, static_cast<lua_Integer>(Owner(state)->GetServices()->TimeMs()));
    return 1;
}

int LuaFileRead(lua_State* state)
{
    std::string data;
    if (!Owner(state)->Data().Read(luaL_checkstring(state, 1), data))
    {
        lua_pushnil(state);
        return 1;
    }
    lua_pushlstring(state, data.data(), data.size());
    return 1;
}

int LuaFileReadMod(lua_State* state)
{
    std::string data;
    std::string problem;
    const std::string name = luaL_checkstring(state, 1);
    if (!DataFiles::Safe(name) || !Owner(state)->Source()->Read(name, data, &problem))
    {
        lua_pushnil(state);
        return 1;
    }
    lua_pushlstring(state, data.data(), data.size());
    return 1;
}

int LuaFileWrite(lua_State* state)
{
    size_t length = 0;
    const char* text = luaL_checklstring(state, 2, &length);
    lua_pushboolean(state, Owner(state)->Data().Write(luaL_checkstring(state, 1), std::string(text, length)));
    return 1;
}

int LuaFileExists(lua_State* state)
{
    lua_pushboolean(state, Owner(state)->Data().Exists(luaL_checkstring(state, 1)));
    return 1;
}

int LuaFileReadGame(lua_State* state)
{
    std::string data;
    const std::string name = luaL_checkstring(state, 1);
    if (!DataFiles::Safe(name) || !Owner(state)->GetServices()->ReadGameFile(name, &data))
    {
        lua_pushnil(state);
        return 1;
    }
    lua_pushlstring(state, data.data(), data.size());
    return 1;
}

bool ValidCommand(const std::string& name)
{
    if (name.empty() || name.size() > 32)
        return false;
    for (size_t index = 0; index < name.size(); ++index)
    {
        const char letter = name[index];
        if (!((letter >= 'a' && letter <= 'z') || (letter >= '0' && letter <= '9') || letter == '_'))
            return false;
    }
    return true;
}

int LuaCommand(lua_State* state)
{
    const std::string name = luaL_checkstring(state, 1);
    luaL_checktype(state, 2, LUA_TFUNCTION);
    if (!ValidCommand(name))
        return luaL_error(state, "a command name is 1 to 32 characters of a-z, 0-9 and _");
    // an optional usage text and the permission level that may run it
    const std::string usage = luaL_optstring(state, 3, "");
    const int permission = static_cast<int>(luaL_optinteger(state, 4, 0));
    const std::string arguments = luaL_optstring(state, 5, "");
    lua_getfield(state, LUA_REGISTRYINDEX, kCommandsKey);
    lua_pushvalue(state, 2);
    lua_setfield(state, -2, name.c_str());
    Owner(state)->AddCommand(name, usage, permission, arguments);
    return 0;
}

int LuaRequire(lua_State* state)
{
    const std::string name = luaL_checkstring(state, 1);
    LuaInstance* instance = Owner(state);
    const size_t colon = name.find(':');
    const std::string library = colon == std::string::npos ? std::string() : name.substr(0, colon);
    const std::string module = colon == std::string::npos ? name : name.substr(colon + 1);
    for (size_t index = 0; index < module.size(); ++index)
    {
        const char letter = module[index];
        if (!((letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z') || (letter >= '0' && letter <= '9')
              || letter == '_' || letter == '-'))
            return luaL_error(state, "a module name is letters, digits, _ and - only");
    }
    const archive::Archive* source = instance->Source();
    char owner[32];
    snprintf(owner, sizeof(owner), "%p", static_cast<const void*>(source));
    std::string key = library.empty() ? std::string(owner) + "|" + module : "lib|" + name;
    if (!library.empty())
    {
        source = instance->DependencySource(library);
        if (source == NULL)
            return luaL_error(state, "%s is not a mod this one depends on", library.c_str());
    }
    lua_getfield(state, LUA_REGISTRYINDEX, kLoadedKey);
    lua_getfield(state, -1, key.c_str());
    if (!lua_isnil(state, -1))
        return 1;
    lua_pop(state, 1);
    std::string text;
    std::string problem;
    if (!source->Read(module + ".lua", text, &problem))
        return luaL_error(state, "no module %s", name.c_str());
    if (luaL_loadbufferx(state, text.data(), text.size(),
                         (name + ".lua").c_str(), "t") != LUA_OK)
        return lua_error(state);
    instance->PushSource(source);
    lua_call(state, 0, 1);
    instance->PopSource();
    if (lua_isnil(state, -1))
    {
        lua_pop(state, 1);
        lua_pushboolean(state, 1);
    }
    lua_pushvalue(state, -1);
    lua_setfield(state, -3, key.c_str());
    return 1;
}

int LuaClock(lua_State* state)
{
    lua_pushnumber(state, static_cast<lua_Number>(clock()) / CLOCKS_PER_SEC);
    return 1;
}

int LuaWallTime(lua_State* state)
{
    lua_pushinteger(state, static_cast<lua_Integer>(time(NULL)));
    return 1;
}

int LuaPrint(lua_State* state)
{
    std::string line;
    const int count = lua_gettop(state);
    for (int index = 1; index <= count; ++index)
    {
        size_t length = 0;
        const char* text = luaL_tolstring(state, index, &length);
        if (index > 1)
            line += "\t";
        line.append(text, length);
        lua_pop(state, 1);
    }
    LuaInstance* instance = Owner(state);
    instance->GetServices()->Log(instance->Id(), line);
    return 0;
}

void PushValue(lua_State* state, const Value& value)
{
    switch (value.Type())
    {
    case Value::eType_Bool:
        lua_pushboolean(state, value.Bool());
        break;
    case Value::eType_Int:
        lua_pushinteger(state, static_cast<lua_Integer>(value.Int()));
        break;
    case Value::eType_Float:
        lua_pushnumber(state, value.Float());
        break;
    case Value::eType_String:
        lua_pushlstring(state, value.String().data(), value.String().size());
        break;
    case Value::eType_Handle:
        lua_createtable(state, 0, 2);
        lua_pushinteger(state, value.Kind());
        lua_setfield(state, -2, "kind");
        lua_pushinteger(state, static_cast<lua_Integer>(value.Id()));
        lua_setfield(state, -2, "id");
        lua_getfield(state, LUA_REGISTRYINDEX, kMetaKey);
        lua_rawgeti(state, -1, static_cast<lua_Integer>(value.Kind()));
        lua_setmetatable(state, -3);
        lua_pop(state, 1);
        break;
    case Value::eType_List:
    {
        // a Lua sequence, from 1, with each item converted the same way
        const std::vector<Value> items = value.Items();
        lua_createtable(state, static_cast<int>(items.size()), 0);
        for (size_t index = 0; index < items.size(); ++index)
        {
            PushValue(state, items[index]);
            lua_rawseti(state, -2, static_cast<lua_Integer>(index + 1));
        }
        break;
    }
    default:
        lua_pushnil(state);
        break;
    }
}

bool ReadValue(lua_State* state, int index, const api::ArgumentInfo& expected, Value* value)
{
    switch (expected.type)
    {
    case Value::eType_Bool:
        *value = Value::FromBool(lua_toboolean(state, index) != 0);
        return true;
    case Value::eType_Int:
        if (!lua_isnumber(state, index))
            return false;
        *value = Value::FromInt(static_cast<int64_t>(lua_tonumber(state, index)));
        return true;
    case Value::eType_Float:
        if (!lua_isnumber(state, index))
            return false;
        *value = Value::FromFloat(lua_tonumber(state, index));
        return true;
    case Value::eType_String:
    {
        if (lua_type(state, index) != LUA_TSTRING)
            return false;
        size_t length = 0;
        const char* text = lua_tolstring(state, index, &length);
        *value = Value::FromString(std::string(text, length));
        return true;
    }
    case Value::eType_Handle:
    {
        if (!lua_istable(state, index))
            return false;
        // a relative index would point at the field just pushed by the next call
        index = lua_absindex(state, index);
        lua_getfield(state, index, "kind");
        lua_getfield(state, index, "id");
        const bool good = lua_isinteger(state, -2) && lua_isinteger(state, -1)
                          && api::HandleAccepts(expected.handle, static_cast<unsigned int>(lua_tointeger(state, -2)));
        if (good)
            // the kind it really is, which a call that takes several (a mob by pointer or an entity handle) tells apart by
            *value = Value::FromHandle(static_cast<unsigned int>(lua_tointeger(state, -2)), static_cast<uint32_t>(lua_tointeger(state, -1)));
        lua_pop(state, 2);
        return good;
    }
    default:
        return false;
    }
}


struct ApiIndex
{
    // the modules of the static functions, each once, and the functions of each module and of each handle kind
    std::vector<const char*> staticModules;
    std::vector<std::vector<int> > moduleFunctions;
    std::vector<std::vector<int> > kindFunctions;
    std::vector<std::vector<unsigned int> > kindBases;
};

const ApiIndex& Index()
{
    static ApiIndex index;
    static bool built = false;
    if (built)
        return index;
    built = true;
    index.kindFunctions.resize(api::eHandle_Count);
    index.kindBases.resize(api::eHandle_Count);
    for (int function = 0; function < api::eFunction_Count; ++function)
    {
        const api::FunctionInfo& info = api::kFunctions[function];
        if (info.isStatic)
        {
            size_t module = 0;
            while (module < index.staticModules.size() && strcmp(index.staticModules[module], info.module) != 0)
                ++module;
            if (module == index.staticModules.size())
            {
                index.staticModules.push_back(info.module);
                index.moduleFunctions.push_back(std::vector<int>());
            }
            index.moduleFunctions[module].push_back(function);
        }
        else
            index.kindFunctions[info.arguments[0].handle].push_back(function);
    }
    for (int pair = 0; pair < api::kHandleBaseCount; ++pair)
        index.kindBases[api::kHandleBases[pair].derived].push_back(api::kHandleBases[pair].base);
    return index;
}

int ApiCall(lua_State* state);

void PushApiFunction(lua_State* state, int function)
{
    lua_pushinteger(state, function);
    lua_pushcclosure(state, ApiCall, 1);
    lua_setfield(state, -2, api::kFunctions[function].name);
}

// the table on top of the stack gets a metatable whose __index is the given function with the number as its one upvalue
void SetLazy(lua_State* state, lua_CFunction lookup, int number)
{
    lua_newtable(state);
    lua_pushinteger(state, number);
    lua_pushcclosure(state, lookup, 1);
    lua_setfield(state, -2, "__index");
    lua_setmetatable(state, -2);
}

// __index(module table, name): fills the whole module, then answers
int LazyModule(lua_State* state)
{
    const std::vector<int>& functions = Index().moduleFunctions[static_cast<size_t>(lua_tointeger(state, lua_upvalueindex(1)))];
    for (size_t index = 0; index < functions.size(); ++index)
    {
        lua_pushvalue(state, 1);
        PushApiFunction(state, functions[index]);
        lua_pop(state, 1);
    }
    lua_pushnil(state);
    lua_setmetatable(state, 1);
    lua_pushvalue(state, 2);
    lua_rawget(state, 1);
    return 1;
}

// __index(base lookup, name): the first of the base kinds that has it
int LazyBases(lua_State* state)
{
    const int count = static_cast<int>(lua_rawlen(state, lua_upvalueindex(1)));
    for (int index = 1; index <= count; ++index)
    {
        lua_rawgeti(state, lua_upvalueindex(1), index);
        lua_pushvalue(state, 2);
        lua_gettable(state, -2);
        if (!lua_isnil(state, -1))
            return 1;
        lua_pop(state, 2);
    }
    lua_pushnil(state);
    return 1;
}

// __index(method table of a kind, name): fills the kind's own methods, chains the kinds it derives from, then answers
int LazyKind(lua_State* state)
{
    const unsigned int kind = static_cast<unsigned int>(lua_tointeger(state, lua_upvalueindex(1)));
    const std::vector<int>& functions = Index().kindFunctions[kind];
    for (size_t index = 0; index < functions.size(); ++index)
    {
        lua_pushvalue(state, 1);
        PushApiFunction(state, functions[index]);
        lua_pop(state, 1);
    }
    const std::vector<unsigned int>& bases = Index().kindBases[kind];
    if (bases.empty())
    {
        lua_pushnil(state);
        lua_setmetatable(state, 1);
    }
    else
    {
        lua_createtable(state, static_cast<int>(bases.size()), 0);
        for (size_t index = 0; index < bases.size(); ++index)
        {
            lua_getfield(state, LUA_REGISTRYINDEX, kMetaKey);
            lua_rawgeti(state, -1, static_cast<lua_Integer>(bases[index]));
            lua_remove(state, -2);
            lua_rawseti(state, -2, static_cast<lua_Integer>(index + 1));
        }
        lua_pushcclosure(state, LazyBases, 1);
        lua_newtable(state);
        lua_pushvalue(state, -2);
        lua_setfield(state, -2, "__index");
        lua_remove(state, -2);
        lua_setmetatable(state, 1);
    }
    lua_pushvalue(state, 2);
    lua_gettable(state, 1);
    return 1;
}

int ApiCall(lua_State* state)
{
    const int function = static_cast<int>(lua_tointeger(state, lua_upvalueindex(1)));
    const api::FunctionInfo& info = api::kFunctions[function];
    std::vector<Value> arguments(info.argumentCount);
    for (int index = 0; index < info.argumentCount; ++index)
    {
        // nil stands for no object where a call takes one, as the game's own functions take a null pointer; the first argument is the object itself
        if (index > 0 && info.arguments[index].type == Value::eType_Handle && lua_isnil(state, index + 1))
        {
            arguments[index] = Value::FromHandle(info.arguments[index].handle, 0);
            continue;
        }
        if (!ReadValue(state, index + 1, info.arguments[index], &arguments[index]))
            return luaL_error(state, "bad argument %d to %s (%s expected)", index + 1, info.name,
                              index == 0 ? info.module : "another type");
    }
    Value result;
    if (!Owner(state)->GetServices()->Call(Owner(state)->Id(), function, arguments, &result))
        return luaL_error(state, "%s failed (a handle may be stale)", info.name);
    if (info.result == Value::eType_Nil)
        return 0;
    PushValue(state, result);
    return 1;
}

int LuaHook(lua_State* state)
{
    const char* name = luaL_checkstring(state, 1);
    luaL_checktype(state, 2, LUA_TFUNCTION);
    const int hook = api::FindHook(name);
    if (hook < 0)
        return luaL_error(state, "there is no hook called %s", name);
    lua_getfield(state, LUA_REGISTRYINDEX, kHookHandlersKey);
    lua_rawgeti(state, -1, hook);
    if (lua_isnil(state, -1))
    {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_pushvalue(state, -1);
        lua_rawseti(state, -3, hook);
    }
    lua_pushvalue(state, 2);
    lua_rawseti(state, -2, static_cast<lua_Integer>(lua_rawlen(state, -2) + 1));
    int priority = 0;
    if (lua_istable(state, 3))
    {
        lua_getfield(state, 3, "priority");
        if (lua_isnumber(state, -1))
            priority = static_cast<int>(lua_tonumber(state, -1));
        lua_pop(state, 1);
    }
    Owner(state)->AddHook(hook, priority);
    return 0;
}

int LuaListen(lua_State* state)
{
    const char* name = luaL_checkstring(state, 1);
    luaL_checktype(state, 2, LUA_TFUNCTION);
    lua_getfield(state, LUA_REGISTRYINDEX, kListenersKey);
    lua_getfield(state, -1, name);
    if (lua_isnil(state, -1))
    {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_pushvalue(state, -1);
        lua_setfield(state, -3, name);
    }
    lua_pushvalue(state, 2);
    lua_rawseti(state, -2, static_cast<lua_Integer>(lua_rawlen(state, -2) + 1));
    Owner(state)->AddListener(name);
    return 0;
}

bool ScalarValue(lua_State* state, int index, Value* value)
{
    switch (lua_type(state, index))
    {
    case LUA_TBOOLEAN:
        *value = Value::FromBool(lua_toboolean(state, index) != 0);
        return true;
    case LUA_TNUMBER:
        *value = lua_isinteger(state, index) ? Value::FromInt(lua_tointeger(state, index)) : Value::FromFloat(lua_tonumber(state, index));
        return true;
    case LUA_TSTRING:
    {
        size_t length = 0;
        const char* text = lua_tolstring(state, index, &length);
        *value = Value::FromString(std::string(text, length));
        return true;
    }
    case LUA_TTABLE:
    {
        lua_getfield(state, index, "kind");
        lua_getfield(state, index, "id");
        const bool handle = lua_isinteger(state, -2) && lua_isinteger(state, -1);
        if (handle)
            *value = Value::FromHandle(static_cast<unsigned int>(lua_tointeger(state, -2)), static_cast<uint32_t>(lua_tointeger(state, -1)));
        lua_pop(state, 2);
        return handle;
    }
    case LUA_TNIL:
        *value = Value();
        return true;
    default:
        return false;
    }
}

int LuaFire(lua_State* state)
{
    const char* name = luaL_checkstring(state, 1);
    std::vector<Value> arguments;
    for (int index = 2; index <= lua_gettop(state); ++index)
    {
        Value value;
        if (!ScalarValue(state, index, &value))
            return luaL_error(state, "an event argument is a boolean, number, string, handle or nil");
        arguments.push_back(value);
    }
    lua_pushboolean(state, Owner(state)->Bus() == NULL || Owner(state)->Bus()->Fire(name, arguments, Owner(state)));
    return 1;
}

void Register(lua_State* state, const char* table, const char* name, lua_CFunction function)
{
    lua_getglobal(state, table);
    lua_pushcfunction(state, function);
    lua_setfield(state, -2, name);
    lua_pop(state, 1);
}
}

LuaInstance::LuaInstance(const Context& context, const Manifest& manifest)
    : m_used(0), m_context(context), m_manifest(manifest), m_state(NULL), m_failures(0), m_budget(0)
{
    m_memoryLimit = m_context.limits.memoryBytes;
    if (manifest.memoryMegabytes > 0)
        m_memoryLimit = static_cast<size_t>(manifest.memoryMegabytes) * 1024 * 1024;
    if (m_memoryLimit > m_context.limits.maxMemoryBytes)
        m_memoryLimit = m_context.limits.maxMemoryBytes;
    m_instructionBudget = m_context.limits.instructionBudget;
    if (manifest.budgetMillions > 0)
        m_instructionBudget = manifest.budgetMillions * 1000000;
    if (m_instructionBudget > m_context.limits.maxInstructionBudget)
        m_instructionBudget = m_context.limits.maxInstructionBudget;
    m_requireSources.push_back(context.source);
}

const archive::Archive* LuaInstance::DependencySource(const std::string& id) const
{
    for (size_t index = 0; index < m_context.dependencies.size(); ++index)
        if (m_context.dependencies[index].first == id)
            return m_context.dependencies[index].second;
    return NULL;
}

int LuaInstance::HookPriority(int hook) const
{
    for (size_t index = 0; index < m_hooks.size(); ++index)
        if (m_hooks[index] == hook)
            return m_hookPriorities[index];
    return 0;
}

LuaInstance::~LuaInstance()
{
    if (m_state != NULL)
        lua_close(m_state);
}

void LuaInstance::Fail(const std::string& message)
{
    m_error = message;
    m_context.services->Log(m_manifest.id, "error: " + message);
}

void LuaInstance::NoteFailure(const std::string& message)
{
    ++m_failures;
    Fail(message);
}

bool LuaInstance::Call(int arguments, int results, std::string* error)
{
    m_budget = m_instructionBudget;
    if (lua_pcall(m_state, arguments, results, 0) == LUA_OK)
        return true;
    const char* text = lua_tostring(m_state, -1);
    if (error != NULL)
        *error = text != NULL ? text : "an error without a message";
    lua_pop(m_state, 1);
    return false;
}

bool LuaInstance::Start(std::string* error)
{
    m_state = lua_newstate(&Allocate, this);
    if (m_state == NULL)
    {
        *error = "not enough memory to start";
        return false;
    }
    lua_pushlightuserdata(m_state, this);
    lua_setfield(m_state, LUA_REGISTRYINDEX, "lce.owner");
    const luaL_Reg libraries[] = {{LUA_GNAME, luaopen_base},     {LUA_TABLIBNAME, luaopen_table}, {LUA_STRLIBNAME, luaopen_string},
                                  {LUA_MATHLIBNAME, luaopen_math}, {LUA_UTF8LIBNAME, luaopen_utf8},  {LUA_COLIBNAME, luaopen_coroutine},
                                  {NULL, NULL}};
    for (const luaL_Reg* library = libraries; library->func != NULL; ++library)
    {
        luaL_requiref(m_state, library->name, library->func, 1);
        lua_pop(m_state, 1);
    }
    static const char* const removed[] = {"dofile", "loadfile", "load", "loadstring", "collectgarbage", NULL};
    for (int index = 0; removed[index] != NULL; ++index)
    {
        lua_pushnil(m_state);
        lua_setglobal(m_state, removed[index]);
    }
    lua_pushcfunction(m_state, LuaPrint);
    lua_setglobal(m_state, "print");
    lua_pushcfunction(m_state, LuaRequire);
    lua_setglobal(m_state, "require");

    lua_newtable(m_state);
    lua_setfield(m_state, LUA_REGISTRYINDEX, kCommandsKey);
    lua_newtable(m_state);
    lua_setfield(m_state, LUA_REGISTRYINDEX, kLoadedKey);

    lua_newtable(m_state);
    lua_setglobal(m_state, "game");
    lua_newtable(m_state);
    lua_setglobal(m_state, "mod");
    lua_newtable(m_state);
    lua_setglobal(m_state, "file");
    lua_newtable(m_state);
    lua_setglobal(m_state, "os");
    Register(m_state, "game", "log", LuaLog);
    Register(m_state, "game", "say", LuaSay);
    Register(m_state, "game", "run", LuaRun);
    Register(m_state, "game", "players", LuaPlayers);
    Register(m_state, "game", "time_ms", LuaTime);
    Register(m_state, "file", "read", LuaFileRead);
    Register(m_state, "file", "write", LuaFileWrite);
    Register(m_state, "file", "exists", LuaFileExists);
    Register(m_state, "file", "read_game", LuaFileReadGame);
    Register(m_state, "file", "read_mod", LuaFileReadMod);
    Register(m_state, "mod", "command", LuaCommand);
    Register(m_state, "os", "clock", LuaClock);
    Register(m_state, "os", "time", LuaWallTime);
    lua_getglobal(m_state, "mod");
    lua_pushlstring(m_state, m_manifest.id.data(), m_manifest.id.size());
    lua_setfield(m_state, -2, "id");
    lua_pushlstring(m_state, m_manifest.version.data(), m_manifest.version.size());
    lua_setfield(m_state, -2, "version");
    lua_pop(m_state, 1);

    Register(m_state, "mod", "hook", LuaHook);
    lua_newtable(m_state);
    lua_setfield(m_state, LUA_REGISTRYINDEX, kHookHandlersKey);
    lua_newtable(m_state);
    lua_setfield(m_state, LUA_REGISTRYINDEX, kListenersKey);
    Register(m_state, "mod", "listen", LuaListen);
    Register(m_state, "mod", "fire", LuaFire);
    // the API is thousands of functions, and a mod uses a few: a module's functions and a handle kind's methods are put in their table the first time
    // the mod looks one up, so a mod that never touches the world pays nothing for it
    lua_newtable(m_state);
    for (int kind = 1; kind < api::eHandle_Count; ++kind)
    {
        lua_newtable(m_state);
        lua_pushvalue(m_state, -1);
        lua_setfield(m_state, -2, "__index");
        SetLazy(m_state, LazyKind, kind);
        lua_rawseti(m_state, -2, kind);
    }
    lua_setfield(m_state, LUA_REGISTRYINDEX, kMetaKey);
    for (size_t module = 0; module < Index().staticModules.size(); ++module)
    {
        lua_getglobal(m_state, Index().staticModules[module]);
        if (lua_isnil(m_state, -1))
        {
            lua_pop(m_state, 1);
            lua_newtable(m_state);
            lua_pushvalue(m_state, -1);
            lua_setglobal(m_state, Index().staticModules[module]);
        }
        SetLazy(m_state, LazyModule, static_cast<int>(module));
        lua_pop(m_state, 1);
    }

    lua_sethook(m_state, CountHook, LUA_MASKCOUNT, 10000);

    std::string source;
    std::string problem;
    if (!m_context.source->Read(m_manifest.entry, source, &problem))
    {
        *error = "the entry file can't be read";
        return false;
    }
    if (luaL_loadbufferx(m_state, source.data(), source.size(),
                         m_manifest.entry.c_str(), "t") != LUA_OK)
    {
        const char* text = lua_tostring(m_state, -1);
        *error = text != NULL ? text : "the entry file does not compile";
        lua_pop(m_state, 1);
        return false;
    }
    return Call(0, 0, error);
}

void LuaInstance::AddHook(int hook, int priority)
{
    for (size_t index = 0; index < m_hooks.size(); ++index)
    {
        if (m_hooks[index] == hook)
        {
            if (priority < m_hookPriorities[index])
                m_hookPriorities[index] = priority;
            return;
        }
    }
    m_hooks.push_back(hook);
    m_hookPriorities.push_back(priority);
}

void LuaInstance::AddListener(const std::string& name)
{
    for (size_t index = 0; index < m_listens.size(); ++index)
        if (m_listens[index] == name)
            return;
    m_listens.push_back(name);
}

bool LuaInstance::Custom(const std::string& name, std::vector<Value>& arguments, bool* cancel)
{
    if (m_state == NULL || Failed())
        return false;
    lua_getfield(m_state, LUA_REGISTRYINDEX, kListenersKey);
    lua_getfield(m_state, -1, name.c_str());
    if (!lua_istable(m_state, -1))
    {
        lua_pop(m_state, 2);
        return true;
    }
    const size_t count = lua_rawlen(m_state, -1);
    bool clean = true;
    for (size_t listener = 1; listener <= count && !Failed(); ++listener)
    {
        lua_rawgeti(m_state, -1, static_cast<lua_Integer>(listener));
        for (size_t index = 0; index < arguments.size(); ++index)
            PushValue(m_state, arguments[index]);
        std::string problem;
        if (!Call(static_cast<int>(arguments.size()), 1, &problem))
        {
            NoteFailure(problem);
            clean = false;
            continue;
        }
        m_failures = 0;
        if (lua_isboolean(m_state, -1) && lua_toboolean(m_state, -1) == 0)
            *cancel = true;
        lua_pop(m_state, 1);
    }
    lua_pop(m_state, 2);
    return clean;
}

bool LuaInstance::Hook(int hook, std::vector<Value>& arguments, bool* cancel)
{
    if (m_state == NULL || Failed() || hook < 0 || hook >= api::eHook_Count)
        return false;
    lua_getfield(m_state, LUA_REGISTRYINDEX, kHookHandlersKey);
    lua_rawgeti(m_state, -1, hook);
    if (!lua_istable(m_state, -1))
    {
        lua_pop(m_state, 2);
        return true;
    }
    const api::HookInfo& info = api::kHooks[hook];
    const size_t count = lua_rawlen(m_state, -1);
    bool clean = true;
    for (size_t handler = 1; handler <= count && !Failed(); ++handler)
    {
        lua_rawgeti(m_state, -1, static_cast<lua_Integer>(handler));
        lua_createtable(m_state, 0, info.argumentCount + 1);
        for (int index = 0; index < info.argumentCount && index < static_cast<int>(arguments.size()); ++index)
        {
            PushValue(m_state, arguments[index]);
            lua_setfield(m_state, -2, info.arguments[index].name);
        }
        lua_pushboolean(m_state, 0);
        lua_setfield(m_state, -2, "cancel");
        lua_pushvalue(m_state, -1);
        lua_insert(m_state, -3);
        std::string problem;
        if (!Call(1, 1, &problem))
        {
            lua_pop(m_state, 1);
            NoteFailure(problem);
            clean = false;
            continue;
        }
        m_failures = 0;
        if (info.cancel && lua_isboolean(m_state, -1) && lua_toboolean(m_state, -1) == 0)
            *cancel = true;
        lua_pop(m_state, 1);
        lua_getfield(m_state, -1, "cancel");
        if (info.cancel && lua_toboolean(m_state, -1) != 0)
            *cancel = true;
        lua_pop(m_state, 1);
        for (int index = 0; index < info.argumentCount && index < static_cast<int>(arguments.size()); ++index)
        {
            if (!info.arguments[index].mutableArgument)
                continue;
            lua_getfield(m_state, -1, info.arguments[index].name);
            Value changed;
            if (ReadValue(m_state, -1, info.arguments[index], &changed))
                arguments[index] = changed;
            lua_pop(m_state, 1);
        }
        lua_pop(m_state, 1);
    }
    lua_pop(m_state, 2);
    return clean;
}

bool LuaInstance::Command(const std::string& name, const std::string& sender, const std::string& arguments,
                          std::string* reply)
{
    if (m_state == NULL || Failed())
        return false;
    lua_getfield(m_state, LUA_REGISTRYINDEX, kCommandsKey);
    lua_getfield(m_state, -1, name.c_str());
    if (!lua_isfunction(m_state, -1))
    {
        lua_pop(m_state, 2);
        return false;
    }
    lua_pushlstring(m_state, sender.data(), sender.size());
    lua_pushlstring(m_state, arguments.data(), arguments.size());
    std::string problem;
    if (!Call(2, 1, &problem))
    {
        NoteFailure(problem);
        *reply = "The command failed: " + problem;
        lua_pop(m_state, 1);
        return true;
    }
    m_failures = 0;
    if (lua_isstring(m_state, -1))
    {
        size_t length = 0;
        const char* text = lua_tolstring(m_state, -1, &length);
        reply->assign(text, length);
    }
    lua_pop(m_state, 2);
    return true;
}
}
