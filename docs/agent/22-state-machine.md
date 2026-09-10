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

**A machine's name becomes a C++ namespace**, so no class of yours may carry it. The
generated application names its component `ServiceProvider` and never collides; a class
you add by hand must not be named after either document.

## Wiring it into a component

**Do not type this by hand, and do not write a host component.** The provider that owns
the machine is generated whole:

```
python3 <areg-sdk>/tools/agent/gen_skeleton.py --doc src/services/GateService.siml \
        --machine src/services/Gate.fsml --app --mode ipc --force
```

The action handler is a base of the provider, the machine is a member, `init_fsm` and
`release_fsm` are already placed, and every action is declared with a `TODO(you)`.
On Windows the interpreter is `python`, not `python3`; nothing else changes.
`init_fsm(&comThread)` binds the machine's timers and events to that dispatcher; a
machine that is never initialised accepts no stimulus and runs nothing.

**The division of work.** A request handler converts the call into a stimulus and
decides nothing. An action performs an effect and asks the machine nothing. Every
decision lives in the document.

## Writing the document

**Do not write the XML. Describe the machine and generate it:**

```bash
python3 <areg-sdk>/tools/agent/gen_docs.py --example > design.json
python3 <areg-sdk>/tools/agent/gen_docs.py --spec design.json --outdir src/services
```

The spec's `"machines"` names states, triggers, timers, events, guards and transitions;
the tool assigns every `ID`, resolves every `To`, binds every guard operand to the
declaration it means, and refuses a name that is not declared. The same spec writes the
`.siml` and the `.dtml`, so a type or an attribute the service and the machine share is
declared once. Only `Overview` and `StateList` are required, so no `Layout` block is
needed; `fsml_layout.py <document>` adds one for the editor.

Everything this page describes has a field: `"submachines"` and a state's
`"submachine"`, `"kind": "history"` with `"depth"`, `"kind": "final"` with
`"final_event"`, `"conditions"` a guard calls, `"constants"`, and `{"send": "..."}` in
`"do"`. A guard is `[left, "lt", right]`, or `{"all": [...]}`, `{"any": [...]}`,
`{"not": ...}` nested as deep as it needs; each operand is a bare declared name, or
`param:`/`attr:`/`const:`/`lit:`/`raw:` when it has to be spelled out.

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

Each is a list in the spec: `"events"`, `"timers"`, `"triggers"` and `"actions"`,
`"attributes"`, `"constants"`.

A name used but not declared is `error[46/RULE_UNRESOLVED_ELEMENT]`, and the message
names the kind it was looked up as, which names the list it is missing from.

The machine below is smaller than the recipe's, and is shown whole so the shape is
visible at a glance. This is what you write; the XML is what `gen_docs.py` writes.

```json
{"machines": [{
  "name": "Gate",
  "timers":   [{"name": "Hold", "timeout": 300}],
  "triggers": [{"name": "open"}],
  "actions":  [{"name": "on_open"}, {"name": "on_close"}],
  "initial": "GATE_CLOSED",
  "states": [
    {"name": "GATE_CLOSED", "transitions": [{"on": "open", "to": "GATE_OPEN"}]},
    {"name": "GATE_OPEN", "entry": ["start Hold", "on_open"],
     "transitions": [{"on": "Hold", "to": "GATE_DONE"}]},
    {"name": "GATE_DONE", "kind": "final", "entry": ["on_close"]}
  ]
}]}
```

The `Kind="Start"` marker, every `ID`, and the `To` of every transition are the tool's
work. What is still yours is the rule below.

**A transition's target must be a sibling.** A transition cannot reach into or out of a composite: to leave a subtree,
put the transition on the composite, whose transitions fire from anywhere inside it.
A `Kind="History"` marker is the one exception and exists for it -- a transition from
outside a composite may name a marker in that composite's `StateList`, which is how a
resume re-enters where it left off. See "Re-entering a composite where it left off".

### The pieces

