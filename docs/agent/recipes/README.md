# Recipes

Each directory is a complete, buildable AREG project. Copy one, rename it, and change
the service document. Copying costs nothing to read; adapting an example costs a lot.

| Recipe | Shows | Needs `mtrouter` |
|---|---|---|
| `01-local-single-process/` | One request and one response, provider and consumer in two threads of one process | no |
| `02-ipc-two-processes/` | The same service split into a provider process and a consumer process | yes |
| `03-attributes-and-broadcast/` | Publishing an attribute, subscribing to it, and sending a broadcast | no |
| `04-timer/` | A provider that broadcasts on every period of a timer | no |
| `05-two-services/` | Two services and a component that is provider of one and consumer of the other | no |
| `06-state-machine/` | A service whose logic is a `.fsml` state machine, with its action handler | no |
| `07-worker-events/` | A worker thread doing the slow part of a request, a custom event each way, a watchdog and `config/areg.init` | no |
| `08-observability/` | Two processes logging to `logcollector` instead of their own consoles, collected into a `.sqlog` database, which `query_sqlog.py` beside the recipe reads back | yes |
| `09-shared-types/` | Two services carrying one structure, declared once in a `.dtml` and included by both, kept in an `areg::ArrayList` | no |
| `10-runtime-model/` | The model built with `areg::Model` at run time, when the number of components is not known at compile time | no |
| `11-monitored-workers/` | A worked decomposition: two services, three workers, a monitor holding one proxy per worker, an operator | no |
| `12-testing/` | A consumer tested against a scripted provider: canned answers, assertions on the worker's own thread, and a non-zero exit code when one fails | no |

## How to use one

```bash
cp -r <areg-sdk>/docs/agent/recipes/01-local-single-process ./myproject
cd myproject
cmake -B build
cmake --build build -j
./build/bin/hello_local.elf
```

On Windows, copy the folder with `xcopy /E /I`, and the last line is
`build\bin\hello_local.exe`. The two configure and build lines are the same
everywhere. Copying a recipe needs no Python: CMake 3.20+, a Java 17+ runtime and a
C++17 compiler are the whole requirement.

The top level `CMakeLists.txt` finds an installed areg package, or fetches the SDK
from GitHub when there is none. Nothing else has to be set up.

## What to change

| To ... | Edit |
|---|---|
| Rename the project | `PROJECT_NAME` in `CMakeLists.txt`, and the target names in `src/CMakeLists.txt` |
| Change the contract | `src/services/*.siml`, then rebuild; the generator runs by itself |
| Add a request | The document, then implement the new `request_*` in the provider |
| Add a service | A second `.siml`, a second `addServiceInterface` line, a second component |

The naming rule that turns a document into C++ names is in
`../20-service-interface.md` section 3.

## Recipe 02 needs the router

A service reaching beyond its own process is declared `Category="Public"` and needs
`mtrouter` running. Start it first, then the provider, then the consumer:

```bash
./build/bin/mtrouter.elf --service &     # --service, not the console default
./build/bin/hello_provider.elf
./build/bin/hello_consumer.elf
```

```bat
start "" build\bin\mtrouter.exe --service
build\bin\hello_provider.exe
build\bin\hello_consumer.exe
```

`--service` is the unattended mode and is required. The console default reads
commands from a terminal; started from a shell with no terminal it keeps running,
paints its display and never binds 8181, with no error. Check the port with
`ss -ltn | grep 8181`, not the process list.

Only one router can hold port 8181. If one is already running, a second prints its
banner, binds nothing and routes nothing, so check the port rather than assuming the
one you just started is the one serving.

Executables land in `build/bin/` with a platform suffix: `.elf` on Linux, `.mac` on
macOS, `.exe` on Windows. `mtrouter` is built there too.

Consumers may start before providers. They wait and connect when the provider appears.

## Verification status

Every recipe here has been generated, compiled and, where it ends by itself, run.
From a clone of the SDK, this repeats the check:

```bash
python3 tools/agent/check_recipes.py --lib build/bin
```

A recipe that no longer compiles is worse than no recipe, because it is copied first
and debugged afterwards. Run the check after any change to the framework.
