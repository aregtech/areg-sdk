# Prompt: a charging site, with a site manager and a simulated driver

A task for an AI agent: what the software must do, not how to build it.

This is the shape of any site that runs several identical devices, shares a limited
resource between them, and coordinates them from a separate program that answers its
own clients only after the devices have answered -- while a background reading changes
what the site may do.

---

## The task

Build a **charging station**, a **site manager** and a **simulated driver**, as three
separate programs that talk to each other. None may reach into another's memory:
everything passes over the connection.

### The charging station

The station runs **three charging bays**, named `bay 1`, `bay 2` and `bay 3`. They are
three instances of one charger, each with its own state, reached by a client through its
name. Adding a fourth bay must need no new code, only one more name.

**What a bay publishes.** What it is doing (idle, authorizing, charging, suspended,
finishing, fault) and the energy delivered in the current session, in whole kWh, are
published values: a client that starts watching late receives the current values at
once, and every client receives every change.

**Starting.** A start names a target of **1 to 50** kWh. It is accepted only when the bay
is idle; otherwise it is refused naming the reason (busy, fault, or target out of range).
An accepted start sets the energy to 0 and authorizes.

**Authorizing.** A handshake try takes **150 ms**. A bay makes at most **2** tries; a try
that fails is announced with its attempt number. When both fail, the bay goes to
**fault**, naming the handshake. **Every authorization counts its tries from 1.**

**Simulating a handshake failure.** A client can tell a bay that its next **K** handshake
tries fail. Each failed try uses up one of the K. Without this request every try
succeeds.

**Charging.** While charging, the energy rises by **1 kWh every 100 ms** until it equals the
target. Then the bay is **finishing** for **200 ms**, announces that the session is done
with the energy delivered, and is idle.

**Suspend and resume.** A suspend is accepted only while charging: the energy stops
rising at once. A resume is accepted only while suspended: the bay **authorizes again**
(the same handshake, its tries counted from 1), then continues charging **from the
energy already delivered**, toward the same target. Nothing is lost and nothing is
counted twice. A suspend when not charging and a resume when not suspended are refused,
naming the reason.

**Reset.** A reset in fault returns the bay to idle; outside fault it is refused, naming
the reason.

**The bay's behaviour is a state machine**: named states, and the handshake declared
once and used both when a session starts and when it resumes.

### The site manager

A program that is a client of all three bays at once and serves the driver.

**Sessions.** The site has **2** slots: at most two sessions at once. A **start session**
names a bay and a target. It is refused at once naming the reason when the bay name is
unknown, when both slots are taken (site full), or when that bay already has a session.
Otherwise the manager takes a slot, asks the bay to start, and **answers only after the
bay has answered and its authorization has ended**: accepted once the bay is charging;
refused, naming the bay's reason, when the bay refused or went to fault -- and then
**the slot is given back**, as if the session had never been asked for.

**It never blocks.** While one start session waits for its bay, the manager keeps
answering other requests, and two start sessions may be in flight at once.

**What the manager publishes.** The list of current sessions, one record per session with
its bay, target, energy and what the bay is doing, is a published value with the same
late-watcher rule. Every session done is announced with its bay and energy, and the slot
is given back.

**The power meter.** The manager reads the power the site may draw from a meter, every
**100 ms**, on **a thread of its own**. The readings reach the manager's logic as
messages; the meter thread never touches the manager's state directly. The meter
normally reads **22 kW**. A charging bay draws **7 kW**. A **brownout** request makes
the meter read **10 kW** for the next **600 ms**, then 22 kW again.

When the reading is lower than what the charging bays draw, the manager **suspends the
most recently started charging bay** and announces it. When the reading is enough again
for every bay that would then charge, the resumed one included, it **resumes** that bay
and announces it. A brownout during a brownout is refused,
naming the reason.

**Stopping and timing out.** The station and the manager run until they are stopped:
each accepts `-q` or `--quit` typed at its console and exits cleanly, with exit code 0: a
quit is normal behaviour, not a failure. No program may wait forever -- if something it
is waiting for has not arrived within 20 seconds, it reports that and exits. The site
manager is the site's shutdown point: the station is also its client, only to learn when
it is gone. If a program goes away abruptly mid-scenario, every program that is its
client reports the loss and exits non-zero: the station and the driver when
the site manager goes away; the site manager when the station goes away, and then the
driver, which has lost the site manager.

### The simulated driver

A third program. It is a client of the site manager, and also of `bay 3` directly. It
runs this scenario, reporting each step to the console so a person can read what
happened:

1. start a session on `bay 9` with target 5 -- expect a refusal naming the unknown bay
2. start a session on `bay 1` with target **5** and, without waiting for that answer, a
   session on `bay 2` with target **30** -- expect both accepted
3. start a session on `bay 3` with target 5 -- expect a refusal naming the full site
4. expect `bay 1`'s session done with **5** kWh, and `bay 1` gone from the session list
5. tell `bay 3` directly that its next **2** handshake tries fail, then start a session on
   `bay 3` with target 3 -- expect handshake attempts 1 and 2 failed, then a refusal
   naming the fault, and the session list holding only `bay 2`
