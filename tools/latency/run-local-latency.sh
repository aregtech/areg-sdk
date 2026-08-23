#!/usr/bin/env bash
# ===========================================================================
# Unattended runner for example 31 (local latency benchmark).
#
# Runs the same mode list in both topologies -- "same" (one component thread)
# and "cross" (two component threads) -- and collects everything into one CSV
# file and one plain-text log. Nothing has to be typed while it runs, and the
# example itself ends by its own watchdog if it ever gets stuck.
#
# Usage:
#   tools/latency/run-local-latency.sh [options]
#
#   -b, --bin=<path>     The 31_loclatency executable. Auto-detected if omitted.
#   -m, --modes=<list>   Mode list passed to the example. Default: all
#   -c, --count=<n>      Measured messages per run.        Default: 100000
#   -w, --warmup=<n>     Warm-up messages per run.         Default: 10000
#   -r, --repeat=<n>     Repetitions of the whole list.    Default: 3
#   -l, --label=<text>   Free text stamped into every CSV line. Default: baseline
#   -o, --out=<dir>      Output folder. Default: product/tasks/measurements
#   -h, --help           Print this text.
#
# Exit codes: 0 all runs finished, 2 at least one run got stuck or failed.
# ===========================================================================

set -u

REPO_ROOT="$(cd "$(dirname "$(realpath "$0")")/../.." && pwd)"

BIN=""
MODES="all"
COUNT=100000
WARMUP=10000
REPEAT=3
LABEL="baseline"
OUTDIR="$REPO_ROOT/product/tasks/measurements"

for arg in "$@"; do
    case "$arg" in
        -b=*|--bin=*)    BIN="${arg#*=}" ;;
        -m=*|--modes=*)  MODES="${arg#*=}" ;;
        -c=*|--count=*)  COUNT="${arg#*=}" ;;
        -w=*|--warmup=*) WARMUP="${arg#*=}" ;;
        -r=*|--repeat=*) REPEAT="${arg#*=}" ;;
        -l=*|--label=*)  LABEL="${arg#*=}" ;;
        -o=*|--out=*)    OUTDIR="${arg#*=}" ;;
        -h|--help)       sed -n '2,25p' "$0"; exit 0 ;;
        *) echo "ERROR: unknown option '$arg'. Use --help."; exit 1 ;;
    esac
done

# Find the newest built executable when none was given. Folders that hold a build made for
# a different purpose are skipped: the AREG_LATENCY_TRACE build lives in
# product/build/latency-trace and is slower on purpose, so picking it by accident would
# silently turn a baseline into a traced measurement.
if [ -z "$BIN" ]; then
    BIN=$(find "$REPO_ROOT/product/build" -name '31_loclatency*' -type f -perm -u+x 2>/dev/null \
          | grep -v '/latency-trace/' \
          | xargs -r ls -1t 2>/dev/null | head -n 1)
fi

# A traced binary prints its per-stage table at exit. Refuse it rather than produce numbers
# that cannot be compared with anything.
case "$BIN" in
    */latency-trace/*)
        echo "ERROR: '$BIN' is an AREG_LATENCY_TRACE build. Its numbers are not a baseline."
        echo "       Pass --bin=<path> with a normal build."
        exit 1
        ;;
esac

if [ -z "$BIN" ] || [ ! -x "$BIN" ]; then
    echo "ERROR: 31_loclatency was not found. Build it first, or pass --bin=<path>."
    echo "       cmake -B build -DAREG_EXAMPLES=ON && cmake --build build --target 31_loclatency"
    exit 1
fi

mkdir -p "$OUTDIR"
STAMP=$(date +%Y%m%d-%H%M%S)
CSV="$OUTDIR/$STAMP-loclatency-$LABEL.csv"
LOG="$OUTDIR/$STAMP-loclatency-$LABEL.log"

{
    echo "==========================================================================="
    echo "Example 31 -- local latency benchmark"
    echo "started    : $(date -Is)"
    echo "executable : $BIN"
    echo "label      : $LABEL"
    echo "modes      : $MODES"
    echo "count      : $COUNT   warmup: $WARMUP   repeat: $REPEAT"
    echo "host       : $(uname -srm)"
    echo "cpu        : $(grep -m1 'model name' /proc/cpuinfo 2>/dev/null | cut -d: -f2- | sed 's/^ //')"
    if grep -qi microsoft /proc/version 2>/dev/null; then
        echo "WARNING    : this is WSL. Thread wake-up is far slower here than on the"
        echo "             same machine booted natively. Use these numbers to follow a"
        echo "             trend, never to state an absolute latency."
    fi
    echo "==========================================================================="
    echo
} | tee "$LOG"

STATUS=0
for TOPO in same cross; do
    echo "--- topology: $TOPO ---" | tee -a "$LOG"
    "$BIN" -t="$TOPO" -m="$MODES" -c="$COUNT" -w="$WARMUP" -r="$REPEAT" \
           -o="$CSV" -l="$LABEL" 2>&1 | tee -a "$LOG"
    RC=${PIPESTATUS[0]}
    if [ "$RC" -ne 0 ]; then
        echo "ERROR: topology '$TOPO' ended with exit code $RC." | tee -a "$LOG"
        STATUS=2
    fi
    echo | tee -a "$LOG"
done

{
    echo "finished   : $(date -Is)"
    echo "csv        : $CSV"
    echo "log        : $LOG"
} | tee -a "$LOG"

exit $STATUS
