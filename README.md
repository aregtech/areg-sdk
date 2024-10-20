<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.aregtech.com"><img align="center" src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-sdk-1280x360px-logo.png" alt="AREG SDK Home" style="width:100%;height:100%"/></a>
  <br /><br /><strong>AREG SDK</strong>
</h1>

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

**AREG** (*Automated Real-time Event Grid*) is an advanced communication framework designed for seamless data transmission in IoT [fog- and mist-network environments](https://csrc.nist.gov/publications/detail/sp/500-325/final). Lightweight and platform-agnostic, AREG enables efficient, automated communication using a combination of **Client-Server** and **Publish-Subscribe** models. By adopting an interface-centric approach, AREG facilitates the creation of service grids, where multiple devices and software nodes function collaboratively as distributed micro-servers and clients, and ensure reliable real-time data exchange across a network of connected nodes.

---

## Table of contents[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#table-of-contents)
- [Motivation](#motivation)
- [Interface-centricity](#interface-centricity)
- [More than embedded](#more-than-embedded)
- [Composition of AREG SDK](#composition-of-areg-sdk)
- [Getting Started: Clone and Build the AREG SDK](#getting-started-clone-and-build-the-areg-sdk)
  - [Cloning Sources](#cloning-sources)
  - [Build Instructions](#build-instructions)
    - [Build with CMake](#build-with-cmake)
    - [Build with Microsoft Visual Studio](#build-with-microsoft-visual-studio)
    - [Additional Build Options](#additional-build-options)
- [Integration and Development](#integration-and-development)
  - [Starting a Project with CMake](#starting-a-project-with-cmake)
  - [Starting a Project with Microsoft Visual Studio](#starting-a-project-with-microsoft-visual-studio)
  - [Starting a Project using the `areg` Package](#starting-a-project-using-the-areg-package)
  - [Service Creation and Development](#service-creation-and-development)
  - [Example: Defining an Application Model](#example-defining-an-application-model)
  - [Multicast Router and Log Collector](#multicast-router-and-log-collector)
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

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/mist-network.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/mist-network.png" alt="IoT-to-Cloud (Nebula) network" style="width:70%;height:70%"/></a></div>

As data is generated and collected at the edge of the network (**mist network**), there is a growing need to redefine the role of connected Things and enable network-accessible _Public Services_ on the edge device, thereby extending the _Cloud_ capabilities to the extreme edge. This approach serves as a strong foundation for implementing robust solutions such as:
* _Increase data privacy_, which is an important factor for sensitive data.
* _Decrease data streaming_, which is a fundamental condition to optimize network communication.
* _Autonomous, intelligent and self-aware devices_ with services directly in the environment of data origin.

<div align="right"><kbd>[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</kbd></div>

---

## Interface-centricity[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#interface-centricity)

At the core of AREG is **ORPC** (_Object Remote Procedure Call_), which targets interfaces on objects. This allows AREG to establish a **service mesh** or **service grid** where applications expose reusable services. Clients, without knowledge of the server’s network location, can request services seamlessly via method invocation.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/interface-centric.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/interface-centric.png" alt="Service oriented and interface-centric" style="width:50%;height:50%"/></a></div>

This interface-centric ORPC model mirrors object-oriented programming principles and is flexible in managing multiple object instances. It imposes no protocol limitations and supports bi-directional communication to ensure seamless messaging between connected nodes. In this model:
* **Service Providers** (server objects) offer reusable, accessible services.
* **Service Consumers** (client objects) invoke services without needing to know the network details.

AREG’s design integrates **Client-Server (Request-Reply)** and **Publish-Subscribe (PubSub)** models, enabling it to support both action- and data-centric communication.

<div align="right"><kbd>[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</kbd></div>

---

## More than Embedded[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#more-than-embedded)

AREG's **interface-centric** architecture is perfect for embedded applications but extends to scalable **multithreading**, **multiprocessing**, and **internet-based communications**. It organizes services into three categories: **Local**, **Public**, and **Internet**, enabling flexible and efficient remote communication across diverse environments.

The **interface-centric** architecture of AREG is ideal for embedded applications, but its capabilities extend far beyond. AREG offers distributed and scalable solutions for **multithreading**, **multiprocessing**, and **internet-based communications**, making it a versatile choice for a wide range of applications. Services in AREG are categorized into three types: **Local**, **Public**, and **Internet**, enabling flexible and efficient remote communication across diverse environments.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-services.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-services.png" alt="AREG SDK distributed services" style="width:70%;height:70%"/></a></div>

> [!NOTE]
> AREG currently supports **Local** (multithreading) and **Public** (multiprocessing) services.

Key benefits of AREG's fault-tolerant design include:
* **Resilience:** Failure in one application does not affect the overall system.
* **Automatic Discovery:** Services are automatically discovered by clients without manual configuration.
* **Thread-Safe Execution:** All service methods are executed within their respective thread contexts, ensuring thread safety and independence.

<div align="right"><kbd>[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</kbd></div>

---

## Composition of AREG SDK[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#composition-of-areg-sdk)

The **AREG SDK** consists of several essential modules to streamline development:
- **[AREG communication engine (*areg*)](./framework/areg/):** Core framework enabling communication between software components.
- **[Multicast router (_mcrouter_)](./framework/mcrouter/):** Facilitates message routing between services.
- **[Log collector (*logger*)](./framework/logger/):** Collects logs from various applications and forwards them to log observers.
- **[Log observer (*areglogger*)](./framework/areglogger/):** Library for receiving logs from the log collector service.
- **[AREG Extended Library (*aregextend*)](./framework/aregextend/):** Offers additional objects with extended features.
- **[Code generator](./master/tools/):** Generates Service **Provider** and **Consumer** objects from Service Interface documents.
- **[Examples](./examples/):** Illustrates the features of AREG Framework and use of the AREG SDK components.
- **[*Lusan* UI Tool](https://github.com/aregtech/areg-sdk-tools):** Simplifies and visualizes service design, log views.

> [!NOTE]
> The UI tool **[Lusan](https://github.com/aregtech/areg-sdk-tools)** is currently under the development. It supposed to provide multiple features like Service Interface design, log viewing and runtime testing. We call to join this open source project to develop the tool.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Getting Started: Clone and Build the AREG SDK[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#getting-started-clone-and-build-the-areg-sdk)

### Cloning Sources

To obtain the AREG SDK source codes, use the following command:
```bash
git clone https://github.com/aregtech/areg-sdk.git
```

### Build Instructions

The AREG SDK is written in **C++17** and supports multiple platforms and compilers:
- **Supported platforms:** Linux, Windows, Cygwin and macOS. 
- **Supported compilers:** GNU, Clang, MSVC and Cygwin GNU. 
- **Supported CPU:** x86, x86_64, arm, and aarch64. 

The following build tools are supported:

| Tool | Solution | Platforms | API | Quick actions to compile |
| --- | --- | --- | --- | --- |
| **CMake**   | `CMakeLists.txt` | Linux, Windows, Cygwin | POSIX, Win32 | Build with CMake, VSCode, or MSVS. |
| **MSBuild** | `areg-sdk.sln`   | Windows                | Win32        | Build with MSBuild or MSVS. |

For detailed build instructions, check the **[Software Build](./docs/wiki/BUILD.md)** page.


> [!NOTE] 
> Other POSIX-compliant operating systems and compilers have not yet been tested yet.

#### Build with CMake

To compile the AREG SDK using **CMake**, follow these steps:

```bash
cmake -B ./build
cmake --build ./build -j 20
```

For custom builds, pass the options of overwrite the settings listed in **[user.cmake](./docs/wiki/USER-CMAKE.md)**.

#### Build with Microsoft Visual Studio

Open `areg-sdk.sln` file in Microsoft Visual Studio and build the solution or build with **MSBuild** on Windows by running the following:

```bash
MSBuild .
```

For more details on customizing builds, visit the **[Build with MSBuild](./docs/wiki/MSVS-BUILD.md)** document.

#### Additional Build Options

- **IDE Support:** Includes instructions for **[Microsoft Visual Studio](./docs/wiki/MSVS-BUILD.md)** and **[Visual Studio Code](./docs/wiki/VSCODE.md)**.
- **WSL Support:** Detailed steps for building under **[Windows Subsystem for Linux (WSL)](./docs/wiki/WSL.md)** are provided.

> [!NOTE]
> This chapter focuses on building AREG and examples using **Microsoft Visual Studio** and **Visual Studio Code**. The other IDEs are currently not in the focus.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Integration and Development[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#integration)

> [!TIP]  
> For detailed examples, please refer to the **[AREG SDK Demo](https://github.com/aregtech/areg-sdk-demo)** project.

Integrating the AREG SDK into a project is straightforward with the following three methods:

### Starting a Project with CMake

To integrate AREG SDK into a CMake project, add the following to your `CMakeLists.txt`:

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
endif()

include("${AREG_CMAKE_CONFIG_DIR}/conf/cmake/areg.cmake")
include_directories("${AREG_SDK_ROOT}/framework")
```

This ensures that either the AREG SDK components are found or fetched the sources and included automatically. For detailed information of integrating with CMake, see the appropriate **[Integrate with CMake](./docs/wiki/CMAKE-INTEGRATE.md)** document.

### Starting a Project with Microsoft Visual Studio

To integrate with Microsoft Visual Studio:
1. Add `areg-sdk` as a submodule to your project.
2. Include the `*.vcxproj` files from the `<areg-sdk>/framework` directory in your solution.
3. Link your project with the `areg` library and don't forget to set project depdencies.

This ensures that the sources of AREG SDK are compiled and linked with the project. For detailed information of integrating with Microsoft Visual Studio, see the appropriate **[Integrate with MS Visual Studio](./docs/wiki/MSVS-INTEGRATE.md)** document.

### Starting a Project using the `areg` Package

> [!IMPORTANT]  
> Starting from AREG SDK version 2.0, the `areg` package is planned to be included in the official `vcpkg` environment.

Using the `areg` package simplifies integration by avoiding manual compilation or submodule setup. Follow these steps:

1. **Set up vcpkg:**  
   Clone the **[vcpkg repository](https://github.com/microsoft/vcpkg)** and follow the **[installation instructions](https://github.com/microsoft/vcpkg#quick-start)** to set it up.

2. **Install the `areg` package:**  
   Use vcpkg to install the `areg` package and its dependencies. For example, on Linux, run:
   ```bash
   ./vcpkg install areg:linux-64
   ```

3. **Integrate with your environment:**  
   Run the following command to integrate the `areg` package:
   ```bash
   ./vcpkg integrate install
   ```
   This command will display the required CMake options for integration.

4. **CMake integration:**  
   Add the following to your `CMakeLists.txt`:
   ```cmake
   find_package(areg CONFIG REQUIRED)
   ```
   Then link your target with the `areg::areg` library:
   ```cmake
   target_link_libraries(<example> PRIVATE areg::areg)
   ```
   When building your project, use the CMake toolchain file from vcpkg:
   ```bash
   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
   ```
   where `<vcpkg-root>` should the root path of `vcpkg` installation directory.

5. **Visual Studio integration:**  
   For Microsoft Visual Studio, link your project with `areg.lib` either through project settings or by adding:
   ```cpp
   #pragma comment(lib, "areg")
   ```

This ensures that the components and header files of AREG SDK and its dependencies are installed on the machine and can be used. For detailed information of installing and integrating with `areg` package, see the appropriate **[integrate `areg` package](./docs/wiki/AREG-PACKAGE.md)** document.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

### Service Creation and Development

Services can be created by following the **["Hello Service!"](./docs/HelloService.md)** tutorial, with step-by-step guidance for local and public services. Generated service interface files integrate with CMake or Visual Studio projects. Service Providers extend **xxxStub**, and Service Consumers extend **xxxClientBase**.

### Example: Defining an Application Model

When developing Service Provider and Service Consumer components, developers can easily decide whether these components should run in the same process (multithreading) or in different processes (multiprocessing). This decision is made when defining the **Application Model**. Below is an example of how to set up a *model* where the Service Provider and Service Consumer components run in the same process but on different threads.

```cpp
BEGIN_MODEL("ServiceModel")
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
END_MODEL("ServiceModel")

int main(void)
{
    Application::initApplication();
    Application::loadModel("ServiceModel");
    Application::waitAppQuit();
    Application::unloadModel("ServiceModel");
    Application::releaseApplication();
    return 0;
}
```

In this example:
- **ServiceProvider** runs on `ProviderThread`, and **ServiceConsumer** runs on `ConsumerThread`.
- The `ServiceConsumer` depends on the `ServiceProvider`, meaning it consumes the services provided by the `ServiceProvider`.

Developers can similarly set up an application with multiple processes: one process running the Service Provider, and another running the Service Consumer. To see how to define both multithreading and multiprocessing applications, or a mixed model where the Service Provider and Consumer components are used across different processes, refer to the **[00_helloservice_](./examples/00_helloservice/)** example in the AREG SDK.

> [!TIP]
> Additionally, visit **[examples](./examples/README.md)** to see the list of demonstrated applications and features of the AREG communication engine.

### Multicast Router and Log Collector

Both **mcrouter** and **logger** are essential components for communication and log collection. Configuration templates for these services can be found in the `areg.init` file. Both processes are designed to run as console application or as *OS-managed* services.

For in-depth details of building and using these applications, review the descriptions in **[Multicast Router Service](./docs/wiki/MCROUTER.md)** and **[Log Collector Service](./docs/wiki/LOGGER.md)** pages.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Pipeline and Roadmap[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#pipeline-and-roadmap)

The AREG SDK is continuously evolving to help developers create complex applications more efficiently on **Desktop**, **Embedded**, and **IoT edge** devices. It aims to reduce development time by **30-50%** while ensuring reliability and flexibility.

**Upcoming Features:**
- **Multi-channel & multi-protocol communication**: Enhanced support for various communication methods, improving interoperability.
- **Internet category services**: Expansion into web-based services, enabling more modern, internet-driven applications.

**Tools in Development:**
- **Service Interface Designer**: A visual tool to simplify service interface creation.
- **Interactive Log Viewer**: A tool for easy log visualization and performance analysis.
- **Service Testing & Simulation Tool**: Helps simulate services and data, streamlining testing and development.

The tools are actively being developed in the **[AREG SDK Tools repository](https://github.com/aregtech/areg-sdk-tools)**.

<div align="right"><kbd>[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</kbd></div>

---

## Use Cases and Benefits[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#use-cases-and-benefits)

The AREG SDK enables efficient multithreading and multiprocessing communication. It is ideal for developing:

- **Distributed solutions** for embedded systems.
- **Real-time applications** for IoT devices.
- **Simulation and test environments**.

Explore **[real-world use cases and examples](./docs/USECASES.md)** to learn more about its applications.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## License[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#license)

The AREG SDK is available under the **[Apache License Version 2.0](./LICENSE.txt)**, a permissive free open-source license. For developers or businesses that need commercial licensing, this option is available and includes:
- Commercial support.
- Full rights to create and distribute software without open-source license obligations.

For more information on commercial licensing, visit the **[Aregtech](https://www.aregtech.com/)** website or contact us at **info[at]aregtech[dot]com**.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Call to action[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#call-to-action)

We encourage the developer community to get involved and contribute to the growth of the AREG SDK. Here’s how you can help:

- Check out our list of [open issues](https://github.com/aregtech/areg-sdk/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) where assistance is needed.
- If you have suggestions or need new features, open a [new issue](https://github.com/aregtech/areg-sdk/issues) or start a [discussion](https://github.com/aregtech/areg-sdk/discussions).
- For pull requests, ensure that the code adheres to our coding style and allow time for review and testing.

For commercial support, training, or partnership inquiries, reach out to **info[at]aregtech[dot]com**.

Lastly, please consider [**starring** the AREG SDK on GitHub](https://github.com/aregtech/areg-sdk/) to support the project and help grow our community!

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Thank you all!![![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#thank-you)

Special thanks to all contributors and supporters that starred this repository.

**Our amazing contributors**:

[![Contributors for @aregtech/areg-sdk](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/contributors.svg)](https://github.com/aregtech/areg-sdk/graphs/contributors)

**Our supportive stargazers**:

[![Stargazers of @aregtech/areg-sdk repo](http://reporoster.com/stars/aregtech/areg-sdk)](https://github.com/aregtech/areg-sdk/stargazers)

Do you like this project? Please join us or [give](https://github.com/aregtech/areg-sdk/stargazers) a ⭐. This will help to attract more contributors.<br/>
Do you have an idea or found a bug? Please open an [issue](https://github.com/aregtech/areg-sdk/issues) or start a [discussion](https://github.com/aregtech/areg-sdk/discussions).

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

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
