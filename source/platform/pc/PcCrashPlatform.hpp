#ifndef PLATFORM_PC_PCCRASHPLATFORM_HPP
#define PLATFORM_PC_PCCRASHPLATFORM_HPP

#include <filesystem>
#include <string>
#include <vector>

struct PcCrashThread
{
    unsigned long long id;
    std::string name;
};

std::string PcCrashFormat(const char* format, ...);
std::string PcCrashWriteReport(const char* reason, const void* context);

PcCrashThread PcCrashCurrentThread();
void PcCrashAppendStack(std::string& out, const void* context);
void PcCrashAppendThreads(std::string& out, unsigned long long failingThread);
void PcCrashWriteExtras(const std::filesystem::path& folder, const void* context, std::string& report);
void PcCrashInstallHandlers();
void PcCrashSetThreadName(const std::string& name);
bool PcCrashWalkStack(const void* context, std::vector<unsigned long long>* frames);

#endif
