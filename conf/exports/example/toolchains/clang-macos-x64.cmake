# This toolchain builds 64-bit binaries with Apple Clang compiler for macOS on x86-64 processor (Intel Macs)
# Run example:
# cmake -B ./product/cache/clang-macos-x64 -DCMAKE_TOOLCHAIN_FILE=/path/to/areg-sdk/toolchains/clang-macos-x64.cmake
# cmake --build ./product/cache/clang-macos-x64 -j20

set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# macOS SDK path (use xcrun to find it dynamically if needed)
# execute_process(COMMAND xcrun --show-sdk-path OUTPUT_VARIABLE CMAKE_OSX_SYSROOT OUTPUT_STRIP_TRAILING_WHITESPACE)

# Target architecture
set(CMAKE_OSX_ARCHITECTURES x86_64)

# Minimum macOS deployment target (adjust as needed)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS deployment version")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
