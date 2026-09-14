#!/bin/sh
# =============================================================================
#  check-env.sh
#
#  Reports whether this machine can build an AREG application.
#  Plain POSIX sh, so it also works when Python is the missing piece.
#
#  Usage:  tools/check-env.sh
#  Exit:   0 = every requirement is present, 1 = at least one is missing.
# =============================================================================

failed=0

echo ""
echo "AREG environment check"
echo "----------------------"

# --- CMake 3.20+ : required --------------------------------------------------
if command -v cmake >/dev/null 2>&1; then
    ver=`cmake --version 2>/dev/null | head -n 1 | awk '{print $3}'`
    echo "  [ok]       cmake      $ver"
else
    echo "  [MISSING]  cmake      - required 3.20+"
    echo "             apt install cmake | dnf install cmake | brew install cmake"
    failed=1
fi

# --- Java 17+ : required, runs tools/codegen.jar -----------------------------
if command -v java >/dev/null 2>&1; then
    ver=`java -version 2>&1 | head -n 1 | sed 's/.*version "\([^"]*\)".*/\1/'`
    echo "  [ok]       java       $ver"
else
    echo "  [MISSING]  java       - required 17+, runs tools/codegen.jar"
    echo "             apt install default-jre | dnf install java-17-openjdk | brew install openjdk@17"
    failed=1
fi

# --- C++ compiler : required -------------------------------------------------
cxx=""
if   command -v g++     >/dev/null 2>&1; then cxx="g++"
elif command -v clang++ >/dev/null 2>&1; then cxx="clang++"
elif command -v c++     >/dev/null 2>&1; then cxx="c++"
fi

if [ -n "$cxx" ]; then
    ver=`$cxx --version 2>/dev/null | head -n 1`
    echo "  [ok]       c++        $ver"
else
    echo "  [MISSING]  c++        - required C++17 compiler"
    echo "             apt install build-essential | dnf install gcc-c++ | xcode-select --install"
    failed=1
fi

# --- Python 3 : optional -----------------------------------------------------
py=""
pyver=""
for cand in python3 python; do
    if command -v "$cand" >/dev/null 2>&1; then
        out=`"$cand" --version 2>&1`
        # Guard against shims that print an advertisement instead of a version.
        case "$out" in
            Python\ [0-9]*) py="$cand"; pyver=`echo "$out" | awk '{print $2}'`; break ;;
        esac
    fi
done

if [ -n "$py" ]; then
    echo "  [ok]       python     $pyver ($py)"
else
    echo "  [absent]   python     - OPTIONAL. Only the SDK helper scripts need it."
    echo "             Without it, copy a recipe from docs/agent/recipes/ instead."
fi

echo ""
if [ "$failed" -eq 1 ]; then
    echo "RESULT: cannot build. Install the [MISSING] items above."
    exit 1
fi

echo "RESULT: ready to build."
echo "  next: cmake -B build  &&  cmake --build build -j"
exit 0
