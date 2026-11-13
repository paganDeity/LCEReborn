cmake_minimum_required(VERSION 3.24)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

project(LCEReborn LANGUAGES C CXX)

set(LCE_THIRD_PARTY "${CMAKE_SOURCE_DIR}/build/third_party" CACHE PATH
    "Third-party sources laid out by tools/build/deps.py and tools/build/layout.py")

if(NOT EXISTS "${LCE_THIRD_PARTY}/boost/boost")
  message(FATAL_ERROR "Third-party sources not found in ${LCE_THIRD_PARTY}. Run:\n"
                      "  python tools/build/deps.py build/deps\n"
                      "  python tools/build/layout.py build/deps build/third_party")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LCE_SOURCE "${CMAKE_SOURCE_DIR}/source")

# Settings every unit of the PC build shares.
add_library(lce_common INTERFACE)

target_include_directories(lce_common INTERFACE
  "${LCE_SOURCE}"
  "${LCE_THIRD_PARTY}/curl/include"
  "${LCE_THIRD_PARTY}/stb"
  "${LCE_THIRD_PARTY}/zlib"
  "${LCE_THIRD_PARTY}/libpng-wiiu"
  "${LCE_THIRD_PARTY}/libpng"
  "${LCE_THIRD_PARTY}/rapidxml")
target_include_directories(lce_common SYSTEM INTERFACE "${LCE_THIRD_PARTY}/boost")

# The names of the target operating system and processor. They choose the _os and _arch source files, with the rule
# in tools/build/platform_files.py, which the Wii U build uses too.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(LCE_OS windows)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(LCE_OS linux)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
  set(LCE_OS android)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(LCE_OS darwin)
else()
  message(FATAL_ERROR "No operating system name for the system ${CMAKE_SYSTEM_NAME}")
endif()
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" LCE_PROCESSOR)
if(LCE_PROCESSOR MATCHES "^(x86_64|amd64)$")
  set(LCE_ARCH amd64)
elseif(LCE_PROCESSOR MATCHES "^(aarch64|arm64)$")
  set(LCE_ARCH arm64)
else()
  message(FATAL_ERROR "No architecture name for the processor ${CMAKE_SYSTEM_PROCESSOR}")
endif()
target_compile_definitions(lce_common INTERFACE
  LCE_PLATFORM_PC=1
  LCE_OS="${LCE_OS}"
  LCE_ARCH="${LCE_ARCH}"
  NDEBUG
  BOOST_ENABLE_ASSERT_HANDLER
  BOOST_NO_EXCEPTIONS
  $<$<CONFIG:Debug,RelWithDebInfo>:LCE_DEBUG_BUILD=1>)

if(MSVC)
  target_compile_definitions(lce_common INTERFACE
    _HAS_EXCEPTIONS=0
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
    WIN32_LEAN_AND_MEAN)
  # Debug information for the crash reports' stacks, without changing the optimised code.
  target_compile_options(lce_common INTERFACE /EHs-c- /GR /utf-8 /bigobj /Z7)
else()
  # the game is ported Java, whose integer arithmetic wraps on overflow (hashes, random numbers, seeds); -fwrapv
  # makes that defined here instead of undefined behaviour the optimiser may assume never happens
  target_compile_options(lce_common INTERFACE -fno-exceptions -frtti -fsigned-char -fwrapv)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(lce_common INTERFACE -Wno-c++11-narrowing)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(lce_common INTERFACE -Wno-narrowing)
endif()

# SDL3, for the window, input and audio of the PC layer.
include(FetchContent)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(SDL3
  URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.16/SDL3-3.4.16.tar.gz
  URL_HASH SHA256=7322236cd12090c3eb40b9728be4d49c76f66ad17d04369584d4ecad5cf77c68
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(SDL3)

function(lce_select_platform_files variable)
  set(listing "${CMAKE_BINARY_DIR}/platform-files-${variable}.txt")
  list(JOIN ${variable} "\n" lines)
  file(WRITE "${listing}" "${lines}\n")
  execute_process(
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tools/build/platform_files.py" select --os ${LCE_OS}
            --arch ${LCE_ARCH} --from "${listing}"
    OUTPUT_VARIABLE selected ERROR_VARIABLE problem RESULT_VARIABLE result)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "${problem}")
  endif()
  string(STRIP "${selected}" selected)
  string(REPLACE "\n" ";" selected "${selected}")
  set(${variable} ${selected} PARENT_SCOPE)
endfunction()

