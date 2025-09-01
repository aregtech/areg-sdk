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

Threads, IPC, and distributed messaging often slow development with fragile glue code, hidden bugs, and endless debugging. By handling threading, IPC, and distributed execution automatically, Areg SDK delivers lightweight, reliable auto-discovery, async RPC, and event-driven messaging.

This lets developers **design, connect, and debug services effortlessly** across threads, processes, and devices—so they can focus on features, not plumbing.

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
- [Motivation](#motivation)
- [Interface-centricity](#interface-centricity)
- [More than Embedded](#more-than-embedded)
- [Composition of AREG SDK](#composition-of-areg-sdk)
  - [Cloning Sources](#cloning-sources)
  - [Build Instructions](#build-instructions)
- [Integration and Development](#integration-and-development)
  - [AREG SDK Integration Methods](#areg-sdk-integration-methods)
  - [Service Creation and Development](#service-creation-and-development)
  - [Example: Application Model Setup](#example-application-model-setup)
  - [Multicast Router and Log Collector](#multicast-router-and-log-collector)
- [Examples](#examples)
- [Pipeline and Roadmap](#pipeline-and-roadmap)
- [Use Cases and Benefits](#use-cases-and-benefits)
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

* **Productivity & Developer Experience** — declarative threading, auto-discovery, and zero-boilerplate RPC let developers focus on **services, not plumbing**. Avoid fragile glue code and wasted hours wiring threads, processes, and network calls.
* **Reliable & Fault-Tolerant** — Watchdog-protected threads, resilient messaging, and dynamic service lifecycles **keep apps running even when parts fail**. No manual recovery or fragile orchestration required.
* **Scalable & Observable** — Services automatically form a **self-organizing mesh**, handle cross-dependencies, and include built-in profiling with scope-based logging for actionable insights—**from a single thread to multi-device orchestration**.

✅ Areg SDK helps developers build scalable, reliable, and maintainable C++ systems without drowning in IPC, threading, or distributed complexity.

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

## Getting Started[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#getting-started)

### Prerequisites

* **Git** for cloning the repository
* **C++17-compatible compiler**: GNU, Clang/LLVM, or MSVC (Windows)
* **CMake ≥ 3.20**
* **Java ≥ 17** (for code generation tools)

Optional: SQLite, GTest, or OS-specific libraries for extended features (see advanced setup guides in the [wiki](./docs/wiki/)).

---

### Build

Clone and build the **AREG SDK**:

```bash
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
cmake -B ./build
cmake --build ./build -j $(nproc)
```

> [!TIP]
> By default, this builds **Examples and Unit Tests**. To skip them:

```bash
cmake -B ./build -DAREG_BUILD_TESTS=OFF -DAREG_BUILD_EXAMPLES=OFF
cmake --build ./build
```

**Windows / Visual Studio**

Open `areg-sdk.sln` or run:

```bash
MSBuild ./areg-sdk.sln
```

For **IDE-specific builds, cross-compilation, or WSL**, see the **[Areg SDK Wiki](./docs/wiki/)**.

---

### Hello Service example

A minimal example showing a `ServiceConsumer` in one thread calling `requestHelloService()` on a `ServiceProvider` in another thread:

```cpp
class ServiceProvider : public Component, public HelloServiceStub {
    void requestHelloService() override {
        std::cout << "Hello Service!" << std::endl;
        Application::signalAppQuit();
    }
};

class ServiceConsumer : public Component, public HelloServiceClientBase {
    bool serviceConnected(NEService::eServiceConnection status, ProxyBase& proxy) override {
        if (HelloServiceClientBase::serviceConnected(status, proxy) && NEService::isServiceConnected(status))
            requestHelloService();
        return true;
    }
};
```

**Model setup snippet:**

```cpp
BEGIN_MODEL("ServiceModel")
    BEGIN_REGISTER_THREAD("Thread1")
        BEGIN_REGISTER_COMPONENT("ServiceProvider", ServiceProvider)
            REGISTER_IMPLEMENT_SERVICE(NEHelloService::ServiceName, NEHelloService::InterfaceVersion)
        END_REGISTER_COMPONENT()
    END_REGISTER_THREAD()
    BEGIN_REGISTER_THREAD("Thread2")
        BEGIN_REGISTER_COMPONENT("ServiceClient", ServiceConsumer)
            REGISTER_DEPENDENCY("ServiceProvider")
        END_REGISTER_COMPONENT()
    END_REGISTER_THREAD()
END_MODEL()
```

Run 🔗 [the application](examples/01_minimalrpc/) to see the provider print `"Hello Service!"` and quit.

---

### Examples & References

1. 🔗 [This minimal RPC project](examples/01_minimalrpc/) and 🔗 [Service Interface definition (HelloService.xml)](examples/01_minimalrpc/services/HelloService.siml).
2. 🔗 [Minimal IPC](examples/02_minimalipc/).
3. 🔗 [Unification: IPC/RPC and multithreading](examples/03_helloservice/).

---

### Integration & Development

The [**areg-sdk-demo**](https://github.com/aregtech/areg-sdk-demo) repository is the best starting point for integrating **AREG SDK** into your project. It provides:

* A complete working template (CMake integration + submodule usage)
* Demo projects that you can build and run immediately
* A practical reference for structuring your own applications

You can integrate **AREG SDK** in three main ways:

1. **CMake FetchContent** – automatically fetch the SDK sources in your `CMakeLists.txt`.
   → [CMake Integration Guide](./docs/wiki/02c-cmake-integrate.md)

2. **Git Submodule** – add `areg-sdk` as a submodule and include it in your build (Visual Studio or CMake).
   → [MSVC & Submodule Guide](./docs/wiki/01c-msvc-build.md)

3. **vcpkg Package** – install and link the `areg` package via [vcpkg](https://vcpkg.io/).
   → [vcpkg Integration Guide](./docs/wiki/01a-areg-package.md)

**Service Development**
Follow the [Hello Service tutorial](./docs/HelloService.md) to implement Service Providers (`xxxStub`) and Consumers (`xxxClientBase`). Define an **Application Model** to control threading and process distribution. See [03\_helloservice](./examples/03_helloservice/) for a working example.

**Supporting Tools**
The SDK ships with two optional services:

* **`mcrouter`** → distributed message routing
* **`logcollector`** → centralized logging

Both come with configuration templates in `config/areg.init`. Setup guides: [Multicast Router](./docs/wiki/05a-mcrouter.md), [Log Collector](./docs/wiki/04d-logcollector.md).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Motivation[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#motivation)

Traditionally, devices act as connected clients to stream data to the cloud or fog servers for further processing.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/mist-network.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/mist-network.png" alt="Diagram showing IoT-to-Cloud (Nebula) network connections" style="width:70%;height:70%"/></a></div>

As data is generated and collected at the edge of the network (**mist network**), there is a growing need to redefine the role of connected Things and enable network-accessible **Public Services** on the edge device, thereby extending the **Cloud** capabilities to the extreme edge. This approach provides a robust foundation for solutions like:
* **Enhancing data privacy**, which is crucial for sensitive information.
* **Decrease data streaming**, which is a fundamental condition to optimize network communication.
* **Autonomous, intelligent and self-aware devices** with services directly in the environment of data origin.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Interface-centricity[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#interface-centricity)

At the core of AREG is **ORPC** (_Object Remote Procedure Call_), which targets interfaces on objects. This allows AREG to establish a **service mesh** or **service grid** where applications expose reusable services. Clients, without knowledge of the server's network location, can request services seamlessly via method invocation.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/interface-centric.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/interface-centric.png" alt="Diagram showing multiprocess communication" style="width:50%;height:50%"/></a></div>

This **interface-driven Object RPC** model mirrors object-oriented programming principles and is flexible in managing multiple object instances. It imposes no protocol limitations and supports bi-directional communication to ensure seamless messaging between connected software nodes. In this model:
* **Service Providers** (*micro-server objects*) offer reusable, accessible services.
* **Service Consumers** (*micro-client objects*) invoke services without needing to know the network details.

AREG's design integrates **Client-Server (Request-Reply)** and **Publish-Subscribe (PubSub)** models, enabling it to support both action- and data-centric communication.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## More than Embedded[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#more-than-embedded)

The architecture of AREG is ideal for embedded applications, but its capabilities extend far beyond. AREG offers distributed and scalable solutions for **multithreading**, **multiprocessing**, and **internet** communications, making it a versatile choice for a wide range of applications. Services in AREG are categorized into three types: **Local**, **Public**, and **Internet**, enabling flexible and efficient remote communication across diverse environments.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-services.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-services.png" alt="Diagram showing Services and AREG Framework message handling" style="width:70%;height:70%"/></a></div>

> [!NOTE]
> AREG currently supports **Local** (multithreading) and **Public** (multiprocessing) services.

The fault-tolerant design of AREG offers key advantages, such as:
* **Resilience:** Failure in one application does not affect the overall system.
* **Automatic Discovery:** Services are automatically discovered by clients without manual configuration.
* **Thread-Safe Execution:** All service methods are executed within their respective thread contexts, ensuring thread safety and independence.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Composition of AREG SDK[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#composition-of-areg-sdk)

The **AREG SDK** consists of several modules to streamline distributed, real-time applications development:
- **[AREG Communication Engine (*areg*)](./framework/areg/):** Core framework enabling communication between software components.
- **[Multicast Router (*mcrouter*)](./framework/mcrouter/):** Facilitates message routing between services.
- **[Log Collector (*logcollector*)](./framework/logcollector/):** Collects logs from applications and forwards them to log observers.
- **[Log Observer Library (*areglogger*)](./framework/areglogger/):** Library for receiving logs from the log collector service.
- **[Log Observer (*logobserver*)](./framework/logobserver/):** In real-time mode monitors, saves and dynamically controls logs.
- **[AREG Extended Library (*aregextend*)](./framework/aregextend/):** Offers additional objects with extended features.
- **[Code Generator (*codegen.jar*)](./master/tools/):** Generates Service **Provider** and **Consumer** objects from Service Interface.
- **[Examples](./examples/):** Illustrates the features of AREG Framework and use of the AREG SDK components.
- **[UI Tool (*lusan*)](https://github.com/aregtech/areg-sdk-tools):** Simplifies and visualizes service design, log views.

> [!NOTE]
> The UI tool **[Lusan](https://github.com/aregtech/areg-sdk-tools)** is currently under the development. It is supposed to provide multiple features like Service Interface design, log viewing, and runtime testing. We call to join this open source project to develop the tool.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Examples[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#examples)

The AREG SDK offers hands-on examples demonstrating **Multithreading** and **Multiprocessing** applications, **Client-Server** and **Publish-Subscribe** models, **Object Remote Procedure Call (Object RPC)** and **Inter-Process Communication (IPC)**, featured **Finite-State Machines (FSM)** creation, and more. Each project highlights key features that facilitate efficient development of distributed services.

**Some Featured Examples:**
1. **[03\_helloservice](./examples/03_helloservice/)**: Master service creation across single-threaded, multi-threaded, and multi-process environments, showcasing AREG's intuitive, interface-driven approach.

2. **[07\_logging](./examples/07_logging/)**: Configure and manage logging to track application behavior, aiding debugging, performance analysis, and log management.

3. **[19\_pubfsm](./examples/19_pubfsm/)**: Build and control a *Finite-State Machine (FSM)* with AREG's *Timers* and *Events* for smooth state transitions.

4. **[22\_pubwatchdog](./examples/22_pubwatchdog/)**: Implement a watchdog to monitor thread activity, restart unresponsive threads, and notify components as needed.

5. **[27\_pubsubmulti](./examples/27_pubsubmulti/)**: Explore the PubSub model, which reduces data traffic by delivering only relevant updates.

For the full list of examples and additional documentation, visit **[AREG SDK Examples](./examples/README.md)**.

> *Accelerate your multithreading, multiprocessing, embedded, IoT edge, and event-driven development with these examples. For technical inquiries, please contact us.*

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

## Use Cases and Benefits[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#use-cases-and-benefits)

The **AREG SDK** enables efficient multithreading and multiprocessing communication, offering full support for *edge computing* and *fog computing* use cases that demand efficient communication between devices and services in real-time. It is ideal for developing:

- **Distributed solutions** for embedded systems.
- **Real-time applications** for IoT devices.
- **Simulation and test environments**.

Explore **[real-world use cases and examples](./docs/USECASES.md)** to learn more about its applications.

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
