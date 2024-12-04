# This toolchain builds 64-bit binaries with GNU g++/gcc compiler for Linux on x86-64 processor
# Run example:
# cmake -B ./product/cache/gnu-linux-x64 -DCMAKE_TOOLCHAIN_FILE=/path/to/areg-sdk/toolchains/gnu-linux-x64.cmake
# cmake --build ./product/cache/gnu-linux-x64 -j20

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_COMPILER_TARGET x86_64-linux-gnu)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_COMPILER_TARGET x86_64-linux-gnu)

# set(CMAKE_FIND_ROOT_PATH "/usr/lib/x86_64-linux-gnu;/usr/x86_64-linux-gnu;/usr/lib;/usr/include")
set(CMAKE_FIND_ROOT_PATH "/usr")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_LIBRARY_ARCHITECTURE x86_64-linux-gnu)
