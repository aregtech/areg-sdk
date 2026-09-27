# Prompt: an elevator, with a panel and a display

A task for an AI agent: what the software must do, not how to build it.

This is the shape of any machine that runs a long sequence, can be halted anywhere in
it, and must pick up exactly where it stopped -- while several independent programs
watch it at once.

---

## The task

Build an **elevator controller**, a **simulated passenger** and a **floor display**, as
three separate programs that talk to each other. None may reach into another's memory:
everything passes over the connection. The passenger and the display are both clients
of the controller at the same time, and neither knows the other exists.

### The elevator controller

One car serves floors **0 to 5**. It starts at floor **0** with its doors closed.

**Timing.** The car takes **400 ms** to travel one floor. Doors take **300 ms** to
open, stay open for **800 ms**, and take **300 ms** to close. All of this timing is
kept by the controller itself; no client drives it.

**A call.** A client calls the car to a floor. The call is answered at once with
accepted or refused, and a refusal names the reason. A call to a floor outside 0-5 is
refused. While the car is serving a call, a second call is refused, naming that the
car is busy.

**Serving a call to another floor.** The car moves one floor at a time toward the
target, announcing every floor it reaches. At the target it opens its doors, holds
them open, closes them, and becomes idle there.

**A call to the floor the car is already at.** The car does not move and announces no
floor: it goes straight to opening its doors, and continues exactly as it would on
arriving at a floor. This is the same door sequence as after a journey, entered at a
later step -- not a second copy of it.

**Emergency stop -- the important requirement.** A client can press the emergency
stop at any time while a call is being served. The car halts where it is: if it is
travelling it stops at the floor it has reached, and if its doors are opening, open or
closing they stay as they are. The controller announces that it is halted and where.
While halted, a call is refused naming the reason, and a second emergency stop is
refused naming the reason. When the stop is **released**, the car **continues from the
exact step it was halted in**: toward the same target if it was travelling, without
re-announcing the floor it is at; or with the same door step if the doors were moving
or open. Nothing is restarted from the beginning, and the target is not forgotten.

**What the controller publishes.** The car's current floor, its direction (up, down,
none), and what it is doing (idle, travelling, doors opening, doors open, doors
closing, halted, out of service) are published values: a client that starts watching
late receives the current values at once, and every client receives every change.

**Out of service.** A client can take the car out of service when it is idle. The
controller announces it to every client and exits 0 after the announcement has been
sent. A request to go out of service while a call is being served is refused naming
the reason.

**Stopping and timing out.** The controller runs until it is stopped: it accepts `-q`
or `--quit` typed at its console and exits cleanly. No program may wait forever -- if
something it is waiting for has not arrived within 20 seconds, it reports that and
exits. If one side goes away mid-scenario, the other reports the loss and exits
non-zero.

### The floor display

A second client that watches the controller and does nothing else. It prints one line
per change of floor and one per change of what the car is doing, so a person can
follow the car from the display alone. When the controller announces it is out of
service, the display prints that and exits 0. If the controller goes away without that
announcement, the display reports the loss and exits non-zero.

### The simulated passenger

A third program that connects to the controller and runs this scenario, reporting each
step to the console so a person can read what happened:

1. call the car to floor **0**, where it already is -- expect the doors to open and
   close with **no** floor announced and no movement, then idle at floor 0
2. call the car to floor **4** -- expect floors 1, 2, 3, 4 announced in that order, the
   door sequence at 4, then idle at floor 4
3. while step 2 is still being served, call the car to floor **2** -- expect a refusal
   naming that the car is busy
4. call the car to floor **9** -- expect a refusal naming the floor as out of range
5. call the car to floor **0**; when floor **3** has been announced, press the
   emergency stop -- expect the car halted at floor 3 or 2, whichever it had reached
6. while halted, call the car to floor **5** -- expect a refusal naming the halt
7. release the stop -- expect the car to continue down to floor 0 from where it halted,
   with no floor announced twice and none skipped, then the door sequence at 0 and idle
8. call the car to floor **1**; when the doors at floor 1 are **open**, press the
   emergency stop -- expect the doors to stay open and the car halted at floor 1
9. release the stop -- expect the doors to close, not to open again, and the car idle
   at floor 1
10. take the car out of service -- expect the announcement

Then exit. **Exit code 0 if every expectation held, non-zero otherwise**, printing
which step failed. The passenger program must survive the controller being started
after it.

### Proving it

Two of the requirements below cannot be shown by a run in which everything works, so
they need a second run of their own:

1. **The normal run** -- all three programs, the sequence above end to end, the
   passenger and the display both exit 0.
2. **A run where the other side is taken away.** Start all three, let the sequence
   reach the middle, then stop the controller abruptly. The passenger and the display
   must each say that they lost the other side and exit non-zero. Neither may hang,
   and neither may exit 0.

**An acceptance item counts as passing only when the output of one of those runs shows
it.** An item you believe you implemented but never observed is reported as not
passing: naming the ones you could not prove is worth more than a full score.

### Acceptance checklist

The run is a success when all of these hold. Score any implementation, in any
framework, against this list:

- [ ] three separate programs, talking over the connection only
- [ ] the passenger and the display are clients of the controller at the same time
- [ ] a call to another floor announces every floor on the way, in order, then runs
      the door sequence
- [ ] a call to the current floor runs the door sequence with no movement and no floor
      announced
- [ ] the door sequence after a journey and after a call to the current floor is one
      piece of logic, entered at a different step, not two copies
- [ ] a call while busy, a call out of range and a call while halted are each refused,
      naming the reason
- [ ] **an emergency stop while travelling, once released, continues toward the same
      target from the floor reached: no floor announced twice, none skipped**
- [ ] **an emergency stop with the doors open, once released, closes them without
      opening them again**
- [ ] all timing is kept by the controller; no client sends ticks or waits to pace it
- [ ] the display shows every floor change and every change of what the car is doing,
      and a display started late receives the current values at once
- [ ] the controller's behaviour is a state machine: named states, and the door
      sequence and the journey are each declared once
- [ ] out of service is announced to every client; the display exits 0 on it
- [ ] the controller accepts `-q` / `--quit` at its console and exits cleanly
- [ ] no program waits more than 20 seconds for something that never arrives
- [ ] if one side goes away mid-scenario, the other reports it and exits non-zero
- [ ] the scenario exits 0, and non-zero when an expectation fails
- [ ] no busy-waiting and no sleeping inside a message handler

---

## What to deliver

**Three programs**, each with its own entry point, in their own subdirectories of the
project. Nothing outside the project directory, nothing added to the framework's own
build, and no IDE or editor project files.

**The controller's behaviour is a declared state machine.** States, transitions, guards
and the actions each transition runs are named in one declaration, and the code that
executes them comes from it. Resuming after the emergency stop comes from that
declaration, not from a saved copy of the state kept by hand.

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
Say whether entering the door sequence at a later step, or resuming after the stop,
made you change the machine's structure, and why. Say plainly wherever you had to
search for an answer instead of being routed to one -- that is the finding this
exercise is really after, and it is worth more than the table.
