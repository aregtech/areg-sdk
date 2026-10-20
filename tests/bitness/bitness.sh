#!/usr/bin/env bash
# =============================================================================
#  bitness.sh
#
#  Runs mtrouter, the provider and the consumer of areg-bitness-test from a 32-bit
#  and a 64-bit build, in every mix, and checks that each process reads the other's
#  sample exactly. Linux, macOS and Cygwin; tests/bitness/bitness.ps1 on Windows.
#
#  Usage:  tests/bitness/bitness.sh --bin64 <dir> [--bin32 <dir>] [--timeout <s>]
#  Without --bin32 only the 64-bit build runs, which checks the test itself.
#  Exit:   0 = every run passed, 1 = a run failed, 2 = bad argument or missing file.
# =============================================================================

set -u

BIN64=""
BIN32=""
TIMEOUT=60

case "$(uname -s)" in
    Darwin)               EXE=".mac" ;;
    CYGWIN*|MINGW*|MSYS*) EXE=".exe" ;;
    *)                    EXE=".elf" ;;
esac

fail() {
    echo "error: $1" >&2
    exit 2
}

while [ $# -gt 0 ]; do
    case "$1" in
        --bin64)   [ $# -ge 2 ] || fail "--bin64 needs a directory"; BIN64="$2"; shift 2 ;;
        --bin32)   [ $# -ge 2 ] || fail "--bin32 needs a directory"; BIN32="$2"; shift 2 ;;
        --timeout) [ $# -ge 2 ] || fail "--timeout needs seconds";   TIMEOUT="$2"; shift 2 ;;
        -h|--help) sed -n '2,13p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *)         fail "unknown argument: $1" ;;
    esac
done

[ -n "$BIN64" ] || fail "--bin64 is required"
BIN64=$(cd "$BIN64" && pwd) || fail "no directory $BIN64"
[ -z "$BIN32" ] || BIN32=$(cd "$BIN32" && pwd) || fail "no directory $BIN32"

# The bin directory of one bitness.
bin_dir() {
    if [ "$1" = "64" ]; then echo "$BIN64"; else echo "$BIN32"; fi
}

# True while something accepts connections on the port.
listening() {
    (exec 3<>"/dev/tcp/127.0.0.1/$1") 2>/dev/null
}

# A port nothing listens on.
free_port() {
    port=$(( 20000 + RANDOM % 20000 ))
    while listening "$port"; do
        port=$(( 20000 + RANDOM % 20000 ))
    done
    echo "$port"
}

# The router's areg.init with this run's port and logging off.
write_config() {
    sp='[[:space:]]*'
    sed -e "s|^\(${sp}router::\*::port::tcpip${sp}=\).*|\1 $3|" \
        -e "s|^\(${sp}router::\*::address::tcpip${sp}=\).*|\1 127.0.0.1|" \
        -e "s|^\(${sp}service::\*::list${sp}=\).*|\1 router|" \
        -e "s|^\(${sp}log::\*::enable${sp}=\).*|\1 false|" \
        "$1/config/areg.init" > "$2"
}

# Run in the background: becomes the process, from its bin directory, its output in a log.
start() {
    dir="$1"; log="$2"; shift 2
    cd "$dir" || exit 1
    LD_LIBRARY_PATH="$dir:${LD_LIBRARY_PATH:-}" DYLD_LIBRARY_PATH="$dir:${DYLD_LIBRARY_PATH:-}" \
        exec "$@" > "$log" 2>&1
}

stop() {
    for pid in "$@"; do
        kill "$pid" 2>/dev/null
        wait "$pid" 2>/dev/null
    done
}

# One run of router, provider and consumer bitness; prints its problems.
run_mix() {
    router_dir=$(bin_dir "$1"); provider_dir=$(bin_dir "$2"); consumer_dir=$(bin_dir "$3")
    for f in "$router_dir/mtrouter$EXE" "$provider_dir/areg-bitness-test$EXE" "$consumer_dir/areg-bitness-test$EXE"; do
        [ -f "$f" ] || { echo "$f not found"; return; }
    done

    work=$(mktemp -d "${TMPDIR:-/tmp}/areg-bitness-XXXXXX")
    port=$(free_port)
    config="$work/areg.init"
    write_config "$router_dir" "$config" "$port"

    # The console quits when its stdin ends, so it reads a FIFO this script holds open.
    mkfifo "$work/stdin"
    exec 4<>"$work/stdin"
    start "$router_dir" "$work/router.log" "./mtrouter$EXE" --console "--load=$config" < "$work/stdin" &
    router=$!
    for _ in $(seq 1 100); do listening "$port" && break; sleep 0.1; done
    if ! listening "$port"; then
        echo "mtrouter did not listen on $port"
        stop "$router"; exec 4>&-
        return
    fi

    start "$provider_dir" "$work/provider.log" "./areg-bitness-test$EXE" provider "$config" &
    provider=$!
    start "$consumer_dir" "$work/consumer.log" "./areg-bitness-test$EXE" consumer "$config" &
    consumer=$!

    waited=0
    while kill -0 "$consumer" 2>/dev/null && [ "$waited" -lt $(( TIMEOUT * 10 )) ]; do
        sleep 0.1
        waited=$(( waited + 1 ))
    done
    if kill -0 "$consumer" 2>/dev/null; then
        echo "the consumer did not finish in $TIMEOUT s"
        stop "$consumer"
    else
        wait "$consumer"; code=$?
        [ "$code" -eq 0 ] || echo "the consumer exited $code"
    fi
    stop "$provider" "$router"
    exec 4>&-

    check "$work/consumer.log" consumer "$3" "response ok" "attribute ok" "broadcast ok"
    check "$work/provider.log" provider "$2" "provider request ok"
}

# Prints what is wrong in one process's log: its bitness, a missing line, a differing field.
check() {
    log="$1"; side="$2"; bits="$3"; shift 3
    grep -q "bitness: $side is $bits-bit" "$log" || echo "the $side is not the $bits-bit build"
    for line in "$@"; do
        grep -q "bitness: $line" "$log" || echo "$side: \"$line\" missing"
    done
    grep "differs" "$log" | sed "s/^/$side: /"
}

MIXES="64:64:64 64:64:32 64:32:64 32:64:32 32:32:32"
total=0
failed=0
for mix in $MIXES; do
    case "$mix" in *32*) [ -n "$BIN32" ] || continue ;; esac
    set -- $(echo "$mix" | tr ':' ' ')
    problems=$(run_mix "$1" "$2" "$3")
    total=$(( total + 1 ))
    if [ -z "$problems" ]; then
        echo "PASS: router $1-bit, provider $2-bit, consumer $3-bit"
    else
        echo "FAIL: router $1-bit, provider $2-bit, consumer $3-bit"
        echo "$problems" | sed 's/^/    /'
        failed=$(( failed + 1 ))
    fi
done

echo "$(( total - failed )) of $total runs passed"
[ "$failed" -eq 0 ]
