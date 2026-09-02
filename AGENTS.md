# Working with the AREG SDK

AREG is a framework for service-oriented applications. Describe a service once in a
`.siml` document; the generator writes the communication code and you implement only
the service logic. The same code runs in one thread, many threads, many processes or
many machines: what changes is where a component is registered, not what it does.

This file is the entry point: read the one page your task needs, then stop.

**Fast path.** Define or change a service interface, implement a provider or a
consumer, register components into threads or processes, add a timer, log from
application code, use `areg::String` or a container: for any of those,
`docs/agent/00-cheatsheet.md` is the whole reading list. Go there now, skip
`CODEBASE.md` and the rest of this file, and return only for section 6.

---

## 1. Scope

This documentation set has one purpose: **building an application on top of areg**.
`CODEBASE.md` is not a pre-read: it is one row of the section 2 table, answering what
the seven core concepts are.

Your application lives **outside** this repository and may use any coding style; it
owes the contract in section 6 and nothing more. Changing the framework itself is a
different job: start from `CONTRIBUTING.md`. C++17 is the floor -- the public headers
and the generated code also compile as C++20 and C++23.

### What you think you know about areg is probably out of date

areg was renamed throughout: the `NE*`, `TE*` and `IE*` prefixes are gone, types are
PascalCase, methods `snake_case`, members `mCamelCase`, and everything public lives in
namespace `areg`. **The names you remember from training data no longer exist.** Take
every name from the page section 2 sends you to; `tools/agent/check_contract.py` knows the
removed ones and prints their replacements.

---

## 2. Task routing

Find your task, open that one file, and do not search the repository.

| I need to ... | Read |
|---|---|
| **Anything ordinary** | `docs/agent/00-cheatsheet.md` (most tasks end here) |
| **Start from working code** | `docs/agent/recipes/` - copy one, do not read it |
| **Decide what the services are** | `docs/agent/05-design.md`, before writing any file |
| Start a new project by hand | `docs/agent/10-new-project.md` |
| Define an interface; what to override, what to call | `docs/agent/20-service-interface.md` (section 3: the names) |
| Implement a service provider | `docs/agent/30-provider.md` |
| Implement a service consumer | `docs/agent/31-consumer.md` |
| Register components into threads / processes | `docs/agent/32-model.md` |
| Watchdog, worker thread, model built at run time | `docs/agent/37-threads.md` |
| Send a custom event between threads | `docs/agent/23-events.md` |
| Do periodic or delayed work | `docs/agent/33-timers.md` |
| Log from code; collect several processes' logs | `docs/agent/34-logging.md` |
| Understand the seven core concepts | `CODEBASE.md` section 1 |
| Add custom data types (struct, enum, `.dtml`) | `docs/agent/21-data-types.md` |
| Add a state machine | `docs/agent/22-state-machine.md` |
| **Use `areg::String` or a container** | `docs/agent/40-base-api.md` |
| Application, component, thread, timer, time, file, buffer | `docs/agent/42-runtime-api.md` |
| Integrate areg into an existing CMake project | `docs/wiki/02b-cmake-integrate.md` |
| Work out why it does not work | `docs/agent/51-debug.md` |
| Test the application, or a component on its own | `docs/agent/52-testing.md` |
| Start `mtrouter`, `logcollector` or `logobserver` | `docs/agent/50-running.md` |
| Read or query a `.sqlog` log database | `docs/agent/35-sqlog.md` |
| Set the router address, ports, anything in `areg.init` | `docs/agent/36-config.md` |
| Find the example showing a given facility | `docs/agent/41-examples.md` |
| See a complete working application | `examples/03_helloservice/` -- it sleeps in a response handler, which the contract forbids; do not copy that |

`docs/wiki/` is written for people and is large: open a page there only when a row
above names it. Every row points at a file that exists; a path that does not resolve
is a defect to report, not a reason to search.

---

## 3. What must be installed

CMake 3.20+, a Java 17+ runtime for `tools/codegen.jar` (the code generator), and a
C++17 compiler -- GCC, Clang, MSVC or MinGW. Python 3 is optional, see below. Tested
on Linux and other POSIX, macOS, Windows and Cygwin.

Run `tools/check-env.sh`, or `tools\check-env.bat`, before anything else: it needs no
interpreter and exits non-zero when a requirement is missing.

**Python is not needed to build or run an areg application**, only for the section 5
scripts. Without it, copy a recipe from `docs/agent/recipes/` by hand.

---

## 4. Golden path

Four commands, from nothing to a running application. **Python is not a
requirement:** copying a recipe needs only CMake, Java and a compiler.

```bash
cp -r <areg-sdk>/docs/agent/recipes/01-local-single-process ~/myapp
cd ~/myapp
cmake -B build                      # fetches areg, runs the generator
cmake --build build -j
./build/bin/hello_local.elf         # .mac on macOS, .exe on Windows
```

`recipes/README.md` says which recipe shows what; rename the project in the two
`CMakeLists.txt` files, then change the `.siml`.

