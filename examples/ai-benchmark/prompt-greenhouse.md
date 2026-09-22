# Prompt: a greenhouse, with zones that can move between programs

A task for an AI agent: what the software must do, not how to build it.

This is the shape of any system made of several identical units behind one contract,
where which program hosts which unit is a deployment decision taken after the code is
written -- and may change without touching it.

---

## The task

Build a **zone host** and a **simulated operator console**, as two separate programs
that talk to each other. Neither may reach into the other's memory: whatever crosses
between them passes over the connection.

### The zones

A greenhouse has **three climate zones**, named `north`, `centre` and `south`. Every
zone is the **same** controller with the same contract, running as its own instance
under its own name. A zone has:

- a **target temperature** and a **current temperature**, both published; a client
  that starts watching late receives the current values at once
- a **mode**: idle, heating, cooling, or held
- a **set target** request, answered with accepted or refused. A target below 5 or
  above 35 is refused, naming the reason
- a **hold** request, which freezes the zone in held until a **release** request; a
  set target while held is refused, naming the reason

**The climate.** Each zone keeps its own timing: every **200 ms** its current
temperature moves one degree toward the target and the mode says which way it is
going; on reaching the target it becomes idle and announces that it has settled. The
zones start at `north` 15, `centre` 18, `south` 21, each with its target equal to its
current temperature.

### Where the zones run -- the important requirement

**Which program hosts which zone is configuration, not code.** Two deployments must
work from the **same build**, chosen when the programs are started:

- **Deployment A**: all three zones run in the zone host. The console is a client of
  all three over the connection.
- **Deployment B**: `north` and `centre` run in the zone host; `south` runs **inside
  the console program**. The console talks to `south` through the same contract as to
  the other two, and its code does not know or care that `south` is now local.

No source file differs between the two deployments, nothing is rebuilt, and the
console's scenario code contains no branch on where a zone runs. Switching deployment
is a change to a configuration file or a startup option, and nothing else.

**Stopping and timing out.** The zone host runs until it is stopped: it accepts `-q`
or `--quit` typed at its console and exits cleanly. Neither program may wait
forever -- if something it is waiting for has not arrived within 20 seconds, it
reports that and exits. If one side goes away mid-scenario, the other reports the loss
and exits non-zero.

### The simulated console

A second program that connects to the three zones by name and runs this scenario,
reporting each step to the console so a person can read what happened:

1. wait until all three zones are connected, and print each zone's current and target
   temperature -- expect 15, 18 and 21, each equal to its target
2. set `north` to **20**, `centre` to **16** and `south` to **24**, all three before
   waiting for any -- expect all three accepted, `north` heating, `centre` cooling,
   `south` heating
3. wait for all three to settle -- expect `north` to pass 16, 17, 18, 19, 20 in order,
   `centre` 17, 16, and `south` 22, 23, 24, and each to announce that it settled
4. set `centre` to **40** -- expect a refusal naming the range
5. hold `south`, then set it to **18** -- expect a refusal naming the hold
6. release `south`, set it to **22** -- expect it accepted, cooling, and settling at 22
7. print the final temperature of each zone -- expect 20, 16 and 22

Then exit. **Exit code 0 if every expectation held, non-zero otherwise**, printing
which step failed. The console must survive the zone host being started after it.

### Proving it

Three of the requirements below cannot be shown by one run in which everything works,
so they need runs of their own:

1. **The normal run, deployment A** -- the sequence above, end to end, exit 0.
2. **The normal run, deployment B** -- the same programs from the same build, started
   with the other deployment, the same sequence, exit 0.
3. **A run where the other side is taken away**, in deployment A. Start both, let the
   sequence reach the middle, then stop the zone host abruptly. The console must say
   that it lost the other side and exit non-zero. It must not hang, and it must not
   exit 0.

**An acceptance item counts as passing only when the output of one of those runs shows
it.** An item you believe you implemented but never observed is reported as not
passing: naming the ones you could not prove is worth more than a full score.

### Acceptance checklist

The run is a success when all of these hold. Score any implementation, in any
framework, against this list:

- [ ] two separate programs, talking over the connection only
- [ ] the three zones are three instances of one controller behind one contract,
      reached by name; the controller is written once
- [ ] each zone keeps its own timing, and the three move at the same time
- [ ] a zone that starts being watched late reports its current values at once
- [ ] every step of every zone is announced in order, and each zone announces that it
      settled
- [ ] a target out of range, and a target while held, are each refused naming the
      reason
- [ ] **deployment A and deployment B both pass the same scenario, from the same
      build, chosen at start-up**
- [ ] **in deployment B the console reaches `south` inside its own program through the
      same contract, and the scenario code does not branch on where a zone runs**
- [ ] the zone host accepts `-q` / `--quit` at its console and exits cleanly
- [ ] neither program waits more than 20 seconds for something that never arrives
- [ ] if one side goes away mid-scenario, the other reports it and exits non-zero
- [ ] the scenario exits 0, and non-zero when an expectation fails
- [ ] no busy-waiting and no sleeping inside a message handler

---

## What to deliver

**Two programs**, each with its own entry point, in their own subdirectories of the
project. Nothing outside the project directory, nothing added to the framework's own
build, and no IDE or editor project files.

**One controller for all three zones.** A zone is an instance with a name, not a
copy of the code.

**The deployment is data.** Where each zone runs is read at start-up, from a file or
an option, and the two deployments are the only difference between the two normal
runs.

**The contract between the programs is declared once**, in whatever form the framework
declares an interface, and the code that carries it over the connection is generated
from that declaration rather than written by hand. Never edit a generated file and never
commit one.

Do **not** write a readme file.

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
Say what moving `south` into the console cost you: nothing, a configuration entry, or
code. Say plainly wherever you had to search for an answer instead of being routed to
one -- that is the finding this exercise is really after, and it is worth more than the
table.
