# When it does not work

Find the symptom. The first cause listed for it is by far the most common. Starting
and stopping `mtrouter`, `logcollector` and `logobserver` is `50-running.md`.

---

## Look up the message you actually got

Match the text of the error, not your guess at its meaning. Each row is a substring
of what the tool printed.

| The message contains | Cause | Go to |
|---|---|---|
| `is not recognized`, or `command not found`, for `java`, `cmake` or a compiler | a prerequisite is missing | `../../AGENTS.md` section 3 |
| `CMake 3.20 or higher is required` | CMake is too old | `../../AGENTS.md` section 3 |
| `Could not find a package configuration file provided by "areg"` | the project does not find the SDK | `10-new-project.md`, or `../wiki/02b-cmake-integrate.md` |
| `Unknown CMake command "addServiceInterface"` | the areg CMake package was never included | `../wiki/02b-cmake-integrate.md` |
| `Nothing is generated`, or `Nothing was generated from the document`, before the compiler ran | the generator refused the document | "The generator refused the document" below |
| `cannot declare variable to be of abstract type` | a request is not implemented | "The provider does not compile" below |
| `no member named 'request_`, `no member named 'broadcast_`, `no member named 'set_` | the member name is invented, not derived | `20-service-interface.md` section 3 |
| `marked 'override' but does not override`, or `marked 'final'` on a member | the signature or the name does not match the generated one | `20-service-interface.md` section 3 |
| `no member named 'begin'`, or `'begin' was not declared`, on an areg container | areg containers have no iterators | `40-base-api.md` |
| `undefined reference to \`areg::` | headers and library are different builds | "The build links against the wrong framework" below |
| `No such file or directory` for a `...Base.hpp` | the generator did not run, or the path is wrong | "The generated code does not match the document" below |
| nothing at all: the consumer prints nothing and does not exit | it never connected | "The consumer never connects" below |
| nothing at all: a worker thread, a custom event handler or a watchdog never runs | it was never wired to a name that exists | "A worker thread or a custom event does nothing" below |
| `RequestBusy` in a `request_*_failed` handler | the provider answers later without releasing the request first | `30-provider.md` section 3 |
| `Failed to bind`, or a second router that routes nothing | port 8181 is already held | `50-running.md` |

---
## Working out where it broke

Take these in order. Each one costs less than the one after it.

1. **Read the output.** A consumer that never prints has never connected; go to the
   first section of this page. A provider that never prints was never reached.
2. **Ask the generator.** If the build failed before the compiler ran, the generator
   refused the document and named the rule in words:
   `explain_rule.py --search "words from the message"`.
3. **Check the contract.** `check_contract.py` reads the same rules the pages state
   and reports the ones a build cannot see: a dependency string that matches no role,
   a request called in a constructor, a blocking handler, an invented member name.
4. **Turn on a scope.** Logging is off until a configuration file says otherwise, and
   that file is `build/bin/config/areg.init` -- inside a `config/` subdirectory, not
   beside the executable. See `34-logging.md`. Switch on the scope of the handler that
   should have run. If it never entered, the message never arrived; if it entered, the
   logic is wrong.
5. **Check the wiring, not the code.** Most silent failures are a role name that does
   not match, a `Category` that does not reach, or a missing `mtrouter` -- none of
   which the compiler can see.
6. **Compare with something that works.** Copy the nearest `recipes/` recipe, confirm
   it runs, then move your document into it one piece at a time.

## A working example is the cheapest reference

`../../examples/` holds 32 complete applications. Running one with the shape you are
writing proves the environment is sound before you blame your own code, and the tiers
save deriving which of them need `mtrouter`. Which example shows what: `41-examples.md`.

```bash
python3 <areg-sdk>/tools/run-all-examples.py --tier smoke   # no router needed
python3 <areg-sdk>/tools/run-all-examples.py --tier ipc     # these need mtrouter
```

Every command here has a Windows form: `python` for `python3`, `.exe` for `.elf`.

---

## The generator refused the document

The generator validates a document before generating, so this is a defect in the
`.siml`, `.fsml` or `.dtml` file and never in the build. Every finding belongs to a
rule in a registry. Ask the rule instead of reading the schema.

```bash
python3 <areg-sdk>/tools/explain_rule.py --search "resolves to nothing"   # the usual path
python3 <areg-sdk>/tools/explain_rule.py 27                     # when the message had a number
```

The number carries the severity: bare is an error, plus 100 a warning, plus 200
information, so 4, 104 and 204 are different rules. `--list --document fsml` shows
every rule for one document type.

Most messages carry no number today, so `--search` is the common path. Quote the
distinctive words of the message, not the file name or the identifiers; a phrase that
matches nothing exactly is scored word by word and the closest rules are offered.

Without Python, the same registry is `tools/schema/rules.xml`, keyed by the number.

---

## The consumer never connects

`service_connected` is never called with a connected state. There is no error
message: this failure is silent by design.

