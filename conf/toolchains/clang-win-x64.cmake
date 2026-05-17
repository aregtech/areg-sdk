# Toolchain: Clang-CL for Windows x86-64
# Requires: LLVM + Visual Studio Build Tools with the "Clang compiler for Windows" component
#
# Usage from areg-sdk root (Visual Studio generator — default on Windows):
#   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=./conf/toolchains/clang-win-x64.cmake
#   cmake --build ./build -j20 --config Release
#
# Usage with Ninja (run inside a Visual Studio Developer Command Prompt):
#   cmake -B ./build -G Ninja -DCMAKE_TOOLCHAIN_FILE=./conf/toolchains/clang-win-x64.cmake
#   cmake --build ./build -j20

set(CMAKE_SYSTEM_NAME      Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

set(CMAKE_C_COMPILER   clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)

# Tell areg-sdk to apply its LLVM/Clang compiler settings
set(AREG_COMPILER_FAMILY "llvm")

# Visual Studio generator: select 64-bit platform and ClangCL toolset
set(CMAKE_GENERATOR_PLATFORM x64     CACHE INTERNAL "Target platform: 64-bit")
set(CMAKE_GENERATOR_TOOLSET  ClangCL CACHE INTERNAL "Visual Studio toolset: Clang-CL")
