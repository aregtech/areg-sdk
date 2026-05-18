# Toolchain: GCC cross-compiler for Linux ARM 64-bit (AArch64)
# Requires: aarch64-linux-gnu-gcc/g++  (apt: gcc-aarch64-linux-gnu g++-aarch64-linux-gnu)
#
# Usage from areg-sdk root:
#   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=./conf/toolchains/gnu-linux-arm64.cmake
#   cmake --build ./build -j$(nproc)
#
# For a proper cross-build with target headers and libraries, also pass:
#   -DCMAKE_SYSROOT=/path/to/aarch64-sysroot

set(CMAKE_SYSTEM_NAME      Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# GCC cross-compilers encode the target triple in the binary name;
# CMAKE_C*_COMPILER_TARGET is a Clang concept and is not used here.
set(CMAKE_C_COMPILER   aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Tell areg-sdk to apply its GNU compiler settings
set(AREG_COMPILER_FAMILY "gnu")

# Restrict search paths to the target sysroot (set CMAKE_SYSROOT to override /usr)
set(CMAKE_FIND_ROOT_PATH              /usr)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_LIBRARY_ARCHITECTURE aarch64-linux-gnu)
