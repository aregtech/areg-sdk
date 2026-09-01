# AREG cheat sheet

Everything needed for an ordinary task, in one page. Open a task page only when the
answer is not here.

This page holds only what you cannot derive. The seven concepts are in `CODEBASE.md`
section 1 and the prohibitions are in `AGENTS.md` section 6; neither is repeated here.

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
child is no longer read and a document still using it is refused. A structure `Field`
and an `EnumEntry` are the other way round, carrying their value as a child element.
The generator validates before it writes: a refused document reports a rule number,
and `explain_rule.py <number>` says what it means.

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

Scalars pass by value; `String`, structures and containers pass as `const T &`. A
constant, an enumeration, a structure and the service name all keep the name the
document gives them, reachable as `<Name>::Thing`. The full table, including the
transform each kind of name goes through, is `20-service-interface.md` section 3;
it is the authoritative copy and this one is the short form of it.

## Component skeletons

```cpp
// provider
class P final : public areg::Component, protected XProviderBase {
    P(const areg::ComponentEntry & e, areg::ComponentThread & o)
        : areg::Component(e, o), XProviderBase(static_cast<areg::Component &>(self())) {}
    void request_foo(const areg::String & a) final { /* ... */ response_foo(true); }
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
compile, and the recipes and examples use each. Do not "fix" one to match the other.

## Model and lifecycle

```cpp
BEGIN_MODEL("MyModel")
  BEGIN_REGISTER_THREAD("T1")
    BEGIN_REGISTER_COMPONENT("Provider", P)
      REGISTER_IMPLEMENT_SERVICE(X::ServiceName, X::InterfaceVersion)
    END_REGISTER_COMPONENT("Provider")
  END_REGISTER_THREAD("T1")
  BEGIN_REGISTER_THREAD("T2")
    BEGIN_REGISTER_COMPONENT("Consumer", C)
      REGISTER_DEPENDENCY("Provider")          // == the provider's role name
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
subdirectory, not beside the executable. The scope above is named `myapp_W.process_timer`:

```ini
log::*::enable        = true
log::*::enable::debug = true
log::myapp::scope::*  = DEBUG | SCOPE ;           # every scope line ends with ';'
```

Collecting several processes' logs needs `logcollector` on port 8282 plus the remote
keys: `34-logging.md` section 4. (`mtrouter` is a different service on 8181.)

## Connection states

`areg::is_service_connected(s)` is true only for `Connected`.
Transient, do nothing: `Pending`, `Unknown`, `Disconnected`, `ConnectionLost`, `Failed`.
Terminal, clean up and quit: `Rejected`, `Shutdown`.

## Never

The full list is `AGENTS.md` section 6. The two that cost the most time:
`REGISTER_DEPENDENCY` must equal the provider's role name character for character,
and no handler may block.

## Tools

```bash
python3 <sdk>/tools/setup_project.py --name app --root ~/app --mode local
python3 <sdk>/tools/gen_skeleton.py --doc src/services/X.siml --out src
python3 <sdk>/tools/check_contract.py .        # before building
python3 <sdk>/tools/run_scenarios.py            # exit 0 means it works
```

On Windows the interpreter is `python`, not `python3`, and a binary is
`build\bin\name.exe` rather than `./build/bin/name.elf`. Nothing else differs.
Needed to build at all: CMake 3.20+, a Java 17+ runtime, a C++17 compiler.

## When this is not enough

`05-design.md` what the services are  `20-service-interface.md` documents
`30-provider.md` providers  `31-consumer.md` consumers  `32-model.md` models
`33-timers.md` timers  `34-logging.md` logging  `21-data-types.md` structs and enums
`22-state-machine.md` state machines
`40-base-api.md` String, containers, files, threads  `41-examples.md` which example shows what
`51-debug.md` failures  `recipes/` complete projects to copy
