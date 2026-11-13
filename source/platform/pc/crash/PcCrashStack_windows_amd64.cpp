#include "platform/pc/PcCrashPlatform.hpp"

#include <windows.h>
#include <dbghelp.h>

static const size_t kMaxFrames = 64;

bool PcCrashWalkStack(const void* context, std::vector<unsigned long long>* frames)
{
    const EXCEPTION_POINTERS* exception = static_cast<const EXCEPTION_POINTERS*>(context);
    HANDLE process = GetCurrentProcess();
    CONTEXT registers = *exception->ContextRecord;
    STACKFRAME64 frame = {};
    frame.AddrPC.Offset = registers.Rip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = registers.Rbp;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = registers.Rsp;
    frame.AddrStack.Mode = AddrModeFlat;
    while (frames->size() < kMaxFrames
           && StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, GetCurrentThread(), &frame, &registers, nullptr,
                          SymFunctionTableAccess64, SymGetModuleBase64, nullptr)
           && frame.AddrPC.Offset != 0)
        frames->push_back(frame.AddrPC.Offset);
    return true;
}
