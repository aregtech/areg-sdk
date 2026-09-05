# Prompt: an ATM with PIN retries and card retention

A task prompt for an AI agent. It is written the way a developer hands work to a
colleague: what the software must do, not how to build it. It names no framework, so
the same requirements can be handed to gRPC, ZeroMQ, DDS, areg, or anything else.

**To run it.** This file is only the task and its acceptance checklist. To build it on
**areg**, copy `examples/ai-prompts/ai-prompt-template-text.txt`, fill in its settings,
and paste this file's content (or the path to it) into its task-description section,
then hand the result to an agent. For another framework, hand this file to an agent
together with your own delivery instructions.

This is the shape of any system that gates a sensitive action behind a retry-limited
secret, and that sometimes has to ask for the same secret twice, for two different
reasons, in the same session.

---

## The task

Build an **ATM** and a **simulated customer**, as two separate programs that talk to
each other. Neither may reach into the other's memory: everything passes over the
connection.

### The ATM

**A session starts with a card.** Once a card is inserted, the ATM asks for a PIN
before anything else is possible.

**Checking the PIN.** The customer program submits a PIN. Each wrong PIN is refused
with a reason and how many attempts remain in *this* check. The **third** wrong PIN in
a row, in any single PIN check, **retains the card**: the session ends immediately, and
the card cannot be used again for the rest of the run. A correct PIN, at any attempt
before the third, succeeds and the count of wrong attempts is forgotten -- it does not
carry over to a later, separate PIN check in the same session.

**After a successful login**, the customer program can:

- ask for the **balance**
- **withdraw** cash
- **change the PIN**

**Withdrawing.** The ATM dispenses cash only in whole units of a fixed note value. A
request that is not a whole multiple of the note value is refused, naming the reason.
A request that would push the total withdrawn *today* over a fixed daily limit is also
refused, naming the reason. Otherwise the withdrawal is accepted, the balance is
reduced by exactly that amount, and the running total withdrawn today is updated.

**Changing the PIN -- the important requirement.** Before a new PIN takes effect, the
ATM asks the customer to re-enter the **current** PIN. This is a second, independent PIN
check: it is refused and retried exactly as the PIN check at login is, including the
three-strikes card retention rule, and its attempt count starts fresh regardless of how
many wrong attempts the login already used.

### The simulated customer

A second program that connects to the ATM and runs this scenario, reporting each step
to the console so a person can read what happened:

1. insert a card, submit a wrong PIN twice, then the correct PIN -- expect two
   refusals naming how many attempts remain, then a successful login
2. ask for the balance -- expect the correct starting balance
3. try to withdraw an amount that is not a whole multiple of the note value -- expect
   a refusal naming the reason
4. try to withdraw an amount that would exceed the daily limit -- expect a refusal
   naming the reason
5. withdraw a valid amount -- expect it to succeed, the balance to drop by exactly
   that amount, and the daily total to reflect it
6. start changing the PIN, submit the current PIN wrong once, then correctly --
   expect this check to succeed with its own attempt count, unaffected by step 1
7. end the session
8. insert the same card again and submit three wrong PINs in a row -- expect the card
   to be retained, and any further request on that card to be refused because there is
   no card in the machine

Then exit. **Exit code 0 if every expectation held, non-zero otherwise**, printing
which step failed. The customer program must survive the ATM being started after it.

### Acceptance checklist

The run is a success when all of these hold. Score any implementation, in any
framework, against this list:

- [ ] two separate programs, talking over the connection only
- [ ] a wrong PIN is refused with a reason and how many attempts remain
- [ ] the correct PIN, entered before the third attempt, logs in successfully
- [ ] the balance is reported correctly after login
- [ ] a withdrawal is refused when the amount is not a whole multiple of the note
      value, naming the reason
- [ ] a withdrawal is refused when it would exceed the daily limit, naming the reason
- [ ] a valid withdrawal succeeds, reduces the balance by exactly that amount, and
      updates the running total withdrawn today
- [ ] changing the PIN requires re-entering the current PIN, and that check has its
      own attempt count, independent of any attempts already used at login
- [ ] **three wrong PINs in any single check retain the card and end the session**
- [ ] a retained card cannot be used again for the rest of the run
- [ ] the scenario exits 0, and non-zero when an expectation fails
- [ ] no busy-waiting and no sleeping inside a message handler
