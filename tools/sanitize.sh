#!/usr/bin/env bash
# ===========================================================================
# Areg SDK - sanitizer & profiler driver
# ---------------------------------------------------------------------------
# Configures an isolated CMake build that instruments the framework with a
# compiler sanitizer (or wires a binary into a profiler), builds the chosen
# target and runs it.  Nothing in the normal build tree or the build system
# files is touched: every mode uses its own throw-away build directory and
# passes flags on the CMake command line only.
#
# Sanitizers ship with the compiler (GCC/Clang) - no package install needed.
# Profilers (perf, heaptrack, valgrind) are optional system tools; the script
# detects them and tells you the exact apt line if one is missing.
#
#   Usage:  tools/sanitize.sh <mode> [options] [-- <args passed to the binary>]
#
#   Modes
#     asan        AddressSanitizer + LeakSanitizer + UndefinedBehavior (memory
#                 errors, use-after-free/scope, leaks, UB).  Primary tool.
#     tsan        ThreadSanitizer (data races, lock-order inversions).
#                 Mutually exclusive with asan - separate build.
#     ubsan       UndefinedBehaviorSanitizer only (cheap; no ASan slowdown).
#     build-only  Configure + build the instrumented target, do not run.
#     perf        Run <target binary> under `perf stat`/`perf record` (CPU,
#                 cache misses, context switches) - latency/throughput work.
#     heaptrack   Run <target binary> under heaptrack (allocation profiling;
#                 peak RSS, alloc hot spots) - the memory-retention lens.
#     callgrind   Run <target binary> under valgrind --tool=callgrind.
#     help        This text.
#
#   Options
#     --compiler gnu|llvm   Toolchain family (default: gnu).
#     --target  <name>      CMake target to build (default: areg-unit-tests).
#     --run     <what>      What to execute after build:
#                             ctest      -> ctest in the build dir (default for tests)
#                             <path>     -> an explicit binary path
#                             auto       -> locate the built target binary (default)
#                             none       -> build only
#     --lib     shared|static  Library type (default: static - robust for ASan).
#     --examples            Also build examples (default: off, tests only).
#     --jobs N              Parallel build jobs (default: nproc).
#     --keep                Reuse an existing build dir (skip reconfigure).
#     --                    Everything after is forwarded to the run binary.
#
#   Examples
#     tools/sanitize.sh asan                         # build+run unit tests under ASan/LSan/UBSan
#     tools/sanitize.sh tsan --run ctest             # data-race scan over the test suite
#     tools/sanitize.sh asan --target 23_pubclient --examples -- --some-arg
#     tools/sanitize.sh heaptrack --target mtrouter --examples -- -e
# ===========================================================================

set -euo pipefail

# --------------------------------------------------------------------------
# Locate the repository root (this script lives in <root>/tools/).
# --------------------------------------------------------------------------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
SUPP_DIR="${SCRIPT_DIR}/sanitizer"

# --------------------------------------------------------------------------
# Colours (disabled when stdout is not a TTY).
# --------------------------------------------------------------------------
if [[ -t 1 ]]; then
    C_RED=$'\033[31m'; C_GRN=$'\033[32m'; C_YEL=$'\033[33m'; C_CYN=$'\033[36m'; C_BLD=$'\033[1m'; C_RST=$'\033[0m'
else
    C_RED=; C_GRN=; C_YEL=; C_CYN=; C_BLD=; C_RST=
fi
info() { printf '%s==>%s %s\n' "${C_CYN}${C_BLD}" "${C_RST}" "$*"; }
warn() { printf '%s[warn]%s %s\n' "${C_YEL}" "${C_RST}" "$*" >&2; }
die()  { printf '%s[error]%s %s\n' "${C_RED}" "${C_RST}" "$*" >&2; exit 1; }

# --------------------------------------------------------------------------
# Defaults.
# --------------------------------------------------------------------------
MODE=""
COMPILER="gnu"
TARGET="areg-unit-tests"
RUN="auto"
LIBTYPE="static"
EXAMPLES="OFF"
JOBS="$(nproc 2>/dev/null || echo 4)"
KEEP=0
RUN_ARGS=()

