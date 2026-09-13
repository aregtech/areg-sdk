# AI benchmark

Task prompts written for an AI agent, the wrappers that hand them to one framework, and
the scripts that run and measure an agent against them.

The benchmark asks one question: **can an agent that has never seen a framework build a
correct application from that framework's own documentation, and what does it cost?**
areg is measured against a framework the model already knows from training (gRPC), on
the same requirements, with the same agent, model and effort. The first measured pair is
[`baseline-2026-09-13.md`](baseline-2026-09-13.md).

- [What is here](#what-is-here)
- [The four task prompts](#the-four-task-prompts)
- [The wrappers](#the-wrappers)
- [Running with run-benchmark.sh](#running-with-run-benchmarksh)
- [Running one by hand, with any agent](#running-one-by-hand-with-any-agent)
- [What an agent can report, and what it cannot](#what-an-agent-can-report-and-what-it-cannot)
- [Judging the result](#judging-the-result)
- [Writing a prompt of your own](#writing-a-prompt-of-your-own)

---

## What is here

| File | What it is | Who edits it |
|---|---|---|
| `prompt-tempalarm.md` | task: temperature monitor with a threshold alarm | nobody; it is the measuring instrument |
| `prompt-coffeemachine.md` | task: coffee machine with a state machine | nobody |
| `prompt-atm.md` | task: ATM with PIN retries and card retention | nobody |
| `prompt-printscan.md` | task: multifunction printer with print, scan and copy | nobody |
| `areg-ai-prompt-template.txt` | the areg wrapper for any task | **you**, four values at the top, when running by hand |
| `areg-coffeemachine-prompt.txt` | the areg wrapper for `prompt-coffeemachine.md` | as above |
| `grpc-coffeemachine-prompt.txt` | the gRPC wrapper for `prompt-coffeemachine.md`; the worked example of another framework's arm | **you**, three values at the top, when running by hand |
| `runbook-demo.md` | what a measured areg run adds to `docs/agent/01-runbook.md` | nobody |
| `run-benchmark.sh` | one cold, isolated, measured run in one command | nobody |
| `run-benchmark.ps1` | the same, for Windows PowerShell 5.1 and PowerShell 7: the same options and the same run directory | nobody |
| `analyze_run.py` | reads a run request by request: cost, reasoning, cycles, pages read, lines written | nobody |
| `measure.py` | reads a Claude Code or GitHub Copilot result file and scores it against a budget | nobody |
| `verify_run.py` | the hidden acceptance probes, run on the finished project; the agent never sees it | nobody |
| `INSTALL-grpc.md` | the gRPC toolchain, before the first gRPC run | **you**, once |
| `baseline-2026-09-13.md` | the first areg and gRPC pair, side by side | -- |

---

## The four task prompts

| Prompt | Builds | Demonstrates |
|---|---|---|
| `prompt-tempalarm.md` | a temperature monitor and a simulated operator, in two processes | the plain service shape: a request, a published value, and broadcasts -- no state machine |
| `prompt-coffeemachine.md` | a coffee machine and a simulated user, in two processes | a state machine: nested states, guarded transitions, and resuming a sequence that was interrupted |
| `prompt-atm.md` | an ATM and a simulated customer, in two processes | one retry-limited check reached from two places, each with its own attempt count |
| `prompt-printscan.md` | a multifunction device and a simulated operator, in two processes | two engines scheduled one job at a time, reused by a copy job, with faults reported the same way |

Run the first two in that order. `prompt-tempalarm.md` is deliberately small and has no
state machine, so what it costs is what the plain service path costs;
`prompt-coffeemachine.md` adds the state machine on top of the same shape. Two points
make a slope. The other two are larger tasks of the same kind.

**Every task prompt names no framework and no operating system.** Each one holds the same
sections, and nothing about how to build it:

| Section | Holds |
|---|---|
| `## The task` | the two programs, what each must do, and the scenario the simulated side runs |
| `### Proving it` | the normal run, and the run in which the other side is taken away |
| `### Acceptance checklist` | the items any implementation, in any framework, is scored against |
| `## What to deliver` | two programs, a contract declared once and generated from, the stop rule |
| `## The report` | one small table the agent can fill without measuring anything |

That is what lets the same requirements be scored against gRPC, ZeroMQ, DDS or areg.
`python3 tools/agent/check_corpus.py` fails if a framework name, an operating system
name, a tool name or a build command gets into one of them.

---

## The wrappers

A wrapper is the framework half of a prompt. Everything above its line
`--- PROMPT BEGINS BELOW THIS LINE` is for the person running it; everything below is
what the agent receives, with these placeholders filled in:

| Placeholder | Becomes |
|---|---|
| `<areg-sdk>` | the absolute path of the areg checkout the agent reads (areg wrappers only) |
| `<runner>` | the absolute path of `run_scenarios.py`, the framework-neutral scenario runner |
| `<task>` | the absolute path of the task prompt |
| `<project>` | the project name, a C identifier |
| `<mode>` | `ipc`, `local` or `pubsub` (areg only) |

**Naming.** A wrapper written for one task is `<framework>-<key>-prompt.txt` for the task
`prompt-<key>.md`. `run-benchmark.sh` picks it up by that name. An areg task with no
wrapper of its own uses `areg-ai-prompt-template.txt`; a gRPC task with no wrapper of its
own refuses to start, because the gRPC steps are written for one task.

- **The areg wrappers** may name anything in the areg checkout. They name no operating
  system and no path outside the placeholders.
- **The gRPC wrapper** names gRPC and protobuf only. It names no areg file, no areg
  concept and no other arm, and `check_corpus.py` fails if it does. To measure another
  framework, copy it, rename it `<framework>-<key>-prompt.txt`, and rewrite its numbered
  steps for that framework's normal workflow.

---

## Running with run-benchmark.sh

### What it needs

- `git` and Python 3, and a shell to start it from:
  - Linux, macOS or WSL: `bash`, with `run-benchmark.sh`.
  - Windows: PowerShell 5.1 or 7, with `run-benchmark.ps1`. It takes the same options
    as the `.sh`, so every example below works with the name changed:
    `.\examples\ai-benchmark\run-benchmark.ps1 --framework grpc --attempts 15`. If
    scripts are blocked, start it as
    `powershell -ExecutionPolicy Bypass -File examples\ai-benchmark\run-benchmark.ps1 ...`.
- Claude Code, installed and logged in: `claude` on the `PATH`.
- CMake and a C++17 compiler.
- The areg arm: no installed areg package, or the run would build against it instead of
  the snapshot; the script checks this and refuses.
- The gRPC arm: `protoc` and `grpc_cpp_plugin` on the `PATH`, see
  [`INSTALL-grpc.md`](INSTALL-grpc.md).
- A git checkout of areg-sdk. The script is run from inside it and snapshots it.

### What it does

1. Makes a run directory outside the checkout: `<out>/<date><label>-<project>`, or
   `...-grpc-<project>` for the gRPC arm.
2. **Stages what the agent may read.**
   - areg: a snapshot of the checkout -- tracked files, and untracked files that are
     not ignored, as the working tree has them. Nothing local to the operator's
     machine (ignored notes, editor state) is copied.
   - gRPC: only `tools/agent/run_scenarios.py`, and the task. The script refuses to start
     if anything else is staged, or if the task or the prompt names areg.
3. Builds `prompt.txt` from the wrapper, with the placeholders filled in, and records
   `meta.txt` and the fingerprints of the files the agent reads.
4. Starts Claude Code headless in the empty `work/` directory, with no skills and no MCP
   servers, and the same tools for both arms.
5. After the run, verifies that the agent did not change what it read, runs
   `analyze_run.py` on the run, and then the [hidden acceptance
   probes](#hidden-acceptance-probes) on the project it built.

### Help

```
$ examples/ai-benchmark/run-benchmark.sh --help
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
  --verify MODE      none | probes | sanitize             (default: probes)
                       the hidden acceptance probes of verify_run.py, run on the
                       project after the agent ends. sanitize adds a rebuild under
                       ASan and UBSan.
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
    run-benchmark.sh --verify sanitize                areg, probes and sanitizers

  The run directory holds the measurement: meta.txt, prompt.txt, result.json,
  run.err, the fingerprints and sdk/, the snapshot the agent read. The agent works
  in work/, which starts empty.

  The agent is Claude Code, headless. Another agent is run by hand with the same
  prompt.txt; README.md says which of its numbers compare.
```

### The options, one by one

| Option | What it changes | Example |
|---|---|---|
| `label` | the letter after the date in the run directory name; runs with the same label on the same day are refused | `run-benchmark.sh b` |
| `--framework` | which arm: areg reads the snapshot, gRPC reads only the runner | `--framework grpc` |
| `--task` | the task prompt; its `prompt-<key>.md` name also sets the project name and the wrapper | `--task examples/ai-benchmark/prompt-atm.md` |
| `--wrapper` | a wrapper other than the default | `--wrapper /home/me/my-areg-wrapper.txt` |
| `--project` | the directory and CMake project name inside `work/`, and the run directory suffix | `--project atm2` |
| `--mode` | the areg application shape the scaffold writes | `--mode local` |
| `--model` | the model the agent runs on | `--model opus` |
| `--effort` | the agent's reasoning effort | `--effort high` |
| `--attempts` | the fix bound; the prompt says it, the same for both arms | `--attempts 15` |
| `--debrief` | adds a diagnostic pass after the report; never compare such a run with a normal one | `--debrief` |
| `--verify` | the hidden acceptance probes after the run: `none`, `probes`, or `sanitize` for an ASan and UBSan rebuild as well | `--verify sanitize` |
| `--recipes` | areg only: whether a documented recipe may be copied | `--recipes copy` |
| `--out` | where run directories are made; also `AREG_BENCHMARK_RUNS` | `--out /data/runs` |
| `--dry-run` | stages everything and prints the prompt, starts no agent and spends nothing. `claude` must still be installed | `--dry-run` |
| `--allow-installed-areg` | measures against an installed areg package on purpose | `--allow-installed-areg` |

**Exit code.** `0` the agent finished; `2` the script refused to start, and says why; `3`
the agent changed a file it was measured against, so the run is not valid; anything else
is the agent's own exit code.

### Copy and paste

Run from the root of the checkout. The fix bound of 15 is what the published pair used.

```bash
# The four prompts on areg
examples/ai-benchmark/run-benchmark.sh --task examples/ai-benchmark/prompt-tempalarm.md    --attempts 15
examples/ai-benchmark/run-benchmark.sh --task examples/ai-benchmark/prompt-coffeemachine.md --attempts 15
examples/ai-benchmark/run-benchmark.sh --task examples/ai-benchmark/prompt-atm.md          --attempts 15
examples/ai-benchmark/run-benchmark.sh --task examples/ai-benchmark/prompt-printscan.md    --attempts 15

# The coffee machine on gRPC, the comparison arm
examples/ai-benchmark/run-benchmark.sh --framework grpc --attempts 15
```

```bash
# Print the prompt a run would get, and spend nothing
examples/ai-benchmark/run-benchmark.sh --task examples/ai-benchmark/prompt-atm.md --dry-run

# Another model and effort, into a directory of your choice
examples/ai-benchmark/run-benchmark.sh --model opus --effort high --attempts 15 --out /data/runs

# Three draws of one arm on one tree -- compare medians, never one run
for label in a b c; do
    examples/ai-benchmark/run-benchmark.sh "$label" --attempts 15
done

# Read a finished run again, and probe it again with sanitizers
python3 examples/ai-benchmark/analyze_run.py ~/runs/20260913d-coffeemachine
python3 examples/ai-benchmark/verify_run.py  ~/runs/20260913d-coffeemachine --sanitize
```

### Hidden acceptance probes

Every task's checklist asks for things a run in which everything works cannot show, and
an agent's own scenarios rarely try: a start in the wrong order, a peer that never comes,
a peer that dies at an unlucky moment, a spinning thread. Every agent reports 15 of 15
on the checklist, so the checklist alone cannot tell two implementations apart.

`verify_run.py <run directory>` runs the finished project against those requirements
after the agent has ended. The probes are generated from the project's own
`scenarios.json` -- the scenario with two or more processes, no stop, and a lead that
exits 0 -- so they assume no framework, and the same probes score every arm. The agent
never sees them: `run-benchmark.sh` does not copy this file into the snapshot.

| Probe | Requirement it scores | Passes when |
|---|---|---|
| `repeat` | the scenario exits 0 | the normal scenario passes 10 times out of 10 |
| `start-order` | the client survives the server being started after it | the normal scenario passes with the lead started 3 s before the rest |
| `no-peer` | neither program waits more than 20 seconds for something that never arrives | the lead alone exits non-zero within 30 s |
| `peer-loss` | if one side goes away mid-scenario, the other exits non-zero | with the other process killed at 25%, 50% and 75% of a normal run, the lead exits non-zero each time and never hangs |
| `cpu` | no busy-waiting | a normal run averages under 0.5 cores (where the platform reports child CPU time) |
| `sanitize` (`--sanitize`) | no memory or undefined-behaviour defect -- not a checklist item | a rebuild under ASan and UBSan runs the normal scenario and one peer loss with no finding |

The result is printed and written to `<run>/verify.json`. The sanitizer build goes to
`<run>/verify-sanitize-build`, outside the project. A framework compiled from source is
checked by the sanitizers along with the application; a framework linked as a prebuilt
system package is not.

### Reading a run

`analyze_run.py <run directory>` prints, per run: cost and its split, API requests,
reasoning tokens (exact, from the result), cache reads and writes, peak context, build and
scenario runs, the build-and-fix and run-and-fix cycles, filesystem searches, reads of
framework sources, every documentation page opened with its size, the lines of C++
written by hand, and a per-request timeline. It runs at the end of every run.

**Never quote one run.** The same tree run twice has come out 45-80% apart in cost. Run
each arm at least three times on one tree and compare medians; the quality rows
(acceptance items, scenarios passing) are the ones that hold at one run.

**Score the result yourself.** The agent's own count is a claim. Re-run its scenarios:

```bash
cd ~/runs/20260913d-coffeemachine/work
python3 ../sdk/tools/agent/run_scenarios.py --build build/bin
```

---

## Running one by hand, with any agent

The same prompt works for any agent that can read files and run commands -- GitHub
Copilot, Cursor, or Claude Code in a terminal -- and on any operating system.

**1. Make one empty directory and start the agent in it.** That directory becomes the
project. Not your home directory, and not inside the checkout: a session started inside
it loads the SDK's maintainer instructions, which no user of areg ever sees.

**2. Write the prompt.** Copy the wrapper for the task and fill in the values at its top.

| Arm | Copy | Fill in |
|---|---|---|
| areg, coffee machine | `areg-coffeemachine-prompt.txt` | `areg-sdk`, `task`, `project`, `mode` |
| areg, any other task | `areg-ai-prompt-template.txt` | `areg-sdk`, `task`, `project`, `mode` |
| gRPC, coffee machine | `grpc-coffeemachine-prompt.txt` | `task`, `project`, `runner` |

Replace every `<name>` below the prompt line with the value you gave it, or paste the
whole file: the agent is told that `<name>` means that value. For example:

```
  areg-sdk  /path/to/areg-sdk
  task      /path/to/areg-sdk/examples/ai-benchmark/prompt-coffeemachine.md
  project   coffeemachine
  mode      ipc
```

**Give the agent a fresh clone**, not a checkout you work in: a working checkout carries
ignored files of its own, and the agent can read them.

**3. Run it, and keep what the harness records.** For Claude Code:

```bash
claude -p --output-format json --disable-slash-commands --strict-mcp-config \
       --add-dir /path/to/areg-sdk < prompt.txt > result.json
```

The two flags keep the operator's skills and MCP servers out of the run. The prompt goes
in on standard input because `--add-dir` takes every word after it as a directory. For
GitHub Copilot, keep its `--usage-output-file`.

Allow the build commands generously. **Every permission denial costs the agent a turn**,
and a denial-heavy run is not comparable with a clean one.

**4. Read the numbers.**

```bash
python3 /path/to/areg-sdk/examples/ai-benchmark/measure.py result.json --project .
```

It prints cost, turns, wall time and the token split, counts the tool calls from the
session transcript, counts what the run wrote over the whole project, and scores the run
against the budget below. `num_turns` is assistant turns, not tool calls; only tool calls
compare across runs. Budget against *fresh* tokens -- uncached input plus output -- not
the all-in figure, which cache reads push into the millions.

| Task | Fresh tokens | Tool calls |
|---|---:|---:|
| `prompt-tempalarm.md` | 50K | ~20 |
| `prompt-coffeemachine.md` | 75K | ~30 |

**5. Score the result yourself.**

```bash
cmake --build build -j8
python3 /path/to/areg-sdk/tools/agent/run_scenarios.py --build build/bin
```

Then walk the task's acceptance checklist against the captured output, item by item.

**6. Record it** next to the run: tokens, cost, requests, tool calls, build-and-fix and
run-and-fix cycles, checklist score out of the list's length, and what the agent said it
had to guess. That last one is where the next documentation fix comes from.

---

## What an agent can report, and what it cannot

**No agent is asked for a token count, a price or a wall time, and none should be.** Most
harnesses never show an agent its own usage. An agent asked for a number it cannot see
either writes "not available to me" or spends turns computing a substitute -- measured at
about 8% of one run before it was removed.

| | who produces it | portable across harnesses |
|---|---|---|
| build-and-fix cycles, run-and-fix cycles, acceptance items, checker findings, files opened off-route | **the agent**, from what it already knows | **yes** -- the report table in every task |
| tokens, cache reads, cost, requests, wall time | **the operator**, afterwards, from what the harness records | no |

**Two harnesses do not report a comparable cost.** Claude Code reports `total_cost_usd`.
GitHub Copilot reports **premium requests**, a quota unit and not money, so `measure.py`
prints no cost for a Copilot run and says why. Compare such arms on tokens, requests and
the agent-reported rows. Copilot reports reasoning tokens directly, and its own system
prompt is large -- a one-word prompt billed 22,575 input tokens -- which is the floor of
any Copilot run.

**Wall time compares only on one machine, and only with care.** A slow filesystem search
or a framework compiled from source moves it more than the framework does; model time is
the fairer row.

---

## Judging the result

Read the code. areg's `.siml`, `.fsml` and `.dtml` documents open in
[Lusan](https://github.com/aregtech/areg-sdk-tools), which draws the state machine, so you
can see whether the logic matches what was asked. Then build it and run the scenarios.

The number worth comparing is not tokens. It is **cost per application that builds and
passes its own acceptance list** -- a cheaper run that produces code nobody can verify
has not won anything.

---

## Writing a prompt of your own

Prompts that produce something worth reading are welcome.

- **Name it `prompt-<key>.md`**, with `<key>` a C identifier; it becomes the project name.
- **Keep the task free of framework, tool and operating-system vocabulary.** Say *"on
  resume the machine must continue from the stage it was interrupted in"*, not *"use a
  history state"*. Requirements, not mechanisms.
- **Keep the five sections** listed under [The four task prompts](#the-four-task-prompts),
  and add the file to `TASK_PROMPTS` in `tools/agent/check_corpus.py`, which then checks
  it.
- **Write an acceptance checklist someone could score without asking you what you meant**,
  and make every item observable in the output of the normal run or the peer-loss run.
- To run it on a framework other than areg, write `<framework>-<key>-prompt.txt` beside
  it, starting from `grpc-coffeemachine-prompt.txt`.

A prompt whose result cannot be checked is not a benchmark, it is a demonstration.
