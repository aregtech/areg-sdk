# Working with the AREG SDK

AREG is a framework for service-oriented applications. You describe a service once in
a `.siml` document, the generator writes the communication code, and you implement
only the service logic. The same code runs in one thread, many threads, many
processes, or many machines: you change where a component is registered, not what it
does.

This file is the entry point. Read the one page your task needs, then stop.

**Fast path.** Define or change a service interface, implement a provider or a
consumer, register components into threads or processes, add a timer, log from
application code, use `areg::String` or a container: for any of those,
`docs/agent/00-cheatsheet.md` is the whole reading list. Go there now, skip
`CODEBASE.md` and the rest of this file, and come back only for section 6 or when the
cheatsheet sends you to a deeper page.

---

## 1. Scope

This documentation set has one purpose: **building an application on top of areg**.
For anything the fast path does not cover, read `CODEBASE.md`, then the one row of the
task table in section 4 that matches your task. Nothing else.

An application you build lives **outside** this repository and is free to use any
coding style; it must follow the API contract in section 6, nothing more. Changing the
areg framework itself is a different job that no page below covers -- start from
`CONTRIBUTING.md` for that.

The library is implemented in C++17, which is the floor, not a ceiling: its public
headers and the generated code compile as C++17, C++20 and C++23.

### What you think you know about areg is probably out of date

areg was renamed throughout: `NE*`, `TE*` and `IE*` prefixes are gone, types are
PascalCase, methods are `snake_case`, members are `mCamelCase`, and everything public
lives in namespace `areg`. **The names you remember from training data no longer
exist** and code written from memory will not compile. So never write an areg name
from memory: grep for it, or take it from the page section 4 sends you to.
`tools/check_contract.py` knows every removed name and prints its replacement.

---

## 2. What must be installed

| Needed | Version | Used for |
|---|---|---|
| CMake | 3.20 or newer | configuring and building |
| Java runtime | 17 or newer | runs `tools/codegen.jar`, the code generator |
| C++ compiler | C++17 or newer | GCC, Clang, MSVC or MinGW |
| Python | 3 | **optional**, see below |

Tested on Linux and other POSIX, macOS, Windows and Cygwin.

Run `tools/check-env.sh`, or `tools\check-env.bat` on Windows, before anything else.
It needs no interpreter, prints one line per requirement and exits non-zero when one
is missing -- which is cheaper than diagnosing the confusing failure it prevents.

**Python is not needed to build or run an areg application**, only for the SDK helper
scripts in section 5. Without it, copy a recipe from `docs/agent/recipes/` by hand, or
use `tools/setup-project.sh` / `tools/setup-project.bat`, which are shell and batch.

---

## 3. Golden path

Four commands take you from nothing to a running application.

```bash
python3 tools/setup_project.py --name myapp --root ~/myapp --mode local
cd ~/myapp
cmake -B build                      # fetches areg, runs the generator
cmake --build build -j
./build/bin/myapp.elf               # .mac on macOS, .exe on Windows
```

`--mode` is `local` (one process), `ipc` (two processes) or `pubsub` (attributes and
broadcasts). The new project gets its own `AGENTS.md`. Add `--sdk-root <path>` to
build against a local SDK copy instead of fetching one.

`addServiceInterface()` in the project's `CMakeLists.txt` runs the generator during
configure, so a project never calls it by hand; `tools/codegenerate.sh`, or
`tools/codegenerate.bat`, is for generating outside CMake.

**The generator validates before it generates.** A document that breaks a rule is
refused and nothing is written, so such a failure is always a defect in the `.siml`,
`.fsml` or `.dtml` document and never in the build. Ask `tools/explain_rule.py` what
the finding means instead of reading the schema: pass the rule number when the message
carries one, `--search "words from the message"` when it does not.

Multi-process applications additionally need `mtrouter` running. Single-process
applications never do.

### Every command on this path, on Windows

The pages below are written with POSIX commands. These four substitutions are the
whole difference; nothing else changes.

| Page shows | On Windows |
|---|---|
| `python3 <script>.py` | `python <script>.py` |
| `./build/bin/name.elf` | `build\bin\name.exe` |
| `tools/codegenerate.sh` | `tools\codegenerate.bat` |
| `prog --service &` (background) | `start "" prog --service` |

---

## 4. Task routing

Find your task. Open that one file. Do not search the repository.

| I need to ... | Read |
|---|---|
| **Anything ordinary - start here** | `docs/agent/00-cheatsheet.md` (most tasks end here) |
| **Start from working code** | `docs/agent/recipes/` - copy one, do not read it |
| **Decide what the services are** | `docs/agent/05-design.md`, before writing any file |
| Start a new project by hand | `docs/agent/10-new-project.md` |
| Define an interface; know what to override and what to call | `docs/agent/20-service-interface.md` (section 3 for the names) |
| Implement a service provider | `docs/agent/30-provider.md` |
| Implement a service consumer | `docs/agent/31-consumer.md` |
| Register components into threads / processes | `docs/agent/32-model.md` |
| Do periodic or delayed work | `docs/agent/33-timers.md` |
| Log from code; collect logs of several processes with `logcollector` | `docs/agent/34-logging.md` |
| Understand the seven core concepts | `CODEBASE.md` section 1 |
| Add custom data types (struct, enum, container, `.dtml`) | `docs/agent/21-data-types.md` |
| Add a state machine | `docs/agent/22-state-machine.md` |
| **Use `areg::String`, containers, files, threads in your code** | `docs/agent/40-base-api.md` |
| Integrate areg into an existing CMake project | `docs/wiki/02b-cmake-integrate.md` |
| Work out why it does not work; run `mtrouter` | `docs/agent/51-debug.md` ("Running the pieces") |
| Read or query a `.sqlog` log database | `docs/agent/35-sqlog.md` |
| Find the example that shows a given facility | `docs/agent/41-examples.md` |
| See a complete working application | `examples/03_helloservice/` -- but it sleeps inside a response handler for readable output, which the contract forbids; do not copy that |