| Cause | Check | Fix |
|---|---|---|
| The dependency string does not match the role name | Compare `REGISTER_DEPENDENCY("X")` with `BEGIN_REGISTER_COMPONENT("X", ...)` character for character | Make them equal |
| Multi process, and the router is not running | Is `mtrouter` running? | Start it before the applications |
| Multi process, but the service is `Private` | `Category` in the `.siml` | Set `Public`, regenerate, rebuild |
| The provider is not registered | Is there a `REGISTER_IMPLEMENT_SERVICE` line? | Add it |
| Two components claim one role name | Search the model for the name | Give each a unique name, or `areg::generate_name()` |

---

## The provider does not compile

```
error: cannot declare variable to be of abstract type 'ServiceProvider'
note: because the following virtual functions are pure within ...
```

Every request in the document is pure virtual in the generated base. The note names
the one you have not implemented, or whose signature does not match.

| Cause | Fix |
|---|---|
| A request is not overridden | Implement it; the name is `request_` plus the document's name in `snake_case` |
| The signature differs | Copy the parameter types from the note; do not guess |
| The document changed and the code did not | Implement the new request, delete the removed one |

---

## Nothing happens and the application never exits

| Cause | Fix |
|---|---|
| Nothing calls `areg::Application::signal_quit()` | Call it when the work is done |
| The consumer is waiting for a provider that never appears | See "The consumer never connects" |
| A handler is blocking | See below |

---

## One thread stops responding

Every component in a thread shares one dispatcher. A handler that sleeps, loops, or
waits blocks all of them.

| Cause | Fix |
|---|---|
| A handler blocks | Move the work to a timer or another thread and answer later |
| A handler waits for another component in the same thread | Register them in different threads |

A request does not have to be answered inside its handler -- but releasing it first
is not optional. Call `unblock_current_request()`, carry the session it returns, and
give it to `prepare_response()` before the answer: `30-provider.md` section 3.

---

## The consumer quits whenever the provider restarts

The consumer treats a transient state as fatal. `Disconnected`, `ConnectionLost` and
`Failed` are transient and the framework reconnects by itself. Only `Rejected` and
`Shutdown` are terminal. See `31-consumer.md` section 3.

---

## An attribute or broadcast never arrives

| Cause | Fix |
|---|---|
| Never subscribed | Call `notify_on_<attribute>_update(true)` or `notify_on_broadcast_<name>(true)` inside `service_connected` |
| Subscribed before connecting | Subscribe only after `areg::is_service_connected(status)` is true |
| The provider never set the attribute | Set it once at startup |
| The value looks wrong on the first callback | The first notification can report the value as not valid. Check `areg::DataState` before using it |
| Resubscription after a reconnect | `service_connected` runs again; subscribe there, not once at construction |

---

## A worker thread or a custom event does nothing

Nothing here reports an error. Every one of these compiles, starts, and stays silent.

| Cause | Check | Fix |
|---|---|---|
| The worker consumer name is not the one the component answers to | Compare the string in `REGISTER_WORKER_THREAD` with every name `worker_thread_consumer()` tests | Make them equal. An unknown name returns `nullptr` and the thread runs nothing |
| A custom event has no listener | Is there an `add_listener` for that event, in the thread that should receive it? | Register in the consumer's own thread; see `23-events.md` |
| A custom event is sent before its listener registers | Is `add_listener` called in the component constructor or in `started()`? | Register before the first send |
| `AREG_DECLARE_EVENT_EX` names a thread that is not in the model | Compare the macro's thread name with `BEGIN_REGISTER_THREAD` | Make them equal; the automatic registration silently finds no thread |
| A watchdog timeout never bites | Was `areg::Application::setup()` called with `startWatchdog` true? | A non-zero timeout in `BEGIN_REGISTER_THREAD_EX` does nothing while the watchdog manager is off; see `37-threads.md` |

`python3 <sdk>/tools/agent/check_contract.py . --strict` reports the first of these as `P-11`
and the last as `P-10` without building anything.

---

## The build links against the wrong framework

```
undefined reference to `areg::ScopeMessage::log_message(...) const'
```

The library and the headers come from different versions. A symbol exists but its
signature differs, so the mangled name does not match.

| Cause | Fix |
|---|---|
| A stale prebuilt library | Rebuild the framework, or point the project at the SDK you compiled against |
| Mixed build directories | Remove the build directory and configure again |

---

## The generated code does not match the document

Delete the generate target and build again. The generator skips files that are already
current, so a partial or interrupted run can leave old code in place. Passing `--force`
to the generator has the same effect.

---

## Getting more out of the application

Turning scopes on is step 4 above: key syntax in `34-logging.md` section 3, the full
description in `../wiki/04a-logging-config.md`. Collected logs are plain SQLite; any
client can query them: `35-sqlog.md`, or `../wiki/04e-log-database-format.md`.
