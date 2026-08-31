# Working with the AREG SDK

AREG is a framework for service-oriented applications. You describe a service
once in a `.siml` document, the generator writes the communication code, and you
implement only the service logic. The same code runs in one thread, many threads,
many processes, or many machines: you change where a component is registered, not
what it does.

This file is the entry point. Read the one page your task needs, then stop.

---

## 1. Which are you doing?

| | |
|---|---|
| **Building an application WITH areg** | Read `CODEBASE.md`, then the one row of the table below that matches your task. Nothing else. |
| **Changing areg itself** | Read `CODEBASE.md` and `docs/AREG_AI_CODING_RULES.md`. The style rules bind this repository only. |

An application you build lives **outside** this repository and is free to use any
coding style. It must follow the API contract in section 4, nothing more.

The library is implemented in C++17, which is the floor, not a ceiling: its public
headers and the generated code compile as C++17, C++20 and C++23, so your application
may pick any of them.

---

## 2. Golden path

Six commands take you from nothing to a running application.

```bash
python3 tools/setup_project.py --name myapp --root ~/myapp --mode local
cd ~/myapp
cmake -B build                      # fetches areg, runs the generator
cmake --build build -j              # build
./build/bin/myapp.elf               # run (.mac on macOS, .exe on Windows)
```

`--mode` is `local` (one process), `ipc` (two processes) or `pubsub` (attributes and
broadcasts). The new project gets its own `AGENTS.md`. Add `--sdk-root <path>` to
build against a local SDK copy instead of fetching one.

A project needs no manual generator call: `addServiceInterface()` in the project's
`CMakeLists.txt` runs it during configure. Call `tools/codegenerate.sh` only when
generating outside CMake.

Multi-process applications additionally need `mtrouter` running. Single-process
applications never do.

---

## 3. Task routing

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
| Integrate areg into an existing CMake project | `docs/wiki/02b-cmake-integrate.md` |
| Run the message router for multi-process apps | `docs/wiki/03a-mtrouter.md` |
| Work out why it does not work | `docs/agent/51-debug.md` |
| Turn logging on and read the logs | `docs/wiki/04a-logging-config.md` |
| See a complete working application | `examples/03_helloservice/` |

Every row above points at a file that exists. If a path does not resolve, that is a
defect: report it instead of searching.

---

## 3a. Tools

Run these instead of writing what they produce.

| Tool | Does |
|---|---|
| `tools/setup_project.py` | Creates a ready-to-build project from a recipe, with its own `AGENTS.md` and `scenarios.json` |
| `tools/gen_skeleton.py` | Writes the components a `.siml` or `.fsml` needs, with every override in place |
| `tools/fsml_layout.py` | Writes the `<Layout>` of a `.fsml` so the machine opens laid out in the editor |
| `tools/run_scenarios.py` | Runs the application and checks its output; exit 0 means it works |
| `tools/check_agent_docs.py` | Verifies every path named in the agent documentation exists |
| `tools/explain_rule.py` | Explains a validation rule number the generator reported, e.g. `explain_rule.py 27` |
| `tools/run_evals.py` | Grades a finished application against a task from the evaluation bank |
| `tools/check_recipes.py` | Generates, builds and runs every recipe; catches a framework change that rots one |

Each has `--help`. Read that, not a page about it.

---

## 4. Never

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

## 5. Definition of done

A task is finished when the application builds and its behaviour is observed, not
when the code looks correct.

```bash
cmake --build build -j        # must succeed
<run the application>         # must show the expected output and exit 0
```

For a multi-process application, start `mtrouter` first, then the provider process,
then the consumer process. A consumer that starts first is not an error: it waits and
connects when the provider appears.

---

## 6. Repository layout in one line each

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
