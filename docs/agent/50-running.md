# Running the pieces

Nothing here needs installing: `mtrouter`, `logcollector` and `logobserver` are built
into the same `build/bin/` as the application. This page is how to start them, stop
them and change what they do while they run. Why an application does not work is
`51-debug.md`.

---

| Process | Needed when | Start | Stop |
|---|---|---|---|
| `mtrouter` | provider and consumer are in different processes | `./build/bin/mtrouter.elf --service &` | kill the process |
| the application | always | `./build/bin/<name>.elf` | its own exit, or Ctrl-C |
| `logcollector` | you want the logs of several processes in one place | `./build/bin/logcollector.elf --service &` | kill the process |
| `logobserver` | you want to watch those logs live, or to change a scope | `./build/bin/logobserver.elf -n -q` | its own exit |

**`--service` is not optional.** Console mode is the default and reads commands from
a terminal. With no terminal -- every shell an agent runs in -- neither service binds
its port. Neither reports an error, and they fail differently, so no exit code tells
you:

| Started without `--service` | What happens |
|---|---|
| `mtrouter` | keeps running, paints its display, **never binds 8181**. It looks alive, routes nothing, and every consumer waits for a provider it can never reach. |
| `logcollector` | prints its banner and **exits 0 in under half a second**. The application then logs to its own file and the collector database stays empty. |

So check the port, never the process list and never the exit code:

```bash
ss -ltn | grep 8181        # the router
ss -ltn | grep 8282        # the collector
```

`logobserver` is the exception: it takes no `--service`. With no option it opens a
console; with any option below it runs and exits, the form an agent wants.

`mtrouter` needs no configuration to route. **`logcollector` does:** nothing reaches
it until `config/areg.init` enables the remote target and names port 8282, so
starting it alone changes nothing. The keys are in `34-logging.md` section 4.

To keep the logs rather than watch them scroll past, start the collector with
`--log=db`. It writes a SQLite `.sqlog`; an optional path as the next argument is the
only way to place it, and with none it lands in
`<working dir>/logs/logcollector_<timestamp>.sqlog`. Queries are in `35-sqlog.md`.

```bash
./build/bin/logcollector.elf --service --log=db /tmp/run.sqlog &
```

On Windows the same four are `build\bin\<name>.exe`, backgrounded with
`start "" build\bin\mtrouter.exe --service` rather than a trailing `&`. They install
as Windows services with `-i` and are removed with `-u`.

**A second router is the other way this fails.** Only one process can hold 8181. A
router already running -- from an earlier scenario, or left by a crashed test --
makes the second one print its banner, bind nothing and route nothing.

All four take the same options, and no option at all means console:

| Option | Does |
|---|---|
| none, or `-c` / `--console` | run in the foreground, in this terminal |
| `-h` / `--help` | list the options |
| `-v` / `--verbose` | show the data rate while running |
| `-l <file>` / `--load <file>` | read a configuration file instead of `areg.init` |
| `-i` / `-u` (Windows) | install and uninstall as a system service |
| `-s` / `--service` | run in the background as a system service |

## Changing what is logged while it runs

`logobserver` controls the log levels of every connected application live, so a scope
can be switched on without editing `areg.init` and restarting anything. These work
both on its command line and at its prompt:

| Command | Does |
|---|---|
| `-n` / `--instances` | list the applications currently connected |
| `-e <instance>` / `--query` | list the scopes of one application |
| `-o <scope>=<prio>` / `--scope` | change a scope's priority now |
| `-p` / `-r` / `-x` | pause, restart and stop logging |
| `-q` / `--quit` | leave |

```bash
./build/bin/logobserver.elf -n -q                               # list the connected applications
./build/bin/logobserver.elf -o "*::areg_base_NESocket=DEBUG" -q # raise one scope everywhere
```

**Use the command line.** The observer connects itself, waits for the collector to
report the applications and their scopes -- up to 15 seconds, and it says so when the
data never came -- then runs the options in the order written and exits, printing
plain lines that pipe and grep. `-c` opens the console afterwards instead of exiting.
The wait is the point: a command issued the instant the socket opens finds nothing.

