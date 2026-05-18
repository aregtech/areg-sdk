# Toolchain: GCC for Linux x86 (32-bit, native multilib)
# Requires: gcc, g++, multilib support  (apt: build-essential gcc-multilib g++-multilib)
#
# Usage from areg-sdk root:
#   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=./conf/toolchains/gnu-linux-x86.cmake
#   cmake --build ./build -j$(nproc)
#
# areg-sdk detects i386 and automatically adds -m32 to compiler and linker flags.

set(CMAKE_SYSTEM_NAME      Linux)
set(CMAKE_SYSTEM_PROCESSOR i386)

set(CMAKE_C_COMPILER   gcc)
set(CMAKE_CXX_COMPILER g++)

# Tell areg-sdk to apply its GNU compiler settings
set(AREG_COMPILER_FAMILY "gnu")

# On a 64-bit host the default library search paths include 64-bit multilib
# directories (e.g. /usr/lib/x86_64-linux-gnu/).  The settings below steer
# CMake toward 32-bit paths instead:
#   CMAKE_LIBRARY_ARCHITECTURE — makes find_library() prefer /usr/lib/i386-linux-gnu/
#   CMAKE_FIND_ROOT_PATH + ONLY — prevents accidental pickup of 64-bit headers/libs
#                                  from outside /usr (e.g. /usr/local/lib64)
# PROGRAM stays NEVER so host build tools (cmake, python, etc.) are still found.
set(CMAKE_LIBRARY_ARCHITECTURE i386-linux-gnu)
set(CMAKE_FIND_ROOT_PATH              /usr)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
