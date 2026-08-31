# Working with the AREG SDK

AREG is a framework for service-oriented applications. You describe a service
once in a `.siml` document, the generator writes the communication code, and you
implement only the service logic. The same code runs in one thread, many threads,
many processes, or many machines: you change where a component is registered, not
what it does.

This file is the entry point. Read the one page your task needs, then stop.

---

## 1. Scope

This documentation set has one purpose: **building an application on top of areg**.
Read `CODEBASE.md`, then the one row of the task table in section 4 that matches your
task. Nothing else.

An application you build lives **outside** this repository and is free to use any
coding style. It must follow the API contract in section 6, nothing more.

Changing the areg framework itself is a different job that this documentation set does
not cover: no page below applies to it. Start from `CONTRIBUTING.md` instead.

The library is implemented in C++17, which is the floor, not a ceiling: its public
headers and the generated code compile as C++17, C++20 and C++23, so your application
may pick any of them.

### What you think you know about areg is probably out of date

areg was renamed throughout. If you have seen this framework before, in training data
or elsewhere, **the names you remember no longer exist** and code written from memory
will not compile.

| Gone | Now |
|---|---|
| `NEService`, `NECommon`, `NELogging`, and every other `NE*` name | plain names in namespace `areg`, e.g. `areg::LogPriority` |
| `TEArrayList`, `TEHashMap`, and every other `TE*` template | plain names, e.g. `areg::TokenList`; see `docs/agent/40-base-api.md` |
| camelCase methods: `getTime()`, `isValid()`, `getAppName()` | snake_case: `to_time()`, `is_valid()`, `app_name()` |

The current conventions are: **types** PascalCase, **methods** `snake_case`,
**members** `mCamelCase`, everything inside **namespace `areg`**. No file in the
repository is named `NE*` or `TE*`. One legacy *namespace* does survive in current
code -- `NEMultitargetRouterSettings` in `framework/mtrouter/app/MTRouterNames.hpp` --
so seeing it there is not a licence to assume the others still exist.

So: never write an areg name from memory. Grep for it, or take it from the page in
section 4 that covers your task. A name that "looks right" from an older version is
the single most likely reason your first build fails.

---

## 2. What must be installed

Check these before the first command. A missing one fails later with a message that
does not name the cause.

| Needed | Version | Used for |
|---|---|---|
| CMake | 3.20 or newer | configuring and building |
| Java runtime | 17 or newer | runs `tools/codegen.jar`, the code generator |
| C++ compiler | C++17 or newer | GCC, Clang, MSVC or MinGW |
| Python | 3 | **optional**, see below |

Tested platforms: Linux and other POSIX, macOS, Windows, Cygwin.

**Check all of this with one command**, which needs no interpreter and so still works
when Python is the missing piece:

```bash
tools/check-env.sh            # POSIX; tools\check-env.bat on Windows
```

It prints one line per requirement and exits non-zero when something is missing. Run
it before the golden path rather than diagnosing a confusing failure later.

**Python is not needed to build or run an areg application.** It is needed only for
the SDK helper scripts: project setup, skeleton generation, scenario runs, rule
lookup. Without Python, copy a recipe from `docs/agent/recipes/` by hand -- they are
ordinary project files -- or use the interactive scaffolder `tools/setup-project.sh`
or `tools/setup-project.bat`, which are shell and batch only.

---

## 3. Golden path

Five commands take you from nothing to a running application.

```bash
# Linux, macOS, Cygwin
python3 tools/setup_project.py --name myapp --root ~/myapp --mode local
cd ~/myapp
cmake -B build                      # fetches areg, runs the generator
cmake --build build -j              # build
./build/bin/myapp.elf               # .mac on macOS
```

```bat
REM Windows
python tools\setup_project.py --name myapp --root %USERPROFILE%\myapp --mode local
cd %USERPROFILE%\myapp
cmake -B build
cmake --build build -j
build\bin\myapp.exe
```

`--mode` is `local` (one process), `ipc` (two processes) or `pubsub` (attributes and
broadcasts). The new project gets its own `AGENTS.md`. Add `--sdk-root <path>` to
build against a local SDK copy instead of fetching one.

A project needs no manual generator call: `addServiceInterface()` in the project's
`CMakeLists.txt` runs it during configure. Call `tools/codegenerate.sh`, or
`tools/codegenerate.bat` on Windows, only when generating outside CMake.

**The generator validates before it generates.** A document that breaks a rule is
refused and nothing is written, so such a failure is a defect in the `.siml`, `.fsml`
or `.dtml` document and never in the build. Ask `tools/explain_rule.py` what the
finding means; do not read the schema. Pass the rule number when the message carries
one, and `--search` with words from the message when it does not.

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
| **Anything ordinary - start here** | `docs/agent/00-cheatsheet.md` (one page, most tasks end here) |
| **Start from working code** | `docs/agent/recipes/` - copy one, do not read it |
| **Decide what the services are** | `docs/agent/05-design.md` - do this before writing any file |
| Start a new project by hand | `docs/agent/10-new-project.md` |
| Define or change a service interface | `docs/agent/20-service-interface.md` |
| Know what to override and what to call | `docs/agent/20-service-interface.md` section 3 |
| Implement a service provider | `docs/agent/30-provider.md` |
| Implement a service consumer | `docs/agent/31-consumer.md` |
| Register components into threads / processes | `docs/agent/32-model.md` |
| Do periodic or delayed work | `docs/agent/33-timers.md` |
| Log from application code | `docs/agent/34-logging.md` |
| Understand the seven core concepts | `CODEBASE.md` section 1 |
| Add custom data types (struct, enum, container, `.dtml`) | `docs/agent/21-data-types.md` |
| Add a state machine | `docs/agent/22-state-machine.md` |
| **Use `areg::String`, containers, files, threads in your code** | `docs/agent/40-base-api.md` |
| Integrate areg into an existing CMake project | `docs/wiki/02b-cmake-integrate.md` |
| Run the message router for multi-process apps | `docs/agent/51-debug.md` section "Running the pieces" |
| Work out why it does not work | `docs/agent/51-debug.md` |
| Turn logging on, and collect logs from several processes with `logcollector` | `docs/agent/34-logging.md` |
| Read or query a `.sqlog` log database | `docs/agent/35-sqlog.md` |
| Find the example that shows a given facility | `docs/agent/41-examples.md` |
| See a complete working application | `examples/03_helloservice/` -- it sleeps inside a response handler to keep its console output readable. That is a demo trick the contract forbids; do not copy it |

