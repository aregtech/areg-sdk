# Prompt: a camera, with a viewer that checks every frame

A task for an AI agent: what the software must do, not how to build it.

This is the shape of any device that streams large blocks of data at a fixed rate from
a thread of its own, must notice when that thread gets stuck and replace it without
losing the stream, and has clients that come and go while it runs.

---

## The task

Build a **camera** and a **viewer**, as two separate programs that talk to each other.
Neither may reach into the other's memory: everything passes over the connection.

### The camera

**Frames.** A frame carries its **sequence number**, **65,536 bytes** of data, and a
**checksum**. Byte `i` of frame `n` is `(n + i) mod 256`. The checksum is the sum of all
65,536 bytes, modulo 2^32. Sequence numbers start at **1** and count up for the life of
the camera: no number is ever produced twice, and none is skipped. Every frame is
delivered to every client that watches frames, whole and unchanged.

**Streaming.** A **start** begins streaming: a **capture thread of the camera's own**
produces one frame every **50 ms**. A **stop** ends it: the capture thread ends and is
waited for, and no frame is delivered after the stop is answered. A start while
streaming is refused naming that it is already streaming; a stop when not streaming is
refused naming the reason. All timing is kept by the camera; no client drives it.

**A burst.** When not streaming, a **burst** of **1 to 100** frames produces that many
frames as fast as the camera can, then the camera is idle again. A burst while streaming
is refused, naming the reason; a burst outside 1-100 is refused, naming the range.

**A stuck capture thread -- the important requirement.** A client can tell the camera
to **hang** its capture thread: the thread stops producing and stays stuck until it is
told to end. The camera must notice that no frame has been produced for **200 ms**
while streaming, announce that capture **stalled**, end the stuck thread and wait for it
(it is not abandoned), start a new capture thread, and announce that capture
**restarted**, naming the sequence number it continues from. Streaming must resume
within **500 ms** of the hang. The sequence continues with the next number: nothing is
skipped and nothing is repeated.

**What the camera publishes.** What it is doing (idle, streaming, bursting, stalled)
and the number of frames produced so far are published values: a client that starts
watching late receives the current values at once, and every client receives every
change.

**Stopping and timing out.** The camera runs until it is stopped: it accepts `-q` or
`--quit` typed at its console and exits cleanly, ending its capture thread first.
Neither program may wait forever -- if something it is waiting for has not arrived
within 20 seconds, it reports that and exits. If one side goes away mid-scenario, the
other reports the loss and exits non-zero.

### The viewer

A second program, made of **two independent parts** that run on their own threads and
talk to each other only by messages:

- **the checker** watches the camera's frames and published values. It checks every
  frame it receives: the data, the checksum, and that its sequence number is the one
  after the previous frame it received. It reports to the controller how many frames it
  received, the first and last sequence numbers, and every frame that failed a check.
- **the controller** runs the scenario below and decides whether it passed.

**The checker can be stopped and started again while the viewer runs**, on the
controller's request, without the viewer restarting. While stopped it receives nothing.
Started again, it watches anew: it receives the camera's current values at once, and its
first frame is simply whatever comes next.

The controller runs this scenario, reporting each step to the console so a person can
read what happened:

1. stop the camera -- expect a refusal naming that it is not streaming
2. start the camera -- expect the checker to receive **20** frames, numbered 1 to 20,
   every one intact, and the 20 frames to take at least **950 ms**
3. start the camera again -- expect a refusal naming that it is already streaming
4. stop the checker; wait **300 ms**; start it again -- expect it to receive the frames
   produced so far at once (at least **26**), then **10** more frames, numbered on from
   one another, every one intact
5. hang the capture thread -- expect capture stalled, then restarted, streaming again
   within **500 ms** of the hang, and **10** more frames numbered on from the last one
   before the hang, every one intact
6. stop the camera -- expect it accepted, and no frame in the **200 ms** after the answer
7. ask for a burst of **100** frames -- expect **100** frames, numbered on from the last
   one before, every one intact, all received within **1 second** of the answer
8. ask for a burst of **101** frames -- expect a refusal naming the range

Once a step's expectation has held, the controller prints its line below, with this
exact wording, alone or at the end of a line, nothing after it:

```lines
step 1: refused, not streaming
step 2: 20 frames intact, 1 to 20
step 3: refused, already streaming
step 4: checker restarted, frame count received at once, 10 frames intact
step 5: capture stalled and restarted, 10 frames intact, none skipped
step 6: stopped, no frame after the stop
step 7: burst of 100 frames intact within 1 second
step 8: refused, burst out of range
```

Then exit. **Exit code 0 if every expectation held, non-zero otherwise**, printing
which step failed. The viewer must survive the camera being started after it.

### Proving it

Two of the requirements below cannot be shown by a run in which everything works, so
they need a second run of their own:

1. **The normal run** -- both programs, the sequence above end to end, the viewer exits
   0.
2. **A run where the other side is taken away.** Start both, let the sequence reach the
   middle, then stop the camera abruptly. The viewer must say that it lost the other side
   and exit non-zero. It must not hang, and it must not exit 0.

**An acceptance item counts as passing only when the output of one of those runs shows
it.** An item you believe you implemented but never observed is reported as not
passing: naming the ones you could not prove is worth more than a full score.

### Acceptance checklist

The run is a success when all of these hold. Score any implementation, in any
framework, against this list:

- [ ] two separate programs, talking over the connection only
- [ ] every frame carries 65,536 bytes and arrives whole, its data and checksum correct
- [ ] sequence numbers start at 1 and are never repeated or skipped, across stops,
      bursts and a restarted capture thread
- [ ] frames are produced on a thread of the camera's own, every 50 ms while streaming
- [ ] **a stuck capture thread is noticed within 200 ms, ended and waited for, replaced,
      and streaming resumes within 500 ms, with stalled and restarted announced**
- [ ] a burst of 100 frames arrives intact within 1 second
- [ ] a start while streaming, a stop when idle, a burst while streaming and a burst out
      of range are each refused, naming the reason
- [ ] no frame is delivered after a stop is answered
- [ ] **the viewer's checker is stopped and started again while the viewer runs, and
      receives the camera's current values at once when it starts**
- [ ] the viewer's two parts talk to each other only by messages
- [ ] what the camera is doing and the frame count are published, and a client that
      starts watching late receives the current values at once
- [ ] the camera accepts `-q` / `--quit` at its console and exits cleanly
- [ ] neither program waits more than 20 seconds for something that never arrives
- [ ] if one side goes away mid-scenario, the other reports it and exits non-zero
- [ ] the scenario exits 0, and non-zero when an expectation fails
- [ ] no busy-waiting and no sleeping inside a message handler

---

## What to deliver

**Two programs**, each with its own entry point, in their own subdirectories of the
project. Nothing outside the project directory, nothing added to the framework's own
build, and no IDE or editor project files.

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
Say how the camera noticed and replaced the stuck thread, how the frames crossed the
connection, and how the checker was stopped and started inside a running viewer. Say
plainly wherever you had to search for an answer instead of being routed to one -- that
is the finding this exercise is really after, and it is worth more than the table.
