# Prompt: coffee machine with a state machine

A task prompt for an AI agent. It is written the way a developer hands work to a
colleague: what the software must do, not how to build it.

**To run it.** Start the session in the directory you want the project generated
into, with read access to the checkout -- **not inside the checkout**, where a session
loads the SDK's own maintainer-facing `CLAUDE.md`, which no user of areg ever sees and
which makes the run incomparable. Then say:

> Read `<checkout>/examples/ai-prompts/coffee-machine.md` and carry it out.
> Generate into this directory.

Everything the agent needs is below. Part 1 is the task and names no framework, so
the same requirements can be given to gRPC, ZeroMQ, DDS or anything else; only
Part 2 changes. Part 3 is the report to produce, and it is what makes two runs
comparable.

---

## Part 1 -- The task

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

**Start by scaffolding, not by reading.** From an empty directory outside the SDK:

```
python3 <areg-sdk>/tools/agent/setup_project.py --name <name> --root . --mode ipc \
        --sdk-root <areg-sdk>
```

It writes a project that already builds and runs, and its own short `AGENTS.md`.
Read that, then follow where it routes you -- do not search the checkout. Replace the
scaffolded document under `src/services/` with your own, point `src/CMakeLists.txt`
at it, and run `tools/agent/gen_skeleton.py --doc <your document> --out src --force`
for the provider and consumer classes rather than hand-writing declarations.

Read `AGENTS.md` first and follow where it routes you. That file is the entry point,
and it is deliberately the only thing here that points at anything: whether it gets
you the rest is what this exercise measures. Work out the project layout, the build
wiring and the conventions from where it sends you, not from files named here.

**The machine's logic must be a state machine described in a `.fsml` document, not
hand-written `if` chains.** The pause/resume requirement is what the document's
history support is for. The service contract between the two programs is a `.siml`
document. Both are generated into code at build time -- never write generated files
by hand, and never commit them.

Deliver:

- the service contract and the state machine documents, under `services/`
- the provider sources (the machine) and the consumer sources (the simulated user),
  in separate subdirectories, each with its own `main.cpp`
- the build file that compiles both programs and turns both documents into code

Do **not** write `ReadMe.md`: the maintainer writes it, and it records that
everything else here was generated.

**Verify before you report, in this order:**

1. `python3 <areg-sdk>/tools/agent/check_contract.py . --strict` -- the mistakes a
   build cannot catch; run it before you build
2. `cmake -B build && cmake --build build -j`
3. `python3 <areg-sdk>/tools/agent/run_scenarios.py --build build/bin` -- exit 0 is
   a pass

You are finished only when it builds **and** the scenario exits 0. At most **3
build-and-fix cycles and 3 run-and-fix cycles**; if it has not converged after the
third of either, stop and report what fails, the exact output, and what you think
the cause is. Widening a timeout, adding a sleep or loosening what the scenario
expects is not a fix.

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
| Lines generated from the two documents | |
| Acceptance checklist items passing | out of 15 |
| Documentation pages you opened, and their total size | |
| Files you opened that the documentation did not send you to | |
| **Total bytes of every file you read**, documents and sources together | the number that drives the cost: context is re-sent on every turn, so a file opened early is paid for again on every turn after it |

Then, in a few sentences: what the documentation answered well, what you had to guess
or discover the hard way, and which page you wish had said something it did not. Say
plainly wherever you had to search the repository instead of being routed to an
answer -- that is the finding this exercise is really after, and it is worth more
than the numbers.
