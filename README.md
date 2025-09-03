<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.aregtech.com"><img align="center" src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-sdk-1280x360px-logo.png" alt="AREG SDK Home" style="width:100%;height:100%"/></a>
  <br /><br /><strong>Areg SDK</strong>
</h1>

*Concurrency without the complexity.*

[![Latest release](https://img.shields.io/github/v/release/aregtech/areg-sdk?label=Latest%20release&style=social)](https://github.com/aregtech/areg-sdk/releases/tag/v1.5.0)
[![GitHub commits](https://img.shields.io/github/commits-since/aregtech/areg-sdk/v1.5.0.svg?style=social)](https://GitHub.com/aregtech/areg-sdk/commit/)
[![Stars](https://img.shields.io/github/stars/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/stargazers)
[![Fork](https://img.shields.io/github/forks/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/network/members)
[![Watchers](https://img.shields.io/github/watchers/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/watchers)
[![Wiki Pages](https://img.shields.io/badge/AREG%20Wiki%20Pages-8-brightgreen?style=social&logo=wikipedia)](https://github.com/aregtech/areg-sdk/wiki/)

---

## Introduction[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#introduction)

*Areg is like a phone switchboard operator, but for programs — a lightweight C++ framework that automates software communication.*

Threads, IPC, and distributed messaging often slow development, creating fragile glue code and hidden bugs. By combining async RPC, auto-discovery, and event-driven messaging, AREG SDK makes it faster and easier to build and debug distributed services across threads, processes, and devices—enabling developers to focus on features, not plumbing.

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
    <td><img src="https://img.shields.io/badge/OS-linux%20%7C%20windows-blue??style=flat&logo=Linux&logoColor=b0c0c0&labelColor=363D44" alt="Operating systems"/></td>
    <td colspan="2"><img src="https://img.shields.io/badge/CPU-x86%20%7C%20x86__64%20%7C%20arm%20%7C%20aarch64-blue?style=flat&logo=amd&logoColor=b0c0c0&labelColor=363D44" alt="CPU Architect"/></td>
  </tr>
</table>

---

## Table of contents[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#table-of-contents)
- [Why Areg SDK?](#why-areg-sdk)
  - [Key Solutions / Top 5 Pain Killers](#key-solutions--top-5-pain-killers)
  - [Compared to Alternatives](#compared-to-alternatives)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Quick Start](#quick-start)
  - [Recommended Learning Path](#recommended-learning-path)
  - [Integration](#integration)
- [Modules](#modules)
- [Architecture](#architecture)
- [Use Cases](#use-cases)
  - [IoT: Mist-to-Cloud](#iot--mist-to-cloud)
  - [More than Embedded](#more-than-embedded)
  - [Distributed Solutions](#distributed-solutions)
  - [Driverless Devices](#driverless-devices)
  - [Simulation & Testing](#Simulation-Testing)
- [Pipeline and Roadmap](#pipeline-and-roadmap)
- [License](#license)
- [Call to action](#call-to-action)
- [Thank you all!](#thank-you-all)

> [!IMPORTANT]
> For full technical guidance of building and using AREG SDK, see the [following documents](./docs/wiki/).

---

## Why Areg SDK?[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#why-areg-sdk)

Software complexity rarely comes from algorithms—it comes from **frameworks that don’t scale**. Threads, processes, and distributed components quickly lead to **manual wiring, hidden bugs, and costly rewrites**. Late framework decisions can lock teams into fragile architectures, wasting weeks of development.

**Areg SDK removes these risks from day one.** Its unified **Pub/Sub + Request-Reply model** handles threading, IPC, and distributed execution across threads, processes, and devices—letting developers focus on **services and features, not plumbing**.

### Key Solutions / Top 5 Pain Killers

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

👉 Tested on **Linux and Windows**, scaling from single-device prototypes to multi-node production systems.

### Compared to Alternatives

| Feature              | AREG SDK                          | Competitors (gRPC, DDS, ZeroMQ)          |
| -------------------- | --------------------------------- | ---------------------------------------- |
| Ease of Use          | ✅ GUI, automated setup           | ⚠️ Manual, boilerplate, complex          |
| Automation           | ✅ Codegen, modeling, auto-dispatch | ⚠️ Stubs only, manual wiring/dispatch    |
| Auto-Discovery       | ✅ Mesh of services               | ✅ Topics (DDS), ⚠️ Control-plane (gRPC), ❌ Manual (ZeroMQ) |
| Fault-Tolerance      | ✅ Watchdog + resilience          | ✅ QoS (DDS), ⚠️ Retries (gRPC), ❌ Manual |
| Request-Reply (RPC)  | ✅ Native ORPC                    | ✅ RPC (gRPC), ⚠️ Over topics (DDS), ⚠️ Patterns (ZeroMQ) |
| Pub/Sub Messaging    | ✅ Built-in, event-driven         | ✅ DDS, ⚠️ Add-ons (gRPC), ⚠️ Manual (ZeroMQ) |
| Built-in Logging     | ✅ Integrated + viewer            | ⚠️ Vendor-specific (DDS), ❌ External     |
| Dev Time Saved       | ✅ High via automation            | ⚠️ Lower, external tooling needed        |

👉 Areg SDK helps developers build scalable, reliable, and maintainable C++ systems without drowning in IPC, threading, or distributed complexity.

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

### Recommended Learning Path

Start small and progress gradually:

1. **01\_minimalrpc** – basic RPC between 2 components
2. **02\_minimalipc** – IPC communication across processes
3. **03\_helloservice** – full multithreaded RPC + IPC example

> Explore more [examples](./examples/) for advanced features.

### Integration

* **CMake FetchContent:** [Integration Guide](./docs/wiki/02c-cmake-integrate.md)
* **Complete demo project:** [areg-sdk-demo](https://github.com/aregtech/areg-sdk-demo)
* **Optional GUI / Qt tools:** [areg-sdk-tools](https://github.com/aregtech/areg-sdk-tools)

> For advanced builds (IDE setup, cross-compilation, disabling tests/examples), see the [Areg SDK Wiki](./docs/wiki/).


**Next Steps:** Learn **Service Development** (Providers + Consumers) and optional tools (`mcrouter`, `logcollector`) → see [Documentation](#documentation).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Modules[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#modules)

| Module                                                                                         | Purpose                                                             | When You Need It                                                         | Key Notes                                                                                                                           |
| ---------------------------------------------------------------------------------------------- | ------------------------------------------------------------------- | ------------------------------------------------------------------------ | ----------------------------------------------------------------------------------------------------------------------------------- |
| **[Areg Framework (`areg`)](./docs/wiki/01-introduction.md)**                                  | Core library for IPC and multithreading services                    | Always required for any app using Areg                                   | [View source](./framework/areg/) – linked to the code; linked into every service-based application                                  |
| **[Multitarget Router (`mcrouter`)](./docs/wiki/05a-mcrouter.md)**                             | Routes messages across processes/machines (IPC)                     | Required for inter-process or network communication                      | [View source](./framework/mcrouter/) – console or system service; fast and lightweight; configurable via `areg.init`                |
| **[Log Collector (`logcollector`)](./docs/wiki/04d-logcollector.md)**                          | Collects and forwards logs from apps                                | Only if centralized/remote logging is needed                             | [View source](./framework/logcollector/) – fast and lightweight; configurable ([logging config](./docs/wiki/04a-logging-config.md)) |
| **[Log Observer (`logobserver`)](./docs/wiki/04c-logobserver.md)**                             | Controls and saves logs                                             | Use to activate/deactivate scopes, adjust priorities, and save logs      | [View source](./framework/logobserver/) – CLI tool; does not display logs; supports configuration and persistence                   |
| **[Log Observer Library (`areglogger`)](./docs/wiki/04d-logcollector.md#logobserver-library)** | Connects to `logcollector`                                          | Needed for building custom log viewers or used by Log Observer and Lusan | [View source](./framework/areglogger/) – flexible; not only for internal use                                                        |
| **[Extended Library (`aregextend`)](./docs/wiki/05a-mcrouter.md#areg-extend)**                 | Extra utilities (console helpers, service wrappers, SQLite wrapper) | Optional — only if you need these utilities                              | [View source](./framework/aregextend/) – used by `mcrouter`, `logcollector`, `logobserver`, and `areglogger`                        |
| **[Code Generator (`codegen.jar`)](./docs/wiki/03a-code-generator.md)**                        | Generates provider/consumer code from SIML files                    | Always used during build when service interfaces are included            | [View source](./master/tools/) – integrated with CMake/MSVC; examples provided                                                      |
| **[Lusan (UI Tool)](https://github.com/aregtech/areg-sdk-tools)**                              | GUI for service design and log viewing                              | Use to create/edit SIML files and analyze logs visually                  | [View source](https://github.com/aregtech/areg-sdk-tools) – Qt-based; still evolving but usable                                     |
| **[Examples](./examples/README.md)**                                                           | Demonstrates Areg usage                                             | Always helpful for learning and quick starts                             | [View source](./examples/) – covers IPC, RPC, watchdogs, state machines, logging, and more                                          |

---

## Architecture

**AREG** is built around an **interface-centric Object RPC (ORPC)** model. Applications expose **Service Providers** (servers) and interact with **Service Consumers** (clients) via automatically generated **Stubs & Proxies**, which handle requests, responses, and data updates through the **Multitarget Router**. Services can be invoked **without knowing network locations**, enabling seamless distributed communication.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/interface-centric.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/blob/master/docs/img/interface-centric.png" alt="Interface-centric communication diagram" style="width:50%;height:50%"/></a></div>

This **interface-driven design** supports both **Client-Server (Request-Reply)** and **Publish-Subscribe (PubSub)** patterns, enabling **action- and data-centric messaging**. It is designed for **multithreading**, **multiprocessing**, and **with the focus on internet-scale deployment in the future**, keeping messaging reliable and consistent across applications.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Use Cases

### IoT: Mist-to-Cloud

**Problem:** Traditionally, edge devices stream data to servers, causing latency, privacy risks, and heavy network usage. In addition, wireless communication consumes more power than CPU, which is critical for small devices.
**Solution:** AREG allows **Public Services to run directly on devices** in a **mist network**, where devices act as **micro-servers and micro-data centers**, capable of storing, combining, and processing data locally—**highly requested for AI-driven applications**.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/mist-network.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/blob/master/docs/img/mist-network.png" alt="IoT Mist-to-Cloud network diagram" style="width:70%;height:70%"/></a></div>

Benefits:

* **Enhanced data privacy** – data is processed at the source.
* **Reduced network traffic** – minimal streaming required.
* **Autonomous, intelligent devices** – capable of local decision-making and data fusion.

---

### More than Embedded

**Problem:** Small devices often lack scalable service infrastructure, while larger systems need multithreading/multiprocessing support.
**Solution:** AREG scales **beyond embedded devices** to PCs, servers, and clusters running GPOS (e.g., embedded Linux), turning devices into **service-enabled nodes**.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-services.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/blob/master/docs/img/areg-services.png" alt="Service types and message handling diagram" style="width:70%;height:70%"/></a></div>

* **Local Services** – internal to a device (multithreading).
* **Public Services** – accessible across processes or devices (multiprocessing).
* **Internet Services (concept only)** – **currently not supported**; for cloud connection, external solutions should be used.

Key benefits:

* **Resilient** – failure in one service does not disrupt others.
* **Automatic Discovery** – services dynamically located by clients.
* **Thread-Safe Execution** – each service runs in its own thread context.

---

### Distributed Solutions

**Problem:** Developing multi-device or multi-process systems is complex, often requiring **custom communication layers or centralized servers**.
**Solution:** AREG enables **decentralized distributed communication**, where processes and devices interact as if part of a single application, forming a **mesh of services** that supports **action- and data-centric messaging**.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-sdk-features.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/blob/master/docs/img/areg-sdk-features.png" alt="AREG SDK features diagram" style="width:70%;height:70%"/></a></div>

---

### Driverless Devices

**Problem:** Writing device drivers is **complex, slow, unsafe, OS-specific, and hard to maintain**.
**Solution:** AREG allows **driverless service-enabled devices**, exposing hardware as services that are portable, safe, and fast to develop.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/driverless-solution.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/blob/master/docs/img/driverless-solution.png" alt="AREG driverless solution" style="width:70%;height:70%"/></a></div>

---

### Simulation & Testing

**Problem:** Testing devices or distributed applications is hard when hardware is unavailable or costly.
**Solution:** AREG enables **simulation of data and services**, providing a realistic software environment for testing and rapid prototyping.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/software-layers.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/blob/master/docs/img/software-layers.png" alt="Software simulation layers" style="width:70%;height:70%"/></a></div>

* **Portable & accessible** – tests run without full hardware.
* **Safe prototyping** – reduces risk and speeds up development.
* **API-driven automation** – facilitates end-to-end test automation.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>
---

## Pipeline and Roadmap[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#pipeline-and-roadmap)

The AREG SDK is continuously evolving to help developers create complex applications more efficiently on **Desktop**, **Embedded**, and **IoT edge** devices. It aims to reduce development time by **30-50%** while ensuring automation, reliability and flexibility.

**Upcoming Features:**
- **Multi-channel & multi-protocol communication**: Enhanced support for various communication methods, improving interoperability.
- **Internet category services**: Expansion into web-based services, enabling more modern, internet-driven applications.

**Tools in Development:**
- **Service Interface Designer**: A visual tool to simplify service interface creation.
- **Interactive Log Viewer**: A tool for easy log visualization and performance analysis.
- **Service Testing & Simulation Tool**: Helps simulate services and data, streamlining testing and development.

The tools are actively being developed in the **[AREG SDK Tools repository](https://github.com/aregtech/areg-sdk-tools)**.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## License[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#license)

The AREG SDK is available under the **[Apache License Version 2.0](./LICENSE.txt)**, a permissive free open-source license. For developers or businesses that need commercial licensing, this option is available and includes:
- Commercial support.
- Full rights to create and distribute software without open-source license obligations.

For more information on commercial licensing, commercial support, trainings, or partnership inquiries, visit the **[Aregtech](https://www.aregtech.com/)** website or contact us at **info[at]aregtech[dot]com**.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Call to action[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#call-to-action)

We encourage the developer community to get involved and contribute to the growth of the AREG SDK. Join AREG SDK community and collaborate with AREG developers. Here's how you can help:

- Check out our list of [open issues](https://github.com/aregtech/areg-sdk/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) where assistance is needed.
- If you have suggestions or need new features, open a [new issue](https://github.com/aregtech/areg-sdk/issues) or start a [discussion](https://github.com/aregtech/areg-sdk/discussions).
- For pull requests, ensure that the code adheres to our coding style and allow time for review and testing.

Lastly, help us grow the AREG SDK community by **giving a star** on GitHub! Whether you're working on **embedded applications**, **multiprocessing and multithreading applications**, **real-time data transfer**, **IoT applications**, or **microservices architecture**, your support helps us continue to improve this cutting-edge communication framework.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Thank you all!![![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#thank-you)

Special thanks to all contributors and supporters that starred this repository.

**Our amazing contributors**:

[![Contributors for @aregtech/areg-sdk](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/contributors.svg)](https://github.com/aregtech/areg-sdk/graphs/contributors)

**Our supportive stargazers**:

[![Stargazers of @aregtech/areg-sdk repo](http://reporoster.com/stars/aregtech/areg-sdk)](https://github.com/aregtech/areg-sdk/stargazers)

Do you like this project? Join us or [give](https://github.com/aregtech/areg-sdk/stargazers) a ⭐.<br/>
Do you have an idea or found a bug? Please open an [issue](https://github.com/aregtech/areg-sdk/issues) or start a [discussion](https://github.com/aregtech/areg-sdk/discussions).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

**Follow us** at<br />
![X (formerly Twitter) Follow](https://img.shields.io/twitter/follow/aregtech) 
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Aregtech-blue?style=flat&logo=linkedin&logoColor=b0c0c0&labelColor=363D44)](https://www.linkedin.com/company/aregtech)
[![Join the chat at https://gitter.im/areg-sdk/community](https://badges.gitter.im/areg-sdk/community.svg)](https://gitter.im/areg-sdk/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

---

**Share** the project link with your network on social media.

<a href="https://www.reddit.com/submit?url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk&title=Awesome%20communcation%20engine!" target="_blank"><img src="https://img.shields.io/twitter/url?label=Reddit&logo=Reddit&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on Reddit"/></a>&nbsp;
<a href="https://www.linkedin.com/shareArticle?mini=true&url=https%3A//github.com/aregtech/areg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=LinkedIn&logo=LinkedIn&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on LinkedIn"/></a>&nbsp;
<a href="https://x.com/intent/tweet?text=%23AREG%20%23realtime%20communication%20engine%20for%20%23embedded%20and%20%23IoT%0A%0Ahttps%3A//github.com/aregtech/areg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=Twitter&logo=X&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Shared on X"/></a>&nbsp;
<a href="https://www.facebook.com/sharer/sharer.php?u=https%3A//github.com/aregtech/areg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=Facebook&logo=Facebook&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on Facebook"/></a>&nbsp;
<a href="https://t.me/share/url?text=Awesome%20communication%20engine!&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=Telegram&logo=Telegram&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on Telegram"/></a>&nbsp;
<a href="https://wa.me/?text=Awesome%20communication%20engine!%5Cn%20https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=Whatsapp&logo=Whatsapp&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on Whatsapp"/></a>&nbsp;
<a href="mailto:?subject=Awesome%20communication%20engine&body=Checkout%20this%20awesome%20communication%20engine%3A%0Ahttps%3A//github.com/aregtech/areg-sdk%0A" target="_blank"><img src="https://img.shields.io/twitter/url?label=GMail&logo=GMail&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk"/></a>

<!-- markdownlint-enable -->
