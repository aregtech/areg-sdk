# State machines

Use one when the reaction to an input depends on what happened before. A service whose
handlers are a chain of `if (mPhase == ...)` is a state machine written by hand.

The machine is described in a `.fsml` document. The generator turns it into code the
same way it turns a `.siml` into a service. You write the actions, never the machine.

A working project that shows every piece: `recipes/06-state-machine/`. One machine that
builds and runs, carrying a history marker, a guard, an internal transition, an event
the machine sends itself, `OnFinal` and a final observer. It resumes an interrupted cycle
and starts a fresh one in the same run. A reference, not a starting point: a project
gets its machine from the `"machines"` block of `design.json`, and this page is its
lookup.

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

| In the spec | Generates |
|---|---|
| `"triggers"`: `open` | `bool GateFSM::open()` -- you **call** it |
| `"actions"`: `on_open` | `virtual void action_on_open() = 0` -- you **implement** it |
| `"conditions"`: `is_ready` | `virtual bool is_ready() = 0` -- you **implement** it |
| an action or condition `"params"` entry | the same parameter on the generated method |
| `"events"`: `Ready` | `Gate::FsmEventValue::EVENT_Ready`, sent with `send_event()` |
| `"timers"`: `Hold` | `Gate::FsmTimer::Hold`, started and stopped by the document |
| a state named `GATE_OPEN` | an internal enumerator; the application never names a state |

`gen_skeleton.py --doc <the .fsml> --contract` prints this list for a real document.

The `EVENT_` prefix is added to event enumerators and to nothing else: a timer of the
same name keeps it. The two lists are not symmetrical, and assuming they are is the
usual reason a machine does not compile.

A trigger returns `bool`: `true` when a transition was taken, `false` when the current
state has no transition for it. A trigger the current state ignores is not an error.
**It does not report acceptance.** A state that refuses through a second, unguarded
transition -- the shape `--example` shows -- takes the stimulus either way, so the
trigger returns `true` whether the guard held or not. Report the outcome from the
action the guarded transition calls -- its `response_` call -- never from the trigger.
That action runs inside the request handler that called the trigger, so the request is
still open.

**A machine's name becomes a C++ namespace**, so no class of yours may carry it. The
generated application names its components after the service (`GateServiceProvider`)
and refuses a machine of that name; a class you add must not be named after a document.

## Wiring it into a component

**Do not type this by hand, and do not write a host component.** The provider that owns
the machine is generated whole, by `01-runbook.md` section 5.

The action handler is a base of the provider, the machine is a member, `init_fsm` and
`release_fsm` are already placed, and every action is declared with a `TODO(you)`.
`init_fsm(&comThread)` binds the machine's timers and events to that dispatcher; a
machine that is never initialised accepts no stimulus and runs nothing.

**The division of work.** A request handler converts the call into a stimulus and
decides nothing. An action performs an effect and asks the machine nothing. Every
decision lives in the document. An action does not know which trigger ran it, so two
requests that answer differently each get their own action.

## Writing the document

**Do not write the XML. Describe the machine in `design.json` and generate it** with
the same build command.

The spec's `"machines"` names states, triggers, timers, events, guards and transitions;
the tool assigns every `ID`, resolves every `To`, binds every guard operand to the
declaration it means, and refuses a name that is not declared. The same spec writes the
`.siml` and the `.dtml`, so a type the service and the machine share is declared
once. Only `Overview` and `StateList` are required, so no `Layout` block is
needed; `fsml_layout.py <document>` adds one for the editor.

Everything this page describes has a field: `"submachines"` and a state's
`"submachine"`, `"kind": "history"` with `"depth"`, `"kind": "final"` with
`"final_event"`, `"conditions"` a guard calls, `"constants"`, and `{"send": "..."}` in
`"do"`. A guard is `[left, "lt", right]`, or `{"all": [...]}`, `{"any": [...]}`,
`{"not": ...}` nested as deep as it needs; each operand is a bare declared name, or
`param:`/`attr:`/`const:`/`lit:`/`raw:` when it has to be spelled out, as is a `"set"`
value: `"raw:mAttrTries + 1"` counts.

This page is what a machine *means*. The rest of it still applies -- the spec has a
field for each of these -- and a document you were handed is read with the same rules.

