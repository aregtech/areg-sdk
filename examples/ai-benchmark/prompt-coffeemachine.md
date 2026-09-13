# Prompt: coffee machine with a state machine

A task for an AI agent: what the software must do, not how to build it.

This is the shape of any system that runs a multi-stage sequence which can be
interrupted and then has to carry on from where it stopped, rather than start
again.

---

## The task

Build a **coffee machine** and a **simulated user**, as two separate programs that
talk to each other. The user program drives the machine; the machine does the work.
Neither may reach into the other's memory: everything passes over the connection.

### The machine

**Money.** It accepts coins of 5, 10, 20, 50, 100 and 200 cents. Inserted coins add
to the credit. The credit is visible to the user program at all times, and the user
program is told when it changes without having to ask.

**Drinks.** Three of them, each with a price and a recipe:

| Drink | Price | Water | Beans | Milk |
|---|---|---|---|---|
| Espresso | 120 | 40 | 9 | 0 |
| Cappuccino | 180 | 60 | 9 | 80 |
| Latte | 200 | 60 | 9 | 150 |

Water and milk are in millilitres, beans in grams. The tanks start full: 2000 ml
water, 500 g beans, 1000 ml milk.

**Choosing.** The user program asks for a drink. The machine accepts the order only
when the credit is at least the price **and** every ingredient the recipe needs is
available. If either fails, the order is refused with a reason the user program can
report -- and refusing is not an error, the machine simply keeps waiting.

**Making it.** An accepted order takes the machine through several stages, each
taking a short but visible time (a few hundred milliseconds is enough; this is a
simulation):

1. grinding the beans
2. heating the water
3. brewing
4. frothing the milk -- **only for drinks whose recipe needs milk**
5. dispensing

The user program is told which stage the machine is in as it changes. When the drink
is finished, the price is taken from the credit and the ingredients from the tanks.

**Interruption -- the important requirement.** The user program can **pause** the
machine at any point while a drink is being made, and **resume** it later. On resume
the machine must continue from the stage it was interrupted in. It must not start the
drink again, and it must not skip ahead. Pausing during frothing resumes in frothing.

**Cancelling.** The user program can cancel. A cancel before an order returns the
whole credit; a cancel while a drink is being made abandons it and returns the whole
credit. A drink abandoned part-way is not charged and no ingredient is drawn for it:
only the time is lost.

**Running low.** The machine warns the user program when an ingredient falls below
20% of its tank, and again when a tank is empty. A warning is sent once when the
level is crossed, not repeated on every check. The user program does not poll for
this; it is told.

**Refilling.** The user program can refill any tank to full.

**Stopping and timing out.** The machine runs until it is stopped: it accepts
`-q` or `--quit` typed at its console and exits cleanly. Neither program may wait
forever -- if something it is waiting for has not arrived within 20 seconds, it
reports that and exits. If one side goes away mid-scenario, the other reports the
loss and exits non-zero.

### The simulated user

A second program that connects to the machine and runs this scenario, reporting each
step to the console so a person can read what happened:

1. order a Cappuccino with no money -- expect a refusal for insufficient credit
2. insert 200 cents, order a Cappuccino -- expect it to be accepted
3. pause while it is being made, wait, resume -- expect it to continue from the stage
   it was in, and say which stage that was
4. wait for the drink, then check the credit is 20 cents
5. insert coins and order Lattes until the milk warning arrives, then until an
   ingredient runs out -- expect a refusal naming the missing ingredient
6. refill, order one more Latte, and confirm it succeeds
7. order an Espresso and confirm it finishes without any frothing stage
8. start one more small drink, then cancel it -- expect the whole credit returned
   and no ingredient drawn

Then exit. **Exit code 0 if every expectation held, non-zero otherwise**, printing
which step failed. The consumer must survive the provider being started after it.

### Proving it

Two of the requirements below cannot be shown by a run in which everything works, so
they need a second run of their own:

1. **The normal run** -- the sequence above, end to end, exit 0.
2. **A run where the other side is taken away.** Start both, let the sequence reach
   the middle, then stop the machine abruptly. The simulated user must say that it
   lost the other
   side and exit non-zero. It must not hang, and it must not exit 0.

**An acceptance item counts as passing only when the output of one of those runs shows
it.** An item you believe you implemented but never observed is reported as not
passing: naming the ones you could not prove is worth more than a full score.

### Acceptance checklist

The run is a success when all of these hold. Score any implementation, in any
framework, against this list:

- [ ] two separate programs, talking over the connection only
- [ ] credit is published and the user program is notified when it changes
- [ ] an order is refused when credit is short, with a reason
- [ ] an order is refused when an ingredient is missing, naming it
- [ ] the making stages are reported to the user program as they change
- [ ] milk frothing happens for Cappuccino and Latte and not for Espresso
- [ ] **pause and resume continue from the interrupted stage, proven in the output**
- [ ] cancel returns the whole credit
- [ ] a low warning is sent once per crossing, not repeatedly
- [ ] refilling works and the machine recovers
- [ ] the machine accepts `-q` / `--quit` at its console and exits cleanly
- [ ] neither program waits more than 20 seconds for something that never arrives
- [ ] if one side goes away mid-scenario, the other reports it and exits non-zero
- [ ] the scenario exits 0, and non-zero when an expectation fails
- [ ] no busy-waiting and no sleeping inside a message handler

---

## What to deliver

**Two programs**, each with its own entry point, in their own subdirectories of the
project. Nothing outside the project directory, nothing added to the framework's own
build, and no IDE or editor project files.

**The machine's behaviour must be an explicit state machine** -- nested stages,
guarded transitions, and resuming an interrupted sequence where it left off --
not a hand-written chain of `if`s over flags. The pause-and-resume requirement is
the whole reason this task exists.

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
Say plainly wherever you had to search for an answer instead of being routed to one
-- that is the finding this exercise is really after, and it is worth more than the
table.
