#!/bin/sh
# =============================================================================
#  setup-project.sh
#
#  Creates a ready-to-build Areg project outside the SDK. Plain POSIX sh, no
#  Python. It writes the same project as tools/agent/setup_project.py: one of
#  the recipes under docs/agent/recipes/, copied and renamed. The agent files
#  (AGENTS.md, design.json, scenarios.json) are written by the Python tool only.
#
#  Usage:  tools/setup-project.sh [--name NAME] [--root DIR] [--mode local|ipc|pubsub]
#                                 [--sdk-root DIR] [--tag TAG] [--force]
#  Asks for a missing name, mode or directory when run on a terminal.
#  Exit:   0 = created, 1 = bad argument or refused, 2 = a required tool is missing.
# =============================================================================

set -eu

HERE=$(cd "$(dirname "$0")" && pwd)
FALLBACK_TAG="master"

fail() {
    echo "error: $1" >&2
    exit "${2:-1}"
}

usage() {
    cat <<'EOF'
usage: setup-project.sh [--name NAME] [--root DIR] [--mode local|ipc|pubsub]
                        [--sdk-root DIR] [--tag TAG] [--force]

Create a ready-to-build Areg project.

  --name NAME      project name; a C identifier
  --root DIR       directory to create; defaults to ./<name>
  --mode MODE      local: one process, two threads. ipc: two processes.
                   pubsub: local, whose interface also declares attributes
                   and broadcasts
  --sdk-root DIR   use this local SDK copy instead of fetching from GitHub
  --tag TAG        SDK git tag to fetch
  --force          scaffold into a directory that is not empty; files of the
                   same name are overwritten, nothing is deleted
EOF
}

# The recipe and the CMake name tokens of each mode, as in setup_project.py.
mode_recipe() {
    case "$1" in
        local) RECIPE="01-local-single-process"
               TOKENS="hello_local={name}" ;;
        ipc)   RECIPE="02-ipc-two-processes"
               TOKENS="hello_provider={name}_provider
hello_consumer={name}_consumer
hello_ipc={name}" ;;
        pubsub) RECIPE="03-attributes-and-broadcast"
               TOKENS='gen_monitor=gen_{name}
"monitor"="{name}"
macro_declare_executable(monitor =macro_declare_executable({name} ' ;;
        *)     return 1 ;;
    esac
}

find_recipes() {
    for candidate in "$HERE/../docs/agent" "$HERE/../../share/areg/sdk/docs/agent"; do
        if [ -d "$candidate/recipes" ]; then
            echo "$candidate"
            return 0
        fi
    done
    return 1
}

# The path CMake understands: a Windows path under Git Bash or MSYS2, the plain one elsewhere.
native_dir() {
    (cd "$1" && (pwd -W 2>/dev/null || pwd))
}

ask() {
    printf '%s [%s]: ' "$1" "$2" >&2
    read -r answer || answer=""
    [ -n "$answer" ] && echo "$answer" || echo "$2"
}

NAME=""
ROOT=""
MODE=""
SDK_ROOT=""
TAG=""
FORCE=0

while [ $# -gt 0 ]; do
    case "$1" in
        --name)     [ $# -ge 2 ] || fail "--name needs a value"; NAME="$2"; shift 2 ;;
        --root)     [ $# -ge 2 ] || fail "--root needs a value"; ROOT="$2"; shift 2 ;;
        --mode)     [ $# -ge 2 ] || fail "--mode needs a value"; MODE="$2"; shift 2 ;;
        --sdk-root) [ $# -ge 2 ] || fail "--sdk-root needs a value"; SDK_ROOT="$2"; shift 2 ;;
        --tag)      [ $# -ge 2 ] || fail "--tag needs a value"; TAG="$2"; shift 2 ;;
        --force)    FORCE=1; shift ;;
        -h|--help)  usage; exit 0 ;;
        *)          fail "unknown option: $1 (see --help)" ;;
    esac
done

AGENT_DOCS=$(find_recipes) || fail "agent documentation not found next to $HERE. Expected it at ../docs/agent (a clone) or ../../share/areg/sdk/docs/agent (an installation)."

DEFAULT_TAG=$(sed -n 's/.*"fetch_ref"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' "$AGENT_DOCS/api.json" 2>/dev/null | head -n 1)
[ -n "$DEFAULT_TAG" ] || DEFAULT_TAG="$FALLBACK_TAG"
[ -n "$TAG" ] || TAG="$DEFAULT_TAG"

if [ -t 0 ]; then
    [ -n "$NAME" ] || NAME=$(ask "Project name" "areg_hello")
    [ -n "$MODE" ] || MODE=$(ask "Mode (local, ipc, pubsub)" "local")
    [ -n "$ROOT" ] || ROOT=$(ask "Project directory" "./$NAME")
else
    [ -n "$NAME" ] || fail "--name is required when there is no terminal to ask on"
    [ -n "$MODE" ] || MODE="local"
    [ -n "$ROOT" ] || ROOT="./$NAME"
fi

mode_recipe "$MODE" || fail "unknown mode: $MODE (local, ipc or pubsub)"

case "$NAME" in
    ''|[0-9]*|*[!A-Za-z0-9_]*) fail "project name \"$NAME\" is not a C identifier; it becomes a CMake target" ;;
esac

if [ -n "$SDK_ROOT" ]; then
    [ -f "$SDK_ROOT/areg.cmake" ] || fail "no areg.cmake under --sdk-root $SDK_ROOT"
    SDK_ROOT=$(native_dir "$SDK_ROOT" | sed 's#\\#/#g')
fi

command -v cmake >/dev/null 2>&1 || fail "not found on PATH: cmake" 2
if [ -z "$SDK_ROOT" ]; then
    command -v git >/dev/null 2>&1 || fail "not found on PATH: git" 2
