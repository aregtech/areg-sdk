#!/usr/bin/env bash
# ===========================================================================
# Areg SDK - GCC coverage of framework/, counted like Codecov
# ---------------------------------------------------------------------------
# Builds an instrumented Debug tree, runs the unit tests and the example
# scenarios, and writes the reports that .github/workflows/coverage.yml
# uploads. The workflow calls this script step by step, so a local run and
# the CI run use the same flags and filters.
#
#   Usage:  tools/intern/coverage.sh <step> [options]
#
#   Steps
#     sync      Copy the working tree to <dir>/src (skipped with --in-place).
#     build     Configure and build the instrumented tree.
#     unit      Run ctest, collect unit.json, reset the counters.
#     examples  Run the example scenarios, collect examples.json, reset the counters.
#     report    Write the Cobertura, HTML and Markdown reports and print them. The
#               first tables count like Codecov: a line once, and a hit line with
#               an untaken branch is partial, which is not covered. The gcovr
#               lines, functions and branches follow.
#     all       sync, build, unit, examples, report. With --only: sync, build,
#               unit, report.
#
#   Options
#     --dir <path>    Work directory (default: $AREG_COV_DIR, else ~/areg-gcov).
#                     Holds src/, build/, report/ and last.json.
#     --in-place      Measure the tree this script belongs to; no copy (CI).
#     --build <path>  Build directory (default: <dir>/build).
#     --out <path>    Report directory (default: <dir>/report).
#     --only <list>   unit: a ctest regular expression; examples: scenario names,
#                     comma separated. The counters are added to the last full run
#                     of that step, and the report does not update last.json until
#                     a full run of that step replaces them.
#     --files <regex> report: also list the files matching the expression.
#     --top N         report: list the N files with most lines not hit (default: 25).
#     --jobs N        Build jobs (default: 6).
#
#   Environment
#     GCOVR           gcovr executable (default: gcovr from PATH, else ~/.local/bin/gcovr).
#     PYTHON          Python interpreter (default: python3).
#     GITHUB_STEP_SUMMARY  When set, the report is appended to it.
#
#   Example: a full run, then a quick re-run of new tests
#     tools/intern/coverage.sh all
#     tools/intern/coverage.sh all --only 'SocketTest' --files 'base/private/Socket'
#
#   Build on a Linux file system: a WSL build on /mnt/c fails under load.
# ===========================================================================
set -u

