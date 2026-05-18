# Toolchain: Clang for Linux x86-64 (native build)
# Requires: clang, clang++  (apt: clang)
#
# Usage from areg-sdk root:
#   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=./conf/toolchains/clang-linux-x64.cmake
#   cmake --build ./build -j$(nproc)

set(CMAKE_SYSTEM_NAME      Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER   clang)
set(CMAKE_CXX_COMPILER clang++)

# --target tells Clang which ABI to target; required for correct code generation
set(CMAKE_C_COMPILER_TARGET   x86_64-linux-gnu)
set(CMAKE_CXX_COMPILER_TARGET x86_64-linux-gnu)

# Tell areg-sdk to apply its LLVM/Clang compiler settings
set(AREG_COMPILER_FAMILY "llvm")
