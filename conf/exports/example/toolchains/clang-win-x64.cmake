# This toolchain builds 64-bit binaries with Clang compiler for Windows on x86-64 processor
# Run example:
# cmake -B ./product/cache/clang-win-x64 -DCMAKE_TOOLCHAIN_FILE=C:/path/to/areg-sdk/toolchains/clang-win-x64.cmake
# cmake --build ./product/cache/clang-win-x64 -j20 --config=Release

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)

set(CMAKE_FIND_ROOT_PATH "/")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_LIBRARY_ARCHITECTURE x64)

set(CMAKE_GENERATOR_PLATFORM x64  CACHE INTERNAL "Force 64-bit compilation")
set(CMAKE_GENERATOR_TOOLSET ClangCL CACHE INTERNAL "Force ClangCL tool-set")
