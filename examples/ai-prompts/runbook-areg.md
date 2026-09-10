# Runbook: building an application on areg

The procedure an agent follows for any task on this framework. Your prompt gives four
settings -- **areg-sdk**, **task**, **project**, **mode**. Everything here uses them.
Nothing in this file needs editing, ever.

On Windows: `python` for `python3`, `build\bin\x.exe` for `./build/bin/x.elf`,
`start "" prog` for `prog &`, `-j%NUMBER_OF_PROCESSORS%` for `-j$(nproc)`. Nothing
else differs.

**Always give `-j` a number.** `cmake --build build -j` with no number lets make run
every job at once; on a large tree that swaps, and a build that should take a minute
takes ten or is killed.

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
| `src/provider.cpp` | the provider's `main()` |
| `src/consumer.cpp` | the consumer's `main()` |
| `scenarios.json` | what to run, and the output that proves it worked |
| `run.sh` | starts the router and both applications |
| `AGENTS.md` | **this project's own guide. Read it now. It is short.** |

That `AGENTS.md` is the SDK's routing table narrowed to this project, with the exact
commands for it. Follow where it routes you and do not search the checkout for anything
else.

## 3. Design

From the task, decide the requests, responses, broadcasts, attributes and data types.
Write nothing yet. If the routing table offers a design page, open it once, here.

## 4. The documents

Under `src/services/`, replace the scaffolded documents with your own. **Every
document of the project is written from one JSON description**, so no XML, no `ID`,
no `To` and no cross-document type check is ever done by hand:

```
python3 <areg-sdk>/tools/agent/gen_docs.py --example > design.json
python3 <areg-sdk>/tools/agent/gen_docs.py --spec design.json --outdir src/services
```

The spec holds `"datatypes"` (one `.dtml`, when two documents share a type),
`"interfaces"` (a `.siml` each) and `"machines"` (a `.fsml` each). Everything is named:
a state names its target state, a guard names an attribute, a parameter names its type,
and the tool refuses a name that was never declared, in the message that says what was.
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

Then point `src/CMakeLists.txt` at them:

```cmake
addServiceInterface(gen_<project> src/services/YourService.siml)
macro_declare_executable(<project>_provider gen_<project> provider.cpp ...)
macro_declare_executable(<project>_consumer gen_<project> consumer.cpp ...)
```

**The two path kinds on those lines are different.** The document path is relative to
the **project root** (`src/services/X.siml`). The source paths are relative to **the
CMakeLists.txt naming them** (`provider.cpp`, already inside `src/`). Rename the two
executables to suit the task, and add every source file you write to them.

## 5. The application -- generated whole, not hand-written

```
python3 <areg-sdk>/tools/agent/gen_skeleton.py \
        --doc src/services/YourService.siml --app --mode ipc --force
        [--machine src/services/YourMachine.fsml]
```

**With a state machine, pass `--machine`.** The provider then owns the machine: the
action handler is a base, every action is declared, and there is no separate host
component to write, merge or delete.

Writes the **whole application** into `src/`: the components, every subscription, the
model and `main()`. `--mode ipc` writes `src/provider.cpp` and `src/consumer.cpp`;
`--mode local` writes one `src/main.cpp`. Match the mode `setup_project.py` scaffolded.

**It compiles and runs as written. Build it and run it before you change anything.**
The model, `main()`, the connection test and every `notify_on_*` subscription are
already correct, so none of them needs a page. Every place a rule of yours belongs is
marked `TODO(you)`.

**Do not rewrite these files, and do not read them back**: the tool has already
printed what they contain. Section 6 fills the markers in.

`--contract` in place of `--app` prints every signature and every generated data type
in a few hundred tokens and writes no file. Read that instead of opening a generated
header.

Never invent a method name on a generated base class: the names come from a fixed rule
and the tool has applied it. Run it again with `--force` whenever the document changes.

**A generated application file carries its components, the model and `main()` together**
-- that is the shape every recipe uses. A class you add by hand gets its own `.hpp` and
`.cpp`, named after it, and no class is ever defined inside the body of `main()`.

## 6. Implement

Fill the `TODO(you)` markers with the task's logic, using `Edit`. The model, `main()`
and the subscriptions are already wired and are not written again.

`40-base-api.md` is the one page a body still needs: every body uses a string or a
container and those names are not the ones training data carries. Read it before
writing bodies rather than after, in the same request as the last page you needed.

Update `scenarios.json` so it names your executables and the output lines that prove
each requirement.

## 7. Check, build, run -- this order, once each

