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

What a project needs follows from the `--mode` it was set up with, and nothing else:

| `setup_project.py --mode` | Start, in this order |
|---|---|
| `local`, `pubsub` | the application. Nothing else: no router, no collector |
| `ipc` | `mtrouter`, then the provider, then the consumer |

Add `logcollector` to either only to gather the logs of several processes in one
place; no application needs it to run.

**With no option at all, both run as `--console`.** That is the documented default and
it is not broken: a console `mtrouter` binds 8181 and routes, a console `logcollector`
binds 8282 and collects. Started by hand from a terminal, either is a working service.

**Console mode also reads commands from stdin, and end of input is `--quit`.** The
shell an agent runs in redirects stdin from `/dev/null`, so the service reads end of
input immediately and shuts itself down. It exits **0**, having printed only its
banner, so nothing in what you can see afterwards reports a failure:

| POSIX, started without `--service`, stdin from `/dev/null` | What happens |
|---|---|
| `mtrouter` | paints its display, then quits within a second. 8181 is left unbound and every consumer waits for a provider it can never reach. |
| `logcollector` | prints its banner and quits in under half a second. The application then logs to its own file and the collector database stays empty. |

**On POSIX** `--service` is the same program without the console loop: it never reads
stdin, so the redirect cannot end it. Start both with it there.

**On Windows it means the Service Control Manager and fails from a command line**, so
start them with **no option**: `start ""` gives the process a console, and console mode
with a live stdin stays up. `-i` and `-u` install and remove the real service, which is
the only thing `--service` is for.

A test run lets `run_scenarios.py` start the router; it knows both forms
(`53-scenarios.md`).

`--help` on any of the four prints its whole option table, which is the answer to a
spelling this page does not give.

So check the port, never the process list and never the exit code:

```bash
ss -ltn | grep 8181        # the router
ss -ltn | grep 8282        # the collector
```

On Windows: `netstat -ano | findstr :8181` and `:8282`.

`logobserver` is the exception: it takes no `--service`. With no option it opens a
console; with any option below it runs and exits, the form an agent wants.

`mtrouter` needs no configuration to route. **`logcollector` does:** nothing reaches
it until the application's configuration file enables the remote target and names port
8282, so starting it alone changes nothing. The keys are below.

To keep the logs, start the collector with `--log=db`: it writes a SQLite `.sqlog`, at
the path given next, else `<working dir>/logs/`. Queries: `35-sqlog.md`.

```bash
./build/bin/logcollector.elf --service --log=db /tmp/run.sqlog &
```

On Windows the same four are `build\bin\<name>.exe`, backgrounded with
`start "" build\bin\mtrouter.exe` -- no option -- rather than a trailing `&`. They
install as Windows services with `-i` and are removed with `-u`.

**A second router binds nothing and routes nothing**: only one process holds 8181.

All four take the same options, and no option at all means console:

| Option | Does |
|---|---|
| none, or `-c` / `--console` | run in the foreground, in this terminal |
| `-h` / `--help` | list the options |
| `-v` / `--verbose` | show the data rate while running |
| `-l <file>` / `--load <file>` | read a configuration file instead of `areg.init` |
| `-i` / `-u` (Windows) | install and uninstall as a system service |
| `-s` / `--service` | POSIX: run with no console loop. Windows: only for the process the Service Control Manager starts; it fails from a command line |

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
./build/bin/logobserver.elf -o "*::areg_base_Socket=DEBUG" -q # raise one scope everywhere
```

**Use the command line.** The observer connects, waits up to 15 seconds for the
collector to report the applications and their scopes, runs the options in order and
exits, printing plain lines. `-c` opens the console afterwards instead of exiting.

At the prompt the connection is **not** automatic: type `-r` first, then `-n`.

**Order matters once, at the start.** Start `mtrouter` before the processes that need
it. After that, order is free: a consumer started before its provider waits and
connects when the provider appears, and every process reconnects on its own after the
router restarts.

Single process applications never need any of this.

---

## Collecting the logs of several processes

`logcollector` gathers the logs of several processes and `logobserver` displays them.
It is configuration only, in the file the application reads (`34-logging.md`):

```ini
log::*::target            = remote | file   # one list: change the line, never add a second
log::*::enable::remote    = true
log::*::remote::service   = logger          # the section that holds the address below

logger::*::service        = logcollector
logger::*::connect        = tcpip
logger::*::enable::tcpip  = true
logger::*::address::tcpip = localhost       # the machine running logcollector
logger::*::port::tcpip    = 8282            # logcollector's port
```

**8282 is logcollector. 8181 is `mtrouter`.** A router that is running says nothing
about whether logs are being collected. A project with this wired:
`recipes/08-observability/`.

`logobserver` displays and does not store. An application whose collector is not
running still runs, and retries quietly: **a silent `logobserver` is not evidence that
the application is broken** -- check that `logcollector` is up on 8282.
