# This toolchain builds 32-bit binaries with MSVC compiler for Windows on x86 processor
# Run example:
# cmake -B ./product/cache/msvc-win-x86 -DCMAKE_TOOLCHAIN_FILE=C:/path/to/areg-sdk/toolchains/msvc-win-x86.cmake
# cmake --build ./product/cache/msvc-win-x86 -j20 --config=Release

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(CMAKE_C_COMPILER cl)
set(CMAKE_CXX_COMPILER cl)

set(CMAKE_FIND_ROOT_PATH "/")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_LIBRARY_ARCHITECTURE x86)

set(CMAKE_GENERATOR_PLATFORM Win32  CACHE INTERNAL "Force 32-bit compilation")