# --------------------------------------------------------------------------
# Parse arguments.
# --------------------------------------------------------------------------
[[ $# -ge 1 ]] || { sed -n '2,60p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'; exit 1; }
MODE="$1"; shift
while [[ $# -gt 0 ]]; do
    case "$1" in
        --compiler) COMPILER="$2"; shift 2;;
        --target)   TARGET="$2";   shift 2;;
        --run)      RUN="$2";      shift 2;;
        --lib)      LIBTYPE="$2";  shift 2;;
        --examples) EXAMPLES="ON"; shift;;
        --jobs)     JOBS="$2";     shift 2;;
        --keep)     KEEP=1;        shift;;
        --)         shift; RUN_ARGS=("$@"); break;;
        *) die "Unknown option: $1 (run 'tools/sanitize.sh help')";;
    esac
done

case "${MODE}" in
    help|-h|--help) sed -n '2,60p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'; exit 0;;
esac

# --------------------------------------------------------------------------
# Map compiler family to CMake variables.
# --------------------------------------------------------------------------
case "${COMPILER}" in
    gnu)  CC_BIN="gcc";   CXX_BIN="g++";     FAMILY="gnu";;
    llvm) CC_BIN="clang"; CXX_BIN="clang++"; FAMILY="llvm";;
    *) die "Unknown --compiler '${COMPILER}' (use gnu or llvm)";;
esac
command -v "${CXX_BIN}" >/dev/null 2>&1 || die "Compiler '${CXX_BIN}' not found in PATH."

# --------------------------------------------------------------------------
# Sanitizer flag sets.
#   -O1, frame pointers and no tail-call merging give readable, accurate
#   stacks without hiding frames.  -g keeps line info.
# --------------------------------------------------------------------------
COMMON_DBG="-g -O1 -fno-omit-frame-pointer -fno-optimize-sibling-calls"
ASAN_FLAGS="-fsanitize=address,undefined -fsanitize-address-use-after-scope ${COMMON_DBG}"
TSAN_FLAGS="-fsanitize=thread ${COMMON_DBG}"
UBSAN_FLAGS="-fsanitize=undefined -fno-sanitize-recover=all ${COMMON_DBG}"

# --------------------------------------------------------------------------
# Runtime option strings + suppression files.
# --------------------------------------------------------------------------
export ASAN_OPTIONS="detect_leaks=1:halt_on_error=0:abort_on_error=0:detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1:print_stats=1:log_threads=1"
export UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=0"
export TSAN_OPTIONS="second_deadlock_stack=1:history_size=7:halt_on_error=0"
[[ -f "${SUPP_DIR}/lsan.supp" ]] && export LSAN_OPTIONS="suppressions=${SUPP_DIR}/lsan.supp:print_suppressions=0"
[[ -f "${SUPP_DIR}/tsan.supp" ]] && export TSAN_OPTIONS="${TSAN_OPTIONS}:suppressions=${SUPP_DIR}/tsan.supp"
[[ -f "${SUPP_DIR}/ubsan.supp" ]] && export UBSAN_OPTIONS="${UBSAN_OPTIONS}:suppressions=${SUPP_DIR}/ubsan.supp"

# --------------------------------------------------------------------------
# Helper: configure + build an instrumented tree.
#   $1 build dir, $2 sanitizer flags.
# --------------------------------------------------------------------------
configure_build() {
    local build_dir="$1" flags="$2"
    if [[ ${KEEP} -eq 0 || ! -d "${build_dir}" ]]; then
        info "Configuring ${build_dir} (compiler=${FAMILY}, lib=${LIBTYPE}, examples=${EXAMPLES})"
        # AREG_OUTPUT_DIR keeps each sanitizer's binaries/libs in its own tree.
        # Without it every build emits to the same product/<compiler>-<cfg> path
        # and the asan / tsan executables clobber each other.
        cmake -B "${build_dir}" -S "${ROOT_DIR}" \
            -DAREG_COMPILER_FAMILY="${FAMILY}" \
            -DCMAKE_C_COMPILER="${CC_BIN}" \
            -DCMAKE_CXX_COMPILER="${CXX_BIN}" \
            -DCMAKE_BUILD_TYPE=Debug \
            -DAREG_LIB_TYPE="${LIBTYPE}" \
            -DAREG_TESTS=ON \
            -DAREG_EXAMPLES="${EXAMPLES}" \
            -DAREG_OUTPUT_DIR="${build_dir}/product" \
            -DCMAKE_C_FLAGS="${flags}" \
            -DCMAKE_CXX_FLAGS="${flags}" \
            -DCMAKE_EXE_LINKER_FLAGS="${flags}" \
            -DCMAKE_SHARED_LINKER_FLAGS="${flags}"
    else
        info "Reusing existing ${build_dir} (--keep)"
    fi
    info "Building target '${TARGET}' with ${JOBS} jobs"
    cmake --build "${build_dir}" -j"${JOBS}" --target "${TARGET}"
}

