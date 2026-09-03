# Periodic and delayed work

A component that has to do something repeatedly, or after a delay, owns a
`areg::Timer` and inherits `areg::TimerConsumer`. The timer fires on a dispatcher
thread, so the callback obeys the same rule as every handler: it must not block.

```cpp
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"

#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"

class Worker final : public  areg::Component
                   , private areg::TimerConsumer
{
public:
    Worker(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , areg::TimerConsumer()
        , mTimer(static_cast<areg::TimerConsumer &>(self()), "WorkerTimer")
    {
        mTimer.start_timer(1000, static_cast<areg::DispatcherThread &>(owner),
                           areg::TimerBase::CONTINUOUSLY);
    }

protected:
    void process_timer(areg::Timer & timer) final
    {
        // Runs every 1000 ms on the component's own thread.
    }

private:
    areg::Timer mTimer;

    inline Worker & self()
    {   return (*this); }
};
```

---

## 1. Starting a timer

| Call | Meaning |
|---|---|
| `start_timer(ms, count)` | Fires on the thread that calls it |
| `start_timer(ms, thread, count)` | Fires on the named dispatcher thread |
| `stop_timer()` | Stops it; safe whether or not it is running |

`count` is how many times it fires: `areg::TimerBase::CONTINUOUSLY` until stopped, or
`areg::TimerBase::ONE_TIME` for a single delayed call.

Prefer the three argument form inside a component and pass the owning
`ComponentThread`. It states where the callback runs instead of depending on which
thread happened to call.

---

## 2. Rules

- **One `process_timer` for all timers.** A component with several timers gets one
  callback; tell them apart with `timer.name()` or by comparing addresses.
- **Do not block in `process_timer`.** It runs on a dispatcher thread and stops every
  component of that thread while it runs.
- **Always `stop_timer()` before restarting.** A timer that expired naturally still
  reports itself as running, so an unconditional `stop_timer()` first is the only
  reliable way to restart one.
- **The timer must outlive its firing.** Keep it as a member, never a local.
- **A one-shot timer is not an alarm clock.** The callback arrives on a queue, so the
  delay is a minimum, not a guarantee.

---

## 3. What a timer is for

| Use it for | Do not use it for |
|---|---|
| Polling something that has no notification | Waiting for a response; that arrives on its own |
| Retrying after a failed request | Sleeping inside a handler |
| Publishing an attribute on a schedule | Long computation; that belongs on its own thread |
| Giving up on something that took too long | Ordering messages; the framework preserves order |

---

## 4. Before you move on

- [ ] The timer is a member of the component, not a local.
- [ ] The component inherits `areg::TimerConsumer` and overrides `process_timer`.
- [ ] `process_timer` does not block, sleep or loop for long.
- [ ] Every restart calls `stop_timer()` first.
- [ ] Timers are told apart by name when there is more than one.
