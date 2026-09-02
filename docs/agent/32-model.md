# Register components: the model

The model is a declarative block that says which components exist, which thread each
one runs in, which services each provides, and which services each depends on. It is
the only thing that changes when you move a component between threads or processes.

Complete `main.cpp` for a provider and a consumer in two threads of one process:

```cpp
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "src/ServiceProvider.hpp"
#include "src/ServiceConsumer.hpp"

constexpr char const _modelName[]{ "ServiceModel" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("Thread1")
        BEGIN_REGISTER_COMPONENT("ServiceProvider", ServiceProvider)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceProvider")
    END_REGISTER_THREAD("Thread1")

    BEGIN_REGISTER_THREAD("Thread2")
        BEGIN_REGISTER_COMPONENT("ServiceConsumer", ServiceConsumer)
            REGISTER_DEPENDENCY("ServiceProvider")
        END_REGISTER_COMPONENT("ServiceConsumer")
    END_REGISTER_THREAD("Thread2")

END_MODEL(_modelName)

int main()
{
    areg::Application::setup();
    areg::Application::load_model(_modelName);
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::unload_model(_modelName);
    areg::Application::release();
    return 0;
}
```

---

## 1. The five macros

| Macro | Argument | Meaning |
|---|---|---|
| `BEGIN_MODEL(name)` | model name | Opens the model. The same string goes to `load_model`. |
| `BEGIN_REGISTER_THREAD(name)` | thread name | Opens a dispatcher thread. Any unique string. |
| `BEGIN_REGISTER_COMPONENT(role, Class)` | **role name**, class | Declares one component instance of `Class`. The macro constructs it: there is no factory function to write. |
| `REGISTER_IMPLEMENT_SERVICE(name, version)` | from the generated header | This component provides that service. |
| `REGISTER_DEPENDENCY(role)` | **the provider's role name** | This component consumes the service of that component. |

Every `BEGIN_` has a matching `END_` taking the same argument.

Components in different threads run at the same time, so `std::cout` from two handlers
interleaves and a line can arrive split. Expect it when reading output, and log instead
of printing when the order matters.

---

## 2. The role name is the routing identity

`REGISTER_DEPENDENCY("ServiceProvider")` must match
`BEGIN_REGISTER_COMPONENT("ServiceProvider", ...)` character for character.

This is the single most common defect in an AREG application, because a mismatch is
not a compile error and not a runtime error. The consumer simply never connects, and
`service_connected` is never called with `Connected`.

The role name is **not** the class name and **not** the service name:

```
"ServiceProvider"          role name      -> used by REGISTER_DEPENDENCY
ServiceProvider            class name     -> used by BEGIN_REGISTER_COMPONENT
HelloService::ServiceName  service name   -> used by REGISTER_IMPLEMENT_SERVICE
```

When several processes each run a consumer of the same provider, give each consumer
instance a unique role name with `areg::generate_name("ServiceConsumer")`, otherwise
two components claim one identity.

---

## 3. Threads

A thread is a dispatcher: every handler of every component registered in it runs
there, one at a time, in order. Put components together when they share state or the
work is light, apart when one may be slow or they must progress independently.

Components of one thread never run handlers at the same time and so need no locking
between themselves; components of different threads do. A provider and its consumer
may share a thread, and the call still returns asynchronously.

---

## 4. Thread options, worker threads, runtime models

A thread registered with `BEGIN_REGISTER_THREAD` has no watchdog, the system stack
size and the default event queue. Longer forms set all three, a component can own a
worker thread for slow work, and a model can be built at run time instead of by
macro: `37-threads.md` covers the three together.

---

## 5. Same code, four deployments

Only the model changes; the component classes never do.

| Deployment | The model |
|---|---|
| One process, one thread | both components in one `BEGIN_REGISTER_THREAD` block |
| One process, several threads | the example at the top of this page |
| Several processes | one `main.cpp` per process, each registering only its own component; the consumer keeps its `REGISTER_DEPENDENCY`. `Category="Public"` in the `.siml`, and `mtrouter` started first |
| Several machines | as several processes, with `Category="Internet"` and the router address set in `config/areg.init` (`36-config.md`) |

---

## 6. Application lifecycle

The five calls are the `main()` at the top of this page: `setup()` starts logging,
routing and timers; `load_model()` creates the threads and components;
`wait_quit()` blocks until any component calls `areg::Application::signal_quit()`;
`unload_model()` and `release()` undo the first two.

---

## 7. Before you move on

- [ ] Every `REGISTER_DEPENDENCY` string equals an existing role name exactly.
- [ ] Every provided service has a `REGISTER_IMPLEMENT_SERVICE` line.
- [ ] Every `BEGIN_` macro has its matching `END_` with the same argument.
- [ ] The string in `BEGIN_MODEL` is the string passed to `load_model`.
- [ ] Role names are unique within the process.
- [ ] For multi process: the `.siml` says `Public`, and `mtrouter` is running.
