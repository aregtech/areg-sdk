# Prompt: a multifunction printer with print, scan and copy

A task prompt for an AI agent. It is written the way a developer hands work to a
colleague: what the software must do, not how to build it. It names no framework, so
the same requirements can be handed to gRPC, ZeroMQ, DDS, areg, or anything else.

**To run it.** This file is only the task and its acceptance checklist. To build it on
**areg**, copy `examples/ai-prompts/ai-prompt-template-text.txt`, set its `task` line to
`examples/ai-prompts/printer-scanner.md`, and paste that file as the prompt -- four lines are
all you edit. For another framework, hand this file to an agent together with your own
delivery instructions.

This is the shape of any controller built from independent units that must be
scheduled one at a time and whose failures all have to be handled the same way,
however different the units are underneath.

---

## The task

Build a **multifunction device** and a **simulated operator**, as two separate programs
that talk to each other. Neither may reach into the other's memory: everything passes
over the connection.

### The device

The device has one print engine and one scan engine, and can do three things: print,
scan, and copy. **Only one job runs at a time**, whichever kind it is: a job requested
while another is running is refused, naming the reason.

**Printing.** Given a page count, a print job goes through visible stages -- warming
up, feeding paper, then printing each page in turn -- and the operator program is told
of each stage and each page as it happens. It finishes having printed exactly the
requested number of pages.

**Scanning.** Given a page count, a scan job goes through its own stages -- warming
up, then scanning each page in turn -- reported the same way, and finishes having
scanned exactly the requested number of pages.

**Copying -- the important requirement.** A copy job of a given page count is a
**scan of that many pages followed by a print of that same number of pages**, and this
must be visible in the stages reported: every scan stage for all requested pages, in
order, then every print stage for all requested pages, in order. No other job may be
accepted in between, and copying is not a separate, shorter path to the same result --
it goes through both engines, in sequence, for the same reason a real device only has
one of each.

**A jam -- the other important requirement.** Either engine can jam partway through a
job, including partway through the print half or the scan half of a copy. However
different the two engines are, a jam is reported to the operator program **the same
way**: which engine jammed, and how many pages it had completed before the jam. Once
jammed, the device accepts no new job until the fault is cleared. The operator program
can clear it; clearing returns the device to idle and does not resume the interrupted
job -- a job that jammed has to be requested again from the start.

To keep the run repeatable, jams are **not random**. The operator program can arm
the **next** print or scan job to jam after a specified number of completed pages.

**Structure -- the reason for this task.** Printing and scanning are each one
piece of behaviour, written once and used from more than one place: a print job
and the print half of a copy are the same behaviour, and so are a scan job and
the scan half of a copy. A copy runs the scan behaviour and then the print
behaviour as a single job.

**Stopping and timing out.** The device runs until it is stopped: it accepts `-q`
or `--quit` typed at its console and exits cleanly. Neither program may wait
forever -- if something it is waiting for has not arrived within 20 seconds, it
reports that and exits. If one side goes away mid-scenario, the other reports the
loss and exits non-zero.

### The simulated operator

A second program that connects to the device and runs this scenario, reporting each
step to the console so a person can read what happened:

1. request a scan of **3** pages with nothing else running -- expect it accepted,
   each stage and page reported, finishing with the right page count
2. while that scan is still running, request a print of **2** pages -- expect a
   refusal naming the reason
3. once the scan has finished, request a print of **4** pages -- expect it
   accepted, each stage and page reported, finishing with the right page count
4. request a copy of **2** pages -- expect the scan stages for every page, then
   the print stages for every page, in that order, and the finished page count
   to match what was asked for
5. arm the next print job to jam after **2** completed pages, then request a
   print of **5** pages -- expect a fault naming the printer and that it
   completed **2** pages first, and any further job refused while the fault
   stands
6. clear the fault -- expect the device to return to idle
7. arm the next scan job to jam after **1** completed page, then request a scan
   of **3** pages -- expect the fault reported in the same shape as step 5,
   naming the scanner this time and reporting **1** completed page
8. clear the fault and run one more small print job of **1** page -- expect it
   to succeed, proving the device fully recovered

Then exit. **Exit code 0 if every expectation held, non-zero otherwise**, printing
which step failed. The operator program must survive the device being started after
it.

### Acceptance checklist

The run is a success when all of these hold. Score any implementation, in any
framework, against this list:

- [ ] two separate programs, talking over the connection only
- [ ] a scan job reports each stage and page as it happens, and finishes with the
      right page count
- [ ] a print job reports each stage and page as it happens, and finishes with the
      right page count
- [ ] a second job is refused while one is already running, naming the reason
- [ ] **a copy job scans every requested page, then prints every requested page, in
      that order, and only that order**
- [ ] a copy job's finished page count matches what was requested
- [ ] a jam during a print job is reported, naming the printer and how many pages
      completed first
- [ ] a jam during a scan job is reported the same way, naming the scanner
- [ ] no new job is accepted while a fault stands
- [ ] clearing the fault returns the device to idle and it accepts jobs again
- [ ] jams are injected deterministically for the next named job, not randomly
- [ ] the print behaviour and the scan behaviour are each written once and reused
      by the copy job, not duplicated
- [ ] the device accepts `-q` / `--quit` at its console and exits cleanly
- [ ] neither program waits more than 20 seconds for something that never arrives
- [ ] if one side goes away mid-scenario, the other reports it and exits non-zero
- [ ] the scenario exits 0, and non-zero when an expectation fails
- [ ] no busy-waiting and no sleeping inside a message handler
