#!/usr/bin/env bash
# ===========================================================================
# Areg SDK - run all example applications as a smoke test (Linux / macOS).
# ---------------------------------------------------------------------------
# Launches every built example, makes sure it starts and does not crash, then
# runs the unit-test binary. Interactive examples that wait for the user to
# quit (e.g. typing '-q') are killed after a few seconds - having started
# cleanly is the pass criterion.
#
#   Rules:
#     * Examples whose name contains 'pub' are IPC examples and need 'mtrouter';
#       the router is started before the run and stopped afterwards.
#     * Other examples run standalone.
#     * Example 20 (winchat) is a Windows GUI app and is skipped.
#
#   Usage:  tools/run-all-examples.sh [bin-dir]
#     bin-dir   Directory holding the built binaries. Auto-detected under
#               <root>/product/build/*/*/bin when omitted.
#
#   Env:
#     RUN_SECONDS   Seconds to let an interactive example run before stopping
#                   it (default: 3).
#
#   Exit code: 0 if everything passed, 1 otherwise.
# ===========================================================================

set -u

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
RUN_SECONDS="${RUN_SECONDS:-3}"

if [ -t 1 ]; then
    C_RED=$'\033[31m'; C_GRN=$'\033[32m'; C_YEL=$'\033[33m'; C_CYN=$'\033[36m'; C_BLD=$'\033[1m'; C_RST=$'\033[0m'
else
    C_RED=; C_GRN=; C_YEL=; C_CYN=; C_BLD=; C_RST=
fi

# --------------------------------------------------------------------------
# Locate the binary output directory (first argument overrides auto-detect).
# --------------------------------------------------------------------------
BIN_DIR="${1:-${AREG_BIN_DIR:-}}"
if [ -z "${BIN_DIR}" ]; then
    # shellcheck disable=SC2012
    BIN_DIR="$(ls -dt "${ROOT_DIR}"/product/build/*/*/bin 2>/dev/null | head -n1)"
fi
if [ -z "${BIN_DIR}" ] || [ ! -d "${BIN_DIR}" ]; then
    printf '%s[error]%s binary directory not found - build the examples first or pass it as an argument.\n' "${C_RED}" "${C_RST}" >&2
    exit 1
fi
cd "${BIN_DIR}" || exit 1
printf '%s==>%s Binaries: %s\n' "${C_CYN}${C_BLD}" "${C_RST}" "${BIN_DIR}"

# --------------------------------------------------------------------------
# Resolve a binary by base name, trying every platform suffix.
# --------------------------------------------------------------------------
find_bin() {
    local name="$1" suffix
    for suffix in "" ".elf" ".mac" ".out"; do
        if [ -x "./${name}${suffix}" ]; then printf './%s%s' "${name}" "${suffix}"; return 0; fi
    done
    return 1
}

ROUTER="$(find_bin mtrouter || true)"
UNIT="$(find_bin areg-unit-tests || true)"

# --------------------------------------------------------------------------
# Router lifecycle (started once for the whole run; covers all IPC examples).
# --------------------------------------------------------------------------
ROUTER_PID=""
start_router() {
    [ -n "${ROUTER}" ] || { printf '%s[warn]%s mtrouter not found - IPC examples run without a router.\n' "${C_YEL}" "${C_RST}"; return 0; }
    "${ROUTER}" </dev/null >/dev/null 2>&1 &
    ROUTER_PID=$!
    sleep 1     # let it bind before clients connect
    printf '%s==>%s Started mtrouter (pid %s)\n' "${C_CYN}${C_BLD}" "${C_RST}" "${ROUTER_PID}"
}
stop_router() {
    [ -n "${ROUTER_PID}" ] || return 0
    kill -TERM "${ROUTER_PID}" 2>/dev/null
    wait "${ROUTER_PID}" 2>/dev/null
    ROUTER_PID=""
}
trap stop_router EXIT

