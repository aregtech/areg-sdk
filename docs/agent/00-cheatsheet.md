# AREG cheat sheet

Everything needed for an ordinary task, in one page. Open a task page only when the
answer is not here.

## The model in seven lines

```
.siml            the contract; generates <Name>ProviderBase and <Name>ConsumerBase
Component        your class; inherits areg::Component plus one or both bases
role name        the string naming one component instance; the routing identity
dependency       a consumer names the provider's role name; must match exactly
thread           a named dispatcher; all handlers of its components run there, one at a time
model            the block wiring threads, components, services and dependencies
mtrouter         a separate process; needed only across processes
```

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
| `Constant Qux` | `<Name>::Qux` | `<Name>::Qux` |
| `Enumeration E` | `<Name>::E`, `<Name>::as_string(E)` | same |
| name and version | `<Name>::ServiceName`, `<Name>::InterfaceVersion` | same |

Scalars pass by value; `String`, structures and containers pass as `const T &`.

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
};
```

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

## Connection states

`areg::is_service_connected(s)` is true only for `Connected`.
Transient, do nothing: `Pending`, `Unknown`, `Disconnected`, `ConnectionLost`, `Failed`.
Terminal, clean up and quit: `Rejected`, `Shutdown`.

## Never

- Never edit generated code. Change the `.siml` and rebuild.
- Never invent a name on a generated base. Use the table above.
- `REGISTER_DEPENDENCY` must equal the provider's role name, character for character.
- Never call a request before `service_connected` reports connected.
- Never block in any handler; it stops every component in that thread.
- Never treat `Disconnected` / `ConnectionLost` / `Failed` as fatal.
- Never use exceptions. Return `bool`, `std::optional` or a code.

## Tools

```bash
python3 <sdk>/tools/setup_project.py --name app --root ~/app --mode local
python3 <sdk>/tools/gen_skeleton.py --doc src/services/X.siml --out src
python3 <sdk>/tools/run_scenarios.py            # exit 0 means it works
```

## When this is not enough

`05-design.md` what the services are  `20-service-interface.md` documents
`30-provider.md` providers  `31-consumer.md` consumers  `32-model.md` models
`33-timers.md` timers  `34-logging.md` logging  `21-data-types.md` structs and enums
`22-state-machine.md` state machines
`51-debug.md` failures  `recipes/` complete projects to copy