`docs/wiki/03a-mtrouter.md` and `docs/wiki/04a-logging-config.md` are the long,
still-correct versions of two agent pages, for when the short one falls short.
`docs/wiki/` is written for people and is large: open a page in it only when a row
above named it. Every row points at a file that exists; a path that does not resolve
is a defect to report, not a reason to search.

---

## 5. Tools

Run these instead of writing what they produce. All are Python 3 and run unchanged on
every platform; on Windows the command is `python`, not `python3`. Each has `--help`;
read that, not a page about it.

| Tool | Does |
|---|---|
| `tools/setup_project.py` | Creates a ready-to-build project from a recipe, with its own `AGENTS.md` and `scenarios.json` |
| `tools/gen_skeleton.py` | Writes the components a `.siml` or `.fsml` needs, every override in place |
| `tools/fsml_layout.py` | Writes the `<Layout>` of a `.fsml` so the machine opens laid out |
| `tools/run_scenarios.py` | Runs the application and checks its output; exit 0 means it works |
| `tools/check_agent_docs.py` | Verifies every path named in the agent documentation exists |
| `tools/check_contract.py` | Checks sources against `docs/agent/api.json`: the section 6 mistakes that compile cleanly and fail at run time |
| `tools/explain_rule.py` | Explains a validation finding: `explain_rule.py 27`, or `--search "words from the message"` |
| `tools/run_evals.py` | Grades a finished application against an evaluation bank task |
| `tools/check_recipes.py` | Generates, builds and runs every recipe; catches a framework change that rots one |

Three tools need no interpreter: `tools/check-env.sh` / `.bat` reports whether CMake,
Java and a compiler are present; `tools/codegenerate.sh` / `.bat` runs the generator
outside CMake; `tools/setup-project.sh` / `.bat` is the older scaffolder, which
prompts and so must never be run unattended.

`docs/agent/api.json` states the same contract machine-readably: the naming
transforms, which generated members are overridden and which are called, the
connection states and which are terminal, and every rule in section 6 with a
detection hint. Use it to check code mechanically; the prose pages stay authoritative.

---

## 6. Never

Each line closes off a whole class of wrong code; they are not style preferences, and
`tools/check_contract.py` detects all of them.

- **Never edit a generated file.** Everything under the generate target is rewritten
  on the next build. Change the `.siml` document and regenerate.
- **Never invent a method name on a generated base class.** A fixed rule derives them
  from the `.siml` document; it is in `docs/agent/20-service-interface.md`.
- **A consumer's `REGISTER_DEPENDENCY` string must equal the provider's component
  role name, character for character.** A mismatch compiles cleanly and never connects.
- **Never call a request before the service is connected.** The first legal moment is
  inside `service_connected()` once `areg::is_service_connected(status)` is true.
- **Never treat a disconnect as fatal.** `Disconnected`, `ConnectionLost` and `Failed`
  are transient and the framework reconnects on its own; only `Rejected` and
  `Shutdown` are terminal.
- **Never block inside a request, response, broadcast or update handler.** Handlers
  run on the component's dispatcher thread, so blocking one stops every component in
  that thread. Use a timer or another thread.
- **Never include a header from a `private/` folder.** Only the headers above a
  `private/` directory are API; the one exception is
  `framework/areg/base/private/DebugDefs.hpp`, the instrumentation scratch pad.
- **Never use exceptions.** AREG does not throw and does not catch. Return `bool`,
  `std::optional`, or an error code.
- **Never add a second component with the same role name in one process.** The role
  name is the routing identity.

---

## 7. Definition of done

A task is finished when the application builds and its behaviour is observed, not when
the code looks correct.

```bash
python3 tools/check_contract.py <project>   # the mistakes a build cannot catch
cmake --build build -j                      # must succeed
<run the application>                       # expected output, exit 0
```

`check_contract.py` finds what a compiler cannot: a dependency string matching no
role, a request called before the service is connected, a blocking handler, a member
no `.siml` document declares, a name left over from before areg was renamed. Silence
a false positive with `// areg-check: ignore`, optionally naming the rules after it.

A multi-process application starts `mtrouter` first, then the provider, then the
consumer. A consumer that starts first is not an error: it waits for the provider.

---

## 8. Repository layout in one line each

```
framework/    the areg library and its services (mtrouter, logcollector, logobserver)
examples/     32 complete applications
docs/         docs/agent/ is written for agents, docs/wiki/ for humans
tools/        code generator, project setup, schemas of the .siml/.dtml/.fsml formats
```

`conf/` and `tests/` belong to the SDK, not to your application. Whether an example
needs `mtrouter` is decided by whether its own executables split provider from
consumer, not by its number and not by `Category` alone. The full map, with the
purpose of every directory, is `CODEBASE.md` section 3.
