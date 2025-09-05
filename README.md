<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.aregtech.com"><img align="center" src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-sdk-1280x360px-logo.png" alt="Areg SDK Home" style="width:100%;height:100%"/></a>
  <br /><br /><strong>Areg SDK</strong>
</h1>

*Concurrency without the complexity.*

[![Latest release](https://img.shields.io/github/v/release/aregtech/areg-sdk?label=Latest%20release&style=social)](https://github.com/aregtech/areg-sdk/releases/tag/v1.5.0)
[![GitHub commits](https://img.shields.io/github/commits-since/aregtech/areg-sdk/v1.5.0.svg?style=social)](https://GitHub.com/aregtech/areg-sdk/commit/)
[![Stars](https://img.shields.io/github/stars/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/stargazers)
[![Fork](https://img.shields.io/github/forks/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/network/members)
[![Watchers](https://img.shields.io/github/watchers/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/watchers)
[![Wiki Pages](https://img.shields.io/badge/Areg%20Wiki%20Pages-8-brightgreen?style=social&logo=wikipedia)](https://github.com/aregtech/areg-sdk/wiki/)

---

## Introduction[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#introduction)

*Areg is like a network operator for C++ programs — a lightweight framework automating software communication.*

Threads, IPC, and distributed messaging often slow development and create fragile glue code. By combining **async RPC**, **auto-discovery**, and **automatic messaging**, Areg SDK accelerates building and debugging distributed services across **threads, processes, and devices**, letting developers focus on features rather than plumbing.

---

## Project Status[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#project-status)

<table class="no-border">
  <tr>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml" alt="CMake"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml/badge.svg" alt="CMake build"/></a></td>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/msbuild.yml" alt="MS Build"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/msbuild.yml/badge.svg" alt="MS Build"/></a></td>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml" alt="CodeQL"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml/badge.svg" alt="CodeQL"/></a></td>
  </tr>
  <tr>
    <td><img src="https://img.shields.io/badge/Solution-C++17-blue.svg?style=flat&logo=c%2B%2B&logoColor=b0c0c0&labelColor=363D44" alt="C++ solution"/></td>
    <td><img src="https://img.shields.io/badge/OS-linux%20%7C%20windows-blue?style=flat&logo=Linux&logoColor=b0c0c0&labelColor=363D44" alt="Operating systems"/></td>
    <td colspan="2"><img src="https://img.shields.io/badge/CPU-x86%20%7C%20x86__64%20%7C%20arm%20%7C%20aarch64-blue?style=flat&logo=amd&logoColor=b0c0c0&labelColor=363D44" alt="CPU Architect"/></td>
  </tr>
</table>

---

## Table of contents[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#table-of-contents)
- [Why Areg SDK?](#why-areg-sdk)
  - [Top 5 Benefits](#top-5-benefits)
  - [Compared to Alternatives](#compared-to-alternatives)
- [Features](#features)
- [Areg SDK Decision Checklist](#areg-sdk-decision-checklist)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Quick Start](#quick-start)
  - [Learning Path](#learning-path)
  - [Integration](#integration)
- [Modules](#modules)
- [Architecture](#architecture)
- [Use Cases](#use-cases)
  - [IoT: Mist-to-Cloud](#iot--mist-to-cloud)
  - [Beyond Embedded](#beyond-embedded)
  - [Driverless Devices](#driverless-devices)
  - [Simulation & Testing](#Simulation-Testing)
- [Roadmap](#roadmap)
- [Documentation](#documentation)
- [License](#license)
- [Call to action](#call-to-action)

> [!IMPORTANT]
> For full technical guidance of building, using and integrating Areg SDK, see the [following documents](./docs/wiki/).

---

## Why Areg SDK?[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#why-areg-sdk)

Software complexity rarely comes from algorithms—it comes from **frameworks that don’t scale**. Threads, processes, and distributed components quickly lead to **manual wiring, hidden bugs, and costly rewrites**. Late framework decisions can lock teams into fragile architectures, wasting weeks of development and refactoring.

**Areg SDK eliminates these risks.** Its unified **Pub/Sub + Request-Reply model** handles threading, IPC, and distributed execution across threads, processes, and devices—so developers can focus on **features, not plumbing**.

### Top 5 Benefits

1. **Eliminate manual threading & boilerplate:**
   Define components and threads declaratively; the runtime manages routing, queuing, and execution context automatically.

2. **Unified local & remote API + low-latency async RPC:**  
   Call services the same way in-thread, across processes, or over the network. RPCs and events are queued for **non-blocking, responsive execution**.

3. **Self-organizing service mesh:**
   Services auto-discover and connect, forming a dynamic mesh that **handles dependencies automatically**.

4. **Fault-tolerant by design:**
   Providers can appear, disappear, or restart without disrupting consumers. Watchdog-protected threads recover stuck components safely.

5. **Lightweight observability & profiling:**
   Scoped logging captures entry/exit timings, while the log viewer highlights per-method latency **without intrusive instrumentation**.

Tested on **Linux and Windows**, scalable from prototypes to multi-node production systems.

### Compared to Alternatives

**Areg SDK helps developers build scalable, reliable, and maintainable C++ systems without drowning in IPC, threading, or distributed complexity.**

| Feature          | Areg SDK                        | Competitors (gRPC, DDS, ZeroMQ)              |
| ---------------- | ------------------------------- | -------------------------------------------- |
| Ease of Use      | ✅ Automated setup              | ⚠️ Manual, boilerplate, complex             |
| Automation       | ✅ Codegen, modeling, dispatch  | ⚠️ Manual setup, stubs only                 |
| Auto-Discovery   | ✅ Self-organizing service mesh | ✅ Topics (DDS), ⚠️ Control-plane or Manual |
| Fault-Tolerance  | ✅ Watchdog & auto-restart      | ✅ QoS (DDS), ⚠️ Retries or Manual          |
| Request-Reply    | ✅ Native Object RPC in service | ✅ RPC (gRPC), ⚠️ Over topics or Patterns   |
| Pub/Sub          | ✅ Native, built-in in service  | ✅ Topics (DDS), ⚠️ Add-ons or Manual       |
| Built-in Logging | ✅ Integrated, dynamic + viewer | ⚠️ Vendor-specific (DDS), ❌ External       |
| Dev Time Saved   | ✅ Significant, via automation  | ⚠️ Limited, external tooling                |

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Features[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#features)

**Core Model**

* **Unified Service Model** — define threads and components in models; load/unload dynamically.
* **Virtual Clients & Servers** — reusable micro-servers/clients, mapped across threads/processes.
* **Async RPC & Events** — non-blocking RPC/events executed in the target thread.

**Automations**

* **Auto-Discovery** — consumers find providers automatically.  
* **Code Generation & Serialization** — generate stubs and proxies with built-in serialization.
* **Automatic Dispatching** — routes RPC calls to target objects without manual wiring.

**Tooling**

* **Service Interface Editor** — define APIs in XML; generate type-safe C++ code.
* **Runtime Logging & Control** — scoped logs with filters; debug without code changes.
* **Performance Measurement** — per-method timing for latency profiling and bottleneck detection.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Areg SDK Decision Checklist[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#areg-sdk-decision-checklist)

Unsure if Areg SDK fits your project? ✅ Take this quick self-check:

* [ ] Are **threads, IPC, or service setup** consuming too much time?  
* [ ] Would **automatic dispatching** of requests/events speed your workflow?  
* [ ] Is **debugging distributed services** painful?  
* [ ] Do you want services to **auto-discover and connect**, reducing setup effort?  
* [ ] Would **integrated logging** save time troubleshooting?  
* [ ] Would **automated code generation** save effort?  
* [ ] Do you want to focus on **core features** rather than manual integration and debugging?  

💡 **Tip:** Checking **4 or more** boxes means Areg SDK is a strong fit.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Getting Started[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#getting-started)

### Prerequisites

* **C++17-compatible compiler**: GCC, Clang/LLVM, MSVC, MinGW
* **CMake ≥ 3.20**
* **Java ≥ 17** (for code generation tools)

> **Supported OS:** Linux, Windows</br>
> **Supported Hardware:** x86, AMD64, ARM32, AArch64

For **compiler options, setup, and troubleshooting**, see the [CMake Configuration Guide](./docs/wiki/02a-cmake-config.md) and [README Wiki](./docs/wiki/README.md).

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

> [This example source code](examples/01_minimalrpc/) demonstrates a simple RPC:
>
> * **Provider** and **Consumer** run in different threads.
> * Consumer calls Provider request when connects.
> * Provider prints a message.
> * App exits after completion.

**Message Flow:**

```text
main() → load model → system auto-connects components → Consumer calls request → Provider handles request → prints & signals quit → main() releases & exits
```

### Learning Path

1. [**01\_minimalrpc**](examples/01_minimalrpc/) – basic RPC between 2 components.
2. [**02\_minimalipc**](examples/01_minimalipc/) – IPC across processes, **requires `mcrouter`**.
3. [**03\_helloservice**](examples/03_helloservice/) – multithreaded RPC + IPC using same components, **requires `mcrouter`**.

👉 Explore more [**more examples**](./examples/) for advanced features. IPC apps **require** `mcrouter` running ([mcrouter guide](./docs/wiki/05a-mcrouter.md)). The `mcrouter` binary can run as a console application or system managed service under Linux and Windows.

### Integration

* **CMake FetchContent:** [Integration Guide](./docs/wiki/02c-cmake-integrate.md)
* **Demo Project:** [areg-sdk-demo](https://github.com/aregtech/areg-sdk-demo)
* **Qt-based integration:** [areg-sdk-tools](https://github.com/aregtech/areg-sdk-tools)

> 💡 Advanced builds (IDE setup, cross-compilation, disabling tests/examples) documented in [Wiki](./docs/wiki/).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Modules[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#modules)

| Module                                                              | Purpose & Description                                                                                                                         |
| ------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------- |
| [Areg Framework (`areg`)](./docs/wiki/01-introduction.md)           | Core IPC + threading library. Handles ORPC, auto-dispatch, and safe component execution—removes glue code and manual wiring.                  |
| [Multitarget Router (`mcrouter`)](./docs/wiki/05a-mcrouter.md)      | **Required for IPC** to routes messages across processes and devices, enabling to for mesh of service. Runs as console app or system service. |
| [Log Collector (`logcollector`)](./docs/wiki/04d-logcollector.md)   | Collects logs from multiple processes. Simplifies debugging distributed systems. Runs as console app or system service                        |
| [Log Observer (`logobserver`)](./docs/wiki/04c-logobserver.md)      | CLI for saving, controlling and filtering logs in runtime.                                                                                    |
| [Code Generator (`codegen.jar`)](./docs/wiki/03a-code-generator.md) | Generates stubs, proxies, and serialization code. Eliminates repetitive boilerplate.                                                          |
| [Lusan (UI Tool)](https://github.com/aregtech/areg-sdk-tools)       | GUI for designing services and API, view and control logs in runtime, and monitoring performance. Helps understand runtime behavior visually. |
| [Examples](./examples/README.md)                                    | Sample apps demonstrating RPC, Pub/Sub, auto-discovery, logging, and other features. Speeds learning and onboarding.                          |

---

## Architecture

Areg uses an **interface-centric Object RPC (ORPC)** model. Applications expose **Service Providers** and interact via **Service Consumers** using auto-generated **Stubs & Proxies**, communicating through the **Multitarget Router**. Services work without knowing network locations, enabling seamless distributed communication.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/interface-centric.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/interface-centric.png" alt="Interface-centric communication diagram" style="width:50%;height:50%"/></a></div>

Supports both **Client-Server (Request-Reply)** and **Publish-Subscribe (PubSub)** patterns, designed for **multithreading**, **multiprocessing**, and future **internet-scale deployment**, ensuring reliable messaging.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Use Cases

### IoT: Mist-to-Cloud

* **Problem:** Traditionally, edge devices stream data to servers, causing latency, privacy risks, and heavy network usage. Wireless communication consumes more power than CPU, which is critical for small devices.<br/>
**Solution:** Areg lets **services run directly on devices**, forming a **mist network** of micro-servers that process and combine data locally.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/mist-network.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/mist-network.png" alt="IoT Mist-to-Cloud network diagram" style="width:70%;height:70%"/></a></div>

**Benefit:** Service-enabled devices form a **mesh at the edge** and communicate in near real-time, acting as a fast, autonomous system without a centralized server. **Private data remains on-device**, reducing streaming to the cloud.

---

### Beyond Embedded

**Problem:** Small devices often lack scalable service infrastructure, while larger systems need multithreading/multiprocessing support.<br/>
**Solution:** Areg provides **multithreading, inter-process, and distributed service orchestration**, turning devices and PCs into **service-enabled nodes**.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-services.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-services.png" alt="Service types and message handling diagram" style="width:70%;height:70%"/></a></div>

* Local Services – internal, for multithreading.
* Public Services – across processes, for multiprocessing.
* Internet Services – not supported yet.

**Benefit:** Devices expose **public services** that can be seamlessly accessed and reused by other applications and devices.

---

### Driverless Devices

**Problem:** Writing device drivers is slow, complex, OS-specific, and error-prone.<br/>
**Solution:** Areg enables **driverless, service-enabled devices**, exposing hardware as portable services.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/driverless-solution.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/driverless-solution.png" alt="Areg driverless solution" style="width:70%;height:70%"/></a></div>

Benefit: Engineers can **prototype and test hardware faster**, fix issues early, and build **network-accessible devices** without OS-dependent drivers.

---

### Simulation & Testing

**Problem:** Testing distributed systems or devices is costly and hardware-dependent.<br/>
**Solution:** Areg enables **service and data simulation**, providing realistic environments for testing and prototyping.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/software-layers.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/software-layers.png" alt="Software simulation layers" style="width:70%;height:70%"/></a></div>

**Benefit:** Since **service location is transparent**, developers can move data-layer services into external apps that simulate data, making it easy to **develop and stress-test safely** across one or multiple applications.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Roadmap[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#roadmap)

Areg SDK evolves to accelerate complex applications on **Desktop**, **Embedded**, and **IoT edge**, reducing development time by \~30% while keeping automation, reliability, and flexibility.

**Upcoming Features:**

* Multi-channel & multi-protocol communication
* Internet services – cloud-ready applications

**[Lusan GUI Tool](https://github.com/aregtech/areg-sdk-tools/)** (in active development)

* Service API Designer (usable) – visually create/edit interfaces
* Interactive Log Viewer (usable) – real-time log/performance analysis
* Service Testing & Simulation – simulate services/data for testing

GUI tools in the **[Areg SDK Tools repository](https://github.com/aregtech/areg-sdk-tools)**.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Documentation

* [Getting Started](./docs/wiki/getting-started.md)
* [CMake Configuration](./docs/wiki/02a-cmake-config.md)
* [Architecture](./docs/wiki/architecture.md)
* [Examples](./examples/README.md)
* [Troubleshooting](./docs/wiki/README.md)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## License[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#license)

Areg SDK is released under the **[Apache License Version 2.0](./LICENSE.txt)**, a permissive open-source license.

**Commercial licensing options** are available, providing:

* Dedicated commercial support
* Full rights to distribute software without open-source obligations

For details, support, training, or partnerships, visit **[Aregtech](https://www.aregtech.com/)** or contact **info\[at]aregtech\[dot]com**.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Call to Action[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#call-to-action)

Help grow the Areg SDK community and improve the framework for developers working on embedded systems, multithreaded/multiprocess apps, real-time data, and IoT projects.  

* Explore [open issues](https://github.com/aregtech/areg-sdk/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22)
* Share ideas or request features via [issues](https://github.com/aregtech/areg-sdk/issues) or [discussions](https://github.com/aregtech/areg-sdk/discussions)
* Submit pull requests following project style
* ⭐ Star Areg SDK on GitHub

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

**Follow us** at<br/>
![X (formerly Twitter) Follow](https://img.shields.io/twitter/follow/aregtech) 
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Aregtech-blue?style=flat&logo=linkedin&logoColor=b0c0c0&labelColor=363D44)](https://www.linkedin.com/company/aregtech)
[![Join the chat at https://gitter.im/areg-sdk/community](https://badges.gitter.im/areg-sdk/community.svg)](https://gitter.im/areg-sdk/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
