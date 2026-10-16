# Testing an application with scenarios

`tools/agent/run_scenarios.py` starts a list of processes in order, waits for the
leading one, stops every process still running, and exits 0 only when each ended as
the scenario says and every expected line appeared: a provider that runs until stopped
needs no `stop` and no `exit`. A line is a message a process logged (`34-logging.md`), read
from its log file as it is written, without its time, thread and priority; what a process writes
to its standard output counts too. `build_project.py --run` runs it after every build.
The file is `scenarios.json` unless `--file` says otherwise:

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
| `expect` | regular expressions that must all match that process's lines |
| `reject` | regular expressions that must **not** match them |
| `stdin` | lines written to the process's standard input, then the stream is closed |
| `exit` | required exit code; omit or `null` to accept any |
| `router` | `true` starts `mtrouter` first. Default `false` |
| `timeout` | seconds before the scenario is failed and killed. Default 60 |
| `stop` | stops one process while the others run, so a peer can be seen reacting |

```bash
python3 <areg-sdk>/tools/agent/run_scenarios.py               # exit 0 means it works
python3 <areg-sdk>/tools/agent/run_scenarios.py --only smoke --verbose  # every line each wrote
```

Windows: `python <areg-sdk>\tools\agent\run_scenarios.py`.

**One run is the evidence.** A pass prints the line every `expect` matched, a failure
prints what the process wrote. Never start the processes by hand with `&`, `sleep`,
`kill` or `ps`: a scenario does it repeatably and leaves nothing running.

---

## The peer going away

`stop` ends one process while the rest run on. `after` is a regular expression matched
against the leading process's lines as they appear, a number of seconds, or absent for
"as soon as it is running"; `"signal": "kill"` is an abrupt loss, `"term"` the default:

```json
{ "name": "peer-lost", "router": true,
  "stop": { "proc": "provider", "after": "consumer: ordered", "signal": "kill" },
  "procs": [
    { "binary": "myprovider", "name": "provider" },
    { "binary": "myconsumer", "name": "consumer",
      "expect": ["service lost, waiting"], "reject": ["crash"] } ] }
```

`gen_skeleton.py --app` writes this scenario for a design whose consumer has steps and a
reconnect deadline; its trigger, when it cannot pick one, is a worksheet section.

**A scripted consumer on local IPC finishes in well under a second**, so a `stop` whose
`after` matches a line partway through the script is read only once the lead has
exited, and nothing is stopped. The runner fails the scenario for it. The remedy is a
step, never a sleep: a `{"name": "settle", "wait": 3000}` step in `design.json`, right
after the step `after` matches.

A long-running consumer survives it -- `Disconnected` and `ConnectionLost` reconnect
on their own -- so expect its reconnect line and no exit; one that runs a fixed script
may give up on a deadline: expect its line and the non-zero exit.
