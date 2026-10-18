# Agentic Coding with Areg SDK
## How a coding agent builds correctly on a framework it has never seen

> **Scope:** this page is for developers and decision-makers who want to know what an AI
> coding agent can do with Areg, how it works, and what was measured. An agent building
> an application starts at [`AGENTS.md`](./../../AGENTS.md), not here.

---

## 1. Why it exists

A coding agent knows a popular framework from its training data and pays nothing to use
that knowledge. For anything else it guesses, builds, fails and guesses again, and that
fix loop is where the time and the money go. Areg adds a second problem: models remember
an older version of it. Asked without any help, a current model recalls the Areg API of
2022 -- the `NE*`, `TE*` and `IE*` prefixes, the old method names -- and every one of those
names is wrong today.

Areg therefore ships its knowledge with the framework, and tools that write and check
the code, so the agent declares what it wants instead of learning the API by trial.

## 2. How it works

| Part | What it gives the agent |
|---|---|
| [`AGENTS.md`](./../../AGENTS.md) | the entry: routes each task to the one page it needs |
| [`docs/agent/`](./../agent/) | short task pages and a runbook (the golden path), each fact where it is used |
| [`docs/agent/api.json`](./../agent/api.json) | the API as data: names, signatures, removed names and their replacements |
| [`tools/agent/`](./../../tools/agent/) | setup, the generator wrapper, one-call build-and-run, API lookup, a contract checker |
| CI | keeps the pages, the API contract and the framework saying the same thing on every commit |

The golden path, in [`docs/agent/01-runbook.md`](./../agent/01-runbook.md):

1. **Declare.** The agent fills one `design.json`: services, attributes, requests,
   broadcasts, state machines, which program hosts what.
2. **Generate.** One command validates the declaration, writes the `.siml`, `.fsml` and
   `.dtml` documents and a compiling, running application: components, the model,
   `main()`, CMake and a scenario driver. A refused declaration writes nothing and names
   the rule and its fix.
3. **Write the behaviour.** A generated worksheet lists every method body the
   application still needs, with the names and spellings that apply to it. The agent
   writes only those bodies.
4. **Prove.** One command builds, runs every program and checks each printed line against
   the scenario.

## 3. What a developer gets

