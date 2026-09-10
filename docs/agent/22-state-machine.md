# State machines

Use one when the reaction to an input depends on what happened before. A service whose
handlers are a chain of `if (mPhase == ...)` is a state machine written by hand.

The machine is described in a `.fsml` document. The generator turns it into code the
same way it turns a `.siml` into a service. You write the actions, never the machine.

Working project to copy: `recipes/06-state-machine/`. One machine that builds and runs,
carrying a history marker, a guard, an internal transition, an event the machine sends
itself, `OnFinal` and a final observer. It resumes an interrupted cycle and starts a
fresh one in the same run. Read it first; this page is the lookup
for what it does not settle.

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
| `<MethodList>` `<Method MethodType="Trigger">` named `open` | `bool GateFSM::open()` -- you **call** it |
| `<MethodList>` `<Method MethodType="Action">` named `on_open` | `virtual void action_on_open() = 0` -- you **implement** it |
| an action `Parameter` `DataType="bool"` | the same parameter on `action_*` |
| `<EventList>` `<Event>` named `Ready` | `Gate::FsmEventValue::EVENT_Ready`, sent with `send_event()` |
| `<TimerList>` `<Timer>` named `Hold` | `Gate::FsmTimer::Hold`, started and stopped by the document |
| `State` named `GATE_OPEN` | an internal enumerator; the application never names a state |

`MethodType` is `Trigger`, `Action` or `Condition`, and nothing else. An event is not a
method: writing `MethodType="Event"` is refused as `error[45/RULE_BAD_VALUE]`.

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

It writes `<Name>Host.hpp/.cpp`, to merge into the providing component or use as it
stands:

```cpp
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"

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

**Do not write the XML. Describe the machine and generate it:**

```bash
python3 <areg-sdk>/tools/agent/gen_fsml.py --example > machine.json
python3 <areg-sdk>/tools/agent/gen_fsml.py --spec machine.json \
        --out src/services/Gate.fsml
```

The spec names states, triggers, timers and transitions; the tool assigns every `ID`,
resolves every `To`, and refuses a name that is not declared. Only `Overview` and
`StateList` are required, so no `Layout` block is needed; `fsml_layout.py <document>`
adds one for the editor.

This page is what a machine *means*. The rest of it still applies -- the spec has a
field for each of these -- and a document you were handed is read with the same rules.

**State names are unique across the whole document, not per level.** Every level is
flattened into one C++ enumeration, so a substate of one composite collides with a
substate of another and with the top level. A `Kind="Start"` marker counts as a state:
a nested level that also begins at one needs a different name for it -- `Start`,
`BrewStart`, `RinseStart`. A collision is `error[3/RULE_STATE_NAME]`, reported by
`check_contract.py` and refused by the generator.

**Every name a state or a transition uses is declared in its own top-level list**, all
of them direct children of `<StateMachine>` and all of them optional:

| List | Named from |
|---|---|
| `<DataTypeList>` | any `DataType` attribute; spelled as in `21-data-types.md` |
| `<AttributeList>` | `AttributeSet`, and `Attr` in a `Guard` |
| `<EventList>` | `StimulusKind="Event"`, `EventSend`, `OnFinal` |
| `<TimerList>` | `StimulusKind="Timer"`, `TimerStart`, `TimerStop` |
| `<MethodList>` | `Stimulus` of a `Trigger`, `ActionCall`, a `Guard`'s condition |
| `<ConstantList>` | `Const` in a `Guard` |
| `<StateList>` | `Transition/@To`, by `ID` |

What each list holds is `schema_help.py <Event|Timer|Method|Attribute|Constant>`.

A name used but not declared is `error[46/RULE_UNRESOLVED_ELEMENT]`, and the message
names the kind it was looked up as, which names the list it is missing from.

The machine below is smaller than the recipe's, and is shown whole so the shape is
visible at a glance.

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

`Transition/@To` names the target by `ID`, not by name, and the target must be a
**sibling**. A transition cannot reach into or out of a composite: to leave a subtree,
put the transition on the composite, whose transitions fire from anywhere inside it.
A `Kind="History"` marker is the one exception and exists for it -- a transition from
outside a composite may name a marker in that composite's `StateList`, which is how a
resume re-enters where it left off. See "Re-entering a composite where it left off".

### The pieces

| Element | Means |
|---|---|
| `Kind="Start"` | not a state, only a marker saying where a level begins, and nothing may target it. Every rule it obeys: `schema_help.py --full tStateKind` |
| `Kind="Normal"` | a state the machine occupies |
| `Kind="Final"` | the machine stops here and reports through the final observer |
| `EntryList` / `ExitList` | `ActionCall`, `TimerStart`, `TimerStop`, `EventSend`, `AttributeSet`, run on entering or leaving |
| `Kind="External"` | leaves the state, runs its exit, then the target's entry; needs `To` |
| `Kind="Internal"` | runs its operations in place; the state is not left or re-entered; no `To` |
| `Kind="Initial"` | the level's entry transition, owned only by a `Kind="Start"`; no `Stimulus`. Which of `To` and `Stimulus` each kind needs: `schema_help.py --full tTransitionKind` |
| `StimulusKind` | `Trigger`, `Timer` or `Event`; `Stimulus` is the name in that list |
| `OperationList` on a transition | runs between the exit and the entry |

A state may hold its own `StateList`. Its transitions then fire from anywhere inside
that subtree, which is how one `power_off` trigger reaches every nested state at once.

### Re-entering a composite where it left off

A composite records the substate it was left in, and a resume re-activates it.
`Shallow` restores that direct substate, whose own children then start afresh; `Deep`
restores the subtree down to the deepest state that was active. With nothing recorded
-- a first entry, or one after `release_fsm(true)` -- it descends the `Kind="Start"`
chain. `init_fsm(thread, mode)` says how the top level is entered; `release_fsm(false)`
keeps the record.

`State/@History` -- `Shallow` or `Deep` on the composite itself -- is the legacy
spelling, still read and superseded: it makes *every* entry resume. Where one entry must begin something new
and another must resume, put a marker in the composite's `StateList` and point only the
resuming transition at it:

```xml
<State ID="20" Name="MakingHistory" Kind="History" HistoryDepth="Shallow"/>
```

`To="20"` resumes; `To` naming the composite descends the Start chain. So `order`
targeting `MAKING` starts a fresh drink and `resume` targeting the marker continues the
interrupted one, in one run.

A document using a marker states `FormatVersion="1.2.0"`; one that does not stays
`1.1.0`.

**Before designing pause/resume, run `schema_help.py --full tHistoryDepth`.** It says
what a restored state does on entry, which decides where the resume actions go, and
`--full tStateKind` gives every rule a marker obeys. `--full Source` says what
`Source="Value"` pastes into the generated call.

### Leaving a level when it finishes: `OnFinal`

A `Kind="Final"` substate stops its own level, not the machine, and a transition
cannot cross out of a composite. `OnFinal` on the composite names an `Event` the
machine sends to itself when the nested level reaches Final; a transition on the
composite then carries it out of the subtree.

```xml
<State ID="10" Name="WORK" Kind="Normal" OnFinal="Done">
    <TransitionList>
        <Transition ID="30" Kind="External" StimulusKind="Event" Stimulus="Done" To="40"/>
    </TransitionList>
    <StateList>
        <State ID="31" Name="WorkStart" Kind="Start">
            <TransitionList>
                <Transition ID="32" Kind="Initial" To="33"/>
            </TransitionList>
        </State>
        <State ID="33" Name="WORK_RUNNING" Kind="Normal"> <!-- ... --> </State>
        <State ID="35" Name="WORK_DONE" Kind="Final"/>
    </StateList>
