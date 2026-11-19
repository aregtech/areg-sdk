#!/bin/sh
# =============================================================================
#  build.sh
#
#  Configures and builds the Areg SDK, or a CMake project that uses it, in a
#  build directory of its own per configuration:
#  <source>/product/build/<family>-<type>-<processor>[-static].
#  Plain POSIX sh: Linux, macOS, Cygwin and MSYS2. On Windows with MSVC or
#  clang-cl, use build.ps1 or build.bat.
#
#  Usage:  sh tools/build.sh [options] [-- <CMake configure arguments>]
#  Exit:   0 = done, 1 = bad argument, 2 = a required tool is missing,
#          3 = the configure, the build, the tests or the install failed.
# =============================================================================

set -eu

HERE=$(cd "$(dirname "$0")" && pwd -P)

fail() {
    echo "error: $1" >&2
    exit "${2:-1}"
}

usage() {
    cat <<'EOF'
usage: build.sh [options] [-- <CMake configure arguments>]

Configure and build with CMake, one build directory per configuration.

  -c, --compiler FAMILY  gnu, llvm, cygwin or mingw. Default: the host family,
                         gnu on Linux, llvm on macOS, cygwin on Cygwin,
                         mingw on MSYS2
  -t, --type TYPE        release (default), debug, relwithdebinfo, minsizerel
  -a, --arch ARCH        x86, x64, arm, aarch64. Default: the host processor
  -s, --static           build areg as a static library
  -S, --source DIR       project to build. Default: the current directory when
                         it holds a CMakeLists.txt, else this Areg SDK
  -B, --build-dir DIR    default: <source>/product/build/<family>-<type>-<arch>[-static]
  -j, --jobs N           parallel build jobs. Default: all cores
      --target NAME      build only this target; repeat for more
      --test             run the tests with ctest after the build; fails when
                         the project has none
      --install          install after the build; the location is
                         -DCMAKE_INSTALL_PREFIX=<dir> after '--'. Builds all
                         targets, so it does not combine with --target
      --clean            remove the build output before building
      --configure-only   configure, do not build
  -n, --dry-run          print the commands, run nothing
  -h, --help             show this help

Arguments after '--' go to the CMake configure step, for example:
  sh tools/build.sh -c llvm -t debug -- -DAREG_TESTS=OFF -DAREG_EXAMPLES=OFF
EOF
}

lower() {
    printf '%s' "$1" | tr '[:upper:]' '[:lower:]'
}

need_value() {
    [ "$2" -ge 2 ] || fail "$1 needs a value"
}

FAMILY=""
TYPE="release"
ARCH=""
STATIC=0
SOURCE=""
BUILD_DIR=""
JOBS=""
TARGETS=""
RUN_TESTS=0
RUN_INSTALL=0
CLEAN=0
CONFIGURE_ONLY=0
DRY_RUN=0

while [ $# -gt 0 ]; do
    case "$1" in
        -c|--compiler)   need_value "$1" $#; FAMILY=$(lower "$2"); shift 2 ;;
        -t|--type)       need_value "$1" $#; TYPE=$(lower "$2"); shift 2 ;;
        -a|--arch)       need_value "$1" $#; ARCH=$(lower "$2"); shift 2 ;;
        -s|--static)     STATIC=1; shift ;;
        -S|--source)     need_value "$1" $#; SOURCE=$2; shift 2 ;;
        -B|--build-dir)  need_value "$1" $#; BUILD_DIR=$2; shift 2 ;;
        -j|--jobs)       need_value "$1" $#; JOBS=$2; shift 2 ;;
        --target)        need_value "$1" $#; TARGETS="$TARGETS --target $2"; shift 2 ;;
        --test)          RUN_TESTS=1; shift ;;
        --install)       RUN_INSTALL=1; shift ;;
        --clean)         CLEAN=1; shift ;;
        --configure-only) CONFIGURE_ONLY=1; shift ;;
        -n|--dry-run)    DRY_RUN=1; shift ;;
        -h|--help)       usage; exit 0 ;;
        --)              shift; break ;;
        *)               fail "unknown option '$1'; see --help" ;;
    esac
done

case "$TYPE" in
    release)        CMAKE_TYPE=Release ;;
    debug)          CMAKE_TYPE=Debug ;;
    relwithdebinfo) CMAKE_TYPE=RelWithDebInfo ;;
    minsizerel)     CMAKE_TYPE=MinSizeRel ;;
    *)              fail "unknown build type '$TYPE'; use release, debug, relwithdebinfo or minsizerel" ;;
esac

if [ "$RUN_INSTALL" -eq 1 ] && [ -n "$TARGETS" ]; then
    fail "--install needs every target built; leave out --target"
fi

case "$JOBS" in
    "") ;;
    *[!0-9]*|0) fail "--jobs needs a positive number, not '$JOBS'" ;;
esac

# A compiler or a toolchain file given to CMake decides the compiler instead of the family.
CUSTOM_COMPILER=0
for arg in "$@"; do
    case "$arg" in
        -DCMAKE_CXX_COMPILER=*|-DCMAKE_CXX_COMPILER:*=*|-DCMAKE_TOOLCHAIN_FILE=*|-DCMAKE_TOOLCHAIN_FILE:*=*) CUSTOM_COMPILER=1 ;;
    esac
done

HOST_OS=$(uname -s)
case "$HOST_OS" in
    Linux)             HOST_FAMILY=gnu ;;
    Darwin|*BSD)       HOST_FAMILY=llvm ;;
    CYGWIN*)           HOST_FAMILY=cygwin ;;
    MINGW*|MSYS*)      HOST_FAMILY=mingw ;;
    *)                 HOST_FAMILY=gnu ;;
