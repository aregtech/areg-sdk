#!/bin/bash
# ---------------------------------------------------------------------------
# hang-watchdog.sh -- reports and stops a process of a CI step that hangs.
#
# Usage: hang-watchdog.sh <done-file> <limit-seconds> <pattern> [<pattern> ...]
#
# Watches until <done-file> exists. A process whose executable name, with or
# without its extension, matches one of the shell patterns and runs longer
# than <limit-seconds> gets every process and its thread stacks printed, and
# is then killed. Quote a pattern that has a wildcard. The report is also
# written to <done-file>.report. Exits 1 if a process was killed. Runs on
# Linux, macOS and Cygwin, with bash 3.2.
# ---------------------------------------------------------------------------

done_file="$1"
limit="$2"
shift 2
patterns="$*"
report="${done_file}.report"

if [ -z "${done_file}" ] || [ -z "${limit}" ] || [ -z "${patterns}" ]; then
    echo "usage: $0 <done-file> <limit-seconds> <pattern> [<pattern> ...]" >&2
    exit 2
fi

# The patterns are matched by 'case', never expanded against files.
set -f

platform="$(uname -s)"
case "${platform}" in
    CYGWIN*) patterns="$(echo "${patterns}" | tr '[:upper:]' '[:lower:]')" ;;
esac

# Runs a command and kills it after the given seconds.
bounded() {
    local seconds="$1"
    shift
    "$@" &
    local pid=$!
    local count=0
    while kill -0 "${pid}" 2>/dev/null; do
        if [ "${count}" -ge "${seconds}" ]; then
            kill -9 "${pid}" 2>/dev/null
            echo "[hang-watchdog] '$*' did not finish in ${seconds} s"
            break
        fi
        sleep 1
        count=$((count + 1))
    done
    wait "${pid}" 2>/dev/null
}

# Prints the PID of every process whose executable name matches one of the patterns.
matching_pids() {
    local pid cmd base pattern
    case "${platform}" in
        CYGWIN*)
            ps -aW | awk 'NR > 1 {
                cmd = $8
                for (i = 9; i <= NF; ++i) cmd = cmd " " $i
                sub(/.*[\\\/]/, "", cmd)
                print $1 "\t" tolower(cmd)
            }'
            ;;
        *)
            ps -axo pid=,comm= | awk '{
                cmd = $2
                for (i = 3; i <= NF; ++i) cmd = cmd " " $i
                sub(/.*\//, "", cmd)
                print $1 "\t" cmd
            }'
            ;;
    esac | while IFS=$'\t' read -r pid cmd; do
        base="${cmd%.*}"
        for pattern in ${patterns}; do
            case "${cmd}" in ${pattern}) echo "${pid}"; break ;; esac
            case "${base}" in ${pattern}) echo "${pid}"; break ;; esac
        done
    done
}

# Prints the thread stacks of one process.
print_stacks() {
    local pid="$1"
    echo "[hang-watchdog] ---- stacks of PID ${pid} ----"
    case "${platform}" in
        Darwin)
            bounded 60 sample "${pid}" 3 -file /dev/stdout
            ;;
        *)
            bounded 90 gdb -batch -nx -p "${pid}" -ex "info threads" -ex "thread apply all bt"
            ;;
    esac
}

stop_process() {
    case "${platform}" in
        CYGWIN*) /bin/kill -f -9 "$1" 2>/dev/null ;;
        *)       kill -9 "$1" 2>/dev/null ;;
    esac
}

# Prints every process and the stacks of the hung one, then kills it.
report_hang() {
    local pid="$1"
    local age="$2"
    {
        echo "::error::[hang-watchdog] PID ${pid} still running after ${age} s: ${patterns}"
        echo "[hang-watchdog] ---- processes ----"
        case "${platform}" in
            CYGWIN*)
                ps -aW
                bounded 30 tasklist
                ;;
            *)
                ps -axo pid,ppid,etime,stat,command
                ;;
        esac
        print_stacks "${pid}"
        echo "[hang-watchdog] ---- stopping: ${pid} ----"
    } 2>&1 | tee -a "${report}"
    stop_process "${pid}"
}

# Each entry of 'seen' is ' <pid>:<second it was first seen>'.
seen=""
stopped=0
while [ ! -e "${done_file}" ]; do
    now=${SECONDS}
    current=""
    for pid in $(matching_pids); do
        case "${seen} " in
            *" ${pid}:"*)
                start="${seen#* ${pid}:}"
                start="${start%% *}"
                ;;
            *)
                start=${now}
                ;;
        esac
        if [ $((now - start)) -ge "${limit}" ]; then
            report_hang "${pid}" "$((now - start))"
            stopped=1
        else
            current="${current} ${pid}:${start}"
        fi
    done
    seen="${current}"
    sleep 1
done

exit ${stopped}
