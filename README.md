<div align="center">
  <a href="https://www.areg.tech">
    <img src="./docs/img/areg-sdk-logo.svg" alt="Areg SDK" width="320"/>
  </a>
</div>

# Areg SDK - C++ IPC and distributed services

**Change the deployment. Keep the service code.**

Moving a C++ component into another process should not mean rewriting its communication.
Areg is a C++17 framework for typed, asynchronous services. Keep the same provider and consumer
implementations in one thread, across threads, or between processes and machines.

Describe the interface. Areg generates the communication code and handles discovery,
thread-owned dispatch and reconnection. You implement the service behavior.

[![CMake build](https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml/badge.svg?branch=master)](https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml)
[![Coverage](https://codecov.io/gh/aregtech/areg-sdk/graph/badge.svg?branch=master)](https://codecov.io/gh/aregtech/areg-sdk)
[![Release](https://img.shields.io/github/v/release/aregtech/areg-sdk?include_prereleases&style=flat-square)](https://github.com/aregtech/areg-sdk/releases)
[![License: Apache-2.0](https://img.shields.io/badge/license-Apache--2.0-blue?style=flat-square)](./LICENSE.txt)

Linux · macOS · Windows · Embedded Linux · Trusted networks

[Run your first service](#quick-start) · [See where it fits](#use-cases) · [Evaluate the tradeoffs](#areg-vs-alternatives)

<div align="center"><img src="./docs/img/areg-one-code-three-deployments.svg" alt="The same provider and consumer in one thread, two threads, or separate processes connected through mtrouter over TCP" width="720"/></div>

Same service implementations; different deployment boundaries. Change the model and build
layout; configure interface visibility and the router as needed. This is not automatic live migration.
See the shared source in [`03_helloservice`](./examples/03_helloservice/).

Published Linux result: **~9.9 µs one-way P50** through the full API over TCP loopback,
204-byte messages, i7-13700H, pinned cores, bare TTY (July 2026).
[Conditions and raw data](#performance).

Not using it today? Star Areg to keep it in your C++ toolbox and help other developers discover it.

## The code that stays the same

A request goes to the provider; its response returns asynchronously to the consumer.
These are excerpts from the [quick-start application](./docs/agent/recipes/01-local-single-process/src/main.cpp);
class declarations, connection handling and logging setup are omitted.

```cpp
// Consumer: inside the connected branch of service_connected()
request_hello_service(role_name());

// Provider: the generated base class dispatches to this handler
void request_hello_service(const areg::String & client) final
{
    GLOBAL_INFO("provider: hello, %s", client.as_string());
    response_hello_service(true);
}
```

The answer arrives in the consumer's `response_hello_service(bool success)` handler.
Neither handler selects a socket, finds a remote address, or manages a dispatch thread.

---

## Quick start[![](./docs/img/pin.svg)](#quick-start)

Requires Git, a C++17 compiler, CMake 3.20+ and Java 17+ for code generation.
Java is not needed at runtime; Python is not needed for this path.
[Install the prerequisites](./docs/wiki/01e-build-environment.md).

On Linux or macOS, start in a directory where `areg-sdk` and `myapp` do not already exist:

```bash
git clone https://github.com/aregtech/areg-sdk.git
sh areg-sdk/tools/setup-project.sh --name myapp --root myapp --mode local --sdk-root areg-sdk
cd myapp
cmake -B build
cmake --build build -j4
./build/bin/myapp.elf                # use myapp.mac on macOS
cat build/bin/logs/myapp_*.log
```

The provider and consumer run in two threads. The program makes one round trip and exits;
the log contains these messages, with timestamps and logging metadata:

```text
provider: hello, ServiceConsumer
consumer: greeted
```

For two processes, scaffold a **new directory** with `--mode ipc`, build it the same way,
then run `./run.sh` to start `mtrouter`, the provider and the consumer.
On Windows, use `setup-project.bat`, run `build\bin\myapp.exe`, and read the logs with
`type build\bin\logs\myapp_*.log`; the IPC launcher is `run.bat`.
[Setup options and platform instructions](./docs/wiki/02a-quick-project-setup.md).

## Table of Contents

[Use cases](#use-cases) · [Why Areg](#why-areg) · [How it works](#how-it-works) ·
[Performance](#performance) · [Embedded ready](#embedded-ready) · [Alternatives](#areg-vs-alternatives) ·
[Project status](#project-status) · [Tools](#tools) · [Agentic coding](#agentic-coding) ·
[Getting started](#getting-started) · [Roadmap](#roadmap) · [Documentation](#documentation) ·
[License](#license) · [Community](#community)

---

## Use cases[![](./docs/img/pin.svg)](#use-cases)

Three architecture patterns to apply to your own system.

### Move the processing stage, not its implementation

A camera pipeline starts with acquisition, processing and storage in one application.
Later, processing needs its own process for isolation, or a separate machine for compute.
Keep the service implementations; change their model and build arrangement.
The same pattern fits scientific imaging, industrial inspection and edge inference.

<div align="center"><img src="./docs/img/areg-pipeline-deployments.svg" alt="Acquisition, processing and storage share one process during development; the same stages can run in separate processes, each connected to mtrouter over TCP" width="720"/></div>

Same stages, different process boundaries. Actual data rates depend on the network,
hardware and work inside each stage; Areg supplies communication, not an inference engine.
[Imaging](./docs/USECASES.md#scientific-and-industrial-imaging-pipelines) ·
[Edge inference](./docs/USECASES.md#edge-ai-and-inference-pipelines).

### Give a device a typed API

A measurement instrument or controller exposes capabilities as an Areg service.
Host applications use a generated proxy and a named service dependency instead of
inventing a host-facing protocol. The device still needs its OS and hardware drivers;
the reusable boundary is the service interface.
[Device services](./docs/USECASES.md#device-as-a-service-driverless-hardware).

### Develop the client before the hardware exists

Implement that same interface in a simulated provider, registered under the role the consumer
expects. Exercise the consumer, then replace the simulated provider with the device-backed one.
Keep the consumer's service calls. Simulation fidelity and hardware validation remain your responsibility.
[Simulation and hardware-in-the-loop](./docs/USECASES.md#simulation-and-hardware-in-the-loop-testing).

<div align="center"><img src="./docs/img/areg-device-simulation.svg" alt="Two alternative deployments: the same consumer connects through mtrouter to either a simulated provider or a device-backed provider, using the same interface and role name" width="720"/></div>

One service contract, alternative providers -- not two providers competing for the same role.
Explore the patterns in [Use cases and benefits](./docs/USECASES.md).
For a runnable demonstration of deployment reuse, start with [`03_helloservice`](./examples/03_helloservice/).

Also useful for [industrial automation](./docs/USECASES.md#industrial-automation-and-robotics),
[digital twins](./docs/USECASES.md#digital-twins-and-real-time-monitoring),
[splitting a C++ monolith](./docs/USECASES.md#incremental-decomposition-of-c-monoliths)
and [multiple UIs over one backend](./docs/USECASES.md#multi-display-and-multi-ui-architectures).

---

## Why Areg[![](./docs/img/pin.svg)](#why-areg)

- **Less infrastructure to maintain:** generated proxies, serialization and dispatch replace hand-written communication code.
- **Simpler state ownership:** service handlers run in the component's owning thread. State accessed only there needs no application locks; shared external state still needs synchronization.
- **Flexible startup order:** consumers connect when their named providers appear and reconnect when they return. Applications receive connection-state callbacks.
- **Recovery hooks built in:** a configured watchdog can restart a stuck component thread and reinitialize its components. It does not restart crashed processes or restore lost application state.
- **One view of many processes:** collect distributed logs and change logging priorities at runtime without restarting the application.

## How it works[![](./docs/img/pin.svg)](#how-it-works)

1. **Define the contract.** A `.siml` interface declares requests/responses, attributes
   (state updates for subscribers) and broadcasts (events). Edit XML directly or use the optional
   [Lusan designer](./docs/wiki/06e-lusan-service-interface.md).
2. **Generate and implement.** CMake runs the generator; you implement the provider and consumer
   handlers in the generated base-class model.
3. **Choose the deployment.** The model declares threads, components and named dependencies.
   In-process calls use event dispatch. Calls between processes use `mtrouter` over TCP,
   including when both processes run on the same machine.

```cmake
addServiceInterface(MyServiceLib ./services/MyService.siml)
```

The tradeoff: you adopt Areg's interfaces, generated base classes and component lifecycle,
not just a transport library. Keep handlers non-blocking; move long-running work to worker threads.
Cross-process interfaces must be `Public`.
[Code generator](./docs/wiki/06b-code-generator.md) ·
[Complete deployment models](./examples/03_helloservice/).

---

## Performance[![](./docs/img/pin.svg)](#performance)

Published Linux TCP-loopback results on an i7-13700H with DDR4, using the framework stack
rather than raw sockets:

<a id="latency-tcp-localhost-full-stack-204-byte-messages"></a>
<a id="throughput-tcp-localhost-11-measured-at-mtrouter"></a>

| Measurement | Result | Conditions |
|---|---|---|
| One-way latency, P50 | ~9.9 µs | 204-byte messages; pinned cores, bare TTY; July 2026 dataset |
| Sustained data rate | ~6.0-6.5 GB/s | ~3 MB payloads; unpinned; 5-minute run |
| Sustained message rate | ~2.0M msg/s | ~0.5 KB payloads; unpinned; 5-minute run |

Latency covers serialization through receiver dispatch, over two hops:
sender -> `mtrouter` -> receiver. Throughput is counted at `mtrouter`, not at the application
handler. These are separate configurations: the pinned latency and unpinned throughput results
are **not simultaneously achievable**. Loopback results are not cross-machine bandwidth guarantees.

[Methodology and Linux/macOS/Windows results](./docs/wiki/08b-areg-sdk-performance-benchmarks.md) ·
[Raw latency CSV](./docs/wiki/areg-latency-benchmark-20260705.csv).
Reproduce on your hardware with [`23_pubdatarate`](./examples/23_pubdatarate/) and
[`30_publatency`](./examples/30_publatency/).
[Logging measurements](./docs/wiki/04f-logging-performance.md) have their own workload and methodology.

## Embedded ready[![](./docs/img/pin.svg)](#embedded-ready)

For embedded Linux with sockets and pthreads, on ARMv7, ARM64, x86 and x86_64.
Published ARMv7 Release measurements, GCC 15.2.0:

| Artifact | Measured size |
|---|---|
| Minimal static service provider, logging off | 288 KB of `text` (code and read-only data); 485 KB whole binary |
| Shared `libareg.so`, logging on | 741 KB of `text`, stored once for multiple services |

These are cross-compiled binary sizes, not RAM measurements. The published minimal-service
RAM measurements are on x86_64, not ARM.
Bare metal, RTOS and 16-bit targets are **not supported today**.
[Configurations, RAM measurements and limits](./docs/wiki/08e-embedded-ready.md).

---

## Areg vs. alternatives[![](./docs/img/pin.svg)](#areg-vs-alternatives)

Choose by the architecture you need, not a universal framework ranking.

| Start by evaluating | When your deciding requirement is |
|---|---|
| Areg | An integrated C++ service/component model across threads, processes and trusted networks |
| [gRPC](https://grpc.io/docs/what-is-grpc/introduction/) | Cross-language RPC and the gRPC ecosystem |
| [DDS](https://www.dds-foundation.org/what-is-dds-3/) | DDS interoperability, typed publish/subscribe and its QoS model |
| [ZeroMQ](https://zeromq.org/) or [NNG](https://nng.nanomsg.org/) | Messaging primitives while retaining control of your service and lifecycle architecture |

Actor frameworks and Qt queued connections also offer thread-owned dispatch.
Areg combines that model with generated service interfaces, discovery by name,
reconnection and distributed logging. It is not a web framework or an internet-facing API gateway.

## Project status[![](./docs/img/pin.svg)](#project-status)

| | |
|---|---|
| **Build** | [![CMake build](https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml/badge.svg?branch=master)](https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml) [![MS Build](https://github.com/aregtech/areg-sdk/actions/workflows/msbuild.yml/badge.svg?branch=master)](https://github.com/aregtech/areg-sdk/actions/workflows/msbuild.yml) [![Platforms](https://github.com/aregtech/areg-sdk/actions/workflows/platforms.yml/badge.svg?branch=master)](https://github.com/aregtech/areg-sdk/actions/workflows/platforms.yml) |
| **Quality** | [![CodeQL](https://github.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml/badge.svg?branch=master)](https://github.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml) [![Sanitizers](https://github.com/aregtech/areg-sdk/actions/workflows/sanitizers.yml/badge.svg?branch=master)](https://github.com/aregtech/areg-sdk/actions/workflows/sanitizers.yml) [![Coverage](https://codecov.io/gh/aregtech/areg-sdk/graph/badge.svg?branch=master)](https://codecov.io/gh/aregtech/areg-sdk) [![Agent docs](https://github.com/aregtech/areg-sdk/actions/workflows/agent-docs.yml/badge.svg?branch=master)](https://github.com/aregtech/areg-sdk/actions/workflows/agent-docs.yml) |
| **Release** | [`v2.0.0-rc1`](https://github.com/aregtech/areg-sdk/releases/tag/v2.0.0-rc1) is the first 2.0 release candidate. Release 1.5.0 and earlier use a different, unsupported API. |
| **Supported** | 2.0.0-rc1 and newer, and `master`. Build from source or let CMake fetch Areg. |
| **Networking boundary** | `mtrouter` uses plain TCP, without authentication or encryption today. Deploy only in trusted networks; optional TLS is planned, not shipped. |

Badges link to the build, analysis and coverage reports; coverage is not a guarantee of correctness.

---

## Tools[![](./docs/img/pin.svg)](#tools)

Debug the system, not one process at a time. `logcollector` gathers logs from applications;
[Lusan](https://github.com/aregtech/areg-sdk-tools) shows their sources and messages together.
Change log priorities in running applications without a restart.

<div align="center"><a href="./docs/img/lusan-live-log-gen.png"><img src="./docs/img/lusan-live-log-readme.png" alt="Lusan showing a provider and two consumer processes alongside their collected log messages" width="860"/></a></div>

An existing Lusan capture, cropped to sources and log rows; it shows older example names,
not output from the quick start above. [Live-logging walkthrough](./docs/wiki/06f-lusan-live-logging.md).

| Tool | Purpose |
|---|---|
| `codegen.jar` | Generate service communication code from `.siml` and state machines from `.fsml` |
| `mtrouter` | Route messages between processes and machines |
| `logcollector` / `logobserver` | Collect distributed logs / observe, store and control logging |
| Lusan (optional GUI) | Design interfaces; view live and recorded logs |

[Interface designer screenshot](./docs/img/lusan-service-if-general.png).
Libraries: `areg` (core), `aregextend` (utilities, SQLite wrapper), `areglogger` (log observer API).

## Agentic coding[![](./docs/img/pin.svg)](#agentic-coding)

An unfamiliar framework should not make a coding agent guess its API.
[`AGENTS.md`](./AGENTS.md) provides the entry point; generators write the plumbing,
API checks catch invented names, and executable scenarios exercise the result.
Python 3.10+ is needed for this tooling, not for ordinary C++ development.

In two published October 2026 baselines, **28/28 Areg runs** passed their assigned hidden checks,
versus **12/15 gRPC runs**. These are maintainer-authored tasks with unequal run counts and
different tooling; some prompt requirements differ, and runs could include fix cycles.
They evaluate those workflows, not general framework correctness.
[Sonnet baseline](./examples/ai-benchmark/baseline-2026-10-17.md) ·
[Terra baseline](./examples/ai-benchmark/baseline-2026-10-18.md).

[Workflow guide](./docs/wiki/09a-agentic-coding.md) ·
[Reproducible tasks](./examples/ai-benchmark/) ·
[Agent-written state machine in Lusan](./docs/img/screenshot-fsm-ai.png) ·
[Share a run, passed or failed](https://github.com/aregtech/areg-sdk/discussions/800).

---

## Getting started[![](./docs/img/pin.svg)](#getting-started)

<a id="ways-in"></a>
<a id="learning-path"></a>

| Your next step | Start here |
|---|---|
| Build a new application | [Quick start](#quick-start); `local`, `ipc` and `pubsub` setup modes |
| Add Areg to an existing project | [CMake integration](./docs/wiki/02b-cmake-integrate.md) |
| Inspect one implementation in three deployments | [`03_helloservice`](./examples/03_helloservice/): shared provider/consumer sources and separate models |
| Build the SDK and explore examples | [CMake build](./docs/wiki/01b-cmake-build.md) · [Visual Studio](./docs/wiki/01c-msvc-build.md) · [Examples](./examples/README.md) |
| Prepare a development machine | [Build environment](./docs/wiki/01e-build-environment.md); GCC, Clang or MSVC |

## Roadmap[![](./docs/img/pin.svg)](#roadmap)

Next: the 2.0.0 release and vcpkg port, followed by RTOS support starting with Zephyr.
Planned: shared-memory transport and optional TLS for `mtrouter`.
Language bindings and additional transports remain open for community input.
These are future directions, not current capabilities.

## Documentation[![](./docs/img/pin.svg)](#documentation)

[Wiki](./docs/wiki/README.md) for concepts and guides ·
[HOWTO](./docs/HOWTO.md) for practical tasks ·
[Examples](./examples/README.md) for working code ·
[AGENTS.md](./AGENTS.md) for coding agents.

## License[![](./docs/img/pin.svg)](#license)

[Apache License 2.0](./LICENSE.txt), permitting open-source and commercial use.
Optional commercial licensing, training and dedicated support: **info[at]areg[dot]tech**.
Commercial support is not required to use the Apache-licensed SDK commercially.

## Community[![](./docs/img/pin.svg)](#community)

[Ask a question or share a project](https://github.com/aregtech/areg-sdk/discussions) ·
[Report an issue](https://github.com/aregtech/areg-sdk/issues) ·
[Contribute](./CONTRIBUTING.md).
Reproductions, measurements on your hardware, and documentation improvements are welcome.

[![Use Areg SDK](https://img.shields.io/badge/uses-areg--sdk-blue)](https://github.com/aregtech/areg-sdk)

---

*Areg (Արեգ) – Old Armenian: the Sun.*
