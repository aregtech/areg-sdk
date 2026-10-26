# AI benchmark

Task prompts written for an AI agent, the wrappers that hand them to one framework, and the
scripts that run an agent against them and measure the result.

The benchmark asks one question: **can an agent that has never seen a framework build a correct
application from that framework's own documentation, and what does it cost?** areg is measured
against a framework the model already knows from training (gRPC): the same requirements, the same
agent, the same model and the same effort.

**Results:** [`baseline-2026-10-18.md`](baseline-2026-10-18.md) (GPT-5.6 Terra, six tasks),
[`baseline-2026-10-17.md`](baseline-2026-10-17.md) (Claude Sonnet 5.5, low effort, three tasks),
[`baseline-2026-10-05.md`](baseline-2026-10-05.md) (GPT-5.6 Terra, four tasks),
[`baseline-2026-09-29.md`](baseline-2026-09-29.md) (Claude Sonnet 5.5, coffee machine),
[`baseline-2026-09-13.md`](baseline-2026-09-13.md) (the first pair, Sonnet 5).

- [Quick start](#quick-start)
- [The task prompts](#the-task-prompts)
- [Agents and models](#agents-and-models)
- [areg, gRPC, and areg without the agent layer](#areg-grpc-and-areg-without-the-agent-layer)
- [Options](#options)
- [What a run leaves, and how to judge it](#what-a-run-leaves-and-how-to-judge-it)
- [Running one by hand, with any agent](#running-one-by-hand-with-any-agent)
- [Troubleshooting](#troubleshooting)
- [Writing a prompt of your own](#writing-a-prompt-of-your-own)

---

## Quick start

Start from an empty directory **outside** the checkout: the run directory is made where you stand.

```bash
mkdir -p ~/runs && cd ~/runs
/path/to/areg-sdk/examples/ai-benchmark/run-benchmark.sh --attempts 15
```

```powershell
mkdir C:\runs; cd C:\runs
C:\src\areg-sdk\examples\ai-benchmark\run-benchmark.ps1 --attempts 15
```

That is areg, the coffee machine, Claude Code with `sonnet` at `medium` effort. Every other run is
the same command with options added; both scripts take the same options. If PowerShell blocks the
script, start it with `powershell -ExecutionPolicy Bypass -File ...\run-benchmark.ps1 ...`.
`--dry-run` stages everything and prints the prompt without spending anything.

| File | What it is |
|---|---|
| `prompt-*.md` | the task prompts: the measuring instrument, never edited inside a series |
| `areg-ai-prompt-template.txt`, `areg-coffeemachine-prompt.txt` | the areg wrappers |
| `grpc-ai-prompt-template.txt`, `grpc-coffeemachine-prompt.txt`, `grpc-scenario-dialect.py` | the gRPC wrappers and the scenario runner's remedies in gRPC words |
| `areg-nolayer-prompt-template.txt`, `strip_layer.py` | the run without the agent layer |
| `run-benchmark.sh`, `run-benchmark.ps1` | one cold, isolated, measured run: Linux, macOS, WSL / Windows PowerShell 5.1 and 7 |
| `analyze_run.py`, `measure.py`, `repeat_report.py` | the usage report: a Claude run request by request; a Claude Code or Copilot result file against a budget; the band of a `--repeat` |
| `verify_run.py`, `build_config.py` | the hidden acceptance probes; the configuration a run built |
| `INSTALL-grpc.md`, `install-protobuf-cmake-config.sh` | installing the gRPC toolchain before the first gRPC run; the protobuf CMake package Debian and Ubuntu leave out |
| `runbook-demo.md` | what a measured areg run adds to `docs/agent/01-runbook.md` |

---

## The task prompts

Each prompt builds one small but complete system: two or three programs that talk to each other, a
scenario that proves it end to end, and a run in which one side is taken away. Each one stresses a
different area, so a change is judged across several of them, never on one.

| Prompt | Builds | Programs | State machine | What it exercises |
|---|---|---|---|---|
| `prompt-tempalarm.md` | a temperature monitor and a simulated operator | 2 | no | the plain service shape: a request, a published value, broadcasts, a threshold with hysteresis |
| `prompt-coffeemachine.md` | a coffee machine and a simulated user | 2 | yes | nested states, guarded transitions, resuming an interrupted sequence, a provider timer, counters and warnings |
| `prompt-atm.md` | an ATM and a simulated customer | 2 | **ruled out** | one retry-limited check reached from two places, written by hand |
| `prompt-atm-fsm.md` | the same ATM | 2 | yes | the same behaviour declared: one hosted machine reused from two states, a count that starts again on every visit |
| `prompt-printscan.md` | a multifunction device and an operator | 2 | yes | two engines scheduled one job at a time, an injected fault, recovery |
| `prompt-elevator.md` | an elevator, a passenger and a floor display | **3** | yes | a sequence entered at a later step, halt and resume from the exact step, two clients of one provider |
| `prompt-sensorgateway.md` | a sensor gateway and a monitor | 2 | yes | an application thread feeding a component by messages; shared records, enumerations and lists |
| `prompt-greenhouse.md` | a zone host and a console | 2 | no | three named instances of one controller; one moved into the console's process by configuration only |
| `prompt-washer.md` | a washing machine and a user | 2 | yes | **the hardest machine:** reused stages, an attempt limit that starts again, pause at a boundary, retry of the failed stage only |
| `prompt-orderdesk.md` | a warehouse, an order desk and a customer | **3** | no | two services in one program, deferred answers, three orders in flight, undo on failure |
| `prompt-chargepoint.md` | a charging station, a site manager and a driver | **3** | yes | **the widest:** named instances, a reused handshake, deferred answers, a meter thread, suspend and resume, a late watcher |
| `prompt-camera.md` | a camera and a viewer | 2 | no | 64 KB frames at a fixed rate and in a burst, a stuck capture thread replaced, part of a client restarted at run time |

**Where to start:** `prompt-tempalarm.md`, then `prompt-coffeemachine.md`: the plain service path,
then the same shape with a state machine. `prompt-atm.md` and `prompt-atm-fsm.md` are a pair: the
same behaviour hand-written and declared.

**Every task prompt names no framework and no operating system.** Each holds the same sections --
`## The task`, `### Proving it`, `### Acceptance checklist`, `## What to deliver`, `## The report` --
so the same requirements can be scored against gRPC, ZeroMQ, DDS or areg.
`python3 tools/intern/check_corpus.py` fails if a framework, operating system or tool name, or a build
command, gets into one of them.

---

## Agents and models

`--agent` selects the CLI, `--model` the model inside it; only the selected CLI must be installed and
logged in. Model names are examples, not a supported list: pin an exact model ID when two runs are to
be compared. `--effort low|medium|high` becomes Claude's `--effort`, Copilot's `--reasoning-effort` or
Codex's `model_reasoning_effort`; Gemini refuses it. The levels are not equal across providers.

| `--agent` | Defaults | Usage report | Harness |
|---|---|---|---|
| `claude` (default) | `sonnet`, `medium` | `analyze_run.py`, automatic | six-tool allowlist, skills and MCP off, web tools withheld |
| `copilot` | the CLI's (`--model auto`, `gpt-5.6-terra`, `claude-sonnet-5`, ...) | `measure.py`, in AI credits (AIC, converted once at USD 0.01) | `ask_user` off, built-in MCP off, web tools excluded; a one-word prompt already bills ~22 500 input tokens |
| `codex` | the CLI's | none; native `result.jsonl` | `workspace-write`, approvals off |
| `gemini` | the CLI's | none; native `result.json` | `yolo` |

```bash
run-benchmark.sh --task examples/ai-benchmark/prompt-tempalarm.md --attempts 15
run-benchmark.sh --agent copilot --model gpt-5.6-terra --effort medium --attempts 15
run-benchmark.sh --model opus --effort high --repeat 3 --attempts 15
```

**One run measures the draw, not the tree:** the same tree has come out 51% apart over 31 runs.
`--repeat N` runs the same arm N times, prints the spend after each, and the band at the end; 2 or 3
settles most questions, 4 is the ceiling.

Run only in a disposable environment with trusted prompts: a snapshot is not a security sandbox.
Allow the build commands -- every permission denial costs the agent a turn.

---

## areg, gRPC, and areg without the agent layer

| | areg arm (`--framework areg`, default) | gRPC arm (`--framework grpc`) |
|---|---|---|
| the agent may read | `sdk/`: the snapshot of `--sdk` | `task/`: the task and the scenario runner. Nothing else |
| the framework comes from | the snapshot, compiled by the agent's first build | the prebuilt install `--grpc` names, or the `PATH` |
| the web | **off** by default | **on** by default |
| the run directory | `<date><label>-<project>` | `<date><label>-grpc-<project>` |

**The fair setting is each framework's documentation where it lives:** areg ships it in the snapshot
under test, gRPC on the web. `--web on|off` overrides either side, and `meta.txt` records it.

**`--sdk`** must be the root of an areg-sdk **git checkout** (the directory holding `AGENTS.md`), not a
build or an install. Tracked and untracked-but-not-ignored files are copied, so an uncommitted change
**is** measured. An installed areg package makes the script refuse (`--allow-installed-areg` measures
it on purpose). **`--grpc`** names the directory holding `protoc` and `grpc_cpp_plugin`, or the install
prefix whose `bin/` holds them; [`INSTALL-grpc.md`](INSTALL-grpc.md) covers each install route.
Nothing is installed during a run.

**The gRPC arm is a cold start by check:** the script refuses to run if the prompt, a staged file or
the run path carries an areg name. Run it from a directory whose path does not contain "areg".

### Without the agent layer

`--layer off` runs the areg arm on the framework as a plain checkout gives it: sources, human
documentation, examples and code generator, with everything written for a coding agent removed
(`strip_layer.py` holds the list).

Measured with GitHub Copilot and GPT-5.6 Terra at medium effort, one run per task:

| Task | Without the layer | With the layer |
|---|---|---|
| temperature alarm (no state machine) | $0.61, 25 requests, 10 fix cycles, every hidden probe passed | $0.23-0.34, 7-14 requests, 2 fix cycles, every probe passed |
| coffee machine (one state machine) | **$2.09, 71 requests, 31 fix cycles; stopped at the bound of 15, the program does not work** (1 of 6 probes) | $0.32-0.46, 7-12 requests, 2-4 fix cycles, every probe passed |

> [!CAUTION]
> A run without the layer can cost several dollars and may never finish. Keep `--attempts` bounded and
> watch the spend. To see where the coffee machine would have ended, run it with `--attempts 30` and
> share the result in [the discussion](https://github.com/aregtech/areg-sdk/discussions/800).

A third framework needs a value in both scripts, its staging and its cold-start ban, plus a
`<framework>-ai-prompt-template.txt` and a `<framework>-scenario-dialect.py`. Until then, run it
[by hand](#running-one-by-hand-with-any-agent).

---

## Options

`run-benchmark.sh --help` prints the full list.

| Option | What it changes | Default |
|---|---|---|
| `label` | the letter after the date in the run directory name | next free letter |
| `--framework` | `areg` or `grpc` | `areg` |
| `--agent`, `--model`, `--effort` | the CLI, its model, the reasoning effort | `claude`, `sonnet`, `medium` |
| `--task` | the task prompt; `prompt-<key>.md` also sets the project name and the wrapper | `prompt-coffeemachine.md` |
| `--wrapper`, `--project`, `--mode` | another wrapper; the project name; the areg shape `ipc`, `local` or `pubsub` | by name; `<key>`; `ipc` |
| `--attempts` | the build-and-fix and run-and-fix bound, the same for both arms; `0` removes it | `15` |
| `--repeat` | N runs of the same arm and their band | `1` |
| `--debrief` | a pass after the report: what the run could not find. Costs requests; never compare with a normal run | off |
| `--debug` | the full analysis after the run instead of the short headline | off |
| `--verify` | the hidden probes: `none`, `probes`, or `sanitize` (ASan and UBSan as well) | `probes` |
| `--cache-ttl` | Claude only: the prompt-cache TTL, `1h` or `5m` | `1h` |
| `--recipes`, `--layer` | areg only: whether a recipe may be copied; `off` removes the agent layer | `none`, `on` |
| `--web` | whether the agent may search and fetch pages | `off` areg, `on` gRPC |
| `--sdk`, `--grpc` | the areg checkout; the gRPC tools or prefix | this checkout; the `PATH` |
| `--dry-run` | stage and print the prompt, start no agent | off |
| `--allow-installed-areg` | measure an installed areg package on purpose | off |

**Exit code:** `0` the agent finished; `2` the script refused to start, and says why; `3` the agent
changed a file it was measured against, so the run is not valid; `4` no output, or the usage report
failed. Acceptance is in `verify.json`, never implied by a zero exit.

---

## What a run leaves, and how to judge it

| In the run directory | Holds |
|---|---|
| `meta.txt`, `prompt.txt` | the settings of the run; exactly what the agent received |
| `result.json` / `result.jsonl`, `run.out`, `run.err` | the CLI's own output and usage |
| `transcript.jsonl` | Claude only: the whole session; read it before sending a run to anyone (it holds your paths and every command) |
| `sdk/` or `task/`, `sdk-head.txt`, `sdk-md5.txt` | what the agent could read, which tree, and proof it was not edited |
| `work/` | the project the agent built |
| `verify.json`, `verify-sanitize.json` | the verdict of the hidden probes, with evidence |

**The hidden probes** score what a run in which everything works cannot show. They are generated from
the project's own `scenarios.json`, assume no framework, and the agent never sees them:

| Probe | Passes when |
|---|---|
| `repeat` | the normal scenario passes 10 times out of 10 |
| `start-order` | it passes with the lead started 3 s before the rest |
| `no-peer` | the lead alone exits non-zero within 30 s |
| `peer-loss` | with another process killed at 25%, 50% and 75% of a run, the lead exits non-zero and never hangs |
| `cpu` | a normal run averages under 0.5 cores |
| `programs`, `lines`, `timing` | as many programs as the task asks; every line of the task's scenario printed; the timed work not skipped |
| `sanitize` | an ASan and UBSan rebuild finds nothing (`--verify sanitize`) |

```bash
python3 examples/ai-benchmark/analyze_run.py ./20260921c-coffeemachine
python3 examples/ai-benchmark/verify_run.py  ./20260921c-coffeemachine --sanitize
```

**How to compare runs:**

- **Never quote one run.** Compare the medians of at least three per arm on one tree; the quality rows
  (acceptance, probes) are the ones that hold at one run.
- **Compare `cost, cold @1h`, not the bill.** A run within an hour of another reuses its prompt cache;
  the normalised line adds that back. Every published dollar assumes the 1-hour TTL.
- **Keep billing units apart** (USD for Claude Code, AI credits for Copilot), and compare wall time
  only on one machine.
- **The number worth comparing is the cost per application that builds and passes its acceptance list
  and the probes.** A cheaper run that produces code nobody can verify has not won anything.

Read the code too: areg's `.siml`, `.fsml` and `.dtml` documents open in
[Lusan](https://github.com/aregtech/areg-sdk-tools), which draws the state machine.

---

## Running one by hand, with any agent

1. **Make an empty directory and start the agent in it**, not in your home directory and not inside
   the checkout. Give the agent a fresh clone, not the checkout you work in.
2. **Write the prompt:** copy `areg-ai-prompt-template.txt` (or `areg-coffeemachine-prompt.txt`) and
   fill in `areg-sdk`, `task`, `project`, `mode` at its top; for gRPC copy
   `grpc-ai-prompt-template.txt` and fill in `task`, `project`, `runner`.
3. **Run it and keep the usage record.** For Claude Code:
   `claude -p --output-format json --disable-slash-commands --strict-mcp-config --add-dir /path/to/areg-sdk < prompt.txt > result.json`
   (the prompt goes on standard input because `--add-dir` takes every word after it). For Copilot keep
   its `--usage-output-file`.
4. **Measure and score:** `python3 .../measure.py result.json --project .`, then `verify_run.py` on the
   directory that holds `work/`, and record what the agent said it had to guess -- that is where the
   next documentation fix comes from.

---

## Troubleshooting

| Symptom | Fix |
|---|---|
| `find_package(areg) resolves to an installed package` | uninstall it or clear it from `CMAKE_PREFIX_PATH`, or pass `--allow-installed-areg` |
| `... holds no AGENTS.md` / `... is not a git checkout` | point `--sdk` at the root of a git clone |
| gRPC refused because the path carries an areg name | start from a neutral directory such as `~/runs` |
| `no protoc in DIR or DIR/bin`, or the agent reports `find_package(gRPC)` failed | pass the install **prefix** to `--grpc`, and run the CMake probe in `INSTALL-grpc.md` from the same shell |
| gRPC on Windows (vcpkg layout, MSVC Debug/Release mismatch) | the easy route is WSL: Linux packages install in one command; run from `~/runs`, not under `/mnt/c` |
| `<cli> not found` | install and log in the selected CLI only |
| a rate limit or 429 in `run.err` | not a result: discard the run and run again |
| the agent keeps asking for permission | allow build and run commands; a denial-heavy run is not comparable |

---

## Writing a prompt of your own

Prompts that produce something worth reading are welcome; each should make an agent use an ability the
others do not.

- Name it `prompt-<key>.md`, `<key>` a C identifier; `--task` picks it up with the template wrappers.
- Keep it free of framework, tool and operating-system vocabulary: requirements, not mechanisms.
- Keep the five sections, keep the requirements the hidden probes score (start order, the 20-second
  limit, the other side going away, no busy-waiting, exit codes), and add the file to `TASK_PROMPTS` in
  `tools/intern/check_corpus.py`.
- Write a checklist someone could score without asking you, and pin every number the scenario checks.

A prompt whose result cannot be checked is not a benchmark, it is a demonstration.
