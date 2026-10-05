# Prompt: an order desk, with a warehouse and a customer

A task for an AI agent: what the software must do, not how to build it.

This is the shape of any service that answers its own clients only after asking other
services, keeps several such conversations open at once, and undoes the first step when
a later one fails.

---

## The task

Build a **warehouse**, an **order desk** and a **simulated customer**, as three separate
programs that talk to each other. None may reach into another's memory: everything
passes over the connection.

### The warehouse

The warehouse program offers **two separate services**, each with its own contract:
**stock** and **payment**. Both run in the warehouse program.

**Stock.** It starts with:

| item | in stock | unit price |
|---|---|---|
| `bolt` | 10 | 3 |
| `nut` | 5 | 2 |
| `gear` | 3 | 40 |

- **Reserve** an item and a quantity for an order: answered with reserved, or refused
  naming the reason -- an unknown item, or too few in stock, naming how many there are.
  A reservation takes the quantity out of stock at once.
- **Release** an order's reservation: the quantity goes back into stock.
- **Commit** an order's reservation: it becomes final.
- The quantity in stock of every item is a published value: a client that starts
  watching late receives the current values at once, and every client receives every
  change.

**Payment.** **Charge** an amount for an order: answered with approved or declined. An
amount above **100** is declined, naming the limit. Every charge takes **300 ms** to
answer, and that time is kept by the payment service itself. The total of all approved
charges is a published value.

### The order desk

The order desk serves the customer. It is a client of both warehouse services.

**Placing an order.** A customer places an order for an item and a quantity. The desk:

1. reserves it at the stock service; if that is refused, it refuses the order naming
   the stock's reason;
2. charges the price times the quantity at the payment service;
3. if the charge is approved, commits the reservation and accepts the order, naming its
   order number and the amount; if it is declined, releases the reservation and
   refuses the order naming the payment's reason.

**The desk answers an order only after the warehouse has answered, and never blocks
while it waits.** Several orders may be in progress at once, from one customer or many,
and each is answered with its own outcome, naming the order it belongs to. Order numbers
start at 1 and count every order placed, accepted or not.

The number of accepted orders is a published value.

**Stopping and timing out.** The warehouse and the desk run until they are stopped:
each accepts `-q` or `--quit` typed at its console and exits cleanly. No program may
wait forever -- if something it is waiting for has not arrived within 20 seconds, it
reports that and exits. If a program goes away mid-scenario, every program connected to
it reports the loss and exits non-zero.

### The simulated customer

A third program, a client of the order desk **and** of the stock service at the same
time, that runs this scenario, reporting each step to the console so a person can read
what happened:

1. wait until the desk and the stock are both connected, and print the stock -- expect
   bolt 10, nut 5, gear 3
2. order **4 bolt** -- expect it accepted as order 1, amount 12, and the stock of bolt
   to become 6
3. order **6 nut** -- expect it refused naming the stock, with 5 available; nut stays 5
4. order **3 gear** -- expect it refused naming the payment limit (the amount is 120),
   and the stock of gear back at 3 once the order is refused
5. place three orders **all before waiting for any**: **2 gear**, **2 bolt**, **2 gear**
   -- expect order 4 accepted (amount 80), order 5 accepted (amount 6), and order 6
   refused naming the stock, with 1 gear available; each answer names its own order
6. print the stock and the accepted orders -- expect bolt 4, nut 5, gear 1, and 3
   accepted orders

Then exit. **Exit code 0 if every expectation held, non-zero otherwise**, printing
which step failed. The customer must survive the other programs being started after it.

### Proving it

Two of the requirements below cannot be shown by a run in which everything works, so
they need a second run of their own:

1. **The normal run** -- all three programs, the sequence above end to end, the
   customer exits 0.
2. **A run where a side is taken away.** Start all three, let the sequence reach the
   middle, then stop the warehouse abruptly. The desk and the customer must each say
   that they lost the other side and exit non-zero. Neither may hang, and neither may
   exit 0.

**An acceptance item counts as passing only when the output of one of those runs shows
it.** An item you believe you implemented but never observed is reported as not
passing: naming the ones you could not prove is worth more than a full score.

### Acceptance checklist

The run is a success when all of these hold. Score any implementation, in any
framework, against this list:

- [ ] three separate programs, talking over the connection only
- [ ] stock and payment are two services with two contracts, both in the warehouse
      program
- [ ] the desk is a client of both warehouse services; the customer is a client of
      the desk and of the stock at the same time
- [ ] **the desk answers an order only after the warehouse has answered, and never
      blocks while it waits**
- [ ] **three orders in progress at once are each answered with their own outcome and
      order number**
- [ ] **a declined charge releases the reservation: the stock returns to its level
      before the order**
- [ ] a refusal names its reason: the stock with the quantity available, the payment
      with its limit
- [ ] the 300 ms of a charge is kept by the payment service; no client paces it
- [ ] the stock, the approved total and the accepted orders are published, and a
      client that starts watching late receives the current values at once
- [ ] the warehouse and the desk accept `-q` / `--quit` at their consoles and exit
      cleanly
- [ ] no program waits more than 20 seconds for something that never arrives
- [ ] if the warehouse goes away mid-scenario, the desk and the customer each report it
      and exit non-zero
- [ ] the scenario exits 0, and non-zero when an expectation fails
- [ ] no busy-waiting and no sleeping inside a message handler

---

## What to deliver

**Three programs**, each with its own entry point, in their own subdirectories of the
project. Nothing outside the project directory, nothing added to the framework's own
build, and no IDE or editor project files.

**Each contract is declared once** -- stock, payment and the order desk -- in whatever
form the framework declares an interface, and the code that carries it over the
connection is generated from that declaration rather than written by hand. Never edit a
generated file and never commit one.

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
Say how the desk keeps track of the orders in progress while it waits for the warehouse,
and where you learned how. Say plainly wherever you had to search for an answer instead
of being routed to one -- that is the finding this exercise is really after, and it is
worth more than the table.
