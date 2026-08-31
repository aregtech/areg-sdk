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

## How to use one

```bash
cp -r <areg-sdk>/docs/agent/recipes/01-local-single-process ./myproject
cd myproject
cmake -B build
cmake --build build -j
./build/bin/hello_local.elf
```

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

`--service` is the unattended mode; the console default paints a live status display
that only makes sense on a terminal. Both route the same.

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
python3 tools/check_recipes.py --lib build/bin
```

A recipe that no longer compiles is worse than no recipe, because it is copied first
and debugged afterwards. Run the check after any change to the framework.
