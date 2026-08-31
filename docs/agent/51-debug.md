# When it does not work

Find the symptom. Each one has a small set of causes, and the first cause listed is
by far the most common.

---

## Running the pieces

Nothing here needs installing. The framework services are built into the same
`build/bin/` as the application.

| Process | Needed when | Start | Stop |
|---|---|---|---|
| `mtrouter` | provider and consumer are in different processes | `./build/bin/mtrouter.elf --service &` | kill the process |
| the application | always | `./build/bin/<name>.elf` | its own exit, or Ctrl-C |
| `logcollector` | you want the logs of several processes in one place | `./build/bin/logcollector.elf &` | Ctrl-C |
| `logobserver` | you want to watch those logs live | `./build/bin/logobserver.elf` | Ctrl-C |

**Check the port, do not assume the router started.** Only one process can hold
8181. If a router is already running -- from an earlier scenario, or left behind by a
crashed test -- a second one cannot bind, prints its banner anyway, and routes nothing.
Every consumer then waits for a provider it can never reach.

Use `--service` for an unattended run. Console mode, the default, paints a live status
display meant for a terminal; captured into a log it is noise. Both modes route
identically.

All four take the same options, and no option at all means console:

| Option | Does |
|---|---|
| none, or `-c` / `--console` | run in the foreground, in this terminal |
| `-h` / `--help` | list the options |
| `-v` / `--verbose` | show the data rate while running |
| `-l <file>` / `--load <file>` | read a configuration file instead of `areg.init` |
| `-i` / `-u` (Windows) | install and uninstall as a system service |
| `-s` / `--service` | run in the background as a system service |

### Changing what is logged while it runs

`logobserver` controls the log levels of every connected application live, so a scope
can be switched on without editing `areg.init` and restarting anything. Type these at
its prompt:

| Command | Does |
|---|---|
| `-n` / `--instances` | list the applications currently connected |
| `-e <instance>` / `--query` | list the scopes of one application |
| `-o <scope>=<prio>` / `--scope` | change a scope's priority now |
| `-p` / `-r` / `-x` | pause, restart and stop logging |
| `-q` / `--quit` | leave |

Only `-l <file>` is read from the command line at start-up; the rest are console
commands, so an agent driving `logobserver` has to write them to its standard input.

**Order matters once, at the start.** Start `mtrouter` before the processes that need
it. After that, order is free: a consumer started before its provider waits and
connects when the provider appears, and every process reconnects on its own after the
router restarts.

Single process applications never need any of this.

---

## Working out where it broke

Take these in order. Each one costs less than the one after it.

1. **Read the output.** A consumer that never prints has never connected; go to the
   first section of this page. A provider that never prints was never reached.
2. **Ask the generator.** If the build failed before the compiler ran, the generator
   refused the document and reported a number: `explain_rule.py <number>`.
3. **Turn on a scope.** Logging is off until `areg.init` says otherwise; see
   `34-logging.md`. Switch on the scope of the handler that should have run. If it
   never entered, the message never arrived; if it entered, the logic is wrong.
4. **Check the wiring, not the code.** Most silent failures are a role name that does
   not match, a `Category` that does not reach, or a missing `mtrouter` -- none of
   which the compiler can see.
5. **Compare with something that works.** Copy the nearest recipe from `recipes/`,
   confirm it runs, then move your document into it one piece at a time.

## A working example is the cheapest reference

`../../examples/` holds 32 complete applications. Which of them need `mtrouter` is
decided by whether they split into separate processes, not by their number. The list
is not worth deriving by hand -- `--tier smoke` runs the ones that do not need it and
`--tier ipc` the ones that do. When a shape is unfamiliar, run the example that has it
before writing anything:

```bash
python3 <areg-sdk>/tools/run-all-examples.py --tier smoke     # the quick set
python3 <areg-sdk>/tools/run-all-examples.py --tier ipc       # the ones needing mtrouter
```

It starts the processes that belong together, gives the driving one a deadline, and
passes only when every process ended as it should. Use it to prove the environment is
sound before blaming your own code.

---

## The generator refused the document

It reports a numbered rule. Ask what the number means instead of reading the schema:

```bash
python3 <areg-sdk>/tools/explain_rule.py 27
```

The number carries the severity: bare is an error, plus 100 a warning, plus 200
information, so 4, 104 and 204 are different rules. `--list --document fsml` shows
every rule for one document type.

---

## The consumer never connects

`service_connected` is never called with a connected state, and nothing happens.
There is no error message: this failure is silent by design.

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

A request does not have to be answered inside its handler. Store what you need and
call the response later.

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

Logging is configured in `areg.init` next to the executable. Turn scopes on there and
read the output. Full description: `../wiki/04a-logging-config.md`. Collected logs
are a plain SQLite database, so any SQLite client can query them:
`../wiki/04e-log-database-format.md`.
