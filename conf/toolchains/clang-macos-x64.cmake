# Toolchain: Apple Clang for macOS x86-64 (Intel)
# Requires: Xcode Command Line Tools  (xcode-select --install)
#
# Usage from areg-sdk root:
#   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=./conf/toolchains/clang-macos-x64.cmake
#   cmake --build ./build -j$(sysctl -n hw.logicalcpu)
#
# On Apple Silicon, this produces an x86-64 binary that runs under Rosetta 2.

set(CMAKE_SYSTEM_NAME      Darwin)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER   clang)
set(CMAKE_CXX_COMPILER clang++)

# Resolve the active Xcode SDK at configure time so headers and frameworks are found
execute_process(
    COMMAND xcrun --show-sdk-path
    OUTPUT_VARIABLE _xcrun_sdk
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if (NOT "${_xcrun_sdk}" STREQUAL "")
    set(CMAKE_OSX_SYSROOT "${_xcrun_sdk}" CACHE PATH "macOS SDK path")
endif()
unset(_xcrun_sdk)

# Target Intel x86-64
set(CMAKE_OSX_ARCHITECTURES x86_64)

# Minimum macOS deployment target; adjust for your requirements
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS deployment version")

# Tell areg-sdk to apply its LLVM/Clang compiler settings
set(AREG_COMPILER_FAMILY "llvm")
