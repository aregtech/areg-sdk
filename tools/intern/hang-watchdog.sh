#!/bin/bash
# ---------------------------------------------------------------------------
# hang-watchdog.sh -- reports and stops a CI step that does not finish.
#
# Usage: hang-watchdog.sh <done-file> <limit-seconds> <name> [<name> ...]
#
# Waits until <done-file> exists or <limit-seconds> pass. On the limit, prints
# every process, the thread stacks of each process whose executable is one of
# <name>, with or without its extension, kills those processes and exits 1. The
# report is also written to <done-file>.report. Runs on Linux, macOS and Cygwin,
# with bash 3.2.
# ---------------------------------------------------------------------------

done_file="$1"
limit="$2"
shift 2
names="$*"
report="${done_file}.report"

if [ -z "${done_file}" ] || [ -z "${limit}" ] || [ -z "${names}" ]; then
    echo "usage: $0 <done-file> <limit-seconds> <name> [<name> ...]" >&2
    exit 2
fi

waited=0
while [ "${waited}" -lt "${limit}" ]; do
    if [ -e "${done_file}" ]; then
        exit 0
    fi
    sleep 1
    waited=$((waited + 1))
done

if [ -e "${done_file}" ]; then
    exit 0
fi

platform="$(uname -s)"

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

# Prints the PID of every process whose executable name is one of the names.
matching_pids() {
    case "${platform}" in
        CYGWIN*)
            ps -aW | awk -v names=" ${names} " 'NR > 1 {
                cmd = $8
                for (i = 9; i <= NF; ++i) cmd = cmd " " $i
                sub(/.*[\\\/]/, "", cmd)
                cmd = tolower(cmd)
                base = cmd
                sub(/\.[^.]*$/, "", base)
                if (index(tolower(names), " " cmd " ") > 0 || index(tolower(names), " " base " ") > 0) print $1
            }'
            ;;
        *)
            ps -axo pid=,comm= | awk -v names=" ${names} " '{
                cmd = $2
                for (i = 3; i <= NF; ++i) cmd = cmd " " $i
                sub(/.*\//, "", cmd)
                base = cmd
                sub(/\.[^.]*$/, "", base)
                if (index(names, " " cmd " ") > 0 || index(names, " " base " ") > 0) print $1
            }'
            ;;
    esac
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

pids="$(matching_pids)"

{
    echo "::error::[hang-watchdog] still running after ${limit} s: ${names}"
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
    for pid in ${pids}; do
        print_stacks "${pid}"
    done
    echo "[hang-watchdog] ---- stopping: ${pids} ----"
} 2>&1 | tee "${report}"

for pid in ${pids}; do
    stop_process "${pid}"
done

exit 1