REPO=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
STEP=${1:-help}
[ $# -gt 0 ] && shift

DIR=${AREG_COV_DIR:-$HOME/areg-gcov}
IN_PLACE=0
BUILD=""
OUT=""
ONLY=""
FILES=""
TOP=25
JOBS=6

die() { echo "coverage.sh: $*" >&2; exit 2; }
absolute() { case "$1" in /*) echo "$1" ;; *) echo "$PWD/$1" ;; esac; }

while [ $# -gt 0 ]; do
  case "$1" in
    --dir)      DIR=$(absolute "${2:?}"); shift 2 ;;
    --in-place) IN_PLACE=1; shift ;;
    --build)    BUILD=$(absolute "${2:?}"); shift 2 ;;
    --out)      OUT=$(absolute "${2:?}"); shift 2 ;;
    --only)     ONLY=${2:?}; shift 2 ;;
    --files)    FILES=${2:?}; shift 2 ;;
    --top)      TOP=${2:?}; shift 2 ;;
    --jobs)     JOBS=${2:?}; shift 2 ;;
    *) die "unknown option '$1'" ;;
  esac
done

if [ "$IN_PLACE" = 1 ]; then SRC=$REPO; else SRC=$DIR/src; fi
BUILD=${BUILD:-$DIR/build}
OUT=${OUT:-$DIR/report}
LAST=$DIR/last.json
PYTHON=${PYTHON:-python3}
if [ -z "${GCOVR:-}" ]; then
  GCOVR=$(command -v gcovr || echo "$HOME/.local/bin/gcovr")
fi
NPROC=$(nproc 2>/dev/null || echo 4)

# A Lock or ScopedLock that could not take its object ends the test process.
export AREG_LOCK_FAILURE_FATAL=${AREG_LOCK_FAILURE_FATAL:-1}

# The counters of one run: unit.json or examples.json, or the *-only.json of --only.
tracefile() { if [ -n "$ONLY" ]; then echo "$OUT/$1-only.json"; else echo "$OUT/$1.json"; fi; }

reset_counters() { find "$BUILD" -name '*.gcda' -delete; }

check_gcovr() {
  [ -x "$GCOVR" ] || command -v "$GCOVR" >/dev/null || die "gcovr not found: pip install gcovr==8.8"
  local version
  version=$("$GCOVR" --version | head -1)
  case "$version" in *" 8.8"*) ;; *) echo "coverage.sh: warning: CI uses gcovr 8.8, found '$version'" >&2 ;; esac
}

collect() {
  check_gcovr
  mkdir -p "$OUT"
  ( cd "$SRC" && "$GCOVR" --root . --filter 'framework/' -j "$NPROC" \
      --exclude-unreachable-branches --exclude-throw-branches \
      --json "$1" "$BUILD" ) || die "gcovr failed to collect $1"
}

step_sync() {
  [ "$IN_PLACE" = 1 ] && return 0
  [ "$SRC" = "$REPO" ] && return 0
  mkdir -p "$SRC"
  # Tracked and untracked files that git does not ignore; mtimes are kept for incremental builds.
  ( cd "$REPO" && git ls-files -z --cached --others --exclude-standard ) > "$DIR/sync.list" \
    || die "git ls-files failed in $REPO"
  rsync -rlt --ignore-missing-args --from0 --files-from="$DIR/sync.list" "$REPO/" "$SRC/" || die "rsync failed"
  # Files removed from the working tree are removed from the copy.
  ( cd "$SRC" && find . -type f -printf '%P\0' ) | LC_ALL=C sort -z > "$DIR/copy.list"
  LC_ALL=C sort -z "$DIR/sync.list" | LC_ALL=C comm -z -23 "$DIR/copy.list" - \
    | ( cd "$SRC" && xargs -0 -r rm -f )
  echo "sync: $REPO -> $SRC"
}

step_build() {
  # Debug is the only unoptimized configuration, so each line maps to its own counter.
  # '-fprofile-update=atomic' keeps the counters exact when several threads run one line.
  local flags="--coverage -fprofile-update=atomic"
  cmake -S "$SRC" -B "$BUILD" -DAREG_COMPILER_FAMILY=gnu -DCMAKE_BUILD_TYPE=Debug -DAREG_LIB_TYPE=shared \
        -DAREG_EXTENDED:BOOL=ON -DAREG_LOGGING:BOOL=ON -DAREG_EXAMPLES:BOOL=ON -DAREG_TESTS:BOOL=ON \
        -DAREG_OUTPUT_LAYOUT:BOOL=OFF \
        -DCMAKE_C_FLAGS="$flags" -DCMAKE_CXX_FLAGS="$flags" \
        -DCMAKE_EXE_LINKER_FLAGS="--coverage" -DCMAKE_SHARED_LINKER_FLAGS="--coverage" \
    || die "configure failed"
  cmake --build "$BUILD" -j "$JOBS" || die "build failed"
}

step_unit() {
  local rc
  reset_counters
  if [ -n "$ONLY" ]; then
    ctest --test-dir "$BUILD" --timeout 300 --output-on-failure -R "$ONLY"
  else
    ctest --test-dir "$BUILD" --timeout 300 --output-on-failure
  fi
  rc=$?
  collect "$(tracefile unit)"
  [ -z "$ONLY" ] && rm -f "$OUT/unit-only.json"
  reset_counters
  return $rc
}

step_examples() {
  local rc
  reset_counters
  local args=(--bin-dir "$BUILD/bin" --tier all --out-dir "$OUT/examples" --summary '')
  [ -n "$ONLY" ] && args+=(--only "$ONLY")
  "$PYTHON" "$SRC/tools/intern/run-all-examples.py" "${args[@]}"
  rc=$?
  collect "$(tracefile examples)"
  [ -z "$ONLY" ] && rm -f "$OUT/examples-only.json"
  reset_counters
  return $rc
}

step_report() {
  check_gcovr
  local flag adds=() summary=()
  [ -n "${GITHUB_STEP_SUMMARY:-}" ] && summary=(--summary "$GITHUB_STEP_SUMMARY")
  cd "$SRC" || die "no source tree $SRC"
  # One Cobertura file per Codecov flag, as uploaded.
  for flag in unit examples; do
    [ -f "$OUT/$flag.json" ] || die "missing $OUT/$flag.json: run the '$flag' step first"
    "$GCOVR" --root . --json-add-tracefile "$OUT/$flag.json" \
             --cobertura "$OUT/$flag.xml" --json-summary "$OUT/$flag-summary.json" \
      || die "gcovr failed on $flag.json"
    adds+=(--json-add-tracefile "$OUT/$flag.json")
  done
  for flag in unit examples; do
    [ -f "$OUT/$flag-only.json" ] && adds+=(--json-add-tracefile "$OUT/$flag-only.json")
  done
  mkdir -p "$OUT/html"
  "$GCOVR" --root . "${adds[@]}" \
           --html-nested "$OUT/html/index.html" --html-title 'Areg SDK framework coverage' \
           --json-summary "$OUT/total-summary.json" --txt-summary \
    || die "gcovr failed on the HTML report"
  # Codecov reports a line once, so the template instances and constructor variants are merged.
  local flag_adds
  for flag in unit examples; do
    flag_adds=(--json-add-tracefile "$OUT/$flag.json")
    [ -f "$OUT/$flag-only.json" ] && flag_adds+=(--json-add-tracefile "$OUT/$flag-only.json")
    "$GCOVR" --root . --merge-lines "${flag_adds[@]}" --cobertura "$OUT/$flag-rule.xml" \
      || die "gcovr failed on $flag-rule.xml"
  done
  "$GCOVR" --root . --merge-lines "${adds[@]}" --cobertura "$OUT/total.xml" \
    || die "gcovr failed on total.xml"
  local rule=(--last "$LAST" --top "$TOP")
  [ -z "$ONLY" ] && ! ls "$OUT"/*-only.json >/dev/null 2>&1 && rule+=(--save)
  [ -n "$FILES" ] && rule+=(--files "$FILES")
  mkdir -p "$DIR"
  "$PYTHON" "$SRC/tools/intern/coverage-report.py" --unit "$OUT/unit-summary.json" \
           --examples "$OUT/examples-summary.json" --total "$OUT/total-summary.json" \
           --cobertura "$OUT/total.xml" --unit-cobertura "$OUT/unit-rule.xml" \
           --examples-cobertura "$OUT/examples-rule.xml" --root . \
           "${rule[@]}" ${summary[@]+"${summary[@]}"} > "$OUT/coverage.md" || die "coverage-report.py failed"
  cat "$OUT/coverage.md"
}

case "$STEP" in
  sync)     step_sync ;;
  build)    step_build ;;
  unit)     step_unit ;;
  examples) step_examples ;;
  report)   step_report ;;
  all)
    step_sync && step_build || exit 1
    RC=0
    step_unit || RC=1
    if [ -z "$ONLY" ]; then step_examples || RC=1; fi
    step_report || RC=1
    exit $RC ;;
  help|-h|--help) sed -n '3,/^# =====/{/^# =====/d;p;}' "$0" | sed 's/^# \{0,1\}//' ;;
  *) die "unknown step '$STEP' (sync, build, unit, examples, report, all)" ;;
esac
