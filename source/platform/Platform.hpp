#ifndef PLATFORM_PLATFORM_HPP
#define PLATFORM_PLATFORM_HPP

#if !defined(LCE_PLATFORM_WIIU) && !defined(LCE_PLATFORM_PC)
#if defined(__WIIU__)
#define LCE_PLATFORM_WIIU 1
#else
#error "The build defines no target platform; it must define LCE_PLATFORM_PC or LCE_PLATFORM_WIIU"
#endif
#endif

#if defined(LCE_PLATFORM_WIIU) && defined(LCE_PLATFORM_PC)
#error "More than one target platform"
#endif

#endif
