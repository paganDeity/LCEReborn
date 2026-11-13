#ifndef PLATFORM_PC_PCCALENDAR_HPP
#define PLATFORM_PC_PCCALENDAR_HPP

#include <ctime>

std::tm PcLocalTime(std::time_t seconds);
std::tm PcUtcTime(std::time_t seconds);

#endif