Two rows above point at short agent pages that replaced long reference guides.
`docs/wiki/03a-mtrouter.md` and `docs/wiki/04a-logging-config.md` are still correct
and far more detailed; read them only when the agent page does not answer the
question.

`docs/wiki/` is written for people and is large. Open a page in it only when a row
above named it, never to look around.

Every row above points at a file that exists. If a path does not resolve, that is a
defect: report it instead of searching.

---

## 5. Tools

Run these instead of writing what they produce. All are Python 3 and run unchanged on
every platform; on Windows the command is `python`, not `python3`.

| Tool | Does |
|---|---|
| `tools/setup_project.py` | Creates a ready-to-build project from a recipe, with its own `AGENTS.md` and `scenarios.json` |
| `tools/gen_skeleton.py` | Writes the components a `.siml` or `.fsml` needs, with every override in place |
| `tools/fsml_layout.py` | Writes the `<Layout>` of a `.fsml` so the machine opens laid out in the editor |
| `tools/run_scenarios.py` | Runs the application and checks its output; exit 0 means it works |
| `tools/check_agent_docs.py` | Verifies every path named in the agent documentation exists |
| `tools/check_contract.py` | Checks your sources against `docs/agent/api.json`: the mistakes in section 6 that compile cleanly and fail only at run time |
| `tools/explain_rule.py` | Explains a validation finding: `explain_rule.py 27`, or `explain_rule.py --search "words from the message"` |
| `tools/run_evals.py` | Grades a finished application against a task from the evaluation bank |
| `tools/check_recipes.py` | Generates, builds and runs every recipe; catches a framework change that rots one |

Each has `--help`. Read that, not a page about it.

Two tools are not Python and need no interpreter:

| Tool | Platform | Does |
|---|---|---|
| `tools/check-env.sh` / `tools/check-env.bat` | POSIX / Windows | Reports whether CMake, Java and a C++ compiler are present. Run it first |
| `tools/codegenerate.sh` / `tools/codegenerate.bat` | POSIX / Windows | Runs the generator outside CMake; copy it and set the two paths at the top |
| `tools/setup-project.sh` / `tools/setup-project.bat` | POSIX / Windows | The older scaffolder. It prompts, so never run it unattended |

`docs/agent/api.json` states the same contract in machine-readable form: the naming
transforms, which generated members are overridden and which are called, the
connection states and which are terminal, and the prohibitions below with detection
hints. Use it to check code mechanically; the prose pages stay authoritative.

---

## 6. Never

Each line closes off a whole class of wrong code. They are not style preferences.

- **Never edit a generated file.** Everything under the generate target is rewritten
  on the next build. Change the `.siml` document and regenerate.
- **Never invent a method name on a generated base class.** The names are derived by
  a fixed rule from the `.siml` document. The rule is in `docs/agent/20-service-interface.md`.
- **A consumer's `REGISTER_DEPENDENCY` string must equal the provider's component
  role name, character for character.** A mismatch compiles cleanly and never connects.
- **Never call a request before the service is connected.** The first legal moment is
  inside `service_connected()` once `areg::is_service_connected(status)` is true.
- **Never treat a disconnect as fatal.** `Disconnected`, `ConnectionLost` and
  `Failed` are transient and the framework reconnects on its own. Only `Rejected` and
  `Shutdown` are terminal.
- **Never block inside a request, response, broadcast or update handler.** Handlers
  run on the component's dispatcher thread. Blocking one stops every component in
  that thread. Use a timer or another thread.
- **Never include a header from a `private/` folder.** Only the headers above a
  `private/` directory are API. The one exception is
  `framework/areg/base/private/DebugDefs.hpp`, the instrumentation scratch pad.
- **Never use exceptions.** AREG does not throw and does not catch. Return `bool`,
  `std::optional`, or an error code.
- **Never add a second component with the same role name in one process.** The role
  name is the routing identity.

---

## 7. Definition of done

A task is finished when the application builds and its behaviour is observed, not
when the code looks correct.

```bash
cmake --build build -j        # must succeed
<run the application>         # must show the expected output and exit 0
```

Run `tools/check_contract.py` on the sources before the build. It finds what a build
cannot: a dependency string that matches no role, a request called before the service
is connected, a blocking handler, a member name no `.siml` document declares.

For a multi-process application, start `mtrouter` first, then the provider process,
then the consumer process. A consumer that starts first is not an error: it waits and
connects when the provider appears.

---

## 8. Repository layout in one line each

```
framework/    the areg library and its services (mtrouter, logcollector, logobserver)
examples/     32 complete applications. mtrouter is needed when an example splits
              provider and consumer into separate processes, which its own
              executables decide -- not its number, and not Category alone
docs/         documentation; docs/agent/ is written for agents, docs/wiki/ for humans
tools/        code generator, project setup, schemas of the .siml/.dtml/.fsml formats
conf/         build system configuration and installation templates
tests/        unit tests of the framework itself
```

Full map with the purpose of every directory: `CODEBASE.md`.