6. start a session on `bay 1` with target **4**; once the session list shows `bay 1` at 1
   kWh, ask for a brownout -- expect `bay 1` suspended once, `bay 2` still charging, then
   `bay 1` resumed once, after the brownout, authorized again at attempt 1, and its
   session done with exactly **4** kWh
7. expect `bay 2`'s session done with **30** kWh, never suspended
8. reset `bay 3` directly -- expect `bay 3`'s published values to read idle and 0 kWh;
   then suspend `bay 3` and reset it again -- expect both refused, naming that it is not
   charging and not in fault
9. start a session on `bay 3` with target **2** -- expect it accepted and done with **2**
   kWh

Once a step's expectation has held, the driver prints its line below, with this exact
wording, alone or at the end of a line, nothing after it:

```lines
step 1: refused, unknown bay
step 2: bay 1 and bay 2 accepted
step 3: refused, site full
step 4: bay 1 done, 5 kWh
step 5: bay 3 handshake attempts 1 and 2 failed, refused, fault
step 6: bay 1 suspended in the brownout, resumed, done, 4 kWh
step 7: bay 2 done, 30 kWh, never suspended
step 8: bay 3 reset, idle and 0 kWh, suspend and reset refused
step 9: bay 3 done, 2 kWh
```

Then exit. **Exit code 0 if every expectation held, non-zero otherwise**, printing
which step failed. The driver must survive the manager being started after it, and the
manager must survive the station being started after it.

### Proving it

Two of the requirements below cannot be shown by a run in which everything works, so
they need a second run of their own:

1. **The normal run** -- all three programs, the sequence above end to end, the driver
   exits 0.
2. **A run where the other side is taken away.** Start all three, let the sequence
   reach the middle, then stop the site manager abruptly. The driver and the station
   must each say that they lost the site manager and exit non-zero. Neither may hang,
   and neither may exit 0.

**An acceptance item counts as passing only when the output of one of those runs shows
it.** An item you believe you implemented but never observed is reported as not
passing: naming the ones you could not prove is worth more than a full score. An item
marked (code) is one no output can show: count it apart, as present only where you can
name the file and the class or function that does it.

### Acceptance checklist

The run is a success when all of these hold. Score any implementation, in any
framework, against this list:

- [ ] three separate programs, talking over the connection only
- [ ] (code) three bays are three instances of one charger, reached by name; a fourth needs no
      new code
- [ ] **a start session is answered only after the bay answered and its authorization
      ended, and two start sessions are in flight at once without blocking**
- [ ] **a start session refused by its bay gives its slot back**
- [ ] a start on an unknown bay, on a full site, a suspend when not charging and a
      reset outside fault are each refused, naming the reason
- [ ] **every authorization counts its tries from 1, and the second failed try puts the
      bay in fault naming the handshake**
- [ ] **a resumed session authorizes again and continues from the energy delivered:
      nothing lost, nothing counted twice**
- [ ] (code) the handshake is declared once and used at start and at resume
- [ ] (code) the bay's behaviour is a state machine: named states
- [ ] (code) the meter is read on a thread of the manager's own, and its readings reach the
      manager's logic as messages
- [ ] a brownout suspends the most recently started bay and resumes it afterwards
- [ ] (code) all timing is kept by the station and the manager; no client sends ticks or waits
      to pace them
- [ ] what a bay is doing, its energy and the session list are published, and a client
      that starts watching late receives the current values at once
- [ ] the station and the manager accept `-q` / `--quit` at their console and exit
      cleanly, with exit code 0
- [ ] no program waits more than 20 seconds for something that never arrives
- [ ] if a program goes away abruptly mid-scenario, every program that is its client reports it
      and exits non-zero; the station is a client of the site manager for this
- [ ] the scenario exits 0, and non-zero when an expectation fails
- [ ] (code) no busy-waiting and no sleeping inside a message handler

---

## What to deliver

**Three programs**, each with its own entry point, in their own subdirectories of the
project. Nothing outside the project directory, nothing added to the framework's own
build, and no IDE or editor project files.

**The bay's behaviour is a declared state machine.** States, transitions, guards and the
actions each transition runs are named in one declaration, and the code that executes
them comes from it. The handshake is declared once and reused, not repeated.

**The contracts between the programs are declared once**, in whatever form the framework
declares an interface, and the code that carries them over the connection is generated
from those declarations rather than written by hand. Never edit a generated file and
never commit one.

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
| acceptance items passing | n of the unmarked items shown by the output, and which were not; c of the (code) items, each with where |
| checker findings, first run | name the checker, or "none run" |
| files you opened that the documentation did not route you to | names, or "none" |

**Fill it only from what you already know, and measure nothing to fill it in.** Byte
counts, line counts, token counts, tool calls and wall time are the operator's to read
from the session afterwards; computing them yourself costs turns and tells nobody
anything. An invented number makes every comparison worthless, so a figure you do not
already have is left out, not guessed.

Then three sentences at most: what the documentation answered well, what you had to
guess or discover the hard way, and which page you wish had said something it did not.
Say how the three bays became instances of one charger, how the manager held a start
session open without blocking, and how the meter's readings reached its logic. Say
plainly wherever you had to search for an answer instead of being routed to one -- that
is the finding this exercise is really after, and it is worth more than the table.
