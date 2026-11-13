#include "platform/pc/PcCrashReport.hpp"

#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include "platform/pc/Log.hpp"
#include "platform/pc/PcCalendar.hpp"
#include "platform/pc/PcCrashPlatform.hpp"
#include "platform/pc/PcFileSystem.hpp"

namespace fs = std::filesystem;

namespace
{
const Logger sLog("Crash");

struct Section
{
    std::string title;
    PcCrashReport::SectionWriter writer;
};

std::vector<Section>& Sections()
{
    static std::vector<Section> sections;
    return sections;
}

fs::path& ReportRoot()
{
    static fs::path root = fs::absolute(fs::path("crash-reports"));
    return root;
}

std::atomic<bool> sWriting(false);

std::string Timestamp(const char* format)
{
    const std::tm local = PcLocalTime(std::time(nullptr));
    char text[64];
    std::strftime(text, sizeof(text), format, &local);
    return text;
}

bool WriteFile(const fs::path& path, const std::string& text)
{
    std::FILE* file = PcOpenFile(path, "wb");
    if (file == nullptr)
        return false;
    const bool written = std::fwrite(text.data(), 1, text.size(), file) == text.size();
    return std::fclose(file) == 0 && written;
}
}

std::string PcCrashFormat(const char* format, ...)
{
    char buffer[1024];
    va_list arguments;
    va_start(arguments, format);
    std::vsnprintf(buffer, sizeof(buffer), format, arguments);
    va_end(arguments);
    return buffer;
}

std::string PcCrashWriteReport(const char* reason, const void* context)
{
    bool expected = false;
    if (!sWriting.compare_exchange_strong(expected, true))
        return std::string();
    Log::Flush();

    std::error_code error;
    fs::path folder = ReportRoot() / ("crash-" + Timestamp("%Y-%m-%d_%H-%M-%S"));
    fs::create_directories(folder, error);
    if (error)
    {
        LOG_ERROR(sLog, "The crash report folder %s could not be created: %s", folder.u8string().c_str(),
                  error.message().c_str());
        return std::string();
    }

    std::string report = "LCEReborn crash report\n";
    report += "Time: " + Timestamp("%Y-%m-%d %H:%M:%S") + "\n";
    report += std::string("Reason: ") + reason + "\n";
    const PcCrashThread thread = PcCrashCurrentThread();
    report += PcCrashFormat("Thread: %s (%llu)\n", thread.name.empty() ? "(unnamed)" : thread.name.c_str(), thread.id);

    for (const Section& section : Sections())
    {
        report += "\n-- " + section.title + " --\n";
        section.writer(report);
    }

    report += "\n-- Stack of the failing thread --\n";
    PcCrashAppendStack(report, context);
    report += "\n-- Threads --\n";
    PcCrashAppendThreads(report, thread.id);
    PcCrashWriteExtras(folder, context, report);

    if (!WriteFile(folder / "report.txt", report))
        LOG_ERROR(sLog, "%s could not be written", (folder / "report.txt").u8string().c_str());
    LOG_FATAL(sLog, "Crash report written to %s", folder.u8string().c_str());
    return folder.u8string();
}

void PcCrashReport::Install(const std::string& logDirectory)
{
    ReportRoot() = fs::absolute(fs::u8path(logDirectory)).parent_path() / "crash-reports";
    PcCrashInstallHandlers();
}

void PcCrashReport::AddSection(const char* title, SectionWriter writer)
{
    Sections().push_back(Section{title, writer});
}

void PcCrashReport::NameCurrentThread(const char* rawName)
{
    std::string trimmed(rawName);
    while (!trimmed.empty() && (trimmed.back() == '\n' || trimmed.back() == '\r' || trimmed.back() == ' '))
        trimmed.pop_back();
    PcCrashSetThreadName(trimmed);
}

std::string PcCrashReport::Write(const char* reason)
{
    return PcCrashWriteReport(reason, nullptr);
}
