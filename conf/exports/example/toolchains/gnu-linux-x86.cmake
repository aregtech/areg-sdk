# This toolchain builds 32-bit binaries with GNU compiler for Linux on x86 processor
# Run example:
# cmake -B ./product/cache/gnu-linux-x86 -DCMAKE_TOOLCHAIN_FILE=/path/to/areg-sdk/toolchains/gnu-linux-x86.cmake
# cmake --build ./product/cache/gnu-linux-x86 -j20

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR i386)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_COMPILER_TARGET i386-linux-gnu)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_COMPILER_TARGET i386-linux-gnu)

set(CMAKE_FIND_ROOT_PATH "/usr")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_LIBRARY_ARCHITECTURE i386-linux-gnu)