fi

if [ -d "$ROOT" ] && [ -n "$(ls -A "$ROOT" 2>/dev/null)" ] && [ "$FORCE" -eq 0 ]; then
    fail "$ROOT exists and is not empty. Pass --force to scaffold into it anyway, or use a different --root."
fi

SOURCE="$AGENT_DOCS/recipes/$RECIPE"
[ -d "$SOURCE" ] || fail "recipe not found: $SOURCE"

mkdir -p "$ROOT"
cp -R "$SOURCE"/. "$ROOT"/
ROOT=$(cd "$ROOT" && pwd)

for cmake in "$ROOT/CMakeLists.txt" "$ROOT/src/CMakeLists.txt"; do
    printf '%s\n' "$TOKENS" | while IFS= read -r pair; do
        old=${pair%%=*}
        new=$(printf '%s' "${pair#*=}" | sed "s/{name}/$NAME/g")
        sed "s/$old/$new/g" "$cmake" > "$cmake.tmp" && mv "$cmake.tmp" "$cmake"
    done
done

TOP="$ROOT/CMakeLists.txt"
grep -q '^    FetchContent_Declare(areg$' "$TOP" || fail "recipe CMakeLists.txt does not carry the expected FetchContent block"

if [ -n "$SDK_ROOT" ]; then
    BLOCK="    FetchContent_Declare(areg SOURCE_DIR \"$SDK_ROOT\")"
else
    BLOCK="    FetchContent_Declare(areg
        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
        GIT_TAG \"$TAG\")"
fi

awk -v block="$BLOCK" '
    /^    FetchContent_Declare\(areg$/ { print block; skip = 1; next }
    skip && /^    FetchContent_MakeAvailable\(areg\)$/ { skip = 0 }
    skip { next }
    /^add_subdirectory\(src\)$/ && !done {
        print "include_directories(\"${CMAKE_CURRENT_SOURCE_DIR}\")"
        done = 1
    }
    { print }
' "$TOP" > "$TOP.tmp" && mv "$TOP.tmp" "$TOP"

printf 'build/\nbuild-*/\nproduct/\n*.sqlog\n*.log\n' > "$ROOT/.gitignore"

if [ "$MODE" = "ipc" ]; then
    PROVIDER="${NAME}_provider"
    CONSUMER="${NAME}_consumer"

    cat > "$ROOT/run.sh" <<EOF
#!/usr/bin/env bash
# Starts the router and the applications in the order they need.
set -eu
case "\$(uname -s)" in Darwin) SFX=.mac ;; *) SFX=.elf ;; esac
BIN=./build/bin
# Every process this script starts in the background. The consumer runs
# in the foreground and ends on its own; the router and the provider do
# not, so a script that leaves either behind adds one orphan per run.
# An orphan holds this script's stdout open, and "./run.sh | tail" then
# never ends: the output looks like a hung application.
STARTED=""
stop() { [ -n "\$STARTED" ] && kill \$STARTED 2>/dev/null || true; }
trap stop EXIT INT TERM
# --service is the unattended mode; the console default draws a live
# status display that only makes sense on a terminal.
"\$BIN/mtrouter\$SFX" --service &
STARTED="\$!"
# Wait for the router to accept a connection. Only one process can hold
# 8181, so this also catches one that was already running.
for _ in \$(seq 1 100); do
    (exec 3<>/dev/tcp/127.0.0.1/8181) 2>/dev/null && break
    sleep 0.1
done
"\$BIN/$PROVIDER\$SFX" &
STARTED="\$STARTED \$!"
sleep 1
"\$BIN/$CONSUMER\$SFX"
EOF
    chmod +x "$ROOT/run.sh"

    sed -e "s/@PROVIDER@/$PROVIDER/g" -e "s/@CONSUMER@/$CONSUMER/g" <<'EOF' | awk '{ printf "%s\r\n", $0 }' > "$ROOT/run.bat"
@echo off
rem Starts the router and the applications in the order they need, and
rem stops the ones it started once the consumer ends.
setlocal
set BIN=build\bin
rem The router runs in console mode, in a window of its own: --service is
rem the Service Control Manager and returns at once from a command line.
for /f %%p in ('powershell -NoProfile -Command "(Start-Process -FilePath '%BIN%\mtrouter.exe' -PassThru).Id"') do set ROUTER=%%p
rem Waits for the router to listen on 8181.
for /l %%i in (1,1,20) do (
    netstat -an | findstr /r /c:":8181 .*LISTENING" >nul && goto ready
    timeout /t 1 /nobreak >nul
)
:ready
for /f %%p in ('powershell -NoProfile -Command "(Start-Process -FilePath '%BIN%\@PROVIDER@.exe' -PassThru).Id"') do set PROVIDER=%%p
timeout /t 1 /nobreak >nul
"%BIN%\@CONSUMER@.exe"
set RC=%errorlevel%
taskkill /pid %PROVIDER% /t /f >nul 2>&1
taskkill /pid %ROUTER% /t /f >nul 2>&1
exit /b %RC%
EOF
fi

echo "created $ROOT ($MODE mode)"
echo "  Build it:"
echo "    cd $ROOT"
echo "    cmake -B build"
echo "    cmake --build build -j"
if [ "$MODE" = "ipc" ]; then
    echo "  Run it: ./run.sh starts mtrouter, the provider and the consumer in order."
else
    echo "  Run it: $NAME in build/bin/."
fi
echo "  Building with an AI coding agent? Install Python 3 and create the project with"
echo "  tools/agent/setup_project.py instead: it also writes AGENTS.md and design.json,"
echo "  and the agent tools generate, check and test the code for the agent."
