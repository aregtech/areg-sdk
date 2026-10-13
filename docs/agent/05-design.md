# Designing the application

The step before any file is written. The documents, the components and the model all
derive from this split, so a wrong one is rewritten, not corrected.

Answer four questions, in this order. Each has a rule, not a preference.

## 1. What are the service interfaces?

A **service interface** (`.siml`) is a contract; it provides nothing. A **service**
is a component that implements one or more interfaces under its role name. A consumer
knows the service's role name and the interface it uses. Several services, each with
its own role name, may implement one interface.

**One interface per contract between two parties**, not per class, entity or file.

Draw the arrows between the parts of the system first. Every arrow that crosses a
responsibility boundary is a candidate interface; everything inside one is ordinary
C++ and needs no document.

Two tests that settle most cases:

- **Who owns the state?** The owner is the provider. If neither side owns anything,
  there is no interface, only a function call.
- **Would a second consumer make sense?** If yes it is an interface. If the answer is
  "only this one caller, ever", it is a method.

If both sides call each other, that is **two interfaces**, one provided by each side.
An interface is one-directional by construction: consumers call requests and read
attributes, providers answer and publish.

## 2. What goes in the contract?

The commonest design error is putting everything in requests. An interface reads like a
C++ class shared between processes: attributes are its data members, requests its
methods, broadcasts its signals. The three are not interchangeable.

| Use | When | Costs |
|---|---|---|
| **Attribute** | a data member: one value a consumer must know the current state of, as many as the state has | kept by the provider. A subscriber holds a copy it reads at any time, sent the moment it subscribes and again on every `set_` (`Notify="Always"`) or only when it changes (`OnChange`, the default) |
| **Broadcast** | a signal: as many values as the event needs, meaningful only together, at the moment it happened | fire and forget. Heard by whoever is subscribed at that instant; nothing is kept |
| **Request / Response** | a method call: the caller wants an answer to its own call | the response goes only to the caller, at once or when the work is done (`unblock_current_request()` frees the provider meanwhile) |

Rules that follow from the table:

- **What separates them is whether the value outlives the moment it was sent**, not
  subscribing: neither reaches a consumer before it subscribes. A later subscriber of
  an attribute is still sent the current value, with an `areg::DataState` saying
  whether it is valid yet.
- Where several values are reported together **and** a late subscriber must still
  learn the latest, publish that one as an attribute beside the broadcast:
  `recipes/03-attributes-and-broadcast/` is that pair.
- **An attribute says what something is, not that something happened.** A value that
  leaves and returns (idle, busy, idle) looks unchanged; the first one a subscriber
  gets is from before anything it asks next. That a job finished is a broadcast or a
  response.
- One provider's messages reach a consumer in the order it sent them; two providers'
  in no order.
- A consumer that polls with a request wants an **attribute**. Attributes exist so
  round trips do not.
- Telling everyone is a **broadcast**, not a response.
- A request needs a response only when the caller must know the outcome. A request
  with nothing to report is declared without one.

## 3. What are the components, and in which threads?

A **component** is one instance with one **role name**, its routing identity; one
that implements an interface is a service. It provides one and uses others (several: `24-component.md`); programs,
instances, deployments: `gen_docs.py --example programs`.

- One component per role, not per interface. A role name is unique in its process, and
  a public one in the network: `mtrouter` rejects a second provider.
- A consumer names the service by that role name, character for character.

A **thread** is a dispatcher. Every handler of every component in it runs there, one
at a time, in order. So the thread boundary is a **blocking** boundary, not a
performance one:

- Components that must not delay each other go in different threads.
- Anything slow -- a file, a socket, a long computation -- gets its own thread, or it
  stops every component sharing its thread. Make it a **worker** (`workers` in
  design.json): it is told things by events, so it can be stopped, paused, re-set and
  guarded by the watchdog, runs every N ms on a timer, and takes many tasks in order;
  one started, stopped or replaced on request is created by hand (`38-workers.md`
  section 3). A **plain thread** (`areg::Thread`, `std::thread`) only for a short task
  that runs alone, is started seldom and is bound to no component, or for a library
  call that never returns: `37-threads.md` section 2. Either hands its result to its
  component as a custom event (`23-events.md`).
- Components that only exchange messages may share a thread. Sharing is cheaper.

Start with a thread per component. Merge later, with a reason.

## 4. How many processes?

Ask only when there is a reason to separate: independent lifetimes, isolation of a
crash, different privileges, or different machines. Otherwise use one process.

Decide the split first: it fixes the `Category` in every document it touches.

| `Category` | Reach | Needs `mtrouter` |
|---|---|---|
| `Private` (the default) | inside one process | no |
| `Public` | between processes on one machine | **yes** |
| `Internet` | between machines | yes |

A `Private` interface consumed from another process never connects, and nothing reports it.

## Is a state machine needed?

Yes when the answer to an input depends on what came before -- a phase, a mode, a
sequence that must not be skipped. A handler that starts with `if (mPhase == ...)` is
a state machine already, written where it cannot be read.

No when each request is answered from the arguments and the stored data. Most are.

Answer it here, not in `22-state-machine.md`: that page is the format, not the
decision. Without one, the same logic is written by hand in the bodies. It lives in the service that controls what it
describes (`"machine"` on that interface), not in the contract: consumers see requests
and broadcasts, never states. Shared by several: `24-component.md`.

## Write this table before any file

| Interface | Service (role) | Consumed by | Category | Contract |
|---|---|---|---|---|
| `MeterService` | `MeterProvider` | `Collector` | Private | attribute `Reading` |
| `ReportService` | `Collector` | `Display` | Private | attribute `Report`, not a broadcast: `Display` must not miss it |

Then a second table of components: role name, interfaces provided, dependencies,
thread, process. Everything after this is mechanical -- one `.siml` per row of the
first table, one class per row of the second, and the model is the second table
written as macros.

## Worked example

> "A monitor watching three clients, and a logger."

Arrows: each client reports its state; the monitor decides something; everything
writes log lines.

- **`ClientState`**, provided by each client, consumed by the monitor. The state is a
  value that must be current, so it is an **attribute**, not a request. Three
  components provide it -- one interface, three services: `Client1`, `Client2`,
  `Client3`.
- **`Alarm`**, provided by the monitor, consumed by whoever reacts. A raised alarm
  reports the client, the level and the reason together, at the moment it happened:
  a **broadcast**, with the level also an attribute for whoever connects later.
- **The logger is not a service**, but a framework facility: `34-logging.md`.

Processes: one, unless the clients must survive the monitor restarting. If they must,
both documents become `Category="Public"` and `mtrouter` has to run.

## Smells

| Sign | What it usually means |
|---|---|
| An interface with one request and no state | it is a function; delete it |
| A component with fifteen requests | several interfaces wearing one name |
| Two interfaces that always change together | one interface |
| A consumer polling an attribute | subscribe instead |
| A broadcast a consumer must not miss | it is an attribute |
| An attribute holding values that only mean something together | it is a broadcast |
| A component that provides nothing and consumes nothing | it is a class, not a component |

## Then

`design.json` writes the documents and the model (`01-runbook.md` section 4); by
hand, `20-service-interface.md`, `32-model.md` or a recipe from `recipes/`.
