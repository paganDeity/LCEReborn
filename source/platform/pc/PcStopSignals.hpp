#ifndef PLATFORM_PC_PCSTOPSIGNALS_HPP
#define PLATFORM_PC_PCSTOPSIGNALS_HPP

void PcInstallStopSignals();
bool PcStopSignalled();
void PcRequestStop();
void PcInstallConsoleStop();

#endif
