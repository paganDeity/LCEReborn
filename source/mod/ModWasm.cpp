#include "mod/ModWasm.hpp"

#include <string.h>

#include "mod/ModApi.hpp"
#include "archive/ArchiveFiles.hpp"

extern "C"
{
#include "m3_env.h"
#include "wasm3.h"
}

namespace
{
// a call into the module runs with the budget the mod has, counted in passes of its loops, and traps when it is spent
template <typename... Arguments> M3Result CallBounded(int64_t budget, IM3Function function, Arguments... arguments)
{
    m3_fuel = budget;
    const M3Result result = m3_CallV(function, arguments...);
    m3_fuel = -1;
    return result;
}
}

namespace mod
{
namespace
{
WasmInstance* Owner(IM3Runtime runtime)
{
    return static_cast<WasmInstance*>(m3_GetUserData(runtime));
}

bool Text(IM3Runtime runtime, uint32_t pointer, uint32_t length, std::string* text)
{
    uint32_t size = 0;
    const uint8_t* memory = m3_GetMemory(runtime, &size, 0);
    if (memory == NULL || length > 65536 || static_cast<uint64_t>(pointer) + length > size)
        return false;
    text->assign(reinterpret_cast<const char*>(memory) + pointer, length);
    return true;
}

m3ApiRawFunction(HostLog)
{
    m3ApiGetArg(uint32_t, pointer);
    m3ApiGetArg(uint32_t, length);
    std::string text;
    if (Text(runtime, pointer, length, &text))
        Owner(runtime)->GetServices()->Log(Owner(runtime)->Id(), text);
    return NULL;
}

m3ApiRawFunction(HostSay)
{
    m3ApiGetArg(uint32_t, pointer);
    m3ApiGetArg(uint32_t, length);
    std::string text;
    if (Text(runtime, pointer, length, &text))
        Owner(runtime)->GetServices()->Say(text);
    return NULL;
}

m3ApiRawFunction(HostRun)
{
    m3ApiReturnType(int32_t);
    m3ApiGetArg(uint32_t, pointer);
    m3ApiGetArg(uint32_t, length);
    std::string line, reply;
    int32_t ok = 0;
    if (Text(runtime, pointer, length, &line))
        ok = Owner(runtime)->GetServices()->Run(line, &reply) ? 1 : 0;
    m3ApiReturn(ok);
}

m3ApiRawFunction(HostReply)
{
    m3ApiGetArg(uint32_t, pointer);
    m3ApiGetArg(uint32_t, length);
    std::string text;
    if (Text(runtime, pointer, length, &text))
        Owner(runtime)->AppendReply(text);
    return NULL;
}

m3ApiRawFunction(HostCommand)
{
    m3ApiGetArg(uint32_t, pointer);
    m3ApiGetArg(uint32_t, length);
    std::string name;
    if (Text(runtime, pointer, length, &name) && !name.empty() && name.size() <= 32)
        Owner(runtime)->AddCommand(name);
    return NULL;
}

m3ApiRawFunction(HostCall)
{
    m3ApiReturnType(int32_t);
    m3ApiGetArg(int32_t, function);
    m3ApiGetArg(uint32_t, inPointer);
    m3ApiGetArg(uint32_t, inLength);
    m3ApiGetArg(uint32_t, outPointer);
    m3ApiGetArg(uint32_t, outCapacity);
    uint32_t size = 0;
    uint8_t* memory = m3_GetMemory(runtime, &size, 0);
    int32_t written = -1;
    std::vector<Value> arguments;
    if (memory != NULL && static_cast<uint64_t>(inPointer) + inLength <= size && static_cast<uint64_t>(outPointer) + outCapacity <= size
        && function >= 0 && function < api::eFunction_Count && Value::Decode(memory + inPointer, inLength, arguments))
    {
        Value result;
        if (Owner(runtime)->GetServices()->Call(Owner(runtime)->Id(), function, arguments, &result))
        {
            std::vector<uint8_t> encoded;
            Value::Encode(std::vector<Value>(1, result), encoded);
            if (encoded.size() <= outCapacity)
            {
                memcpy(memory + outPointer, &encoded[0], encoded.size());
                written = static_cast<int32_t>(encoded.size());
            }
        }
    }
    m3ApiReturn(written);
}

bool Slice(IM3Runtime runtime, uint32_t pointer, uint32_t length, uint8_t** out)
{
    uint32_t size = 0;
    uint8_t* memory = m3_GetMemory(runtime, &size, 0);
    if (memory == NULL || static_cast<uint64_t>(pointer) + length > size)
        return false;
    *out = memory + pointer;
    return true;
}

int32_t CopyOut(IM3Runtime runtime, const std::string& data, uint32_t pointer, uint32_t capacity)
{
    uint8_t* target;
    if (data.size() > capacity || !Slice(runtime, pointer, capacity, &target))
        return -1;
    if (!data.empty())
        memcpy(target, data.data(), data.size());
    return static_cast<int32_t>(data.size());
}

m3ApiRawFunction(HostFileRead)
{
    m3ApiReturnType(int32_t);
    m3ApiGetArg(uint32_t, namePointer);
    m3ApiGetArg(uint32_t, nameLength);
    m3ApiGetArg(uint32_t, outPointer);
    m3ApiGetArg(uint32_t, outCapacity);
    std::string name, data;
    int32_t result = -1;
    if (Text(runtime, namePointer, nameLength, &name) && Owner(runtime)->Data().Read(name, data))
        result = CopyOut(runtime, data, outPointer, outCapacity);
    m3ApiReturn(result);
}

m3ApiRawFunction(HostGameRead)
{
    m3ApiReturnType(int32_t);
    m3ApiGetArg(uint32_t, namePointer);
    m3ApiGetArg(uint32_t, nameLength);
    m3ApiGetArg(uint32_t, outPointer);
    m3ApiGetArg(uint32_t, outCapacity);
    std::string name, data;
    int32_t result = -1;
    if (Text(runtime, namePointer, nameLength, &name) && DataFiles::Safe(name)
        && Owner(runtime)->GetServices()->ReadGameFile(name, &data))
        result = CopyOut(runtime, data, outPointer, outCapacity);
    m3ApiReturn(result);
}

m3ApiRawFunction(HostModRead)
{
    m3ApiReturnType(int32_t);
    m3ApiGetArg(uint32_t, namePointer);
    m3ApiGetArg(uint32_t, nameLength);
    m3ApiGetArg(uint32_t, outPointer);
    m3ApiGetArg(uint32_t, outCapacity);
    std::string name, data, problem;
    int32_t result = -1;
    if (Text(runtime, namePointer, nameLength, &name) && DataFiles::Safe(name)
        && Owner(runtime)->Source()->Read(name, data, &problem))
        result = CopyOut(runtime, data, outPointer, outCapacity);
    m3ApiReturn(result);
}

m3ApiRawFunction(HostFileWrite)
{
    m3ApiReturnType(int32_t);
    m3ApiGetArg(uint32_t, namePointer);
    m3ApiGetArg(uint32_t, nameLength);
    m3ApiGetArg(uint32_t, dataPointer);
    m3ApiGetArg(uint32_t, dataLength);
    std::string name, data;
    uint8_t* source;
    int32_t result = 0;
    if (Text(runtime, namePointer, nameLength, &name) && Slice(runtime, dataPointer, dataLength, &source))
    {
        data.assign(reinterpret_cast<const char*>(source), dataLength);
        result = Owner(runtime)->Data().Write(name, data) ? 1 : 0;
    }
    m3ApiReturn(result);
}

m3ApiRawFunction(HostFileExists)
{
    m3ApiReturnType(int32_t);
    m3ApiGetArg(uint32_t, namePointer);
    m3ApiGetArg(uint32_t, nameLength);
    std::string name;
    m3ApiReturn(Text(runtime, namePointer, nameLength, &name) && Owner(runtime)->Data().Exists(name) ? 1 : 0);
}

m3ApiRawFunction(HostHookSet)
{
    m3ApiGetArg(int32_t, index);
    m3ApiGetArg(uint32_t, pointer);
    m3ApiGetArg(uint32_t, length);
    uint32_t size = 0;
    uint8_t* memory = m3_GetMemory(runtime, &size, 0);
    std::vector<Value> decoded;
    if (memory != NULL && static_cast<uint64_t>(pointer) + length <= size && Value::Decode(memory + pointer, length, decoded)
        && decoded.size() == 1)
        Owner(runtime)->SetArgument(index, decoded[0]);
    return NULL;
}

m3ApiRawFunction(HostHookRegister)
{
    m3ApiGetArg(int32_t, hook);
    m3ApiGetArg(int32_t, priority);
    if (hook >= 0 && hook < api::eHook_Count)
        Owner(runtime)->AddHook(hook, priority);
    return NULL;
}

m3ApiRawFunction(HostListen)
{
    m3ApiGetArg(uint32_t, pointer);
    m3ApiGetArg(uint32_t, length);
    std::string name;
    if (Text(runtime, pointer, length, &name) && !name.empty())
        Owner(runtime)->AddListener(name);
    return NULL;
}

m3ApiRawFunction(HostFire)
{
    m3ApiReturnType(int32_t);
    m3ApiGetArg(uint32_t, namePointer);
    m3ApiGetArg(uint32_t, nameLength);
    m3ApiGetArg(uint32_t, argumentPointer);
    m3ApiGetArg(uint32_t, argumentLength);
    std::string name;
    uint32_t size = 0;
    uint8_t* memory = m3_GetMemory(runtime, &size, 0);
    std::vector<Value> arguments;
    int32_t allowed = 1;
    if (Text(runtime, namePointer, nameLength, &name) && memory != NULL
        && static_cast<uint64_t>(argumentPointer) + argumentLength <= size
        && Value::Decode(memory + argumentPointer, argumentLength, arguments) && Owner(runtime)->Bus() != NULL)
        allowed = Owner(runtime)->Bus()->Fire(name, arguments, Owner(runtime)) ? 1 : 0;
    m3ApiReturn(allowed);
}
}

void WasmInstance::AddHook(int hook, int priority)
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

WasmInstance::WasmInstance(const Context& context, const Manifest& manifest)
    : m_context(context), m_manifest(manifest), m_customEntry(NULL), m_environment(NULL), m_runtime(NULL), m_command(NULL),
      m_allocate(NULL), m_hookEntry(NULL), m_failures(0)
{
    uint64_t limit = m_context.limits.memoryBytes;
    if (manifest.memoryMegabytes > 0)
        limit = static_cast<uint64_t>(manifest.memoryMegabytes) * 1024 * 1024;
    if (limit > m_context.limits.maxMemoryBytes)
        limit = m_context.limits.maxMemoryBytes;
    if (limit > 0xFFFFFFFFull)
        limit = 0xFFFFFFFFull;
    m_memoryLimit = static_cast<uint32_t>(limit);
}

int WasmInstance::HookPriority(int hook) const
{
    for (size_t index = 0; index < m_hooks.size(); ++index)
        if (m_hooks[index] == hook)
            return m_hookPriorities[index];
    return 0;
}

void WasmInstance::AddListener(const std::string& name)
{
    for (size_t index = 0; index < m_listens.size(); ++index)
        if (m_listens[index] == name)
            return;
    m_listens.push_back(name);
}

bool WasmInstance::Custom(const std::string& name, std::vector<Value>& arguments, bool* cancel)
{
    if (m_runtime == NULL || Failed() || m_customEntry == NULL || m_allocate == NULL)
        return false;
    std::vector<uint8_t> blob;
    Value::Encode(arguments, blob);
    const std::string text = name;
    int32_t pointer = 0;
    M3Result result = CallBounded(Budget(), m_allocate, static_cast<int32_t>(text.size() + blob.size() + 2));
    if (result == NULL)
        result = m3_GetResultsV(m_allocate, &pointer);
    uint32_t size = 0;
    uint8_t* memory = m3_GetMemory(m_runtime, &size, 0);
    if (result != NULL || memory == NULL || pointer < 0 || static_cast<uint64_t>(pointer) + text.size() + blob.size() > size)
    {
        ++m_failures;
        return false;
    }
    memcpy(memory + pointer, text.data(), text.size());
    memcpy(memory + pointer + text.size(), &blob[0], blob.size());
    int32_t outcome = 0;
    result = CallBounded(Budget(), m_customEntry, pointer, static_cast<int32_t>(text.size()), static_cast<int32_t>(pointer + text.size()),
                      static_cast<int32_t>(blob.size()));
    if (result == NULL)
        result = m3_GetResultsV(m_customEntry, &outcome);
    if (result != NULL)
    {
        ++m_failures;
        m_error = std::string("lce_custom failed: ") + result;
        m_context.services->Log(m_manifest.id, "error: " + m_error);
        return false;
    }
    m_failures = 0;
    if (outcome == 1)
        *cancel = true;
    return true;
}

WasmInstance::~WasmInstance()
{
    if (m_runtime != NULL)
        m3_FreeRuntime(m_runtime);
    if (m_environment != NULL)
        m3_FreeEnvironment(m_environment);
}

int64_t WasmInstance::Budget() const
{
    int64_t budget = m_context.limits.instructionBudget;
    if (m_manifest.budgetMillions > 0)
        budget = static_cast<int64_t>(m_manifest.budgetMillions) * 1000000;
    return budget > m_context.limits.maxInstructionBudget ? m_context.limits.maxInstructionBudget : budget;
}

bool WasmInstance::Start(std::string* error)
{
    std::string problem;
    if (!m_context.source->Read(m_manifest.entry, m_binary, &problem) || m_binary.empty())
    {
        *error = "the entry file can't be read";
        return false;
    }
    m_environment = m3_NewEnvironment();
    if (m_environment == NULL)
    {
        *error = "not enough memory to start";
        return false;
    }
    m_runtime = m3_NewRuntime(m_environment, m_context.limits.wasmStackBytes, this);
    if (m_runtime == NULL)
    {
        *error = "not enough memory to start";
        return false;
    }
    m_runtime->memoryLimit = m_memoryLimit;
    IM3Module module = NULL;
    M3Result result = m3_ParseModule(m_environment, &module, reinterpret_cast<const uint8_t*>(m_binary.data()), static_cast<uint32_t>(m_binary.size()));
    if (result != NULL)
    {
        *error = std::string("the module does not parse: ") + result;
        return false;
    }
    result = m3_LoadModule(m_runtime, module);
    if (result != NULL)
    {
        m3_FreeModule(module);
        *error = std::string("the module does not load: ") + result;
        return false;
    }
    struct Import
    {
        const char* name;
        const char* signature;
        M3RawCall function;
    };
    const Import imports[] = {{"log", "v(ii)", HostLog}, {"say", "v(ii)", HostSay},         {"run", "i(ii)", HostRun},
                              {"reply", "v(ii)", HostReply}, {"command", "v(ii)", HostCommand},
                              {"call", "i(iiiii)", HostCall},    {"hook_set", "v(iii)", HostHookSet}, {"hook_register", "v(ii)", HostHookRegister},
                              {"listen", "v(ii)", HostListen},   {"fire", "i(iiii)", HostFire},
                              {"file_read", "i(iiii)", HostFileRead}, {"file_write", "i(iiii)", HostFileWrite},
                              {"file_exists", "i(ii)", HostFileExists}, {"game_read", "i(iiii)", HostGameRead},
                              {"mod_read", "i(iiii)", HostModRead}};
    for (size_t index = 0; index < sizeof(imports) / sizeof(imports[0]); ++index)
    {
        result = m3_LinkRawFunction(module, "lce", imports[index].name, imports[index].signature, imports[index].function);
        if (result != NULL && result != m3Err_functionLookupFailed)
        {
            *error = std::string("an import could not be linked: ") + result;
            return false;
        }
    }
    uint32_t memorySize = 0;
    if (m3_GetMemory(m_runtime, &memorySize, 0) != NULL && memorySize > m_memoryLimit)
    {
        *error = "the module asks for more memory than a mod may have";
        return false;
    }
    m3_FindFunction(&m_command, m_runtime, "lce_command");
    m3_FindFunction(&m_allocate, m_runtime, "lce_alloc");
    m3_FindFunction(&m_hookEntry, m_runtime, "lce_hook");
    m3_FindFunction(&m_customEntry, m_runtime, "lce_custom");
    IM3Function init = NULL;
    if (m3_FindFunction(&init, m_runtime, "lce_init") == NULL)
    {
        result = CallBounded(Budget(), init);
        if (result != NULL)
        {
            *error = std::string("lce_init failed: ") + result;
            return false;
        }
    }
    return true;
}

bool WasmInstance::Deliver(const std::string& blob, int32_t* outcome)
{
    *outcome = 0;
    if (m_command == NULL || m_allocate == NULL)
        return true;
    int32_t pointer = 0;
    M3Result result = CallBounded(Budget(), m_allocate, static_cast<int32_t>(blob.size() + 1));
    if (result == NULL)
        result = m3_GetResultsV(m_allocate, &pointer);
    uint32_t size = 0;
    uint8_t* memory = m3_GetMemory(m_runtime, &size, 0);
    if (result != NULL || memory == NULL || pointer < 0 || static_cast<uint64_t>(pointer) + blob.size() + 1 > size)
    {
        m_error = result != NULL ? std::string("lce_alloc failed: ") + result : "lce_alloc returned a bad address";
        ++m_failures;
        m_context.services->Log(m_manifest.id, "error: " + m_error);
        return false;
    }
    memcpy(memory + pointer, blob.data(), blob.size());
    memory[pointer + blob.size()] = 0;
    result = CallBounded(Budget(), m_command, pointer, static_cast<int32_t>(blob.size()));
    if (result == NULL)
        result = m3_GetResultsV(m_command, outcome);
    if (result != NULL)
    {
        m_error = std::string("lce_command failed: ") + result;
        ++m_failures;
        m_context.services->Log(m_manifest.id, "error: " + m_error);
        return false;
    }
    m_failures = 0;
    return true;
}

bool WasmInstance::Hook(int hook, std::vector<Value>& arguments, bool* cancel)
{
    if (m_runtime == NULL || Failed() || m_hookEntry == NULL || m_allocate == NULL)
        return false;
    std::vector<uint8_t> blob;
    Value::Encode(arguments, blob);
    int32_t pointer = 0;
    M3Result result = CallBounded(Budget(), m_allocate, static_cast<int32_t>(blob.size() + 1));
    if (result == NULL)
        result = m3_GetResultsV(m_allocate, &pointer);
    uint32_t size = 0;
    uint8_t* memory = m3_GetMemory(m_runtime, &size, 0);
    if (result != NULL || memory == NULL || pointer < 0 || static_cast<uint64_t>(pointer) + blob.size() > size)
    {
        ++m_failures;
        m_error = "lce_alloc failed for a hook";
        return false;
    }
    memcpy(memory + pointer, &blob[0], blob.size());
    m_changes.clear();
    int32_t outcome = 0;
    result = CallBounded(Budget(), m_hookEntry, static_cast<int32_t>(hook), pointer, static_cast<int32_t>(blob.size()));
    if (result == NULL)
        result = m3_GetResultsV(m_hookEntry, &outcome);
    if (result != NULL)
    {
        ++m_failures;
        m_error = std::string("lce_hook failed: ") + result;
        m_context.services->Log(m_manifest.id, "error: " + m_error);
        return false;
    }
    m_failures = 0;
    const api::HookInfo& info = api::kHooks[hook];
    if (info.cancel && outcome == 1)
        *cancel = true;
    for (size_t index = 0; index < m_changes.size(); ++index)
    {
        const int at = m_changes[index].first;
        if (at >= 0 && at < info.argumentCount && at < static_cast<int>(arguments.size()) && info.arguments[at].mutableArgument)
            arguments[at] = m_changes[index].second;
    }
    return true;
}

bool WasmInstance::Command(const std::string& name, const std::string& sender, const std::string& arguments,
                           std::string* reply)
{
    if (m_runtime == NULL || Failed())
        return false;
    bool known = false;
    for (size_t index = 0; index < m_commands.size(); ++index)
        known = known || m_commands[index].name == name;
    if (!known)
        return false;
    m_reply.clear();
    int32_t outcome = 0;
    if (!Deliver(name + "\n" + sender + "\n" + arguments, &outcome))
    {
        *reply = "The command failed: " + m_error;
        return true;
    }
    *reply = m_reply;
    return true;
}
}
