#ifndef PLATFORM_PC_PCDEBUGSERVER_HPP
#define PLATFORM_PC_PCDEBUGSERVER_HPP

#if LCE_DEBUG_BUILD
void PcDebugStart(unsigned short port, int idleQuitSeconds);
void PcDebugTick();
void PcDebugFrame(int width, int height);
#endif

#endif
