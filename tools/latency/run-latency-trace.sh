#!/usr/bin/env bash
# ===========================================================================
# Per-stage latency trace of areg-sdk (task T0, step 3).
#
# The framework can be built with AREG_LATENCY_TRACE=1. Every instrumented
# stage then accumulates count, min, mean and max, and the table is printed to
# standard error when the process leaves through its own exit point. The stages
# are:
#
#   SendNode     the send thread: drain, serialize and write the batch
#   SendSyscall  the send thread: only the write syscall
#   RecvNode     the receive thread: deserialize and route one message
#   CompDispatch the dispatcher: the component's own handler
#   MpscHandoff  the event queue: from enqueue to dequeue, so the cost of
#                handing a message to another thread, wake-up included
#
# IMPORTANT
#   * The table is printed by std::atexit. It appears ONLY when the process
#     ends through its own exit path. Never kill the processes.
#   * The instrumentation itself takes time. The absolute latencies of a traced
#     build are higher than those of a normal build. Read the SHARE of each
#     stage, not its absolute value, and never mix the two builds in one table.
#   * The traced build gets its own output folder through AREG_OUTPUT_DIR, so
#     it can never overwrite the normal binaries used for the baseline.
#
# Usage:
#   tools/latency/run-latency-trace.sh
# ===========================================================================

set -u

REPO_ROOT="$(cd "$(dirname "$(realpath "$0")")/../.." && pwd)"
TRACE_OUT="$REPO_ROOT/product/build/latency-trace"
BIN_DIR="$TRACE_OUT/bin"
OUTDIR="$REPO_ROOT/product/tasks/measurements"
STAMP=$(date +%Y%m%d-%H%M%S)
LOG="$OUTDIR/$STAMP-latency-trace.log"

for arg in "$@"; do
    case "$arg" in
        -h|--help) sed -n '2,28p' "$0"; exit 0 ;;
        *) echo "ERROR: unknown option '$arg'"; exit 1 ;;
    esac
done

mkdir -p "$OUTDIR"
cd "$REPO_ROOT" || exit 1

echo "=== building with AREG_LATENCY_TRACE=1 ===" | tee "$LOG"
cmake -B build-lt -G Ninja -DCMAKE_BUILD_TYPE=Release -DAREG_COMPILER_FAMILY=gnu \
      -DAREG_EXAMPLES=ON -DAREG_TESTS=OFF \
      -DAREG_OUTPUT_DIR="$TRACE_OUT" \
      -DCMAKE_CXX_FLAGS="-DAREG_LATENCY_TRACE=1" >> "$LOG" 2>&1 || { echo "configure failed"; exit 1; }
cmake --build build-lt -j 8 >> "$LOG" 2>&1 || { echo "build failed, see $LOG"; exit 1; }

# ---------------------------------------------------------------- example 31
# One process, so the only stages that can fire are CompDispatch and
# MpscHandoff. That is the point: MpscHandoff IS the cross-thread handoff.
for TOPO in same cross; do
    echo "" | tee -a "$LOG"
    echo "=== example 31, topology $TOPO, AREG_LATENCY_TRACE ===" | tee -a "$LOG"
    "$BIN_DIR/31_loclatency.elf" -t="$TOPO" -m=bc0,pp0 -c=50000 -w=5000 -q 2>&1 | tee -a "$LOG"
done

# ---------------------------------------------------------------- example 30
# Two processes and the router, so every stage fires. The driver quits every
# process through its own '-q' command, which is what makes the table appear.
echo "" | tee -a "$LOG"
echo "=== example 30, T-C, AREG_LATENCY_TRACE ===" | tee -a "$LOG"
python3 tools/latency/run-pub-latency.py --modes=bc0,pp0 --count=20000 --warmup=2000 \
        --runs=1 --label=trace --bin-dir="$BIN_DIR" \
        --out-dir="$OUTDIR" >> "$LOG" 2>&1

echo "" | tee -a "$LOG"
echo "=== per-stage tables found in the process logs ===" | tee -a "$LOG"
grep -rl "AREG LATENCY TRACE" "$OUTDIR"/*-publatency-trace/ 2>/dev/null | while read -r f; do
    echo "" | tee -a "$LOG"
    echo "--- $f" | tee -a "$LOG"
    sed 's/\x1b\[[0-9;?]*[a-zA-Z]//g' "$f" \
        | sed -n '/AREG LATENCY TRACE/,/^=\{20,\}$/p' | tee -a "$LOG"
done

echo "" | tee -a "$LOG"
echo "log: $LOG"
