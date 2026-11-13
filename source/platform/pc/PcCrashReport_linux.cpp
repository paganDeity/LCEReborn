#include "platform/pc/PcCrashPlatform.hpp"

#include <execinfo.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <system_error>

#include "platform/pc/Log.hpp"

namespace fs = std::filesystem;

namespace
{
const Logger sLog("Crash");
const int kMaxFrames = 64;
const int kCrashSignals[] = {SIGSEGV, SIGBUS, SIGFPE, SIGILL, SIGABRT, SIGTRAP};

std::string ReadSmallFile(const fs::path& path)
{
    std::string text;
    std::FILE* file = std::fopen(path.c_str(), "rb");
    if (file == nullptr)
        return text;
    char buffer[256];
    const size_t length = std::fread(buffer, 1, sizeof(buffer), file);
    std::fclose(file);
    text.assign(buffer, length);
    while (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
        text.pop_back();
    return text;
}

void CrashSignal(int signal, siginfo_t* info, void*)
{
    const std::string reason = PcCrashFormat("Signal %d (%s) at %p", signal, strsignal(signal),
                                             info != nullptr ? info->si_addr : nullptr);
    LOG_FATAL(sLog, "%s", reason.c_str());
    PcCrashWriteReport(reason.c_str(), nullptr);
    Log::Stop();
    struct sigaction fallback = {};
    fallback.sa_handler = SIG_DFL;
    sigemptyset(&fallback.sa_mask);
    sigaction(signal, &fallback, nullptr);
    raise(signal);
}
}

PcCrashThread PcCrashCurrentThread()
{
    PcCrashThread thread;
    thread.id = static_cast<unsigned long long>(gettid());
    char name[64] = {};
    if (pthread_getname_np(pthread_self(), name, sizeof(name)) == 0)
        thread.name = name;
    return thread;
}

void PcCrashAppendStack(std::string& out, const void*)
{
    void* addresses[kMaxFrames];
    const int captured = backtrace(addresses, kMaxFrames);
    char** symbols = backtrace_symbols(addresses, captured);
    for (int index = 0; index < captured; ++index)
        out += PcCrashFormat("%3d  %s\n", index, symbols != nullptr ? symbols[index] : "?");
    free(symbols);
}

void PcCrashAppendThreads(std::string& out, unsigned long long failingThread)
{
    std::error_code error;
    fs::directory_iterator tasks("/proc/self/task", error);
    if (error)
    {
        out += "The thread list could not be read.\n";
        return;
    }
    int count = 0;
    for (const fs::directory_entry& task : tasks)
    {
        const std::string id = task.path().filename().string();
        const std::string name = ReadSmallFile(task.path() / "comm");
        out += PcCrashFormat("%6s  %s%s\n", id.c_str(), name.empty() ? "(unnamed)" : name.c_str(),
                             std::strtoull(id.c_str(), nullptr, 10) == failingThread ? "  <- failed here" : "");
        ++count;
    }
    out += PcCrashFormat("%d threads.\n", count);
}

void PcCrashWriteExtras(const fs::path&, const void*, std::string&)
{
}

void PcCrashInstallHandlers()
{
    static std::vector<char> alternateStack(1 << 16);
    stack_t stack = {};
    stack.ss_sp = alternateStack.data();
    stack.ss_size = alternateStack.size();
    sigaltstack(&stack, nullptr);
    struct sigaction action = {};
    action.sa_sigaction = CrashSignal;
    action.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigemptyset(&action.sa_mask);
    for (int signal : kCrashSignals)
        sigaction(signal, &action, nullptr);
}

void PcCrashSetThreadName(const std::string& name)
{
    char shortName[16];
    std::snprintf(shortName, sizeof(shortName), "%s", name.c_str());
    pthread_setname_np(pthread_self(), shortName);
}
