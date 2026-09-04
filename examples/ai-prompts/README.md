# AI prompts

Task prompts written for an AI agent, and a place to keep your own.

Each file here is a complete piece of work handed to an agent: what the software has
to do, what to deliver, and what to measure. They exist for two reasons -- to give
you a starting point for your own prompts, and to make a claim about areg testable
instead of asking you to believe it.

| Prompt | Builds | Demonstrates |
|---|---|---|
| `temperature-alarm.md` | a temperature monitor and a simulated operator, in two processes | the plain service shape: a request, a published value, and broadcasts -- no state machine |
| `coffee-machine.md` | a coffee machine and a simulated user, in two processes | a `.fsml` state machine: nested states, guarded transitions, machine attributes, and resuming a sequence that was interrupted |

Run them in that order. The first is deliberately small and has no state machine in
it, so what it costs is what the plain service path costs; the second adds the state
machine on top of the same shape. Two points make a slope, and the slope is the only
thing that says anything.

## Running one

Open a session in this repository and say:

> Read `examples/ai-prompts/temperature-alarm.md` and carry it out.
> Generate into `<the directory you want>`.

The agent reads `AGENTS.md` for everything else. Nothing has to be explained to it
beforehand.

`*-text.txt` beside each prompt is the driver text of the measured run, verbatim: the
two-phase split that separates writing from building, what to report at the stop, and
when to abandon the task rather than work around a defect. Copy one and change the two
paths in its first lines -- the checkout and the target directory -- and the run is the
one the figures came from.

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