</State>
```

Without `OnFinal` a finished level simply stops and nothing follows. The nested marker
is `WorkStart` and not `Start`, because the top level already has a state of that name
and the two levels share one enumeration.

**Keep the nested `Final` empty, and run `schema_help.py --full State/@OnFinal`
before placing any operation near one.** The self-event is queued, so an operation on
the nested `Final` runs while the machine is still inside the composite; the schema
gives the ordering, where each kind of operation goes instead, and how a root-level
`Final` differs. The wrong placement is rule `108`, reported by `check_contract.py`
before the build and by the generator while generating; `explain_rule.py 108` gives the
whole rule. Working project, both kinds in one document: `recipes/06-state-machine/`.

### Reusing a whole machine: `Submachine`

A state may host another `.fsml` instead of owning a `StateList`. Import the document
and name its alias on the state. `schema_help.py --full State/@Submachine` gives the
document rules, including how the imported machine is entered.

```xml
<IncludeList>
    <Location ID="2" Name="services/Inner.fsml" Alias="Inner" Version="1.0.0"/>
</IncludeList>
...
<State ID="10" Name="RUNNING" Kind="Normal" Submachine="Inner" OnFinal="InnerDone"/>
```

The generated host carries a `static_assert` on the pinned `Version` that fails when
the import moves past it. What else changes in the generated code:

- the host's constructor takes one extra `InnerActionHandler &` per hosting state, in
  document order, so the host supplies the inner machine's actions
- the host implements `InnerFSM::FinalObserver`, and `OnFinal` turns the inner machine
  reaching Final into an event the host can transition on
- the inner machine's triggers are forwarded through the host
- `addStateMachine` is called once, naming only the importing document

Working project, hosting one machine from two states: `recipes/13-submachine/`.

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

The guard is an expression tree, not text:

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

`schema_help.py --full Guard` gives what each node is and how a guard is refused;
`schema_help.py Cmp`, `And`, `Not` give the operands each takes. `Attr` names an
`AttributeList` entry, `Param` an argument of the stimulus, and `Const` a
`<ConstantList>` entry, declared beside `AttributeList` exactly as a `.siml` declares
one: `<Constant ID="18" Name="MaxHolds" DataType="uint32" Value="3"/>`.

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
- Never target a `Kind="Start"`, nor a `Kind="History"` from inside its own level.
- Never give two states one name, however deeply apart they sit. The commonest case is
  a nested level whose `Kind="Start"` marker is also called `Start`.
- Never import a machine whose Start chain lands on a state that waits for a trigger.
  Entering the hosting state will not send one, and the machine stops there.
- Never expect `State/@History` to tell a fresh entry from a resume. It is on the state
  and applies to both; a `Kind="History"` marker is what tells them apart.
- Never give an `Internal` transition a `To`, and never leave one off an `External`.

## The spelling, when this page does not have it

`schema_help.py` answers one name out of `../../tools/schema/fsml.xsd`. Ask it; never
read the 50 KB schema.

```bash
python3 <areg-sdk>/tools/schema_help.py State --document fsml
python3 <areg-sdk>/tools/schema_help.py State/@Kind --document fsml
python3 <areg-sdk>/tools/schema_help.py --full tStateKind --document fsml
python3 <areg-sdk>/tools/schema_help.py --list --document fsml
```

The first gives where the element goes, every attribute with the values it accepts,
and the children in order; the second one attribute; `--full` on a type name adds what
the values mean; `--list` names every element. A bare attribute name (`Source`,
`MethodType`, `HistoryDepth`, `Threading`) is answered with the elements that carry it,
and `--search <word>` finds a name from a word in it.

A refused document is `explain_rule.py <number> --at <Element>/@<Attribute>`: it gives
the rule and the values that attribute accepts in one call.
