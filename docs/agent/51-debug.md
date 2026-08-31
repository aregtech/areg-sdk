# When it does not work

Find the symptom. Each one has a small set of causes, and the first cause listed is
by far the most common.

---

## Look up the message you actually got

Match the text of the error, not your guess at its meaning. Each row is a substring:
search for it in what the tool printed.

| The message contains | Cause | Go to |
|---|---|---|
| `is not recognized`, or `command not found`, for `java`, `cmake` or a compiler | a prerequisite is missing | `../../AGENTS.md` section 2 |
| `CMake 3.20 or higher is required` | CMake is too old | `../../AGENTS.md` section 2 |
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
| `Failed to bind`, or a second router that routes nothing | port 8181 is already held | "Running the pieces" below |

Before building, `tools/check_contract.py` finds the mistakes that compile cleanly and
fail only at run time -- a role name that does not match, a request called too early,
a handler that blocks:

```bash
python3 <areg-sdk>/tools/check_contract.py .
```

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

`mtrouter` needs no configuration to route. **`logcollector` does:** an application
sends nothing to it until `config/areg.init` enables the remote target and names port
8282. The keys are in `34-logging.md` section 4. Starting the collector alone changes
nothing.

To keep the logs rather than watch them scroll past, start the collector with
`--log=db`; it then writes a SQLite `.sqlog` file that you can query after the run.
The schema and ready-made queries are in `35-sqlog.md`.

On Windows the same four are `build\bin\<name>.exe`, and a process is backgrounded
with `start "" build\bin\mtrouter.exe --service` rather than a trailing `&`. They can
also be installed as Windows services with `-i`, and removed with `-u`.

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
6. **Compare with something that works.** Copy the nearest recipe from `recipes/`,
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

The generator validates a document before it generates anything, so this is a defect
in the `.siml`, `.fsml` or `.dtml` file and never in the build. Every finding belongs
to a rule in a registry. Ask what the rule says instead of reading the schema.

When the message carries a number, pass it:

```bash
python3 <areg-sdk>/tools/explain_rule.py 27
```

The number carries the severity: bare is an error, plus 100 a warning, plus 200
information, so 4, 104 and 204 are different rules. `--list --document fsml` shows
every rule for one document type.

When the message carries no number, which is the common case today, give the tool the
words of the message instead:

```bash
python3 <areg-sdk>/tools/explain_rule.py --search "a data type that resolves to nothing"
```

It answers with the rule, its number and its band. The registry and the generator do
not always word a thing the same way, so a phrase that matches nothing exactly is
scored word by word and the closest rules are offered. Quote the distinctive part of
the message, not the file name or the identifiers.

Without Python, the same registry is `tools/schema/rules.xml`, keyed by the number.

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

Logging is configured in `config/areg.init`, in a `config/` subdirectory of the
executable's folder -- for a project, `build/bin/config/areg.init`. Turn scopes on
there and read the output; the key syntax is in `34-logging.md` section 3. Full
description: `../wiki/04a-logging-config.md`. Collected logs are a plain SQLite
database, so any SQLite client can query them:
`../wiki/04e-log-database-format.md`.
