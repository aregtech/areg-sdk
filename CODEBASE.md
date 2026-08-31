# AREG SDK - structure and mental model

Read section 1 before writing any code. Everything else in this file is a lookup
table you come back to.

---

## 1. The seven concepts

Every AREG application is built from these seven things. Get them right and the rest
of the API follows. Get one wrong and the code compiles and silently does nothing.

| Concept | What it is |
|---|---|
| **Service interface** | A `.siml` document. The contract: requests, responses, broadcasts, attributes, data types, constants. The generator turns it into a Provider base class and a Consumer base class. |
| **Component** | Your C++ class. It inherits `areg::Component` plus a generated Provider base, a Consumer base, or both. It holds the logic; it never holds the communication. |
| **Role name** | The string that names one component instance, for example `"ServiceProvider"`. This is the routing identity. It is not the class name and not the service name. |
| **Dependency** | A consumer declares the role name it wants to talk to. The string must match the provider's role name exactly. |
| **Thread** | A named dispatcher thread. Components are registered into one. All handlers of a component run on its thread, one at a time. |
| **Model** | The declarative block that wires threads, components, provided services and dependencies together. Loaded at start, unloaded at exit. |
| **Router (`mtrouter`)** | A separate process that forwards messages between processes. Needed only when provider and consumer are in different processes. |

An eighth thing is optional. When a component's behaviour depends on what happened
before, that behaviour is described in a `.fsml` document and generated the same way:
`docs/agent/22-state-machine.md`. The machine lives inside one component and is not
part of any contract; consumers see requests and broadcasts, never states.

### The relationship in one picture

```
HelloService.siml  --- generator --->  HelloServiceProviderBase   HelloServiceConsumerBase
                                                |                          |
                                       (you inherit it)           (you inherit it)
                                                |                          |
                                       class ServiceProvider      class ServiceConsumer
                                                |                          |
   BEGIN_MODEL -> BEGIN_REGISTER_THREAD("Thread1")                         |
                     BEGIN_REGISTER_COMPONENT("ServiceProvider", ...)      |
                        REGISTER_IMPLEMENT_SERVICE(...)                    |
                  BEGIN_REGISTER_THREAD("Thread2")                         |
                     BEGIN_REGISTER_COMPONENT("ServiceConsumer", ...) <----+
                        REGISTER_DEPENDENCY("ServiceProvider")
                                              ^
                                              |
                        this string must equal the provider's role name
```

### Complexity scales by repetition, not by difficulty

A fifty-service application is this same picture fifty times. There is no advanced
mode and no second API. Adding a service means: one more `.siml`, one more provider
class, one more consumer class, more rows in the model.

### Where a component runs is a deployment decision

The component code is identical in all four cases. Only the model changes.

| Deployment | What changes |
|---|---|
| One thread | Provider and consumer registered in the same `BEGIN_REGISTER_THREAD` block |
| Many threads | Registered in different thread blocks, one process, one model |
| Many processes | Two executables, two models, `Category="Public"` in the `.siml`, `mtrouter` running |
| Many machines | As multi process, with `Category="Internet"` and the router address configured |

---

## 2. What the generator produces

One `.siml` document produces a directory of files. You include two of them and
inherit from two of them. You never open the others.

| Generated file | You use it |
|---|---|
| `<Name>ProviderBase.hpp` | Inherit this in the component that provides the service |
| `<Name>ConsumerBase.hpp` | Inherit this in the component that consumes the service |
| `<Name>.hpp` | Constants, enums and structures declared in the document, plus `<Name>::ServiceName` and `<Name>::InterfaceVersion` |
| everything else | Never. Proxy, stub, events and serialization are internal. |

The generator runs from CMake through `addServiceInterface()`. It also runs standalone:

```bash
java -jar tools/codegen.jar --root=<project-root> --doc=<path.siml> --target=<generate-root>
```

Generated code is large: a 23 line `.siml` produces about 2600 lines. Do not read it
to learn the API. The naming rule in `docs/agent/20-service-interface.md` tells you
every name without opening a file.

---

## 3. Directory map

```
framework/
  areg/               the framework library, linked by every application
    appbase/          Application: setup, load_model, wait_quit, release
    base/             String, containers, threads, synchronization, file, date-time
    component/        Component, ComponentThread, ComponentLoader, Timer, dispatcher
    ipc/              sockets, serialization, connection to the router
    logging/          scoped logging used by applications
    persist/          key-value configuration storage
    system/           operating system abstraction
  aregextend/         optional helpers: console UI, service base classes
  areglogger/         client library for observing logs
  mtrouter/           message router process, required for multi-process applications
  logcollector/       collects logs from running applications
  logobserver/        reads collected logs live

examples/             32 complete applications, numbered by increasing complexity.
                      mtrouter is needed only where an example splits provider and
                      consumer into separate processes. The number does not say, and
                      neither does Category on its own: 01_minimalrpc is Public and
                      still runs in one process.
docs/
  agent/              task pages written for agents; start here
  wiki/               reference guides written for humans
  AREG_CODING_STYLE.md        style of this repository, with rationale
  AREG_AI_CODING_RULES.md     the same rules in compact normative form
tools/
  codegen.jar         the code generator
  schema/*.xsd        grammar of the .siml, .dtml and .fsml documents
  schema/rules.xml    numbered validation rules the generator reports
  setup_project.py    creates a ready-to-build project (use this one)
  setup-project.sh    the older interactive scaffolder; it prompts, so never
                      run it unattended
conf/
  cmake/              build configuration, compiler settings, helper functions
  exports/            installation templates, service unit files, package config
tests/units/          unit tests of the framework itself
```

---

## 4. Build output

**Your application.** Binaries land in `<project-root>/build/bin/`, with a platform
suffix: `.elf` on Linux, `.mac` on macOS, `.exe` on Windows. The framework libraries
and `mtrouter` are placed there too, so a multi process project needs nothing
installed.

**This repository's own build** uses a different layout, which an application never
sees:

```
product/build/<compiler>/<os>-<bits>-<arch>-<config>-<libtype>/bin/
```

The `AREG_OUTPUT_LAYOUT` option selects between them. A project created from a recipe
or by `tools/setup_project.py` turns it off, which is what puts binaries in
`build/bin/`.

Common CMake options:

| Option | Default | Meaning |
|---|---|---|
| `AREG_LIB_TYPE` | `shared` | `shared` or `static` |
| `AREG_LOGGING` | `ON` | compile logging in |
| `AREG_EXAMPLES` | `ON` | build the 32 examples; a project turns this off |
| `AREG_TESTS` | `ON` | build unit tests (fetches Google Test); a project turns this off |

Full list: `docs/wiki/02d-cmake-config.md`.

---

## 5. Changing areg itself

Not needed to build an application. The rules, the style and the platform layout are
in `docs/AREG_AI_CODING_RULES.md` and `docs/AREG_CODING_STYLE.md`; platform specific
sources live in `framework/<module>/private/{posix,win32}/`.
