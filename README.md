<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
  <a name="logo" href="https://www.aregtech.com">
    <img 
      align="center" 
      src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-sdk-1280x360px-logo.png" 
      alt="Areg SDK – C++ Middleware and Framework for Remote Procedure Call (RPC) and Inter-Process Communication (IPC)"
      title="Areg SDK – Lightweight Cross-Platform C++ Framework and Middleware for RPC, IPC, and Distributed Systems"
      style="width:100%;height:100%" 
    />
  </a>
</h1>

[![Latest release](https://img.shields.io/github/v/release/aregtech/areg-sdk?label=Latest%20release&style=social)](https://GitHub.com/aregtech/areg-sdk/releases/tag/v1.5.0)
[![GitHub commits](https://img.shields.io/github/commits-since/aregtech/areg-sdk/v1.5.0.svg?style=social)](https://GitHub.com/aregtech/areg-sdk/commit/)
[![Stars](https://img.shields.io/github/stars/aregtech/areg-sdk?style=social)](https://GitHub.com/aregtech/areg-sdk/stargazers)
[![Fork](https://img.shields.io/github/forks/aregtech/areg-sdk?style=social)](https://GitHub.com/aregtech/areg-sdk/network/members)
[![Watchers](https://img.shields.io/github/watchers/aregtech/areg-sdk?style=social)](https://GitHub.com/aregtech/areg-sdk/watchers)
[![Wiki Pages](https://img.shields.io/badge/Areg%20Wiki%20Pages-8-brightgreen?style=social&logo=wikipedia)](https://GitHub.com/aregtech/areg-sdk/wiki/)

---

*Areg is like a network operator, but for C++ programs — automating communication in distributed systems.*

Threads, IPC, and distributed messaging often slow development. **Areg SDK** combines **async RPC**, **auto-discovery**, and **automatic messaging**, letting developers build and debug distributed services across threads, processes, or devices — focusing on features instead of plumbing.

---

## Project Status[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#project-status)

<table class="no-border">
  <tr>
    <td><a href="https://GitHub.com/aregtech/areg-sdk/actions/workflows/cmake.yml"><img src="https://GitHub.com/aregtech/areg-sdk/actions/workflows/cmake.yml/badge.svg" alt="CMake build"/></a></td>
    <td><a href="https://GitHub.com/aregtech/areg-sdk/actions/workflows/msbuild.yml"><img src="https://GitHub.com/aregtech/areg-sdk/actions/workflows/msbuild.yml/badge.svg" alt="MS Build"/></a></td>
    <td><a href="https://GitHub.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml"><img src="https://GitHub.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml/badge.svg" alt="CodeQL"/></a></td>
  </tr>
  <tr>
    <td><img src="https://img.shields.io/badge/Solution-C++17-blue.svg?style=flat&logo=c%2B%2B&logoColor=b0c0c0&labelColor=363D44" alt="C++ solution"/></td>
    <td><img src="https://img.shields.io/badge/OS-linux%20%7C%20windows-blue?style=flat&logo=Linux&logoColor=b0c0c0&labelColor=363D44" alt="Operating systems"/></td>
    <td><img src="https://img.shields.io/badge/CPU-x86%20%7C%20x86__64%20%7C%20arm%20%7C%20aarch64-blue?style=flat&logo=amd&logoColor=b0c0c0&labelColor=363D44" alt="CPU Architectures"/></td>
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
- [Core Modules and Architecture](#core-modules-and-architecture)
- [Use Cases: Distributed, RPC and IPC](#use-cases-distributed-rpc-and-ipc)
  - [IoT: Mist-to-Cloud](#iot-mist-to-cloud)
  - [Beyond Embedded](#beyond-embedded)
  - [Driverless Devices](#driverless-devices)
  - [Simulation & Testing](#simulation--testing)
- [Roadmap](#roadmap)
- [Documentation](#documentation)
- [License](#license)
- [Call to Action](#call-to-action)

> [!IMPORTANT]
> Full technical guidance for building, using, and integrating Areg SDK is available in the [documentation](./docs/wiki/ "Areg SDK documentation").

---

## Why Areg SDK?[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#why-areg-sdk "Why Areg SDK Lightweight C++ Framework")

Software complexity rarely comes from algorithms — it comes from **frameworks that don’t scale**. Threads and IPC introduce **manual wiring, hidden bugs, and costly rewrites**. The wrong **communication framework** locks teams into fragile architectures and wastes weeks.

**Areg SDK removes that glue.** It unifies service interaction across threads, processes, or devices. With built-in **async RPC**, **Pub/Sub messaging**, and **auto-discovery service mesh**, teams can scale **from prototype to fully distributed systems** with minimal effort.

### Top 5 Benefits

1. **Eliminate manual threading**<br/>
   Declare threads and components in a model; loading the model auto-instantiates, executes, and monitors them with watchdogs.

2. **One API, everywhere**<br/>
   Call services the same way locally or remotely — always async and non-blocking.

3. **Self-organizing service mesh**<br/>
   Auto-discovery connects services without manual setup.

4. **Fault-tolerant by design**<br/>
   Components join or leave without disruption; watchdogs restart failed threads with components.

5. **Lightweight observability**<br/>
   Scoped logging with a log viewer shows per-method duration when enabled.

💡 **When to use:** Linux & Windows, embedded and desktop C++ apps, scaling from prototype to multi-node.<br/>
⚠️ **When not to use:** RTOS (at the moment), web, or non-C++ apps — use web/RTOS-specific tools instead.

### Compared to Alternatives

| Feature          | Areg SDK                        | Competitors (gRPC, DDS, ZeroMQ)                      |
| ---------------- | ------------------------------- | ---------------------------------------------------- |
| Ease of Use      | ✅ Automated setup              | ⚠️ Manual, boilerplate, complex                     |
| Automation       | ✅ Codegen, modeling, dispatch  | ⚠️ Manual setup, stubs only                         |
| Auto-Discovery   | ✅ Self-organizing service mesh | ✅ DDS: built-in, ⚠️ gRPC/ZeroMQ: external          |
| Fault-Tolerance  | ✅ Watchdog & auto-restart      | ✅ DDS: QoS, ⚠️ gRPC/ZeroMQ: Retries/Manual         |
| Request-Reply    | ✅ Native Object RPC in service | ✅ gRPC: RPC, ⚠️ DDS/ZeroMQ: Over topics/patterns   |
| Pub/Sub          | ✅ Native, built-in in service  | ✅ DDS: Topics, ⚠️ Add-ons/Manual                   |
| Built-in Logging | ✅ Integrated, dynamic + viewer | ⚠️ Vendor-specific (DDS) or External                |
| Dev Time Saved   | ✅ Significant, via automation  | ⚠️ Limited, external tooling                        |

### Decision Checklist

* [ ] Are **threads, IPC, or service wiring** consuming too much time?
* [ ] Would **automatic dispatching and code generation** remove boilerplate?
* [ ] Is **debugging** across distributed components slowing delivery?
* [ ] Do you need **auto-discovery and runtime reconnection** across systems?
* [ ] Could a **unified solution across threads, processes, and devices** reduce errors and accelerate delivery?

💡 **Tip:** If you answer **yes** to 3+ questions, Areg SDK is a strong candidate.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Getting Started[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#getting-started)

### Prerequisites

* **C++17-compatible compiler**: GCC, Clang/LLVM, MSVC, MinGW  
* **CMake ≥ 3.20**  
* **Java ≥ 17** (required for code generation tools)

> **Supported OS:** Linux, Windows</br>
> **Supported Hardware:** x86, x86_64, ARM, AArch64

See [CMake Configuration Guide](./docs/wiki/02a-cmake-config.md) and [README Wiki](./docs/wiki/README.md) for compiler options, setup instructions, and troubleshooting.

### Quick Start

1. Clone and build the SDK:

```bash
git clone https://GitHub.com/aregtech/areg-sdk.git
cd areg-sdk
cmake -B build
cmake --build build -j 12
```

2. Run the pre-built minimal RPC example:

```bash
./product/build/<compiler>/<os>-<hw>-<build-type>-<lib-type>/bin/01_minimalrpc
```

> [!TIP]
> [Complete example source code](examples/01_minimalrpc/src/main.cpp) demonstrates a fully working minimal RPC:
>
> * Provider & Consumer run in separate threads
> * Consumer calls Provider when connected
> * Provider prints a message
> * Application exits after completion

**Message Flow:**

```text
🟢 main() → 🏗 load model → 🔗 auto-connect → 📤 Consumer requests → 📥 Provider responds → 🖨 prints → ✅ quit
```

### Learning Path

1. [**01\_minimalrpc**](examples/01_minimalrpc/) – minimal RPC example between 2 components.
2. [**02\_minimalipc**](examples/02_minimalipc/) – minimal IPC example across processes, **requires `mcrouter`**.
3. [**03\_helloservice**](examples/03_helloservice/) – multithreaded RPC + IPC using the same components, **requires `mcrouter`**.

> [!IMPORTANT]
> For IPC, **`mcrouter` must be running**. See [mcrouter documentation](./docs/wiki/05a-mcrouter.md).

### Integration

* **CMake FetchContent:** [Integration Guide](./docs/wiki/02c-cmake-integrate.md)
* **Demo Project:** [areg-sdk-demo](https://GitHub.com/aregtech/areg-sdk-demo)
* **Qt-based Integration:** [areg-sdk-tools](https://GitHub.com/aregtech/areg-sdk-tools)

> 💡 Advanced builds (IDE setup, cross-compilation, disabling tests/examples) are documented in [Wiki](./docs/wiki/).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Core Modules and Architecture[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#core-modules--architecture)

| Module                                                              | Purpose                                                                                                                                |
| ------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------- |
| [Areg Framework (`areg`)](./docs/HelloService.md)                   | Cross-platform C++17 framework with middleware layer: automated, service-oriented Object RPC (ORPC) and IPC for distributed computing. |
| [Multitarget Router (`mcrouter`)](./docs/wiki/05a-mcrouter.md)      | **Required for IPC** — routes messages across processes/devices to form a service mesh. Runs as console app or system service.         |
| [Log Collector (`logcollector`)](./docs/wiki/04d-logcollector.md)   | Centralized log collection for distributed apps — helps monitoring and analysis. Runs as console app or system service.                |
| [Log Observer (`logobserver`)](./docs/wiki/04c-logobserver.md)      | CLI tool to receive, filter, and save runtime logs — useful for debugging.                                                             |
| [Code Generator (`codegen.jar`)](./docs/wiki/03a-code-generator.md) | Generates stubs, proxies, and serialization from service object API (SIML) — eliminates boilerplate.                                   |
| [Lusan (UI Tool)](https://GitHub.com/aregtech/areg-sdk-tools)       | GUI app to design service APIs, visualize and filter logs, and analyze performance.                                                    |
| [Examples](./examples/README.md)                                    | Sample projects (RPC, Pub/Sub, IPC, FSM, auto-discovery, etc.) — the fastest way to validate Areg in action.                           |

Areg implements an **interface-centric Object RPC (ORPC)** model. Applications expose **Service Providers** and interact via **Service Consumers** with auto-generated **Stubs, Proxies & Rules**, communicating through the **Multitarget Router**. Services (logical micro-servers) need not know physical network locations — enabling portable **C++ microservices** and scalable **service-oriented architecture (SOA)**.

<div align="center"><a href="https://GitHub.com/aregtech/areg-sdk/blob/master/docs/img/interface-centric.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/interface-centric.png" alt="Interface-centric communication diagram" style="width:50%;height:50%"/></a></div>

Supports **Client-Server (Request-Reply)** and **Publish-Subscribe (Pub/Sub)** patterns, designed for **multithreading**, **multiprocessing**, and **distributed systems** with low-latency requirements.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Use Cases: Distributed, RPC and IPC[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#use-cases-distributed-rpc-and-ipc)

### IoT: Mist-to-Cloud

Edge devices often stream raw data to central servers, creating latency, network load, and privacy issues. With **Areg**, services run on devices, forming a **mist network of micro-servers** that process and aggregate data locally.


<div align="center"><a href="https://GitHub.com/aregtech/areg-sdk/blob/master/docs/img/mist-network.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/mist-network.png" alt="IoT Mist-to-Cloud network diagram" style="width:70%;height:70%"/></a></div>

**Benefit:** Low-latency, autonomous edge mesh; sensitive data remains on-device; reduced cloud traffic.

---

### Beyond Embedded

Many small devices lack scalable infrastructure. **Areg** transforms embedded applications into **distributed modules** that scale across remote nodes and platforms, orchestrating **multithreaded and multiprocess C++ services**.

<div align="center"><a href="https://GitHub.com/aregtech/areg-sdk/blob/master/docs/img/areg-services.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-services.png" alt="Service types and message handling diagram" style="width:70%;height:70%"/></a></div>

* **Local Services** – multithreaded within a device  
* **Public Services** – accessible across processes and applications

**Benefit:** Seamlessly expose services to other apps and devices without modifying code.

---

### Driverless Devices

Device drivers are often **slow, complex, and platform-specific**. **Areg** lets you **expose hardware as portable, service-enabled components**, making devices platform-independent.

<div align="center"><a href="https://GitHub.com/aregtech/areg-sdk/blob/master/docs/img/driverless-solution.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/driverless-solution.png" alt="Areg driverless service-enabled diagram" style="width:70%;height:70%"/></a></div>

**Benefit:** Accelerates prototyping, testing, and development; enables early bug detection; devices are network-accessible.

---

### Simulation & Testing

Validating distributed systems is expensive and hardware-dependent. **Areg** allows simulation of **Data Layer services** in external applications, providing realistic environments to test behavior, performance, and fault-tolerance. Services appear **location-transparent** to higher layers.

<div align="center"><a href="https://GitHub.com/aregtech/areg-sdk/blob/master/docs/img/software-layers.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/software-layers.png" alt="Software 4 Layer Architecture: Simulate Data Layer" style="width:70%;height:70%"/></a></div>

**Benefit:** Transparent service location enables testing of higher-layer logic without real hardware dependencies.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Roadmap[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#roadmap)

Areg SDK continues to evolve for **Desktop and Embedded**, focusing on automation, reliability, and performance.

**Planned work:**

- Multi-channel support for multiplexed communications
- RTOS support (**Zephyr OS**) to bring Areg into real-time environments
- Lusan app improvements: performance, stability, and usability ([Areg SDK Tools](https://GitHub.com/aregtech/areg-sdk-tools))

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Documentation[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#documentation)

* [Installation and Build](./docs/wiki/README.md#1--installation-and-build) — cross-platform builds, toolchains, CMake integration for embedded and desktop targets
* [Build Options and Integrations](./docs/wiki/README.md#2--build-options-and-integrations) — FetchContent, packaging, and embedding Areg as a library
* [Development Tools](./docs/wiki/README.md#3--development-tools) — code generation for service-oriented development
* [Logging and Monitoring](./docs/wiki/README.md#4--logging-and-monitoring) — logcollector and logobserver usage
* [Networking and Communication](./docs/wiki/README.md#5--networking-and-communication) — multicast router for IPC and low-latency messaging
* [Persistence](./docs/wiki/README.md#6--persistence) — for local data storage in a text file
* [Troubleshooting](./docs/wiki/README.md#7--troubleshooting) — common issues and recommended solutions
* [Examples and Tests](./docs/wiki/README.md#8--examples-and-tests) — catalog of sample projects (RPC, IPC, Pub/Sub, FSM, and more)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## License[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#license)

Areg SDK is released under the **[Apache License 2.0](./LICENSE.txt)** — permissive and suitable for open-source and commercial use.

**Commercial licensing & services:** Dedicated support, training, and tailored licensing options are available for enterprise customers. Visit **[Aregtech](https://www.aregtech.com/)** or email **info\[at]aregtech\[dot]com** for details.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Call to Action[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#call-to-action)

Help grow the Areg SDK community:

* Explore [open issues](https://GitHub.com/aregtech/areg-sdk/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) and contribute
* Share ideas or request features via [issues](https://GitHub.com/aregtech/areg-sdk/issues) or [discussions](https://GitHub.com/aregtech/areg-sdk/discussions)
* Submit pull requests (see project contribution guidelines)
* ⭐ Star Areg SDK to accelerate discovery of distributed C++ solutions

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

**Follow us:**<br/>
[![X Follow](https://img.shields.io/twitter/follow/aregtech)](https://x.com/aregtech?lang=en)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Aregtech-blue?style=flat&logo=linkedin\&logoColor=b0c0c0\&labelColor=363D44)](https://www.linkedin.com/company/aregtech)
[![Gitter](https://badges.gitter.im/areg-sdk/community.svg)](https://gitter.im/areg-sdk/community)
