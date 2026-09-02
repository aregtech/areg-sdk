# State machines

Use one when the reaction to an input depends on what happened before. A service whose
handlers are a chain of `if (mPhase == ...)` is a state machine written by hand.

The machine is described in a `.fsml` document. The generator turns it into code the
same way it turns a `.siml` into a service. You write the actions, never the machine.

Working project to copy: `recipes/06-state-machine/`.

## What gets generated

`Gate.fsml` with `Overview/@Name="Gate"` produces four public files:

| File | Holds |
|---|---|
| `GateFSM.hpp` | the machine: one method per trigger, `init_fsm`, `release_fsm` |
| `GateActionHandler.hpp` | one **pure virtual** `action_*` per action; you implement these |
| `GateDefs.hpp` | namespace `Gate`: `FsmTimer`, `FsmEventValue`, `eHistory`, `InstanceDefaultName` |
| `Gate.fsm.files` | the manifest CMake reads |

The document name and the file name need not match. Everything follows `Overview/@Name`.

## The name contract

Names are kept exactly as written in the document; nothing is re-cased.

| In the `.fsml` | Generates |
|---|---|
| `Method` `MethodType="Trigger"` named `open` | `bool GateFSM::open()` -- you **call** it |
| `Method` `MethodType="Action"` named `on_open` | `virtual void action_on_open() = 0` -- you **implement** it |
| an action `Parameter` `DataType="bool"` | the same parameter on `action_*` |
| `Event` named `Ready` | `Gate::FsmEventValue::EVENT_Ready`, sent with `send_event()` |
| `Timer` named `Hold` | `Gate::FsmTimer::Hold`, started and stopped by the document |
| `State` named `GATE_OPEN` | an internal enumerator; the application never names a state |

The `EVENT_` prefix is added to event enumerators and to nothing else: a timer of the
same name keeps it. The two lists are not symmetrical, and assuming they are is the
usual reason a machine does not compile.

A trigger returns `bool`: `true` when a transition was taken, `false` when the current
state has no transition for it. A trigger the current state ignores is not an error.

## Wiring it into a component

Do not type this by hand. The skeleton, with every action override already in place:

```bash
python3 <areg-sdk>/tools/agent/gen_skeleton.py --doc src/services/Gate.fsml --out src
```

On Windows the command is `python`, not `python3`; nothing else changes.

It writes `<Name>Host.hpp/.cpp`. Merge it into the component that provides the
service, or use it as it stands. What it produces:

```cpp
class GateProvider final    : public    areg::Component
                            , protected GateServiceProviderBase
                            , protected GateActionHandler
{
public:
    GateProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , GateServiceProviderBase(static_cast<areg::Component &>(self()))
        , GateActionHandler()
        , mFsm(static_cast<GateActionHandler &>(self()))
    { }

protected:
    void startup_component(areg::ComponentThread & comThread) final
    {   areg::Component::startup_component(comThread); mFsm.init_fsm(&comThread); }

    void shutdown_component(areg::ComponentThread & comThread) final
    {   mFsm.release_fsm(); areg::Component::shutdown_component(comThread); }

    void request_open_gate() final      { mFsm.open(); }        // request -> stimulus
    void action_on_open() final         { broadcast_gate_changed(true); }
    void action_on_close() final        { broadcast_gate_changed(false); }

private:
    inline GateProvider & self() { return (*this); }
    GateFSM mFsm;
};
```

`init_fsm(&comThread)` binds the machine's timers and events to that dispatcher. A
machine that is never initialised accepts no stimulus and runs nothing.

**The division of work.** A request handler converts the call into a stimulus and
decides nothing. An action performs an effect and asks the machine nothing. Every
decision lives in the document.

## Writing the document

Only `Overview` and `StateList` are required. `Layout` holds the drawing coordinates
the editor uses. **Write the document without it, then generate one:**

```bash
python3 <areg-sdk>/tools/agent/fsml_layout.py src/services/Gate.fsml
```

It places every state of a level in a column, sizes each composite around its
children and joins the transitions, so the machine opens laid out instead of as a
heap of overlapping boxes. Re-running it replaces the block. Never write coordinates
by hand: it costs a great many tokens and draws worse than the tool.

Every element carries an `ID`, and the IDs are unique across the whole document.
Numbering them in reading order is enough.

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<StateMachine FormatVersion="1.1.0">
    <Overview ID="1" Name="Gate" Version="1.0.0" Threading="Local"/>
    <TimerList>
        <Timer ID="2" Name="Hold" Timeout="300" Repeat="1"/>
    </TimerList>
    <MethodList>
        <Method ID="3" Name="open"     MethodType="Trigger"/>
        <Method ID="4" Name="on_open"  MethodType="Action"/>
        <Method ID="5" Name="on_close" MethodType="Action"/>
    </MethodList>
    <StateList>
        <State ID="6" Name="Start" Kind="Start">
            <TransitionList>
                <Transition ID="7" Kind="Initial" To="8"/>
            </TransitionList>
        </State>
        <State ID="8" Name="GATE_CLOSED" Kind="Normal">
            <TransitionList>
                <Transition ID="9" Kind="External" StimulusKind="Trigger" Stimulus="open" To="10"/>
            </TransitionList>
        </State>
        <State ID="10" Name="GATE_OPEN" Kind="Normal">
            <EntryList>
                <TimerStart ID="11" Timer="Hold"/>
                <ActionCall ID="12" Action="on_open"/>
            </EntryList>
            <TransitionList>
                <Transition ID="13" Kind="External" StimulusKind="Timer" Stimulus="Hold" To="14"/>
            </TransitionList>
        </State>
        <State ID="14" Name="GATE_DONE" Kind="Final">
            <EntryList>
                <ActionCall ID="15" Action="on_close"/>
            </EntryList>
        </State>
    </StateList>