| In the spec | Means |
|---|---|
| `"kind": "normal"`, the default | a state the machine occupies |
| `"kind": "final"` | the level stops here and reports through the final observer |
| `"entry"` / `"exit"` | steps run on entering or leaving: an action, `start`/`stop <Timer>`, `send <Event>` |
| a transition with `"to"` | leaves the state, runs its exit, then the target's entry |
| a transition without `"to"` | runs its steps in place; the state is not left or re-entered |
| `"on"` | the trigger, timer or event that fires it, read from those lists so it is never spelled twice |
| `"do"` and `"set"` | run between the exit and the entry; `"set"` first, so an action sees it |

`"initial"` becomes the level's `Kind="Start"` marker and the transition out of it.

A state may hold its own `"states"`. Its transitions then fire from anywhere inside
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

```json
{"name": "MakingHistory", "kind": "history", "depth": "Shallow"}
```

A transition whose `"to"` is the marker resumes; one whose `"to"` is the composite
descends its Start chain. So `order` targeting `MAKING` starts a fresh drink and
`resume` targeting the marker continues the interrupted one, in one run.

A document using a marker states `FormatVersion="1.2.0"`; one that does not stays
`1.1.0`.

**A restored state re-runs its `entry`.** That is what decides where the resume
actions go: anything that must not happen twice belongs on the transition into the
marker, not on the entry of the stage being resumed.

### Leaving a level when it finishes: `OnFinal`

A `Kind="Final"` substate stops its own level, not the machine, and a transition
cannot cross out of a composite. `OnFinal` on the composite names an `Event` the
machine sends to itself when the nested level reaches Final; a transition on the
composite then carries it out of the subtree.

```json
{"name": "WORK", "final_event": "Done", "initial": "WORK_RUNNING",
 "transitions": [{"on": "Done", "to": "NEXT"}],
 "states": [{"name": "WORK_RUNNING"}, {"name": "WORK_DONE", "kind": "final"}]}
```

Without `OnFinal` a finished level simply stops and nothing follows. The nested marker
the tool writes for the nested level is named after the composite, because the top
level already has a `Start` and the two levels share one enumeration.

**Keep the nested `Final` empty.** The self-event is queued, so an operation on the
nested `Final` runs while the machine is still inside the composite, and a request
arriving in between meets a machine that has not left it yet. Put the work on the
transition out. The wrong placement is rule `108`, reported by `check_contract.py`
before the build and by the generator while generating; `explain_rule.py 108` gives the
whole rule. Working project, both kinds in one document: `recipes/06-state-machine/`.

### Reusing a whole machine: `Submachine`

A state may host another `.fsml` instead of owning nested states: it carries one or
the other, never both. The hosting state is entered through the imported machine's own
`Start` chain, and **nothing calls the imported machine's triggers**, so an inner
machine whose first state waits for a trigger stops there. Pair it with `final_event`.

```json
"submachines": [{"name": "Inner", "path": "src/services/Inner.fsml", "version": "1.0.0"}],
"states": [{"name": "RUNNING", "submachine": "Inner", "final_event": "InnerDone"}]
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

```json
"attributes": [{"name": "Opened", "type": "bool", "value": "false"}],
"constants":  [{"name": "MaxHolds", "type": "uint32", "value": "3"}]
```

That generates `bool opened() const` and `set_opened(bool)` on the machine, and a
transition assigns it with `"set": {"Opened": "lit:true"}` wherever an action is
allowed. The guard is an expression tree, not text:

```json
{"on": "open", "to": "GATE_OPEN", "guard": ["Opened", "eq", "lit:false"]}
```

which generates `const bool isEligible = (mAttrOpened == false);` and takes the
transition only when it holds. A refused transition is not an error: the trigger
returns `false`, exactly as it does for a state with no transition at all. Nest with
`{"all": [...]}`, `{"any": [...]}` and `{"not": ...}`; call a declared `"conditions"`
entry with `{"call": "is_ready"}`.

An operand that is a bare declared name is that declaration -- an attribute, a
constant, or a parameter of the stimulus -- and anything else is a literal. Force one
with `attr:`, `const:`, `param:`, `lit:` or `raw:<c++>`. A refused guard is not an
error -- the trigger returns `false`.

## Knowing when it finished

```cpp
#include "areg/appbase/Application.hpp"

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
