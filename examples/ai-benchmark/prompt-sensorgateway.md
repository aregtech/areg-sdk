# Prompt: a sensor gateway, with a monitor

A task for an AI agent: what the software must do, not how to build it.

This is the shape of any device that reads hardware on a thread of its own, turns the
readings into structured records, and serves them to a client -- while its health is
decided both by what the hardware does and by what the client asks.

---

## The task

Build a **sensor gateway** and a **simulated monitor**, as two separate programs that
talk to each other. Neither may reach into the other's memory: everything passes over
the connection.

### The sensor gateway

**Three simulated sensors**, each with an id, a name and a kind:

| id | name | kind | reading number `n` gives |
|---|---|---|---|
| 1 | `inlet` | temperature | `20 + (n mod 5)` |
| 2 | `room` | humidity | `40 + (n mod 5)` |
| 3 | `line` | pressure | `1000 + (n mod 5)` |

`n` counts that sensor's readings from 0. Every reading carries the sensor id, the
kind, the value, its number `n`, and a quality: good, or suspect.

**The reading thread.** The sensors are read by **one thread of the gateway's own**,
not the thread that serves the monitor. Every **100 ms** it reads all three sensors
that are live. It does not publish anything itself and touches no data the serving
side owns: it hands every set of readings to the serving side as a message within the
process, and the serving side does everything else.

**What the gateway serves.**

- **The sensor list**, on request: every sensor with its id, name and kind.
- **Readings**, published: the most recent set of readings, one record per live
  sensor, as one value that changes every 100 ms. Every client watching receives
  every set.
- **A summary**, on request, for one sensor: the lowest, the highest and the average
  value of all its readings so far, and how many readings that covers.
- **The health**, published: one of starting, normal, degraded, fault, together with
  the list of sensors that are silent.

**The health is one state machine, driven from two threads.** The reading thread
drives it with what the hardware does: the first full set of readings moves it from
starting to normal; in normal, a sensor that has given no reading for **500 ms** moves
it to degraded; the last silent sensor coming back moves it back to normal. In
starting, a silent sensor keeps it in starting until a full set arrives. The serving side drives it
with what the monitor asks: a **reset** moves degraded or fault back to starting. Both
inputs go through the **same** machine instance, each is applied whole, and neither
thread may see the machine half-changed by the other.

**Requests that depend on the health.** A **calibrate** request is accepted only in
normal, and answered with the sensor it calibrated. In any other state it is refused
and the refusal names the state. A reset in normal is refused naming the reason.

**Simulating a fault.** The monitor can tell the gateway to **silence** one sensor and
to **restore** it. A silenced sensor gives no reading until it is restored; its
reading number continues counting while it is silent, so the numbers after the gap
are not contiguous with the ones before it.

**Stopping and timing out.** The gateway runs until it is stopped: it accepts `-q` or
`--quit` typed at its console and exits cleanly, stopping the reading thread first.
Neither program may wait forever -- if something it is waiting for has not arrived
within 20 seconds, it reports that and exits. If one side goes away mid-scenario, the
other reports the loss and exits non-zero.

### The simulated monitor

A second program that connects to the gateway and runs this scenario, reporting each
step to the console so a person can read what happened:

1. ask for the sensor list -- expect the three sensors with their names and kinds
2. wait for the health to become **normal**
3. collect **20** sets of readings -- expect every set to hold all three sensors,
   every value to match the table for its reading number, and no reading number
   skipped or repeated within a sensor
4. ask for the summary of sensor 1 -- expect lowest **20**, highest **24**, and an
   average within 0.5 of **22**
5. calibrate sensor 1 -- expect it accepted
6. silence sensor 2 -- expect the health to become **degraded**, naming sensor 2 as
   silent, and the sets of readings to continue with sensors 1 and 3 only
7. calibrate sensor 3 -- expect a refusal naming the degraded state
8. restore sensor 2 -- expect the health to return to **normal** and sensor 2 to
   reappear in the readings, with values that still match the table
9. silence sensor 3, wait for **degraded**, then reset -- expect the health to go to
   **starting**, and not to normal until sensor 3 is restored; restore it and expect
   **normal**
10. reset once more -- expect a refusal naming the normal state

Then exit. **Exit code 0 if every expectation held, non-zero otherwise**, printing
which step failed. The monitor must survive the gateway being started after it.

### Proving it

Two of the requirements below cannot be shown by a run in which everything works, so
they need a second run of their own:

1. **The normal run** -- the sequence above, end to end, exit 0.
2. **A run where the other side is taken away.** Start both, let the sequence reach
   the middle, then stop the gateway abruptly. The monitor must say that it lost the
   other side and exit non-zero. It must not hang, and it must not exit 0.

**An acceptance item counts as passing only when the output of one of those runs shows
it.** An item you believe you implemented but never observed is reported as not
passing: naming the ones you could not prove is worth more than a full score.

### Acceptance checklist

The run is a success when all of these hold. Score any implementation, in any
framework, against this list:

- [ ] two separate programs, talking over the connection only
- [ ] the sensors are read on a thread of the gateway's own, which hands its readings
      to the serving side as messages and shares no data with it
- [ ] a reading is one record type carrying id, kind, value, number and quality; the
      kind and the quality are enumerations; the set of readings and the silent
      sensors are lists; all are declared once and shared by both programs
- [ ] the sensor list names all three sensors with their kinds
- [ ] 20 sets of readings arrive with every value matching the table and no number
      skipped or repeated
- [ ] the summary of sensor 1 is correct
- [ ] silencing a sensor moves the health to degraded, naming it; restoring it moves
      the health back to normal
- [ ] calibrate is accepted in normal and refused elsewhere, naming the state
- [ ] reset moves degraded back to starting, and is refused in normal naming the reason
- [ ] **the health is one state machine instance, driven both from the reading thread
      and from the serving side, and no input from one thread is lost or applied
      half-way because of the other**
- [ ] the gateway accepts `-q` / `--quit` at its console and exits cleanly, stopping
      the reading thread first
- [ ] neither program waits more than 20 seconds for something that never arrives
- [ ] if one side goes away mid-scenario, the other reports it and exits non-zero
- [ ] the scenario exits 0, and non-zero when an expectation fails
- [ ] no busy-waiting and no sleeping inside a message handler

---

## What to deliver

**Two programs**, each with its own entry point, in their own subdirectories of the
project. Nothing outside the project directory, nothing added to the framework's own
build, and no IDE or editor project files.

**The data types are declared once**, in whatever form the framework declares shared
types -- the reading record, the enumerations, the lists -- and both programs use the
generated types. No program re-declares one of them by hand.

**The health is a declared state machine.** Its states, transitions and guards are
named in one declaration, and the code that executes them comes from it.

**The contract between the two programs is declared once**, in whatever form the
framework declares an interface, and the code that carries it over the connection is
generated from that declaration rather than written by hand. Never edit a generated
file and never commit one.

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
Say how you made the health machine safe to drive from two threads, and where you
learned how. Say plainly wherever you had to search for an answer instead of being
routed to one -- that is the finding this exercise is really after, and it is worth
more than the table.
