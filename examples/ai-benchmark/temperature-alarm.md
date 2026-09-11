# Prompt: temperature monitor with a threshold alarm

A task prompt for an AI agent. It is written the way a developer hands work to a
colleague: what the software must do, not how to build it. It names no framework, so
the same requirements can be handed to gRPC, ZeroMQ, DDS, areg, or anything else, and
it names no operating system.

**To run it.** This file is the task, its acceptance checklist and the report to end
with -- nothing else. To build it on **areg**, copy
`examples/ai-benchmark/ai-prompt-template-text.txt`, set its `task` line to
`examples/ai-benchmark/temperature-alarm.md`, and paste that file as the prompt:
four lines are all you edit. For another framework, hand this file to an agent
together with your own delivery instructions --
`examples/ai-benchmark/grpc-coffee-machine.txt` is a worked
example of one.

This is the plain service shape: one program publishes a changing value and
announces events about it, another watches and reacts. No state machine.

---

## The task

Build a **temperature monitor** and a **simulated operator**, as two separate
programs that talk to each other. Neither may reach into the other's memory:
everything passes over the connection.

This is the shape of every condition-monitoring system: something measures, something
watches, and an alarm must be trustworthy enough that a person can act on it.

### The monitor

**The reading.** The monitor produces a temperature in tenths of a degree Celsius,
one reading every 200 ms. It publishes the current reading, and the operator program
receives pushed updates when it changes; it must not poll or wake itself every
200 ms to ask for the next value.

To keep the run repeatable regardless of which program starts first, the monitor
does not begin the sequence below until thresholds have been accepted. Before
that it holds the first value and publishes no change.

The readings are **not random** -- a run has to be repeatable. Walk this sequence,
one step per reading, then stop and hold the last value:

```
200, 210, 220, 230, 240, 250, 260, 270, 280, 290,   (rising)
300, 310, 320, 310, 300, 295, 290, 292, 296, 300,   (over the top, then wobbling)
290, 270, 250, 230, 210, 190, 170, 150, 130, 110    (falling away)
```

**The thresholds.** The operator program sets a high limit and a hysteresis, both in
tenths of a degree. The request is accepted only when the high limit is between 0 and
1000 and the hysteresis is greater than 0 and smaller than the high limit; otherwise
it is refused with a reason the operator program can print. Until thresholds are set,
the monitor watches nothing and raises nothing.

**The alarm -- the part that has to be right.** The monitor tells the operator
program when the alarm is **raised** and when it is **cleared**, each carrying the
reading that caused it.

- it is raised when the reading first goes **at or above** the high limit
- once raised it stays raised, however the reading moves, until the reading falls
  **below `high - hysteresis`**; only then is it cleared
- it is never raised twice without being cleared in between, and never cleared twice
  without being raised in between

With a high limit of 300 and a hysteresis of 30 the sequence above must produce
**exactly one raise and exactly one clear** -- the wobble between 290 and 320 sits
inside the band and must not produce a single extra message. An alarm that chatters
is worse than no alarm, because a person stops reading it.

The current alarm state is also published, so an operator program that connects late
learns immediately whether the alarm is on, without waiting for the next change.

**Statistics.** The operator program can ask for the lowest reading seen, the highest
reading seen, and how many times the alarm has been raised since the thresholds
were accepted.

**Stopping and timing out.** The monitor runs until it is stopped: it accepts
`-q` or `--quit` typed at its console and exits cleanly. Neither program may wait
forever -- if something it is waiting for has not arrived within 20 seconds, it
reports that and exits. If the monitor goes away, the operator reports the loss
and exits non-zero.

### The simulated operator

A second program that connects to the monitor and runs this scenario, printing each
step so a person can read what happened:

1. try to set a high limit of 300 with a hysteresis of 0 -- expect a refusal with a
   reason
