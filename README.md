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

*Areg is like a network operator, but for C++ programs — a lightweight framework that automates software communication.*

Threads, IPC, and distributed messaging often slow development, creating fragile glue code and hidden bugs. By combining async RPC, auto-discovery, and event-driven messaging, Areg SDK makes it faster and easier to build and debug distributed services across threads, processes, and devices—enabling developers to focus on features, not plumbing.

---

<!-- markdownlint-disable -->
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
  - [More than Embedded](#more-than-embedded)
  - [Distributed Solutions](#distributed-solutions)
  - [Driverless Devices](#driverless-devices)
  - [Simulation & Testing](#Simulation-Testing)
- [Roadmap](#pipeline-and-roadmap)
- [License](#license)
- [Call to action](#call-to-action)

> [!IMPORTANT]
> For full technical guidance of building and using Areg SDK, see the [following documents](./docs/wiki/).

---

## Why Areg SDK?[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#why-areg-sdk)

Software complexity rarely comes from algorithms—it comes from **frameworks that don’t scale**. Threads, processes, and distributed components quickly lead to **manual wiring, hidden bugs, and costly rewrites**. Late framework decisions can lock teams into fragile architectures, wasting weeks of development.

**Areg SDK removes these risks from day one.** Its unified **Pub/Sub + Request-Reply model** handles threading, IPC, and distributed execution across threads, processes, and devices—letting developers focus on **services and features, not plumbing**.

### Top 5 Benefits

1. **Eliminate manual threading & boilerplate:**  
   Define components and threads declaratively; the runtime handles routing, queuing, and execution context—no hand-wired threads, sockets, or fragile glue code.

2. **Unified local & remote API + low-latency async RPC:**  
   Call services the same way in-thread, across processes, or over the network. RPCs and events are queued in the target thread for **non-blocking, responsive behavior**.

3. **Self-organizing service mesh:**  
   Services auto-discover and connect logically regardless of location, forming a dynamic mesh that **handles cross-dependencies automatically**.

4. **Fault-tolerant by design:**  
   Providers can appear, disappear, or restart without breaking consumers. Watchdog-protected threads restart stuck components cleanly.

5. **Lightweight observability & profiling:**  
   Scope-based logging captures entry/exit timings automatically, and the log viewer highlights per-method latency **without intrusive instrumentation**.

Tested on **Linux and Windows**, scaling from single-device prototypes to multi-node production systems.

### Compared to Alternatives

👉 **Areg SDK helps developers build scalable, reliable, and maintainable C++ systems without drowning in IPC, threading, or distributed complexity.**

| Feature              | Areg SDK                               | Competitors (gRPC, DDS, ZeroMQ)                                |
| -------------------- | -------------------------------------- | -------------------------------------------------------------- |
| Ease of Use          | ✅ Automated setup, abstracted API     | ⚠️ Manual, boilerplate, complex                               |
| Automation           | ✅ Codegen, modeling, auto-dispatch    | ⚠️ Manual setup, stubs only                                   |
| Auto-Discovery       | ✅ Self-organizing service mesh        | ✅ Topics (DDS), ⚠️ Control-plane (gRPC), ❌ Manual (ZeroMQ) |
| Fault-Tolerance      | ✅ Watchdog, auto-restart, resilience  | ✅ QoS (DDS), ⚠️ Retries (gRPC), ❌ Manual (ZeroMQ)          |
| Request-Reply (RPC)  | ✅ Native ORPC (Object RPC in service) | ✅ RPC (gRPC), ⚠️ Over topics (DDS), ⚠️ Patterns (ZeroMQ)    |
| Pub/Sub Messaging    | ✅ Native, built-in in service         | ✅ Topics (DDS), ⚠️ Add-ons (gRPC), ⚠️ Manual (ZeroMQ)       |
| Built-in Logging     | ✅ Integrated, dynamic + viewer tool   | ⚠️ Vendor-specific (DDS), ❌ External                        |
| Dev Time Saved       | ✅ Significant, via automation         | ⚠️ Limited, external tooling                                  |

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Features[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#features)

**Core Model**

* **Unified Service Model** — define threads and components in models; load/unload dynamically.
* **Virtual Clients & Servers** — reusable micro-servers/clients, mapped across threads/processes.
* **Async RPC & Events** — non-blocking RPC/events executed in the target thread.

**Automations**

* **Auto-Discovery** — consumers find providers across threads, processes, and machines automatically.
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

* [ ] Is manually managing **threads, IPC, or service setup** taking too much time?  
* [ ] Would **automatic dispatching** of requests and events speed up your workflow?  
* [ ] Is **debugging distributed services** across threads and processes painful?  
* [ ] Do you want services to **auto-discover and connect**, reducing configuration and setup effort?  
* [ ] Would **integrated logging** save you time when troubleshooting?  
* [ ] Would **automatically generating boilerplate code** save you effort?  
* [ ] Are you tired of **costly manual integration and debugging**, and want a unified framework to focus on your core product?  

💡 **Tip:** If you check **3 or more** boxes, Areg SDK is likely a strong fit for your project.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Getting Started [![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#getting-started)

### Prerequisites

* **C++17-compatible compiler**: GCC, Clang/LLVM, MSVC (Windows) or MinGW
* **CMake ≥ 3.20**
* **Java ≥ 17** (for code generation tools)

> **Supported OS:** Linux, Windows
> **Supported Hardware:** x86, AMD64, ARM32, AArch64

For **compiler options, environment setup, and troubleshooting**, see the [CMake Configuration Guide](./docs/wiki/02a-cmake-config.md) and [README Wiki](./docs/wiki/README.md).

### Quick Start

1. **Clone and build the SDK**:

```bash
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
cmake -B build
cmake --build build -j 12
```

2. **Run the pre-built minimal RPC example**:

```bash
./product/build/<compiler>/<os>-<hw>-<build-type>-<lib-type>/bin/01_minimalrpc
```

> [This example](examples/01_minimalrpc/) demonstrates a simple RPC between 2 components running in 2 threads:
>
> * **Provider** prints a message
> * **Consumer** triggers it automatically
> * The app exits once the message is printed
>   [Full source code here](examples/01_minimalrpc/src/main.cpp)

**Message Flow Overview:**

```text
main() → load model → system auto-connects components → Consumer calls request → Provider handles request → prints & signals quit → main() releases & exits
```

> This shows how minimal services communicate in Areg without boilerplate.

### Learning Path

Start small and progress gradually:

1. **01\_minimalrpc** – basic RPC between 2 components
2. **02\_minimalipc** – IPC communication across processes
3. **03\_helloservice** – full multithreaded RPC + IPC example

👉 Explore **more** [examples](./examples/) for advanced features.

### Integration

* **CMake FetchContent:** [Integration Guide](./docs/wiki/02c-cmake-integrate.md)
* **Complete demo project:** [areg-sdk-demo](https://github.com/aregtech/areg-sdk-demo)
* **Optional GUI / Qt tools:** [areg-sdk-tools](https://github.com/aregtech/areg-sdk-tools)

> For advanced builds (IDE setup, cross-compilation, disabling tests/examples), see the [Areg SDK Wiki](./docs/wiki/).


**Next Steps:** Learn **Service Development** (Providers + Consumers) and optional tools (`mcrouter`, `logcollector`) → see [Documentation](#documentation).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Modules[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#modules)

The modules listed below are the core components of Areg SDK; additional optional utilities like `aregextend` and `areglogger` are available for advanced use or internal integration.

| Module                                                                  | Purpose                                 | When You Need It                  | Key Notes                                                       |
| ----------------------------------------------------------------------- | --------------------------------------- | --------------------------------- | --------------------------------------------------------------- |
| **[Areg Framework (`areg`)](./docs/wiki/01-introduction.md)**           | Core IPC + multithreading library	    | Always required	                | Includes ORPC, auto-dispatch, thread-safe component execution   |
| **[Multitarget Router (`mcrouter`)](./docs/wiki/05a-mcrouter.md)**      | IPC & network communication	            | Inter-process or network apps     | Enables self-organizing service mesh                            |
| **[Log Collector (`logcollector`)](./docs/wiki/04d-logcollector.md)**   | Centralized log collection	            | Remote logging required	        | Collects logs from multiple apps/services                       |
| **[Log Observer (`logobserver`)](./docs/wiki/04c-logobserver.md)**      | Control and save remote logs via CLI	| Save and configure logs           | Real-time view and filtering of logs                            |
| **[Code Generator (`codegen.jar`)](./docs/wiki/03a-code-generator.md)** | Automates code generation from SIML	    | During development and build      | Generates threads, components, and service glue automatically   |
| **[Lusan (UI Tool)](https://github.com/aregtech/areg-sdk-tools)**       | GUI for design, logging, and metrics	| For visual design and diagnostics	| Design threads, services, and monitor runtime metrics           |
| **[Examples](./examples/README.md)**                                    | Illustrates framework usage	            | Learning and quick starts	        | Demonstrates ORPC, Pub/Sub, auto-discovery, logging             |

---

## Architecture

**Areg** is built around an **interface-centric Object RPC (ORPC)** model. Applications expose **Service Providers** (servers) and interact with **Service Consumers** (clients) via automatically generated **Stubs & Proxies**, which handle requests, responses, and data updates through the **Multitarget Router**. Services can be invoked **without knowing network locations**, enabling seamless distributed communication.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/interface-centric.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/interface-centric.png" alt="Interface-centric communication diagram" style="width:50%;height:50%"/></a></div>

This **interface-driven design** supports both **Client-Server (Request-Reply)** and **Publish-Subscribe (PubSub)** patterns, enabling **action- and data-centric messaging**. It is designed for **multithreading**, **multiprocessing**, and **with the focus on internet-scale deployment in the future**, keeping messaging reliable and consistent across applications.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Use Cases

### IoT: Mist-to-Cloud

**Problem:** Traditionally, edge devices stream data to servers, causing latency, privacy risks, and heavy network usage. In addition, wireless communication consumes more power than CPU, which is critical for small devices.
**Solution:** Areg allows **Public Services to run directly on devices** in a **mist network**, where devices act as **micro-servers and micro-data centers**, capable of storing, combining, and processing data locally—**highly requested for AI-driven applications**.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/mist-network.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/mist-network.png" alt="IoT Mist-to-Cloud network diagram" style="width:70%;height:70%"/></a></div>

Benefits:

* **Enhanced data privacy** – data is processed at the source.
* **Reduced network traffic** – minimal streaming required.
* **Autonomous, intelligent devices** – capable of local decision-making and data fusion.

---

### More than Embedded

**Problem:** Small devices often lack scalable service infrastructure, while larger systems need multithreading/multiprocessing support.
**Solution:** Areg scales **beyond embedded devices** to PCs, servers, and clusters running GPOS (e.g., embedded Linux), turning devices into **service-enabled nodes**.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-services.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-services.png" alt="Service types and message handling diagram" style="width:70%;height:70%"/></a></div>

* **Local Services** – internal to a device (multithreading).
* **Public Services** – accessible across processes or devices (multiprocessing).
* **Internet Services (not existing yet)** – **currently not supported**; for cloud connection, external solutions should be used.

Key benefits:

* **Resilient** – failure in one service does not disrupt others.
* **Automatic Discovery** – services dynamically located by clients.
* **Thread-Safe Execution** – each service runs in its own thread context.

---

### Distributed Solutions

**Problem:** Developing multi-device or multi-process systems is complex, often requiring **custom communication layers or centralized servers**.
**Solution:** Areg enables **decentralized distributed communication**, where processes and devices interact as if part of a single application, forming a **mesh of services** that supports **action- and data-centric messaging**.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-sdk-features.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-sdk-features.png" alt="Areg SDK features diagram" style="width:70%;height:70%"/></a></div>

---

### Driverless Devices

**Problem:** Writing device drivers is **complex, slow, unsafe, OS-specific, and hard to maintain**.
**Solution:** Areg allows **driverless service-enabled devices**, exposing hardware as services that are portable, safe, and fast to develop.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/driverless-solution.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/driverless-solution.png" alt="Areg driverless solution" style="width:70%;height:70%"/></a></div>

---

### Simulation & Testing

**Problem:** Testing devices or distributed applications is hard when hardware is unavailable or costly.
**Solution:** Areg enables **simulation of data and services**, providing a realistic software environment for testing and rapid prototyping.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/software-layers.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/software-layers.png" alt="Software simulation layers" style="width:70%;height:70%"/></a></div>

* **Portable & accessible** – tests run without full hardware.
* **Safe prototyping** – reduces risk and speeds up development.
* **API-driven automation** – facilitates end-to-end test automation.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Roadmap[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#roadmap)

The Areg SDK is continuously evolving to help developers build complex applications efficiently on **Desktop**, **Embedded**, and **IoT edge** devices. It aims to reduce development time by **30-50%** while maintaining automation, reliability, and flexibility.

**Upcoming Features:**
- **Multi-channel & multi-protocol communication** – broader support for diverse communication methods, improving interoperability.
- **Internet services** – expansion into web-based, cloud-ready services for modern applications.

**[Lusan GUI tool](https://github.com/aregtech/areg-sdk-tools/) in development** (already usable):
- **Service API Designer** – visually create and edit service interfaces.
- **Interactive Log Viewer** – analyze logs and performance in real time.
- **Service Testing & Simulation Tool** – simulate services and data to streamline testing and development.

All tools are actively developed in the **[Areg SDK Tools repository](https://github.com/aregtech/areg-sdk-tools)**.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## License[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#license)

The Areg SDK is released under the **[Apache License Version 2.0](./LICENSE.txt)**, a permissive open-source license.  

For organizations or developers requiring commercial licensing, options are available, offering:  
- Dedicated commercial support  
- Full rights to create and distribute software without open-source obligations  

For details on commercial licensing, support, training, or partnership opportunities, visit **[Aregtech](https://www.aregtech.com/)** or contact **info[at]aregtech[dot]com**.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Call to Action[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#call-to-action)

Help grow the Areg SDK community and improve the framework for developers working on embedded systems, multithreaded/multiprocess apps, real-time data, and IoT projects.  

- Explore [open issues](https://github.com/aregtech/areg-sdk/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) to contribute where help is needed.  
- Share ideas or request new features via [issues](https://github.com/aregtech/areg-sdk/issues) or [discussions](https://github.com/aregtech/areg-sdk/discussions).  
- Submit pull requests following the project’s coding style.  
- ⭐ Consider starring Areg SDK on GitHub to support the project and help grow the community.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

**Follow us** at<br />
![X (formerly Twitter) Follow](https://img.shields.io/twitter/follow/aregtech) 
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Aregtech-blue?style=flat&logo=linkedin&logoColor=b0c0c0&labelColor=363D44)](https://www.linkedin.com/company/aregtech)
[![Join the chat at https://gitter.im/areg-sdk/community](https://badges.gitter.im/areg-sdk/community.svg)](https://gitter.im/areg-sdk/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

<!-- markdownlint-enable -->