# --------------------------------------------------------------------------
# Helper: find the binary produced for ${TARGET} inside a build dir.
# --------------------------------------------------------------------------
locate_binary() {
    local build_dir="$1"
    # Each sanitizer build emits into <build_dir>/product/bin (see AREG_OUTPUT_DIR).
    local found
    found="$(find "${build_dir}/product" -type f -name "${TARGET}*" -perm -u+x 2>/dev/null \
             | grep -vE '\.(so|a|o)(\.|$)' | head -1 || true)"
    [[ -n "${found}" ]] || found="$(find "${build_dir}" -type f -name "${TARGET}*" -perm -u+x 2>/dev/null | grep -vE '\.(so|a|o)(\.|$)' | head -1 || true)"
    printf '%s' "${found}"
}

# --------------------------------------------------------------------------
# Helper: run whatever --run asks for.
# --------------------------------------------------------------------------
do_run() {
    local build_dir="$1"; local prefix=("${@:2}")
    case "${RUN}" in
        none) info "Run skipped (--run none)"; return 0;;
        ctest)
            info "Running ctest in ${build_dir}"
            ( cd "${build_dir}" && "${prefix[@]}" ctest --output-on-failure ) ;;
        auto)
            local bin; bin="$(locate_binary "${build_dir}")"
            [[ -n "${bin}" ]] || die "Could not locate the '${TARGET}' binary; pass --run <path>."
            info "Running ${bin} ${RUN_ARGS[*]:-}"
            "${prefix[@]}" "${bin}" "${RUN_ARGS[@]:-}";;
        *)
            [[ -x "${RUN}" ]] || die "--run '${RUN}' is not an executable path."
            info "Running ${RUN} ${RUN_ARGS[*]:-}"
            "${prefix[@]}" "${RUN}" "${RUN_ARGS[@]:-}";;
    esac
}

# --------------------------------------------------------------------------
# Helper: ensure an optional profiler tool exists, else explain how to get it.
# --------------------------------------------------------------------------
require_tool() {
    local tool="$1" apt_pkg="$2"
    command -v "${tool}" >/dev/null 2>&1 && return 0
    warn "'${tool}' is not installed."
    warn "Install it with:  sudo apt-get install ${apt_pkg}"
    die  "Profiler '${tool}' unavailable."
}

