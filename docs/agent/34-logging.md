# Logging from an application

AREG logs through scopes. A scope is declared once per file and entered once per
function; messages inside it carry its name, and a scope can be switched on and off
by name at run time without rebuilding.

```cpp
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

Logging is configured in `areg.init`, next to the executable. Scopes are addressed by
their group name, and a group can be enabled with a wildcard. The application does not
change; the file does. See `../wiki/04a-logging-config.md`.

---

## 4. Logging versus printing

They answer different questions and both belong in an application.

| | Use |
|---|---|
| `LOG_*` | What the program did, for diagnosis. Off by default, collectable, queryable. |
| `std::cout` | The result the user or a test harness reads. Always visible. |

The recipes in `recipes/` print with `std::cout` so a scenario can check
their output. A real application prints its result and logs its reasoning.

Collected logs are a plain SQLite database, so any SQLite client can query them:
`../wiki/04e-log-database-format.md`.

---

## 5. Before you move on

- [ ] Every function that logs has a `DEF_LOG_SCOPE` at file scope and a matching
      `LOG_SCOPE` as its first line.
- [ ] No `LOG_*` argument does work that would be wasted when logging is off.
- [ ] `areg::String` arguments are passed with `.as_string()`.
- [ ] Results a test checks go to `std::cout`, not to the log.