**State names are unique across the whole document, not per level.** Every level is
flattened into one C++ enumeration, so a substate of one composite collides with a
substate of another and with the top level. A level's start marker is a pseudo-state,
like a History marker: the machine passes through it into the state `"initial"` names.
Its name is still taken: `Start`, or `<Composite>Start` on a nested level. A collision
is `error[3/RULE_STATE_NAME]`, reported by `check_contract.py` and refused by the
generator.

**Every name a state or a transition uses is declared in a list of its own**, and all
of them are optional:

| Spec list | Named from |
|---|---|
| `"types"` | any `"type"`; spelled as in `21-data-types.md` |
| `"attributes"` | a `"set"` key, and an attribute operand of a guard |
| `"events"` | `"on"` of an event, `{"send": ...}`, `"final_event"` |
| `"timers"` | `"on"` of a timer, `"start X"` (fires `"repeat"` times, default 1) / `"stop X"` |
| `"triggers"`, `"actions"`, `"conditions"` | `"on"` of a trigger, `{"call": ...}`, a guard's `{"call": ...}` |
| `"constants"` | a constant operand of a guard |
| `"states"` | every `"to"` |

A name used but not declared is `error[46/RULE_UNRESOLVED_ELEMENT]`, and the message
names the kind it was looked up as, which names the list it is missing from.

`gen_docs.py --example` prints a whole machine in this shape -- timers, triggers,
actions, conditions, guards, a composite level and a final state. What is still yours
is the rule below.

**A transition's target must be a sibling of the state that declares it.** A transition cannot reach into or out of a composite: to leave a subtree,
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
| a transition with `"to"` another state | leaves the state, runs its exit, then the target's entry |
| a transition without `"to"`, or to its own state | runs its steps in place; the state is not left or re-entered |
| `"on"` | the trigger, timer or event that fires it, read from those lists so it is never spelled twice |
| `"do"` and `"set"` | between exit and entry, `"set"` first: a guard reads old values, `"do"` new |

`"initial"` becomes the level's start marker and the transition out of it.

A state may hold its own `"states"`. Its transitions then fire from anywhere inside
that subtree, which is how one `power_off` trigger reaches every nested state at once.

### Re-entering a composite where it left off

A composite records the substate it was left in, and a resume re-activates it.
`Shallow` restores that direct substate, whose own children then start afresh; `Deep`
restores the subtree down to the deepest state that was active. With nothing recorded
-- a first entry, or one after `release_fsm(true)` -- it descends the Start
chain. `init_fsm(thread, mode)` says how the top level is entered; `release_fsm(false)`
keeps the record.

`State/@History` -- `Shallow` or `Deep` on the composite itself -- is the legacy
spelling, still read and superseded: it makes *every* entry resume. Where one entry must begin something new
and another must resume, put a marker in the composite's `StateList` and point only the
resuming transition at it:

```json
{"name": "RunHistory", "kind": "history", "depth": "Shallow"}
```

A transition whose `"to"` is the marker resumes; one whose `"to"` is the composite
descends its Start chain. So `start` targeting `RUNNING` begins a fresh job and
`resume` targeting the marker continues the interrupted one, in one run.

A document using a marker states `FormatVersion="1.2.0"`; one that does not stays
`1.1.0`.

**A restored state re-runs its `entry`.** That is what decides where the resume
actions go: anything that must not happen twice belongs on the transition into the
marker, not on the entry of the stage being resumed.

**A phase a consumer watches is published from each state's `entry` by one action
that takes it**: `{"call": "publish_phase", "args": {"phase": "lit:<Enum>::<Value>"}}`,
whose one body sets the service attribute on the provider. A resume re-runs that entry; under
`OnChange` the consumer hears it only if the value changed in between.

### Leaving a level when it finishes: `OnFinal`

A `Kind="Final"` substate stops its own level, not the machine, and a transition
cannot cross out of a composite. `OnFinal` on the composite names an `Event` the
machine sends to itself when the nested level reaches Final; a transition on the
composite then carries it out of the subtree.

The `OPENING` state of `gen_docs.py --example` is that shape, whole: its own
`"initial"`, its substates, a `"kind": "final"` and the `"final_event"` the transition
out of it carries.

Without `OnFinal` a finished level simply stops and nothing follows. The nested marker
the tool writes for the nested level is named after the composite, because the top
level already has a `Start` and the two levels share one enumeration.

