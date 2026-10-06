# Worker threads

Read this page when a component has work too slow for a handler, or a source of its own:
a device read every N ms, a blocking read, a long computation. A worker thread belongs to
one component and stops with it. Thread options, the watchdog and a model built at run
time are `37-threads.md`.

## 1. The rules

- **A worker talks by custom events and timers only** (`23-events.md`). It never calls the
  component's members; state both touch needs a lock (`42-runtime-api.md` section 8).
- **Its consumer.** Every worker is bound to an `areg::WorkerThreadConsumer`, named; the
  component returns it from `worker_thread_consumer()`. One consumer may serve several
  workers: the `areg::WorkerThread &` each call passes tells them apart.
- **The component adds every listener a worker receives on**, in `notify_thread_started()`
  (declared in the model) or right after `create_worker_thread()` (by hand), before the
  service is announced; and removes it in `shutdown_component()` before calling the base.
  An event sent before its listener exists is dropped: `send_event()` returns false.
- **The component's own listeners**, for what a worker sends it, are added in its
  constructor: the component thread accepts events before it constructs its components,
  and no worker exists yet.
- **`register_event_consumers()` / `unregister_event_consumers()` are optional.** They run
  on the worker thread as it starts and stops: start the worker's timers there, or run a
  loop of its own that blocks on its source. Never add a listener there.

## 2. Declared in the model

The model creates the thread, then calls `notify_thread_started()` on the component
thread, before `startup_component()`:

```cpp
#include "areg/component/ComponentLoader.hpp"       // the model macros
#include "areg/component/Model.hpp"                 // areg::ComponentEntry
#include "areg/component/Component.hpp"
#include "areg/component/WorkerThreadConsumer.hpp"

    REGISTER_WORKER_THREAD("ScanThread", "ScanConsumer")     // in BEGIN_REGISTER_COMPONENT

Provider::Provider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component(entry, owner), mWorker(entry.mWorkerThreads[0].mConsumerName)
{   ResultEvent::add_listener(static_cast<ResultEventConsumer &>(*this), owner); }

areg::WorkerThreadConsumer * Provider::worker_thread_consumer(const areg::String & name, const areg::String & thread)
{   return name == mWorker.consumer_name() ? &mWorker : areg::Component::worker_thread_consumer(name, thread); }

void Provider::notify_thread_started(areg::WorkerThreadConsumer & consumer, areg::WorkerThread & thread)
{
    if (&consumer == &mWorker) { mWorkerThread = &thread; ScanEvent::add_listener(mWorker, thread); }
}
// Members: Worker mWorker; areg::WorkerThread * mWorkerThread{ nullptr };
```

Take the consumer name from `entry.mWorkerThreads[]`, never the literal: the model
qualifies it by the role name (`P-11`). `REGISTER_WORKER_THREAD_EX` and `_EX2` take the
thread macros' watchdog and stack arguments (`37-threads.md` section 1).

## 3. Created by hand

No macro and no `worker_thread_consumer()`. `create_worker_thread()` returns once the
worker accepts events, or `nullptr`:

```cpp
void Provider::startup_component(areg::ComponentThread & owner)
{
    mWorkerThread = create_worker_thread("ScanThread", mWorker, owner);
    if (mWorkerThread != nullptr) ScanEvent::add_listener(mWorker, *mWorkerThread);
    areg::Component::startup_component(owner);                  // announces the service
}
```

`shutdown_component()` removes the listener, calls the base, which stops every worker of
the component, then `delete_worker_thread("ScanThread")`.

## 4. Who sends to whom

| Direction | Listener added | Runs on |
|---|---|---|
| component -> worker | by the component, for the worker's thread (sections 2, 3) | the worker |
| worker -> component | by the component, in its constructor | the component thread |
| worker -> worker, one component | by the component, for the receiving worker's thread | the receiving worker |

`send_event(data)` with no thread reaches the thread whose listener was added for that
event: from a worker it looks at the worker's own thread, then the component thread, then
the component's other workers.

## 5. Work every N ms

A worker that is also an `areg::TimerConsumer` starts its timer on its own thread, so
`process_timer()` runs there. No listener is needed for a timer.

```cpp
#include "areg/component/Timer.hpp"

class Reader final : public areg::WorkerThreadConsumer, public areg::TimerConsumer
{
public:
    explicit Reader(const areg::String & name) : areg::WorkerThreadConsumer(name), mTick(*this, "Tick") {}
protected:
    void register_event_consumers(areg::WorkerThread &, areg::ComponentThread &) override
    {   mTick.start_timer(100); }                  // on the worker's own thread, until stopped
    void unregister_event_consumers(areg::WorkerThread &) override
    {   mTick.stop_timer(); }
    void process_timer(areg::Timer &) override
    {   ReadingEvent::send_event(ReadingData{ /* what was read */ }); }   // to the component
private:
    areg::Timer mTick;
};
```

A worker that blocks on its source instead runs its loop in `register_event_consumers()`
and returns to stop; it dispatches no event and no timer while the loop runs, and the
component's shutdown waits until it returns: `examples/18_pubworker` (`pubservice`).

## 6. Before you move on

- [ ] Every `REGISTER_WORKER_THREAD` consumer name is answered by `worker_thread_consumer()`,
      comparing against `entry.mWorkerThreads[..].mConsumerName`.
- [ ] Every listener a worker receives on is added by the component in
      `notify_thread_started()` or right after `create_worker_thread()`, and removed in
      `shutdown_component()`; none in `register_event_consumers()`.
- [ ] The component's listener for what a worker sends is added in its constructor.
- [ ] A worker's timer is started on the worker thread and stopped in
      `unregister_event_consumers()`.

Working project: `recipes/07-worker-events/`.
