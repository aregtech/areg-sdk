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

# The major and the minor of a version string, as two numbers on one line. A
# version is compared as two numbers, so 3.9 is older than 3.20 and 3.20.1 is not.
# Anything after the minor is ignored: a build number never decides this.
two_numbers() {
    major=`echo "$1" | sed 's/[^0-9].*//'`
    minor=`echo "$1" | sed 's/^[0-9]*[.]*//' | sed 's/[^0-9].*//'`
    [ -n "$major" ] || major=0
    [ -n "$minor" ] || minor=0
    echo "$major $minor"
}

# Whether the first version is older than <major>.<minor>. An unreadable version
# is not called old: a tool that prints something this cannot parse is reported as
# present, and the build is what finds out.
older_than() {
    set -- `two_numbers "$1"` "$2" "$3"
    [ "$1" -eq 0 ] && return 1
    [ "$1" -lt "$3" ] && return 0
    [ "$1" -gt "$3" ] && return 1
    [ "$2" -lt "$4" ]
}

echo ""
echo "AREG environment check"
echo "----------------------"

# --- CMake 3.20+ : required --------------------------------------------------
if command -v cmake >/dev/null 2>&1; then
    ver=`cmake --version 2>/dev/null | head -n 1 | awk '{print $3}'`
    if older_than "$ver" 3 20; then
        echo "  [OLD]      cmake      $ver - required 3.20+"
        echo "             apt install cmake | dnf install cmake | brew install cmake"
        failed=1
    else
        echo "  [ok]       cmake      $ver"
    fi
else
    echo "  [MISSING]  cmake      - required 3.20+"
    echo "             apt install cmake | dnf install cmake | brew install cmake"
    failed=1
fi

# --- Java 17+ : required, runs tools/codegen.jar -----------------------------
if command -v java >/dev/null 2>&1; then
    ver=`java -version 2>&1 | head -n 1 | sed 's/.*version "\([^"]*\)".*/\1/'`
    # Java 8 and older spell themselves 1.8.0_292, so the number that decides is
    # the one after the 1.
    case "$ver" in
        1.*) compare=`echo "$ver" | sed 's/^1[.]//'` ;;
        *)   compare="$ver" ;;
    esac
    if older_than "$compare" 17 0; then
        echo "  [OLD]      java       $ver - required 17+, runs tools/codegen.jar"
        echo "             apt install default-jre | dnf install java-17-openjdk | brew install openjdk@17"
        failed=1
    else
        echo "  [ok]       java       $ver"
    fi
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
    # A compiler's own version number does not say which standard its default
    # invocation accepts. Compiling one C++17 line does.
    probe="${TMPDIR:-/tmp}/areg-env-$$"
    mkdir -p "$probe" 2>/dev/null
    echo '#include <string_view>' > "$probe/probe.cpp"
    echo 'int main() { std::string_view text{"ok"}; return text.size() == 2 ? 0 : 1; }' >> "$probe/probe.cpp"
    if $cxx -std=c++17 -o "$probe/probe" "$probe/probe.cpp" >/dev/null 2>&1; then
        echo "  [ok]       c++        $ver"
    else
        echo "  [OLD]      c++        $ver - does not compile C++17"
        echo "             apt install build-essential | dnf install gcc-c++ | xcode-select --install"
        failed=1
    fi
    rm -rf "$probe"
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
echo "  next: cmake -B build  &&  cmake --build build -j8"
exit 0