**Keep the nested `Final` empty.** The self-event runs only after the step that reached
the `Final` has settled, so an operation on the nested `Final` runs while the machine is
still inside the composite. Put the work on the transition out. The wrong placement is rule `108`, reported by `check_contract.py`
before the build and by the generator while generating; `explain_rule.py 108` gives the
whole rule. Working project, both kinds in one document: `recipes/06-state-machine/`.

### Events the machine sends itself; entering a nested state

A `{"send": ...}` or `"final_event"` runs after the step that sent it has settled and
before its trigger returns: the next trigger meets the state it led to, and on a
`Shared` machine no other thread gets in between. To go from `A` straight to `B2`
inside `B`, target `B`, send an event on that transition, and give `B`'s initial
substate a transition on it to `B2`.

### Reusing a whole machine: `Submachine`

A state may host another machine instead of owning nested states, never both. The
hosted machine is either another entry of `"machines"` in the same design, or a `.fsml`
the project already holds, named by `"path"` from the project root and not repeated in
`"machines"`:

```json
"submachines": [{"name": "Inner", "version": "1.0.0"}],
"states": [{"name": "RUNNING", "submachine": "Inner", "final_event": "RunDone"},
           {"name": "RETRYING", "submachine": "Inner", "final_event": "RetryDone"}]
```

The build command is the same.

- **Each hosting state runs its own instance**, started at its initial state on entry
  and stopped on exit. Its attributes keep their value: reset a per-visit count to
  `lit:0` inside it as visits start.
- **Its triggers are called on the host**: `mFsm.<trigger>(...)` reaches the active
  instance, `mFsm.<trigger>_<state>(...)` one state's, as the worksheet spells them. A
  host declaring the same trigger keeps only the second form.
- **Reaching its `Final` raises the host's `final_event`**, which carries no outcome:
  when the host must know how it ended, a hosted action records it and a host
  condition reads it.
- **The provider implements the hosted actions and conditions once**, beside the
  host's; one body serves every instance. What differs between places belongs to the
  host's transitions.

A hosted major version past the pinned `"version"` does not compile.

Timed, hosted from two states, run N times: `gen_docs.py --example machine`;
`recipes/13-submachine/`.

### Where a piece of data lives

Decide this once, per value, before writing the spec. Who reads it decides:

| Read by | Declared in | Reached as |
|---|---|---|
| a guard, a condition or an action of the machine | the `.fsml` `"attributes"` | `mAttrX` inside the machine, `x()` / `set_x()` on it |
| another process | the `.siml` `"attributes"` | pages 30 and 31 |
| neither: only the component that computes it | nothing. A plain C++ member | itself |

A machine attribute is a member of the machine: `x()` reads it with no parameter,
`set_x()` writes it, and a change notifies nobody.

### Guarding a transition

A transition can be refused unless something holds. The machine declares the data its
guards test before `MethodList`:

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
transition only when it holds. **Transitions that share one trigger are tried in
document order, and the first whose guard holds is the one taken**; write the
guarded ones first and the unguarded fallback last. A refused transition is not an
error: the trigger returns `false`, as it does for a state with no
transition at all. Nest with
`{"all": [...]}`, `{"any": [...]}` and `{"not": ...}`; call a declared `"conditions"`
entry with `{"call": "is_ready"}`.

## Knowing when it finished

```cpp
#include "areg/appbase/Application.hpp"

class GateProvider : ..., private GateFSM::FinalObserver
{
    void on_fsm_final(GateFSM & /*machine*/, const char * const /*finalState*/) final
    {   areg::Application::signal_quit(); }
};
```

Register it with `mFsm.set_final_observer(this)`. Where the project defines
`quit_with()`, which every scaffolded one does, that is the call here instead:
`signal_quit()` past it is reported as P-18.

## CMake

`addStateMachine(<library> <the .fsml>)`, beside the `addServiceInterface` line and
taking the same arguments. `build_project.py --spec` writes both.

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
- Never expect `State/@History` to tell a fresh entry from a resume. It is on the state
  and applies to both; a `Kind="History"` marker is what tells them apart.
- Never give an `Internal` transition a `To`, and never leave one off an `External`.
- Never leave a trigger that carries a request without a transition in a state the
  machine can be in. The stimulus is dropped, no action answers the caller, and the
  consumer waits until its deadline for a response that was never sent. A state that
  should turn the request down still needs its own transition, calling the action that
  sends the refusal.
