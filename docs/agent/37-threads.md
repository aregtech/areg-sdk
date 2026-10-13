# Thread options, worker threads and runtime models

Read this page to choose a thread, for a plain thread of your own, a watchdog or a
different queue, or components known only at run time. Worker threads are
`38-workers.md`; `32-model.md` is the page for everything else about the model.

## 1. Thread options and the watchdog

`BEGIN_REGISTER_THREAD` has two longer forms; all three are closed by the same
`END_REGISTER_THREAD(name)`.

| Macro | Extra arguments |
|---|---|
| `BEGIN_REGISTER_THREAD(name)` | none: no watchdog, system stack size, default queue |
| `BEGIN_REGISTER_THREAD_EX(name, timeout)` | the watchdog timeout in milliseconds; `areg::WATCHDOG_IGNORE` (0) means none |
| `BEGIN_REGISTER_THREAD_EX2(name, timeout, stackSizeKb, queueSize, dropOnFull, queueWait)` | stack size in KB (0 = system default), event queue capacity (0 = default), whether an event is dropped when the queue is full, and how long a sender waits before that |

The watchdog measures how long the thread takes to process one event; a thread that
takes longer than its timeout is ended and created again, with new components that
start again. Set it well above the slowest legitimate handler. On Linux and macOS a
thread that does not return cannot be ended: it is abandoned, still running, with its
components, which are never released. The watchdog manager starts with the service
manager, so the default `setup()` runs it; `startWatchdog`, its fifth argument, matters
only where no service manager starts (`check_contract.py` `P-10`).

---

## 2. Worker or plain thread

Component threads host the components. A component's own extra thread is a worker or a
plain thread, and the design decides which. Ask of the job:

| Question | Yes means |
|---|---|
| Does it exchange messages both ways with its component or another worker? | worker |
| Must it be told things while it runs: stop, pause, a new setting? | worker |
| Must queued jobs run one at a time, in order? | worker |
| Does it run every N ms? A timer is an event, so the period is a worker's `Timer`, not a loop | worker |
| Does it run long, or many tasks one after another? Resuming a waiting worker costs less than starting a thread | worker |
| Must a stuck run be caught by the watchdog (section 1)? | worker |
| Is it a short task that runs alone, is started seldom, hands back at most its result, and must not be bound to a component? | plain thread |
| Does a library call hold the thread and never return? | plain thread |

When in doubt, a worker: it is more controllable.

A worker is declared in the model (`REGISTER_WORKER_THREAD`, or `workers` in
design.json) and lives with its component, or is created by hand and replaced on
request: `38-workers.md`. A plain thread is `areg::Thread` with an
`areg::ThreadConsumer` whose `on_run()` is the task, or `std::thread`:
`areg::Thread::wait_exit(ms)` returns false once `shutdown()` asks, `shutdown()` waits
for the thread, `start()` runs `on_run()` again, and the result goes to a component as a
custom event (`23-events.md`).

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

- [ ] Work every N ms is a timer of a worker, not a loop.
- [ ] A runtime model is added with `add_model_unique()` before `load_model()`.
