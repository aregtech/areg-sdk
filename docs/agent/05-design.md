# Designing the application

The step before any file is written. A wrong split here is not a bug to fix later: the
documents, the components and the model are all derived from it, so it is rewritten
rather than corrected. Spend a few minutes here and the rest follows.

Answer four questions, in this order. Each has a rule, not a preference.

## 1. What are the services?

**One service per contract between two parties.** Not one per class, per entity, or
per file.

Draw the arrows between the parts of the system first. Every arrow that crosses a
responsibility boundary is a candidate service; everything inside one responsibility
is ordinary C++ and needs no document at all.

Two tests that settle most cases:

- **Who owns the state?** The owner is the provider. If neither side owns anything,
  there is no service, only a function call.
- **Would a second consumer make sense?** If yes it is a service. If the answer is
  "only this one caller, ever", it is a method.

If both sides call each other, that is **two services**, each with its own provider.
A service is one-directional by construction: consumers call requests and read
attributes, providers answer and publish. There is no bidirectional service.

## 2. What goes in the contract?

The commonest design error is putting everything in requests. Three shapes exist and
they are not interchangeable.

| Use | When | Costs |
|---|---|---|
| **Attribute** | one value a consumer must know the current state of | kept by the provider. A subscriber is sent it the moment it subscribes, and again on every `set_` (`Notify="Always"`) or only when it changes (`OnChange`, the default) |
| **Broadcast** | several values that mean something only together, at the moment it happened | fire and forget. Heard by whoever is subscribed at that instant; nothing is kept |
| **Request / Response** | a caller wants an answer to its own call | one round trip; the response goes only to the caller |

Rules that follow from the table:

- **Subscribing is not what separates them.** Neither reaches a consumer until it
  subscribes, and either can be unsubscribed again at any point while it runs.
- **What separates them is whether the value outlives the moment it was sent.** An
  attribute does: a consumer that subscribes later is still sent the current value,
  carrying a `areg::DataState` that says whether it is valid yet, so it always
  receives something. A broadcast does not: one sent before a consumer subscribed is
  gone, and that consumer waits for the next.
- **A broadcast carries as many parameters as the event needs; an attribute is one
  value.** Where several values are reported together **and** a late subscriber must
  still learn the latest, publish that one as an attribute beside the broadcast:
  `recipes/03-attributes-and-broadcast/` is that pair.
- **Only an attribute has a validity state.** Broadcast parameters are always
  meaningful, because they exist only in the message that carried them.
- A consumer that polls with a request wants an **attribute**. Attributes exist so
  round trips do not.
- A response goes to one caller. Telling everyone is a **broadcast**.
- A request needs a response only when the caller must know the outcome. A request
  with nothing to report is declared without one.

## 3. What are the components, and in which threads?

A **component** is one instance with one **role name**, and the role name is its
routing identity. A component may provide several services, consume several, or do
both at once -- the middle component in `recipes/05-two-services/` does both.

- One component per role in the system, not one per service.
- Two components with the same role name in one process is an error.
- A consumer names its provider by that role name, character for character.

A **thread** is a dispatcher. Every handler of every component in it runs there, one
at a time, in order. So the thread boundary is a **blocking** boundary, not a
performance one:

- Components that must not delay each other go in different threads.
- Anything slow -- a file, a socket, a long computation -- gets its own thread, or it
  stops every component sharing its own.
- Components that only exchange messages may share a thread. Sharing is cheaper.

Start with a thread per component. Merge later, with a reason.

## 4. How many processes?

Ask only when there is a reason to separate: independent lifetimes, isolation of a
crash, different privileges, or different machines. Otherwise use one process.

This is the choice with the highest cost, because it fixes the `Category` in every
document it touches:

| `Category` | Reach | Needs `mtrouter` |
|---|---|---|
| `Private` (the default) | inside one process | no |
| `Public` | between processes on one machine | **yes** |
| `Internet` | between machines | yes |

A `Private` service consumed from another process never connects, and nothing
reports it. Set `Category` from the process split, and set the process split first.

## Is a state machine needed?

Yes when the answer to an input depends on what came before -- a phase, a mode, a
sequence that must not be skipped. A handler that starts with `if (mPhase == ...)` is
a state machine already, written where it cannot be read.

No when each request is answered from the arguments and the stored data. Most
services are in this group.

If yes: `22-state-machine.md`. The machine lives inside one component's provider and
is not part of the contract; consumers see requests and broadcasts, never states.

## Write this table before any file

| Service | Provider role | Consumed by | Category | Contract |
|---|---|---|---|---|
| `MeterService` | `MeterProvider` | `Collector` | Private | attribute `Reading` |
| `ReportService` | `Collector` | `Display` | Private | attribute `Report`, not a broadcast: `Display` must not miss it |

Then a second table of components: role name, services provided, dependencies,
thread, process. Everything after this is mechanical -- one `.siml` per row of the
first table, one class per row of the second, and the model is the second table
written as macros.

## Worked example

> "A monitor watching three clients, and a logger."

Arrows: each client reports its state; the monitor decides something; everything
writes log lines.

- **`ClientState`**, provided by each client, consumed by the monitor. The state is a
  value that must be current, so it is an **attribute**, not a request. Three
  components provide it -- same service, three role names: `Client1`, `Client2`,
  `Client3`. One document, three providers.
- **`Alarm`**, provided by the monitor, consumed by whoever reacts. A raised alarm
  reports the client, the level and the reason together, at the moment it happened:
  a **broadcast**, with the level also an attribute for whoever connects later.
- **The logger is not a service.** Logging is a framework facility, not a contract
  between parties: `34-logging.md`. Making it a service adds a hop to every line and
  buys nothing.

Five components, four services? No -- **two** services, five components. The count of
services follows the contracts, never the parts.

Processes: one, unless the clients must survive the monitor restarting. If they must,
both documents become `Category="Public"` and `mtrouter` has to run.

## Smells

| Sign | What it usually means |
|---|---|
| A service with one request and no state | it is a function; delete the service |
| A component with fifteen requests | several services wearing one name |
| Two services that always change together | one service |
| A request whose response nobody reads | it needs no response |
| A consumer polling an attribute | subscribe instead |
| A broadcast a consumer must not miss | it is an attribute |
| An attribute holding values that only mean something together | it is a broadcast |
| A component that provides nothing and consumes nothing | it is a class, not a component |

## Then

`20-service-interface.md` to write the documents, `32-model.md` to write the model.
Or copy the nearest recipe from `recipes/` and change its contract.
