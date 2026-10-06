# Prompt: a washing machine, with a simulated user

A task for an AI agent: what the software must do, not how to build it.

This is the shape of any controller that runs a long programme made of the same steps
used again and again, where one step may fail and be retried, and an operator may pause
it between steps.

---

## The task

Build a **washing machine controller** and a **simulated user**, as two separate
programs that talk to each other. Neither may reach into the other's memory: everything
passes over the connection.

### The washing machine

**Programmes.** A cycle is started with one of three programmes:

| programme | rinses | spin |
|---|---|---|
| `quick` | 1 | 400 ms |
| `normal` | 2 | 600 ms |
| `heavy` | 3 | 800 ms |

A cycle runs these stages in order: **wash**, **rinse 1** up to **rinse N**, **spin**,
then it is done.

- **The wash** is: fill, wash for **600 ms**, drain.
- **Every rinse** is: fill, agitate for **300 ms**, drain.
- **A fill** takes **200 ms** when the water flows. **A drain** takes **200 ms**.

The fill and the drain are the same in the wash and in every rinse, and a rinse is the
same every time it runs. Each of them is one piece of behaviour reached from several
places, not a copy per place. All timing is kept by the controller; no client drives it.

**The fill attempt limit -- the important requirement.** A fill attempt that gets no
water ends after **300 ms** and is announced as failed, with its attempt number and the
stage it belongs to. A fill makes at most **2** attempts. When both fail, the cycle goes
to **fault**, naming the stage whose fill failed. **Every fill counts its attempts from
1, however the previous fill ended.**

**Simulating a water failure.** A client can tell the machine that the next **K** fill
attempts of a named stage (`wash`, `rinse 1`, `rinse 2`, ...) get no water. Each failed
attempt uses up one of the K; an attempt after they are used up gets water. Without
this request every fill gets water.

**Fault and retry.** In fault, a start is refused naming the fault. A **retry** continues
the cycle with the stage whose fill failed, starting again from its fill. A stage that
had already finished is not run again. A retry outside fault is refused, naming the
reason.

**Pause.** A pause during a cycle is accepted and takes effect **when the current stage
finishes**: the machine announces that it is paused after that stage and starts no new
stage. A **resume** continues with the next stage. A pause during spin is refused,
naming that the cycle is finishing. A pause while paused, a pause with no cycle, and a
resume when not paused are refused, naming the reason. While paused, a start is refused
naming the pause.

**The door.** A client can open and close the door. A start with the door open is
refused, naming the door. The door is locked from the start of a cycle until the cycle
is done, and stays locked while paused and in fault; an open while locked is refused,
naming the lock.

**What the machine publishes.** What it is doing (idle, running, paused, fault), the
current stage, and the door (open, closed, locked) are published values: a client that
starts watching late receives the current values at once, and every client receives
every change. Every stage started, every stage finished, every failed fill attempt, the
pause taking effect and the cycle done are announced.

**Stopping and timing out.** The controller runs until it is stopped: it accepts `-q`
or `--quit` typed at its console and exits cleanly. Neither program may wait forever --
if something it is waiting for has not arrived within 20 seconds, it reports that and
exits. If one side goes away mid-scenario, the other reports the loss and exits
non-zero.

### The simulated user

A second program that connects to the machine and runs this scenario, reporting each
step to the console so a person can read what happened:

1. with the door open, start `normal` -- expect a refusal naming the door
2. close the door and start `normal` -- expect it accepted, the door locked, then wash,
   rinse 1, rinse 2 and spin each started and finished in that order, then the cycle
   done and the door unlocked
3. start `heavy`; when rinse 2 has **started**, pause -- expect the pause accepted,
   rinse 2 to finish, then the machine paused, and no rinse 3 started
4. while paused, start `quick` -- expect a refusal naming the pause
5. resume -- expect rinse 3, then spin, then the cycle done; rinse 2 is not run again
6. tell the machine that the next **3** fill attempts of `wash` get no water, then start
   `quick` -- expect wash fill attempt 1 failed, attempt 2 failed, then the fault naming
   the wash
