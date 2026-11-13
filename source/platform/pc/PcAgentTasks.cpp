#include "platform/pc/PcAgentTasks.hpp"

#if LCE_DEBUG_BUILD
#include <cwchar>
#include <mutex>

namespace
{
std::mutex sMutex;
std::vector<PcAgentTask> sTasks;
int sNextId = 1;

PcAgentTask* Find(int id)
{
    for (size_t index = 0; index < sTasks.size(); ++index)
    {
        if (sTasks[index].id == id)
            return &sTasks[index];
    }
    return nullptr;
}

std::wstring Resolve(const PcAgentTask& task, const std::wstring& answer)
{
    wchar_t* end = nullptr;
    const long choice = std::wcstol(answer.c_str(), &end, 10);
    if (end != answer.c_str() && *end == L'\0' && choice >= 1 && choice <= static_cast<long>(task.options.size()))
        return task.options[choice - 1];
    return answer;
}
}

int PcAgentTaskAdd(const std::wstring& text, const std::vector<std::wstring>& options)
{
    std::lock_guard<std::mutex> lock(sMutex);
    PcAgentTask task;
    task.id = sNextId++;
    task.text = text;
    task.options = options;
    task.answered = false;
    sTasks.push_back(task);
    return task.id;
}

bool PcAgentTaskGet(int id, PcAgentTask* task)
{
    std::lock_guard<std::mutex> lock(sMutex);
    const PcAgentTask* found = Find(id);
    if (found == nullptr)
        return false;
    *task = *found;
    return true;
}

std::vector<PcAgentTask> PcAgentTaskList()
{
    std::lock_guard<std::mutex> lock(sMutex);
    return sTasks;
}

bool PcAgentTaskAnswer(int id, const std::wstring& answer, std::wstring* error)
{
    std::lock_guard<std::mutex> lock(sMutex);
    PcAgentTask* task = Find(id);
    if (task == nullptr)
    {
        *error = L"There is no request " + std::to_wstring(id) + L".";
        return false;
    }
    if (task->answered)
    {
        *error = L"Request " + std::to_wstring(id) + L" is already answered.";
        return false;
    }
    task->answer = Resolve(*task, answer);
    task->answered = true;
    return true;
}
#endif
