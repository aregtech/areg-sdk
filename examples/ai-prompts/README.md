# AI prompts

Task prompts written for an AI agent, and a place to keep your own.

Each file here is a complete piece of work handed to an agent: what the software has
to do, what to deliver, and what to measure. They exist for two reasons -- to give
you a starting point for your own prompts, and to make a claim about areg testable
instead of asking you to believe it.

| Prompt | Builds | Demonstrates |
|---|---|---|
| `temperature-alarm.md` | a temperature monitor and a simulated operator, in two processes | the plain service shape: a request, a published value, and broadcasts -- no state machine |
| `coffee-machine.md` | a coffee machine and a simulated user, in two processes | a state machine: nested states, guarded transitions, and resuming a sequence that was interrupted |
| `atm.md` | an ATM and a simulated customer, in two processes | one retry-limited check reached from two places, each with its own attempt count |
| `printer-scanner.md` | a multifunction device and a simulated operator, in two processes | two engines scheduled one job at a time, reused by a copy job, with faults reported the same way |

Run the first two in that order. `temperature-alarm.md` is deliberately small and
has no state machine in it, so what it costs is what the plain service path costs;
`coffee-machine.md` adds the state machine on top of the same shape. Two points make
a slope, and the slope is the only thing that says anything. The other two are larger
tasks of the same kind.

Two more files make them runnable on areg, and the split between them is the point:

| File | Who edits it |
|---|---|
| `ai-prompt-template-text.txt` | **you** -- four lines at the top, and nothing else. This is the prompt you paste. |
| `runbook-areg.md` | **nobody.** The agent reads it: where to work, every command in order, the bounded fix loop, and the report. |

Keeping the procedure out of the prompt is what stops a run being spoiled by a path
edited in one place and missed in another.

## Running one, and measuring it

This is the whole procedure. Nothing here is optional if you want a comparable number.

**1. Make one empty directory and start the session in it.** That directory becomes
the project: the agent writes everything into it and creates nothing outside it.

- **not your home directory** -- the agent would scatter a build tree through it
- **not inside the checkout** -- a session started there loads the SDK's own
  maintainer-facing `CLAUDE.md`, which no user of areg ever sees and which orders
  twelve corpus checkers the run does not need. That alone can double the bill.
- **a fresh, empty directory of its own**, anywhere else

```bash
RUN=~/runs/$(date +%Y%m%d)-coffeemachine
mkdir -p "$RUN" && cd "$RUN"
```

You stay in `$RUN` for every remaining step. The agent is told the project root is
wherever the session started, so it never picks a second one.

**2. Write the prompt.** Copy `ai-prompt-template-text.txt` and edit its four-line
block -- that block is the only thing in the file you ever touch.

```bash
cp /mnt/c/projects/areg-sdk/examples/ai-prompts/ai-prompt-template-text.txt prompt.txt
$EDITOR prompt.txt        # areg-sdk, task, project, mode -- four lines, at the top
```

For the coffee machine those four are already the file's defaults:

```
  areg-sdk = /mnt/c/projects/areg-sdk
  task     = examples/ai-prompts/coffee-machine.md
  project  = coffeemachine
  mode     = ipc
```

**3. Run it headless, and keep the JSON.** `-p` prints and exits; the JSON carries the
usage the agent cannot see itself.

```bash
claude -p --output-format json \
       --add-dir /mnt/c/projects/areg-sdk \
       "$(cat prompt.txt)" > result.json
```

Allow the build commands generously, or run with `--dangerously-skip-permissions` in a
throwaway directory. **Every permission denial costs the agent a turn and inflates the
figure**, and a denial-heavy run is not comparable to a clean one.

**4. Read the numbers off the JSON.**

```bash
python3 /mnt/c/projects/areg-sdk/examples/ai-prompts/measure.py result.json
```

It prints cost, turns, wall time and the token split, then finds the session
transcript and counts the tool calls, and scores both against the budget. Needs only
`python3`. Two things it makes explicit, because both are easy to get wrong:

- **`num_turns` is assistant turns, not tool calls.** They differ by 3-4x. Only the
  tool-call count is comparable across runs, and it has to come from the transcript.
- **Budget against *fresh* tokens** -- uncached input plus output. The all-in figure
  is dominated by cache reads and will read in the millions; that is not the number
  the targets below refer to.

**5. Score the result yourself. Do not take the agent's word for it.**

```bash
cd "$RUN" && cmake --build build -j && \
python3 /mnt/c/projects/areg-sdk/tools/agent/run_scenarios.py --build build/bin ; echo "exit=$?"
```

Then walk Part 1's acceptance checklist against the captured output, item by item.
A cheap run that produces code nobody can verify has not won anything.

**6. Record it** next to the run: tokens, cost, turns, tool calls, wall time,
build-and-fix cycles, checklist score out of the list's length, and what the agent
said it had to guess. That last one is where the next corpus fix comes from.

### What counts as a pass

| Task | Tokens | Turns |
|---|---:|---:|
| `temperature-alarm.md` | 50K | ~20 |
| `coffee-machine.md` | 75K | ~30 |

Turns are the cost, not bytes: at roughly 2.4K tokens per turn, everything read in a
whole run is about 13% of the bill. If a change does not remove turns, it does not
save money.

## How these prompts are written, and why it matters

Every prompt here is in three parts, and the split is deliberate.

**Part 1 states the task and names no framework.** It says *"on resume the machine
must continue from the stage it was interrupted in"* rather than *"use a history
state"*. Requirements, not mechanisms. This is what lets you hand the same task to
gRPC, ZeroMQ or DDS, changing only Part 2, and compare the results fairly.

**Part 2 says what to deliver** -- which files, where, and which checks to run before
reporting. This is the only part that knows about areg.

**Part 3 says what to measure.** Tokens, turns, build attempts, how much code the
agent wrote by hand against how much came from the documents, and how many acceptance
items passed. Counts, not opinions.

**Part 1 ends in an acceptance checklist**, and that is the part that makes the whole
thing honest. Without it, "it works" is an argument. With it, two implementations in
two frameworks can be scored against the same list by someone who trusts neither.

## Judging the result

Read the code. `.siml`, `.fsml` and `.dtml` documents open in
[Lusan](https://github.com/aregtech/areg-sdk-tools), which draws the state machine so
you can see whether the logic matches what you asked for. Then build it and run it.

The number worth comparing is not tokens. It is **tokens per application that builds
and passes its own acceptance list** -- a cheaper run that produces code nobody can
verify has not won anything.

## Contributing a prompt

Prompts that produce something worth reading are welcome. Keep the three parts, keep
Part 1 free of framework vocabulary, and give it an acceptance checklist someone
could score without asking you what you meant. A prompt whose result cannot be
checked is not a benchmark, it is a demonstration.
