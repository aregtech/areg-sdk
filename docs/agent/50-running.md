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
| `logobserver` | you want to watch those logs live | `./build/bin/logobserver.elf` | Ctrl-C |

**`--service` is not optional.** Console mode is the default, and it reads commands
from a terminal. A shell with no terminal -- which is every shell an agent runs in --
gives it end of file immediately, and neither service ever binds its port. Neither
reports an error, and the two fail differently, so neither exit code tells you:

| Started without `--service` | What happens |
|---|---|
| `mtrouter` | keeps running, paints its display, **never binds 8181**. It looks alive, routes nothing, and every consumer waits for a provider it can never reach. |
| `logcollector` | prints its banner and **exits 0 in under half a second**. The application then logs to its own file and the collector database stays empty. |

So check the port, never the process list and never the exit code:

```bash
ss -ltn | grep 8181        # the router
ss -ltn | grep 8282        # the collector
```

`logobserver` is the exception: it is meant to be driven from a console, so it keeps
one and takes no `--service`.

`mtrouter` needs no configuration to route. **`logcollector` does:** an application
sends nothing to it until `config/areg.init` enables the remote target and names port
8282. The keys are in `34-logging.md` section 4. Starting the collector alone changes
nothing.

To keep the logs rather than watch them scroll past, start the collector with
`--log=db`; it then writes a SQLite `.sqlog` file that you can query after the run.
`--log=db` takes an optional path as the next argument, which is the only way to say
where the file goes; with no path it lands in `<working dir>/logs/logcollector_<timestamp>.sqlog`.
The schema and ready-made queries are in `35-sqlog.md`.

```bash
./build/bin/logcollector.elf --service --log=db /tmp/run.sqlog &
```

On Windows the same four are `build\bin\<name>.exe`, and a process is backgrounded
with `start "" build\bin\mtrouter.exe --service` rather than a trailing `&`. They can
also be installed as Windows services with `-i`, and removed with `-u`.

**A second router is the other way this fails.** Only one process can hold 8181. If a
router is already running -- from an earlier scenario, or left behind by a crashed
test -- a second one cannot bind, prints its banner anyway, and routes nothing.

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

## Running it unattended, as one verdict

`run_scenarios.py` starts a list of processes in order, waits for the leading one, and
exits 0 only when every process ended as the scenario says and every expected line
appeared. It is how an agent proves an application works without reading output itself.

It reads `scenarios.json` from the working directory unless `--file` says otherwise:

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
| `expect` | regular expressions that must all match that process's captured output |
| `exit` | required exit code; omit or `null` to accept any |
| `router` | `true` starts `mtrouter` first. Default `false` |
| `timeout` | seconds before the scenario is failed and killed. Default 60 |

```bash
python3 <areg-sdk>/tools/agent/run_scenarios.py               # exit 0 means it works
python3 <areg-sdk>/tools/agent/run_scenarios.py --only smoke --json
```

A scenario that names a binary the build did not produce fails the scenario rather
than passing silently.
