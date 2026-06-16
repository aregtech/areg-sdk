# Toolchain: Clang for Linux x86 (32-bit, native multilib)
# Requires: clang, clang++, 32-bit multilib support  (apt: clang gcc-multilib g++-multilib)
#
# Usage from areg-sdk root:
#   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=./conf/toolchains/clang-linux-x86.cmake
#   cmake --build ./build -j$(nproc)
#
# --target=i386-linux-gnu instructs Clang to emit 32-bit code.
# areg-sdk also adds -m32 automatically when it detects an i386 processor.

set(CMAKE_SYSTEM_NAME      Linux)
set(CMAKE_SYSTEM_PROCESSOR i386)

set(CMAKE_C_COMPILER   clang)
set(CMAKE_CXX_COMPILER clang++)

# --target tells Clang which ABI to target; this is the correct mechanism for 32-bit Clang
set(CMAKE_C_COMPILER_TARGET   i386-linux-gnu)
set(CMAKE_CXX_COMPILER_TARGET i386-linux-gnu)

# Tell areg-sdk to apply its LLVM/Clang compiler settings
set(AREG_COMPILER_FAMILY "llvm")

# On a 64-bit host the default library search paths include 64-bit multilib
# directories (e.g. /usr/lib/x86_64-linux-gnu/).  The settings below steer
# CMake toward 32-bit paths instead:
#   CMAKE_LIBRARY_ARCHITECTURE - makes find_library() prefer /usr/lib/i386-linux-gnu/
#   CMAKE_FIND_ROOT_PATH + ONLY - prevents accidental pickup of 64-bit headers/libs
#                                  from outside /usr (e.g. /usr/local/lib64)
# PROGRAM stays NEVER so host build tools (cmake, python, etc.) are still found.
set(CMAKE_LIBRARY_ARCHITECTURE i386-linux-gnu)
set(CMAKE_FIND_ROOT_PATH              /usr)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