</StateMachine>
```

`Transition/@To` names the target state by its `ID`, not by its name, and that state
must be a **sibling** at the same level. A transition cannot reach into or out of a
composite: to leave a subtree, put the transition on the composite itself, whose
transitions fire from anywhere inside it.

### The pieces

| Element | Means |
|---|---|
| `Kind="Start"` | not a state, only a marker saying where a level begins; it owns exactly one `Kind="Initial"` transition and nothing may target it |
| `Kind="Normal"` | a state the machine occupies |
| `Kind="Final"` | the machine stops here and reports through the final observer |
| `EntryList` / `ExitList` | `ActionCall`, `TimerStart`, `TimerStop`, `EventSend`, `AttributeSet`, run on entering or leaving |
| `Kind="External"` | leaves the state, runs its exit, then the target's entry; needs `To` |
| `Kind="Internal"` | runs its operations in place; the state is not left or re-entered; no `To` |
| `StimulusKind` | `Trigger`, `Timer` or `Event`; `Stimulus` is the name in that list |
| `OperationList` on a transition | runs between the exit and the entry |

A state may hold its own `StateList`. Its transitions then fire from anywhere inside
that subtree, which is how one `power_off` trigger reaches every nested state at once.

### Guarding a transition

A transition can be refused unless something holds. The machine needs data of its own
to test, declared before `MethodList`:

```xml
<AttributeList>
    <Attribute ID="16" Name="Opened" DataType="bool" Value="false"/>
</AttributeList>
```

That generates `bool opened() const` and `set_opened(bool)` on the machine, and the
document can assign it wherever an `ActionCall` is allowed:

```xml
<AttributeSet ID="17" Attribute="Opened" Source="Value" Value="true"/>
```

The guard is an expression tree, not text. It hangs on the transition, after any
`Description` and before any `OperationList`:

```xml
<Transition ID="9" Kind="External" StimulusKind="Trigger" Stimulus="open" To="10">
    <Guard state="ok">
        <Expr>
            <Cmp op="eq"><Attr id="16"/><Lit>false</Lit></Cmp>
        </Expr>
    </Guard>
</Transition>
```

which generates `const bool isEligible = (mAttrOpened == false);` and takes the
transition only when it holds. A refused transition is not an error: the trigger
returns `false`, exactly as it does for a state with no transition at all.

| Node | Is |
|---|---|
| `Cmp op="eq\|ne\|lt\|le\|gt\|ge"` | exactly two operands |
| `And`, `Or` | two or more operands |
| `Not` | one operand |
| `Attr`, `Const`, `Param` | a reference, bound by the target's `ID`, never by its name |
| `Lit` | verbatim text, emitted as written |

`state="ok"` is required, and `Expr` with it. `state="draft"` means the guard is still
unfinished text, and the generator refuses the document rather than guess what it
meant. The older flat `<ConditionList>` form is read as a draft, so it is refused for
the same reason: never write one.

## Knowing when it finished

```cpp
class GateProvider : ..., private GateFSM::FinalObserver
{
    void on_fsm_final(GateFSM & /*machine*/, const char * const /*finalState*/) final
    {   areg::Application::signal_quit(); }
};
```

Register it with `mFsm.set_final_observer(this)`.

## CMake

```cmake
addServiceInterface(gen_myapp src/services/GateService.siml)
addStateMachine(gen_myapp     src/services/Gate.fsml)
```

`addStateMachine` takes the same arguments as `addServiceInterface` and picks its
generator from the extension. A machine that imports others needs only one call.

## Never

- Never keep phase state in the component beside the machine. Two sources of truth
  disagree the first time a transition is added.
- Never raise a stimulus from inside an action. The machine is already dispatching:
  it logs an error and asserts. The single exception is `send_event()`, which queues
  the event instead of dispatching it, and is the one call an action may make back
  into the machine.
- Never raise a stimulus before `init_fsm()`. That asserts as well.
- Never edit `*FSM.*`, `*ActionHandler.*` or `*Defs.*`. Change the `.fsml`.
- Never target a `Kind="Start"` state.
- Never give an `Internal` transition a `To`, and never leave one off an `External`.

## More

Full grammar: `../../tools/schema/fsml.xsd` -- its annotations state the rules the
schema itself cannot express. A large machine with nesting, history and internal
transitions: `../../examples/19_pubfsm/services/TrafficLight.fsml`.
