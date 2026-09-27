# AI benchmark

Task prompts written for an AI agent, the wrappers that hand them to one framework, and
the scripts that run an agent against them and measure the result.

The benchmark asks one question: **can an agent that has never seen a framework build a
correct application from that framework's own documentation, and what does it cost?**
areg is measured against a framework the model already knows from training (gRPC): the
same requirements, the same agent, the same model and the same effort. The first
measured pair is [`baseline-2026-09-13.md`](baseline-2026-09-13.md).

- [Quick start](#quick-start)
- [What is here](#what-is-here)
- [The task prompts](#the-task-prompts)
- [What each path option expects](#what-each-path-option-expects)
- [Use cases, by agent](#use-cases-by-agent)
- [Changing the framework](#changing-the-framework)
- [The options, one by one](#the-options-one-by-one)
- [What a run does, and what it leaves](#what-a-run-does-and-what-it-leaves)
- [Reading and judging a run](#reading-and-judging-a-run)
- [Running one by hand, with any agent](#running-one-by-hand-with-any-agent)
- [Troubleshooting](#troubleshooting)
- [Writing a prompt of your own](#writing-a-prompt-of-your-own)

---

## Quick start

Start a run from an empty directory **outside** the checkout: the run directory is
made where you stand.

```bash
mkdir -p ~/runs && cd ~/runs
/path/to/areg-sdk/examples/ai-benchmark/run-benchmark.sh --attempts 15
```

On Windows, in PowerShell 5.1 or 7:

```powershell
mkdir C:\runs; cd C:\runs
C:\src\areg-sdk\examples\ai-benchmark\run-benchmark.ps1 --attempts 15
```

That is areg, the coffee machine, Claude Code with `sonnet` at `medium` effort. Every
other run is the same command with options added. Both scripts take the same options,
so every example below works on Windows with the script name changed. If scripts are
blocked, start it as
`powershell -ExecutionPolicy Bypass -File C:\src\areg-sdk\examples\ai-benchmark\run-benchmark.ps1 ...`.

Print what a run would send, and spend nothing:

```bash
run-benchmark.sh --task examples/ai-benchmark/prompt-elevator.md --dry-run
```

---

## What is here

| File | What it is | Who edits it |
|---|---|---|
| `prompt-*.md` | the task prompts, [listed below](#the-task-prompts) | nobody; they are the measuring instrument |
| `areg-ai-prompt-template.txt` | the areg wrapper for any task | **you**, four values at the top, when running by hand |
| `areg-coffeemachine-prompt.txt` | the areg wrapper for `prompt-coffeemachine.md` | as above |
| `grpc-ai-prompt-template.txt` | the gRPC wrapper for any task | **you**, three values at the top, when running by hand |
| `grpc-coffeemachine-prompt.txt` | the gRPC wrapper for `prompt-coffeemachine.md`, the one the published pair used | as above |
| `grpc-scenario-dialect.py` | the scenario runner's remedies in gRPC words, staged into the gRPC arm | nobody |
| `runbook-demo.md` | what a measured areg run adds to `docs/agent/01-runbook.md` | nobody |
| `run-benchmark.sh` | one cold, isolated, measured run in one command: Linux, macOS, WSL | nobody |
| `run-benchmark.ps1` | the same for Windows PowerShell 5.1 and PowerShell 7: the same options and the same run directory | nobody |
| `analyze_run.py` | reads a Claude run request by request: cost, reasoning, cycles, pages read, lines written | nobody |
| `measure.py` | reads a Claude Code or GitHub Copilot result file and scores it against a budget | nobody |
| `verify_run.py` | the hidden acceptance probes, run on the finished project; the agent never sees it | nobody |
| `build_config.py` | records the configuration a run actually built (Debug or Release) as one line of `meta.txt` | nobody |
| `INSTALL-grpc.md` | installing the gRPC toolchain, before the first gRPC run | **you**, once |
| `baseline-2026-09-13.md` | the first areg and gRPC pair, side by side | -- |

---

## The task prompts

Each prompt builds one small but complete system: two or three programs that talk to
each other, a scenario that proves it end to end, and a run in which one side is taken
away. Each one is chosen for a different area of what a framework must get right.

| Prompt | Builds | What it exercises |
|---|---|---|
| `prompt-tempalarm.md` | a temperature monitor and a simulated operator | the plain service shape: a request, a published value, broadcasts. No state machine |
| `prompt-coffeemachine.md` | a coffee machine and a simulated user | a state machine: nested states, guarded transitions, resuming an interrupted sequence |
| `prompt-atm.md` | an ATM and a simulated customer | one retry-limited check reached from two places, each with its own attempt count, written by hand |
| `prompt-atm-fsm.md` | the same ATM, same checklist | the same behaviour as a declared machine: what a declaration costs against hand-written control flow |
| `prompt-printscan.md` | a multifunction device and a simulated operator | two engines scheduled one job at a time, reused by a copy job, faults reported one way |
| `prompt-elevator.md` | an elevator controller, a simulated passenger and a floor display | **three programs**: a machine with a nested door sequence entered at a later step, halt-and-resume from the exact step, provider-side timing, two clients watching at once |
| `prompt-sensorgateway.md` | a sensor gateway and a simulated monitor | a worker thread handing readings to a component as messages; structured shared types (records, enumerations, lists); **one state machine driven from two threads** |
| `prompt-greenhouse.md` | a zone host and a simulated console | three instances of one controller reached by name; **moving an instance into another program by configuration, with no code change** |

**Where to start.** Run `prompt-tempalarm.md` and `prompt-coffeemachine.md` first, in
that order: the first costs what the plain service path costs, the second adds a state
machine to the same shape. Two points make a slope. The others are larger tasks.

**The pair.** `prompt-atm.md` and `prompt-atm-fsm.md` ask for the same behaviour twice:
once with the session written by hand and a declared state machine ruled out, once with
it declared. Compare the cost against the hand-written body count: the hand-written
arm is cheaper to reach and larger to keep.

**The three newer prompts** each aim at one ability:

- `prompt-elevator.md` checks that an agent can express "enter the same sequence at a
  later step" and "resume exactly where it halted" **inside the machine**, rather than
  duplicating states or keeping a hand-saved copy of the state. It is also the only
  task with three programs, so two clients watch one provider at once.
- `prompt-sensorgateway.md` checks thread discipline: a thread of the application's
  own feeds a component by messages only, and a shared state machine takes input from
  that thread and from the serving side without being corrupted.
- `prompt-greenhouse.md` is **location transparency**: the same controller runs as
  three named instances, and one of them moves from the host program into the
  console's own process by a configuration change, with the scenario passing in both
  deployments from the same build. This is a design where the framework decides
  most of the cost. It is a good gRPC comparison: there, an in-process instance is a
  different code path from a remote one.

**Every task prompt names no framework and no operating system.** Each one holds the
same sections, and nothing about how to build it:

| Section | Holds |
|---|---|
| `## The task` | the programs, what each must do, and the scenario the simulated side runs |
| `### Proving it` | the normal run, and the run in which the other side is taken away |
| `### Acceptance checklist` | the items any implementation, in any framework, is scored against |
| `## What to deliver` | the programs, a contract declared once and generated from, the stop rule |
| `## The report` | one small table the agent can fill without measuring anything |

That is what lets the same requirements be scored against gRPC, ZeroMQ, DDS or areg.
`python3 tools/agent/check_corpus.py` fails if a framework name, an operating system
name, a tool name or a build command gets into one of them.

---

## What each path option expects

The scripts search for nothing: a path is given, inherited from an environment
variable, found where the script sits, or asked for on the terminal. When there is no
terminal to ask on, the script refuses rather than guess.

| Option | Environment variable | Must point at | Not at |
|---|---|---|---|
| `--sdk DIR` | `AREG_SDK_ROOT` | the **root of an areg-sdk git checkout**: the directory that holds `AGENTS.md`. The source tree, not a build. Default: the checkout the script sits in | an install prefix, an `include/` directory, a `build/` directory, or a copy that is not a git checkout |
| `--grpc DIR` | `AREG_GRPC_ROOT` | either **the directory holding `protoc` and `grpc_cpp_plugin`**, or **the install prefix** whose `bin/` holds them. Default: both tools found on the `PATH` | a gRPC source tree, a build directory, or a vcpkg tree (see [Troubleshooting](#troubleshooting)) |
| `--task PATH` | -- | a `prompt-<key>.md` file, absolute or relative to `--sdk`. One the snapshot does not carry -- outside the checkout, or ignored by git -- is copied into the snapshot root under the name task.md, and fingerprinted | -- |
| `--wrapper PATH` | -- | a wrapper file, absolute or relative to `--sdk` | -- |

**What areg needs from `--sdk`.** The source code, the agent documentation and the
tools, and nothing prebuilt. The script copies the tracked files, plus the untracked
files that are not ignored, into the run's `sdk/`. The agent's project then compiles
the framework from that copy, with its own compiler, on its first build. So an
uncommitted change in your checkout **is** measured, and an ignored file is not.
Headers and binaries from somewhere else never enter the run. For that reason an
**installed** areg package (one that `find_package(areg)` finds) makes the script
refuse: the project would build against it instead of the snapshot.
`--allow-installed-areg` measures the installed package on purpose.

**What gRPC needs from `--grpc`.** Two tools and a CMake package, all prebuilt:

| Given | The script does | You must have done |
|---|---|---|
| nothing | uses `protoc` and `grpc_cpp_plugin` from the `PATH` | CMake finds `gRPCConfig.cmake` and `protobuf-config.cmake` by itself |
| the directory holding the tools | puts it first on the `PATH` | the same: CMake is not told anything |
| an install prefix (`<prefix>/bin/protoc`) | puts `<prefix>/bin` first on the `PATH` **and** `<prefix>` first on `CMAKE_PREFIX_PATH` | nothing else, if the prefix holds `lib/cmake/grpc/` and the protobuf package |

The prefix form is the one to use whenever gRPC is not in a system location. Typical
prefixes:

| Install | Pass |
|---|---|
| Debian, Ubuntu, Fedora, Arch packages | nothing: the tools and the CMake packages are in system locations |
| Homebrew | nothing, or `--grpc "$(brew --prefix)"` |
| conda-forge on Linux or macOS | `--grpc "$CONDA_PREFIX"` |
| conda-forge on Windows | `--grpc "$env:CONDA_PREFIX\Library"` |
| built and installed by hand with `cmake --install` | `--grpc <the CMAKE_INSTALL_PREFIX used>` |

[`INSTALL-grpc.md`](INSTALL-grpc.md) installs each of these, and ends with a
three-line CMake probe that proves CMake finds the packages. Run it from the shell that
will start the benchmark.

**Nothing is installed during a run.** An agent that installs its framework measures the
install. Both arms get a toolchain that is ready before the agent starts.

---

## Use cases, by agent

`--agent` selects the CLI; `--model` selects the model inside that CLI. Only the
selected CLI must be installed and logged in. The model names below are examples, not
a supported list or a promise of access: the CLI validates them. Pin an exact model ID
rather than a moving alias when two runs are to be compared.

`--effort low|medium|high` is one vocabulary for every CLI. It becomes Claude's
`--effort`, Copilot's `--reasoning-effort` or Codex's `model_reasoning_effort`, and
Gemini refuses it rather than pretend. The levels are not calibrated equivalents
across providers.

### Claude Code (`--agent claude`, the default)

Default model `sonnet`, default effort `medium`. Aliases (`sonnet`, `opus`, `haiku`) or
full IDs (`claude-sonnet-5`, `claude-opus-5`) are accepted.

```bash
# The two starting tasks, areg
run-benchmark.sh --task examples/ai-benchmark/prompt-tempalarm.md     --attempts 15
run-benchmark.sh --task examples/ai-benchmark/prompt-coffeemachine.md --attempts 15

# The same task, both arms: the comparison
run-benchmark.sh                  --task examples/ai-benchmark/prompt-greenhouse.md --attempts 15
run-benchmark.sh --framework grpc --task examples/ai-benchmark/prompt-greenhouse.md --attempts 15

# Another model and effort
run-benchmark.sh --model opus --effort high --attempts 15

# What the documentation failed to answer: a debrief after the report.
# It costs requests on purpose, so never compare it with a normal run
run-benchmark.sh --task examples/ai-benchmark/prompt-elevator.md --debrief

# Three draws of one arm on one tree: compare medians, never one run
for label in a b c; do run-benchmark.sh "$label" --attempts 15; done
```

Only a Claude run is read by `analyze_run.py` automatically at the end: cost and its
split, requests, reasoning, cycles, pages opened, lines written.

### GitHub Copilot CLI (`--agent copilot`)

Copilot offers models from several providers. The most used IDs:

| Model | `--model` |
|---|---|
| automatic choice | `auto` |
| Claude Sonnet 5 | `claude-sonnet-5` |
| Claude Opus 5 | `claude-opus-5` |
| GPT-5.6 Terra | `gpt-5.6-terra` |
| GPT-5.6 Sol | `gpt-5.6-sol` |
| GPT-5.5 | `gpt-5.5` |
| GPT-5.4 Mini | `gpt-5.4-mini` |
| Gemini 3.5 Flash | `gemini-3.5-flash` |

Display names ("Terra", "Sol") are not IDs; `/model` in the Copilot CLI lists what
your account may use. Omit `--model` and `--effort` to leave both to the CLI;
`meta.txt` then records `agent-default`.

```bash
run-benchmark.sh --agent copilot --model claude-sonnet-5 --effort medium --attempts 15
run-benchmark.sh --agent copilot --model gpt-5.6-terra --task examples/ai-benchmark/prompt-sensorgateway.md
run-benchmark.sh --agent copilot --model gpt-5.6-sol --framework grpc --attempts 15

# The same model on two CLIs: tells the harness apart from the model
run-benchmark.sh --agent claude  --model claude-sonnet-5 --attempts 15
run-benchmark.sh --agent copilot --model claude-sonnet-5 --attempts 15
```

Copilot writes its answer to `run.out` and its usage to `result.json`, and the run ends
with `measure.py`. It bills **AI credits (AIC)**: the report prints them from
`totalNanoAiu / 1,000,000,000`, and converts once at USD 0.01 per AIC on a row that
says so. Premium requests are not converted. Its own system prompt is large -- a
one-word prompt bills about 22,500 input tokens -- which is the floor of any Copilot
run.

### OpenAI Codex CLI (`--agent codex`)

```bash
run-benchmark.sh --agent codex --model gpt-5.4 --effort high --attempts 15
run-benchmark.sh --agent codex --model gpt-5.3-codex --task examples/ai-benchmark/prompt-elevator.md
```

Codex runs with `workspace-write` and approvals disabled; its sandbox may block
downloads. It keeps `result.jsonl`, with its native usage events; no report is
computed from it.

### Gemini CLI (`--agent gemini`)

```bash
run-benchmark.sh --agent gemini --model gemini-2.5-flash
run-benchmark.sh --agent gemini --model gemini-2.5-flash --framework grpc
```

`--effort` is refused. Gemini runs in `yolo` approval mode and keeps `result.json`,
with its response and stats.

### How the agents differ in the harness

What is held constant is the prompt, the snapshot and the fix bound. The harness is
not, and a difference in cost between two CLIs is not only the model's.

| | Claude | Copilot | Codex | Gemini |
|---|---|---|---|---|
| tools | six-tool allowlist | unattended tools, `ask_user` off | `workspace-write` sandbox | `yolo` |
| skills and MCP | off | built-in MCP and custom instructions off; user MCP and skills may load | user settings kept | user settings kept |
| web off | `WebSearch`, `WebFetch` withheld | web tools excluded | recorded, not enforced | recorded, not enforced |
| usage report | `analyze_run.py` | `measure.py` | none; native `result.jsonl` | none; native `result.json` |

Run only in a disposable environment, with trusted prompts: a snapshot is not a
security sandbox. Allow the build commands; **every permission denial costs the agent
a turn**, and a denial-heavy run is not comparable with a clean one.

---

## Changing the framework

### areg or gRPC: `--framework`

```bash
run-benchmark.sh --framework areg ...     # the default
run-benchmark.sh --framework grpc ...
```

| | areg arm | gRPC arm |
|---|---|---|
| the agent may read | `sdk/`: the snapshot of `--sdk` | `task/`: the task, the scenario runner and its gRPC dialect. Nothing else |
| the framework comes from | the snapshot, compiled by the agent's first build | the prebuilt install `--grpc` names |
| the web | **off** by default | **on** by default |
| the wrapper | `areg-<key>-prompt.txt`, else `areg-ai-prompt-template.txt` | `grpc-<key>-prompt.txt`, else `grpc-ai-prompt-template.txt` |
| the run directory | `<date><label>-<project>` | `<date><label>-grpc-<project>` |

**The web is on for gRPC and off for areg, and that is the fair setting.** What is held
constant is not the list of tool flags: it is that each arm has its framework's
documentation, in the place that documentation lives. areg ships it in the snapshot,
and the snapshot is what is under test. An arm that may browse can read the
published copy instead, and nothing can detect it. gRPC ships it on the web, and its
prompt says "you have gRPC and its public documentation". Deny it the web and that
sentence is false, and the run measures recall. `--web on|off` overrides either
side, and `meta.txt` records which was used.

**The gRPC arm is a cold start by check, not by intention.** Before the agent starts,
the script reads the prompt, every staged file, every name beside the project and the
run path itself, and refuses to run if any carries an areg name, a tool of the areg
corpus or one of its file types. The provenance that names the checkout goes to
`provenance/`, outside the agent's reach. Run gRPC from a directory whose path does not
contain "areg": `~/runs` is fine, `~/areg-runs` is refused.

**Each arm is told what to do in its own words.** The scenario runner
`run_scenarios.py` names no framework; its remedies come from `scenario_dialect.py`
beside it -- `tools/agent/scenario_dialect.py` for areg, `grpc-scenario-dialect.py`
staged under that name for gRPC.

### Another areg tree: `--sdk`

Measure a branch, a fork, or a clone with uncommitted changes, without touching the
checkout the script sits in:

```bash
git clone --branch my-branch https://github.com/aregtech/areg-sdk.git ~/src/areg-try
cd ~/runs && ~/src/areg-sdk/examples/ai-benchmark/run-benchmark.sh --sdk ~/src/areg-try
```

The run records `sdk-head.txt` (the commit) and `sdk-md5.txt` (every file the agent
could read), so a run says exactly which tree it measured. Two runs compare only when
those match or the difference is the thing under test.

### Another gRPC install: `--grpc`

```bash
run-benchmark.sh --framework grpc --grpc /opt/grpc-1.66          # a prefix
run-benchmark.sh --framework grpc --grpc "$CONDA_PREFIX"         # conda, Linux or macOS
```

```powershell
.\run-benchmark.ps1 --framework grpc --grpc "$env:CONDA_PREFIX\Library"   # conda, Windows
```

`toolchain.txt` in the run records the `protoc` and plugin versions used.

### A third framework

The task prompts are ready for it; the scripts are not. `--framework` accepts `areg` and
`grpc` only, so a third arm is a change to both scripts: the value, the staging of what
the agent may read, and the cold-start ban. The prompt half is
`<framework>-ai-prompt-template.txt`, copied from `grpc-ai-prompt-template.txt` with
its numbered steps rewritten for that framework's normal workflow, and a
`<framework>-scenario-dialect.py`. Until then, run it [by hand](#running-one-by-hand-with-any-agent).

### A different wrapper: `--wrapper`

A wrapper is the framework half of a prompt. Everything above its line
`--- PROMPT BEGINS BELOW THIS LINE` is for the operator; everything below is what the
agent receives, with these placeholders filled in:

| Placeholder | Becomes |
|---|---|
| `<areg-sdk>` | the absolute path of the snapshot the agent reads (areg only) |
| `<runner>` | the absolute path of `run_scenarios.py` |
| `<task>` | the absolute path of the task prompt |
| `<project>` | the project name, a C identifier |
| `<mode>` | `ipc`, `local` or `pubsub` (areg only) |

The areg wrappers may name anything in the areg checkout and no operating system. The
gRPC wrappers name gRPC and protobuf only -- no areg file, concept or other arm -- and
`check_corpus.py` fails if they do. The rule is symmetric: the areg arm names no gRPC
either, so neither agent is sent looking for the other framework.

---

## The options, one by one

`run-benchmark.sh --help` prints the same list.

| Option | What it changes | Default | Example |
|---|---|---|---|
| `label` | the letter after the date in the run directory name; a label already used today is refused | next free letter | `run-benchmark.sh b` |
| `--framework` | which arm | `areg` | `--framework grpc` |
| `--agent` | which installed CLI runs the task | `claude` | `--agent copilot` |
| `--model` | any model ID or alias the CLI accepts | `sonnet` for Claude, the CLI's otherwise | `--model claude-opus-5` |
| `--effort` | reasoning effort; refused for Gemini | `medium` for Claude, the CLI's otherwise | `--effort high` |
| `--task` | the task prompt; `prompt-<key>.md` also sets the project name and the wrapper | `prompt-coffeemachine.md` | `--task examples/ai-benchmark/prompt-atm.md` |
| `--wrapper` | a wrapper other than the default | by name, then the template | `--wrapper ~/my-areg-wrapper.txt` |
| `--project` | the directory and CMake project name inside `work/`, and the run directory suffix | `<key>` | `--project atm2` |
| `--mode` | the areg application shape the scaffold writes: `ipc`, `local` or `pubsub` | `ipc` | `--mode local` |
| `--attempts` | the build-and-fix and run-and-fix bound, the same for both arms; `0` removes it and is warned about | `15` | `--attempts 15` |
| `--debrief` | a diagnostic pass after the report: what the run could not find | off | `--debrief` |
| `--verify` | the hidden probes after the run: `none`, `probes`, or `sanitize` for an ASan and UBSan rebuild as well | `probes` | `--verify sanitize` |
| `--recipes` | areg only: whether a documented recipe may be copied, `none` or `copy` | `none` | `--recipes copy` |
| `--web` | whether the agent may search and fetch pages | `off` areg, `on` gRPC | `--web on` |
| `--sdk` | the areg-sdk checkout, [see above](#what-each-path-option-expects) | this checkout | `--sdk ~/src/areg-sdk` |
| `--grpc` | the gRPC tools or prefix, [see above](#what-each-path-option-expects) | the `PATH` | `--grpc /usr/local` |
| `--dry-run` | stages everything and prints the prompt; starts no agent and spends nothing. The selected CLI must still be installed | off | `--dry-run` |
| `--allow-installed-areg` | measures against an installed areg package on purpose | off | `--allow-installed-areg` |

**Exit code.** `0` the agent finished; `2` the script refused to start, and says why;
`3` the agent changed a file it was measured against, so the run is not valid; `4` the
agent returned no output or the usage report failed. A non-zero exit of the agent
itself is passed through and can overlap these; see `meta.txt` and `run.err`.
Acceptance is in `verify.json`, never implied by a zero exit.

---

## What a run does, and what it leaves

1. Makes the run directory **in the current directory**, and refuses to start inside
   the checkout it snapshots.
2. Stages what the agent may read: `sdk/` for areg, `task/` for gRPC.
3. Builds `prompt.txt` from the wrapper, and records `meta.txt` and the fingerprints of
   everything the agent may read.
4. Starts the CLI headless in the empty `work/`, with `prompt.txt` on standard input.
5. After the agent ends: checks that it changed nothing it read, prints the usage
   report (Claude and Copilot), and runs the hidden probes on the project it built.

| In the run directory | Holds |
|---|---|
| `meta.txt` | framework, agent, model, effort, web, attempts, head, the time |
| `prompt.txt` | exactly what the agent received |
| `result.json` / `result.jsonl`, `run.out`, `run.err` | the CLI's own output and usage |
| `sdk/` or `task/` | what the agent could read |
| `sdk-head.txt`, `sdk-md5.txt`, `sdk-before.txt` | which tree, and proof it was not edited; `provenance/` in the gRPC arm, out of the agent's reach |
| `toolchain.txt` | gRPC arm: the `protoc` and plugin versions |
| `work/` | the project the agent built |
| `verify.json`, `verify-sanitize.json` | the probes' verdict and its evidence |

---

## Reading and judging a run

### The hidden probes

Every task's checklist asks for things a run in which everything works cannot show,
and an agent's own scenarios rarely try: a start in the wrong order, a peer that never
comes, a peer that dies at an unlucky moment, a spinning thread. Every agent reports
a full checklist, so the checklist alone cannot tell two implementations apart.

`verify_run.py <run directory>` runs the finished project against those requirements
after the agent has ended. The probes are generated from the project's own
`scenarios.json` -- the first scenario with two or more processes, no stop, and a lead
that exits 0 -- so they assume no framework, and the same probes score every arm. The
agent never sees them: the snapshot does not carry this file.

| Probe | Requirement it scores | Passes when |
|---|---|---|
| `repeat` | the scenario exits 0 | the normal scenario passes 10 times out of 10 |
| `start-order` | the client survives the server being started after it | the normal scenario passes with the lead started 3 s before the rest |
| `no-peer` | no program waits more than 20 seconds for something that never arrives | the lead alone exits non-zero within 30 s |
| `peer-loss` | if one side goes away mid-scenario, the other exits non-zero | with the first other process killed at 25%, 50% and 75% of a normal run, the lead exits non-zero each time and never hangs |
| `cpu` | no busy-waiting | a normal run averages under 0.5 cores (where the platform reports child CPU time) |
| `sanitize` (`--sanitize`) | no memory or undefined-behaviour defect -- not a checklist item | a rebuild under ASan and UBSan runs the normal scenario and one peer loss with no finding |

`verify.json` or `verify-sanitize.json` keeps each verdict with its evidence, so a
second verification does not overwrite the first. A framework compiled from source is
checked by the sanitizers along with the application; a prebuilt one is not.

```bash
python3 examples/ai-benchmark/analyze_run.py ./20260921c-coffeemachine
python3 examples/ai-benchmark/verify_run.py  ./20260921c-coffeemachine --sanitize
```

### The numbers

`analyze_run.py` prints, per Claude run: cost and its split, API requests, reasoning
tokens, cache reads and writes, peak context, build and scenario runs, the fix cycles,
filesystem searches, reads of framework sources, every page opened with its size, the
lines of C++ written by hand, and a per-request timeline.

**Never quote one run.** The same tree run twice has come out 45-80% apart in cost.
Run each arm at least three times on one tree and compare medians; the quality rows
(acceptance items, probes passing) are the ones that hold at one run.

**Compare the normalised line, not the bill.** The harness picks the prompt-cache TTL,
and a run started within an hour of another reads the static prefix from that run's
cache. The model is handed the same bytes either way; only the price differs.
`cost, billed` reconciles with `result.json`; `cost, cold @1h` puts both back and is
the one two runs can be held against each other.

**Keep billing units apart.** Claude Code reports USD. Copilot reports AI credits, and a
premium-request count is labelled legacy and never converted. Compare arms on
acceptance, probes, tokens and requests, with each bill in its own unit.

**Wall time compares only on one machine.** A framework compiled from source, or a slow
filesystem, moves it more than the framework does; model time is the fairer row.

### What an agent can report, and what it cannot

No agent is asked for a token count, a price or a wall time. Most harnesses never show
an agent its own usage, and an agent asked for one either writes "not available" or
spends turns computing a substitute.

| | who produces it | portable across harnesses |
|---|---|---|
| fix cycles, acceptance items, checker findings, files opened off-route | **the agent**, the report table in every task | **yes** |
| tokens, cache, cost, requests, wall time | **the operator**, afterwards, from what the harness records | no |

### Judging the result

Read the code. areg's `.siml`, `.fsml` and `.dtml` documents open in
[Lusan](https://github.com/aregtech/areg-sdk-tools), which draws the state machine, so
you can see whether the logic matches what was asked. Then run the scenarios again
yourself -- the agent's own count is a claim:

```bash
cd ./20260921c-coffeemachine/work
python3 ../sdk/tools/agent/run_scenarios.py --build build/bin
```

The number worth comparing is not tokens. It is **cost per application that builds and
passes its acceptance list and the probes**: a cheaper run that produces code nobody
can verify has not won anything.

---

## Running one by hand, with any agent

The same prompt works for any agent that can read files and run commands -- Cursor,
an IDE agent, or a CLI the scripts do not know -- on any operating system.

**1. Make one empty directory and start the agent in it.** It becomes the project. Not
your home directory, and not inside the checkout: a session started inside it loads
the SDK's maintainer instructions, which no user of areg ever sees.

**2. Write the prompt.** Copy the wrapper and fill in the values at its top.

| Arm | Copy | Fill in |
|---|---|---|
| areg, coffee machine | `areg-coffeemachine-prompt.txt` | `areg-sdk`, `task`, `project`, `mode` |
| areg, any other task | `areg-ai-prompt-template.txt` | `areg-sdk`, `task`, `project`, `mode` |
| gRPC, coffee machine | `grpc-coffeemachine-prompt.txt` | `task`, `project`, `runner` |
| gRPC, any other task | `grpc-ai-prompt-template.txt` | `task`, `project`, `runner` |

Replace every `<name>` below the prompt line with its value, or paste the whole file:
the agent is told that `<name>` means that value.

```
  areg-sdk  /path/to/areg-sdk
  task      /path/to/areg-sdk/examples/ai-benchmark/prompt-coffeemachine.md
  project   coffeemachine
  mode      ipc
```

**Give the agent a fresh clone**, not a checkout you work in: a working checkout
carries ignored files of its own, and the agent can read them.

**3. Run it, and keep what the harness records.** For Claude Code:

```bash
claude -p --output-format json --disable-slash-commands --strict-mcp-config \
       --add-dir /path/to/areg-sdk < prompt.txt > result.json
```

The two flags keep the operator's skills and MCP servers out of the run. The prompt goes
in on standard input because `--add-dir` takes every word after it as a directory. For
GitHub Copilot, keep its `--usage-output-file`.

**4. Read the numbers.**

```bash
python3 /path/to/areg-sdk/examples/ai-benchmark/measure.py result.json --project .
```

It prints cost, turns, wall time and the token split, counts the tool calls, and scores
the run against a budget of *fresh* tokens -- uncached input plus output:

| Task | Fresh tokens | Tool calls |
|---|---:|---:|
| `prompt-tempalarm.md` | 50K | ~20 |
| `prompt-coffeemachine.md` | 75K | ~30 |

**5. Score it:** build, run the scenarios, walk the checklist item by item, and run
`verify_run.py` on the directory that holds `work/`.

**6. Record it:** tokens, cost, requests, tool calls, fix cycles, checklist score, and
what the agent said it had to guess. That last one is where the next documentation fix
comes from.

---

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| `find_package(areg) resolves to an installed package` | an areg install on the system or on `CMAKE_PREFIX_PATH` would shadow the snapshot | uninstall it, clear it from `CMAKE_PREFIX_PATH`, or pass `--allow-installed-areg` to measure it on purpose |
| `... holds no AGENTS.md, so it is not an areg-sdk checkout` | `--sdk` names a build, an install or a subdirectory | name the checkout root |
| `... is not a git checkout` | `--sdk` names a copy or an extracted archive | clone it; the snapshot is taken from git's file list |
| refused because the path carries an areg name (gRPC) | the run directory or its parents contain "areg" | start the gRPC run from a neutral directory such as `~/runs` |
| `no protoc in DIR or DIR/bin` | `--grpc` names neither the tools' directory nor a prefix with `bin/` | name one of the two; see [the table](#what-each-path-option-expects) |
| the agent reports that `find_package(gRPC)` failed | the tools were found but CMake was not told where the packages are | pass the **prefix** to `--grpc` rather than its `bin/`, and run the probe in `INSTALL-grpc.md` from the same shell |
| gRPC through **vcpkg** on Windows | vcpkg keeps `protoc.exe` in `installed\<triplet>\tools\protobuf\` and the plugin in `tools\grpc\`, not in `bin\`, so neither `--grpc` form matches. It also compiles gRPC from source on first install | use conda-forge (`INSTALL-grpc.md`, route 1); or put both `tools\` directories on the `PATH`, set `CMAKE_PREFIX_PATH` to `installed\<triplet>`, and pass no `--grpc` |
| gRPC links fail with MSVC on Windows | not reproduced here. A likely cause: prebuilt gRPC is a Release build, and a Visual Studio build of the agent's project is Debug unless `--config Release` is given. The two runtimes do not link together | run the gRPC arm on Linux or WSL, or use the MSYS2 route; record it in the run's notes if you work around it |
| PowerShell will not start the script | the execution policy blocks it | `powershell -ExecutionPolicy Bypass -File ...\run-benchmark.ps1 ...` |
| `<cli> not found: install the selected CLI and log in` | the `--agent` CLI is not on the `PATH` | install and authenticate only that one |
| a run stops early with a rate-limit or 429 in `run.err` | the provider cut it | it is not a result: discard it and run again |
| the agent keeps asking for permission | the CLI prompts for a command the run needs | allow build and run commands; a denial-heavy run is not comparable |

**The easy route for gRPC on Windows is WSL.** The Linux packages in `INSTALL-grpc.md`
install in one command, and `run-benchmark.sh` inside WSL measures the same thing.
Start the run from a directory in the Linux filesystem (`~/runs`), not under `/mnt/c`:
a build on the Windows filesystem is several times slower, and wall time is recorded.

---

## Writing a prompt of your own

Prompts that produce something worth reading are welcome. One prompt per system, and
each one should make an agent use an ability the others do not.

- **Name it `prompt-<key>.md`**, with `<key>` a C identifier; it becomes the project
  name. The scripts pick up any such file with `--task`, and use the template wrappers
  unless a `<framework>-<key>-prompt.txt` sits beside it.
- **Keep the task free of framework, tool and operating-system vocabulary.** Say *"on
  resume the machine must continue from the stage it was interrupted in"*, not *"use a
  history state"*. Requirements, not mechanisms.
- **Keep the five sections** listed under [The task prompts](#the-task-prompts), keep
  the requirements the hidden probes score (start order, 20-second limit, the other
  side going away, no busy-waiting, exit codes), and add the file to `TASK_PROMPTS` in
  `tools/agent/check_corpus.py`, which then checks all of it.
- **Write an acceptance checklist someone could score without asking you what you
  meant**, and make every item observable in the output of the normal run or the
  peer-loss run.
- **Pin every number** the scenario checks -- values, timings, counts -- so two
  implementations can be held against the same expected output.

A prompt whose result cannot be checked is not a benchmark, it is a demonstration.