**In one line: for the same money as gRPC, the agent on Areg delivers more.** In the six-task
GPT-5.6 Terra comparison the cost summed to $2.63 against gRPC's $2.66, while the Areg runs had
the stricter task (every step's exact output line, a declared state machine with nested pieces),
ran a generated state machine instead of hand-written `switch` tables and threads, passed every
hidden check (14 of 14 runs, gRPC 9 of 11) and needed 40% fewer agent requests.

- **Correct on the first build.** Since 2026-09-29 every Areg run in the published
  baselines passed every hidden probe: repeated runs, either start order, a lost peer, no
  peer at all, no busy-waiting. Most Areg runs needed no build-and-fix or run-and-fix
  cycle; the gRPC runs in the Claude Code baselines needed 1-19. With GPT-5.6 Terra on six
  tasks: 1.9 fix cycles per Areg run, 7.3 per gRPC run.
- **Predictable.** An Areg run takes the same steps every time: declare, generate, write
  the bodies, prove. With Claude models, the cost of one task on one revision stayed
  within 1.6x across runs, and in some sets within 5%. The cost of one gRPC task varied up to
  2.5x: $1.37-3.36 on the coffee machine with Sonnet 5, and about 2.4x on the charge point
  with GPT-5.4 (from its token counts; Copilot recorded no credits for those runs),
  because the length of its fix loop varies. On the charge point with
  Sonnet 5.5, two gRPC runs were 32% apart, six Areg runs 15%. With GPT-5.6 Terra both
  frameworks varied about equally: 9-48% per task for Areg, 16-70% for gRPC.
- **Sturdier programs.** Threading, reconnection, start-order independence and
  distributed logging come from the framework, not from code the agent writes. Areg
  programs used 0.01-0.03 CPU cores over a normal run, the gRPC programs of the same tasks
  0.02-0.11 (measured side by side), two of them polling on a 200-250 ms timer. One gRPC
  charge-point run left its manager running forever after the driver was killed; with
  GPT-5.6 Terra, one gRPC coffee machine charged a cancelled drink when it started after
  the user, and one gRPC elevator reported a wrong floor once in ten runs. In
  development runs with GPT-5.4 on the same task (not a published baseline), none of 3
  gRPC runs passed every probe both arms share, against 9 of 11 Areg runs.
- **A real state machine, not a hand-rolled one.** The agent declares states and
  transitions in `.fsml` and runs the generated machine: an elevator's door sequence as a
  composite state with deep history, a printer's print and scan as nested pieces entered
  from two places. Lusan draws it, so a reviewer checks the logic as a diagram. The gRPC
  agents wrote enum tables, worker threads and condition variables for the same behaviour.
- **Less code to review.** The generator writes the plumbing; the agent writes 2-4x fewer
  lines of C++ by hand than on gRPC.
- **Fewer scans of the code.** The pages and the generated worksheet carry what a task
  needs, so the agent does not search the framework to learn it: across the Claude Code
  runs of these comparisons, Areg agents opened no framework or example source file and
  made 0-3 searches per task (0-1 with Sonnet 5.5). With Sonnet 5, gRPC agents made 2-12.
  A real project still needs searches in its own code; the framework's part no longer does.
- **Faster, with Claude Code.** Less wall time on every task measured: 10-17% on the
  camera, charge point and sensor gateway, about 20-35% on the coffee machine, about half on
  the temperature alarm. With GPT-5.6 Terra the model's own time was lower or equal on all 6
  tasks measured (23% less in sum), but the wall time was longer on 4 of them: the
  difference lies outside the model, where the Areg arm also compiles the framework
  from source inside the run. With GPT-5.4 the times were about equal.
- **The current API, not a remembered one.** The guide, the API contract and the tools
  ship with each release, and CI keeps them equal to the code.
- **Any agent, no server.** Plain pages and Python tools in the repository: Claude Code,
  GitHub Copilot CLI and other agents with a shell use the same files, offline too.

## 4. Measured against gRPC

gRPC is the comparison because the models already know it from training. Same task,
agent, model and effort for both arms; each run is scored afterwards by hidden probes the
agent never sees.

| Agent, model | Tasks | Hidden probes, Areg / gRPC | C++ written by hand | Cost, Areg vs gRPC |
|---|---|---|---|---|
| Claude Code, Sonnet 5 | coffee machine | 4 of 5 / 5 of 5 | 449 vs 1,257 lines | -26% |
| Claude Code, Sonnet 5.5 | coffee machine, temperature alarm | all, every run / all, every run | about 3x less | -10%, -26% |
| Copilot CLI, GPT-5.6 Terra | 4 tasks | 12 of 12 runs / 7 of 8 runs | 2-4x less | parity to -22% |
| Claude Code, Sonnet 5.5, low effort | camera, charge point, sensor gateway | all, every run / 3 of 4 runs | 2.3-4x less | +2%, -4%, +0.5% |
| Copilot CLI, GPT-5.6 Terra, current tree | 6 tasks, the Areg task stricter | 14 of 14 runs / 9 of 11 runs | 2-3.5x less | -1% in sum; -18% to +26% per task |

- **What holds:** the code an agent does not have to write, in every comparison;
  correctness, in every comparison since Sonnet 5.5 (the one Sonnet 5 run missed a probe).
- **What does not hold is a fixed cost ratio.** It ranges from 26% below gRPC to 2%
  above, depending on the task and the model (to 26% above on one task with Terra, where
  the Areg prompt had grown since the gRPC runs). One task on one tree has varied by 12-50%
  between runs, so none of the cost differences is significant yet.
- **Where the money goes is opposite in the two arms.** Areg pays to load its knowledge
  into the context (on the charge point, 50-67K more tokens written to the cache);
  gRPC pays in code written by hand and in fix cycles.

Ranges, setup and method for each row:
[Sonnet 5](./../../examples/ai-benchmark/baseline-2026-09-13.md),
[Sonnet 5.5](./../../examples/ai-benchmark/baseline-2026-09-29.md),
[GPT-5.6 Terra](./../../examples/ai-benchmark/baseline-2026-10-05.md),
[Sonnet 5.5 at low effort](./../../examples/ai-benchmark/baseline-2026-10-17.md),
[GPT-5.6 Terra, six tasks](./../../examples/ai-benchmark/baseline-2026-10-18.md).

## 5. Try it, or reproduce it

Point your agent at one of the ready prompts in
[`examples/ai-benchmark/`](./../../examples/ai-benchmark/), or let the harness run it and
score the result:

```bash
mkdir -p ~/runs && cd ~/runs
/path/to/areg-sdk/examples/ai-benchmark/run-benchmark.sh --task examples/ai-benchmark/prompt-tempalarm.md
/path/to/areg-sdk/examples/ai-benchmark/run-benchmark.sh --task examples/ai-benchmark/prompt-tempalarm.md --framework grpc
```

The [benchmark README](./../../examples/ai-benchmark/README.md) covers every option, the
agents it drives, and how to read a run.

**Share what you got.** Post your run, your prompt and your model in
[Agentic coding with Areg: try it, break it, send me the run](https://github.com/aregtech/areg-sdk/discussions/800),
passed or failed. A failed run is worth the most: every failure points at a page, a
generator or a framework path that gets fixed. A prompt of your own is just as welcome:
the prompts name no framework, so the same task runs on Areg and on gRPC.

## 6. Limits

- The tasks were written by the Areg side: multi-process services, several with a state
  machine. That is Areg's target domain, not every gRPC user's.
- gRPC has 1-4 runs per task in these baselines, Areg 1-7. Differences smaller than the
  ranges are not results.
- In the six-task Terra comparison the gRPC runs are older and their prompts asked less;
  that favours gRPC on cost and cannot favour it on correctness.
- The same tasks on a plain Areg repository, without the agent layer, have not been
  measured yet; the gain over that baseline is the next measurement.
- Results are per model and per agent harness, and both change every few months.
