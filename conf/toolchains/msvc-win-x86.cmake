# Toolchain: MSVC for Windows x86 (32-bit)
# Requires: Visual Studio 2019 or later with the "Desktop development with C++" workload
#
# Usage from areg-sdk root:
#   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=./conf/toolchains/msvc-win-x86.cmake
#   cmake --build ./build -j20 --config Release
#
# CMake selects a Visual Studio generator automatically on Windows.
# To be explicit about the VS version, add -G "Visual Studio 17 2022".

set(CMAKE_SYSTEM_NAME      Windows)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(CMAKE_C_COMPILER   cl)
set(CMAKE_CXX_COMPILER cl)

# Tell areg-sdk to apply its MSVC compiler settings
set(AREG_COMPILER_FAMILY "msvc")

# Visual Studio generator: target 32-bit platform
set(CMAKE_GENERATOR_PLATFORM Win32 CACHE INTERNAL "Target platform: 32-bit")
