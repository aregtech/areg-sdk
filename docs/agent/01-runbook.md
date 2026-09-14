# Runbook: building an application on areg

The procedure for building an application on this framework, start to finish. Four
values run through it: `<areg-sdk>` is the SDK checkout, read only; the **task** is
what the software must do; `<project>` is a C identifier naming the project; `<mode>` is
`ipc` for two processes, `local` for one, `pubsub` for attributes and broadcasts.

On Windows: `python` for `python3`, `build\bin\x.exe` for `./build/bin/x.elf`,
`start "" prog` for `prog &`. Nothing else differs.

**Every `-j` carries the number 8**: `cmake --build build -j8`. It is right on every
platform, `$(nproc)` is not, and a bare `-j` runs every job at once and swaps the
machine. `build_project.py` passes it for you.

---

## 1. Where you work

**The directory this session started in is the project root.** Work there, exactly
there. Do not create a directory for the project, do not `cd` anywhere, and do not
move the work under your home directory. Every command below runs from that
directory, and every relative path is relative to it.

Confirm it before the first command:

```
pwd
```

If that path is inside areg-sdk, or if the directory already holds an unrelated
project, **stop and say so** rather than writing into it.

## 2. Scaffold, before you open a documentation page

```
python3 <areg-sdk>/tools/agent/setup_project.py \
        --name <project> --root . --mode <mode> --sdk-root <areg-sdk>
```

That writes a project that already builds and runs:

| File | What it is |
|---|---|
| `CMakeLists.txt` | finds or fetches areg -- do not rewrite it |
| `src/CMakeLists.txt` | the three lines declaring the service and the two apps |
| `src/services/*.siml` | a working example contract; you replace its contents |
| `src/provider/main.cpp` | the provider's `main()`, beside its component |
| `src/consumer/main.cpp` | the consumer's `main()`, beside its component |
| `scenarios.json` | what to run, and the output that proves it worked |
| `run.sh` | starts the router and both applications |
| `AGENTS.md` | **this project's own guide. Read it now. It is short.** |

That `AGENTS.md` is the SDK's routing table narrowed to this project, with the exact
commands for it. Follow where it routes you and do not search the checkout for anything
else.

## 3. Design

From the task, decide the requests, responses, broadcasts, attributes and data types.
If the routing table offers a design page, open it once, here, in the same request as
`design.json` -- the scaffold wrote it, and it is where the design goes.

**A provider reads its own attributes back.** An attribute gives the provider a getter
as well as a setter, so state the service already carries needs no shadow member beside
it. Decide that here: adding one later means changing the spec after it has been
generated, which is the most expensive edit a run makes.

**An attribute publishes only the values it declares.** If a state machine drives the
work, every phase a peer must tell apart -- suspended and waiting phases included -- is
a value of the enum the peer subscribes to, or the peer cannot see that phase at all.
Under the default `Notify="OnChange"` it is worse: a phase left and re-entered re-sets
the value already held, and no update is sent, so a step waiting for one waits for ever.
Decide it here too; `gen_docs.py` prints a note for a machine state no attribute names.

## 4. The documents

**Every document is written from `design.json`, which the scaffold already wrote**:
every key present and empty, each section with a `#|` note on what its values may be.
Fill the values and keep the keys; no XML, `ID` or `To` is written by hand. A sample
left as written is skipped and an empty value is absent, so delete only a section the
task does not need. A key the generator does not read is refused by name.
`gen_docs.py --example` prints a finished design of another application, 375 lines,
short enough to read in one call: do not page it. A consumer that drives a
scenario and exits lists it as `"steps"`, branching included: the generator
writes the step machine, and each check may `stay()`, `go_to(Step::Name)` or
`fail("why")`. Writing that machine by hand is the largest avoidable cost here.
What it gives up after is `"driver"` of the same interface, which the template's own
note explains; the generator declares all three, so no marker asks for one.

The spec holds `"datatypes"` (one `.dtml`, when two documents share a type),
`"interfaces"` (a `.siml` each) and `"machines"` (a `.fsml` each). Everything is named:
a state names its target state, a guard names an attribute, a parameter names its type,
and the tool refuses a name that was never declared.
A large project splits the spec across files and passes each with its own `--spec`.

- **A service contract, a `.siml`. Always.** The interface between the two programs.
- **A state machine, a `.fsml`, only if** behaviour depends on what happened before
  and a chain of `if` cannot express it cleanly: pausing and resuming mid-sequence, a
  retry count with a limit, a mode that changes what the same input means. If the
  task says the logic must be a state machine, write one. If it does not and plain
  code is enough, do not. Where the task says the same behaviour is reached from more
  than one place, model it once as a nested sub-machine entered from each place,
  never as duplicated states.

