#ifndef PLATFORM_DEBUGGPUTIMING_HPP
#define PLATFORM_DEBUGGPUTIMING_HPP

#if LCE_DEBUG_BUILD
namespace DebugGpuTiming
{
const int kMaxSections = 16;

void BeginSection(int section);
void EndSection(int section);
void EndFrame();
bool AverageMilliseconds(int section, float* milliseconds);
}
#endif

#endif
