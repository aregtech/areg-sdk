# Working with the AREG SDK

AREG is a framework for service-oriented applications. Describe a service once in a
`.siml` document; the generator writes the communication code and you implement only
the service logic. The same code runs in one thread, many threads, many processes or
many machines: what changes is where a component is registered, not what it does.

**Fast path.** `tools/agent/build_project.py --spec` writes a whole project -- the
documents, the components, the model and `main()`; section 5 has the chain. What no
tool writes is one page, `docs/agent/00-cheatsheet.md`. Go there now, skip
`CODEBASE.md` and the rest of this file, and return only for section 6. `areg::String`
and the containers are not on it: read `docs/agent/40-base-api.md` **before the first
line of C++**, not after the first error.

---

## 1. Scope

This documentation set has one purpose: **building an application on top of areg**.

Your application lives **outside** this repository and may use any coding style; it
owes the contract in section 6 and nothing more. Changing the framework itself is a
different job: start from `CONTRIBUTING.md`. C++17 is the floor; the public headers
and the generated code also compile as C++20 and C++23.

### What you think you know about areg is probably out of date

areg was renamed throughout: the `NE*`, `TE*` and `IE*` prefixes are gone, types are
PascalCase, methods `snake_case`, members `mCamelCase`, and everything public lives in
namespace `areg`. **The names you remember from training data no longer exist.** Take
every name from the page section 2 sends you to. `check_contract.py` knows the
removed ones, prints their replacements, and reports any method the framework declares
nowhere -- run it before the build, not the compiler.

---

## 2. Task routing

Find your task, open that one file, and do not search the repository.

| I need to ... | Read |
|---|---|
| **Anything ordinary** | `docs/agent/00-cheatsheet.md` -- what the tools do not write |
| **Start from working code** | `docs/agent/recipes/` - copy one, do not read it. `recipes/README.md` maps them |
| **Decide what the services are** | `docs/agent/05-design.md`, before writing any file |
| Start a new project by hand | `docs/agent/10-new-project.md` |
| Define an interface; what to override, what to call | `docs/agent/20-service-interface.md` (section 3: the names) |
| Implement a provider, a consumer, or the model | nothing: `tools/agent/gen_skeleton.py --app` wrote all three, compiling and running. `docs/agent/30-provider.md`, `docs/agent/31-consumer.md` and `docs/agent/32-model.md` describe a program someone else wrote: do not open them to fill a marker or to design one |
| Watchdog, worker thread, model built at run time | copy `docs/agent/recipes/07-worker-events/`; `docs/agent/37-threads.md` |
| Send a custom event between threads | `docs/agent/23-events.md` |
| Do periodic or delayed work | `docs/agent/33-timers.md` |
| Log from code; collect several processes' logs | `docs/agent/34-logging.md` |
| Understand the seven core concepts | `CODEBASE.md` section 1 |
| Data types: C++ spelling, struct, enum, `.dtml` | `docs/agent/21-data-types.md` |
| Add a state machine | copy `docs/agent/recipes/06-state-machine/`, then `docs/agent/22-state-machine.md` |
| **Write any C++ that uses a string or a container** | `docs/agent/40-base-api.md` |
| The signature of one framework name | `tools/agent/api_help.py <name>` -- not a page, not a header |
| What a component knows about itself; the application, threads, timers, time, files | `docs/agent/42-runtime-api.md` |
| Integrate areg into an existing CMake project | `docs/wiki/02b-cmake-integrate.md` |
| Work out why it does not work | `docs/agent/51-debug.md` |
| Test the application, or a component on its own | copy `docs/agent/recipes/12-testing/`; `docs/agent/52-testing.md` |
| A `scenarios.json` key `--app` did not write (`router`, `lead`, `stop`/`after`/`signal`); start `mtrouter` by hand | `docs/agent/50-running.md` -- `run_scenarios.py` starts the pieces itself |
| Read or query a `.sqlog` log database | `docs/agent/35-sqlog.md` |
| Set the router address, ports, anything in `areg.init` | `docs/agent/36-config.md` |
| Find the example showing a given facility | `docs/agent/41-examples.md` |
| See a complete working application | `examples/03_helloservice/` -- it sleeps in a response handler; do not copy that |

`docs/wiki/` is written for people and is large: open a page there only when a row
above names it. A path that does not resolve is a defect to report, not a reason to
search.

---

## 3. What must be installed

CMake 3.20+, a Java 17+ runtime for `tools/codegen.jar` (the code generator), and a
C++17 compiler -- GCC, Clang, MSVC or MinGW. Tested on Linux and other POSIX, macOS,
Windows and Cygwin. **Python is not needed to build or run an areg application**, only
for the section 5 scripts.

Run `tools/check-env.sh`, or `tools\check-env.bat`, first: it needs no interpreter and
exits non-zero when a requirement is missing.

---

## 4. Golden path

Three commands, from nothing to a running application.

```bash
python3 <areg-sdk>/tools/agent/setup_project.py --name myapp --root ~/myapp --mode local
cd ~/myapp
python3 <areg-sdk>/tools/agent/build_project.py   # the five steps of section 5
python3 <areg-sdk>/tools/agent/run_scenarios.py   # runs it; exit 0 means it works
```

`build_project.py` is the build command every time, and its first call compiles the
framework, so **give it a command timeout of at least 15 minutes**.

