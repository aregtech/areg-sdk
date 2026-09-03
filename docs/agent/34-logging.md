# Logging from an application

AREG logs through scopes. A scope is declared once per file and entered once per
function; its messages carry its name, and it can be switched on and off by name at
run time without rebuilding.

```cpp
#include "areg/component/Timer.hpp"

#include "areg/logging/areg_log.h"

// One declaration per function that logs. The first argument groups, the second names.
DEF_LOG_SCOPE(myapp_Worker, process_timer);

void Worker::process_timer(areg::Timer & timer)
{
    LOG_SCOPE(myapp_Worker, process_timer);      // enters the scope for this call
    LOG_INFO("timer [ %s ] fired", timer.name().as_string());
}
```

`DEF_LOG_SCOPE` goes at file scope, outside any function. `LOG_SCOPE` goes as the
first line of the function and takes the same two arguments. A `LOG_*` call without a
`LOG_SCOPE` in the same function does not compile.

---

## 1. The macros

| Macro | Use |
|---|---|
| `DEF_LOG_SCOPE(group, name)` | Declares a scope. File scope, once per function that logs. |
| `LOG_SCOPE(group, name)` | Enters it. First line of the function. |
| `LOG_DBG(fmt, ...)` | Detail useful while developing |
| `LOG_INFO(fmt, ...)` | Normal progress worth keeping |
| `LOG_WARN(fmt, ...)` | Something unexpected that was handled |
| `LOG_ERR(fmt, ...)` | A failure |
| `LOG_FATAL(fmt, ...)` | A failure the application cannot continue past |
| `LOG_*_IF(cond, ...)` | The same, only when the condition holds: `LOG_DBG_IF`, `LOG_INFO_IF`, `LOG_WARN_IF`, `LOG_ERR_IF`, `LOG_FATAL_IF` |

The format string is `printf` style. An `areg::String` is passed with
`.as_string()`, never directly.

---

## 2. Cost when logging is off

Every macro tests whether its priority is enabled before building the message, and a
build with logging disabled compiles them all to nothing. A `LOG_DBG` on a hot path
is cheap, but the arguments are still evaluated, so never put work inside the call:

```cpp
LOG_DBG("state %s", expensive_dump().as_string());   // WRONG: always computed
```

---

## 3. Turning scopes on

Nothing is written until a configuration file says so. **A correct `LOG_INFO` that
prints nothing is almost always a missing or unread configuration file, not a bug in
the code.**

The framework reads `config/areg.init` **relative to the executable's own folder**,
so for a project that builds into `build/bin/` that path is
`build/bin/config/areg.init` -- inside `config/`, not beside the executable. Getting
the file there, and everything in it that is not logging, is `36-config.md`.

Copy `<areg-sdk>/framework/areg/resources/areg.init` and edit it, or write the
minimum by hand:

```ini
log::*::version         = 2.0.0
log::*::enable          = true              # master switch; false silences everything
log::*::target          = debug | file      # where output goes
log::*::enable::debug   = true              # console / debugger output
log::*::enable::file    = true
log::*::file::location  = ./logs/%appname%_%time%.log

log::*::scope::areg_*   = NOTSET ;          # silence the framework's own scopes
log::myapp::scope::*    = DEBUG | SCOPE ;   # everything in the 'myapp' executable
```
### The scope key

```
log::<module>::scope::<scope-pattern> = <priority>
```

| Part | Meaning |
|---|---|
| `<module>` | the **executable** name without extension, or `*` for every application |
| `<scope-pattern>` | a scope name, with `*` as a trailing wildcard |
| `<priority>` | `FATAL`, `ERROR`, `WARN`, `INFO`, `DEBUG`, `SCOPE`, `NOTSET`; combine with `|` |
| `;` | an **optional** value terminator. The parser strips it, so it changes nothing; the shipped file writes one on every scope line, and matching that is the tidier habit |

`DEBUG` includes the more severe priorities. `SCOPE` adds the enter/exit lines and is
normally combined: `DEBUG | SCOPE`. `NOTSET` silences, and is how you quieten a noisy
group while leaving a wildcard on.

### What a scope is called

`DEF_LOG_SCOPE(group, method)` names the scope **`group.method`** -- joined with a
dot, not an underscore. So this declaration:

```cpp
DEF_LOG_SCOPE(myapp_Worker, process_timer);
```

is addressed by any of:

```ini
log::myapp::scope::myapp_Worker.process_timer = DEBUG | SCOPE ;   # exactly this one
log::myapp::scope::myapp_Worker*              = DEBUG | SCOPE ;   # the whole class
log::myapp::scope::*                          = DEBUG | SCOPE ;   # the application
```

Turning one scope on answers "did this handler ever run". If the scope never
entered, the message never arrived and the fault is in the wiring; if it entered,
the fault is in the logic.

Layout formats and the database target: `../wiki/04a-logging-config.md`.

---

## 4. Collecting the logs of several processes

A project with this wired: `recipes/08-observability/`.

A multi-process application writes to as many consoles as it has processes.
`logcollector` gathers them into one place and `logobserver` displays them.
Both are built into the same `build/bin/` as the application, so nothing needs
installing.

This is opt-in, and it is configuration only -- **the application source does not
change.** Add the remote target to `config/areg.init`:

```ini
log::*::target            = remote | debug   # keep 'debug' to also see logs locally
log::*::enable::remote    = true
log::*::remote::service   = logger        # the section that holds the address below

logger::*::service        = logcollector
logger::*::connect        = tcpip
logger::*::enable::tcpip  = true
logger::*::address::tcpip = localhost     # the machine running logcollector
logger::*::port::tcpip    = 8282          # logcollector's port
```

`target` is a single list, not a set of lines to accumulate: writing it twice means
the last one wins. Add `remote` to the list you already have rather than pasting a
second `target` line under the one from section 3.

**8282 is logcollector. 8181 is `mtrouter`.** They are different services on
different ports; a router that is running says nothing about whether logs are being
collected.

Start `logcollector` before the applications, then `logobserver` to watch them.
Both commands, their Windows form, the `--log=db` option that persists a SQLite
`.sqlog` instead of only displaying, and the console commands that change a scope's
priority live are one table in `50-running.md`. Reading the file back, with the
schema and ready-made queries, is `35-sqlog.md`.

Two things about this path are worth knowing before it surprises you:

- `logobserver` **displays; it does not store.** Persisting is the collector's job
  and is off unless it was started with `--log=db`.
- An application whose collector is not running still runs normally: remote logging
  fails quietly and is retried. **A silent `logobserver` is not evidence that the
  application is broken** -- check that `logcollector` is up and that both sides
  agree on port 8282 first.

---

## 5. Logging versus printing

They answer different questions and both belong in an application.

| | Use |
|---|---|
| `LOG_*` | What the program did, for diagnosis. Off by default, collectable, queryable. |
| `std::cout` | The result the user or a test harness reads. Always visible. |

The recipes print with `std::cout` so a scenario can check their output. A real
application prints its result and logs its reasoning. Collected logs are plain SQLite:
`35-sqlog.md`, or `../wiki/04e-log-database-format.md`.

---

## 6. Before you move on

- [ ] Every function that logs has a `DEF_LOG_SCOPE` at file scope and a matching
      `LOG_SCOPE` as its first line.
- [ ] No `LOG_*` argument does work that would be wasted when logging is off.
- [ ] `areg::String` arguments are passed with `.as_string()`.
- [ ] Results a test checks go to `std::cout`, not to the log.
- [ ] The configuration file is at `build/bin/config/areg.init`, not beside the
      executable.