# ==========================================================================
# Dispatch.
# ==========================================================================
case "${MODE}" in
    asan)
        BUILD_DIR="${ROOT_DIR}/build-asan"
        configure_build "${BUILD_DIR}" "${ASAN_FLAGS}"
        info "ASAN_OPTIONS=${ASAN_OPTIONS}"
        [[ -n "${LSAN_OPTIONS:-}" ]] && info "LSAN_OPTIONS=${LSAN_OPTIONS}"
        do_run "${BUILD_DIR}"
        ;;
    tsan)
        BUILD_DIR="${ROOT_DIR}/build-tsan"
        configure_build "${BUILD_DIR}" "${TSAN_FLAGS}"
        info "TSAN_OPTIONS=${TSAN_OPTIONS}"
        # Modern kernels (Ubuntu 24.04 / WSL2: vm.mmap_rnd_bits=32) hand out address
        # layouts TSan's shadow memory cannot map -> "FATAL: unexpected memory mapping"
        # and the process aborts before running. Disabling ASLR for the child via
        # `setarch -R` fixes it without root. (Alternative: sudo sysctl vm.mmap_rnd_bits=28.)
        if command -v setarch >/dev/null 2>&1; then
            info "Disabling ASLR for the run (setarch -R) to satisfy TSan's shadow layout"
            do_run "${BUILD_DIR}" setarch "$(uname -m)" -R
        else
            warn "setarch not found; if you hit 'unexpected memory mapping', run: sudo sysctl vm.mmap_rnd_bits=28"
            do_run "${BUILD_DIR}"
        fi
        ;;
    ubsan)
        BUILD_DIR="${ROOT_DIR}/build-ubsan"
        configure_build "${BUILD_DIR}" "${UBSAN_FLAGS}"
        info "UBSAN_OPTIONS=${UBSAN_OPTIONS}"
        do_run "${BUILD_DIR}"
        ;;
    build-only)
        BUILD_DIR="${ROOT_DIR}/build-asan"
        RUN="none"
        configure_build "${BUILD_DIR}" "${ASAN_FLAGS}"
        ;;
    perf)
        require_tool perf linux-tools-common
        BUILD_DIR="${ROOT_DIR}/build-perf"
        # Profile an optimized build, not an instrumented one.
        if [[ ${KEEP} -eq 0 || ! -d "${BUILD_DIR}" ]]; then
            info "Configuring optimized build for profiling"
            cmake -B "${BUILD_DIR}" -S "${ROOT_DIR}" -DAREG_COMPILER_FAMILY="${FAMILY}" \
                -DCMAKE_C_COMPILER="${CC_BIN}" -DCMAKE_CXX_COMPILER="${CXX_BIN}" \
                -DCMAKE_BUILD_TYPE=RelWithDebInfo -DAREG_LIB_TYPE="${LIBTYPE}" \
                -DAREG_EXAMPLES="${EXAMPLES}" -DCMAKE_CXX_FLAGS="-fno-omit-frame-pointer"
        fi
        cmake --build "${BUILD_DIR}" -j"${JOBS}" --target "${TARGET}"
        BIN="$(locate_binary "${BUILD_DIR}")"; [[ -n "${BIN}" ]] || die "binary not found"
        info "perf stat ${BIN}"
        perf stat -d -d -- "${BIN}" "${RUN_ARGS[@]:-}"
        ;;
    heaptrack)
        require_tool heaptrack heaptrack
        BUILD_DIR="${ROOT_DIR}/build-perf"
        cmake -B "${BUILD_DIR}" -S "${ROOT_DIR}" -DAREG_COMPILER_FAMILY="${FAMILY}" \
            -DCMAKE_C_COMPILER="${CC_BIN}" -DCMAKE_CXX_COMPILER="${CXX_BIN}" \
            -DCMAKE_BUILD_TYPE=RelWithDebInfo -DAREG_LIB_TYPE="${LIBTYPE}" \
            -DAREG_EXAMPLES="${EXAMPLES}" >/dev/null
        cmake --build "${BUILD_DIR}" -j"${JOBS}" --target "${TARGET}"
        BIN="$(locate_binary "${BUILD_DIR}")"; [[ -n "${BIN}" ]] || die "binary not found"
        info "heaptrack ${BIN}  (analyse with: heaptrack_gui / heaptrack_print)"
        heaptrack "${BIN}" "${RUN_ARGS[@]:-}"
        ;;
    callgrind)
        require_tool valgrind valgrind
        BUILD_DIR="${ROOT_DIR}/build-perf"
        cmake -B "${BUILD_DIR}" -S "${ROOT_DIR}" -DAREG_COMPILER_FAMILY="${FAMILY}" \
            -DCMAKE_C_COMPILER="${CC_BIN}" -DCMAKE_CXX_COMPILER="${CXX_BIN}" \
            -DCMAKE_BUILD_TYPE=RelWithDebInfo -DAREG_LIB_TYPE="${LIBTYPE}" \
            -DAREG_EXAMPLES="${EXAMPLES}" >/dev/null
        cmake --build "${BUILD_DIR}" -j"${JOBS}" --target "${TARGET}"
        BIN="$(locate_binary "${BUILD_DIR}")"; [[ -n "${BIN}" ]] || die "binary not found"
        info "valgrind --tool=callgrind ${BIN}"
        valgrind --tool=callgrind --callgrind-out-file="${BUILD_DIR}/callgrind.out" -- "${BIN}" "${RUN_ARGS[@]:-}"
        ;;
    *)
        die "Unknown mode '${MODE}' (run 'tools/sanitize.sh help')"
        ;;
esac

info "${C_GRN}Done.${C_RST}"