At the prompt the connection is **not** automatic: type `-r` first, then `-n`.

**Order matters once, at the start.** Start `mtrouter` before the processes that need
it. After that, order is free: a consumer started before its provider waits and
connects when the provider appears, and every process reconnects on its own after the
router restarts.

Single process applications never need any of this.

---

## Running it unattended, as one verdict

`run_scenarios.py` starts a list of processes in order, waits for the leading one and
exits 0 only when every process ended as the scenario says and every expected line
appeared. It reads `scenarios.json` unless `--file` says otherwise:

```json
{ "scenarios": [ {
    "name":    "smoke",
    "router":  true,
    "timeout": 30,
    "procs": [
      { "binary": "myprovider" },
      { "binary": "myconsumer", "expect": ["connected", "answer: 42"], "exit": 0 }
    ] } ] }
```

| Key | Means |
|---|---|
| `procs` | started in the listed order; the last one leads unless one sets `"lead": true` |
| `binary` | name without a platform suffix, looked up in the build directory |
| `name` | label for this process, used in reports and by `stop`. Default: the binary |
| `args` | command line arguments |
| `expect` | regular expressions that must all match that process's captured output |
| `reject` | regular expressions that must **not** match it |
| `stdin` | lines written to the process, then the stream is closed |
| `exit` | required exit code; omit or `null` to accept any |
| `router` | `true` starts `mtrouter` first. Default `false` |
| `timeout` | seconds before the scenario is failed and killed. Default 60 |
| `stop` | stops one process while the others run, so a peer can be seen reacting |

```bash
python3 <areg-sdk>/tools/agent/run_scenarios.py               # exit 0 means it works
python3 <areg-sdk>/tools/agent/run_scenarios.py --only smoke --json
```

A binary the build did not produce fails the scenario rather than passing silently.

**One run is the evidence.** A pass prints the line every `expect` matched, a failure
prints what the process wrote.

### Two acceptance items that look like they need a terminal

Every areg application runs until told to stop and talks to another process, so
**the console quit path** and **the peer going away** are asked of nearly every
task. Both are scenario keys, so neither needs `&`, `sleep`, `kill` or `ps`.

`stdin` feeds the console loop and then closes the stream:

```json
{ "binary": "myprovider", "lead": true, "stdin": ["-q"], "exit": 0 }
```

**End of input is not a quit request, and this is the trap.** A process with no
`stdin` key still gets its own standard input; nothing is ever written to it, so a
console loop waits there instead of being handed end of input at once. Write the loop
so it agrees:

```cpp
#include "areg/appbase/Application.hpp"   // Application
#include "areg/base/CommonDefs.hpp"       // WAIT_INFINITE

bool quitRequested{ false };
std::string line;
while (std::getline(std::cin, line))
{
    if ((line == "-q") || (line == "--quit")) { quitRequested = true; break; }
}
if (!quitRequested)                       // no console at all: keep serving
{   areg::Application::wait_quit(areg::WAIT_INFINITE); }
```

A loop that unloads the model as soon as `getline` returns false shuts the service
down within milliseconds of starting whenever it runs without a terminal. Every peer
then waits for a provider that has already gone, and there is no error to read: the
process exited normally and printed nothing.

`stop` ends one process while the rest run on. `after` is a regular expression
matched against the leading process's output as it appears, a number of seconds, or
absent for "as soon as it is running"; `"signal": "kill"` is an abrupt loss,
`"term"` the default:

```json
{ "name": "peer-lost", "router": true,
  "stop": { "proc": "provider", "after": "consumer: ordered", "signal": "kill" },
  "procs": [
    { "binary": "myprovider", "name": "provider" },
    { "binary": "myconsumer", "name": "consumer",
      "expect": ["service lost, waiting"], "reject": ["crash"] } ] }
```

What to expect depends on what the consumer is for. A long-running one must survive
it -- `Disconnected` and `ConnectionLost` reconnect on their own, so expect the line
the reconnect path prints and no exit. A client that runs a fixed script and finishes
may instead give up on a deadline: expect the line it prints and the non-zero exit,
and say which of the two the application is.
