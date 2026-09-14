# AREG cheat sheet

What an ordinary task still needs after the tools have run. The documents come from
`gen_docs.py --spec`, and the components, the model and `main()` from
`gen_skeleton.py --app`, so none of them is here: `10-new-project.md` has the layout
those tools write, and `--contract` prints the generated names of a real document in
a few hundred tokens. This page carries what no tool writes.

## Generated names, from the document

`<Name>` is `Overview/@Name`. Attribute names become `snake_case`; a method name is kept
as written after its prefix, so write it `snake_case`. Type, structure and constant
names are kept as written.

| In the `.siml` | Provider | Consumer |
|---|---|---|
| `Request foo(a)` | **must override** `void request_foo(a) final` | **calls** `request_foo(a)` |
| ... it failed | - | **may override** `void request_foo_failed(areg::ResultType) final` |
| `Response foo(b)` | **calls** `response_foo(b)` | **may override** `void response_foo(b) final` |
| `Broadcast bar(c)` | **calls** `broadcast_bar(c)` | **overrides** `void broadcast_bar(c) final`; **subscribe** `notify_on_broadcast_bar(true)` |
| `Attribute Baz` (`T`) | `set_baz(v)`, `is_baz_valid()`, `invalidate_baz()` | read `baz(state)`; **override** `void on_baz_update(T Baz, areg::DataState state) final`; **subscribe** `notify_on_baz_update(true)` |

Scalars pass by value; `String`, structures and containers as `const T &`. A constant,
an enumeration, a structure and the service name keep the name the document gives them,
reachable as `<Name>::Thing`. The authoritative table, with the transform each kind of
name goes through, is `20-service-interface.md` section 3.

## A consumer that drives a scenario

List it as `"steps"` of its interface in `design.json`: each step sends a request,
awaits a response, broadcast or attribute, or waits some milliseconds. The generator
writes the sequence, and the worksheet has one `step_<name>` check per awaiting step,
which may call `fail("why")`, `stay()` or `go_to(Step::Name)`, so a scenario that
branches fits it too. `gen_docs.py --example`
shows four steps.

## Timer and log

```cpp
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/logging/areg_log.h"

class W : public areg::Component, private areg::TimerConsumer {
    W(...) : ..., mTimer(static_cast<areg::TimerConsumer &>(self()), "WTimer")
    { mTimer.start_timer(1000, static_cast<areg::DispatcherThread &>(owner), areg::TimerBase::CONTINUOUSLY); }
    void process_timer(areg::Timer & t) final { }
    areg::Timer mTimer;
};

DEF_LOG_SCOPE(myapp_W, process_timer);            // file scope
void W::process_timer(areg::Timer & t) { LOG_SCOPE(myapp_W, process_timer); LOG_INFO("fired"); }
```

Nothing is logged until `build/bin/config/areg.init` says so -- a `config/`
subdirectory, not beside the executable. The scope above is `myapp_W.process_timer`.
**`target` turns output on**; `debug` writes to a debugger, which on POSIX is nowhere,
so use `file`.

```ini
log::*::version        = 2.0.0
log::*::enable         = true
log::*::target         = file                     # 'debug' alone prints nothing on POSIX
log::*::enable::file   = true
log::*::file::location = ./logs/%appname%.log
log::*::scope::areg_*  = NOTSET ;                 # or the framework fills the file
log::myapp::scope::*   = DEBUG | SCOPE ;          # every scope line ends with ';'
```

Collecting several processes' logs needs `logcollector` on 8282 plus the remote keys:
`34-logging.md` section 4. (`mtrouter` is a different service, on 8181.)

## Connection states

`areg::is_service_connected(s)` is true only for `Connected`.
Transient, do nothing: `Pending`, `Unknown`, `Disconnected`, `ConnectionLost`, `Failed`.
Terminal, clean up and quit: `Rejected`, `Shutdown`.

## Tools

```bash
python3 <sdk>/tools/agent/setup_project.py --name app --root ~/app --mode local
python3 <sdk>/tools/agent/gen_docs.py --template design.json    # every key, empty: fill it
python3 <sdk>/tools/agent/build_project.py --spec design.json   # documents, application,
                                                               # contract, configure, build
python3 <sdk>/tools/agent/run_scenarios.py            # exit 0 means it works; file: 50-running.md
```

On Windows the interpreter is `python` and a binary is `build\bin\name.exe`; nothing
else differs. Needs CMake 3.20+, a Java 17+ runtime, a C++17 compiler.