**Start here, not by hand.** It copies the right recipe, renames it and writes the
project its own `AGENTS.md`. `--mode` is `local`, `ipc` (two processes) or `pubsub`;
`--sdk-root <path>` builds against a local SDK copy instead of fetching one. Without
Python, copy `docs/agent/recipes/01-local-single-process` and rename the project in
its two `CMakeLists.txt` files; `recipes/README.md` says which recipe shows what.

`addServiceInterface()` in the project's `CMakeLists.txt` is what runs the generator,
so a document is built by naming it there.

**The generator validates before it generates.** A refused document writes nothing and
exits 1, so the defect is in the document, never in the build. Every finding is
`file:line:col: error[<number>/<RULE_NAME>]` with a `fix:` line under it: read that,
then `tools/explain_rule.py <number> --at <Element>/@<Attribute>`: the rule, and the
values that attribute accepts. A spelling is `tools/schema_help.py <name>`. Never read
`tools/schema/*.xsd`.

### Every command on this path, on Windows

The pages below use POSIX commands. These three substitutions are the whole difference.

`python3 x.py` -> `python x.py` - `./build/bin/n.elf` -> `build\bin\n.exe` -
`prog --service &` -> `start "" prog` (no `--service`: on Windows it means the Service
Control Manager and fails from a command line). `-j8` is written as `-j8` everywhere:
never `$(nproc)`, which macOS does not have.

---

## 5. Tools

Run these instead of writing what they produce. All are Python 3 (`python` on
Windows), live in `tools/agent/`, and have `--help`.

| Tool | Does |
|---|---|
| `setup_project.py` | Creates a buildable project from a recipe, with its own `AGENTS.md` |
| `gen_docs.py` | Every `.dtml`, `.siml` and `.fsml` of the project, from one JSON description: no XML, no `ID`, no `To` |
| `gen_skeleton.py` | `--app`: the whole application from a `.siml`, running as written, every hole one named `TODO(you)` line; `--machine X.fsml` folds the state machine into the provider |
| `fill_markers.py` | Fills every `TODO(you)` marker from one bodies file: a `== <marker>` line, then the code replacing its line. Nothing is escaped, and a name matching no marker is refused before anything is written |
| `build_project.py` | The five mechanical steps in one: documents, application, contract, configure, build. Stops at the first failure and names the step |
| `api_help.py` | What one framework name is: its declarations and the header carrying them. Never grep a header for a signature |
| `run_scenarios.py` | Runs the application and checks its output; exit 0 is a pass. Its `scenarios.json` is `docs/agent/50-running.md` |
| `check_contract.py` | Checks sources against `docs/agent/api.json`: the section 6 mistakes that compile cleanly and fail later |

`tools/explain_rule.py` is for the refusal above; `tools/schema_help.py <name>` says
what a `.siml`, `.dtml` or `.fsml` may contain, for the thing `gen_docs.py` has no
key for. **Neither has a use before that moment**, and one reached for early costs a
turn. They, `tools/check-env.sh` and `tools/codegenerate.sh` (`.bat`) sit in
`tools/` itself.

**Nothing else under `tools/` is yours**: the rest checks this repository's own
corpus and tells you nothing about your application.

`docs/agent/api.json` states the same contract machine-readably: the naming
transforms, the connection states, and every section 6 rule with its detection hint.

---

## 6. Never

Each line closes a class of wrong code, not a style preference.
`tools/agent/check_contract.py` reports all sixteen from the sources and documents you
write, never from the generate target; run it before you build. The seven below cost a
redesign, not an edit; know them first.

- **Never edit a generated file.** The generate target is rewritten on every build;
  change the `.siml` instead.
- **Never invent a method name on a generated base class.** A fixed rule derives them
  from the document: `docs/agent/20-service-interface.md`.
- **A consumer's `REGISTER_DEPENDENCY` string must equal the provider's component
  role name, character for character.** A mismatch compiles cleanly and never connects.
- **Never call a request before the service is connected, and never treat a
  disconnect as fatal.** The first legal call is inside `service_connected()` once
  `areg::is_service_connected(status)` is true. `Disconnected`, `ConnectionLost` and
  `Failed` reconnect on their own; only `Rejected` and `Shutdown` are terminal.
- **Never block inside a request, response, broadcast or update handler.** Blocking a
  dispatcher thread stops every component in it. Use a timer or a worker thread.
- **Never override a `broadcast_*` or `on_*_update` without subscribing.** The handler
  runs only after `notify_on_...(true)`; without it the application waits for ever.
- **Never use exceptions.** AREG does not throw and does not catch. Return `bool`,
  `std::optional`, or an error code.

The other eight are one-line fixes: a `REGISTER_WORKER_THREAD` consumer name the
component does not answer to, two
components sharing a role name in one process, a header taken from a `private/`
folder, a watchdog timeout on a thread whose watchdog never starts, a response sent
after its handler returned, an operation on a nested `.fsml` `Final` state, a
hand-written source file no `macro_declare_executable` names, and a timer told apart
in `process_timer` by a `name()` compared to a literal rather than by address.

---

## 7. Definition of done

A task is finished when the application builds and its behaviour is observed, not
when the code looks correct.

```bash
python3 tools/agent/build_project.py        # builds, and checks the contract
python3 tools/agent/run_scenarios.py        # expected output, exit 0
```

`build_project.py` runs `check_contract.py`, which reports the mistakes a build
cannot catch. Silence a false positive with `// areg-check: ignore`, or the same
words in a `.fsml` state's `<Description>`.

A multi-process application starts `mtrouter` first, then the provider, then the
consumer. A consumer that starts first is not an error: it waits.

