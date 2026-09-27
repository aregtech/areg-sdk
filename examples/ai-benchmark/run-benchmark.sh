#!/usr/bin/env bash
# One cold agent run, measured, against a clean snapshot of this checkout.
#
# The agent reads a copy of the files a clone carries -- tracked files, and untracked
# files that are not ignored, as the working tree has them -- so nothing local to this
# machine reaches the run: no ignored notes, no editor state, no harness customisation.
# Claude loads no skill or MCP server; other CLIs retain their own configuration.
# The run directory belongs outside the checkout, on a local disk.
#
# Everything below the helpers lives in main(), so bash parses the whole file before it
# runs any of it, and an edit while a run is in flight cannot corrupt the parse.
set -eu

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SDK=""

usage()
{
    cat <<'USAGE'
One cold agent run, measured, against a clean snapshot of this checkout.

  run-benchmark.sh [label] [options]

  label              suffix: e -> ./<date>e-<project>, made in the current directory.
                     Optional; the next unused letter for today is chosen.

  --framework NAME   areg | grpc                          (default: areg)
  --agent NAME       claude | copilot | codex | gemini    (default: claude)
  --task PATH        task file, absolute or relative to the SDK
                       (default: examples/ai-benchmark/prompt-coffeemachine.md)
  --wrapper PATH     the wrapper the prompt is built from, absolute or relative to
                     the SDK. Everything after its "--- PROMPT BEGINS BELOW THIS
                     LINE" marker is the prompt, with <areg-sdk>, <runner>, <task>,
                     <project> and <mode> substituted
                       (default: <framework>-<key>-prompt.txt beside a task named
                        prompt-<key>.md; without one, <framework>-ai-prompt-
                        template.txt beside this script)
  --project NAME     C identifier: directory and CMake project name
                       (default: <key> of the task, e.g. coffeemachine)
  --mode MODE        ipc | local | pubsub, areg only      (default: ipc)
  --model NAME       model ID or alias accepted by the selected CLI; not a whitelist
                       (default: sonnet for Claude; the CLI default otherwise)
  --effort LEVEL     low | medium | high, one vocabulary for every CLI: it becomes
                     Claude's --effort, Copilot's --reasoning-effort or Codex's
                     model_reasoning_effort. Gemini has no such setting and refuses it
                       (default: medium for Claude; the CLI default otherwise)
  --attempts N       the build-and-fix and run-and-fix bound (default: 15, which is
                     what every published run used; the runbook's own bound is 3).
                     Any number other than the runbook's adds one rule to the prompt,
                     the same for both arms. 0 removes the bound, and is warned about:
                     the spend is unbounded.
  --debrief          append a diagnostic pass: what the run could not find. It costs
                     requests on purpose, so such a run is never compared with one
                     made without it.
  --recipes MODE     none | copy, areg only               (default: none)
                       none: no example source may be copied; every file is written
                       or generated. copy: a documented recipe may be copied.
  --web MODE         on | off: whether the agent may search and fetch pages
                       (default: off for areg, on for grpc)
                     areg ships its documentation in the snapshot, and that snapshot
                     is what a run measures; an arm that may browse can read the
                     published copy instead, and no manifest can tell. gRPC ships its
                     documentation on the web, and its prompt says so, so an arm
                     denied the web has no documentation at all and the comparison
                     measures recall rather than the framework.
  --verify MODE      none | probes | sanitize             (default: probes)
                       the hidden acceptance probes of verify_run.py, run on the
                       project after the agent ends. sanitize adds a rebuild under
                       ASan and UBSan.
  --sdk DIR          the areg-sdk checkout: the directory that holds AGENTS.md
                       (default: $AREG_SDK_ROOT, else the checkout this script sits
                        in; asked for when neither answers)
  --grpc DIR         the directory holding protoc and grpc_cpp_plugin, or the prefix
                     they are installed under, whose headers and libraries CMake is
                     then told about. gRPC arm only
                       (default: $AREG_GRPC_ROOT, else the PATH; asked for when
                        neither answers)
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
    run-benchmark.sh --task examples/ai-benchmark/prompt-elevator.md --attempts 15
    run-benchmark.sh --task examples/ai-benchmark/prompt-sensorgateway.md --attempts 15
    run-benchmark.sh --task examples/ai-benchmark/prompt-greenhouse.md --framework grpc
    run-benchmark.sh f --model opus --effort high --dry-run
    run-benchmark.sh --sdk ~/src/areg-sdk --framework grpc --grpc /usr/local
    run-benchmark.sh --agent copilot --model gpt-5.6-terra
    run-benchmark.sh --agent codex --model gpt-5.4 --effort high
    run-benchmark.sh --agent gemini --model gemini-2.5-flash
    run-benchmark.sh --verify sanitize                areg, probes and sanitizers

  The run directory is made in the current directory, so start the run where the
  measurements should land, and never inside the checkout. It holds meta.txt,
  prompt.txt, result.json[l], run.err, the fingerprints and sdk/, the snapshot the
  agent read. The agent works in work/, which starts empty, and is given sdk/ and
  nothing else: never the run directory, which names the source checkout.

  Install and authenticate only the selected CLI. Model availability depends on
  that CLI and your account; README.md explains model names and comparable metrics.
USAGE
}

die() { echo "run-benchmark: $*" >&2; exit 2; }

need() { [ $# -ge 2 ] || die "$1 needs a value"; }

# The gRPC arm is a cold start against gRPC alone. These are the words that would
# tell it otherwise: the SDK's name, its tools, its documents and its file types.
# Nothing the agent can reach may carry one, the prompt and the paths included.
COLD_START_BAN='areg|AGENTS\.md|build_project|gen_skeleton|gen_docs|setup_project|check_contract|check_corpus|run_evals|explain_rule|schema_help|api_help|worksheet|bodies\.txt|design\.json|\.siml|\.fsml|\.dtml|docs/agent|runbook'

# A path is never searched for. It is given, inherited, found where this script sits,
# or asked for: a guessed path measures a tree nobody chose. The answer lands in
# ANSWER, because a die() inside $(...) would only end the subshell.
ANSWER=""
ask_path()
{
    ANSWER=""
    # The device node exists even with no controlling terminal, so it is opened in a
    # subshell rather than tested: -r /dev/tty answers yes and the write then fails.
    ( : > /dev/tty ) 2>/dev/null || die "$1
  There is no terminal to ask on. Pass the option."
    printf 'run-benchmark: %s\n> ' "$1" > /dev/tty
    IFS= read -r ANSWER < /dev/tty || true
    [ -n "${ANSWER}" ] || die "nothing given; pass the option"
}

# The SDK is the directory that holds AGENTS.md, and that is what is checked.
resolve_sdk()
{
    local candidate=""
    if [ -n "${SDK_OPT}" ]; then
        candidate="${SDK_OPT}"
    elif [ -n "${AREG_SDK_ROOT:-}" ]; then
        candidate="${AREG_SDK_ROOT}"
    elif [ -f "${HERE}/../../AGENTS.md" ]; then
        candidate="${HERE}/../.."
    else
        ask_path "the areg-sdk checkout is unknown: give the directory that holds AGENTS.md, or pass --sdk"
        candidate="${ANSWER}"
    fi
    [ -d "${candidate}" ] || die "--sdk ${candidate} is not a directory"
    SDK="$(cd "${candidate}" && pwd)"
    [ -f "${SDK}/AGENTS.md" ] \
        || die "${SDK} holds no AGENTS.md, so it is not an areg-sdk checkout; pass --sdk"
}

# protoc and grpc_cpp_plugin, and the prefix its headers and libraries are under.
resolve_grpc()
{
    local candidate=""
    if [ -n "${GRPC_OPT}" ]; then
        candidate="${GRPC_OPT}"
    elif [ -n "${AREG_GRPC_ROOT:-}" ]; then
        candidate="${AREG_GRPC_ROOT}"
    elif command -v protoc >/dev/null 2>&1 && command -v grpc_cpp_plugin >/dev/null 2>&1; then
        return 0
    else
        ask_path "protoc and grpc_cpp_plugin are not on the PATH: give the directory that holds them (or the install prefix), or pass --grpc"
        candidate="${ANSWER}"
    fi
    [ -d "${candidate}" ] || die "--grpc ${candidate} is not a directory"
    candidate="$(cd "${candidate}" && pwd)"
    local bin="${candidate}"
    [ -x "${bin}/protoc" ] || bin="${candidate}/bin"
    [ -x "${bin}/protoc" ] || die "no protoc in ${candidate} or ${candidate}/bin; see ${HERE}/INSTALL-grpc.md"
    PATH="${bin}:${PATH}"; export PATH
    # A prefix carries the headers and libraries too, so CMake is told where to look
    # rather than left to find whichever copy the machine happens to have first.
    if [ "${bin}" != "${candidate}" ]; then
        CMAKE_PREFIX_PATH="${candidate}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}"
        export CMAKE_PREFIX_PATH
    fi
}

# One vocabulary for every CLI: low, medium, high. Each spells it differently, and the
# spelling lives here so a comparison is never quietly reading three different scales.
EFFORT_ARGS=()
effort_args()
{
    EFFORT_ARGS=()
    [ -n "${EFFORT}" ] || return 0
    case "$1" in
        claude)  EFFORT_ARGS=(--effort "${EFFORT}") ;;
        copilot) EFFORT_ARGS=(--reasoning-effort "${EFFORT}") ;;
        codex)   EFFORT_ARGS=(--config "model_reasoning_effort='${EFFORT}'") ;;
    esac
}

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
    if (not relative or not os.path.isfile(path)
            or relative == "examples/ai-benchmark/verify_run.py"):
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
    local PROJECT="" MODE="ipc" AGENT="claude" MODEL="" EFFORT=""
    local ATTEMPTS="15" DEBRIEF="" RECIPES="none" LABEL="" DRY="" ALLOW_INSTALLED=""
    local VERIFY="probes" SDK_OPT="" GRPC_OPT="" WEB=""

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
            --agent)     need "$@"; AGENT="$2";     shift 2 ;;
            --task)      need "$@"; TASK="$2";      shift 2 ;;
            --wrapper)   need "$@"; WRAPPER="$2";   shift 2 ;;
            --project)   need "$@"; PROJECT="$2";   shift 2 ;;
            --mode)      need "$@"; MODE="$2";      shift 2 ;;
            --model)     need "$@"; [ -n "$2" ] || die "--model needs a non-empty value"; MODEL="$2"; shift 2 ;;
            --effort)    need "$@"; EFFORT="$2";    shift 2 ;;
            --attempts)  need "$@"; ATTEMPTS="$2";  shift 2 ;;
            --recipes)   need "$@"; RECIPES="$2";   shift 2 ;;
            --sdk)       need "$@"; SDK_OPT="$2";   shift 2 ;;
            --grpc)      need "$@"; GRPC_OPT="$2";  shift 2 ;;
            --verify)    need "$@"; VERIFY="$2";    shift 2 ;;
            --web)       need "$@"; WEB="$2";       shift 2 ;;
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
    case "${AGENT}"    in claude|copilot|codex|gemini) ;; *) die "--agent must be claude, copilot, codex or gemini, not '${AGENT}'" ;; esac
    case "${EFFORT}"   in ''|low|medium|high) ;; *) die "--effort must be low, medium or high, not '${EFFORT}'" ;; esac
    if [ "${AGENT}" = "claude" ]; then
        MODEL="${MODEL:-sonnet}"; EFFORT="${EFFORT:-medium}"
    elif [ "${AGENT}" = "gemini" ] && [ -n "${EFFORT}" ]; then
        die "--effort is not supported by Gemini CLI; omit it"
    fi
    case "${RECIPES}"  in none|copy) ;; *) die "--recipes must be none or copy, not '${RECIPES}'" ;; esac
    case "${VERIFY}"   in none|probes|sanitize) ;; *) die "--verify must be none, probes or sanitize, not '${VERIFY}'" ;; esac
    case "${WEB}"      in ''|on|off) ;; *) die "--web must be on or off, not '${WEB}'" ;; esac
    # Each arm gets its own framework's documentation, in the place that documentation
    # lives: the snapshot for areg, the web for gRPC.
    if [ -z "${WEB}" ]; then
        WEB="off"; [ "${FRAMEWORK}" != "grpc" ] || WEB="on"
    fi
    case "${ATTEMPTS}" in ''|*[!0-9]*) die "--attempts must be a whole number, not '${ATTEMPTS}'" ;; esac
    case "${PROJECT}"  in *[!A-Za-z0-9_]*|[!A-Za-z_]*|"") die "--project must be a C identifier, not '${PROJECT}'" ;; esac

    command -v "${AGENT}" >/dev/null || die "${AGENT} not found: install the selected CLI and log in"
    command -v python3 >/dev/null || die "python3 not found"
    resolve_sdk
    git -C "${SDK}" rev-parse HEAD >/dev/null 2>&1 \
        || die "${SDK} is not a git checkout; the snapshot is taken from its file list"

    local TASK_ABS WRAPPER_ABS
    case "${TASK}" in /*) TASK_ABS="${TASK}" ;; *) TASK_ABS="${SDK}/${TASK}" ;; esac
    [ -f "${TASK_ABS}" ] || die "no such task file: ${TASK_ABS}"

    if [ -z "${WRAPPER}" ]; then
        WRAPPER="$(dirname "${TASK_ABS}")/${FRAMEWORK}-${KEY}-prompt.txt"
        if [ ! -f "${WRAPPER}" ]; then
            WRAPPER="${HERE}/${FRAMEWORK}-ai-prompt-template.txt"
        fi
    fi
    case "${WRAPPER}" in /*) WRAPPER_ABS="${WRAPPER}" ;; *) WRAPPER_ABS="${SDK}/${WRAPPER}" ;; esac
    [ -f "${WRAPPER_ABS}" ] || die "no such wrapper file: ${WRAPPER_ABS}"

    if [ "${FRAMEWORK}" = "grpc" ]; then
        # The toolchain is the operator's job; an agent that installs one measures it.
        resolve_grpc
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

    # The run is made where it is started, so no path is assumed and no home is
    # searched. The one rule that remains is the old one.
    local HOST; HOST="$(pwd)"
    case "${HOST}/" in "${SDK}/"*) die "the current directory ${HOST} is inside the checkout; a run must not write where it reads. Start it somewhere else." ;; esac

    local letter
    if [ -z "${LABEL}" ]; then
        for letter in a b c d e f g h i j k l m n o p q r s t u v w x y z; do
            [ -e "${HOST}/$(date -u +%Y%m%d)${letter}-${suffix}" ] || { LABEL="${letter}"; break; }
        done
        [ -n "${LABEL}" ] || die "every label a..z is used for ${suffix} today; pass one explicitly"
    fi

    local RUN="${HOST}/$(date -u +%Y%m%d)${LABEL}-${suffix}"
    [ -e "${RUN}" ] && die "run directory already exists: ${RUN}"
    local WORK="${RUN}/work" SNAP="${RUN}/sdk"
    local HEADF="${RUN}/sdk-head.txt" BEFOREF="${RUN}/sdk-before.txt" MD5F="${RUN}/sdk-md5.txt"
    if [ "${FRAMEWORK}" = "grpc" ]; then
        # The agent works in ${WORK} and "ls .." reaches everything beside it. For a
        # cold start that directory may show work/ and task/ and nothing else: these
        # three files name the checkout the task came from, and the snapshot is a
        # task, not an SDK, so it is not called one.
        SNAP="${RUN}/task"
        mkdir -p "${RUN}/provenance"
        HEADF="${RUN}/provenance/head.txt"
        BEFOREF="${RUN}/provenance/before.txt"
        MD5F="${RUN}/provenance/md5.txt"
        echo "${SDK}" > "${RUN}/provenance/source.txt"
    fi
    mkdir -p "${WORK}"

    # What was measured: the revision, the uncommitted state, and the copy itself.
    git -C "${SDK}" rev-parse HEAD     > "${HEADF}"
    git -C "${SDK}" status --porcelain > "${BEFOREF}"
    local copied
    if [ "${FRAMEWORK}" = "grpc" ]; then
        # The gRPC arm is a cold start against gRPC and its public documentation. The
        # areg corpus must not exist inside the run at all: an agent cannot be asked
        # not to read a file that is sitting next to its working directory. Only the
        # stdlib-only scenario runner and the task are staged.
        mkdir -p "${SNAP}"
        cp "${SDK}/tools/agent/run_scenarios.py" "${SNAP}/run_scenarios.py"
        # The runner names no framework. Its wording comes from the dialect beside
        # it, and the arm is given the gRPC one, never this checkout's.
        cp "${HERE}/grpc-scenario-dialect.py" "${SNAP}/scenario_dialect.py"
        cp "${TASK_ABS}" "${SNAP}/task.md"
        copied=3
        ( cd "${SNAP}" && md5sum run_scenarios.py scenario_dialect.py task.md ) > "${MD5F}"
        # The arm is only a cold start if the corpus is absent, so say so rather than
        # trust it: one stray page next to the working directory invalidates the run.
        local stray
        stray="$(find "${SNAP}" -type f \
                    ! -path "${SNAP}/run_scenarios.py" \
                    ! -path "${SNAP}/scenario_dialect.py" \
                    ! -path "${SNAP}/task.md" | head -5)"
        [ -z "${stray}" ] || die "the gRPC arm staged more than the scenario runner, its dialect and the task:
${stray}"
    else
        copied="$(snapshot "${SDK}" "${SNAP}")"
        # The agent may read the snapshot and nothing else, so a task the snapshot
        # does not carry -- outside the checkout, or ignored by git -- is copied into it.
        local TASK_OWN=""
        case "${TASK_ABS}" in "${SDK}/"*) [ -f "${SNAP}/${TASK_ABS#"${SDK}/"}" ] || TASK_OWN=1 ;; *) TASK_OWN=1 ;; esac
        [ -z "${TASK_OWN}" ] || cp "${TASK_ABS}" "${SNAP}/task.md"
        ( cd "${SNAP}" && md5sum AGENTS.md docs/agent/*.md docs/agent/*.json docs/agent/.budgets \
                               tools/agent/*.py tools/agent/evals/tasks.json \
                               conf/cmake/functions.cmake examples/ai-benchmark/*.md \
                               examples/ai-benchmark/*.txt ${TASK_OWN:+task.md} ) > "${MD5F}"
    fi

    # A task is read from the snapshot: where the checkout has it, or as task.md.
    local TASK_RUN="${SNAP}/task.md"
    if [ -z "${TASK_OWN:-}" ] && [ "${FRAMEWORK}" != "grpc" ]; then
        TASK_RUN="${SNAP}/${TASK_ABS#"${SDK}/"}"
    fi

    # What the prompt may call the procedure. The gRPC arm has no runbook and may
    # not be told that one exists anywhere.
    local GUIDE="the runbook"
    [ "${FRAMEWORK}" != "grpc" ] || GUIDE="the task file"
    local ADD_DIR="${SNAP}" RULES=""
    if [ "${FRAMEWORK}" = "grpc" ]; then
        # The task and runner share the isolated directory granted to the agent.
        TASK_RUN="${SNAP}/task.md"
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

- **This task has no fix bound.** Wherever ${GUIDE} or this prompt says at most 3
  build-and-fix or run-and-fix cycles, there is no limit. Fix the cause, never the
  symptom, and never loosen what a scenario expects."
    elif [ "${ATTEMPTS}" != "3" ]; then
        RULES="${RULES}

- **The maximum fix bound for this task is ${ATTEMPTS}.** Wherever ${GUIDE} or this
  prompt says at most 3 build-and-fix cycles or at most 3 run-and-fix cycles, read
  ${ATTEMPTS}. Everything else about the bound is unchanged: fix the cause, never the
  symptom, and never loosen what a scenario expects."
    fi

    if [ -n "${DEBRIEF}" ] && [ "${FRAMEWORK}" = "grpc" ]; then
        # The same five questions the other arm is asked, with every one that names
        # a tool, a page or an artefact of the SDK removed: naming one would tell
        # this arm the SDK exists.
        RULES="${RULES}

Additionally, for this run only -- a diagnostic pass the normal task does not ask
for. Do it last, after the report, and never let it change what you built:

- **Every document you opened or fetched, in order, with the request you opened it
  at and why**, and what sent you to it.
- **Every question you answered from your own training rather than from a document**,
  and what you would have needed to read to answer it from documentation.
- **Every place two sources said different things**, naming both, and which one
  you followed.
- **Anything you looked for and could not find** -- a signature, a rule, an example
  -- and where you looked first.
- **Every file under the project's own src/ or build/ you opened or searched**, with
  the request, the question it was meant to answer, and whether your .proto or the
  stubs generated from it already answered it.
- **Everything you opened before the first build**: what in the task made you open
  it then, rather than after the stubs were generated.
- **Every command you ran to learn a syntax, a name or a signature**, and whether
  its answer was enough or you had to look again elsewhere.

Be specific and short: a list, not prose."
    elif [ -n "${DEBRIEF}" ]; then
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
- **Every file under the project's src/ or build/ you opened or searched**, with
  the request, the question it was meant to answer, and whether worksheet.txt
  already answered it.
- **Every page you opened before the first build_project.py call**: what in the
  task or in AGENTS.md made you open it then, rather than after generation.
- **Every tool you ran to learn a syntax, a name or a signature**, and whether its
  answer was enough or you had to look again elsewhere.

Be specific and short: a list, not prose."
    fi

    # The prompt is the wrapper a user pastes, with its values substituted, so the
    # benchmark measures exactly what a user runs.
    local BODY
    BODY="$(sed -n '/^--- PROMPT BEGINS BELOW THIS LINE/,$p' "${WRAPPER_ABS}" \
             | tail -n +2 | sed '/./,$!d')"
    [ -n "${BODY}" ] || die "${WRAPPER_ABS} has no '--- PROMPT BEGINS BELOW THIS LINE' marker"
    BODY="${BODY//<areg-sdk>/${SNAP}}"
    local RUNNER="${SNAP}/tools/agent/run_scenarios.py"
    [ "${FRAMEWORK}" != "grpc" ] || RUNNER="${SNAP}/run_scenarios.py"
    BODY="${BODY//<runner>/${RUNNER}}"
    BODY="${BODY//<task>/${TASK_RUN}}"
    BODY="${BODY//<project>/${PROJECT}}"
    BODY="${BODY//<mode>/${MODE}}"
    { printf '%s\n' "${BODY}"; printf '\n%s\n' "${RULES}"; } > "${RUN}/prompt.txt"

    local ISOLATION="snapshot; CLI user configuration may load (see README.md)"
    [ "${AGENT}" != "claude" ] || ISOLATION="snapshot, no skills, no MCP servers"
    # Only claude and copilot are told whether they may browse. What codex and gemini
    # reach is their own sandbox's business, so the line records the intent, not a
    # guarantee, and says which it is.
    case "${AGENT}" in
        claude|copilot) : ;;
        *) ISOLATION="${ISOLATION}; web ${WEB} is not enforced for ${AGENT}" ;;
    esac
    { echo "framework ${FRAMEWORK}"; echo "agent    ${AGENT}"
      echo "model    ${MODEL:-agent-default}"; echo "effort   ${EFFORT:-agent-default}"
      echo "task     ${TASK_RUN}"; echo "mode     ${MODE}"; echo "recipes  ${RECIPES}"
      echo "attempts ${ATTEMPTS}"; echo "debrief  ${DEBRIEF:-no}"
      if [ "${FRAMEWORK}" = "grpc" ]; then echo "staged   ${SNAP}"
      else echo "source   ${SDK}"; echo "sdk      ${SNAP}"; fi
      echo "files    ${copied}"
      echo "head     $(cat "${HEADF}")"
      echo "web      ${WEB}"; echo "isolation ${ISOLATION}"
      date -u +"start    %Y-%m-%dT%H:%M:%SZ"; } > "${RUN}/meta.txt"

    # A cold start is a claim about everything the agent can reach, so check that
    # and not one word in one file: the prompt it is given, every staged file, and
    # the directory beside its own. A tool or a document of the SDK named in any of
    # them has told the arm that the SDK exists.
    if [ "${FRAMEWORK}" = "grpc" ]; then
        local leak
        leak="$( { grep -rniE "${COLD_START_BAN}" "${RUN}/prompt.txt" "${SNAP}" \
                        "${RUN}/meta.txt" "${RUN}/toolchain.txt" 2>/dev/null
                   ls -A "${RUN}" | grep -iE "${COLD_START_BAN}" | sed 's|^|beside the project: |'
                   printf '%s\n' "${RUN}" | grep -iE "${COLD_START_BAN}" \
                        | sed 's|^|the run directory itself: |'; } | head -8 )"
        [ -z "${leak}" ] || die "the gRPC arm is not a cold start -- it names what it exists not to know:
${leak}
  Nothing the agent can reach may carry these words. Check ${WRAPPER_ABS}, the task
  file, and the directory the run was started in."
    fi

    if [ -n "${DRY}" ]; then
        echo "staged ${RUN} (${copied} files in the snapshot)"
        echo "--- prompt.txt ---"
        cat "${RUN}/prompt.txt"
        exit 0
    fi

    echo "run-benchmark: ${RUN}"
    echo "run-benchmark: ${FRAMEWORK}, ${AGENT}, ${MODEL:-agent-default}, effort ${EFFORT:-agent-default}, attempts ${ATTEMPTS}, head $(cut -c1-8 "${HEADF}"), ${copied} files"

    cd "${WORK}"
    local code=0
    local RESULT="${RUN}/result.json"
    local -a agent_args=()
    # ADD_DIR, never RUN: RUN is the parent of the snapshot and holds meta.txt, which
    # names the source checkout. Handing that to the gRPC arm tells it what it exists
    # not to know, and handing it to any arm shows one agent what the others cannot see.
    effort_args "${AGENT}"
    local TOOLS="Bash Read Write Edit Glob Grep"
    [ "${WEB}" = "off" ] || TOOLS="${TOOLS} WebSearch WebFetch"
    case "${AGENT}" in
        claude)
            agent_args=(-p --output-format json
                        --disable-slash-commands --strict-mcp-config
                        --allowedTools "${TOOLS}" --add-dir "${ADD_DIR}")
            ;;
        copilot)
            agent_args=(--allow-all-tools --disable-builtin-mcps --no-custom-instructions
                        --no-ask-user
                        --add-dir "${ADD_DIR}" --usage-output-file "${RUN}/result.json")
            if [ "${WEB}" = "off" ]; then
                # Named out rather than allow-listed: the CLI accepts a tool name it
                # does not have, so an allow-list with one typo would disarm the agent
                # halfway through a paid run, while an exclusion that misses is inert.
                agent_args+=(--excluded-tools=web_search,web_fetch,fetch)
            else
                agent_args+=(--allow-all-urls)
            fi
            RESULT="${RUN}/run.out"
            ;;
        codex)
            agent_args=(--ask-for-approval never exec --sandbox workspace-write
                        --skip-git-repo-check --add-dir "${ADD_DIR}" --json)
            RESULT="${RUN}/result.jsonl"
            ;;
        gemini)
            agent_args=(--output-format json --approval-mode yolo --include-directories "${ADD_DIR}")
            ;;
    esac
    [ ${#EFFORT_ARGS[@]} -eq 0 ] || agent_args+=("${EFFORT_ARGS[@]}")
    [ -z "${MODEL}" ] || agent_args+=(--model "${MODEL}")
    [ "${AGENT}" != "codex" ] || agent_args+=(-)
    # Claude's first build can outlast its default command timeout.
    BASH_DEFAULT_TIMEOUT_MS=600000 BASH_MAX_TIMEOUT_MS=900000 \
        "${AGENT}" "${agent_args[@]}" \
        < "${RUN}/prompt.txt" > "${RESULT}" 2> "${RUN}/run.err" || code=$?
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
    if [ -s "${MD5F}" ]; then
        local changed
        changed="$( cd "${SNAP}" && md5sum -c "${MD5F}" 2>/dev/null \
                    | grep -v ': OK$' || true )"
        if [ -n "${changed}" ]; then
            echo "corpus:  CHANGED DURING THE RUN -- this measurement is not valid" >&2
            echo "${changed}" >&2
            echo "corpus   changed during the run" >> "${RUN}/meta.txt"
            if [ "${code}" -eq 0 ]; then code=3; fi
        else
            echo "corpus:  unchanged, $(wc -l < "${MD5F}") file(s) verified"
            echo "corpus   unchanged" >> "${RUN}/meta.txt"
        fi
    fi

    # Which configuration the run actually built. CMAKE_BUILD_TYPE does not answer
    # this, and a run compared across a different one measures the configuration.
    echo "config   $( python3 "${HERE}/build_config.py" "${RUN}" 2>/dev/null \
                      || echo unknown )" >> "${RUN}/meta.txt"

    echo
    if [ "${code}" -eq 0 ] && [ ! -s "${RESULT}" ]; then
        echo "run-benchmark: ${AGENT} returned no output; see ${RUN}/run.err" >&2
        code=4
    fi
    # A run that failed is the one most worth reading, and the analysis is what says
    # why. It runs whatever the exit code was, and it never changes that code.
    case "${AGENT}" in
        claude)  python3 "${HERE}/analyze_run.py" "${RUN}" --record || true ;;
        copilot) python3 "${HERE}/measure.py" "${RUN}/result.json" || true ;;
        *) echo "usage: native metrics in ${RESULT}; no cross-agent cost conversion" ;;
    esac

    if [ "${VERIFY}" != "none" ]; then
        local SANITIZE=""
        if [ "${VERIFY}" = "sanitize" ]; then SANITIZE="--sanitize"; fi
        echo
        python3 "${HERE}/verify_run.py" "${RUN}" ${SANITIZE} || true
    fi
    exit ${code}
}

main "$@"
