# Thread options, worker threads and runtime models

Three things the plain model macros do not reach. Read this page when a thread needs
a watchdog or a different queue, when a component has work too slow for a handler, or
when the components are known only at run time. `32-model.md` is the page for
everything else about the model.

## 1. Thread options and the watchdog

`BEGIN_REGISTER_THREAD` has two longer forms; all three are closed by the same
`END_REGISTER_THREAD(name)`.

| Macro | Extra arguments |
|---|---|
| `BEGIN_REGISTER_THREAD(name)` | none: no watchdog, system stack size, default queue |
| `BEGIN_REGISTER_THREAD_EX(name, timeout)` | the watchdog timeout in milliseconds; `areg::WATCHDOG_IGNORE` (0) means none |
| `BEGIN_REGISTER_THREAD_EX2(name, timeout, stackSizeKb, queueSize, dropOnFull, queueWait)` | stack size in KB (0 = system default), event queue capacity (0 = default), whether an event is dropped when the queue is full, and how long a sender waits before that |

The watchdog measures how long the thread takes to process one event; a thread that
takes longer than its timeout is terminated and restarted. Set it well above the
slowest legitimate handler.

**A timeout on its own does nothing.** The watchdog service is off by default:
`areg::Application::setup()` takes `startWatchdog` as its fifth argument and it
defaults to `false`, so `setup(true, true, true, true, true)` is what turns a
registered timeout into a running guard. `check_contract.py` reports the mismatch as
`P-10`.

---

## 2. Worker threads

"Never block in a handler" leaves the question of where slow work goes. A worker
thread is the framework's answer: it belongs to one component, is declared in the
model beside it, and runs a consumer the component hands over when asked.

```cpp
#include "areg/component/ComponentLoader.hpp"

BEGIN_REGISTER_COMPONENT("Provider", Provider)
    REGISTER_IMPLEMENT_SERVICE(X::ServiceName, X::InterfaceVersion)
    REGISTER_WORKER_THREAD("ScanThread", "ScanConsumer")
END_REGISTER_COMPONENT("Provider")
```

The second argument is a **consumer name**, and the component must answer to it:

```cpp
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"

class Worker : public areg::WorkerThreadConsumer      // a member of the component
{
public:
    explicit Worker(const areg::String & name) : areg::WorkerThreadConsumer(name) {}
protected:
    void register_event_consumers(areg::WorkerThread & work,
                                  areg::ComponentThread & master) final;   // subscribe here
    void unregister_event_consumers(areg::WorkerThread & work) final;
};

Provider::Provider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component(entry, owner)
    , mWorker(entry.mWorkerThreads[0].mConsumerName)     // not the literal
{ }

areg::WorkerThreadConsumer * Provider::worker_thread_consumer(
        const areg::String & consumerName, const areg::String & workerThreadName)
{
    return mWorker.consumer_name() == consumerName
         ? &mWorker
         : areg::Component::worker_thread_consumer(consumerName, workerThreadName);
}
```

**Take the name from `entry.mWorkerThreads[]`, never from the literal you passed to
the macro.** The model stores it qualified by the role name, so a consumer built from
the bare literal never matches, the thread runs with no consumer, and every event
sent to it is dropped in silence. `check_contract.py` reports a name nothing answers
to as `P-11`. `REGISTER_WORKER_THREAD_EX` and `_EX2` take the same watchdog and stack
arguments as the thread macros.

The component and its worker thread talk with custom events: `23-events.md`, and
`examples/18_pubworker` shows both sides.

---

## 3. Building a model at run time

The macros are the default. Build a model by hand only when the components are known
too late for them -- a count taken from the command line, a model loaded on demand.
The classes are the same ones the macros fill in.

```cpp
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

areg::Application::setup();

areg::Model model("Runtime");
areg::ComponentThreadEntry & thread = model.add_thread("RuntimeThread");
areg::ComponentEntry & entry = thread.add_component<Consumer>(roleName);
entry.add_dependency_service("Provider");

areg::ComponentLoader::add_model_unique(model);
areg::Application::load_model(nullptr);      // nullptr: every model added so far
```

`add_component<T>(roleName)` writes the create and delete functions for you.
`add_supported_service(name, version)` is the runtime form of
`REGISTER_IMPLEMENT_SERVICE`, and `set_data()` passes a `std::any` the component reads
back from its `ComponentEntry`. Working example:
`examples/17_pubtraffic/pubclient/src/main.cpp`.

---

## 4. Before you move on

- [ ] A registered watchdog timeout is matched by `startWatchdog` in `setup()`.
- [ ] Every `REGISTER_WORKER_THREAD` consumer name is answered by
      `worker_thread_consumer()`, comparing against
      `entry.mWorkerThreads[..].mConsumerName` and not a literal.
- [ ] The worker thread subscribes in `register_event_consumers` and unsubscribes in
      `unregister_event_consumers`.
- [ ] A runtime model is added with `add_model_unique()` before `load_model()`.