Where Python is available, `tools/agent/setup_project.py --name myapp --root ~/myapp
--mode local` does the copy and the renaming in one step and writes the project its
own `AGENTS.md`. `--mode` is `local`, `ipc` (two processes) or `pubsub`; `--sdk-root
<path>` builds against a local SDK copy instead of fetching one.

`addServiceInterface()` in the project's `CMakeLists.txt` runs the generator during
configure; `tools/codegenerate.sh` / `.bat` generates outside CMake.

**The generator validates before it generates.** A refused document writes nothing, so
the defect is in the `.siml`, `.fsml` or `.dtml`, never in the build. The message
names the rule in words and usually carries no number, so ask
`tools/explain_rule.py --search "words from the message"`, or
`explain_rule.py <number>` when a number is printed, instead of reading the schema.

### Every command on this path, on Windows

The pages below use POSIX commands. These four substitutions are the whole difference.

`python3 x.py` -> `python x.py` - `./build/bin/n.elf` -> `build\bin\n.exe` -
`tools/codegenerate.sh` -> `tools\codegenerate.bat` - `prog --service &` ->
`start "" prog --service`

---

## 5. Tools

Run these instead of writing what they produce. All are Python 3 (`python` on
Windows), all live in `tools/agent/`, and each has `--help`.

| Tool | Does |
|---|---|
| `setup_project.py` | Creates a buildable project from a recipe, with its own `AGENTS.md` |
| `gen_skeleton.py` | Writes the components a `.siml` or `.fsml` needs |
| `run_scenarios.py` | Runs the application and checks its output; exit 0 is a pass. Its `scenarios.json` is `docs/agent/50-running.md` |
| `check_contract.py` | Checks sources against `docs/agent/api.json`: the section 6 mistakes that compile cleanly and fail later |

`tools/explain_rule.py` explains a validation finding: `--search "words"`, or a number
when the message carries one. It, `tools/check-env.sh` / `.bat` and
`tools/codegenerate.sh` / `.bat` are developer tools and sit in `tools/` itself; only
the first three need Python.

`docs/agent/api.json` states the same contract machine-readably: naming transforms,
which members are overridden and which are called, the connection states, and every
rule of section 6 with its detection hint.

---

## 6. Never

Each line closes a class of wrong code, not a style preference, and
`tools/agent/check_contract.py` detects all of them.

- **Never edit a generated file.** The generate target is rewritten on every build;
  change the `.siml` instead.
- **Never invent a method name on a generated base class.** A fixed rule derives them
  from the document: `docs/agent/20-service-interface.md`.
- **A consumer's `REGISTER_DEPENDENCY` string must equal the provider's component
  role name, character for character.** A mismatch compiles cleanly and never connects.
- **Never call a request before the service is connected.** The first legal moment is
  inside `service_connected()` once `areg::is_service_connected(status)` is true.
- **Never treat a disconnect as fatal.** `Disconnected`, `ConnectionLost` and `Failed`
  reconnect on their own; only `Rejected` and `Shutdown` are terminal.
- **Never block inside a request, response, broadcast or update handler.** Blocking a
  dispatcher thread stops every component in it. Use a timer or a worker thread.
- **Never include a header from a `private/` folder.** Only the headers above one are
  API; the sole exception is `framework/areg/base/private/DebugDefs.hpp`.
- **Never use exceptions.** AREG does not throw and does not catch. Return `bool`,
  `std::optional`, or an error code.
- **Never add a second component with the same role name in one process.** The role
  name is the routing identity.
- **Never register a watchdog timeout on a thread the watchdog does not run for.** The
  timeout in `BEGIN_REGISTER_THREAD_EX` does nothing until the watchdog is started, so
  it reads as protection and is none.
- **Never let a `REGISTER_WORKER_THREAD` consumer name differ from the name the
  component answers to.** Nothing matches it, and the worker's events go nowhere.
- **Never override a `broadcast_*` or `on_*_update` without subscribing.** The handler
  runs only after `notify_on_...(true)`; without it the application waits for ever.
- **Never answer a request after its handler returns without releasing it first.**
  `unblock_current_request()` in the handler returns the session id;
  `prepare_response(session)` restores it later. The second client gets `RequestBusy`.

---

## 7. Definition of done

A task is finished when the application builds and its behaviour is observed, not
when the code looks correct.

```bash
python3 tools/agent/check_contract.py <project>   # the mistakes a build cannot catch
cmake --build build -j                      # must succeed
<run the application>                       # expected output, exit 0
```

Silence a `check_contract.py` false positive with `// areg-check: ignore`.

A multi-process application starts `mtrouter` first, then the provider, then the
consumer. A consumer that starts first is not an error: it waits for the provider.

---

## 8. Repository layout

`framework/` the library and its services - `examples/` 32 complete applications -
`docs/agent/` these pages - `tools/` the generator, scripts and schemas. Full map:
`CODEBASE.md` section 3.
