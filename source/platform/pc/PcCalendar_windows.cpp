#include "platform/pc/PcCalendar.hpp"

std::tm PcLocalTime(std::time_t seconds)
{
    std::tm calendar = {};
    localtime_s(&calendar, &seconds);
    return calendar;
}

std::tm PcUtcTime(std::time_t seconds)
{
    std::tm calendar = {};
    gmtime_s(&calendar, &seconds);
    return calendar;
}
