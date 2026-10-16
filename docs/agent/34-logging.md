# Logging from an application

areg logs through scopes, to a file, a debugger or a log collector, as its configuration
file says. It works the same in a service, a GUI application, a library and a test, and
it is how an application reports what it did.

```cpp
#include "areg/component/Timer.hpp"
#include "areg/logging/areg_log.h"

// Once per .cpp file, at file scope: every GLOBAL_* call in the file logs in it.
GLOBAL_LOG_SCOPE(myapp_Worker, log);

void Worker::process_timer(areg::Timer & timer)
{
    GLOBAL_INFO("timer [ %s ] fired", timer.name().as_string());
}
```

**`gen_skeleton.py` declares the scope in every `.cpp` it writes**: a body just logs.
**A scope is a static, created before `main()`:** one built at run time is not supported.

---

## 1. The macros

| Macro | Use |
|---|---|
| `GLOBAL_LOG_SCOPE(group, name)` | Declares the scope of a file. File scope, once per `.cpp`, never in a header |
| `GLOBAL_DBG(fmt, ...)` | Detail useful while developing |
| `GLOBAL_INFO(fmt, ...)` | Normal progress worth keeping |
| `GLOBAL_WARN(fmt, ...)` | Something unexpected that was handled |
| `GLOBAL_ERR(fmt, ...)` | A failure |
| `GLOBAL_FATAL(fmt, ...)` | A failure the application cannot continue past |

The format string is `printf` style. An `areg::String` is passed with `.as_string()`,
never directly; a `uint32_t` is `%u`, a `bool` is `%s` with `flag ? "true" : "false"`.

A scope per function instead: `DEF_LOG_SCOPE(group, method)` at file scope, and
`LOG_SCOPE(group, method)` as the function's first line; then `LOG_DBG` ... `LOG_FATAL`
and their `_IF(cond, ...)` forms, which do not compile without it. `SCOPE` priority
adds its enter and exit lines.

A disabled priority builds no message, but the arguments are still evaluated, so never
put work inside the call:

```cpp
GLOBAL_DBG("state %s", expensive_dump().as_string());   // WRONG: always computed
```

---

## 2. Where it goes: the configuration file

Nothing is written until the configuration file says so. **A correct `GLOBAL_INFO`
that writes nothing is almost always a configuration file that is missing or was not
read, not a bug in the code.**

`areg::Application::setup()` reads `config/areg.init`, or the file its sixth argument
names, relative to the executable's folder. A project made by `gen_skeleton.py` has its
own `config/project.init`, copied to `build/bin/config/` by its CMake file and named by
every generated `main()`:

```ini
log::*::version             = 2.0.0
log::*::enable              = true
log::*::target              = file
log::*::enable::file        = true
log::*::enable::remote      = false
log::*::enable::debug       = false
log::*::file::location      = ./logs/%appname%_%time%.log
log::*::layout::message     = %d: [ %t  %p >>> ] %m%n
log::*::scope::areg_*       = NOTSET ;
log::*::scope::src_services_* = NOTSET ;
log::*::scope::*            = DEBUG ;
```

So each program writes `build/bin/logs/<program>_<time>.log`, one plain-text line per
message. A key written twice keeps its first value: change a line, never add a second.
Everything in the file that is not logging is `36-config.md`.

### The scope key

```
log::<module>::scope::<scope-pattern> = <priority>
```

`<module>` is the executable name without extension, or `*`; `<scope-pattern>` a scope
name, `*` a trailing wildcard; `<priority>` `FATAL`, `ERROR`, `WARN`, `INFO`, `DEBUG`,
`SCOPE` or `NOTSET`, combined with `|`. `DEBUG` includes the more severe priorities. `SCOPE` adds the enter and exit lines of
`LOG_SCOPE`. `NOTSET` silences, which is how a noisy group is quietened under a
wildcard. `DEF_LOG_SCOPE(group, method)` and `GLOBAL_LOG_SCOPE(group, method)` name the
scope **`group.method`**, joined with a dot:

```ini
log::myapp::scope::myapp_Worker.process_timer = DEBUG | SCOPE ;   # this one
log::myapp::scope::myapp_Worker*              = DEBUG | SCOPE ;   # the whole class
```

`areg_*` are the framework's scopes, `src_services_*` the generated code's.

The layout writes `%d` time, `%t` thread, `%p` priority, `%m` the message and `%n` the
line end. Every other token and key: `../wiki/04a-logging-config.md`.

---

## 3. Reading it

A log file is plain text: `grep`, `tail` and a scenario's `expect` read it as it is
written. A log the collector stored in a SQLite `.sqlog` is queried as `35-sqlog.md`
says. Collecting the logs of several processes in one place is `logcollector`:
`50-running.md`.

A process that ends through `Application::release()` writes every line it logged; one
killed abruptly loses at most its last few milliseconds.

---

## 4. Before you move on

- [ ] Every `.cpp` that logs has one `GLOBAL_LOG_SCOPE` at file scope, or each function a
      `DEF_LOG_SCOPE` and a matching `LOG_SCOPE`.
- [ ] No argument does work that is wasted when logging is off.
- [ ] `areg::String` arguments are passed with `.as_string()`.
- [ ] The configuration file the application names is in `build/bin/config/`.