2. try to set a high limit of 300 with a hysteresis of 400 -- expect a refusal
3. set a high limit of 300 with a hysteresis of 30 -- expect it to be accepted
4. follow the pushed reading updates, printing every raise and every clear as
   they arrive
5. when the last scripted reading has been delivered, ask for the statistics and check
   them: lowest 110, highest 320, alarms raised exactly 1
6. check that exactly one raise and one clear arrived in total

Then exit. **Exit code 0 if every expectation held, non-zero otherwise**, printing
which step failed. The operator program must survive being started before the
monitor, and if the monitor is stopped before the scenario completes it must
report that loss and exit non-zero instead of waiting forever.

### Proving it

Two of the requirements below cannot be shown by a run in which everything works, so
they need a second run of their own:

1. **The normal run** -- the sequence above, end to end, exit 0.
2. **A run where the other side is taken away.** Start both, let the sequence reach
   the middle, then stop the monitor abruptly. The simulated operator must say that
   it lost the other
   side and exit non-zero. It must not hang, and it must not exit 0.

**An acceptance item counts as passing only when the output of one of those runs shows
it.** An item you believe you implemented but never observed is reported as not
passing: naming the ones you could not prove is worth more than a full score.

### Acceptance checklist

The run is a success when all of these hold. Score any implementation, in any
framework, against this list:

- [ ] two separate programs, talking over the connection only
- [ ] the reading is published and the operator is notified as it changes
- [ ] the operator follows pushed updates rather than polling for the next reading
- [ ] setting thresholds is refused for a hysteresis of 0, with a reason
- [ ] setting thresholds is refused for a hysteresis wider than the limit
- [ ] the alarm is raised when the reading reaches the limit
- [ ] **the wobble inside the band produces no extra raise and no extra clear**
- [ ] the alarm is cleared only below `high - hysteresis`
- [ ] alarm state is published, so a late connection learns it without waiting
- [ ] statistics return lowest 110, highest 320, one alarm raised
- [ ] the monitor accepts `-q` / `--quit` at its console and exits cleanly
- [ ] neither program waits more than 20 seconds for something that never arrives
- [ ] if the monitor goes away mid-run, the operator reports it and exits non-zero
- [ ] the scenario exits 0, and non-zero when an expectation fails
- [ ] no busy-waiting and no sleeping inside a message handler

---

## What to deliver

**Two programs**, each with its own entry point, in their own subdirectories of the
project. Nothing outside the project directory, nothing added to the framework's own
build, and no IDE or editor project files.

**There is no state machine in this task.** If you find yourself wanting one, the
task is smaller than you think.

**The contract between the two programs is declared once**, in whatever form the
framework declares an interface, and the code that carries it over the connection is
generated from that declaration rather than written by hand. Never edit a generated
file and never commit one.

Do **not** write a `ReadMe.md`.

**Stop rule.** At most **3 build-and-fix cycles and 3 run-and-fix cycles**. If it has
not converged after the third of either, stop and report what fails, the exact output,
and what you think the cause is. Widening a timeout, adding a sleep, or loosening what
the scenario expects is not a fix.

---

## The report

End with this table and nothing longer.

| | |
|---|---|
| build-and-fix cycles | |
| run-and-fix cycles | |
| acceptance items passing | n of the checklist above, and which failed |
| checker findings, first run | name the checker, or "none run" |
| files you opened that the documentation did not route you to | names, or "none" |

**Fill it only from what you already know, and measure nothing to fill it in.** Byte
counts, line counts, token counts, tool calls and wall time are the operator's to read
from the session afterwards; computing them yourself costs turns and tells nobody
anything. An invented number makes every comparison worthless, so a figure you do not
already have is left out, not guessed.

Then three sentences at most: what the documentation answered well, what you had to
guess or discover the hard way, and which page you wish had said something it did not.
Say plainly wherever you had to search the repository instead of being routed to an
answer -- that is the finding this exercise is really after, and it is worth more than
the table.
