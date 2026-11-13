#include "platform/pc/PcAgentChatCommand.hpp"

#if LCE_DEBUG_BUILD
#include <cwchar>
#include <string>

#include "game/command/CommandArguments.hpp"
#include "game/command/CommandSource.hpp"
#include "platform/pc/PcAgentTasks.hpp"

namespace
{
const wchar_t* const kVoice = L"§7§o";

bool ParseId(const std::wstring& word, int* id)
{
    wchar_t* end = nullptr;
    const long value = std::wcstol(word.c_str(), &end, 10);
    if (end == word.c_str() || *end != L'\0' || value < 1)
        return false;
    *id = static_cast<int>(value);
    return true;
}
}

PcAgentChatCommand::PcAgentChatCommand()
    : ChatCommand(L"agent", L"[answer <request> <answer>|done <request>]", L"text", ePermission_Everyone)
{
}

bool PcAgentChatCommand::run(const CommandSource& source, const Words& words)
{
    const std::wstring action = words.size() > 1 ? CommandArguments::Lower(words[1]) : std::wstring(L"tasks");
    if (action == L"tasks" && words.size() <= 2)
    {
        List(source);
        return true;
    }
    if (action == L"answer" || action == L"done")
        return Answer(source, words);
    int id = 0;
    if (action == L"show" && source.console && words.size() == 3 && ParseId(words[2], &id))
    {
        Show(id);
        return true;
    }
    return false;
}

void PcAgentChatCommand::Show(int id)
{
    PcAgentTask task;
    if (!PcAgentTaskGet(id, &task))
        return;
    const std::wstring number = std::to_wstring(task.id);
    Broadcast(kVoice + std::wstring(L"Agent asks (request ") + number + L"): " + task.text);
    for (size_t index = 0; index < task.options.size(); ++index)
        Broadcast(kVoice + std::wstring(L"  ") + std::to_wstring(index + 1) + L". " + task.options[index]);
    if (task.options.empty())
        Broadcast(kVoice + std::wstring(L"Reply with /agent done ") + number + L" or /agent answer " + number
                  + L" <your answer>");
    else
        Broadcast(kVoice + std::wstring(L"Reply with /agent answer ") + number + L" <number or your own answer>");
}

void PcAgentChatCommand::List(const CommandSource& source)
{
    const std::vector<PcAgentTask> tasks = PcAgentTaskList();
    bool any = false;
    for (size_t index = 0; index < tasks.size(); ++index)
    {
        const PcAgentTask& task = tasks[index];
        if (task.answered)
            continue;
        any = true;
        Tell(source, std::to_wstring(task.id) + L". " + task.text);
        for (size_t option = 0; option < task.options.size(); ++option)
            Tell(source, L"   " + std::to_wstring(option + 1) + L". " + task.options[option]);
    }
    if (!any)
        Tell(source, L"No requests from the agent are waiting.");
}

bool PcAgentChatCommand::Answer(const CommandSource& source, const Words& words)
{
    const bool done = CommandArguments::Lower(words[1]) == L"done";
    int id = 0;
    if (words.size() < (done ? 3u : 4u) || !ParseId(words[2], &id))
        return false;
    const std::wstring answer = done ? std::wstring(L"done") : CommandArguments::Join(words, 3);
    std::wstring error;
    if (!PcAgentTaskAnswer(id, answer, &error))
    {
        Fail(source, error);
        return true;
    }
    Tell(source, L"Answer sent for request " + std::to_wstring(id) + L".");
    return true;
}
#endif
