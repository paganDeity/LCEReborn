#ifndef PLATFORM_PC_PCFRAMECAPTURE_HPP
#define PLATFORM_PC_PCFRAMECAPTURE_HPP

#if LCE_DEBUG_BUILD
#include <string>

void PcFrameCaptureTick();
void PcFrameCaptureFrame(int width, int height);
bool PcFrameCaptureWrite(const std::string& path, int width, int height);
#endif

#endif
