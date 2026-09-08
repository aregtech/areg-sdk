# Working with the AREG SDK

AREG is a framework for service-oriented applications. Describe a service once in a
`.siml` document; the generator writes the communication code and you implement only
the service logic. The same code runs in one thread, many threads, many processes or
many machines: what changes is where a component is registered, not what it does.

**Fast path.** Define or change a service interface, implement a provider or a
consumer, register components into threads or processes, add a timer, log from
application code: for any of those, `docs/agent/00-cheatsheet.md` is the whole
reading list. Go there now, skip `CODEBASE.md` and the rest of this file, and return
only for section 6. `areg::String` and the containers are not on it: read
`docs/agent/40-base-api.md` **before the first line of C++**, not after the first
error. It is what stops a remembered name reaching the compiler.

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
| **Anything ordinary** | `docs/agent/00-cheatsheet.md` (most tasks end here) |
| **Start from working code** | `docs/agent/recipes/` - copy one, do not read it. `recipes/README.md` maps them |
| **Decide what the services are** | `docs/agent/05-design.md`, before writing any file |
| Start a new project by hand | `docs/agent/10-new-project.md` |
| Define an interface; what to override, what to call | `docs/agent/20-service-interface.md` (section 3: the names) |
| Implement a service provider | `docs/agent/30-provider.md` |
| Implement a service consumer | `docs/agent/31-consumer.md` |
| Register components into threads / processes | `docs/agent/32-model.md` |
| Watchdog, worker thread, model built at run time | copy `docs/agent/recipes/07-worker-events/`; `docs/agent/37-threads.md` |
| Send a custom event between threads | `docs/agent/23-events.md` |
| Do periodic or delayed work | `docs/agent/33-timers.md` |
| Log from code; collect several processes' logs | `docs/agent/34-logging.md` |
| Understand the seven core concepts | `CODEBASE.md` section 1 |
| Data types: C++ spelling, struct, enum, `.dtml` | `docs/agent/21-data-types.md` |
| Add a state machine | copy `docs/agent/recipes/06-state-machine/`, then `docs/agent/22-state-machine.md` |
| **Write any C++ that uses a string or a container** | `docs/agent/40-base-api.md` |
| What a component knows about itself; the application, threads, timers, time, files | `docs/agent/42-runtime-api.md` |
| Integrate areg into an existing CMake project | `docs/wiki/02b-cmake-integrate.md` |
| Work out why it does not work | `docs/agent/51-debug.md` |
| Test the application, or a component on its own | copy `docs/agent/recipes/12-testing/`; `docs/agent/52-testing.md` |
| Start `mtrouter`, `logcollector` or `logobserver` | `docs/agent/50-running.md` |
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

Four commands, from nothing to a running application.

```bash
python3 <areg-sdk>/tools/agent/setup_project.py --name myapp --root ~/myapp --mode local
cd ~/myapp
cmake -B build                      # fetches areg, runs the generator
cmake --build build -j$(nproc)
./build/bin/hello_local.elf         # .mac on macOS, .exe on Windows
```

Always give `-j` a number: a bare `-j` starts every job at once and swaps.

**Start here, not by hand.** It copies the right recipe, renames it and writes the
project its own `AGENTS.md`. `--mode` is `local`, `ipc` (two processes) or `pubsub`;
`--sdk-root <path>` builds against a local SDK copy instead of fetching one. Without
Python, copy `docs/agent/recipes/01-local-single-process` and rename the project in
its two `CMakeLists.txt` files; `recipes/README.md` says which recipe shows what.

`addServiceInterface()` in the project's `CMakeLists.txt` runs the generator during
configure; `tools/codegenerate.sh` / `.bat` generates outside CMake.

**The generator validates before it generates.** A refused document writes nothing and
exits 1, so the defect is in the document, never in the build. Every finding is
`file:line:col: error[<number>/<RULE_NAME>]` with a `fix:` line under it: read that,
then `tools/explain_rule.py <number>`. Not the schema: it says what an element may
contain, never which rule refused it. `tools/schema/` is where a spelling is looked
up, and the page for your document kind names the file.

### Every command on this path, on Windows

The pages below use POSIX commands. These five substitutions are the whole difference.

`python3 x.py` -> `python x.py` - `./build/bin/n.elf` -> `build\bin\n.exe` -
`tools/codegenerate.sh` -> `tools\codegenerate.bat` - `prog --service &` ->
`start "" prog --service` - `-j$(nproc)` -> `-j%NUMBER_OF_PROCESSORS%`

---

## 5. Tools

Run these instead of writing what they produce. All are Python 3 (`python` on
Windows), live in `tools/agent/`, and have `--help`.

| Tool | Does |
|---|---|
| `setup_project.py` | Creates a buildable project from a recipe, with its own `AGENTS.md` |
| `gen_skeleton.py` | Writes the components a `.siml` or `.fsml` needs |
| `run_scenarios.py` | Runs the application and checks its output; exit 0 is a pass. Its `scenarios.json` is `docs/agent/50-running.md` |
| `check_contract.py` | Checks sources against `docs/agent/api.json`: the section 6 mistakes that compile cleanly and fail later |

`tools/explain_rule.py` explains a validation finding by its number; `--search
"words"` finds it when the number is lost. It, `tools/check-env.sh` and
`tools/codegenerate.sh` (`.bat` on Windows) sit in `tools/` itself.

**Each has one moment**, and **nothing else under `tools/` is yours**: the rest checks
this repository's own corpus, tells you nothing about your application, and costs a
turn each.

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
in `process_timer` by `name()` rather than by address.

---

## 7. Definition of done

A task is finished when the application builds and its behaviour is observed, not
when the code looks correct.

```bash
python3 tools/agent/check_contract.py <project>   # the mistakes a build cannot catch
cmake --build build -j$(nproc)              # must succeed
<run the application>                       # expected output, exit 0
```

Silence a `check_contract.py` false positive with `// areg-check: ignore`, or the
same words in a `.fsml` state's `<Description>`.

A multi-process application starts `mtrouter` first, then the provider, then the
consumer. A consumer that starts first is not an error: it waits.

