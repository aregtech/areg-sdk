# Toolchain: Apple Clang for macOS ARM64 (Apple Silicon M1/M2/M3/M4)
# Requires: Xcode Command Line Tools  (xcode-select --install)
#
# Usage from areg-sdk root:
#   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=./conf/toolchains/clang-macos-arm64.cmake
#   cmake --build ./build -j$(sysctl -n hw.logicalcpu)

set(CMAKE_SYSTEM_NAME      Darwin)
set(CMAKE_SYSTEM_PROCESSOR arm64)

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

# Target Apple Silicon
set(CMAKE_OSX_ARCHITECTURES arm64)

# Minimum macOS deployment target; adjust for your requirements
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment version")

# Tell areg-sdk to apply its LLVM/Clang compiler settings
set(AREG_COMPILER_FAMILY "llvm")