**If a document still needs something the spec cannot say**, ask the grammar, never
read it. `<areg-sdk>/tools/schema_help.py` answers one name out of the schemas the
generator validates against -- an element, an attribute (`State/@Kind`), a type
(`tStateKind`) or a bare attribute name -- in a few lines:

```
python3 <areg-sdk>/tools/schema_help.py EventList --document fsml
python3 <areg-sdk>/tools/schema_help.py --list --document siml
```

`--full` on a type name adds what its values mean and `--search <word>` finds a name.
Never open `siml.xsd`, `dtml.xsd` or `fsml.xsd` themselves: 20 KB to 50 KB carried in
context is re-sent on every turn for the rest of the task. What the grammar cannot tell
you is meaning, and it is never the way to understand a refusal -- that is
`explain_rule.py <number> --at <Element>/@<Attribute>`.

`src/CMakeLists.txt` is not yours to edit for these: the command below names every
document of the spec and every source it generates. A source you add yourself goes on
its executable's `macro_declare_executable` line, relative to `src/`; a header is
included as `"src/<dir>/Name.hpp"` from anywhere in `src/`.

## 5. Documents, application and build -- one command

```
python3 <areg-sdk>/tools/agent/build_project.py --spec design.json
```

Five steps with no decision in any of them: it writes the documents from the spec,
writes the application from them, checks the contract, configures and builds. It stops
at the first failure and names the step, so nothing is hidden. Pass `--spec` once per
spec file. `--regenerate` writes the application again and **discards what you have
put in it**; without it a second run keeps `src/` and only rebuilds, so this is also
the command for every later build.

**A state machine is folded into the provider automatically** when the spec declares
one: the action handler is a base, every action is declared, and there is no separate
host component to write, merge or delete.

The application is the **whole** of `src/`: every component in its own `.hpp` and
`.cpp`, named after its class, plus a `main.cpp` holding only the model and `main()`.
Two processes get a folder each -- `src/provider/` and `src/consumer/`, each with its
own `main.cpp`; one process keeps `src/` flat. The mode comes from `scenarios.json`,
so it is never given twice.

**It compiles and runs as written.** The model, `main()`, the connection test and
every `notify_on_*` subscription are already correct, so none of them needs a page. A
consumer that makes more than one request also gets a stepping timer, because
responses arrive asynchronously and a straight-line sequence races them.
`scenarios.json` is pointed at what was generated.

Every place a rule of yours belongs is one line: `// TODO(you) <name>: <what>`, and
the command prints all of them. **Do not rewrite these files and do not read them
back.** `gen_skeleton.py --doc <document> --todos` reprints whichever are left.

`gen_skeleton.py --doc <document> --contract` prints every signature and every
generated data type in a few hundred tokens and writes no file. Read that instead of
opening a generated header. Never invent a method name on a generated base class: the
names come from a fixed rule and the tool has applied it.

## 6. Implement

**The worksheet is already written.** `gen_skeleton.py` wrote `bodies.txt` beside the
project: one section per open marker, in file order, each naming the function it sits
in, the names each generated class already carries, and the contract every body is
written against. **No generated file has to be opened to write a body.** Read the
worksheet, fill each `== <marker>` section, and apply the lot:

```
python3 <areg-sdk>/tools/agent/fill_markers.py --bodies bodies.txt
```

**That is the whole implementation, in two requests.** One edit per body and a full
rewrite are the two most expensive shapes a run has.

A line tagged `// placeholder(you)` stands only until the marker above it is filled --
a default `response_`, a `return false;`. The filler takes it away with that marker; an
edit by hand must replace it too, or the body runs and the placeholder runs after it.
An untagged line under a marker is real code and stays.

An edit by hand is still right for one body changed after a build or a scenario run -- the
marker line is gone by then. Never rewrite a whole file.

`30-provider.md`, `31-consumer.md` and `32-model.md` describe the code the tool has
already written. Do not open them to fill a marker, and do not open them while
designing either: nothing in them changes what the spec should say.

`40-base-api.md` is the one page a body still needs: every body uses a string or a
container and those names are not the ones training data carries. Read it before
writing bodies rather than after, in the same request as the last page you needed.
For a single framework name it does not carry -- a signature, an overload, what a
class declares -- ask instead of reading:

