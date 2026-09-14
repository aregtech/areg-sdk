# Custom events between threads

A custom event carries one data object from the thread that sends it to a consumer
running in another dispatcher thread of the **same process**. It is how a worker
thread reports back to its component, and it is the only message the framework
delivers that no service interface describes.

**Reach for a service first.** Anything that has a caller and an answer, anything a
second process may one day need, and anything worth testing on its own belongs in a
`.siml` document: the generator writes the code, `check_contract.py` checks the use,
and the same call works across processes unchanged. Use a custom event only when the
traffic stays inside one process and has no contract worth writing: a worker thread
handing a result back, a component telling another component of its own application
that something happened.

```cpp
#include "areg/component/EventTemplate.hpp"

// 1. The data the event carries. Needs a default constructor, a copy constructor
//    and a copy assignment operator. Anything else is free.
class ReadingData
{
public:
    ReadingData() = default;
    explicit ReadingData(uint32_t value) : mValue(value) {}
    ReadingData(const ReadingData & src) = default;
    ReadingData & operator = (const ReadingData & src) = default;

    inline uint32_t value() const { return mValue; }

private:
    uint32_t mValue{ 0 };
};

// 2. One macro declares both the event class and the consumer base class.
AREG_DECLARE_EVENT(ReadingData, ReadingEvent, ReadingEventConsumer);

// 3. The receiver derives from the consumer and overrides one method.
class Collector : private ReadingEventConsumer
{
protected:
    void process_event(const ReadingData & data) final
    {
        // Runs on the thread the consumer was registered in.
    }
};
```

The macro goes in a header, once, and there is nothing to write in a `.cpp`: it
expands to complete class definitions. Put it in a header both the sender and the
receiver include.

---

## 1. Registering the consumer

Nothing is delivered until the consumer is registered, and the thread it is
registered in is the thread `process_event` runs on.

```cpp
ReadingEvent::add_listener(consumer, thread);      // areg::DispatcherThread &, or its name
ReadingEvent::remove_listener(consumer, thread);
```

Register when the owner starts and unregister when it stops. In a component that is
`startup_component` / `shutdown_component`; in a worker thread consumer it is
`register_event_consumers` / `unregister_event_consumers`, which exist for exactly
this and receive the worker thread to pass in (see `32-model.md` section 4).

## 2. Sending

```cpp
ReadingEvent::send_event(ReadingData(42));                   // to the registered consumer
ReadingEvent::send_event(ReadingData(42), consumer, thread); // to one consumer in one thread
```

`send_event` returns `bool` and returns immediately: the event is queued on the
target thread and dispatched there. It is not a call, so there is no return value to
wait for and no ordering with anything except other events of the same thread. A
`false` return means the event was not queued, normally because no consumer is
registered.

## 3. Which macro

| Macro | Use it for |
|---|---|
| `AREG_DECLARE_EVENT(Data, Event, Consumer)` | The default. The consumer is registered by hand, in any thread. |
| `AREG_DECLARE_EVENT_EX(Data, Event, Consumer, "ThreadName")` | The consumer is registered automatically in the named thread. The name must be a thread of the model. |
| `AREG_DECLARE_INTERNAL_EVENT(Data, Event, Consumer)` | Same, but queued on the dispatcher's internal queue, which is drained before any waiting service event. For work a thread schedules for itself. |
| `AREG_DECLARE_INTERNAL_EVENT_EXTENDED(Data, Event, Consumer, "ThreadName")` | The internal form with automatic registration. |

`AREG_DECLARE_EVENT` and `AREG_DECLARE_EVENT_EX` are the external forms, and external
is the right default: an external event queues behind the service traffic of its
thread instead of ahead of it.

## 4. Never

- **Never send a custom event to another process.** It has no wire format and
  `mtrouter` never sees it. Traffic that crosses a process boundary is a `Public`
  service, and nothing else.
- **Never block in `process_event`.** It is a dispatcher handler like any other, so
  it stops every component of that thread while it runs.
- **Never send before the consumer is registered.** The event is dropped and
  `send_event` returns `false`; nothing else reports it.
- **Never keep a reference to the data after `process_event` returns.** Copy what
  must outlive the call.
- **Never let a registered consumer be destroyed.** Unregister first, in the same
  thread that registered it.
- **Never use a custom event where a request belongs.** A request that fails is
  reported, retried and checked; a custom event that reaches nobody is silent.

## 5. Before you move on

- [ ] The data class has a default constructor, a copy constructor and a copy
      assignment operator.
- [ ] The `AREG_DECLARE_EVENT` line is in a header included by both sides.
- [ ] The consumer overrides `process_event(const Data &)`.
- [ ] Every `add_listener` has its `remove_listener`, in the same thread.
- [ ] Nothing is sent before the consumer is registered.
- [ ] Nothing in `process_event` blocks, sleeps or loops for long.
- [ ] The traffic stays inside one process; if it does not, it is a service.

A worked example, with a worker thread on each side: `examples/18_pubworker`.