```
python3 <areg-sdk>/tools/agent/check_contract.py . --strict
cmake -B build
cmake --build build -j$(nproc)
python3 <areg-sdk>/tools/agent/run_scenarios.py --build build/bin
```

The first catches mistakes that compile cleanly and fail later; run it **before** you
build. The last starts the router, then the provider, then the consumer, and checks
the output. Exit 0 is a pass.

**Every acceptance item goes in `scenarios.json`, including the two that look like
they need a terminal.** A console quit path is `"stdin": ["-q"]` on that process; the
peer going away is a scenario-level `"stop"`. Both are in
`<areg-sdk>/docs/agent/50-running.md`. One run then prints the line each expectation
matched, and that output is the evidence for the report.

**Never start the processes by hand.** No `prog &`, no `sleep`, no `pkill`, no `ps`.
It is slower, it is not repeatable, it leaves background processes behind, and a
command ending in `pkill` or `grep` that matches nothing exits non-zero, which most
harnesses show as a failed tool call with no output -- a trap that can cost a dozen
turns. `./run.sh` exists for a human watching it; a scenario is what you run.

**When you need the raw output of a run, ask the runner for it, not the shell.**
Everything hand-starting a process is reached for is already a flag:

```
python3 <areg-sdk>/tools/agent/run_scenarios.py --build build/bin --verbose
python3 <areg-sdk>/tools/agent/run_scenarios.py --build build/bin --only <name>
```

`--verbose` prints every line each process wrote, which is where a temporary
diagnostic printout comes back. `--only` runs one scenario. Together they are the
whole of ad-hoc debugging, and they leave nothing running.

## 7a. Two habits that halve the cost of the same work

Every request re-sends the whole conversation, so the bill is the number of requests
multiplied by how much each one carries. Both of these are free to follow and neither
changes what gets built.

**Put independent calls in one request.** Reading eight pages is one request, not
eight. A header and its source are written together. So are the two `main()` files
and `scenarios.json`. Only split where the next thing genuinely depends on the result
of the last -- a build, a check, a scenario run.

**Never pour a build log into the conversation.** It stays there for every later
request. `cmake --build build -j$(nproc) 2>&1 | tail -30` is enough to see success; on a
failure ask for the errors, not the transcript:

```
cmake --build build -j$(nproc) 2>&1 | grep -E "error|Error" | head -20
```

The same goes for `find`, `ls -R` and anything else that can print hundreds of lines.

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
- Never search a filesystem for an areg file. Your prompt names the checkout, the
  documentation lives under `<areg-sdk>/docs/agent/` and the tools under
  `<areg-sdk>/tools/agent/`, and a path a page names resolves under one of them. A
  `find`, a `locate` or an `ls -R` that leaves your own project can run for minutes,
  and its output is then re-sent with every later request. If a path does not resolve,
  report it and carry on without it.
- Never open `<areg-sdk>/CLAUDE.md` or anything under `<areg-sdk>/.claude/`. They are
  for developing the framework itself and say nothing about building on it.
- Never edit a generated file. The generate target is rewritten on every build;
  change the document instead.
- Never commit anything, and never run a git command.
- Never diagnose a refused document from a schema. Read the
  `file:line:col: error[<number>/<RULE_NAME>]` message and its `fix:` line, then
  `python3 <areg-sdk>/tools/explain_rule.py <number>`. A schema says what an element
  may contain, never which rule refused it or why. Reading one for a spelling is
  section 4 and is expected.
- Never run any other script under `<areg-sdk>/tools/`. The rest check the SDK's own
  corpus, tell you nothing about your application, and cost a turn each.
- Never write a ReadMe.

## 10. Stop immediately if

- **The task is too large to do well.** Say which part defeated it, and stop.
- **The generator produces wrong or broken code from your document.** It is not
  exercised for every feature and its output can genuinely be defective. Never edit a
  generated file and never work around the defect by hand. Stop and report exactly:
  which generated file, which lines, what it produced, what it should have produced,
  and which part of your document triggered it. That report is worth more than a
  finished application.

## 11. The report -- write it, do not measure it

**Report only what a command cannot recover afterwards.** Byte sizes, line counts and
file lists are the operator's to compute from the tree you leave behind; measuring
them yourself costs turns and tells nobody anything. **Do not guess tokens or wall
time** either: most harnesses do not show an agent its own usage, and an invented
number makes every comparison worthless.

Ten lines, from what you already know:

- build-and-fix cycles, and run-and-fix cycles
- acceptance checklist items passing, out of how many, and which failed -- taken from
  the scenario output, not from a second run
- any file you opened that the documentation did not route you to, and why
- what you had to guess, what the documentation did not answer, and which page you
  wish had said something it did not
