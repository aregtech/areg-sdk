<h1 align="center" style="font-size:2.5em; font-weight:bold; margin:1em 0;">
  <a href="https://www.aregtech.com">
    <img 
      src="./docs/img/areg-sdk-1280x360px-logo.png" 
      alt="Areg SDK – C++ Middleware and Framework for RPC and IPC"
      title="Areg SDK – Lightweight Cross-Platform C++ Framework and Middleware for RPC, IPC, and Distributed Systems"
      style="width:100%;height:auto" 
    />
  </a>
</h1>

[![Latest release](https://img.shields.io/github/v/release/aregtech/areg-sdk?label=Latest%20release&style=social)](https://github.com/aregtech/areg-sdk/releases/tag/v1.5.0)
[![GitHub commits](https://img.shields.io/github/commits-since/aregtech/areg-sdk/v1.5.0.svg?style=social)](https://github.com/aregtech/areg-sdk/compare/v1.5.0...master)
[![Stars](https://img.shields.io/github/stars/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/stargazers)
[![Fork](https://img.shields.io/github/forks/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/network/members)
[![Watchers](https://img.shields.io/github/watchers/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/watchers)
[![Wiki Pages](https://img.shields.io/badge/Areg%20Wiki%20Pages-8-brightgreen?style=social&logo=wikipedia)](https://github.com/aregtech/areg-sdk/wiki/)

⭐ If you find Areg SDK useful, please give us a star — it helps the community grow!  

---

Most C++ projects don’t fail on algorithms — they fail on **threads, IPC, and fragile wiring**. Unlike traditional frameworks, **Areg SDK automates communication**, unifying async RPC, Pub/Sub, and service discovery. Its self-managed service mesh **enables scalable, fault-tolerant systems across threads, processes, and devices — with no boilerplate, no fragile wiring**.

*Areg SDK combines a lightweight C++17 framework, multitarget router, and developer tools for distributed apps.*

---

## Project Status[![](./docs/img/pin.svg)](#project-status)

<table class="no-border">
  <tr>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml/badge.svg" alt="CMake build"/></a></td>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/msbuild.yml"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/msbuild.yml/badge.svg" alt="MS Build"/></a></td>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml/badge.svg" alt="CodeQL"/></a></td>
  </tr>
  <tr>
    <td><img src="https://img.shields.io/badge/Solution-C++17-blue.svg?style=flat&logo=c%2B%2B&logoColor=b0c0c0&labelColor=363D44" alt="C++ solution"/></td>
    <td><img src="https://img.shields.io/badge/OS-linux%20%7C%20windows-blue?style=flat&logo=Linux&logoColor=b0c0c0&labelColor=363D44" alt="Operating systems"/></td>
    <td><img src="https://img.shields.io/badge/CPU-x86%20%7C%20x86__64%20%7C%20arm%20%7C%20aarch64-blue?style=flat&logo=amd&logoColor=b0c0c0&labelColor=363D44" alt="CPU Architectures"/></td>
  </tr>
</table>

---

## Table of Contents[![](./docs/img/pin.svg)](#table-of-contents)

- [Why Areg SDK: Benefits and Comparison](#why-areg-sdk-benefits-and-comparison)
- [Getting Started: Build and Integration](#getting-started-build-and-integration)
- [Core Modules and Architecture](#core-modules-and-architecture)
- [Use Cases: Distributed, RPC and IPC](#use-cases-distributed-rpc-and-ipc)
- [Roadmap](#roadmap)
- [Documentation](#documentation)
- [License](#license)
- [Call to Action](#call-to-action)

> [!IMPORTANT]
> Full technical guidance for building, using, and integrating Areg SDK is available in the [documentation](./docs/wiki/ "Areg SDK documentation").

---

## Why Areg SDK: Benefits and Comparison[![](./docs/img/pin.svg)](#why-areg-sdk-benefits-and-comparison "Why Areg SDK Lightweight C++ Framework")

**Areg SDK** is a **Software Development Kit** built around the **Areg Framework** — a **runtime with built-in middleware that automates threading, service discovery, and inter-node messaging**, so teams can build distributed systems **without boilerplate or fragile wiring**. Unlike gRPC, DDS, or ZeroMQ, it provides a self-managed mesh and a unified API across threads, processes, and devices.  
**Result:** scalable, fault-tolerant systems that are faster, simpler, and safer.

### Top 5 Benefits

1. **No manual threading**  
   Threads, components, and watchdogs are auto-created — preventing subtle concurrency bugs and saving time.

2. **Unified API**  
   Services are called the same way locally or remotely — always async, non-blocking, and consistent.

3. **Self-managed mesh**  
   Automatic service discovery and routing across threads, processes, and devices — no manual wiring.

4. **Resilient by design**  
   Components can join or leave dynamically; watchdogs restart failed threads for fault-tolerant systems.

5. **Built-in observability**  
   Integrated logging and scopes help per-method duration measurement to monitor and optimize efficiently.

💡 **When to use:** Linux & Windows, embedded and desktop C++ apps, scaling from prototype to multi-node.  
⚠️ **When not to use:** RTOS (at the moment), web, or non-C++ apps — use web/RTOS-specific tools instead.

### Compared to Alternatives

| Feature          | Areg SDK                        | Competitors (gRPC, DDS, ZeroMQ)                      |
| ---------------- | ------------------------------- | ---------------------------------------------------- |
| Ease of Use      | ✅ Automated setup              | ⚠️ Manual, boilerplate, [complex](https://www.innoq.com/en/blog/2024/06/grpc/#whataresomechallengesofworkingwithgrpc) |
| Automation       | ✅ Codegen, modeling, dispatch  | ⚠️ Manual setup, [stubs only](https://grpc.io/docs/what-is-grpc/introduction/#overview) |
| Auto-Discovery   | ✅ Self-organizing service mesh | ✅ DDS: [built-in](https://opendds.readthedocs.io/en/latest-release/devguide/introduction_to_dds.html#discovery-matching-and-association), ⚠️ gRPC/ZeroMQ: [external](https://stackoverflow.com/questions/59398556/grpc-equivalent-of-wcf-service-discovery) |
| Fault-Tolerance  | ✅ Watchdog & auto-restart      | ✅ DDS: [QoS](https://opendds.readthedocs.io/en/latest-release/devguide/quality_of_service.html), ⚠️ gRPC/ZeroMQ: [Retries](https://grpc.io/docs/guides/retry/)/Manual |
| Request-Reply    | ✅ Native Object RPC in service | ✅ gRPC: [RPC](https://grpc.io/docs/what-is-grpc/core-concepts/#overview), ⚠️ DDS/ZeroMQ: Over topics/[patterns](https://zguide.zeromq.org/docs/chapter3/) |
| Pub/Sub          | ✅ Native, built-in in service  | ✅ DDS: [Topics](https://opendds.readthedocs.io/en/latest-release/devguide/built_in_topics.html), ⚠️ Add-ons/Manual |
| Built-in Logging | ✅ Integrated, dynamic + viewer | ⚠️ [Vendor-specific](https://community.rti.com/static/documentation/connext-dds/current/doc/manuals/addon_products/observability/telemetry_data/logs.html) (DDS) or External |
| Dev Time Saved   | ✅ Faster, via automation       | ⚠️ Limited, external tooling                        |

### Decision Checklist

- [ ] Do **threading** and **synchronization** issues slow your progress?
- [ ] Does **debugging** across **threads, processes, or components** take excessive time?
- [ ] Is setting up **communication** across **processes or nodes** complex and error-prone?
- [ ] Do **remote failures** and **reconnections** create delays and extra work?
- [ ] Would a **unified communication model** across **threads, processes, and devices** simplify development?

💡 If you answer **Yes** to 3+ questions, Areg SDK is likely a good fit.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Getting Started: Build and Integration [![](./docs/img/pin.svg)](#getting-started-build-and-integration)

### Prerequisites

- **C++17 compiler**: GCC, Clang/LLVM, MSVC, or MinGW
- **CMake ≥ 3.20**
- **Java ≥ 17** (required for code generation tools)

> **Supported OS:** Linux, Windows  
> **Supported Hardware:** x86, x86_64, ARM, AArch64

See the [CMake Configuration Guide](./docs/wiki/02a-cmake-config.md) for detailed setup, compiler options, and troubleshooting tips.

### Build Areg SDK

Clone the repository and build the SDK in one go:

```bash
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
cmake -B build
cmake --build build -j 12
```

> 💡 Tip: Copy-paste these commands directly in **Linux Terminal, Windows CMD, or PowerShell**.

### Run Minimal RPC Example

Run the pre-built `01_minimalrpc` minimal RPC demo to verify the build located in sub-directory:

```bash
./product/build/<compiler>/<os>-<hw>-<build-type>-<lib-type>/bin/
```

This 📄 [example source](./examples/01_minimalrpc/src/main.cpp) demonstrates **Multithreaded RPC** in a single process:

* **Service Consumer** runs in one thread
* **Service Provider** runs in another thread
* Consumer calls the method of remote Provider asynchronously

**Message Flow:**

```text
🟢 main() → 🏗 load model → 🔗 auto-connect → 📤 Consumer request → 🖨 Provider prints → ✅ application exits
```

### Learning Path

1. [**01\_minimalrpc**](examples/01_minimalrpc/) – Minimal RPC between two components
2. [**02\_minimalipc**](examples/02_minimalipc/) – IPC across processes, **requires `mtrouter`**
3. [**03\_helloservice**](examples/03_helloservice/) – Multithreaded RPC + IPC example
4. [**More Examples**](examples/README.md) – Advanced SDK features and usage patterns

> ⚠️ For IPC, ensure **`mtrouter` is running** → see [mtrouter docs](./docs/wiki/05a-mtrouter.md)

### Integration & Extensions

* **CMake FetchContent integration:** [Guide](./docs/wiki/02c-cmake-integrate.md)
* **Demo Project:** [areg-sdk-demo](https://github.com/aregtech/areg-sdk-demo)
* **Qt Integration & Tools:** [areg-sdk-tools](https://github.com/aregtech/areg-sdk-tools)

> 💡 Advanced builds (IDE setup, cross-compilation, disabling tests/examples) → consult [Wiki](./docs/wiki/)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Core Modules and Architecture[![](./docs/img/pin.svg)](#core-modules-and-architecture)

**Areg SDK** combines the Areg Framework, Multitarget Router, logging utilities, and development tools.

### Modules Overview

| Module                                                              | Role & Purpose                                                                                              |
| ------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------- |
| [Areg Library (`areg`)](./docs/HelloService.md)                     | **Core framework + middleware.** Automates Object RPC (ORPC), IPC, threading, routing, and fault recovery.  |
| [Code Generator (`codegen.jar`)](./docs/wiki/03a-code-generator.md) | **Build-time tool.** Generates code from service APIs to eliminate manual coding and automate RPC.          |
| [Multitarget Router (`mtrouter`)](./docs/wiki/05a-mtrouter.md)      | **Required for IPC.** Routes messages across processes and devices to form a service mesh.                  |
| [Log Collector (`logcollector`)](./docs/wiki/04d-logcollector.md)   | **Optional developer tool.** Aggregates logs from distributed apps for monitoring, debugging, and analysis. |
| [Lusan (GUI Tool)](https://github.com/aregtech/areg-sdk-tools)      | **Optional developer tool.** Provides visual API design, log inspection, and performance visualization.     |
| [Examples](./examples/README.md)                                    | **Learning & validation.** Sample projects that demonstrate Areg in action.                                 |

### Architecture

Areg uses an **interface-centric Object RPC (ORPC)** model. Applications expose **Service Providers** and interact via **Service Consumers** using generated code and the **Multitarget Router** for inter-process communication. Services, as logical micro-servers independent of physical locations, enable a **service-oriented architecture (SOA)**.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/interface-centric.png"><img src="./docs/img/interface-centric.png" alt="Interface-centric communication diagram" style="width:50%;height:50%"/></a></div>

Areg supports both **Client–Server (Request–Reply)** and **Publish–Subscribe (Pub/Sub)** patterns, optimized for **multithreading**, **multiprocessing**, and **distributed systems** with low-latency requirements.

### ✅ Quick Summary

* **`areg` + `codegen.jar`** — build services and projects
* **`mtrouter`** — IPC and distributed communication
* **Other tools** — logging and development utilities
* **Key strengths** — automates service communication, manages threading & IPC, enables distributed apps

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Use Cases: Distributed, RPC and IPC[![](./docs/img/pin.svg)](#use-cases-distributed-rpc-and-ipc)

### IoT: Mist-to-Cloud

Edge devices often stream raw data to central servers, which can increase latency, network load, and privacy risks. With **Areg**, services run on devices, forming a **mist network of micro-servers** that process and aggregate data locally.


<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/mist-network.png"><img src="./docs/img/mist-network.png" alt="IoT Mist-to-Cloud network diagram" style="width:70%;height:70%"/></a></div>

**Benefit:** Low-latency, autonomous edge mesh; sensitive data remains on-device; reduced cloud traffic.

---

### Beyond Embedded

Many small devices lack scalable infrastructure. **Areg** transforms embedded applications into **distributed modules** that scale across remote nodes and platforms, orchestrating **multithreaded and multiprocess C++ services**.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-services.png"><img src="./docs/img/areg-services.png" alt="Service types and message handling diagram" style="width:70%;height:70%"/></a></div>

* **Local Services** – multithreaded within a device
* **Public Services** – accessible across processes and applications

**Benefit:** Seamlessly expose services to other apps and devices without modifying code.

---

### Driverless Devices

Device drivers are often **slow, complex, and platform-specific**. **Areg** lets you **expose hardware as portable, service-enabled components**, making devices platform-independent.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/driverless-solution.png"><img src="./docs/img/driverless-solution.png" alt="Areg driverless service-enabled diagram" style="width:70%;height:70%"/></a></div>

**Benefit:** Accelerates prototyping, testing, and development; enables early bug detection; devices are network-accessible.

---

### Embedded and Distributed AI

Integrating AI into embedded and edge systems often requires managing concurrency, communication, and synchronization across multiple modules — sensors, preprocessors, inference engines, and actuators. **Areg SDK** simplifies this by letting developers implement each AI stage as an **independent service** running in its own thread or process. 

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-for-embedded-ai.png"><img src="https://github.com/aregtech/areg-sdk/blob/feature/607-update-readme/docs/img/areg-for-embedded-ai.png" alt="Embedded AI modular service architecture diagram" style="width:70%;height:70%"/></a></div>

**Benefit:** Modular AI pipelines without manual thread or message management; real-time responsiveness for control tasks; scalable orchestration of distributed AI workloads across devices.

---

### Simulation & Testing

Validating distributed systems is expensive and hardware-dependent. **Areg** allows simulation of **Data Layer services** in external applications, providing realistic environments to test behavior, performance, and fault-tolerance. Services appear **location-transparent** to higher layers.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/software-layers.png"><img src="./docs/img/software-layers.png" alt="Software 4 Layer Architecture: Simulate Data Layer" style="width:70%;height:70%"/></a></div>

**Benefit:** Transparent service location enables testing of higher-layer logic without real hardware dependencies.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Roadmap[![](./docs/img/pin.svg)](#roadmap)

Areg SDK continues to evolve for **Desktop and Embedded**, focusing on automation, reliability, and performance.

**Planned work:**

- Multi-channel support for multiplexed communications
- RTOS support (**Zephyr OS**) to bring Areg into real-time environments
- Lusan app improvements: performance, stability, and usability ([Areg SDK Tools](https://github.com/aregtech/areg-sdk-tools))

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Documentation[![](./docs/img/pin.svg)](#documentation)

* [Installation and Build](./docs/wiki/README.md#1-installation-and-build) — cross-platform builds, toolchains, CMake integration for embedded and desktop targets
* [Build Options and Integrations](./docs/wiki/README.md#2-build-options-and-integrations) — FetchContent, packaging, and embedding Areg as a library
* [Development Tools](./docs/wiki/README.md#3-development-tools) — code generation for service-oriented development
* [Logging and Monitoring](./docs/wiki/README.md#4-logging-and-monitoring) — logcollector and logobserver usage
* [Networking and Communication](./docs/wiki/README.md#5-networking-and-communication) — Multitarget router for IPC and low-latency messaging
* [Persistence](./docs/wiki/README.md#6-persistence) — for local data storage in a text file
* [Troubleshooting](./docs/wiki/README.md#7-troubleshooting) — common issues and recommended solutions
* [Examples and Tests](./docs/wiki/README.md#8-examples-and-tests) — catalog of sample projects (RPC, IPC, Pub/Sub, FSM, and more)
* [HOWTO Guide](docs/HOWTO.md) — practical reference.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## License[![](./docs/img/pin.svg)](#license)

Areg SDK is released under the **[Apache License 2.0](./LICENSE.txt)** — permissive and suitable for open-source and commercial use.

**Commercial licensing & services:** Dedicated support, training, and tailored licensing options are available for enterprise customers. Visit **[Aregtech](https://www.aregtech.com/)** or email **info\[at]aregtech\[dot]com** for details.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Call to Action [![](./docs/img/pin.svg)](#call-to-action)

🚀 Join the **Areg SDK community** and help shape the future of distributed C++ apps:

* 🛠️ [Contribute to open issues](https://github.com/aregtech/areg-sdk/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22)
* 💡 Share ideas or request features via [issues](https://github.com/aregtech/areg-sdk/issues) or [discussions](https://github.com/aregtech/areg-sdk/discussions)
* 🔀 Submit pull requests (see [contribution guidelines](CONTRIBUTING.md))
* ⭐ **Liked Areg SDK? Give it a star — it helps others discover the project and supports its growth**

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>  

---

**Follow us:**  
[![X Follow](https://img.shields.io/twitter/follow/aregtech)](https://x.com/aregtech?lang=en)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Aregtech-blue?style=flat&logo=linkedin\&logoColor=b0c0c0\&labelColor=363D44)](https://www.linkedin.com/company/aregtech)
[![Gitter](https://badges.gitter.im/areg-sdk/community.svg)](https://gitter.im/areg-sdk/community)
