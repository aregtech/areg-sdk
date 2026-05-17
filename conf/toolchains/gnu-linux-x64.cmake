# Toolchain: GCC for Linux x86-64 (native build)
# Requires: gcc, g++  (apt: build-essential)
#
# Usage from areg-sdk root:
#   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=./conf/toolchains/gnu-linux-x64.cmake
#   cmake --build ./build -j$(nproc)

set(CMAKE_SYSTEM_NAME      Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER   gcc)
set(CMAKE_CXX_COMPILER g++)

# Tell areg-sdk to apply its GNU compiler settings
set(AREG_COMPILER_FAMILY "gnu")
