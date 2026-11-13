#include "game/core/InfoBarManager.hpp"

#if LCE_DEBUG_BUILD
#include "game/core/System.hpp"
#include "platform/DebugGpuTiming.hpp"

namespace
{
const float kAverageWeight = 0.05f;

uint64_t sStarted[InfoBarManager::eTimingCagegory_Count];
uint64_t sThisFrame[InfoBarManager::eTimingCagegory_Count];
float sAverage[InfoBarManager::eTimingCagegory_Count];
}
#endif

void InfoBarManager::StartTimingCategory(eTimingCagegory eCategory)
{
#if LCE_DEBUG_BUILD
    sStarted[eCategory] = System::processTimeInNanoSecs();
    DebugGpuTiming::BeginSection(eCategory);
#endif
}

void InfoBarManager::EndTimingCategory(eTimingCagegory eCategory)
{
#if LCE_DEBUG_BUILD
    sThisFrame[eCategory] += System::processTimeInNanoSecs() - sStarted[eCategory];
    DebugGpuTiming::EndSection(eCategory);
    if (eCategory != eTimingCagegory_Frame)
        return;
    DebugGpuTiming::EndFrame();
    for (int category = 0; category < eTimingCagegory_Count; ++category)
    {
        const float milliseconds = static_cast<float>(sThisFrame[category]) / 1000000.0f;
        sAverage[category] = sAverage[category] * (1.0f - kAverageWeight) + milliseconds * kAverageWeight;
        sThisFrame[category] = 0;
    }
#endif
}

void InfoBarManager::SetCommandBufferSize(float fSize)
{
}

void InfoBarManager::Render(float fWidth, float fHeight)
{
}

#if LCE_DEBUG_BUILD
float InfoBarManager::AverageMilliseconds(eTimingCagegory eCategory)
{
    return sAverage[eCategory];
}
#endif
