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
#     unit      Run ctest, collect unit.json, reset the counters. The results go to
#               <out>/unit-junit.xml.
#     examples  Run the example scenarios, collect examples.json, reset the counters.
#               On Cygwin the processes write their counters into separate folders,
#               merged with gcov-tool before the collection.
#     report    Write the Cobertura and HTML reports and print the Markdown report
#               (to <out>/coverage.md too with --text). The first tables count like
#               Codecov: a line once, and a hit line with an untaken branch is
#               partial, which is not covered. The critical components
#               (coverage-critical.json), the gcovr lines, functions and branches,
#               and the edge cases of tests/edge-cases.json against unit-junit.xml follow.
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
#     --text          report: also write the Markdown report to <out>/coverage.md.
#     --jobs N        Build jobs (default: 6; 'auto': the number of CPUs).
#
#   Environment
#     GCOVR           gcovr executable (default: gcovr from PATH, else ~/.local/bin/gcovr).
#     PYTHON          Python interpreter (default: python3).
#     AREG_COV_FAMILY    Compiler family passed to cmake (default: gnu; macOS: llvm; Cygwin: cygwin).
#     AREG_COV_GCOV      gcov program gcovr runs (default: gcov of the compiler;
#                        macOS: "xcrun llvm-cov gcov"; Windows: "llvm-cov gcov").
#     AREG_COV_GENERATOR cmake generator (default: the cmake default; Windows: Ninja).
#     AREG_COV_FLAGS     compile flags (default: "--coverage -fprofile-update=atomic -fno-exceptions";
#                        macOS: the same; clang-cl:
#                        "/EHs-c- /D_HAS_EXCEPTIONS=0 /GR /clang:--coverage /clang:-fprofile-update=atomic").
#     AREG_COV_LOG_TESTS ctest regular expression of the tests the unit step runs a second
#                        time with logging started and every scope at DEBUG | SCOPE, so the
#                        log lines of the paths only those tests reach count (default: the
#                        component, IPC and router tests; empty: no second run).
#     AREG_COV_LDFLAGS   link flags of executables and shared libraries (default: "--coverage";
#                        clang-cl: the quoted full path of the clang_rt.profile library).
#     GITHUB_STEP_SUMMARY  When set, the report is appended to it.
#
#   Example: a full run, then a quick re-run of new tests
#     tools/intern/coverage.sh all
#     tools/intern/coverage.sh all --only 'SocketTest' --files 'base/private/Socket'
#
#   Build on a Linux file system: a WSL build on /mnt/c fails under load.
# ===========================================================================
set -u

# The bash of Git for Windows sets OSTYPE to cygwin as well, so Cygwin is told by uname.
case "$(uname -s)" in CYGWIN*) CYGWIN=1 ;; *) CYGWIN=0 ;; esac

# A Cygwin bash started from Git for Windows has the Git tools first in PATH, and they read
# /cygdrive/... as a path below the Git directory. 'mkdir' then succeeds in the wrong place.
[ "$CYGWIN" = 1 ] && export PATH="/usr/local/bin:/usr/bin:$PATH"

# 'pwd -W' (Git for Windows) gives the drive form that native tools read; elsewhere it is 'pwd'.
cwd() { pwd -W 2>/dev/null || pwd; }