7. while in fault, start `normal` -- expect a refusal naming the fault
8. retry -- expect the wash to start again from its fill, with fill attempt **1**
   failed (the third of the three) and attempt 2 succeeding; then rinse 1, spin, and
   the cycle done
9. tell the machine that the next **2** fill attempts of `rinse 1` get no water, then
   start `quick` -- expect the wash to finish, rinse 1 fill attempts 1 and 2 failed,
   then the fault naming rinse 1
10. retry -- expect rinse 1 to start again from its fill and succeed at attempt 1, the
    wash **not** run again, then spin and the cycle done
11. retry once more -- expect a refusal naming that there is no fault

Once a step's expectation has held, the simulated user prints its line below, with this
exact wording, alone or at the end of a line:

```lines
step 1: refused, door open
step 2: normal done: wash, rinse 1, rinse 2, spin, door unlocked
step 3: paused after rinse 2, no rinse 3 started
step 4: refused, paused
step 5: heavy done: rinse 3, spin, rinse 2 not run again
step 6: wash fill attempts 1 and 2 failed, fault in wash
step 7: refused, fault
step 8: wash fill attempt 1 failed, attempt 2 succeeded, quick done
step 9: rinse 1 fill attempts 1 and 2 failed, fault in rinse 1
step 10: rinse 1 filled at attempt 1, wash not run again, quick done
step 11: retry refused, no fault
```

Then exit. **Exit code 0 if every expectation held, non-zero otherwise**, printing
which step failed. The simulated user must survive the controller being started after
it.

### Proving it

Two of the requirements below cannot be shown by a run in which everything works, so
they need a second run of their own:

1. **The normal run** -- the sequence above, end to end, exit 0.
2. **A run where the other side is taken away.** Start both, let the sequence reach
   the middle, then stop the controller abruptly. The simulated user must say that it
   lost the other side and exit non-zero. It must not hang, and it must not exit 0.

**An acceptance item counts as passing only when the output of one of those runs shows
it.** An item you believe you implemented but never observed is reported as not
passing: naming the ones you could not prove is worth more than a full score.

### Acceptance checklist

The run is a success when all of these hold. Score any implementation, in any
framework, against this list:

- [ ] two separate programs, talking over the connection only
- [ ] each programme runs wash, its number of rinses and spin, in order, then is done
- [ ] **the fill, the drain and the rinse are each one piece of behaviour reached from
      several places, not copies**
- [ ] **every fill counts its attempts from 1, however the previous fill ended, and the
      second failed attempt puts the cycle in fault naming the stage**
- [ ] **a retry continues with the stage whose fill failed, from its fill, and no
      finished stage is run again**
- [ ] **a pause takes effect when the current stage finishes, and a resume continues
      with the next stage: no stage run twice, none skipped**
- [ ] a start with the door open, while paused and in fault, a retry with no fault, and
      a pause during spin are each refused, naming the reason
- [ ] the door is locked from start to done, including while paused and in fault
- [ ] all timing is kept by the controller; no client sends ticks or waits to pace it
- [ ] what the machine is doing, the stage and the door are published, and a client
      that starts watching late receives the current values at once
- [ ] the controller's behaviour is a state machine: named states, and each reused
      piece declared once
- [ ] the controller accepts `-q` / `--quit` at its console and exits cleanly
- [ ] neither program waits more than 20 seconds for something that never arrives
- [ ] if one side goes away mid-scenario, the other reports it and exits non-zero
- [ ] the scenario exits 0, and non-zero when an expectation fails
- [ ] no busy-waiting and no sleeping inside a message handler

---

## What to deliver

**Two programs**, each with its own entry point, in their own subdirectories of the
project. Nothing outside the project directory, nothing added to the framework's own
build, and no IDE or editor project files.

**The controller's behaviour is a declared state machine.** States, transitions, guards
and the actions each transition runs are named in one declaration, and the code that
executes them comes from it. The fill, the drain and the rinse are each declared once
and reused where they are needed, not repeated per stage.

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
Say how you made the rinse and the fill reusable, where the attempt count starts again,
and whether pausing between stages or retrying a failed stage made you change the
machine's structure, and why. Say plainly wherever you had to search for an answer
instead of being routed to one -- that is the finding this exercise is really after,
and it is worth more than the table.