# --------------------------------------------------------------------------
# Launch an app, let it run up to RUN_SECONDS, then stop it.
# Returns: 0 = clean exit(0), 2 = still running when stopped (interactive OK),
#          1 = exited with an error before the timeout.
# --------------------------------------------------------------------------
LAST_RC=0
run_app() {
    "$1" </dev/null >/dev/null 2>&1 &
    local pid=$! waited=0 grace
    while kill -0 "${pid}" 2>/dev/null; do
        if [ "${waited}" -ge "${RUN_SECONDS}" ]; then
            kill -TERM "${pid}" 2>/dev/null
            grace=0
            while kill -0 "${pid}" 2>/dev/null && [ "${grace}" -lt 2 ]; do sleep 1; grace=$((grace+1)); done
            kill -KILL "${pid}" 2>/dev/null
            wait "${pid}" 2>/dev/null
            return 2
        fi
        sleep 1; waited=$((waited+1))
    done
    wait "${pid}"; LAST_RC=$?
    [ "${LAST_RC}" -eq 0 ] && return 0 || return 1
}

# --------------------------------------------------------------------------
# Run the examples.
# --------------------------------------------------------------------------
shopt -s nullglob
total=0; passed=0; failed=0; skipped=0; failed_list=""
need_router=0
for app in [0-9][0-9]_*; do
    case "${app}" in *pub*) need_router=1; break;; esac
done
[ "${need_router}" -eq 1 ] && start_router

printf '%s==>%s Running examples (%ss each)\n' "${C_CYN}${C_BLD}" "${C_RST}" "${RUN_SECONDS}"
for app in [0-9][0-9]_*; do
    [ -f "${app}" ] && [ -x "${app}" ] || continue
    case "${app}" in
        20_*) printf '  %sSKIP%s  %s (Windows GUI)\n' "${C_YEL}" "${C_RST}" "${app}"; skipped=$((skipped+1)); continue;;
    esac

    total=$((total+1))
    run_app "./${app}"
    case "$?" in
        0) printf '  %sPASS%s  %s (clean exit)\n'        "${C_GRN}" "${C_RST}" "${app}"; passed=$((passed+1));;
        2) printf '  %sPASS%s  %s (ran %ss, stopped)\n'  "${C_GRN}" "${C_RST}" "${app}" "${RUN_SECONDS}"; passed=$((passed+1));;
        *) printf '  %sFAIL%s  %s (exit %s)\n'           "${C_RED}" "${C_RST}" "${app}" "${LAST_RC}"; failed=$((failed+1)); failed_list="${failed_list} ${app}";;
    esac
done

stop_router

# --------------------------------------------------------------------------
# Run the unit tests.
# --------------------------------------------------------------------------
if [ -n "${UNIT}" ]; then
    printf '\n%s==>%s Running unit tests: %s\n' "${C_CYN}${C_BLD}" "${C_RST}" "${UNIT}"
    total=$((total+1))
    if "${UNIT}" </dev/null; then
        printf '  %sPASS%s  unit tests\n' "${C_GRN}" "${C_RST}"; passed=$((passed+1))
    else
        printf '  %sFAIL%s  unit tests\n' "${C_RED}" "${C_RST}"; failed=$((failed+1)); failed_list="${failed_list} areg-unit-tests"
    fi
else
    printf '\n%s[warn]%s areg-unit-tests not found - skipped.\n' "${C_YEL}" "${C_RST}"
fi

# --------------------------------------------------------------------------
# Summary.
# --------------------------------------------------------------------------
printf '\n%s==== Summary ====%s\n' "${C_BLD}" "${C_RST}"
printf '  passed:  %d\n  failed:  %d\n  skipped: %d\n  total:   %d\n' "${passed}" "${failed}" "${skipped}" "${total}"
if [ "${failed}" -ne 0 ]; then
    printf '%sFailed:%s%s\n' "${C_RED}" "${C_RST}" "${failed_list}"
    exit 1
fi
printf '%sAll good.%s\n' "${C_GRN}" "${C_RST}"
exit 0