# The game core, in the C++ dialect it was written in.
file(GLOB_RECURSE LCE_GAME_SOURCES CONFIGURE_DEPENDS "${LCE_SOURCE}/game/*.cpp")
# The vendor units the game core uses that are not Wii U middleware: its own data structures and stb_truetype.
set(LCE_VENDOR_SOURCES
  vendor/_ConnectionProgressParams.cpp
  vendor/_LoadingInputParams.cpp
  vendor/_MapDataMappings.cpp
  vendor/_MapDataMappings_old.cpp
  vendor/_NetworkGameInitData.cpp
  vendor/_Polygon.cpp
  vendor/_SaveListDetails.cpp
  vendor/_SkinAdjustments.cpp
  vendor/_TutorialPopupInfo.cpp
  vendor/_XPARTY_USER_INFO.cpp
  vendor/_XUSER_SIGNIN_INFO.cpp
  vendor/stb_truetype.cpp)
foreach(unit ${LCE_VENDOR_SOURCES})
  list(APPEND LCE_GAME_SOURCES "${LCE_SOURCE}/${unit}")
endforeach()
lce_select_platform_files(LCE_GAME_SOURCES)
# Source files a mod can intercept (tools/hooks/woven.txt, from `intercept` lines in api/lce.api) are compiled as copies with the hooks woven in
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tools/hooks/woven.txt")
file(STRINGS "${CMAKE_SOURCE_DIR}/tools/hooks/woven.txt" LCE_WOVEN)
foreach(woven ${LCE_WOVEN})
  list(REMOVE_ITEM LCE_GAME_SOURCES "${LCE_SOURCE}/${woven}")
  set(woven_output "${CMAKE_BINARY_DIR}/woven/${woven}")
  add_custom_command(OUTPUT "${woven_output}"
    COMMAND python3 "${CMAKE_SOURCE_DIR}/tools/hooks/hookgen.py" weave "${woven}" "${woven_output}"
    DEPENDS "${LCE_SOURCE}/${woven}" "${CMAKE_SOURCE_DIR}/api/lce.api" "${CMAKE_SOURCE_DIR}/tools/hooks/hookgen.py"
    COMMENT "Weaving ${woven}")
  get_filename_component(woven_directory "${LCE_SOURCE}/${woven}" DIRECTORY)
  set_source_files_properties("${woven_output}" PROPERTIES COMPILE_OPTIONS "-I${woven_directory}")
  list(APPEND LCE_GAME_SOURCES "${woven_output}")
endforeach()
add_library(lce_game OBJECT ${LCE_GAME_SOURCES})
target_link_libraries(lce_game PRIVATE lce_common)
set_target_properties(lce_game PROPERTIES CXX_STANDARD 14)

# The PC platform layer.
file(GLOB_RECURSE LCE_PLATFORM_SOURCES CONFIGURE_DEPENDS "${LCE_SOURCE}/platform/pc/*.cpp")
list(FILTER LCE_PLATFORM_SOURCES EXCLUDE REGEX "/platform/pc/audio/")
# Each program's entry point: the game's Main.cpp and the dedicated server's ServerMain.cpp.
list(FILTER LCE_PLATFORM_SOURCES EXCLUDE REGEX "/platform/pc/(Server)?Main\\.cpp$")
# The LAN and internet play shared with the Wii U build.
file(GLOB LCE_NET_SOURCES CONFIGURE_DEPENDS "${LCE_SOURCE}/net/*.cpp")
list(APPEND LCE_PLATFORM_SOURCES ${LCE_NET_SOURCES})
# Units under platform/wiiu shared with PC: 4J's memory helpers and System (whose clock is switched per platform).
list(APPEND LCE_PLATFORM_SOURCES
  "${LCE_SOURCE}/platform/wiiu/Memory.cpp"
  "${LCE_SOURCE}/platform/wiiu/System.cpp")
lce_select_platform_files(LCE_PLATFORM_SOURCES)
add_library(lce_platform OBJECT ${LCE_PLATFORM_SOURCES})
target_link_libraries(lce_platform PRIVATE lce_common lce_zlib lce_p2p lce_identity lce_archive lce_mod SDL3::SDL3-static)
set_target_properties(lce_platform PROPERTIES CXX_STANDARD 17)
add_custom_target(lce_build_info
  COMMAND "${CMAKE_COMMAND}" -DSOURCE_DIR="${CMAKE_SOURCE_DIR}" -DOUTPUT="${CMAKE_BINARY_DIR}/generated/BuildInfo.hpp"
          -P "${CMAKE_SOURCE_DIR}/launcher/BuildInfo.cmake"
  BYPRODUCTS "${CMAKE_BINARY_DIR}/generated/BuildInfo.hpp")
