# Thread options, worker threads and runtime models

Three things the plain model macros do not reach. Read this page when a thread needs
a watchdog or a different queue, or when the components are known only at run time;
worker threads are `38-workers.md`. `32-model.md` is the page for
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

A component's work too slow for a handler, or with a source of its own, goes to a worker
thread: `38-workers.md`.

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
- [ ] A runtime model is added with `add_model_unique()` before `load_model()`.
