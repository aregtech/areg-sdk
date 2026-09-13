#!/usr/bin/env bash
# One cold agent run, measured, against a clean snapshot of this checkout.
#
# The agent reads a copy of the files a clone carries -- tracked files, and untracked
# files that are not ignored, as the working tree has them -- so nothing local to this
# machine reaches the run: no ignored notes, no editor state, no harness customisation.
# The harness loads no skill and no MCP server, so both arms start from the same
# context. The run directory belongs outside the checkout, on a local disk.
#
# Everything below the helpers lives in main(), so bash parses the whole file before it
# runs any of it, and an edit while a run is in flight cannot corrupt the parse.
set -eu

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SDK="$(cd "${HERE}/../.." && pwd)"

usage()
{
    cat <<'USAGE'
One cold agent run, measured, against a clean snapshot of this checkout.

  run-benchmark.sh [label] [options]

  label              suffix of the run directory: e -> <out>/<date>e-<project>.
                     Optional; the next unused letter for today is chosen.

  --framework NAME   areg | grpc                          (default: areg)
  --task PATH        task file, absolute or relative to the SDK
                       (default: examples/ai-benchmark/prompt-coffeemachine.md)
  --wrapper PATH     the wrapper the prompt is built from, absolute or relative to
                     the SDK. Everything after its "--- PROMPT BEGINS BELOW THIS
                     LINE" marker is the prompt, with <areg-sdk>, <runner>, <task>,
                     <project> and <mode> substituted
                       (default: <framework>-<key>-prompt.txt beside a task named
                        prompt-<key>.md; without one, areg uses
                        areg-ai-prompt-template.txt and grpc refuses to start)
  --project NAME     C identifier: directory and CMake project name
                       (default: <key> of the task, e.g. coffeemachine)
  --mode MODE        ipc | local | pubsub, areg only      (default: ipc)
  --model NAME       sonnet | haiku | opus                (default: sonnet)
  --effort LEVEL     low | medium | high                  (default: medium)
  --attempts N       the build-and-fix and run-and-fix bound (default: 3). Any other
                     number adds one rule to the prompt, the same for both arms.
                     0 removes the bound, and is warned about: the spend is unbounded.
  --debrief          append a diagnostic pass: what the run could not find. It costs
                     requests on purpose, so such a run is never compared with one
                     made without it.
  --recipes MODE     none | copy, areg only               (default: none)
                       none: no example source may be copied; every file is written
                       or generated. copy: a documented recipe may be copied.
  --out DIR          where run directories are made
                       (default: $AREG_BENCHMARK_RUNS, else ~/runs)
  --dry-run          stage the run directory and print the prompt, start nothing
  --allow-installed-areg
                     proceed although find_package(areg) finds an installed package,
                     which would shadow the snapshot
  -h, --help         this text

  Examples:
    run-benchmark.sh                                  areg, the coffee machine
    run-benchmark.sh --framework grpc --attempts 15   gRPC, the coffee machine
    run-benchmark.sh --task examples/ai-benchmark/prompt-tempalarm.md
    run-benchmark.sh --task examples/ai-benchmark/prompt-atm.md --attempts 15
    run-benchmark.sh --task examples/ai-benchmark/prompt-printscan.md --attempts 15
    run-benchmark.sh f --model opus --effort high --out /data/runs --dry-run

  The run directory holds the measurement: meta.txt, prompt.txt, result.json,
  run.err, the fingerprints and sdk/, the snapshot the agent read. The agent works
  in work/, which starts empty.

  The agent is Claude Code, headless. Another agent is run by hand with the same
  prompt.txt; README.md says which of its numbers compare.
USAGE
}

die() { echo "run-benchmark: $*" >&2; exit 2; }