add_dependencies(lce_platform lce_build_info)
set(LCE_DISCORD_APP_ID "" CACHE STRING "Discord application used for Rich Presence when --discord-app is not given")
target_compile_definitions(lce_platform PRIVATE LCE_DISCORD_APP_ID="${LCE_DISCORD_APP_ID}")
target_include_directories(lce_platform PRIVATE "${CMAKE_BINARY_DIR}/generated")
if(MSVC)
  # Boost 1.53 still uses std::auto_ptr and std::unary_function, which C++17 removes.
  target_compile_definitions(lce_platform PRIVATE _HAS_AUTO_PTR_ETC=1)
endif()

# Audio: the game's sound bank and Bink Audio, played through SDL.
file(GLOB_RECURSE LCE_PC_AUDIO_SOURCES CONFIGURE_DEPENDS "${LCE_SOURCE}/platform/pc/audio/*.cpp")
lce_select_platform_files(LCE_PC_AUDIO_SOURCES)
add_library(lce_audio OBJECT ${LCE_PC_AUDIO_SOURCES})
target_link_libraries(lce_audio PRIVATE lce_common SDL3::SDL3-static)
set_target_properties(lce_audio PROPERTIES CXX_STANDARD 17)

# zlib and libpng, the releases the game was built against (libpng with the game's own png.h and pngwrite.c).
set(LCE_ZLIB_SOURCES adler32.c compress.c crc32.c deflate.c infback.c inffast.c inflate.c inftrees.c trees.c
                     uncompr.c zutil.c)
list(TRANSFORM LCE_ZLIB_SOURCES PREPEND "${LCE_THIRD_PARTY}/zlib/")
add_library(lce_zlib STATIC ${LCE_ZLIB_SOURCES})
target_include_directories(lce_zlib PUBLIC "${LCE_THIRD_PARTY}/zlib")

set(LCE_PNG_SOURCES png.c pngerror.c pngget.c pngmem.c pngpread.c pngread.c pngrio.c pngrtran.c pngrutil.c pngset.c
                    pngtrans.c pngwio.c pngwtran.c pngwutil.c)
list(TRANSFORM LCE_PNG_SOURCES PREPEND "${LCE_THIRD_PARTY}/libpng/")
add_library(lce_png STATIC ${LCE_PNG_SOURCES} "${LCE_THIRD_PARTY}/libpng-wiiu/pngwrite.c")
target_include_directories(lce_png PUBLIC "${LCE_THIRD_PARTY}/libpng-wiiu" "${LCE_THIRD_PARTY}/libpng")
target_link_libraries(lce_png PUBLIC lce_zlib)

set(LCE_MONOCYPHER "${CMAKE_SOURCE_DIR}/tools/build/third_party/monocypher")
add_library(lce_monocypher STATIC "${LCE_MONOCYPHER}/monocypher.c")
target_include_directories(lce_monocypher PUBLIC "${LCE_MONOCYPHER}")

file(GLOB LCE_P2P_SOURCES CONFIGURE_DEPENDS "${LCE_SOURCE}/p2p/*.cpp")
lce_select_platform_files(LCE_P2P_SOURCES)
add_library(lce_p2p STATIC ${LCE_P2P_SOURCES})
target_include_directories(lce_p2p PUBLIC "${LCE_SOURCE}")
target_link_libraries(lce_p2p PUBLIC lce_monocypher lce_common)
set_target_properties(lce_p2p PROPERTIES CXX_STANDARD 98)
target_compile_definitions(lce_p2p PUBLIC BOOST_DISABLE_ASSERTS)

file(GLOB LCE_IDENTITY_SOURCES CONFIGURE_DEPENDS "${LCE_SOURCE}/identity/*.cpp")
lce_select_platform_files(LCE_IDENTITY_SOURCES)
add_library(lce_identity STATIC ${LCE_IDENTITY_SOURCES})
target_include_directories(lce_identity PUBLIC "${LCE_SOURCE}")
target_link_libraries(lce_identity PUBLIC lce_p2p)
set_target_properties(lce_identity PROPERTIES CXX_STANDARD 98)

