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

A worker thread takes the slow work of one component; they talk with custom events
(`23-events.md`). **The component adds the worker's listener before its service starts**:
an event sent earlier is dropped, `send_event()` returns false. `register_event_consumers()`
runs with no order against the first request: leave it empty.

```cpp
#include "areg/component/WorkerThreadConsumer.hpp"
class Worker : public areg::WorkerThreadConsumer, public ScanEventConsumer   // and process_event()
{
    void register_event_consumers(areg::WorkerThread &, areg::ComponentThread &) final {}
    void unregister_event_consumers(areg::WorkerThread &) final {}
};
// Provider members: Worker mWorker; areg::WorkerThread * mWorkerThread{ nullptr };
```

**Declared in the model.** The model creates the thread, then calls
`notify_thread_started()` on the component thread, before `startup_component()`:

```cpp
    REGISTER_WORKER_THREAD("ScanThread", "ScanConsumer")     // in BEGIN_REGISTER_COMPONENT

Provider::Provider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component(entry, owner), mWorker(entry.mWorkerThreads[0].mConsumerName) { }

areg::WorkerThreadConsumer * Provider::worker_thread_consumer(const areg::String & name, const areg::String & thread)
{   return name == mWorker.consumer_name() ? &mWorker : areg::Component::worker_thread_consumer(name, thread); }

void Provider::notify_thread_started(areg::WorkerThreadConsumer & consumer, areg::WorkerThread & thread)
{
    if (&consumer == &mWorker) { mWorkerThread = &thread; ScanEvent::add_listener(mWorker, thread); }
}
```

Take the consumer name from `entry.mWorkerThreads[]`, never the literal: the model
qualifies it by the role name (`P-11`). `REGISTER_WORKER_THREAD_EX` and `_EX2` take the
thread macros' watchdog and stack arguments.

**Created by hand.** No macro and no `worker_thread_consumer()`:

```cpp
void Provider::startup_component(areg::ComponentThread & owner)
{
    mWorkerThread = create_worker_thread("ScanThread", mWorker, owner);  // once it accepts events
    if (mWorkerThread != nullptr) ScanEvent::add_listener(mWorker, *mWorkerThread);
    areg::Component::startup_component(owner);                          // announces the service
}
```

Both remove the listener in `shutdown_component()` before calling the base.
`recipes/07-worker-events` is the model form, built and run as written.

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
back from its `ComponentEntry`. Working project: `recipes/10-runtime-model/`.

---

## 4. Before you move on

- [ ] A registered watchdog timeout is matched by `startWatchdog` in `setup()`.
- [ ] Every `REGISTER_WORKER_THREAD` consumer name is answered by
      `worker_thread_consumer()`, comparing against
      `entry.mWorkerThreads[..].mConsumerName` and not a literal.
- [ ] The component adds the worker's listener in `notify_thread_started()` or right after
      `create_worker_thread()`, and removes it in `shutdown_component()`.
- [ ] A runtime model is added with `add_model_unique()` before `load_model()`.
