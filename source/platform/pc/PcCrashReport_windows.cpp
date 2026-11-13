#include "platform/pc/PcCrashPlatform.hpp"

#include <windows.h>
#include <dbghelp.h>
#include <tlhelp32.h>

#include "platform/pc/Log.hpp"
#include "platform/pc/PcUtf8.hpp"

namespace fs = std::filesystem;

namespace
{
const Logger sLog("Crash");
const int kMaxFrames = 64;

typedef HRESULT(WINAPI* SetThreadDescriptionFn)(HANDLE, PCWSTR);
typedef HRESULT(WINAPI* GetThreadDescriptionFn)(HANDLE, PWSTR*);

std::string ThreadName(HANDLE thread)
{
    static GetThreadDescriptionFn getDescription = reinterpret_cast<GetThreadDescriptionFn>(
        GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetThreadDescription"));
    if (getDescription == nullptr || thread == nullptr)
        return std::string();
    PWSTR description = nullptr;
    if (FAILED(getDescription(thread, &description)))
        return std::string();
    std::string name = description != nullptr ? PcWideToUtf8(description) : std::string();
    LocalFree(description);
    return name;
}

std::string DescribeAddress(HANDLE process, DWORD64 address)
{
    std::string module = "?";
    HMODULE handle = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(address), &handle))
    {
        wchar_t path[MAX_PATH];
        if (GetModuleFileNameW(handle, path, MAX_PATH) != 0)
            module = fs::path(path).filename().u8string();
    }

    std::string text = PcCrashFormat("%-20s 0x%016llx", module.c_str(), static_cast<unsigned long long>(address));
    alignas(SYMBOL_INFO) char storage[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
    SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(storage);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;
    DWORD64 displacement = 0;
    if (SymFromAddr(process, address, &displacement, symbol))
    {
        text += PcCrashFormat("  %s + 0x%llx", symbol->Name, static_cast<unsigned long long>(displacement));
        IMAGEHLP_LINE64 line = {};
        line.SizeOfStruct = sizeof(line);
        DWORD lineDisplacement = 0;
        if (SymGetLineFromAddr64(process, address, &lineDisplacement, &line))
            text += PcCrashFormat("  (%s:%lu)", fs::path(line.FileName).filename().u8string().c_str(),
                                  static_cast<unsigned long>(line.LineNumber));
    }
    return text;
}

LONG WINAPI UnhandledException(EXCEPTION_POINTERS* exception)
{
    const DWORD code = exception->ExceptionRecord->ExceptionCode;
    const std::string reason = PcCrashFormat("Unhandled exception 0x%08lX at %p", static_cast<unsigned long>(code),
                                             exception->ExceptionRecord->ExceptionAddress);
    LOG_FATAL(sLog, "%s", reason.c_str());
    PcCrashWriteReport(reason.c_str(), exception);
    Log::Stop();
    return EXCEPTION_CONTINUE_SEARCH;
}
}

PcCrashThread PcCrashCurrentThread()
{
    PcCrashThread thread;
    thread.id = GetCurrentThreadId();
    HANDLE current = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, GetCurrentThreadId());
    thread.name = ThreadName(current);
    if (current != nullptr)
        CloseHandle(current);
    return thread;
}

void PcCrashAppendStack(std::string& out, const void* context)
{
    HANDLE process = GetCurrentProcess();
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
    const bool symbols = SymInitialize(process, nullptr, TRUE) != FALSE;

    std::vector<unsigned long long> frames;
    if (context == nullptr || !PcCrashWalkStack(context, &frames) || frames.empty())
    {
        frames.clear();
        void* addresses[kMaxFrames];
        const USHORT captured = CaptureStackBackTrace(0, kMaxFrames, addresses, nullptr);
        for (USHORT index = 0; index < captured; ++index)
            frames.push_back(reinterpret_cast<DWORD64>(addresses[index]));
    }

    for (size_t index = 0; index < frames.size(); ++index)
        out += PcCrashFormat("%3d  ", static_cast<int>(index)) + DescribeAddress(process, frames[index]) + "\n";
    if (!symbols)
        out += "Symbols could not be loaded; open minidump.dmp with LCEReborn.pdb for names.\n";
    if (symbols)
        SymCleanup(process);
}

void PcCrashAppendThreads(std::string& out, unsigned long long failingThread)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        out += "The thread list could not be read.\n";
        return;
    }
    const DWORD process = GetCurrentProcessId();
    THREADENTRY32 entry = {};
    entry.dwSize = sizeof(entry);
    int count = 0;
    for (BOOL more = Thread32First(snapshot, &entry); more; more = Thread32Next(snapshot, &entry))
    {
        if (entry.th32OwnerProcessID != process)
            continue;
        HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, entry.th32ThreadID);
        std::string name = ThreadName(thread);
        if (thread != nullptr)
            CloseHandle(thread);
        out += PcCrashFormat("%6lu  %s%s\n", static_cast<unsigned long>(entry.th32ThreadID),
                             name.empty() ? "(unnamed)" : name.c_str(),
                             entry.th32ThreadID == failingThread ? "  <- failed here" : "");
        ++count;
    }
    CloseHandle(snapshot);
    out += PcCrashFormat("%d threads; every thread's stack is in minidump.dmp.\n", count);
}

void PcCrashWriteExtras(const fs::path& folder, const void* context, std::string& report)
{
    EXCEPTION_POINTERS* exception = const_cast<EXCEPTION_POINTERS*>(static_cast<const EXCEPTION_POINTERS*>(context));
    HANDLE file = CreateFileW((folder / "minidump.dmp").c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
    bool written = false;
    if (file != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION exceptionInfo = {};
        exceptionInfo.ThreadId = GetCurrentThreadId();
        exceptionInfo.ExceptionPointers = exception;
        exceptionInfo.ClientPointers = FALSE;
        const MINIDUMP_TYPE type = static_cast<MINIDUMP_TYPE>(
            MiniDumpWithThreadInfo | MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithUnloadedModules);
        written = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, type,
                                    exception != nullptr ? &exceptionInfo : nullptr, nullptr, nullptr)
                  != FALSE;
        CloseHandle(file);
    }
    if (!written)
        report += "\nminidump.dmp could not be written.\n";
}

void PcCrashInstallHandlers()
{
    SetUnhandledExceptionFilter(UnhandledException);
}

void PcCrashSetThreadName(const std::string& name)
{
    static SetThreadDescriptionFn setDescription = reinterpret_cast<SetThreadDescriptionFn>(
        GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "SetThreadDescription"));
    if (setDescription == nullptr)
        return;
    setDescription(GetCurrentThread(), PcUtf8ToWide(name).c_str());
}
