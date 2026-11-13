#ifndef PLATFORM_PC_PCAGENTTASKS_HPP
#define PLATFORM_PC_PCAGENTTASKS_HPP

#if LCE_DEBUG_BUILD
#include <string>
#include <vector>

struct PcAgentTask
{
    int id;
    std::wstring text;
    std::vector<std::wstring> options;
    bool answered;
    std::wstring answer;
};

int PcAgentTaskAdd(const std::wstring& text, const std::vector<std::wstring>& options);
bool PcAgentTaskGet(int id, PcAgentTask* task);
std::vector<PcAgentTask> PcAgentTaskList();
bool PcAgentTaskAnswer(int id, const std::wstring& answer, std::wstring* error);
#endif

#endif