esac

case "$FAMILY" in
    "")                    ;;
    gnu|llvm|cygwin|mingw) ;;
    msvc)                  fail "msvc builds on Windows with tools/build.ps1 or tools/build.bat" ;;
    *)                     fail "unknown compiler family '$FAMILY'; use gnu, llvm, cygwin or mingw" ;;
esac

# On macOS the default compiler is Apple Clang, which CMake finds without a family.
NAME_FAMILY=${FAMILY:-custom}
if [ -z "$FAMILY" ] && [ "$CUSTOM_COMPILER" -eq 0 ]; then
    NAME_FAMILY=$HOST_FAMILY
    [ "$HOST_OS" = "Darwin" ] || FAMILY=$HOST_FAMILY
fi

case "$(uname -m)" in
    x86_64|amd64)      HOST_ARCH=x64 ;;
    i?86|x86)          HOST_ARCH=x86 ;;
    aarch64|arm64)     HOST_ARCH=aarch64 ;;
    arm*)              HOST_ARCH=arm ;;
    *)                 HOST_ARCH=$(lower "$(uname -m)") ;;
esac

# A MinGW compiler may build for another processor than the shell reports.
if [ "$FAMILY" = "mingw" ] && command -v "${CXX:-g++}" >/dev/null 2>&1; then
    case "$("${CXX:-g++}" -dumpmachine 2>/dev/null)" in
        i?86-*)        HOST_ARCH=x86 ;;
        x86_64-*)      HOST_ARCH=x64 ;;
        aarch64-*)     HOST_ARCH=aarch64 ;;
    esac
fi

case "$ARCH" in
    "")                ;;
    x86|i386|i686)     ARCH=x86 ;;
    x64|x86_64|amd64)  ARCH=x64 ;;
    arm|arm32|armv7)   ARCH=arm ;;
    aarch64|arm64)     ARCH=aarch64 ;;
    *)                 fail "unknown processor '$ARCH'; use x86, x64, arm or aarch64" ;;
esac

if [ -z "$SOURCE" ]; then
    if [ -f "./CMakeLists.txt" ]; then
        SOURCE=.
    elif [ -f "$HERE/../CMakeLists.txt" ] && [ -f "$HERE/../conf/cmake/setup.cmake" ]; then
        SOURCE=$HERE/..
    else
        fail "no CMakeLists.txt in the current directory; give the project with --source"
    fi
fi
[ -f "$SOURCE/CMakeLists.txt" ] || fail "'$SOURCE' has no CMakeLists.txt"
SOURCE=$(cd "$SOURCE" && pwd -P)

EXPLICIT_DIR=1
if [ -z "$BUILD_DIR" ]; then
    EXPLICIT_DIR=0
    NAME="$NAME_FAMILY-$TYPE-${ARCH:-$HOST_ARCH}"
    [ "$STATIC" -eq 1 ] && NAME="$NAME-static"
    BUILD_DIR="$SOURCE/product/build/$NAME"
fi

if [ -z "$JOBS" ]; then
    JOBS=$(nproc 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi

if [ "$DRY_RUN" -eq 0 ]; then
    command -v cmake >/dev/null 2>&1 || fail "cmake is not installed or not in PATH" 2
    if [ "$RUN_TESTS" -eq 1 ]; then
        command -v ctest >/dev/null 2>&1 || fail "ctest is not installed or not in PATH" 2
    fi
fi

run() {
    echo "+ $*"
    if [ "$DRY_RUN" -eq 0 ]; then
        "$@" || fail "'$1' failed with exit code $?" 3
    fi
}

# A configured default directory is configured again only for new arguments; the build re-runs it on a change.
CONFIGURE=$CONFIGURE_ONLY
if [ ! -f "$BUILD_DIR/CMakeCache.txt" ] || [ $# -gt 0 ] || [ "$EXPLICIT_DIR" -eq 1 ]; then
    CONFIGURE=1
fi

# The configure arguments: the options of this script first, then the caller's.
[ "$STATIC" -eq 1 ] && set -- -DAREG_LIB_TYPE=static "$@"
[ -n "$ARCH" ] && set -- "-DAREG_ARCH=$ARCH" "$@"
[ -n "$FAMILY" ] && set -- "-DAREG_COMPILER_FAMILY=$FAMILY" "$@"
set -- -S "$SOURCE" -B "$BUILD_DIR" "-DCMAKE_BUILD_TYPE=$CMAKE_TYPE" "$@"

if [ "$CONFIGURE" -eq 1 ]; then
    run cmake "$@"
fi
[ "$CONFIGURE_ONLY" -eq 1 ] && exit 0

if [ "$CLEAN" -eq 1 ]; then
    set -- --clean-first
else
    set --
fi
# shellcheck disable=SC2086
run cmake --build "$BUILD_DIR" --config "$CMAKE_TYPE" -j "$JOBS" $TARGETS "$@"

if [ "$RUN_TESTS" -eq 1 ]; then
    run ctest --test-dir "$BUILD_DIR" -C "$CMAKE_TYPE" -j "$JOBS" --output-on-failure --no-tests=error
fi

if [ "$RUN_INSTALL" -eq 1 ]; then
    run cmake --install "$BUILD_DIR" --config "$CMAKE_TYPE"
fi

echo "Build directory: $BUILD_DIR"