file(GLOB LCE_ZSTD_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/tools/build/third_party/zstd/lib/common/*.c"
  "${CMAKE_SOURCE_DIR}/tools/build/third_party/zstd/lib/compress/*.c"
  "${CMAKE_SOURCE_DIR}/tools/build/third_party/zstd/lib/decompress/*.c")
add_library(lce_zstd STATIC ${LCE_ZSTD_SOURCES})
target_include_directories(lce_zstd PUBLIC "${CMAKE_SOURCE_DIR}/tools/build/third_party/zstd/lib")
target_compile_definitions(lce_zstd PRIVATE ZSTD_DISABLE_ASM=1 ZSTD_LEGACY_SUPPORT=0)
set_target_properties(lce_zstd PROPERTIES C_STANDARD 99 POSITION_INDEPENDENT_CODE ON)

file(GLOB LCE_LUA_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tools/build/third_party/lua/*.c")
add_library(lce_lua STATIC ${LCE_LUA_SOURCES})
target_include_directories(lce_lua PUBLIC "${CMAKE_SOURCE_DIR}/tools/build/third_party/lua")
target_compile_definitions(lce_lua PRIVATE LUA_COMPAT_5_3=0)
if(NOT WIN32)
  target_compile_definitions(lce_lua PRIVATE LUA_USE_POSIX)
endif()
set_target_properties(lce_lua PROPERTIES C_STANDARD 99 POSITION_INDEPENDENT_CODE ON)

file(GLOB LCE_WASM3_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tools/build/third_party/wasm3/*.c")
add_library(lce_wasm3 STATIC ${LCE_WASM3_SOURCES})
target_include_directories(lce_wasm3 PUBLIC "${CMAKE_SOURCE_DIR}/tools/build/third_party/wasm3")
set_target_properties(lce_wasm3 PROPERTIES C_STANDARD 99 POSITION_INDEPENDENT_CODE ON)

file(GLOB LCE_ARCHIVE_SOURCES CONFIGURE_DEPENDS "${LCE_SOURCE}/archive/*.cpp")
lce_select_platform_files(LCE_ARCHIVE_SOURCES)
add_library(lce_archive STATIC ${LCE_ARCHIVE_SOURCES})
target_include_directories(lce_archive PUBLIC "${LCE_SOURCE}")
target_link_libraries(lce_archive PUBLIC lce_zlib)
set_target_properties(lce_archive PROPERTIES CXX_STANDARD 98 POSITION_INDEPENDENT_CODE ON)

file(GLOB LCE_MOD_SOURCES CONFIGURE_DEPENDS "${LCE_SOURCE}/mod/*.cpp")
lce_select_platform_files(LCE_MOD_SOURCES)
add_library(lce_mod STATIC ${LCE_MOD_SOURCES})
target_include_directories(lce_mod PUBLIC "${LCE_SOURCE}")
target_link_libraries(lce_mod PUBLIC lce_archive lce_lua lce_wasm3)
set_target_properties(lce_mod PROPERTIES CXX_STANDARD 98)

file(GLOB LCE_REBORN_SOURCES CONFIGURE_DEPENDS "${LCE_SOURCE}/reborn/*.cpp")
add_library(lce_reborn STATIC ${LCE_REBORN_SOURCES})
target_include_directories(lce_reborn PUBLIC "${LCE_SOURCE}")
target_link_libraries(lce_reborn PUBLIC lce_zstd)
set_target_properties(lce_reborn PROPERTIES CXX_STANDARD 98)

add_executable(lce_reborn_test "${CMAKE_SOURCE_DIR}/tests/reborn_test.cpp")
target_link_libraries(lce_reborn_test PRIVATE lce_reborn)
set_target_properties(lce_reborn_test PROPERTIES CXX_STANDARD 17)

file(GLOB LCE_SAVECONTAINER_SOURCES CONFIGURE_DEPENDS "${LCE_SOURCE}/savecontainer/*.cpp")
add_library(lce_savecontainer STATIC ${LCE_SAVECONTAINER_SOURCES})
target_include_directories(lce_savecontainer PUBLIC "${LCE_SOURCE}")
target_link_libraries(lce_savecontainer PUBLIC lce_zstd lce_zlib)
set_target_properties(lce_savecontainer PROPERTIES CXX_STANDARD 98)

add_executable(lce_savecontainer_test "${CMAKE_SOURCE_DIR}/tests/savecontainer_test.cpp")
target_link_libraries(lce_savecontainer_test PRIVATE lce_savecontainer lce_zlib)
set_target_properties(lce_savecontainer_test PROPERTIES CXX_STANDARD 17)

add_executable(LCERebornSaveTool "${LCE_SOURCE}/savecontainer_tool/SaveToolMain.cpp")
target_link_libraries(LCERebornSaveTool PRIVATE lce_savecontainer lce_zlib)
set_target_properties(LCERebornSaveTool PROPERTIES CXX_STANDARD 17)

add_executable(lce_p2p_test "${CMAKE_SOURCE_DIR}/tests/p2p_test.cpp" "${CMAKE_SOURCE_DIR}/tests/p2p_crypto_tests.cpp"
  "${CMAKE_SOURCE_DIR}/tests/p2p_rendezvous_tests.cpp" "${CMAKE_SOURCE_DIR}/tests/p2p_stream_tests.cpp" "${CMAKE_SOURCE_DIR}/tests/p2p_node_tests.cpp"
  "${CMAKE_SOURCE_DIR}/tests/p2p_wire_tests.cpp" "${CMAKE_SOURCE_DIR}/tests/identity_tests.cpp")
target_link_libraries(lce_p2p_test PRIVATE lce_p2p lce_identity)
set_target_properties(lce_p2p_test PROPERTIES CXX_STANDARD 17)
enable_testing()
add_test(NAME p2p COMMAND lce_p2p_test)
add_test(NAME savecontainer COMMAND lce_savecontainer_test)

add_executable(lce_mod_test "${CMAKE_SOURCE_DIR}/tests/mod_test.cpp")
target_link_libraries(lce_mod_test PRIVATE lce_mod lce_zlib)
set_target_properties(lce_mod_test PROPERTIES CXX_STANDARD 17)
target_compile_definitions(lce_mod_test PRIVATE LCE_EXAMPLE_MODS="${CMAKE_SOURCE_DIR}/docs/examples/mods")
add_test(NAME mod COMMAND lce_mod_test)
add_test(NAME sdk COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/sdk/new_mod.py --self-test)
add_test(NAME sdk_template COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/sdk/new_mod.py --check-loads $<TARGET_FILE:lce_mod_test>)
add_test(NAME reborn COMMAND lce_reborn_test)
add_executable(lce_access_test "${CMAKE_SOURCE_DIR}/tests/access_test.cpp" "${LCE_SOURCE}/game/server/ServerAccess.cpp"
               "${LCE_SOURCE}/game/data/NbtBudget.cpp" "${LCE_SOURCE}/game/server/ServerWatchdog.cpp")
target_include_directories(lce_access_test PRIVATE "${LCE_SOURCE}")
target_compile_definitions(lce_access_test PRIVATE LCE_PLATFORM_PC=1)
set_target_properties(lce_access_test PROPERTIES CXX_STANDARD 98)
add_test(NAME access COMMAND lce_access_test)
add_executable(lce_backup_test "${CMAKE_SOURCE_DIR}/tests/backup_test.cpp")
target_include_directories(lce_backup_test PRIVATE "${LCE_SOURCE}")
set_target_properties(lce_backup_test PROPERTIES CXX_STANDARD 17)
add_test(NAME backup COMMAND lce_backup_test)
add_executable(lce_capabilities_test "${CMAKE_SOURCE_DIR}/tests/capabilities_test.cpp")
target_include_directories(lce_capabilities_test PRIVATE "${LCE_SOURCE}")
set_target_properties(lce_capabilities_test PROPERTIES CXX_STANDARD 98)
add_test(NAME capabilities COMMAND lce_capabilities_test)
find_package(Python3 COMPONENTS Interpreter)
if(Python3_FOUND)
  add_test(NAME mod_api COMMAND ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/tools/hooks/hookgen.py" check)
endif()

set(LCE_P2P_SERVER_SOURCES
  "${LCE_SOURCE}/p2p_server/P2pServerMain.cpp"
  "${LCE_SOURCE}/net/NetSocket.cpp"
  "${LCE_SOURCE}/net/NetNative_linux.cpp"
  "${LCE_SOURCE}/net/NetNative_windows.cpp"
  "${LCE_SOURCE}/net/NetNative_wiiu.cpp"
  "${LCE_SOURCE}/platform/pc/CriticalSection.cpp")
lce_select_platform_files(LCE_P2P_SERVER_SOURCES)
add_executable(LCERebornP2pServer ${LCE_P2P_SERVER_SOURCES})
target_link_libraries(LCERebornP2pServer PRIVATE lce_common lce_p2p)

set(LCE_RENDEZVOUS_SMOKE_SOURCES
  "${CMAKE_SOURCE_DIR}/tests/rendezvous_smoke.cpp"
  "${LCE_SOURCE}/net/NetSocket.cpp"
  "${LCE_SOURCE}/net/NetNative_linux.cpp"
  "${LCE_SOURCE}/net/NetNative_windows.cpp"
  "${LCE_SOURCE}/net/NetNative_wiiu.cpp"
  "${LCE_SOURCE}/platform/pc/CriticalSection.cpp")
lce_select_platform_files(LCE_RENDEZVOUS_SMOKE_SOURCES)
add_executable(lce_rendezvous_smoke ${LCE_RENDEZVOUS_SMOKE_SOURCES})
target_link_libraries(lce_rendezvous_smoke PRIVATE lce_common lce_p2p)
set_target_properties(lce_rendezvous_smoke PROPERTIES CXX_STANDARD 17)

set(LCE_LEGACY_STREAM_TEST_SOURCES
  "${CMAKE_SOURCE_DIR}/tests/legacy_stream_test.cpp"
  "${LCE_SOURCE}/net/NetSocket.cpp"
  "${LCE_SOURCE}/net/NetFrameCodec.cpp"
  "${LCE_SOURCE}/net/NetNative_linux.cpp"
  "${LCE_SOURCE}/net/NetNative_windows.cpp"
  "${LCE_SOURCE}/net/NetNative_wiiu.cpp"
  "${LCE_SOURCE}/platform/pc/CriticalSection.cpp")
lce_select_platform_files(LCE_LEGACY_STREAM_TEST_SOURCES)
add_executable(lce_legacy_stream_test ${LCE_LEGACY_STREAM_TEST_SOURCES})
target_link_libraries(lce_legacy_stream_test PRIVATE lce_common lce_p2p)
set_target_properties(lce_legacy_stream_test PROPERTIES CXX_STANDARD 17)
add_test(NAME legacy_stream COMMAND lce_legacy_stream_test)

if(MSVC)
  target_compile_definitions(lce_zlib PRIVATE _CRT_SECURE_NO_WARNINGS)
  target_compile_definitions(lce_png PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

function(lce_add_program target main)
  add_executable(${target} "${LCE_SOURCE}/platform/pc/${main}" $<TARGET_OBJECTS:lce_game>
                 $<TARGET_OBJECTS:lce_platform> $<TARGET_OBJECTS:lce_audio>)
  target_link_libraries(${target} PRIVATE lce_common lce_png lce_zlib lce_p2p lce_identity lce_savecontainer lce_mod lce_reborn SDL3::SDL3-static)
  set_target_properties(${target} PROPERTIES CXX_STANDARD 17)
  target_compile_definitions(${target} PRIVATE LCE_DISCORD_APP_ID="${LCE_DISCORD_APP_ID}")
  if(WIN32)
    target_link_libraries(${target} PRIVATE shell32 ws2_32 dbghelp)
    # The executable's icon (tools/pcart/make_icon.py).
    target_sources(${target} PRIVATE "${CMAKE_BINARY_DIR}/LCEReborn.rc")
  else()
    find_package(Threads REQUIRED)
    target_link_libraries(${target} PRIVATE Threads::Threads ${CMAKE_DL_LIBS})
    # Function names in crash report stacks.
    set_target_properties(${target} PROPERTIES ENABLE_EXPORTS ON)
  endif()
  if(MSVC)
    target_compile_definitions(${target} PRIVATE _HAS_AUTO_PTR_ETC=1)
    target_link_options(${target} PRIVATE /errorlimit:0 /DEBUG /OPT:REF /OPT:ICF)
  endif()
endfunction()

if(WIN32)
  set(LCE_ICON "${CMAKE_SOURCE_DIR}/resources/pc/icon.ico")
  file(WRITE "${CMAKE_BINARY_DIR}/LCEReborn.rc" "1 ICON \"${LCE_ICON}\"\n")
  set_source_files_properties("${CMAKE_BINARY_DIR}/LCEReborn.rc" PROPERTIES OBJECT_DEPENDS "${LCE_ICON}")
endif()

lce_add_program(LCEReborn Main.cpp)
lce_add_program(LCERebornServer ServerMain.cpp)
# The PC interface art (tools/pcart) is read from resources/ in the working folder unless --resource-dir is given.
add_custom_command(TARGET LCEReborn POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources/pc"
          "$<TARGET_FILE_DIR:LCEReborn>/resources")

option(LCE_BUILD_LAUNCHER "Build the Qt launcher (needs python tools/build/qt.py first)" ON)
if(LCE_BUILD_LAUNCHER)
  add_subdirectory(launcher)
endif()
