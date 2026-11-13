#include "platform/DebugGpuTiming.hpp"

#if LCE_DEBUG_BUILD

#include "platform/pc/GlApi.hpp"

namespace
{
const int kFramesInFlight = 4;
const int kPairsPerSection = 4;
const float kAverageWeight = 0.05f;

struct FrameQueries
{
    GLuint begin[DebugGpuTiming::kMaxSections][kPairsPerSection];
    GLuint end[DebugGpuTiming::kMaxSections][kPairsPerSection];
    int pairs[DebugGpuTiming::kMaxSections];
    bool open[DebugGpuTiming::kMaxSections];
    bool issued;
};

enum EState
{
    EState_Unknown,
    EState_Ready,
    EState_Unavailable
};

EState sState = EState_Unknown;
FrameQueries sFrames[kFramesInFlight];
int sCurrent = 0;
float sAverage[DebugGpuTiming::kMaxSections];
bool sMeasured[DebugGpuTiming::kMaxSections];

bool Ready()
{
    if (sState != EState_Unknown)
        return sState == EState_Ready;
    if (Gl::GenQueries == NULL || Gl::QueryCounter == NULL || Gl::GetQueryObjectiv == NULL
        || Gl::GetQueryObjectui64v == NULL)
    {
        sState = EState_Unavailable;
        return false;
    }
    for (int frame = 0; frame < kFramesInFlight; ++frame)
    {
        Gl::GenQueries(DebugGpuTiming::kMaxSections * kPairsPerSection, &sFrames[frame].begin[0][0]);
        Gl::GenQueries(DebugGpuTiming::kMaxSections * kPairsPerSection, &sFrames[frame].end[0][0]);
    }
    sState = EState_Ready;
    return true;
}

bool ValidSection(int section)
{
    return section >= 0 && section < DebugGpuTiming::kMaxSections;
}

bool Available(const FrameQueries& frame)
{
    for (int section = DebugGpuTiming::kMaxSections - 1; section >= 0; --section)
    {
        if (frame.pairs[section] == 0)
            continue;
        GLint available = 0;
        Gl::GetQueryObjectiv(frame.end[section][frame.pairs[section] - 1], Gl::QUERY_RESULT_AVAILABLE, &available);
        return available != 0;
    }
    return true;
}

void Collect(FrameQueries& frame)
{
    if (frame.issued && Available(frame))
    {
        for (int section = 0; section < DebugGpuTiming::kMaxSections; ++section)
        {
            if (frame.pairs[section] == 0)
                continue;
            GLuint64 total = 0;
            for (int pair = 0; pair < frame.pairs[section]; ++pair)
            {
                GLuint64 begin = 0;
                GLuint64 end = 0;
                Gl::GetQueryObjectui64v(frame.begin[section][pair], Gl::QUERY_RESULT, &begin);
                Gl::GetQueryObjectui64v(frame.end[section][pair], Gl::QUERY_RESULT, &end);
                if (end > begin)
                    total += end - begin;
            }
            const float milliseconds = static_cast<float>(total) / 1000000.0f;
            sAverage[section] = sMeasured[section]
                                    ? sAverage[section] * (1.0f - kAverageWeight) + milliseconds * kAverageWeight
                                    : milliseconds;
            sMeasured[section] = true;
        }
    }
    for (int section = 0; section < DebugGpuTiming::kMaxSections; ++section)
    {
        frame.pairs[section] = 0;
        frame.open[section] = false;
    }
    frame.issued = false;
}
}

void DebugGpuTiming::BeginSection(int section)
{
    if (!ValidSection(section) || !Ready())
        return;
    FrameQueries& frame = sFrames[sCurrent];
    if (frame.open[section] || frame.pairs[section] >= kPairsPerSection)
        return;
    Gl::QueryCounter(frame.begin[section][frame.pairs[section]], Gl::TIMESTAMP);
    frame.open[section] = true;
}

void DebugGpuTiming::EndSection(int section)
{
    if (!ValidSection(section) || !Ready())
        return;
    FrameQueries& frame = sFrames[sCurrent];
    if (!frame.open[section])
        return;
    Gl::QueryCounter(frame.end[section][frame.pairs[section]], Gl::TIMESTAMP);
    frame.open[section] = false;
    ++frame.pairs[section];
}

void DebugGpuTiming::EndFrame()
{
    if (!Ready())
        return;
    sFrames[sCurrent].issued = true;
    sCurrent = (sCurrent + 1) % kFramesInFlight;
    Collect(sFrames[sCurrent]);
}

bool DebugGpuTiming::AverageMilliseconds(int section, float* milliseconds)
{
    if (!ValidSection(section) || !sMeasured[section])
        return false;
    *milliseconds = sAverage[section];
    return true;
}

#endif
