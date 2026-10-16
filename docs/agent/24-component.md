# Component: what it owns and how

Read this page when one component implements several interfaces, holds a
provider and a consumer together, or shares a state machine between objects. One
interface per component: `30-provider.md`, `31-consumer.md`.

## 1. Who owns what

The component is the service; its role name is the address of every interface it
implements (`05-design.md` section 1). A component `OneTwo` implementing `One` and `Two`
is reached as `OneConsumerBase("OneTwo", owner)` and `TwoConsumerBase("OneTwo", owner)`.

| Owner | Owns | How |
|---|---|---|
| `areg::ComponentThread` | its components | creates each one the model registers in it when it starts, deletes it when it stops; every handler of its components runs on it |
| `areg::Component` | its worker threads | declared with `REGISTER_WORKER_THREAD`, or `create_worker_thread()` in `startup_component()`; `Component::shutdown_component()` stops them (`38-workers.md`) |
| `areg::Component` | its providers | each `<Service>ProviderBase` takes the component in its constructor and registers with it; `Component::startup_component()` announces every one |
| `areg::Component` | its consumers | each `<Service>ConsumerBase` takes a role and the component, or its thread, and connects to that role |
| `areg::Component` | its state machines and data | ordinary members; a machine has one owner, and any object the component holds may use it by reference |

## 2. Two shapes

**The component is the provider.** It inherits `areg::Component` and every base
(`30-provider.md` section 5). This is the shape the generator writes.

**The providers are members.** Each provider is its own class; the component holds
them. Use it when two bases carry the same method or attribute name, or when several
services share one state machine.

```cpp
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"

class OneProvider final : public OneServiceProviderBase
{
public:
    OneProvider(areg::Component & owner, UnitFSM & fsm)
        : OneServiceProviderBase(owner), mFsm(fsm) {}
protected:
    void request_start(uint32_t target) override { mFsm.start(target); }
private:
    UnitFSM & mFsm;
};

class Unit final : public areg::Component, protected UnitActionHandler
{
public:
    Unit(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , mFsm(static_cast<UnitActionHandler &>(*this))
        , mOne(*this, mFsm)
        , mTwo(*this, mFsm)
    {}
    void startup_component(areg::ComponentThread & thread) override
    {
        mFsm.init_fsm(&thread);
        areg::Component::startup_component(thread);   // announces mOne and mTwo
    }
    void shutdown_component(areg::ComponentThread & thread) override
    {
        mFsm.release_fsm();
        areg::Component::shutdown_component(thread);
    }
private:
    UnitFSM     mFsm;   // declared before the providers that hold it
    OneProvider mOne;
    TwoProvider mTwo;
};
```

Members are built in declaration order: the machine before the objects that take it,
or they hold a reference to an object not yet built; `init_fsm()` before
`areg::Component::startup_component()`. Pass `*this`, never `self()`, to a base (`P-19`). The model registers `Unit`
once, with one `REGISTER_IMPLEMENT_SERVICE` per provider. A consumer is a member the
same way, `<Service>ConsumerBase(role, owner)`: the generated clients of a provider
that uses another service already are.

## 3. Where a state machine lives

Which service owns a machine is a design decision, like which object owns a variable:
the service that controls what the machine describes. In `design.json` that service
names it, `"machine": "<name>"`; its provider then holds the machine and its actions
are worksheet sections. `design.json` gives one machine to one provider. A machine
that two services of one component drive is still declared under `"machines"`, and
its FSM and action handler are generated; the component and the providers that share
it are written by hand in the shape above, the machine passed to both by reference.

**No trigger from inside an action, through any holder.** An `action_` body that calls
`mOne.do_one()`, where `do_one()` calls `mFsm.one()`, raises a trigger of a machine
mid-transition, its state not yet set: Debug asserts, Release runs it on an unfinished
transition. The rule of `22-state-machine.md`
"Never" holds whoever makes the call; `send_event()` queues an event instead.
