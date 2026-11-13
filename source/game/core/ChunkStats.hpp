#ifndef GAME_CORE_CHUNKSTATS_HPP
#define GAME_CORE_CHUNKSTATS_HPP

#if LCE_DEBUG_BUILD

#include <stdint.h>

class ChunkStats
{
public:
    enum EStage
    {
        eStage_Serialise,
        eStage_RegionCompress,
        eStage_RegionRead,
        eStage_RegionDecompress,
        eStage_Deserialise,
        eStage_ContainerEncode,
        eStage_ContainerFlush,
        eStage_Count
    };

    struct Entry
    {
        uint64_t count;
        uint64_t nanoseconds;
        uint64_t bytesIn;
        uint64_t bytesOut;
        uint64_t slowestNanoseconds;
    };

    // what the chunks a server keeps loaded hold in memory, added up while their list is locked
    struct Memory
    {
        Memory() : chunks(0), elementBytes(0), lightBytes(0), fixedBytes(0) {}
        uint64_t chunks;
        uint64_t elementBytes;
        uint64_t lightBytes;
        uint64_t fixedBytes;
    };

    static void Add(EStage stage, uint64_t nanoseconds, uint64_t bytesIn, uint64_t bytesOut);
    static Entry Get(EStage stage);
    static const char* Name(EStage stage);
    static void Reset();
};

class ChunkTimer
{
public:
    explicit ChunkTimer(ChunkStats::EStage stage);
    ~ChunkTimer();
    void Stop();
    void Bytes(uint64_t bytesIn, uint64_t bytesOut)
    {
        m_in = bytesIn;
        m_out = bytesOut;
    }

private:
    ChunkStats::EStage m_stage;
    uint64_t m_start;
    uint64_t m_in;
    uint64_t m_out;
    bool m_running;
};

#define LCE_CHUNK_TIMER(name, stage) ChunkTimer name(stage)
#define LCE_CHUNK_BYTES(name, bytesIn, bytesOut) name.Bytes(bytesIn, bytesOut)
#define LCE_CHUNK_STOP(name) name.Stop()

#else

#define LCE_CHUNK_TIMER(name, stage) ((void)0)
#define LCE_CHUNK_BYTES(name, bytesIn, bytesOut) ((void)0)
#define LCE_CHUNK_STOP(name) ((void)0)

#endif

#endif
