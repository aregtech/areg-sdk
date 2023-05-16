<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.aregtech.com"><img align="center" src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-sdk-1280x360px-logo.png" alt="AREG SDK Home" style="width:100%;height:100%"/></a>
  <br /><br /><strong>AREG SDK</strong>
</h1>

[![Latest release](https://img.shields.io/github/v/release/aregtech/areg-sdk?label=Latest%20release&style=social)](https://github.com/aregtech/areg-sdk/releases/tag/v1.0.0)
[![GitHub commits](https://img.shields.io/github/commits-since/aregtech/areg-sdk/v1.0.0.svg?style=social)](https://GitHub.com/aregtech/areg-sdk/commit/)
[![Stars](https://img.shields.io/github/stars/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/stargazers)
[![Fork](https://img.shields.io/github/forks/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/network/members)
[![Watchers](https://img.shields.io/github/watchers/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/watchers)


---

<!-- markdownlint-disable -->
## Project Status[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#project-status)
<table class="no-border">
  <tr>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml" alt="CMake"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml/badge.svg" alt="CMake build"/></a></td>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/c-cpp.yml" alt="C/C++"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/c-cpp.yml/badge.svg" alt="C/C++ Make"/></a></td>
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

**AREG** (*Automated Real-time Event Grid*) is a communication engine that enables data transmission in IoT [fog- and mist-network](https://csrc.nist.gov/publications/detail/sp/500-325/final). It is lightweight, automated, featured, and works on different platforms. AREG uses an interface-centric approach to create a grid of services that allows multiple devices and software nodes to operate like distributed servers and clients. By automating real-time data transmission, and using both Client-Server and Publish-Subscribe models, AREG ensures reliable and efficient communication between connected software nodes.

---

## Table of contents[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#table-of-contents)
- [Motivation](#motivation)
- [Interface-centricity](#interface-centricity)
- [More than embedded](#more-than-embedded)
- [Composition](#composition)
- [Clone sources](#clone-sources)
- [Software build](#software-build)
  - [Build with `cmake`](#build-with-cmake)
  - [Build with `make`](#build-with-make)
  - [Build with `msbuild`](#build-with-msbuild)
  - [Build with WSL](#build-with-wsl)
  - [Build with IDEs](#build-with-ides)
    - [Build with Microsoft Visual Studio](#build-with-microsoft-visual-studio)
    - [Build with Visual Studio Code](#build-with-visual-studio-code)
- [Integration](#integration)
  - [Start a project](#start-a-project)
  - [Configure multicast router](#configure-multicast-router)
  - [Configure logging](#configure-logging)
- [Roadmap](#roadmap)
- [Use cases and benefits](#use-cases-and-benefits)
- [Examples](#examples)
- [License](#license)
- [Call to action](#call-to-action)
- [Thank you all!](#thank-you-all)

---

## Motivation[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#motivation)

Traditionally, devices act as connected clients to stream data to the cloud or fog servers for further processing.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/mist-network.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/mist-network.png" alt="IoT-to-Cloud (Nebula) network" style="width:70%;height:70%"/></a></div>

As data is generated and collected at the edge of the network (**mist network**), it is logical to redefine the role of connected Things and enable network-accessible services (_Public Services_) on them, thereby extending the _Cloud_ to the extreme edge. This approach serves as a strong foundation for implementing robust solutions such as:
* _Increase data privacy_, which is an important factor for sensitive data.
* _Decrease data streaming_, which is a fundamental condition to optimize network communication.
* _Autonomous, intelligent and self-aware devices_ with services directly in the environment of data origin.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Interface-centricity[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#interface-centricity)

**ORPC**, or _Object Remote Procedure Call_, is a remote procedure call concept that targets an interface on an object. It enables the construction of a _service mesh_ (or _service grid_), where applications offer reusable services, and programmable client objects request method execution of programmable server objects without needing to know their location in the network.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/interface-centric.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/interface-centric.png" alt="Service oriented and interface-centric" style="width:50%;height:50%"/></a></div>

The ORPC concept is interface-centric, similar to object-oriented programming, and supports transparent handling of multiple instances of the same object. There are no protocol restrictions, although the bi-directional communication is required to send messages to all connected nodes. The programmable server objects are called _service providers_, and the programmable clients are called _service consumers_. This approach enables AREG to combine the features of _action-centric_ (Client-Server / Request-Reply) and _data-centric_ (Publish-Subscribe / PubSub) models.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## More than embedded[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#more-than-embedded)

AREG is designed to provide a homogeneous solution for multithreading, multiprocessing, and internet communications through categorized services (_Local_, _Public_, and _Internet_). These services are software components with predefined interfaces, which methods are invoked remotely.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-services.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-services.png" alt="AREG SDK distributed services" style="width:70%;height:70%"/></a></div>

> 💡 Currently AREG engine supports _Local_ (multithreading) and _Public_ (multiprocessing) service categories.

AREG forms a fault-tolerant system that automatically discovers and automates communications between services, allowing developers to focus on application logic development. The system ensures:
* The crash of one application does not affect the entire system.
* The clients automatically receive service availability notifications.
* The requests, responses, and notifications are invoked in their own thread context.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Composition[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#composition)

At present, the Automated Real-time Event Grid software development kit (**AREG SDK**) comprises the following primary modules:
- [Multicast router (_mcrouter_)](https://github.com/aregtech/areg-sdk/tree/master/framework/mcrouter/): a console application or OS-managed service that facilitates the message routing.
- The [AREG engine](https://github.com/aregtech/areg-sdk/tree/master/framework/areg/): a framework library that enables communication between software components.
- [Code generator](https://github.com/aregtech/areg-sdk/tree/master/tools/): a tool to generate service _Provider_ and _Consumer_ objects from interface documents.

Furthermore, the AREG SDK includes a variety of [examples](https://github.com/aregtech/areg-sdk/tree/master/examples/), which illustrate the robust characteristics and functionality of the AREG communication engine. Additionally, there are various [user interface (UI) tools](https://github.com/aregtech/areg-sdk-tools/) currently in the development stage that aim to simplify and expedite the process of designing services and applications, as well as analyzing and interacting with applications on remote machines.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Clone sources[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#clone-sources)

To obtain the AREG SDK source codes and the dependent modules, run the following command in your `projects` folder:
```bash
git clone --recurse-submodules https://github.com/aregtech/areg-sdk.git
```

Alternatively, to clone only the submodule sources, run the following command in the AREG SDK root folder:
```bash
git submodule update --init --recursive
```

To update the AREG SDK to the latest submodule sources, use the following command:
```bash
git submodule update --remote --recursive
```

> 💡 Please note that after running this Git command, your submodule sources may differ from the sources that AREG SDK uses.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Software build[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#software-build)

The source code of the AREG SDK is implemented using **C++17** and can be compiled on multiple platforms, CPUs, and compilers.
- **Supported compilers:** GCC, Clang, Cygwin GCC, and MSVC. 
- **Supported platforms:** Windows, Linux, and macOS. 
- **Supported CPU:** x86, x86_64, arm, and aarch64. 

For comprehensive information and detailed instructions, please visit the regularly updated [AREG SDK Wiki pages](https://github.com/aregtech/areg-sdk/wiki).

To compile the sources, the following tools can be used:

| Tool | Solution | Platforms | API | Quick actions to compile |
| --- | --- | --- | --- | --- |
| `cmake` | `CMakeLists.txt` | Linux, Cygwin, Windows | POSIX, Win32 | - Build with *cmake*.<br/> - Build in *VSCode*.<br/> - Build in *MSVS*. |
| `make` | `Makefile` | Linux, Cygwin | POSIX | - Build with *make*. |
| `msbuild` | `areg-sdk.sln` | Windows | Win32 | - Build with *msbuild*.<br/> - Open and build in *MSVS*. |

After compilation, the binaries are stored in the `<areg-sdk>/product/build/<family>-<compiler>/<platform>-<bitness>-<cpu>-<build type>/bin` folder. For example, if the code is compiled with GNU GCC on a 64-bit Linux system with an x86_64 CPU and in release mode, the binaries will be in the `./product/build/gnu-gcc/linux-64-x86_64-release/bin` subfolder of `areg-sdk`.

To obtain comprehensive information regarding **builds and options** for the AREG SDK, we recommend referring to the [Software build](https://github.com/aregtech/areg-sdk/wiki/02.-Software-build) page on the AREG SDK Wiki. In this document, we will provide a brief outline of the compilation process using default options.


> 💡 Note that other POSIX-compliant operating systems and compilers have not yet been tested. Here is the list of supported [POSIX APIs](https://github.com/aregtech/areg-sdk/blob/master/docs/POSIX.md#posix-api).

### Build with `cmake`

<details open><summary title="Click to show/hide details">Click to show / hide <code>Build with cmake</code>.</summary><br/>

To build the AREG and examples using [CMake](https://cmake.org/download/), ensure that you have cloned the sources properly as [described earlier](#clone-sources). Then, open the _Terminal_ in the `areg-sdk` directory and follow these steps:

1. Initialize the cache and build configuration in the `./build` folder with default options, which include using the *g++* compiler, performing a _release_ build, and enabling examples and unit tests:
```bash
cmake -B ./build
```

2. Compile the sources using the following command (justify parallel jobs with `-j` option):
```bash
cmake --build ./build -j 8
```

> 💡 For instructions on how to customize builds with `cmake` tool, kindly refer to the [Build with CMake](https://github.com/aregtech/areg-sdk/wiki/02.-Software-build#build-with-cmake-cmakeliststxt) Wiki page.
 </details>

### Build with `make`

<details open><summary title="Click to show/hide details">Click to show / hide <code>Build with cmake</code>.</summary><br/>

To build the AREG and examples using [Make](https://www.gnu.org/software/make/), ensure that you have cloned the sources properly as [described earlier](#clone-sources). Then, open the _Terminal_ in the `areg-sdk` directory and compile sources with _g++_ compiler, _release_ build, enabled examples and unit tests by following this command (justify parallel jobs with `-j` option):

```bash
make -j 8
```

> 💡 For instructions on how to customize builds with `make` tool, kindly refer to the [Build with Make](https://github.com/aregtech/areg-sdk/wiki/02.-Software-build#build-with-make-makefile) Wiki page.
</details>

### Build with `msbuild`

<details open><summary title="Click to show/hide details">Click to show / hide <code>Build with msbuild</code>.</summary><br/>

To build the AREG and examples using [MSBuild](https://visualstudio.microsoft.com/downloads/), ensure that you have cloned the sources properly as [described earlier](#clone-sources). Then, open the _Terminal_ in the `areg-sdk` directory and compile sources with _MSVC_ compiler, _release_ build, enabled examples and unit tests by following this step:

```bash
MSBuild .
```

> 💡 For instructions on how to customize builds with `make` tool, kindly refer to the [Build with MSBuild](https://github.com/aregtech/areg-sdk/wiki/02.-Software-build#build-with-msbuild-areg-sdksln) Wiki page.
</details>

### Build with WSL

<details open><summary title="Click to show/hide details">Click to show / hide <code>Build with WSL</code>.</summary><br/>

The *Windows Subsystem for Linux* (**WSL**) allows developers to use Linux applications directly on Windows machines. This means that developers may clone, compile and use AREG engine in a popular Linux distributions. To install and update WSL on your Windows 10 machine, clone and compile AREG SDK sources, and run examples, please follow the step-by-step instructions outlined in the [Compile in Windows Subsystem for Linux (WSL)](https://github.com/aregtech/areg-sdk/wiki/04.-Compile-with-Windows-Subsystem-for-Linux-(WSL)) Wiki page.

</details>

### Build with IDEs

<details open><summary title="Click to show/hide details">Click to show / hide <code>Build with IDE</code>.</summary><br/>

> 💡 In this chapter, we explain how to build the AREG and examples with *Microsoft Visual Studio* and *Visual Studio Code*. The other IDEs currently are not in our focus.

To build the AREG and examples using [Microsoft Visual Studio](https://visualstudio.microsoft.com/downloads/) or [Visual Studio Code](https://code.visualstudio.com/), ensure that you have cloned the sources properly as [described earlier](#clone-sources) and follow the instructions bellow.

#### Build with Microsoft Visual Studio

1. Open [`areg-sdk.sln`](https://github.com/aregtech/areg-sdk/blob/master/areg-sdk.sln) in *Microsoft Visual Studio*.
2. Build the solution with *MSVC*.

#### Build with Visual Studio Code

1. Open the `areg-sdk` folder in *Visual Studio Code*.
2. In the Explorer panel, select [`CMakeLists.txt`](https://github.com/aregtech/areg-sdk/blob/master/CMakeLists.txt).
3. Right-click on the file, select **Configure All Projects** from the context menu and wait for completion.
4. Right-click on the file again and select **Build All Projects** to build the sources with the default options.

</details>

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Integration[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#integration)

The AREG framework offers developers the flexibility to either build their own project on top of it or integrate the framework into an existing project in order to develop separate software layers or components.

### Start a project

<details open><summary title="Click to show/hide details">Click to show / hide <code>Start a project</code>.</summary><br/>

The best practice to start a project is to review [sample projects](https://github.com/aregtech/areg-sdk/tree/master/examples) provided in this repository. To begin a project, follow these general steps:
1. [Clone](#clone-sources) the AREG SDK sources to your workspace.
2. Identify the specific services that your components should provide or use.
3. Create a _Service Interface_ XML document that describes your service (data types, attributes, methods).
4. Use the provided [code generator](https://github.com/aregtech/areg-sdk/tree/master/tools) tool to generate base sources for service components.
5. Configure your project to create a _static_ library from the generated code.
6. Create a separate project for each application or layer and link with the _static_ and `areg` libraries.
7. Create components that provide the service by extending Stub objects and implementing override methods.
8. Create components that consume the service by extending Client objects and implementing override methods.
9. Distribute components in projects, compile and run them. Add logs in sources as needed.
  
For guidance and step-by-step examples, refer to the [_Hello Service!_](https://github.com/aregtech/areg-sdk/blob/master/docs/HelloService.md) document, which showcases various scenarios for creating _Local_ and _Public_ service providers and consumers. In addition, review the file structure guidelines presented in the [DEVELOP.md](https://github.com/aregtech/areg-sdk/blob/master/docs/DEVELOP.md) document.

> 💡 The Service Interface design tool with a GUI is currently [under development](https://github.com/aregtech/areg-sdk-tools) as an open source project, and will be added to the repository when it's ready.
</details>

### Configure multicast router

<details open><summary title="Click to show/hide details">Click to show / hide <code>Configure multicast router</code>.</summary><br/>

To configure the _Multicast Router_, adjust the [_router.init_](https://github.com/aregtech/areg-sdk/blob/master/framework/areg/resources/router.init) file by specifying the IP address and port of the `mcrouter`:
```
connection.address.tcpip    = 127.0.0.1	# IP address of the mcrouter host
connection.port.tcpip       = 8181      # connection port of the mcrouter
```

Note that the *Multicast Router* is necessary only for applications that provide or consume _Public_ services (multiprocessing applications) and can be ignored for application using only _Local_ services (multithreading applications). The Multicast Router can run on any machine with GPOS (General Purpose Operating System). It establishes a network between connected applications and responsible to route messages between programmable service components.
</details>

### Configure logging

<details open><summary title="Click to show/hide details">Click to show / hide <code>Configure logging</code>.</summary><br/>

Applications based on the AREG framework can be compiled with or without logging. If you choose to compile with logging, you'll need to configure [_log.init_](https://github.com/aregtech/areg-sdk/blob/master/framework/areg/resources/log.init) to set scopes, priorities, and file names for logging. 

Here's an example configuration for the log file:
```
log.file        = %home%/logs/%appname%_%time%.log # creates logs in the 'log' subfolder of the user's home directory
scope.mcrouter.*= NOTSET ;                         # disables logs for mcrouter.

scope.my_app.*                   = DEBUG | SCOPE ; # enables all logs of my_app
scope.my_app.ignore_this_scope   = NOTSET ;        # disables logs for a certain scope in my_app
scope.my_app.ignore_this_group_* = NOTSET ;        # disables logs for a certain scope group in my_app
```

Please refer to the [AREG Engine Logging System](https://github.com/aregtech/areg-sdk/wiki/05.-AREG-Engine-Logging-System) Wiki page for detailed instructions on compiling applications with enabled or disabled logging, start and stopping logs during runtime, as well as creating and using logging scopes, and configuring in the `log.init` file. 

> 💡 By default, the `router.init` and `log.init` files are located in the `config` subfolder of binaries.<br/>
> 💡 To enable logging for all applications, use `scope.*  = DEBUG | SCOPE ;`.<br/>
> 💡 At present, logging is only possible in the file.

</details>
<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Roadmap[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#roadmap)

The goal of the AREG SDK is to provide a lightweight, self-sufficient development and testing system to help developers create complex applications for **Desktop**, **Embedded**, and **IoT edge** devices in a shorter amount of time (_30-50% faster_). 

Here are the planned features of the framework:
- Multi-channel and multi-protocol communication.
- Logging service to collect logs in the network.
- _Internet_ (web) category services.

And here are the planned tools:
- Service interface designer.
- Interactive log viewer.
- Service testing and data simulation tool.

The development of the AREG SDK tools is currently underway in a separate repository called [AREG SDK Tools](https://github.com/aregtech/areg-sdk-tools), which is a part of the larger [AREG SDK project](https://github.com/users/aregtech/projects/4/).

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Use cases and benefits[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#use-cases-and-benefits)

The AREG SDK is a dynamic and flexible tool that offers a vast array of use cases for multithreading and multiprocessing application development. The communication engine has been successfully used in a variety of scenarios, from practical examples to real-world applications. Its proven capabilities and potential for success make it a popular choice for developers seeking to create intelligent devices for embedded and IoT applications. 

To explore the various use cases of the AREG communication engine, including **distributed solutions** for embedded applications, **driverless devices**, **real-time solutions**, **digital twins**, and **simulation and tests**, refer to the [AREG Engine Use Cases and Benefits](https://github.com/aregtech/areg-sdk/blob/master/docs/USECASES.md) document.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Examples[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#examples)

There are various [examples](https://github.com/aregtech/areg-sdk/tree/master/examples/) to demonstrate features of the AREG communication engine. The examples are part of SDK and are listed in the [examples/README.md](https://github.com/aregtech/areg-sdk/blob/master/examples/README.md) document.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## License[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#license)
 
AREG SDK is under free open source [_Apache License Version 2.0_](https://github.com/aregtech/areg-sdk/blob/master/LICENSE.txt). However, AREG SDK can be commercially licensed, which includes the commercial support, full rights to create and distribute software without open source license obligations. For commercial license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) website or contact _info[at]aregtech[dot]com_.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Call to action[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#call-to-action)

Join our project and provide assistance by:
* Checking out the list of [open issues](https://github.com/aregtech/areg-sdk/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) where we need help.
* If you need new features, please open a [new issue](https://github.com/aregtech/areg-sdk/issues) or start a [discussion](https://github.com/aregtech/areg-sdk/discussions).
* hen creating a pull request, kindly consider the time it takes for reviewing and testing, and maintain proper coding style.
* If you require invoiced commercial support or training, or if you wish to support AREG SDK commercially, kindly contact us at info[at]aregtech[dot]com. 

Please [![Star AREG SDK](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=Star%20AREG%20SDK)](https://github.com/aregtech/areg-sdk/) repository to encourage contributors and helps to grow our community. Contact us for the feedback or new ideas.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## Thank you all!![![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#thank-you)

Special thanks to all contributors and supporters that starred this repository.

**Our amazing contributors**:

[![Contributors for @aregtech/areg-sdk](https://github.com/aregtech/areg-sdk/raw/master/contributors.svg)](https://github.com/aregtech/areg-sdk/graphs/contributors)

**Our supportive stargazers**:

[![Stargazers for @aregtech/areg-sdk](https://reporoster.com/stars/aregtech/areg-sdk)](https://github.com/aregtech/areg-sdk/stargazers)

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

**Follow us** at<br />
[![Follow us on twitter](https://img.shields.io/twitter/follow/aregtech.svg?style=social)](https://twitter.com/intent/follow?screen_name=aregtech) 
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Aregtech-blue?style=flat&logo=linkedin&logoColor=b0c0c0&labelColor=363D44)](https://www.linkedin.com/company/aregtech)
[![Join the chat at https://gitter.im/areg-sdk/community](https://badges.gitter.im/areg-sdk/community.svg)](https://gitter.im/areg-sdk/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

---

**Share** the project link with your network on social media.

<a href="https://www.reddit.com/submit?url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk&title=Awesome%20communcation%20engine!" target="_blank"><img src="https://img.shields.io/twitter/url?label=Reddit&logo=Reddit&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on Reddit"/></a>&nbsp;
<a href="https://www.linkedin.com/shareArticle?mini=true&url=https%3A//github.com/aregtech/areg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=LinkedIn&logo=LinkedIn&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on LinkedIn"/></a>&nbsp;
<a href="https://twitter.com/intent/tweet?text=%23AREG%20%23realtime%20communication%20engine%20for%20%23embedded%20and%20%23IoT%0A%0Ahttps%3A//github.com/aregtech/areg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=Twitter&logo=Twitter&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Shared on Twitter"/></a>&nbsp;
<a href="https://www.facebook.com/sharer/sharer.php?u=https%3A//github.com/aregtech/areg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=Facebook&logo=Facebook&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on Facebook"/></a>&nbsp;
<a href="https://t.me/share/url?text=Awesome%20communication%20engine!&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=Telegram&logo=Telegram&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on Telegram"/></a>&nbsp;
<a href="https://wa.me/?text=Awesome%20communication%20engine!%5Cn%20https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=Whatsapp&logo=Whatsapp&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on Whatsapp"/></a>&nbsp;
<a href="mailto:?subject=Awesome%20communication%20engine&body=Checkout%20this%20awesome%20communication%20engine%3A%0Ahttps%3A//github.com/aregtech/areg-sdk%0A" target="_blank"><img src="https://img.shields.io/twitter/url?label=GMail&logo=GMail&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk"/></a>

---

![Viewers](https://gpvc.arturio.dev/aregtech)

<!-- markdownlint-enable -->
