# Implement a service provider

A provider is your class inheriting `areg::Component` and the generated
`<Name>ProviderBase`. It implements every request and answers with a response.

Complete, working provider for the `HelloService` document in `20-service-interface.md`:

```cpp
#include <iostream>
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "src/services/HelloServiceProviderBase.hpp"

class ServiceProvider final : public    areg::Component
                            , protected HelloServiceProviderBase
{
public:
    ServiceProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , HelloServiceProviderBase(static_cast<areg::Component &>(self()))
    { }

protected:
    void request_hello_service(const areg::String & client) final
    {
        std::cout << "Hello, " << client << std::endl;
        response_hello_service(true);
    }

private:
    inline ServiceProvider & self()
    {   return (*this); }
};
```

That is the whole pattern. Everything below is detail.

---

## 1. The two fixed parts

**The constructor signature is fixed.** The framework constructs your component
through the model, and passes exactly these two arguments:

```cpp
ServiceProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
```

**The provider base takes the component itself.** The `self()` helper exists only
because the base class needs a reference to a partially constructed object:

```cpp
, HelloServiceProviderBase(static_cast<areg::Component &>(self()))
```

Copy both lines as they are. There is no other correct form.

---

## 2. What you must and may write

| Generated member | Obligation |
|---|---|
| `void request_<name>(params)` | **Must override.** It is pure virtual: the class does not compile until every request is implemented. |
| `void response_<name>(params)` | Call it to answer a request. |
| `void broadcast_<name>(params)` | Call it to notify every subscriber. |
| `void set_<attribute>(value)` | Call it to publish a new attribute value. |
| `bool is_<attribute>_valid()` | Ask whether the attribute has been set at least once. |
| `void invalidate_<attribute>()` | Mark the attribute invalid; subscribers are told. |

Names come from the rule in `20-service-interface.md` section 3. Do not open the
generated header to find them.

---

## 3. Answering a request

A request handler may answer immediately, later, or never.

```cpp
void request_read_file(const areg::String & path) final
{
    // immediately
    response_read_file(read(path));
}
```

### Answering later

To answer after the handler has returned, the request must first be **released**.
A stub holds one request at a time: while one is out, a second client's call is
refused with `RequestBusy` and never reaches the handler at all.

```cpp
void request_read_file(const areg::String & path) final
{
    const areg::SessionID session{ unblock_current_request() };  // let the next client in
    hand_to_worker(path, session);                               // returns at once
}

void on_worker_done(const areg::String & text, areg::SessionID session)
{
    if (prepare_response(session))      // false: that client is gone
    {
        response_read_file(text);
    }
}
```

`unblock_current_request()` returns the session that identifies this call; carry it
with the work and give it back to `prepare_response()` before sending the answer.
Both are `areg::StubBase` members, so a provider already has them.

**Skipping `unblock_current_request()` is a silent defect.** With one client it works
and looks correct; the second client is refused and nothing in the build says so. A
worked example is `recipes/07-worker-events/`, and `examples/24_pubunblock` answers
several clients from a timer.

**Do not block inside a handler.** Handlers run on the component's dispatcher
thread, one at a time. A sleep or a long loop inside one handler stops every
component registered in that thread. Use a `Timer` or hand the work to another
thread and answer when it finishes.

---

## 4. Attributes

An attribute is a value the provider owns and consumers subscribe to. Setting it
notifies subscribers automatically; there is nothing to send.

```cpp
set_service_provider_state(PubSub::RunState::Running);   // subscribers are notified
```

Whether a subscriber hears about every `set_` or only about changes is decided in the
document by `Notify="Always"` or `Notify="OnChange"`, not in this code.

Set every attribute once during startup. Until it is set it is invalid, and a
consumer that subscribes receives the state as invalid rather than a value.

---

## 5. One component, several services

A component may provide more than one service, and may provide and consume at the
same time. Inherit each base class:

```cpp
class Mixed final : public    areg::Component
                  , protected HelloServiceProviderBase
                  , protected StatusServiceProviderBase
                  , protected LoggerConsumerBase
```

Each provided service is declared with its own `REGISTER_IMPLEMENT_SERVICE` line in
the model. See `32-model.md`.

---

## 6. Before you move on

- [ ] Every `request_` of the document is overridden and marked `final`.
- [ ] The constructor takes `(const areg::ComponentEntry &, areg::ComponentThread &)`.
- [ ] The provider base is constructed with `static_cast<areg::Component &>(self())`.
- [ ] No handler blocks, sleeps, or loops for a long time.
- [ ] Every attribute is set at least once during startup.

Next: `32-model.md` to register the component.
