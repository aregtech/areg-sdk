# Toolchain: Clang cross-compiler for Linux ARM 32-bit (armhf)
# Requires: clang, clang++, ARM binutils  (apt: clang binutils-arm-linux-gnueabihf)
# Note: binutils or a GCC cross-sysroot is needed for the linker and C runtime headers.
#
# Usage from areg-sdk root:
#   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=./conf/toolchains/clang-linux-arm32.cmake
#   cmake --build ./build -j$(nproc)
#
# For a proper cross-build with target headers and libraries, also pass:
#   -DCMAKE_SYSROOT=/path/to/arm-sysroot

set(CMAKE_SYSTEM_NAME      Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER   clang)
set(CMAKE_CXX_COMPILER clang++)

# --target tells Clang which ABI to target; required for cross-compilation
set(CMAKE_C_COMPILER_TARGET   arm-linux-gnueabihf)
set(CMAKE_CXX_COMPILER_TARGET arm-linux-gnueabihf)

# Tell areg-sdk to apply its LLVM/Clang compiler settings
set(AREG_COMPILER_FAMILY "llvm")

# Restrict search paths to the target sysroot (set CMAKE_SYSROOT to override /usr)
set(CMAKE_FIND_ROOT_PATH              /usr)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_LIBRARY_ARCHITECTURE arm-linux-gnueabihf)