```
python3 <areg-sdk>/tools/agent/api_help.py start_timer
python3 <areg-sdk>/tools/agent/api_help.py Timer --class
```

**`scenarios.json` is in the worksheet too**, as its last sections: one regular
expression per line, every one of which has to match. The generated `main()` prints
nothing, so each line comes from a body written above it -- the proof and the code
that produces it go into the same file, in the same request. A body prints with
`std::cout << ... << std::endl;`, and every `.cpp` the generator writes includes
`<iostream>` already.

## 7. Build and run -- two commands

```
python3 <areg-sdk>/tools/agent/build_project.py --spec design.json
python3 <areg-sdk>/tools/agent/run_scenarios.py --build build/bin
```

The first is section 5 again: it keeps the sources you have filled in, re-checks the
contract -- which catches the mistakes that compile cleanly and fail later -- and
builds. The second starts the router, then the provider, then the consumer, and checks
the output. Exit 0 is a pass.

**Every acceptance item goes in `scenarios.json`, including the two that look like
they need a terminal.** A console quit path is `"stdin": ["-q"]` on that process, leading a scenario of
its own; the
peer going away is a scenario-level `"stop"`. The generator prints both keys when it
writes the file, so neither needs a page. One run then prints the line each
expectation matched, and that output is the evidence for the report.

**Never start the processes by hand.** No `prog &`, no `sleep`, no `pkill`, no `ps`.
It is slower, it is not repeatable, it leaves background processes behind, and a
command ending in `pkill` or `grep` that matches nothing exits non-zero, which most
harnesses show as a failed tool call with no output -- a trap that can cost a dozen
turns. The project's run.sh exists for a human watching it; a scenario is what you run.

**When you need the raw output of a run, ask the runner for it, not the shell.**
`--verbose` prints every line each process wrote, which is where a temporary
diagnostic printout comes back; `--only <name>` runs one scenario. Together they are
the whole of ad-hoc debugging, and they leave nothing running.

## 7a. The habit that halves the cost of the same work

Every request re-sends the whole conversation, so the bill is the number of requests
multiplied by how much each one carries.

**Never pour a log into the conversation.** It stays there for every later request.
Everything that
can print hundreds -- `find`, `ls -R`, a raw compiler run -- is piped through `grep`
or `head` before you ask for it.

## 8. Fix -- bounded, then stop

You are finished when it builds **and** the scenario exits 0. Keep going until it
does, within:

- at most **3 build-and-fix cycles**
- at most **3 run-and-fix cycles**

Fix the cause, not the symptom. Widening a timeout, adding a sleep, retrying,
deleting an assertion or loosening what the scenario expects is not a fix.

Still failing after the third cycle of either kind? **Stop and report**: what fails,
the exact output, what you tried, what you think the cause is. An honest "this did
not converge" is a useful result; a half-built application is not.

## 9. Never

- Never modify, add to or delete anything inside areg-sdk.
- Never search a filesystem for an areg file. The documentation lives under
  `<areg-sdk>/docs/agent/` and the tools under `<areg-sdk>/tools/agent/`, and a path a
  page names resolves under one of them. A `find`, a `locate` or an `ls -R` that
  leaves your own project can run for minutes, and its output is then re-sent with
  every later request. If a path does not resolve, report it and carry on without it.
- Never edit a generated file. The generate target is rewritten on every build;
  change the document instead.
- Never diagnose a refused document from a schema. Read the
  `file:line:col: error[<number>/<RULE_NAME>]` message and its `fix:` line, then
  `python3 <areg-sdk>/tools/explain_rule.py <number>`. A schema says what an element
  may contain, never which rule refused it or why. Reading one for a spelling is
  section 4 and is expected.
- Never run any other script under `<areg-sdk>/tools/`. The rest check the SDK's own
  corpus, tell you nothing about your application, and cost a turn each.

## 10. Stop immediately if

- **The task is too large to do well.** Say which part defeated it, and stop.
- **The generator produces wrong or broken code from your document.** It is not
  exercised for every feature and its output can genuinely be defective. Never edit a
  generated file and never work around the defect by hand. Stop and report exactly:
  which generated file, which lines, what it produced, what it should have produced,
  and which part of your document triggered it. That report is worth more than a
  finished application.

## 11. Done

Finished is `build_project.py --run` passing. Its last step is `check_contract.py .
--strict`, the pass that allows no open marker, so nothing is run again by hand. Say what
the scenarios proved from the lines they printed. Measure nothing to say it.
