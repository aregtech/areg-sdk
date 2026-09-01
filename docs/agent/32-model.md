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

A thread is a dispatcher. Every handler of every component registered in it runs
there, one at a time, in order.

| Put components in ... | When |
|---|---|
| the same thread | They share state, or the work is light |
| different threads | One may be slow, or they must progress independently |

Two components in one thread cannot run handlers at the same time, so they need no
locking between themselves. Two components in different threads can, so shared data
needs protection.

A provider and its consumer may live in the same thread. The call still goes through
the framework and still returns asynchronously.

---

## 4. Same code, four deployments

Only the model changes. The component classes never do.

**One process, one thread:** put both components in one `BEGIN_REGISTER_THREAD` block.

**One process, several threads:** the example at the top of this page.

**Several processes:** write two `main.cpp` files, each with its own model. The
provider process registers only the provider; the consumer process registers only the
consumer and keeps its `REGISTER_DEPENDENCY`. Set `Category="Public"` in the `.siml`
and start `mtrouter` before the applications.

**Several machines:** as several processes, with `Category="Internet"` and the router
address configured. See `../wiki/03a-mtrouter.md`.

---

## 5. Application lifecycle

```cpp
areg::Application::setup();                          // start logging, routing, timers
areg::Application::load_model(_modelName);           // create threads and components
areg::Application::wait_quit(areg::WAIT_INFINITE);   // block until signal_quit()
areg::Application::unload_model(_modelName);         // stop and destroy components
areg::Application::release();                        // release everything
```

Any component ends the application by calling `areg::Application::signal_quit()`.

---

## 6. Before you move on

- [ ] Every `REGISTER_DEPENDENCY` string equals an existing role name exactly.
- [ ] Every provided service has a `REGISTER_IMPLEMENT_SERVICE` line.
- [ ] Every `BEGIN_` macro has its matching `END_` with the same argument.
- [ ] The string in `BEGIN_MODEL` is the string passed to `load_model`.
- [ ] Role names are unique within the process.
- [ ] For multi process: the `.siml` says `Public`, and `mtrouter` is running.
