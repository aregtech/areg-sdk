# Implement a service consumer

A consumer is your class inheriting `areg::Component` and the generated
`<Name>ConsumerBase`. It calls requests and handles what comes back.

Complete, working consumer for the `HelloService` document in `20-service-interface.md`:

```cpp
#include <iostream>
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "src/services/HelloServiceConsumerBase.hpp"

class ServiceConsumer final : public    areg::Component
                            , protected HelloServiceConsumerBase
{
public:
    ServiceConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , HelloServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
    { }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final
    {
        bool result{ false };
        if (HelloServiceConsumerBase::service_connected(status, proxy))
        {
            result = true;
            if (areg::is_service_connected(status))
            {
                request_hello_service(role_name());     // first legal moment to call
            }
        }

        return result;
    }

    void response_hello_service(bool success) final
    {
        std::cout << (success ? "greeted" : "failed") << std::endl;
        areg::Application::signal_quit();
    }

    void request_hello_service_failed(areg::ResultType reason) final
    {
        if (is_connected())
        {
            request_hello_service(role_name());         // retry
        }
    }
};
```

That is the whole pattern. Everything below is detail.

---

## 1. The two fixed parts

**The constructor signature is fixed**, exactly as for a provider.

**The consumer base takes the role name of the provider it depends on**, read out of
the entry the model gave you:

```cpp
, HelloServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
```

`mDependencyServices[0]` is the first `REGISTER_DEPENDENCY` line of this component in
the model. With several dependencies, index them in the order they are declared.

---

## 2. `service_connected` is the only place to start work

A consumer must not call a request before the service is connected. The proxy exists
from construction, but there is nothing behind it yet.

The pattern above is the correct one, and its three parts all matter:

1. **Call the base first.** `HelloServiceConsumerBase::service_connected(status, proxy)`
   updates the proxy state. Skipping it leaves the consumer inconsistent.
2. **Test `areg::is_service_connected(status)`** before calling anything.
3. **Return the base's result**, so the framework knows the notification was accepted.

This method is called again on every reconnection, so it is also where you re-subscribe.

---

## 3. Connection states: which ones are fatal

`areg::ServiceConnectionState` has eight values. `areg::is_service_connected(status)`
is true for `Connected` and for nothing else.

| State | Meaning | What to do |
|---|---|---|
| `Connected` | The provider is reachable | Call requests, subscribe |
| `Pending` | Waiting for the provider | Nothing. Wait. |
| `Unknown` | State not established yet | Nothing. Wait. |
| `Disconnected` | The provider went away | **Nothing. The framework reconnects.** |
| `ConnectionLost` | The link dropped | **Nothing. The framework reconnects.** |
| `Failed` | The connection attempt failed | Nothing. The framework retries. |
| `Rejected` | Version or interface mismatch | Terminal. Report and quit. |
| `Shutdown` | The system is shutting down | Terminal. Clean up and quit. |

**Quitting on `Disconnected` or `ConnectionLost` is the most common consumer defect.**
Those states are transient. A consumer that quits on them turns a one second provider
restart into a dead application.

Starting a consumer before its provider is normal and supported. It waits.

---

## 4. What you may override and call

| Generated member | Use |
|---|---|
| `void request_<name>(params)` | **Call** it to ask the provider to do something |
| `void response_<name>(params)` | **Override** to receive the answer |
| `void request_<name>_failed(areg::ResultType reason)` | **Override** to handle a request that could not run |
| `void broadcast_<name>(params)` | **Override** to receive a broadcast |
| `void notify_on_broadcast_<name>(bool)` | **Call** to start or stop receiving that broadcast |
| `<attribute>(state)` | **Call** to read the last known attribute value |
| `void on_<attribute>_update(value, areg::DataState state)` | **Override** to receive attribute updates |
| `void notify_on_<attribute>_update(bool)` | **Call** to subscribe or unsubscribe |
| `bool is_connected()` | **Call** to test the current connection |
| `role_name()` | **Call** for this component's own role name |

---

## 5. Subscribing

Broadcasts and attribute updates arrive only after you ask for them. Subscribe inside
`service_connected`, after the connection test:

```cpp
if (areg::is_service_connected(status))
{
    notify_on_service_provider_state_update(true);
    notify_on_broadcast_reached_maximum(true);
}
```

An attribute handler is called once immediately after subscribing, then on every
update. That first call can report the value as not valid even when the provider has
already set it, so a handler must work when there is no value yet. Always check the `areg::DataState` argument: the value is meaningful only when
it is `areg::DataState::DataIsOK`. The other states are `DataIsInvalid`,
`DataIsUnavailable`, `DataUnexpectedError` and `DataIsUndefined`.

### Order of arrival

**A response has no priority over a notification.** Everything the provider sends to
you arrives in the order it sent it, so a handler that updates an attribute and then
calls its response delivers the **update first** -- the consumer sees the new attribute
value before it is told the request was accepted.

Never gate consumer logic on "the response comes before any notification". Drive the
next step from whichever message actually carries the fact you need, and accept the
first stage update arriving early.

### An update you are waiting for may never come

Under `Notify="OnChange"` -- the default -- the provider sends an update only when the
value differs from the one it holds, or when the attribute is invalid. **A step that
waits for an attribute to be set to the value it already has waits forever**, and the
symptom is a scenario that runs correctly to that point and then times out.

The two shapes that hit it are a value that returns to where it started, and a value
whose update crossed with the response that made you start waiting:

```cpp
void Consumer::response_cancel_order(bool accepted)
{
    mStep = WaitRefund;
    if (mCredit == mFullCredit)     // the refund update may already have arrived,
    {                               // and there will not be a second one
        refund_seen();
        return;
    }
    arm_watchdog();
}
```

Test the value you already hold before arming a wait on it. Where a consumer genuinely
needs every `set_` as an event rather than a value, that is what `Notify="Always"` on
the attribute is for -- `20-service-interface.md`.

---

## 6. Before you move on

- [ ] No request is called outside `service_connected` or a later handler.
- [ ] `service_connected` calls the base version first and returns its result.
- [ ] `Disconnected`, `ConnectionLost` and `Failed` do not quit the application.
- [ ] Every broadcast and attribute you handle is subscribed to.
- [ ] Attribute handlers check `areg::DataState` before using the value.
- [ ] No step waits for a response that a notification may legitimately precede.
- [ ] No step waits for an attribute update to a value the consumer already holds.
- [ ] The dependency index matches the order of `REGISTER_DEPENDENCY` in the model.

Next: `32-model.md` to register the component.
