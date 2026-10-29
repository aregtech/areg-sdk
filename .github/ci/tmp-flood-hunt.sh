#!/bin/bash
# ---------------------------------------------------------------------------
# tmp-flood-hunt.sh -- TEMPORARY: runs the router io case stop-while-flooding
# in a loop until it hangs or fails, or the time budget is spent.
#
# Usage: tmp-flood-hunt.sh <bin-dir> <out-dir> <minutes> <pairs> <hang-seconds>
#
# A run still alive after <hang-seconds> is photographed before it is killed:
# the TCP connections of the router port, the thread stacks, and the TCP
# connections again. Every run log and summary.txt go to <out-dir>.
# Exits 0 when the budget is spent with every run passed, 1 on a hang or a
# failure, 2 on a usage error.
# ---------------------------------------------------------------------------

bin_dir="$1"
out_dir="$2"
minutes="$3"
pairs="$4"
hang_s="$5"
if [ -z "${bin_dir}" ] || [ -z "${out_dir}" ] || [ -z "${minutes}" ] || [ -z "${pairs}" ] || [ -z "${hang_s}" ]; then
    echo "usage: $0 <bin-dir> <out-dir> <minutes> <pairs> <hang-seconds>" >&2
    exit 2
fi

mkdir -p "${out_dir}"
out_dir="$(cd "${out_dir}" && pwd)"
cd "${bin_dir}" || exit 2
summary="${out_dir}/summary.txt"
config="./config/router-io-stop-while-flooding-${pairs}.init"

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
            echo "[flood-hunt] '$*' did not finish in ${seconds} s"
            break
        fi
        sleep 1
        count=$((count + 1))
    done
    wait "${pid}" 2>/dev/null
}

# Prints the TCP connections from and to the router port.
connections() {
    local port="$1"
    echo "[flood-hunt] ---- TCP connections of port ${port}, $(date +%T) ----"
    if [ -n "${port}" ]; then
        bounded 30 netstat -ano -p tcp | grep -E "[:.]${port}[[:space:]]" | tr -d '\r'
    fi
}

{
    uname -a
    gcc --version | head -n 1
    echo "budget ${minutes} min, pairs ${pairs}, hang limit ${hang_s} s"
} | tee "${summary}"

end=$((SECONDS + minutes * 60))
run=0
passed=0
while [ ${SECONDS} -lt ${end} ]; do
    run=$((run + 1))
    log="${out_dir}/run.${run}.log"
    start=${SECONDS}
    ./areg-router-io-test stop-while-flooding "${pairs}" . > "${log}" 2>&1 &
    pid=$!
    while kill -0 "${pid}" 2>/dev/null && [ $((SECONDS - start)) -lt "${hang_s}" ]; do
        sleep 1
    done

    if kill -0 "${pid}" 2>/dev/null; then
        port="$(sed -n 's/^router::\*::port::tcpip *= *\([0-9]*\).*/\1/p' "${config}" | head -n 1)"
        report="${out_dir}/hang.${run}.txt"
        {
            echo "::error::[flood-hunt] run ${run} still running after ${hang_s} s, PID ${pid}, ${passed} runs passed before it"
            echo "[flood-hunt] ---- output of the run so far ----"
            cat "${log}"
            connections "${port}"
            echo "[flood-hunt] ---- stacks of PID ${pid} ----"
            bounded 180 gdb -batch -nx -p "${pid}" -ex "set pagination off" -ex "info threads" -ex "thread apply all bt"
            connections "${port}"
        } > "${report}" 2>&1
        cat "${report}"
        /bin/kill -f -9 "${pid}" 2>/dev/null
        wait "${pid}" 2>/dev/null
        echo "run ${run}: HANG after ${hang_s} s, report ${report}" | tee -a "${summary}"
        exit 1
    fi

    wait "${pid}"
    rc=$?
    took=$((SECONDS - start))
    if [ ${rc} -ne 0 ]; then
        echo "::error::[flood-hunt] run ${run} failed with exit code ${rc}"
        cat "${log}"
        echo "run ${run}: FAIL rc=${rc} in ${took} s" | tee -a "${summary}"
        exit 1
    fi

    passed=$((passed + 1))
    echo "run ${run}: pass in ${took} s" >> "${summary}"
done

echo "[flood-hunt] ${passed} of ${run} runs passed in ${minutes} min" | tee -a "${summary}"
exit 0
