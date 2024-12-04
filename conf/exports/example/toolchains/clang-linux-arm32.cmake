# This toolchain builds 32-bit binaries with Clang compiler for Linux on 32-bit ARM processor
# Run example:
# cmake -B ./product/cache/clang-linux-arm32 -DCMAKE_TOOLCHAIN_FILE=/path/to/areg-sdk/toolchains/clang-linux-arm32.cmake
# cmake --build ./product/cache/clang-linux-arm32 -j20

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET arm-linux-gnueabihf)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET arm-linux-gnueabihf)

set(CMAKE_FIND_ROOT_PATH "/usr")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_LIBRARY_ARCHITECTURE arm-linux-gnueabihf)
