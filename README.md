<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.aregtech.com"><img align="center" src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-sdk-1280x360px-logo.png" alt="AREG SDK Home" style="width:100%;height:100%"/></a>
  <br /><br /><strong>AREG SDK</strong>
</h1>

*Discover AREG - an advanced framework for real-time communication in mist- and fog- network environments.*

[![Latest release](https://img.shields.io/github/v/release/aregtech/areg-sdk?label=Latest%20release&style=social)](https://github.com/aregtech/areg-sdk/releases/tag/v1.5.0)
[![GitHub commits](https://img.shields.io/github/commits-since/aregtech/areg-sdk/v1.5.0.svg?style=social)](https://GitHub.com/aregtech/areg-sdk/commit/)
[![Stars](https://img.shields.io/github/stars/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/stargazers)
[![Fork](https://img.shields.io/github/forks/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/network/members)
[![Watchers](https://img.shields.io/github/watchers/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/watchers)
[![Wiki Pages](https://img.shields.io/badge/AREG%20Wiki%20Pages-8-brightgreen?style=social&logo=wikipedia)](https://github.com/aregtech/areg-sdk/wiki/)

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

## Introduction[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#introduction)

**AREG Framework** (*Automated Real-time Event Grid*) is an asynchronous, interface-driven communication engine that enables connected software nodes to form a network of distributed services. By facilitating seamless remote object interactions, AREG allows nodes to communicate without knowledge of their network locations, as if they coexist within the same process and thread. Lightweight and designed for IoT **[fog- and mist-network](https://csrc.nist.gov/publications/detail/sp/500-325/final)**, it empowers devices to deliver real-time public services, enabling efficient remote access and collaboration.

---

## Table of contents[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#table-of-contents)
- [Motivation](#motivation)
- [Interface-centricity](#interface-centricity)
- [More than Embedded](#more-than-embedded)
- [Composition of AREG SDK](#composition-of-areg-sdk)
- [Getting Started: Clone and Build the AREG SDK](#getting-started-clone-and-build-the-areg-sdk)
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

## Getting Started: Clone and Build the AREG SDK[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#getting-started-clone-and-build-the-areg-sdk)

### General Requirements
Ensure your system includes the following:
- **Git** for repository cloning.
- **Compatible Compilers**: *GNU*, *LLVM*, or *MSVC* (Windows only) supporting **C++17** or newer.
- **CMake** (version 3.20+).
- **Java** (version 17+ for code generation tools).
- **Optionally**, you may install following packages:
  - `ncurses` library to support optional objects with extended features in Linux or Cygwin;
  - **CMake**, **Clang** and **MFC** packages of Microsoft Visual Studio;
  - **SQLite** and **GTest** packages.

### Cloning Sources

To obtain the AREG SDK source codes, use the following command:
```bash
git clone https://github.com/aregtech/areg-sdk.git
```

### Build Instructions

The **AREG SDK** is written in **C++17**, supports multiple platforms, processors and compilers:

| Compiler  | Platforms     | Tools         | API           | CPU Architecture          |
|-----------|---------------|---------------|---------------|---------------------------|
| GNU       | Linux, macOS  | CMake         | POSIX         | x86, x86_64, ARM, AARCH64 |
| Clang     | Linux, Windows| CMake, MSVS   | POSIX, Win32  | x86, x86_64, ARM, AARCH64 |
| MSVC      | Windows       | CMake, MSVS   | Win32         | x86, x86_64               |
| Cygwin GNU| Windows       | CMake         | POSIX         | x86, x86_64               |

> [!NOTE] 
> Other POSIX-compliant operating systems and compilers have not been tested yet.

#### Build with CMake

To compile the **AREG SDK** using **CMake**, follow these steps:

```bash
cmake -B ./build
cmake --build ./build -j 20
```

> [!TIP]
> By default, AREG SDK sources are built with Examples and Unit Tests. To exclude **AREG Unit Tests** from the build process, set the `AREG_BUILD_TESTS` CMake option to `OFF`. Similarly, to exclude **AREG Examples**, set the `AREG_BUILD_EXAMPLES` CMake option to `OFF`.
> 
> Below is an example of configuring and building the AREG SDK sources without Unit Tests and Examples:
> ```bash
> cmake -B ./build -DAREG_BUILD_TESTS=OFF -DAREG_BUILD_EXAMPLES=OFF
> cmake --build ./build
> ```

For further details on **customizing builds or cross-compiling** with CMake see **[Building AREG SDK with CMake](./docs/wiki/01b-cmake-build.md)** page.

#### Build with Microsoft Visual Studio

Open `areg-sdk.sln` file in Microsoft Visual Studio and build the solution, or navigate to the root directory of the project and run build with **MSBuild**:

```bash
MSBuild ./areg-sdk.sln
```

For further details on **customizing builds** with Visual Studio, visit the **[Building the AREG SDK with Microsoft Visual Studio and MSBuild](./docs/wiki/01c-msvc-build.md)** page.

#### Additional Build Options

- **IDE Support:** Includes instructions for **[Microsoft Visual Studio](./docs/wiki/01c-msvc-build.md)** and **[Visual Studio Code](./docs/wiki/01b-cmake-build.md)**. The other IDEs are currently not in the focus.
- **WSL Support:** Detailed steps for building under **[Windows Subsystem for Linux (WSL)](./docs/wiki/01d-wsl-build.md)** are provided.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Integration and Development[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#integration-and-development)

For a practical example of building real-time, distributed systems using **AREG SDK**, check out the **[AREG SDK Demo](https://github.com/aregtech/areg-sdk-demo)** project, which includes implementations of multitasking applications for **embedded, IoT mist- and fog-systems**.

### AREG SDK Integration Methods

There are three ways to integrate the AREG SDK into your project:

#### 1. Integrate by Fetching sources

Add the following script to your `CMakeLists.txt` to integrate AREG SDK:

```cmake
find_package(areg CONFIG)
if (NOT areg_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        areg-sdk
        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
        GIT_TAG "master"
    )
    FetchContent_MakeAvailable(areg-sdk)
    set(AREG_SDK_ROOT "${areg-sdk_SOURCE_DIR}")
    include_directories("${AREG_SDK_ROOT}/framework")
endif()

include("${AREG_SDK_ROOT}/areg.cmake")
```

This either finds or fetches the AREG SDK components from `master` branch. See the **[Building AREG SDK with CMake](./docs/wiki/02c-cmake-integrate.md)** page for more details.

#### 2. Integrate as a project submodule

1. Add `areg-sdk` as a **Git submodule** in your project.
2. Include `areg-sdk` project in your build:
   - **Microsoft Visual Studio**: include the `*.vcxproj` files from `<areg-sdk>/framework` in your solution.
   - **CMake**: include the `<areg-sdk-root>/CMakeLists.txt` in your CMake script. 
3. Link your projects with the `areg` library and set project dependencies.

For full details, see the **[Building the AREG SDK with Microsoft Visual Studio and MSBuild](./docs/wiki/01c-msvc-build.md)**.

#### 3. Integrate the `areg` Package (vcpkg)

> [!IMPORTANT]
> Starting with AREG SDK 2.0, you can integrate it using the `vcpkg` package manager.

Before starting, visit the **[official vcpkg](https://github.com/microsoft/vcpkg)** repository to clone and install the `vcpkg` package manager tool in your PC.

1. **Install and integrate the `areg` package:** Example of the AREG SDK components, headers and its dependencies installation under Linux:
   ```bash
   ./vcpkg install areg:linux-64
   ./vcpkg integrate install
   ```

2. **Integrate with CMake:** Add the following script to your `CMakeLists.txt` (replace `<example>` with real target):
   ```cmake
   find_package(areg CONFIG REQUIRED)
   target_link_libraries(<example> PRIVATE areg::areg)
   ```
   Use `vcpkg` toolchain file (`<vcpkg-root>` is the root path of `vcpkg`) to configure and build the project:
   ```bash
   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
   cmake --build ./build -j 20
   ```

3. **Integrate with Microsoft Visual Studio:** In Microsoft Visual Studio, there is no need to add AREG SDK projects in the solution file. Develop projects and make sure to link `areg` library with binaries either in *Project Properties* or by adding this line of code:
   ```cpp
   #pragma comment(lib, "areg")
   ```

For details of installing and using `areg` package, see the appropriate **[integrate `areg` package](./docs/wiki/01a-areg-package.md)** document.

### Service Creation and Development

Follow the **[“Hello Service!”](./docs/HelloService.md)** tutorial for step-by-step instructions. Generated service interface files integrate with CMake or Visual Studio projects. Service Providers extend **xxxStub**, and Service Consumers extend **xxxClientBase**.

### Example: Application Model Setup

When developing **Service Provider** and **Service Consumer** components, developers can easily determine whether to run components in the same process (multithreading) or in separate processes (multiprocessing) for optimal performance in real-time applications by defining the **Application Model**. Below is an example of setting up a *model* where the *Service Provider* and *Service Consumer* components run in the same process, but on different threads.

```cpp
BEGIN_MODEL("ApplicationModel")
    // Provider Thread
    BEGIN_REGISTER_THREAD("ProviderThread", NECommon::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT("ServiceProvider", ServiceComponent)
            REGISTER_IMPLEMENT_SERVICE(NEHelloService::ServiceName, NEHelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceProvider")
    END_REGISTER_THREAD("ProviderThread")
        
    // Consumer Thread
    BEGIN_REGISTER_THREAD("ConsumerThread", NECommon::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT("ServiceConsumer", ClientComponent)
            REGISTER_DEPENDENCY("ServiceProvider")
        END_REGISTER_COMPONENT("ServiceConsumer")
    END_REGISTER_THREAD("ConsumerThread")
END_MODEL("ApplicationModel")

int main(void)
{
    Application::initApplication();
    Application::loadModel("ApplicationModel");
    Application::waitAppQuit();
    Application::unloadModel("ApplicationModel");
    Application::releaseApplication();
    return 0;
}
```

In this example:
- **ServiceProvider** runs on `ProviderThread`, and **ServiceConsumer** runs on `ConsumerThread`.
- `REGISTER_DEPENDENCY("ServiceProvider")` means `ServiceConsumer` component consumes the services provided by `ServiceProvider`.

You can also set up multiprocess applications using same components and changing *model*. As a practical example, follow projects in the **[00_helloservice](./examples/00_helloservice/)** directory.

> [!TIP]
> *Learn how AREG SDK simplifies the creation of Service Providers and Consumers, supporting both multithreading and multiprocessing for real-time, distributed applications.* Visit **[examples](./examples/README.md)** to see the list of demonstrated applications and features of the AREG communication engine.

### Multicast Router and Log Collector

**mcrouter** and **logcollector** are essential services within the AREG SDK ecosystem, enabling seamless communication and efficient logging. Configuration templates for both are included in the [`areg.init`](./framework/areg/resources/areg.init) file, which is placed in the `config` subdirectory of the build binaries. These applications can function as standalone console tools or be deployed as **Operating System managed services**, providing deployment flexibility.  

For detailed instructions on building and using these services, see the **[Multicast Router Service](./docs/wiki/05a-mcrouter.md)** and **[Log Collector Service](./docs/wiki/04d-logcollector.md)** documentation. *Explore how they enhance communication and logging in real-time and edge-computing environments.*  

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Examples[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#examples)

The AREG SDK offers hands-on examples demonstrating **Multithreading** and **Multiprocessing** applications, **Client-Server** and **Publish-Subscribe** models, **Object Remote Procedure Call (Object RPC)** and **Inter-Process Communication (IPC)**, featured **Finite-State Machines (FSM)** creation, and more. Each project highlights key features that facilitate efficient development of distributed services.

**Some Featured Examples:**
1. **[00_helloservice](./examples/00_helloservice/)**: Master service creation across single-threaded, multi-threaded, and multi-process environments, showcasing AREG's intuitive, interface-driven approach.

2. **[04_logging](./examples/04_logging/)**: Configure and manage logging to track application behavior, aiding debugging, performance analysis, and log management.

3. **[16_pubfsm](./examples/16_pubfsm/)**: Build and control a *Finite-State Machine (FSM)* with AREG's *Timers* and *Events* for smooth state transitions.

4. **[19_pubwatchdog](./examples/19_pubwatchdog/)**: Implement a watchdog to monitor thread activity, restart unresponsive threads, and notify components as needed.

5. **[24_pubsubmulti](./examples/24_pubsubmulti/)**: Explore the PubSub model, which reduces data traffic by delivering only relevant updates.

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