REPO=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && cwd)
STEP=${1:-help}
[ $# -gt 0 ] && shift

IN_PLACE=0
BUILD=""
OUT=""
ONLY=""
FILES=""
TOP=25
JOBS=6
TEXT=0

die() { echo "coverage.sh: $*" >&2; exit 2; }
absolute() {
  if pwd -W >/dev/null 2>&1; then
    cygpath -am "$1"
  else
    case "$1" in /*) echo "$1" ;; *:*) echo "$1" ;; *) echo "$(cwd)/${1#./}" ;; esac
  fi
}

DIR=$(absolute "${AREG_COV_DIR:-$HOME/areg-gcov}") || die "cannot resolve the work directory"

while [ $# -gt 0 ]; do
  case "$1" in
    --dir)      DIR=$(absolute "${2:?}") || die "cannot resolve the work directory"; shift 2 ;;
    --in-place) IN_PLACE=1; shift ;;
    --build)    BUILD=$(absolute "${2:?}") || die "cannot resolve the build directory"; shift 2 ;;
    --out)      OUT=$(absolute "${2:?}") || die "cannot resolve the report directory"; shift 2 ;;
    --only)     ONLY=${2:?}; shift 2 ;;
    --files)    FILES=${2:?}; shift 2 ;;
    --top)      TOP=${2:?}; shift 2 ;;
    --jobs)     JOBS=${2:?}; shift 2 ;;
    --text)     TEXT=1; shift ;;
    *) die "unknown option '$1'" ;;
  esac
done

[ "$JOBS" = auto ] && JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

if [ "$IN_PLACE" = 1 ]; then SRC=$REPO; else SRC=$DIR/src; fi
BUILD=${BUILD:-$DIR/build}
OUT=${OUT:-$DIR/report}
LAST=$DIR/last.json
PYTHON=${PYTHON:-python3}
if [ -z "${GCOVR:-}" ]; then
  GCOVR=$(command -v gcovr || echo "$HOME/.local/bin/gcovr")
fi
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
FAMILY=${AREG_COV_FAMILY:-gnu}
GCOV=${AREG_COV_GCOV:-}
GENERATOR=${AREG_COV_GENERATOR:-}
COV_FLAGS=${AREG_COV_FLAGS:---coverage -fprofile-update=atomic -fno-exceptions}
COV_LDFLAGS=${AREG_COV_LDFLAGS:---coverage}
LOG_TESTS=${AREG_COV_LOG_TESTS-^(ComponentLifecycleFailureTest|DispatcherLifecycleTest|ProxyBaseContractTest|StubBaseContractTest|IpcSocketCoverageTest|IpcClientCoverageTest|RouterApplicationTest|ServiceCommServerTest|ServiceCommConnectionTest|areg-system-lifecycle-test|areg-request-failure-test|areg-request-error-test|areg-stub-teardown-test)[.]}

# A Lock or ScopedLock that could not take its object ends the test process.
export AREG_LOCK_FAILURE_FATAL=${AREG_LOCK_FAILURE_FATAL:-1}

# The counters of one run: unit.json or examples.json, or the *-only.json of --only.
tracefile() { if [ -n "$ONLY" ]; then echo "$OUT/$1-only.json"; else echo "$OUT/$1.json"; fi; }

# GCC on Linux: _Exit and abort write the counters too (gcov-exit-flush.c), preloaded in the runs.
FLUSH=$BUILD/gcov-exit-flush.so
build_flush() {
  [ "$FAMILY" = gnu ] && [ "$(uname -s)" = Linux ] || return 0
  "${CC:-cc}" -shared -fPIC -O1 -o "$FLUSH" "$SRC/tools/intern/gcov-exit-flush.c" -lgcov \
    || die "cannot build $FLUSH"
}
preload_flush() { [ -f "$FLUSH" ] && export LD_PRELOAD="$FLUSH${LD_PRELOAD:+:$LD_PRELOAD}"; return 0; }

# Writes the configuration of the logging run: the areg.init of the build, every scope at
# DEBUG | SCOPE, the file target only, the log files in the given directory.
write_log_config() {
  mkdir -p "$2" || return 1
  "$PYTHON" - "$BUILD/bin/config/areg.init" "$1" "$2" <<'PY'
import re, sys
source, target, folder = sys.argv[1:4]
replaced = re.compile(r'\s*log::[^:\s]+::(scope::|target\b|enable::(remote|file|db|debug)\b|file::location\b)')
with open(source, newline='') as file:
    kept = [line for line in file.read().splitlines(True) if not replaced.match(line)]
settings = ('log::*::target = file ;\n'
            'log::*::enable::remote = false ;\n'
            'log::*::enable::db = false ;\n'
            'log::*::enable::debug = false ;\n'
            'log::*::enable::file = true ;\n'
            'log::*::file::location = %s/%%appname%%_%%time%%.log ;\n'
            'log::*::scope::* = DEBUG | SCOPE ;\n' % folder.replace('\\', '/'))
with open(target, 'w', newline='') as file:
    file.write(''.join(kept) + '\n' + settings)
PY
}

reset_counters() { ( cd "$BUILD" && find . -name '*.gcda' -delete ) || die "cannot reset the counters in $BUILD"; }

# On Cygwin every running example process writes its counters below its own slot folder,
# and gcov-tool merges the slots into the build after the run.
GCOV_TOOL=gcov-tool
split_counters() { [ "$CYGWIN" = 1 ]; }

# Creates the slot folder $1 with dirs.txt, the object folders of the build.
split_prepare() {
  rm -rf "$1" && mkdir -p "$1" || die "cannot create $1"
  ( cd "$BUILD" && find . -name '*.gcno' -printf '%h\n' | LC_ALL=C sort -u ) > "$1/dirs.txt" \
    || die "cannot list the object folders of $BUILD"
  command -v "$GCOV_TOOL" >/dev/null || die "$GCOV_TOOL not found"
}

# Runs gcov-tool merge, which applies GCOV_PREFIX and GCOV_PREFIX_STRIP to the files it writes.
# A folder it cannot write is reported as 'Skip' with exit code 0.
split_merge_two() {
  local log
  log=$(env -u GCOV_PREFIX -u GCOV_PREFIX_STRIP "$GCOV_TOOL" merge -o "$1" "$2" "$3" 2>&1) \
    && ! printf '%s' "$log" | grep -q 'Skip' \
    || die "$GCOV_TOOL merge -o $1 $2 $3 failed: $log"
}

# Merges the slots below $1 into the build and removes them.
split_merge() {
  local split=$1 acc="" slot next n=0
  for slot in "$split"/slot-*; do
    [ -d "$slot" ] || continue
    if [ -z "$acc" ]; then acc=$slot; continue; fi
    next="$split/merged-$n"; n=$((n + 1))
    mkdir -p "$next" && ( cd "$next" && xargs -r mkdir -p < "$split/dirs.txt" ) || die "cannot create $next"
    split_merge_two "$next" "$slot" "$acc"
    rm -rf "$slot" "$acc"
    acc=$next
  done
  [ -n "$acc" ] || return 0
  find "$acc" -name '*.gcda' | grep -q . || die "no process wrote counters below $split"
  split_merge_two "$BUILD" "$acc" "$BUILD"
  rm -rf "$split"
}

check_gcovr() {
  [ -x "$GCOVR" ] || command -v "$GCOVR" >/dev/null || die "gcovr not found: pip install gcovr==8.8"
  local version
  version=$("$GCOVR" --version | head -1)
  case "$version" in *" 8.8"*) ;; *) echo "coverage.sh: warning: CI uses gcovr 8.8, found '$version'" >&2 ;; esac
}

collect() {
  check_gcovr
  mkdir -p "$OUT"
  local gcov=()
  [ -n "$GCOV" ] && gcov=(--gcov-executable "$GCOV")
  ( cd "$SRC" && "$GCOVR" --root . --filter 'framework/' -j "$NPROC" ${gcov[@]+"${gcov[@]}"} \
      --exclude-unreachable-branches --exclude-throw-branches \
      --exclude-branches-by-pattern '^\s*ASSERT(_MSG)?\s*\(' \
      --exclude-lines-by-pattern '^\s*ASSERT(_MSG)?\s*\(\s*false\b.*' \
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
  # OUTPUT_DEBUG builds the debug log target.
  local flags="$COV_FLAGS -DOUTPUT_DEBUG=1" gen=()
  [ -n "$GENERATOR" ] && gen=(-G "$GENERATOR")
  cmake -S "$SRC" -B "$BUILD" ${gen[@]+"${gen[@]}"} -DAREG_COMPILER_FAMILY="$FAMILY" -DCMAKE_BUILD_TYPE=Debug -DAREG_LIB_TYPE=shared \
        -DAREG_LOGGING:BOOL=ON -DAREG_EXAMPLES:BOOL=ON -DAREG_TESTS:BOOL=ON \
        -DAREG_OUTPUT_LAYOUT:BOOL=OFF \
        -DCMAKE_C_FLAGS="$flags" -DCMAKE_CXX_FLAGS="$flags" \
        -DCMAKE_EXE_LINKER_FLAGS="$COV_LDFLAGS" -DCMAKE_SHARED_LINKER_FLAGS="$COV_LDFLAGS" \
    || die "configure failed"
  cmake --build "$BUILD" -j "$JOBS" || die "build failed"
  build_flush
}

step_unit() {
  local rc
  check_gcovr
  reset_counters
  ( preload_flush
  if [ -n "$ONLY" ]; then
    ctest --test-dir "$BUILD" --timeout 300 --output-on-failure --output-junit "$OUT/unit-junit.xml" -R "$ONLY"
  else
    ctest --test-dir "$BUILD" --timeout 300 --output-on-failure --output-junit "$OUT/unit-junit.xml"
  fi )
  rc=$?
  if [ -z "$ONLY" ] && [ -n "$LOG_TESTS" ]; then
    write_log_config "$OUT/unit-logs.init" "$OUT/unit-logs" || die "cannot write the logging configuration"
    ( preload_flush
      AREG_TEST_LOG_CONFIG="$OUT/unit-logs.init" ctest --test-dir "$BUILD" --timeout 300 --output-on-failure \
          --output-junit "$OUT/unit-logs-junit.xml" -R "$LOG_TESTS" ) || rc=1
    rm -rf "$OUT/unit-logs"
  fi
  collect "$(tracefile unit)"
  [ -z "$ONLY" ] && rm -f "$OUT/unit-only.json"
  reset_counters
  return $rc
}

step_examples() {
  local rc
  check_gcovr
  reset_counters
  local args=(--bin-dir "$BUILD/bin" --tier coverage --out-dir "$OUT/examples" --summary '')
  [ -n "$ONLY" ] && args+=(--only "$ONLY")
  local split="$OUT/gcda-split" strip=""
  if split_counters; then
    split_prepare "$split"
    args+=(--gcov-split "$split")
    strip=$(printf '%s' "${BUILD%/}" | tr -cd / | wc -c)
  fi
  ( preload_flush
    [ -n "$strip" ] && export GCOV_PREFIX_STRIP=$strip
    "$PYTHON" "$SRC/tools/intern/run-all-examples.py" "${args[@]}" )
  rc=$?
  split_counters && split_merge "$split"
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
  # Every uploaded path is repository-relative; /cygdrive/..., C:/... and ../ would split a file in Codecov.
  local bad
  bad=$(grep -ho 'filename="[^"]*"' "$OUT/unit.xml" "$OUT/examples.xml" | grep -E 'filename="(/|[A-Za-z]:|\.\.|[^"]*\\)' | head -3)
  [ -z "$bad" ] || die "Cobertura paths are not repository-relative: $bad"
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
  local rule=(--last "$LAST" --top "$TOP" --build "$BUILD")
  [ -z "$ONLY" ] && ! ls "$OUT"/*-only.json >/dev/null 2>&1 && rule+=(--save)
  [ -n "$FILES" ] && rule+=(--files "$FILES")
  [ "$TEXT" = 1 ] && rule+=(--text "$OUT/coverage.md")
  rm -f "$OUT/coverage.md"
  mkdir -p "$DIR"
  "$PYTHON" "$SRC/tools/intern/coverage-report.py" --unit "$OUT/unit-summary.json" \
           --examples "$OUT/examples-summary.json" --total "$OUT/total-summary.json" \
           --cobertura "$OUT/total.xml" --unit-cobertura "$OUT/unit-rule.xml" \
           --examples-cobertura "$OUT/examples-rule.xml" --root . \
           "${rule[@]}" ${summary[@]+"${summary[@]}"} || die "coverage-report.py failed"
  # The edge cases of the critical algorithms, against the results of the unit step.
  if [ -f "$OUT/unit-junit.xml" ]; then
    local edges=(--junit "$OUT/unit-junit.xml")
    [ "$TEXT" = 1 ] && edges+=(--text "$OUT/edge-cases.md")
    "$PYTHON" "$SRC/tools/intern/check-edge-cases.py" "${edges[@]}" | tee -a "${GITHUB_STEP_SUMMARY:-/dev/null}"
  fi
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
