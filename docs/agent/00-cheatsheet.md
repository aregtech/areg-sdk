# AREG cheat sheet

Everything needed for an ordinary task, in one page. Open a task page only when the
answer is not here; `recipes/` holds whole projects to copy rather than read.

## The document

A `.siml` is the contract. This one exercises every construct; delete what you do not
need. `Category` is `Private` (one process), `Public` (several processes, needs
`mtrouter`) or `Internet` (other machines). Every `ID` must be unique in the file.

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<ServiceInterface FormatVersion="1.1.0">
    <Overview ID="1" Name="Monitor" Version="1.0.0" Category="Private"/>
    <AttributeList>
        <Attribute ID="10" Name="Temperature" DataType="uint32" Notify="OnChange"/>
    </AttributeList>
    <MethodList>
        <Method ID="20" Name="alarm_raised" MethodType="Broadcast">
            <ParamList>
                <Parameter ID="21" Name="level" DataType="uint32"/>
            </ParamList>
        </Method>
        <Method ID="30" Name="measure" MethodType="Request" Response="measure"/>
        <Method ID="31" Name="measure" MethodType="Response">
            <ParamList>
                <Parameter ID="32" Name="taken" DataType="bool"/>
            </ParamList>
        </Method>
    </MethodList>
    <ConstantList>
        <Constant ID="40" Name="MaxLevel" DataType="uint32" Value="90"/>
    </ConstantList>
</ServiceInterface>
```

A `Request` names its `Response` by name; a request with no reply omits the attribute.
A constant's value is the `Value` **attribute**, and only there: the 1.0.0 `<Value>`
child is refused. A structure `Field` and an `EnumEntry` are the other way round,
carrying their value as a child element. The generator validates before it writes: a
refused document names the rule in words, and
`explain_rule.py --search "words from the message"` explains it.

## Generated names, from the document

`<Name>` is `Overview/@Name`. Method and attribute names become `snake_case`; type,
structure and constant names are kept as written.

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

## Component skeletons

```cpp
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "src/services/XProviderBase.hpp"        // generated
#include "src/services/XConsumerBase.hpp"        // generated

// provider
class P final : public areg::Component, protected XProviderBase {
    P(const areg::ComponentEntry & e, areg::ComponentThread & o)
        : areg::Component(e, o), XProviderBase(static_cast<areg::Component &>(self())) {}
    void request_foo(const areg::String & a) final { /* ... */ response_foo(true); }
    // A handler must not block: no sleep, no wait, no long loop.
    inline P & self() { return (*this); }
};

// consumer
class C final : public areg::Component, protected XConsumerBase {
    C(const areg::ComponentEntry & e, areg::ComponentThread & o)
        : areg::Component(e, o), XConsumerBase(e.mDependencyServices[0].mRoleName, o) {}
    bool service_connected(areg::ServiceConnectionState s, areg::ProxyBase & p) final {
        bool r{false};
        if (XConsumerBase::service_connected(s, p)) {
            r = true;
            if (areg::is_service_connected(s)) { notify_on_baz_update(true); request_foo(role_name()); }
        }
        return r;                      // called again on every reconnect: resubscribe here
    }
    void response_foo(bool ok) final { areg::Application::signal_quit(); }
    void request_foo_failed(areg::ResultType) final {
        if (is_connected()) { request_foo(role_name()); }   // is_connected(): still usable, safe to retry
    }
};
```

`mRoleName` is an `areg::String`; passing it directly or as `.as_string()` both
compile. Do not "fix" one to match the other.

## Model and lifecycle

```cpp
#include "areg/appbase/Application.hpp"
#include "areg/base/CommonDefs.hpp"
#include "areg/component/ComponentLoader.hpp"

BEGIN_MODEL("MyModel")
  BEGIN_REGISTER_THREAD("T1")
    BEGIN_REGISTER_COMPONENT("Provider", P)
      REGISTER_IMPLEMENT_SERVICE(X::ServiceName, X::InterfaceVersion)
    END_REGISTER_COMPONENT("Provider")
  END_REGISTER_THREAD("T1")
  BEGIN_REGISTER_THREAD("T2")
    BEGIN_REGISTER_COMPONENT("Consumer", C)
      REGISTER_DEPENDENCY("Provider")          // == the role name, character for character
    END_REGISTER_COMPONENT("Consumer")
  END_REGISTER_THREAD("T2")
END_MODEL("MyModel")

areg::Application::setup();
areg::Application::load_model("MyModel");
areg::Application::wait_quit(areg::WAIT_INFINITE);   // ends on signal_quit()
areg::Application::unload_model("MyModel");
areg::Application::release();
```

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
python3 <sdk>/tools/agent/gen_skeleton.py --doc src/services/X.siml --out src
python3 <sdk>/tools/agent/check_contract.py .        # before building
python3 <sdk>/tools/agent/run_scenarios.py            # exit 0 means it works; file: 50-running.md
```

On Windows the interpreter is `python` and a binary is `build\bin\name.exe`; nothing
else differs. Needs CMake 3.20+, a Java 17+ runtime, a C++17 compiler.