need() { [ $# -ge 2 ] || die "$1 needs a value"; }

# The files a clone of this checkout carries, copied as the working tree has them.
snapshot()
{
    local from="$1" to="$2"
    mkdir -p "${to}"
    git -C "${from}" ls-files -z --cached --others --exclude-standard \
      | python3 -c '
import os, shutil, sys
source, target = sys.argv[1], sys.argv[2]
copied = 0
for name in sys.stdin.buffer.read().split(b"\0"):
    relative = name.decode("utf-8")
    path = os.path.join(source, relative)
    if not relative or not os.path.isfile(path):
        continue
    destination = os.path.join(target, relative)
    os.makedirs(os.path.dirname(destination), exist_ok=True)
    shutil.copy2(path, destination)
    copied += 1
print(copied)
' "${from}" "${to}"
}

# The scaffolded project calls find_package(areg) before FetchContent, so an installed
# package answers first and the run builds against sources that are not the snapshot.
installed_areg()
{
    local probe; probe="$(mktemp -d)"
    cat > "${probe}/CMakeLists.txt" <<'PROBE'
cmake_minimum_required(VERSION 3.20)
project(probe NONE)
find_package(areg CONFIG QUIET)
if (areg_FOUND)
    message(NOTICE "AREG_INSTALLED_AT ${areg_DIR}")
endif()
PROBE
    local out; out="$(cmake -S "${probe}" -B "${probe}/b" 2>&1 || true)"
    rm -rf "${probe}" 2>/dev/null || true
    printf '%s' "${out}" | sed -n 's/.*AREG_INSTALLED_AT //p' | head -1
}

main()
{
    local FRAMEWORK="areg" TASK="examples/ai-benchmark/prompt-coffeemachine.md" WRAPPER=""
    local PROJECT="" MODE="ipc" MODEL="sonnet" EFFORT="medium"
    local ATTEMPTS="3" DEBRIEF="" RECIPES="none" LABEL="" DRY="" ALLOW_INSTALLED=""
    local OUT="${AREG_BENCHMARK_RUNS:-${HOME}/runs}"

    # A bare first word is the label. Anything starting with a dash is an option.
    if [ $# -gt 0 ]; then
        case "$1" in
            -h|--help) usage; exit 0 ;;
            -*) : ;;
            *)  LABEL="$1"; shift ;;
        esac
    fi

    while [ $# -gt 0 ]; do
        case "$1" in
            -h|--help)   usage; exit 0 ;;
            --framework) need "$@"; FRAMEWORK="$2"; shift 2 ;;
            --task)      need "$@"; TASK="$2";      shift 2 ;;
            --wrapper)   need "$@"; WRAPPER="$2";   shift 2 ;;
            --project)   need "$@"; PROJECT="$2";   shift 2 ;;
            --mode)      need "$@"; MODE="$2";      shift 2 ;;
            --model)     need "$@"; MODEL="$2";     shift 2 ;;
            --effort)    need "$@"; EFFORT="$2";    shift 2 ;;
            --attempts)  need "$@"; ATTEMPTS="$2";  shift 2 ;;
            --recipes)   need "$@"; RECIPES="$2";   shift 2 ;;
            --out)       need "$@"; OUT="$2";       shift 2 ;;
            --debrief)   DEBRIEF=1; shift ;;
            --dry-run)   DRY=1; shift ;;
            --allow-installed-areg) ALLOW_INSTALLED=1; shift ;;
            *) die "unknown option: $1 (run with --help)" ;;
        esac
    done

    # The key of prompt-<key>.md names the project and the wrapper.
    local KEY; KEY="$(basename "${TASK}" .md)"; KEY="${KEY#prompt-}"
    [ -n "${PROJECT}" ] || PROJECT="${KEY//[!A-Za-z0-9_]/}"

    case "${FRAMEWORK}" in areg|grpc) ;; *) die "--framework must be areg or grpc, not '${FRAMEWORK}'" ;; esac
    case "${MODE}"     in ipc|local|pubsub) ;; *) die "--mode must be ipc, local or pubsub, not '${MODE}'" ;; esac
    case "${MODEL}"    in sonnet|haiku|opus) ;; *) die "--model must be sonnet, haiku or opus, not '${MODEL}'" ;; esac
    case "${EFFORT}"   in low|medium|high) ;; *) die "--effort must be low, medium or high, not '${EFFORT}'" ;; esac
    case "${RECIPES}"  in none|copy) ;; *) die "--recipes must be none or copy, not '${RECIPES}'" ;; esac
    case "${ATTEMPTS}" in ''|*[!0-9]*) die "--attempts must be a whole number, not '${ATTEMPTS}'" ;; esac
    case "${PROJECT}"  in *[!A-Za-z0-9_]*|[!A-Za-z_]*|"") die "--project must be a C identifier, not '${PROJECT}'" ;; esac

    command -v claude >/dev/null || die "claude not found: install Claude Code and log in"
    command -v python3 >/dev/null || die "python3 not found"
    git -C "${SDK}" rev-parse HEAD >/dev/null 2>&1 \
        || die "${SDK} is not a git checkout; the snapshot is taken from its file list"

    local TASK_ABS WRAPPER_ABS
    case "${TASK}" in /*) TASK_ABS="${TASK}" ;; *) TASK_ABS="${SDK}/${TASK}" ;; esac
    [ -f "${TASK_ABS}" ] || die "no such task file: ${TASK_ABS}"

    if [ -z "${WRAPPER}" ]; then
        WRAPPER="$(dirname "${TASK_ABS}")/${FRAMEWORK}-${KEY}-prompt.txt"
        if [ ! -f "${WRAPPER}" ]; then
            [ "${FRAMEWORK}" = "areg" ] \
                || die "no ${FRAMEWORK} wrapper for this task: ${WRAPPER}. Write one, or pass --wrapper"
            WRAPPER="${HERE}/areg-ai-prompt-template.txt"
        fi
    fi
    case "${WRAPPER}" in /*) WRAPPER_ABS="${WRAPPER}" ;; *) WRAPPER_ABS="${SDK}/${WRAPPER}" ;; esac
    [ -f "${WRAPPER_ABS}" ] || die "no such wrapper file: ${WRAPPER_ABS}"

    if [ "${FRAMEWORK}" = "grpc" ]; then
        # The toolchain is the operator's job; an agent that installs one measures it.
        command -v protoc          >/dev/null || die "protoc not found; see ${HERE}/INSTALL-grpc.md"
        command -v grpc_cpp_plugin >/dev/null || die "grpc_cpp_plugin not found; see ${HERE}/INSTALL-grpc.md"
    elif [ -z "${ALLOW_INSTALLED}" ]; then
        local FOUND_AREG; FOUND_AREG="$(installed_areg)"
        [ -z "${FOUND_AREG}" ] || die "find_package(areg) resolves to an installed package at ${FOUND_AREG}.
  The scaffolded project calls find_package before FetchContent, so the run would
  build against that package instead of the snapshot. Uninstall it, or pass
  --allow-installed-areg to measure it deliberately."
    fi

    if [ "${ATTEMPTS}" = "0" ]; then
        echo "run-benchmark: WARNING --attempts 0 removes the fix bound; the spend is unbounded" >&2
    fi

    local suffix="${PROJECT}"
    [ "${FRAMEWORK}" = "grpc" ] && suffix="grpc-${PROJECT}"
    mkdir -p "${OUT}"
    OUT="$(cd "${OUT}" && pwd)"
    case "${OUT}/" in "${SDK}/"*) die "--out ${OUT} is inside the checkout; a run must not write where it reads" ;; esac

    local letter
    if [ -z "${LABEL}" ]; then
        for letter in a b c d e f g h i j k l m n o p q r s t u v w x y z; do
            [ -e "${OUT}/$(date -u +%Y%m%d)${letter}-${suffix}" ] || { LABEL="${letter}"; break; }
        done
        [ -n "${LABEL}" ] || die "every label a..z is used for ${suffix} today; pass one explicitly"
    fi

    local RUN="${OUT}/$(date -u +%Y%m%d)${LABEL}-${suffix}"
    [ -e "${RUN}" ] && die "run directory already exists: ${RUN}"
    local WORK="${RUN}/work" SNAP="${RUN}/sdk"
    mkdir -p "${WORK}"

    # What was measured: the revision, the uncommitted state, and the copy itself.
    git -C "${SDK}" rev-parse HEAD     > "${RUN}/sdk-head.txt"
    git -C "${SDK}" status --porcelain > "${RUN}/sdk-before.txt"
    local copied
    if [ "${FRAMEWORK}" = "grpc" ]; then
        # The gRPC arm is a cold start against gRPC and its public documentation. The
        # areg corpus must not exist inside the run at all: an agent cannot be asked
        # not to read a file that is sitting next to its working directory. Only the
        # scenario runner is staged, and it is stdlib-only, so one file is the whole
        # dependency.
        mkdir -p "${SNAP}/tools/agent"
        cp "${SDK}/tools/agent/run_scenarios.py" "${SNAP}/tools/agent/run_scenarios.py"
        copied=1
        ( cd "${SNAP}" && md5sum tools/agent/run_scenarios.py ) > "${RUN}/sdk-md5.txt"
        # The arm is only a cold start if the corpus is absent, so say so rather than
        # trust it: one stray page next to the working directory invalidates the run.
        local stray
        stray="$(find "${SNAP}" -type f ! -name run_scenarios.py | head -5)"
        [ -z "${stray}" ] || die "the gRPC arm staged more than the scenario runner:
${stray}"
        # The task travels with the run.
        cp "${TASK_ABS}" "${RUN}/task.md"
        if grep -qi areg "${RUN}/task.md"; then
            die "${TASK_ABS} names areg; the gRPC arm must not be told of it"
        fi
    else
        copied="$(snapshot "${SDK}" "${SNAP}")"
        ( cd "${SNAP}" && md5sum AGENTS.md docs/agent/*.md docs/agent/*.json docs/agent/.budgets \
                               tools/agent/*.py tools/agent/evals/tasks.json \
                               conf/cmake/functions.cmake examples/ai-benchmark/*.md \
                               examples/ai-benchmark/*.txt ) > "${RUN}/sdk-md5.txt"
    fi

    # A task inside the checkout is read from the snapshot; one outside it as given.
    local TASK_RUN="${TASK_ABS}"
    case "${TASK_ABS}" in "${SDK}/"*) TASK_RUN="${SNAP}/${TASK_ABS#"${SDK}/"}" ;; esac

    local ADD_DIR="${SNAP}" RULES=""
    if [ "${FRAMEWORK}" = "grpc" ]; then
        # The gRPC arm takes only the scenario runner from the SDK, so the task travels
        # with the run rather than through a directory the arm is not given.
        TASK_RUN="${RUN}/task.md"
        ADD_DIR="${SNAP}/tools/agent"
        RULES="- **You have gRPC and its public documentation. You do not have example source.**
  No .cpp, .hpp or .proto arrives by copy: every file in the project is written by
  you or generated by protoc."
        { protoc --version; grpc_cpp_plugin --version 2>&1 | head -1
          pkg-config --modversion grpc++ 2>/dev/null || echo "grpc++ pkg-config: none"
          cmake --version | head -1; } > "${RUN}/toolchain.txt"
    elif [ "${RECIPES}" = "none" ]; then
        RULES="- **You have the tools and the documentation. You do not have example source.**
  Treat ${SNAP}/docs/agent/recipes/ and the applications under ${SNAP}/examples/ as
  absent: do not open a file in them and do not copy one out of them. The files your
  prompt names are the exception. Where AGENTS.md offers a recipe to copy, read the
  page it names instead. Code shown inside a documentation page is there to be read
  and understood, never to be copied in as a starting point.

- **Every file in the project is written by you or generated by a tool in
  ${SNAP}/tools/agent/.** No .cpp, .hpp, .siml or .fsml arrives by copy."
    else
        RULES="- **Start from working code where AGENTS.md offers it.** A recipe under
  ${SNAP}/docs/agent/recipes/ may be copied into the project and adapted. Copy it,
  rename it, and make it yours; never leave a file that still builds the recipe's
  application. The applications under ${SNAP}/examples/ are not for copying."
    fi

    if [ "${ATTEMPTS}" = "0" ]; then
        RULES="${RULES}

- **This task has no fix bound.** Wherever the runbook or this prompt says at most 3
  build-and-fix or run-and-fix cycles, there is no limit. Fix the cause, never the
  symptom, and never loosen what a scenario expects."
    elif [ "${ATTEMPTS}" != "3" ]; then
        RULES="${RULES}

- **The maximum fix bound for this task is ${ATTEMPTS}.** Wherever the runbook or this
  prompt says at most 3 build-and-fix cycles or at most 3 run-and-fix cycles, read
  ${ATTEMPTS}. Everything else about the bound is unchanged: fix the cause, never the
  symptom, and never loosen what a scenario expects."
    fi

    if [ -n "${DEBRIEF}" ]; then
        RULES="${RULES}

Additionally, for this run only -- a diagnostic pass the normal task does not ask
for. Do it last, after the report, and never let it change what you built:

- **Every page you opened, in order, with the request you opened it at and why.**
  Name the row of a routing table that sent you, or say that nothing did.
- **Every question you answered from your own training rather than from a page**,
  and what you would have needed to read to answer it from the documentation.
- **Every place two sources said different things**, naming both, and which one
  you followed.
- **Every marker whose one-line hint was not enough**, and what it should have said.
- **Anything you looked for and could not find** -- a signature, a rule, an example
  -- and where you looked first.

Be specific and short: a list, not prose."
    fi

    # The prompt is the wrapper a user pastes, with its values substituted, so the
    # benchmark measures exactly what a user runs.
    local BODY
    BODY="$(sed -n '/^--- PROMPT BEGINS BELOW THIS LINE/,$p' "${WRAPPER_ABS}" \
             | tail -n +2 | sed '/./,$!d')"
    [ -n "${BODY}" ] || die "${WRAPPER_ABS} has no '--- PROMPT BEGINS BELOW THIS LINE' marker"
    BODY="${BODY//<areg-sdk>/${SNAP}}"
    BODY="${BODY//<runner>/${SNAP}/tools/agent/run_scenarios.py}"
    BODY="${BODY//<task>/${TASK_RUN}}"
    BODY="${BODY//<project>/${PROJECT}}"
    BODY="${BODY//<mode>/${MODE}}"
    { printf '%s\n' "${BODY}"; printf '\n%s\n' "${RULES}"; } > "${RUN}/prompt.txt"
    if [ "${FRAMEWORK}" = "grpc" ] && grep -qi areg "${RUN}/prompt.txt"; then
        die "the gRPC prompt names areg; check ${WRAPPER_ABS}, and that --out ${OUT} does not"
    fi

    { echo "framework ${FRAMEWORK}"; echo "model    ${MODEL}"; echo "effort   ${EFFORT}"
      echo "task     ${TASK_RUN}"; echo "mode     ${MODE}"; echo "recipes  ${RECIPES}"
      echo "attempts ${ATTEMPTS}"; echo "debrief  ${DEBRIEF:-no}"
      echo "source   ${SDK}"; echo "sdk      ${SNAP}"; echo "files    ${copied}"
      echo "head     $(cat "${RUN}/sdk-head.txt")"
      echo "isolation snapshot, no skills, no MCP servers"
      date -u +"start    %Y-%m-%dT%H:%M:%SZ"; } > "${RUN}/meta.txt"

    if [ -n "${DRY}" ]; then
        echo "staged ${RUN} (${copied} files in the snapshot)"
        echo "--- prompt.txt ---"
        cat "${RUN}/prompt.txt"
        exit 0
    fi

    echo "run-benchmark: ${RUN}"
    echo "run-benchmark: ${FRAMEWORK}, ${MODEL}, effort ${EFFORT}, attempts ${ATTEMPTS}, head $(cut -c1-8 "${RUN}/sdk-head.txt"), ${copied} files"

    cd "${WORK}"
    local code=0
    # A project's first build compiles the whole framework, which can outlast the
    # agent's default command timeout; both arms get the same ceiling.
    BASH_DEFAULT_TIMEOUT_MS=600000 BASH_MAX_TIMEOUT_MS=900000 \
    claude -p --output-format json \
           --model "${MODEL}" --effort "${EFFORT}" \
           --disable-slash-commands --strict-mcp-config \
           --allowedTools "Bash Read Write Edit Glob Grep" \
           --add-dir "${ADD_DIR}" \
           < "${RUN}/prompt.txt" > "${RUN}/result.json" 2> "${RUN}/run.err" || code=$?
    { date -u +"end      %Y-%m-%dT%H:%M:%SZ"; echo "exit     ${code}"; } >> "${RUN}/meta.txt"

    echo
    local cache="${WORK}/build/CMakeCache.txt"
    if [ "${FRAMEWORK}" = "areg" ] && [ -f "${cache}" ]; then
        if grep -q 'areg_DIR:PATH=areg_DIR-NOTFOUND' "${cache}" \
           && grep -q "${SNAP}" "${WORK}/build/CMakeFiles/Makefile.cmake" 2>/dev/null; then
            echo "sources: ${SNAP}, verified (no installed areg was used)"
        else
            echo "sources: NOT VERIFIED as ${SNAP}; check ${cache} for areg_DIR" >&2
        fi
    fi

    # The manifest is taken before the run so a run that edited the corpus it is
    # measured against is caught rather than scored.
    if [ -s "${RUN}/sdk-md5.txt" ]; then
        local changed
        changed="$( cd "${SNAP}" && md5sum -c "${RUN}/sdk-md5.txt" 2>/dev/null \
                    | grep -v ': OK$' || true )"
        if [ -n "${changed}" ]; then
            echo "corpus:  CHANGED DURING THE RUN -- this measurement is not valid" >&2
            echo "${changed}" >&2
            echo "corpus   changed during the run" >> "${RUN}/meta.txt"
            if [ "${code}" -eq 0 ]; then code=3; fi
        else
            echo "corpus:  unchanged, $(wc -l < "${RUN}/sdk-md5.txt") file(s) verified"
            echo "corpus   unchanged" >> "${RUN}/meta.txt"
        fi
    fi

    echo
    python3 "${HERE}/analyze_run.py" "${RUN}" || true
    exit ${code}
}

main "$@"
