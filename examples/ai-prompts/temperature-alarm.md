# Prompt: temperature monitor with a threshold alarm

A task prompt for an AI agent. It is written the way a developer hands work to a
colleague: what the software must do, not how to build it.

**To run it.** Open a new session in the areg-sdk repository and say:

> Read `examples/ai-prompts/temperature-alarm.md` and carry it out.
> Generate into `<the directory you choose>`.

Name a directory. If you do not, the agent is told to stop and ask rather than pick
one for you.

This is the small one. There is no state machine in it: it is the plain
request / published-value / broadcast shape, and it exists to measure what that
costs on its own. Part 1 names no framework, so the same requirements can be given
to gRPC, ZeroMQ, DDS or anything else; only Part 2 changes.

---

## Part 1 -- The task

Build a **temperature monitor** and a **simulated operator**, as two separate
programs that talk to each other. Neither may reach into the other's memory:
everything passes over the connection.

This is the shape of every condition-monitoring system: something measures, something
watches, and an alarm must be trustworthy enough that a person can act on it.

### The monitor

**The reading.** The monitor produces a temperature in tenths of a degree Celsius,
one reading every 200 ms. It publishes the current reading, and the operator program
is told when it changes without having to ask.

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
reading seen, and how many times the alarm has been raised since the monitor started.

### The simulated operator

A second program that connects to the monitor and runs this scenario, printing each
step so a person can read what happened:

1. try to set a high limit of 300 with a hysteresis of 0 -- expect a refusal with a
   reason
2. try to set a high limit of 300 with a hysteresis of 400 -- expect a refusal
3. set a high limit of 300 with a hysteresis of 30 -- expect it to be accepted
4. follow the readings, printing every raise and every clear as it arrives
5. when the sequence has finished, ask for the statistics and check them: lowest 110,
   highest 320, alarms raised exactly 1
6. check that exactly one raise and one clear arrived in total

Then exit. **Exit code 0 if every expectation held, non-zero otherwise**, printing
which step failed. The operator program must survive being started before the
monitor.

### Acceptance checklist

The run is a success when all of these hold. Score any implementation, in any
framework, against this list:

- [ ] two separate programs, talking over the connection only
- [ ] the reading is published and the operator is notified as it changes
- [ ] setting thresholds is refused for a hysteresis of 0, with a reason
- [ ] setting thresholds is refused for a hysteresis wider than the limit
- [ ] the alarm is raised when the reading reaches the limit
- [ ] **the wobble inside the band produces no extra raise and no extra clear**
- [ ] the alarm is cleared only below `high - hysteresis`
- [ ] alarm state is published, so a late connection learns it without waiting
- [ ] statistics return lowest 110, highest 320, one alarm raised
- [ ] the scenario exits 0, and non-zero when an expectation fails
- [ ] no busy-waiting and no sleeping inside a message handler

---

## Part 2 -- What to deliver (areg-sdk)

*Replace this part when giving the task to another framework. Part 1 stays as it is.*

**Where to put it.** The operator names the target directory. **If you were not told
one, stop and ask for it. Do not choose a location yourself.**

**It is a project of its own, outside the SDK**, the way an application built on areg
normally is. Do not adopt the SDK's own build conventions, do not add it to any build
file of the SDK, and write no IDE or editor project files: they are not wanted and
they measure nothing. (A target directory inside the SDK is a variation the operator
may ask for. It costs turns that have nothing to do with the framework, so it is not
the default and it is not what a comparison should be run on.)

Read `AGENTS.md` first and follow where it routes you. That file is the entry point,
and it is deliberately the only thing here that points at anything: whether it gets
you the rest is what this exercise measures. Work out the project layout, the build
wiring and the conventions from where it sends you, not from files named here.

The contract between the two programs is a `.siml` document, generated into code at
build time. **There is no state machine in this task** -- if you find yourself
wanting one, the task is smaller than you think. Never write generated files by hand,
and never commit them.

Deliver:

- the service contract document, under `services/`
- the monitor sources and the operator sources, in separate subdirectories, each with
  its own `main.cpp`
- the build file that compiles both programs and turns the document into code

Do **not** write `ReadMe.md`: the maintainer writes it, and it records that
everything else was generated.

**Verify before you report, in this order:**

1. check the sources against the project's own contract checker -- the documentation
   names it and says when to run it; find it the same way you found everything else
2. build it
3. run both programs and confirm the scenario exits 0

---

## Part 3 -- The report

End with a short report. No prices, no estimates -- only what you can count.

**Do not guess the token count.** Most harnesses do not show an agent its own usage,
and an invented number makes every comparison worthless. Report it only if your
harness gives you the figure; otherwise write "not available to me" and leave it for
the operator, who can read it from the session. The same goes for wall time.

| | |
|---|---|
| Total tokens used | from the harness, or "not available to me" |
| Turns / tool calls | |
| Wall time | |
| Build attempts before it compiled | |
| Contract-checker findings, first run | name the tool |
| Lines of C++ you wrote by hand | |
| Lines generated from the document | |
| Acceptance checklist items passing | out of 11 |
| Documentation pages you opened, and their total size | |
| Files you opened that the documentation did not send you to | |
| **Total bytes of every file you read**, documents and sources together | the number that drives the cost: context is re-sent on every turn, so a file opened early is paid for again on every turn after it |

Then, in a few sentences: what the documentation answered well, what you had to guess
or discover the hard way, and which page you wish had said something it did not. Say
plainly wherever you had to search the repository instead of being routed to an
answer -- that is the finding this exercise is really after, and it is worth more
than the numbers.
