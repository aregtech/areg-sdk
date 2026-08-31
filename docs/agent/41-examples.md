# Which example shows what

Thirty-two complete applications live in `../../examples/`. This page exists so you
never have to list that directory or open an example to find out what is in it.

**A working example is the cheapest reference there is.** When a shape is unfamiliar,
run the example that has it before writing anything.

## Reading the table

- **Router** - `yes` means the example splits into separate processes and needs
  `mtrouter` running first. `no` means one process; start it and nothing else.
- The number is not a difficulty ranking and does not say whether the router is
  needed. `Category="Public"` in a document does not say either: `01_minimalrpc`
  is `Public` and still runs in a single process.

---

## By what you need to see

| I need to see ... | Example | Router |
|---|---|---|
| The smallest possible service, thread to thread | `01_minimalrpc` | no |
| The smallest possible service across processes | `02_minimalipc` | yes |
| **One service in all three deployments** - one thread, two threads, two processes | `03_helloservice` | only the `multiprocess/` variant |
| A service provider on its own | `11_service` | no |
| Provider and consumer in one process | `13_locsvc` | no |
| A public service across processes | `15_pubsvc` | yes |
| Several services wired into a mesh, locally | `14_locmesh` | no |
| Several services wired into a mesh, across processes | `16_pubmesh` | yes |
| One service reused by many components and threads | `12_svcmulti` | no |
| Attributes and subscription (Publish / Subscribe) | `25_pubsub` | yes |
| Pub/Sub mixed across threads and processes, surviving interruption | `26_pubsubmix` | yes |
| Many subscribers sharing one thread efficiently | `27_pubsubmulti` | yes |
| A state machine driving a service | `19_pubfsm` | yes |
| Timers: one-shot, periodic and continuous | `08_timer` | no |
| Worker threads and custom events | `18_pubworker` | yes |
| Building the model at run time instead of declaring it | `17_pubtraffic` | yes |
| Answering a request later instead of inside the handler | `24_pubunblock` | yes |
| Watchdog restarting a failed thread | `21_locwatchdog` | no |
| Watchdog across processes | `22_pubwatchdog` | yes |

## By framework facility, not by service

| I need to see ... | Example | Router |
|---|---|---|
| Creating and joining a plain thread | `04_hellothread` | no |
| Custom event dispatching threads | `09_threads` | no |
| Serializing into a shared buffer and reading it back | `05_buffer` | no |
| Reading and writing files, text and binary | `06_file` | no |
| Logging and call tracing | `07_logging` | no |
| Mutexes and synchronization events | `10_sync` | no |
| Synchronization events used with STL threads | `28_stlsync` | no |
| `SyncEvent` signalling semantics in detail | `29_syncevent` | no |

## Benchmarks and platform specific

| Example | Shows | Router |
|---|---|---|
| `23_pubdatarate` | Data transfer rate between processes | yes |
| `30_publatency` | IPC latency, round trip and one way | yes |
| `31_loclatency` | The same measurement inside one process | no |
| `32_pubmixed` | What bulk traffic costs small messages on one connection | yes |
| `20_winchat` | A chat application. **Windows only**, needs MFC and MSVC or ClangCL | yes |

---

## Running them

Do not start them by hand when you only want to know that the environment works:

```bash
python3 <areg-sdk>/tools/run-all-examples.py --tier smoke   # the ones needing no router
python3 <areg-sdk>/tools/run-all-examples.py --tier ipc     # the ones needing mtrouter
```

It starts the processes that belong together, gives the driving one a deadline, and
passes only when every process ended as it should.

To run one by hand, from a built SDK:

```bash
./product/build/.../bin/mtrouter.elf --service &    # only if the table says yes
./product/build/.../bin/<example-binary>.elf
```

Examples build into this repository's own output layout, not into `build/bin/`. That
layout is described in `../../CODEBASE.md` section 4.

---

## An example is a reference, not a template

Copy from `recipes/` when you are starting a project: a recipe is a minimal, verified
project laid out the way a new project should be. An example is built inside this
repository's own build system, so copying one drags that in with it.

| Purpose | Use |
|---|---|
| Start a new project | `recipes/`, or `tools/setup_project.py` |
| See how a facility is used | `../../examples/` |

Fuller descriptions of every example, with the reasoning behind each:
`../../examples/README.md`.
