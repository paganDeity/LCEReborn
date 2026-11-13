#include "game/core/ChunkStats.hpp"

#if LCE_DEBUG_BUILD

#include <string.h>

#include "game/core/CriticalSection.hpp"
#include "game/core/System.hpp"

namespace
{
class Table
{
public:
    Table()
    {
        InitializeCriticalSection(&lock);
        memset(entries, 0, sizeof(entries));
    }
    C4JCriticalSection lock;
    ChunkStats::Entry entries[ChunkStats::eStage_Count];
};

Table sTable;
}

void ChunkStats::Add(EStage stage, uint64_t nanoseconds, uint64_t bytesIn, uint64_t bytesOut)
{
    EnterCriticalSection(&sTable.lock);
    Entry& entry = sTable.entries[stage];
    ++entry.count;
    entry.nanoseconds += nanoseconds;
    entry.bytesIn += bytesIn;
    entry.bytesOut += bytesOut;
    if (nanoseconds > entry.slowestNanoseconds)
        entry.slowestNanoseconds = nanoseconds;
    LeaveCriticalSection(&sTable.lock);
}

ChunkStats::Entry ChunkStats::Get(EStage stage)
{
    EnterCriticalSection(&sTable.lock);
    Entry entry = sTable.entries[stage];
    LeaveCriticalSection(&sTable.lock);
    return entry;
}

const char* ChunkStats::Name(EStage stage)
{
    static const char* const names[eStage_Count] = {"serialise",    "regionCompress", "regionRead",
                                                    "regionDecompress", "deserialise", "containerEncode",
                                                    "containerFlush"};
    return names[stage];
}

void ChunkStats::Reset()
{
    EnterCriticalSection(&sTable.lock);
    memset(sTable.entries, 0, sizeof(sTable.entries));
    LeaveCriticalSection(&sTable.lock);
}

ChunkTimer::ChunkTimer(ChunkStats::EStage stage)
    : m_stage(stage), m_start(System::processTimeInNanoSecs()), m_in(0), m_out(0), m_running(true)
{
}

ChunkTimer::~ChunkTimer()
{
    Stop();
}

void ChunkTimer::Stop()
{
    if (!m_running)
        return;
    m_running = false;
    ChunkStats::Add(m_stage, System::processTimeInNanoSecs() - m_start, m_in, m_out);
}

#endif
