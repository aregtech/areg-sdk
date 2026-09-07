# Runbook: building an application on areg

The procedure an agent follows for any task on this framework. Your prompt gives four
settings -- **areg-sdk**, **task**, **project**, **mode**. Everything here uses them.
Nothing in this file needs editing, ever.

On Windows: `python` for `python3`, `build\bin\x.exe` for `./build/bin/x.elf`,
`start "" prog` for `prog &`. Nothing else differs.

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

## 2. Scaffold, before you read anything

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

That `AGENTS.md` says where the framework documentation is. Follow where it routes
you and do not search the checkout for anything else.

## 3. Design

From the task, decide the requests, responses, broadcasts, attributes and data types.
Write nothing yet. If the routing table offers a design page, open it once, here.

## 4. The documents -- you write these by hand

Under `src/services/`, replace the scaffolded document with your own:

- **A service contract, a `.siml`. Always.** The interface between the two programs.
- **A state machine, a `.fsml`, only if** behaviour depends on what happened before
  and a chain of `if` cannot express it cleanly: pausing and resuming mid-sequence, a
  retry count with a limit, a mode that changes what the same input means. If the
  task says the logic must be a state machine, write one. If it does not and plain
  code is enough, do not. Where the task says the same behaviour is reached from more
  than one place, model it once as a nested sub-machine entered from each place,
  never as duplicated states.

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

## 5. Skeletons -- never hand-write a class declaration

```
python3 <areg-sdk>/tools/agent/gen_skeleton.py \
        --doc src/services/YourService.siml --out src --force
```

Writes a **separate provider class and consumer class** -- four files, every method
name already correct:

```
src/YourServiceProvider.hpp   src/YourServiceProvider.cpp
src/YourServiceConsumer.hpp   src/YourServiceConsumer.cpp
```

Never invent a method name on a generated base class: the names come from a fixed
rule and the skeleton has applied it. Run this again with `--force` whenever the
document changes.

## 6. Implement

Fill the skeleton bodies with the task's logic. Wire the components into the model in
`provider.cpp` and `consumer.cpp`. Update `scenarios.json` so it names your
executables and the output lines that prove each requirement.

## 7. Check, build, run -- this order, once each

```
python3 <areg-sdk>/tools/agent/check_contract.py . --strict
cmake -B build
cmake --build build -j
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

## 7a. Two habits that halve the cost of the same work

Every request re-sends the whole conversation, so the bill is the number of requests
multiplied by how much each one carries. Both of these are free to follow and neither
changes what gets built.

**Put independent calls in one request.** Reading eight pages is one request, not
eight. A header and its source are written together. So are the two `main()` files
and `scenarios.json`. Only split where the next thing genuinely depends on the result
of the last -- a build, a check, a scenario run.

**Never pour a build log into the conversation.** It stays there for every later
request. `cmake --build build -j 2>&1 | tail -30` is enough to see success; on a
failure ask for the errors, not the transcript:

```
cmake --build build -j 2>&1 | grep -E "error|Error" | head -20
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
- Never edit a generated file. The generate target is rewritten on every build;
  change the document instead.
- Never commit anything, and never run a git command.
- Never open a schema (`.xsd`). On a refused document read the
  `file:line:col: error[<number>/<RULE_NAME>]` message and its `fix:` line, then
  `python3 <areg-sdk>/tools/explain_rule.py <number>`.
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
