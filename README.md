<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.aregtech.com"><img align="center" src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-sdk-1280x360px-logo.png" alt="Areg SDK Home" style="width:100%;height:100%"/></a>
</h1>

*Concurrency without the complexity.*

[![Latest release](https://img.shields.io/github/v/release/aregtech/areg-sdk?label=Latest%20release&style=social)](https://github.com/aregtech/areg-sdk/releases/tag/v1.5.0)
[![GitHub commits](https://img.shields.io/github/commits-since/aregtech/areg-sdk/v1.5.0.svg?style=social)](https://GitHub.com/aregtech/areg-sdk/commit/)
[![Stars](https://img.shields.io/github/stars/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/stargazers)
[![Fork](https://img.shields.io/github/forks/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/network/members)
[![Watchers](https://img.shields.io/github/watchers/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/watchers)
[![Wiki Pages](https://img.shields.io/badge/Areg%20Wiki%20Pages-8-brightgreen?style=social&logo=wikipedia)](https://github.com/aregtech/areg-sdk/wiki/)

---

Threads, IPC, and distributed messaging often slow development and create fragile glue code. **Areg SDK** combines **async RPC**, **auto-discovery**, and **automatic messaging**, so developers can build and debug distributed services across threads, processes, and devices — focusing on features instead of plumbing.

---

## Project Status[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#project-status)

<table class="no-border">
  <tr>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml/badge.svg" alt="CMake build"/></a></td>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/msbuild.yml"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/msbuild.yml/badge.svg" alt="MS Build"/></a></td>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml/badge.svg" alt="CodeQL"/></a></td>
  </tr>
  <tr>
    <td><img src="https://img.shields.io/badge/Solution-C++17-blue.svg?style=flat&logo=c%2B%2B&logoColor=b0c0c0&labelColor=363D44" alt="C++ solution"/></td>
    <td><img src="https://img.shields.io/badge/OS-linux%20%7C%20windows-blue?style=flat&logo=Linux&logoColor=b0c0c0&labelColor=363D44" alt="Operating systems"/></td>
    <td colspan="2"><img src="https://img.shields.io/badge/CPU-x86%20%7C%20x86__64%20%7C%20arm%20%7C%20aarch64-blue?style=flat&logo=amd&logoColor=b0c0c0&labelColor=363D44" alt="CPU Architectures"/></td>
  </tr>
</table>

---

## Table of Contents[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#table-of-contents)
- [Why Areg SDK?](#why-areg-sdk)
  - [Top 5 Benefits](#top-5-benefits)
  - [Compared to Alternatives](#compared-to-alternatives)
  - [Decision Checklist](#areg-sdk-decision-checklist)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Quick Start](#quick-start)
  - [Learning Path](#learning-path)
  - [Integration](#integration)
- [Core Modules & Architecture](#core-modules--architecture)
- [Use Cases](#use-cases)
  - [IoT: Mist-to-Cloud](#iot-mist-to-cloud)
  - [Beyond Embedded](#beyond-embedded)
  - [Driverless Devices](#driverless-devices)
  - [Simulation & Testing](#simulation--testing)
- [Roadmap](#roadmap)
- [Documentation](#documentation)
- [License](#license)
- [Call to Action](#call-to-action)

> [!IMPORTANT]
> Full technical guidance for building, using, and integrating Areg SDK is in the [documentation](./docs/wiki/).

---

## Why Areg SDK?[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#why-areg-sdk)

Software complexity rarely comes from algorithms — it comes from **frameworks that don’t scale**. Threads, IPC, and distributed components quickly lead to **manual wiring, hidden bugs, and costly rewrites**. Late framework choices can lock teams into fragile architectures and waste weeks of engineering time.

**Areg SDK solves that problem.** It provides an interface-centric runtime that unifies **async RPC**, **Pub/Sub**, and **auto-discovery** so services behave the same whether they run in-thread, across processes, or on remote devices — helping teams focus on features, not plumbing.

### Top 5 Benefits

1. **Eliminate manual threading**<br/>
   Define components and threads declaratively; the runtime handles routing, queuing, and execution automatically.

2. **Unified local & remote API**<br/>
   Call services the same way in-thread, across processes, or over the network. All calls are **non-blocking and responsive**.

3. **Self-organizing service mesh**<br/>
   Automatic discovery connects and reconnects services across threads, processes, and devices.

4. **Fault-tolerant by design**<br/>
   Providers can appear/disappear without disrupting consumers. Watchdog-protected threads recover safely.

5. **Lightweight observability**<br/>
   Scoped logging and a log viewer surface per-method latency without intrusive instrumentation.

Tested on **Linux and Windows**; scales from prototypes to multi-node deployments.

### Compared to Alternatives

| Feature          | Areg SDK                        | Competitors (gRPC, DDS, ZeroMQ)                  |
| ---------------- | ------------------------------- | ------------------------------------------------ |
| Ease of Use      | ✅ Automated setup              | ⚠️ Manual, boilerplate, complex                 |
| Automation       | ✅ Codegen, modeling, dispatch  | ⚠️ Manual setup, stubs only                     |
| Auto-Discovery   | ✅ Self-organizing service mesh | ✅ DDS: built-in, ⚠️ gRPC/ZeroMQ: external      |
| Fault-Tolerance  | ✅ Watchdog & auto-restart      | ✅ DDS: QoS, ⚠️ gRPC/ZeroMQ: Retries/Manual      |
| Request-Reply    | ✅ Native Object RPC in service | ✅ gRPC: RPC, ⚠️ DDS/ZeroMQ: Over topics/patterns|
| Pub/Sub          | ✅ Native, built-in in service  | ✅ DDS: Topics, ⚠️ Add-ons/Manual               |
| Built-in Logging | ✅ Integrated, dynamic + viewer | ⚠️ Vendor-specific (DDS) or External            |
| Dev Time Saved   | ✅ Significant, via automation  | ⚠️ Limited, external tooling                    |

### Decision Checklist

* [ ] Are **threads, IPC, or service wiring** consuming too much development time?
* [ ] Would **automatic dispatching and code generation** remove repeated boilerplate?
* [ ] Is **debugging** across threads/processes/devices slowing delivery?
* [ ] Do you need **auto-discovery and runtime reconnection** across processes/devices?
* [ ] Would a **unified API across threads, processes, and devices** cut months off your release schedule?

💡 **Tip:** If you answer **yes** to 3 or more, Areg SDK is a strong candidate.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Getting Started[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#getting-started)

### Prerequisites

* **C++17-compatible compiler**: GCC, Clang/LLVM, MSVC, MinGW  
* **CMake ≥ 3.20**  
* **Java ≥ 17** (for code generation tools)

> **Supported OS:** Linux, Windows</br>
> **Supported Hardware:** x86, AMD64, ARM32, AArch64

See [CMake Configuration Guide](./docs/wiki/02a-cmake-config.md) and [README Wiki](./docs/wiki/README.md) for compiler options, setup, and troubleshooting.

### Quick Start

1. Clone and build the SDK:

```bash
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
cmake -B build
cmake --build build -j 12
```

2. Run the pre-built minimal RPC example:

```bash
./product/build/<compiler>/<os>-<hw>-<build-type>-<lib-type>/bin/01_minimalrpc
```

> [!TIP]
> [Complete example source code](examples/01_minimalrpc/) of a fully working minimal RPC:
>
> * Provider & Consumer run in different threads
> * Consumer calls Provider when connected
> * Provider prints a message
> * App exits after completion

**Message Flow:**

```text
main() → load model → auto-connect → Consumer requests → Provider responds → prints → quit
```

### Learning Path

1. [**01\_minimalrpc**](examples/01_minimalrpc/) – minimal example of basic RPC between 2 components.
2. [**02\_minimalipc**](examples/02_minimalipc/) – minimal example of IPC across processes, **requires `mcrouter`**.
3. [**03\_helloservice**](examples/03_helloservice/) – multithreaded RPC + IPC using same components, **requires `mcrouter`**.

> [!IMPORTANT]
> For IPC and multi-device communication, **`mcrouter` must be running**. See: [mcrouter documentation](./docs/wiki/05a-mcrouter.md).

### Integration

* **CMake FetchContent:** [Integration Guide](./docs/wiki/02c-cmake-integrate.md)
* **Demo Project:** [areg-sdk-demo](https://github.com/aregtech/areg-sdk-demo)
* **Qt-based Integration:** [areg-sdk-tools](https://github.com/aregtech/areg-sdk-tools)

> 💡 Advanced builds (IDE setup, cross-compilation, disabling tests/examples) documented in [Wiki](./docs/wiki/).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Core Modules & Architecture[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#core-modules--architecture)

| Module                                                              | Purpose                                                                                                                          |
| ------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| [Areg Framework (`areg`)](./docs/wiki/01-introduction.md)           | Core C++17 runtime: ORPC, thread-safe components, routing and IPC — removes glue code.                                           |
| [Multitarget Router (`mcrouter`)](./docs/wiki/05a-mcrouter.md)      | **Required for IPC** — routes messages across processes/devices to form the service mesh. Runs as console app or system service. |
| [Log Collector (`logcollector`)](./docs/wiki/04d-logcollector.md)   | Centralized log collection for distributed apps — helps monitoring and analysis.                                                 |
| [Log Observer (`logobserver`)](./docs/wiki/04c-logobserver.md)      | CLI tool to view, filter, and save runtime logs — useful for debugging.                                                          |
| [Code Generator (`codegen.jar`)](./docs/wiki/03a-code-generator.md) | Generates stubs, proxies, and serialization from service definitions (SIML) — eliminates boilerplate.                            |
| [Lusan (UI Tool)](https://github.com/aregtech/areg-sdk-tools)       | GUI to design APIs, visualize topology, and debug service interactions.                                                          |
| [Examples](./examples/README.md)                                    | Sample apps (RPC, Pub/Sub, IPC, auto-discovery) — quickest way to validate Areg.                                                 |

Areg follows an **interface-centric Object RPC (ORPC)** model. Applications expose **Service Providers** and interact via **Service Consumers** using auto-generated **Stubs & Proxies**, communicating through the **Multitarget Router**. Services work without knowing network locations, enabling seamless distributed communication.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/interface-centric.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/interface-centric.png" alt="Interface-centric communication diagram" style="width:50%;height:50%"/></a></div>

Supports both **Client-Server (Request-Reply)** and **Publish-Subscribe (Pub/Sub)** patterns, designed for **multithreading**, **multiprocessing**, and **distributed systems**.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Use Cases[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#call-to-action)

### IoT: Mist-to-Cloud

**Problem:** Traditionally, edge devices stream data to servers, causing latency, privacy risks, and heavy network usage. Wireless communication consumes more power than CPU, which is critical for small devices.<br/>
**Solution:** Areg lets **services run directly on devices**, forming a **mist network** of micro-servers that process and combine data locally.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/mist-network.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/mist-network.png" alt="IoT Mist-to-Cloud network diagram" style="width:70%;height:70%"/></a></div>

**Benefit:** Service-enabled devices form a **mesh at the edge** and communicate in near real-time, acting as a fast, autonomous system without a centralized server. **Private data remains on-device**, reducing streaming to the cloud.

---

### Beyond Embedded

**Problem:** Small devices lack scalable service infrastructure; larger systems require multithreading/multiprocessing.<br/>
**Solution:** Areg provides **multithreading, inter-process, and distributed service orchestration**, turning devices and PCs into **service-enabled nodes**.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-services.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-services.png" alt="Service types and message handling diagram" style="width:70%;height:70%"/></a></div>

* Local Services – for multithreading
* Public Services – across processes
* Internet Services – planned (see Roadmap)

**Benefit:** Devices expose **public services** that can be seamlessly accessed and reused by other applications and devices.

---

### Driverless Devices

**Problem:** Writing device drivers is slow, complex, OS-specific, and error-prone.<br/>
**Solution:** Areg enables **driverless, service-enabled devices**, exposing hardware as portable services.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/driverless-solution.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/driverless-solution.png" alt="Areg driverless solution" style="width:70%;height:70%"/></a></div>

**Benefit:** Engineers can **prototype and test hardware faster**, fix issues early, and build **network-accessible devices** without OS-dependent drivers.

---

### Simulation & Testing

**Problem:** Testing distributed systems or devices is costly and hardware-dependent.<br/>
**Solution:** Areg enables **service and data simulation**, providing realistic environments for testing and prototyping.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/software-layers.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/software-layers.png" alt="Software simulation layers" style="width:70%;height:70%"/></a></div>

**Benefit:** Since **service location is transparent**, developers can move data-layer services into external apps that simulate data, making it easy to **develop and stress-test safely** across one or multiple applications.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Roadmap[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#roadmap)

Areg SDK evolves for **Desktop, Embedded, and IoT edge**, reducing development time while keeping automation, reliability, and flexibility.

**Upcoming Features:**

* Multi-channel support for multiplexed communications
* RTOS support (Zephyr) for deeply embedded targets
* Lusan GUI improvements: speed, stability, usability ([Areg SDK Tools](https://github.com/aregtech/areg-sdk-tools))

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Documentation[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#call-to-action)

* [Installation and Build](./docs/wiki/README.md#1--installation-and-build)
* [Build Options and Integrations](./docs/wiki/README.md#2--build-options-and-integrations)
* [Development Tools](./docs/wiki/README.md#3--development-tools)
* [Logging and Monitoring](./docs/wiki/README.md#4--logging-and-monitoring)
* [Networking and Communication](./docs/wiki/README.md#5--networking-and-communication)
* [Persistence](./docs/wiki/README.md#6--persistence)
* [Troubleshooting](./docs/wiki/README.md#7--troubleshooting)
* [Examples and Tests](./docs/wiki/README.md#8--examples-and-tests)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## License[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#license)

Areg SDK is released under the **[Apache License Version 2.0](./LICENSE.txt)** — permissive open-source.

**Commercial licensing options**: Dedicated support and full rights to distribute without open-source obligations.

For support, training, or partnerships, visit **[Aregtech](https://www.aregtech.com/)** or contact **info\[at]aregtech\[dot]com**.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Call to Action[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#call-to-action)

Help grow the Areg SDK community:

* Explore [open issues](https://github.com/aregtech/areg-sdk/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22)
* Share ideas or request features via [issues](https://github.com/aregtech/areg-sdk/issues) or [discussions](https://github.com/aregtech/areg-sdk/discussions)
* Submit pull requests following project style
* ⭐ Star Areg SDK on GitHub

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

**Follow us:**<br/>
[![X Follow](https://img.shields.io/twitter/follow/aregtech)](https://x.com/aregtech?lang=en)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Aregtech-blue?style=flat\&logo=linkedin\&logoColor=b0c0c0\&labelColor=363D44)](https://www.linkedin.com/company/aregtech)
[![Gitter](https://badges.gitter.im/areg-sdk/community.svg)](https://gitter.im/areg-sdk/community)
