<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.areg.tech"><img align="center" src="https://github.com/aregtech/areg-sdk/raw/master/docs/img/areg-sdk-1280x360px-logo.png" alt="Areg SDK Home" style="width:70%;height:70%"/></a>
  <br /><br /><strong>Areg SDK Documentation</strong>
</h1>

Welcome to the **Areg SDK Technical Documentation**. This wiki provides comprehensive guides, tutorials, and reference materials for building distributed, service-oriented C++ applications with the Areg Framework.

**Areg Framework** is an interface-centric C++ framework for building real-time, asynchronous, and distributed systems in embedded, desktop, cloud, and mist-computing environments.

---

## Quick Start

**Never used Areg SDK?** Start here:

1. **[Quick Project Setup](./02a-quick-project-setup.md)** ⭐ Create a working project in 30 seconds
2. **[Build Areg SDK](./01b-cmake-build.md)** - Build from source (5-10 minutes)
3. **[Install with vcpkg](./01a-areg-package.md)** - Use package manager (fastest)
4. **[Examples and Tests](./08a-examples-and-tests.md)** - Run sample applications
5. **[Performance Benchmarks](./08b-areg-sdk-performance-benchmarks.md)** - Measured latency, throughput, and data rate across platforms

**Already familiar?** Jump to: [Documentation by Topic](#documentation-by-topic) | [Search by Task](#quick-navigation-by-task) | [Search by Role](#quick-navigation-by-role)

---

## What is Areg SDK?

Areg SDK is a lightweight framework for building distributed micro-services in C++. Applications expose and consume services across process and network boundaries with automatic message routing, type-safe interfaces, and zero manual synchronization.

### Key Features

- ✅ **Service-Oriented Architecture**: Define services, implement providers, connect consumers
- ✅ **Automatic Message Routing**: Built-in IPC and network communication
- ✅ **Type-Safe Interfaces**: Compile-time verified service contracts
- ✅ **Zero Manual Threading**: Framework handles all synchronization
- ✅ **Cross-Platform**: Linux, Windows, macOS, embedded systems
- ✅ **Real-Time Logging**: Centralized monitoring and diagnostics

### Why Areg SDK?

**Traditional Approach**:
```cpp
// Manual threading, locks, networking code
std::mutex mtx;
void handleRequest() {
    std::lock_guard<std::mutex> lock(mtx);
    // Complex synchronization logic...
}
```

**With Areg SDK**:
```cpp
// Define interface, framework handles everything
virtual void request_greeting(const String& name) override {
    response_greeting("Hello, " + name + "!");
}
```

No locks. No threads. No networking code. Just business logic.

---

## Documentation by Topic

### 📦 Installation and Build

Get Areg SDK running on your system:

| Guide | Platform | Method | Time |
|-------|----------|--------|------|
| **[Quick Project Setup](./02a-quick-project-setup.md)** ⭐ | All | Interactive tool | 30 sec |
| **[vcpkg Installation](./01a-areg-package.md)** | All | Package manager | 5 min |
| **[CMake Build](./01b-cmake-build.md)** | Linux, Windows, macOS | Source build | 10 min |
| **[MSVC Build](./01c-msvc-build.md)** | Windows | Visual Studio | 5 min |
| **[WSL Build](./01d-wsl-build.md)** | Windows | Linux on Windows | 10 min |

---

### 🔧 Integration and Configuration

Integrate Areg SDK into your projects:

**CMake Projects:**
- **[CMake Integration Guide](./02b-cmake-integrate.md)** - FetchContent, find_package, submodules
- **[CMake Configuration Options](./02d-cmake-config.md)** - 22 configuration variables explained
- **[CMake Functions Reference](./02e-cmake-functions.md)** - Helper functions and macros

**Visual Studio Projects:**
- **[MSVC Integration Guide](./02c-msvc-integrate.md)** - Integrate with Visual Studio solutions

**Advanced Configuration:**
- **[Preprocessor Definitions](./02f-preprocessor-definitions.md)** - Compile-time feature control

---

### 🌐 Networking and Communication

Configure distributed communication:

- **[Message Router (mtrouter)](./03a-mtrouter.md)** - IPC backbone and network routing

---

### 📊 Logging and Monitoring

Centralized logging for distributed systems:

**Configuration:**
- **[Logging Configuration](./04a-logging-config.md)** - Setup scopes, priorities, outputs

**Development:**
- **[Logging Development Guide](./04b-logging-develop.md)** - Instrument applications with logging

**Runtime Tools:**
- **[Log Observer](./04c-logobserver.md)** - Console-based log monitoring
- **[Log Collector](./04d-logcollector.md)** - Centralized log aggregation service

---

### 💾 Persistence

Configuration and data persistence:

- **[Key-Value Persistence Syntax](./05a-persistence-syntax.md)** - File syntax, storage model, and the `ConfigManager` API for persisting data
- **[Areg Configuration Reference (`areg.init`)](./05b-areg-configuration-file.md)** - Complete reference of every `areg.init` section and property

---

### 🛠️ Development Tools

Tools that accelerate development:

**Core Tools:**
- **[Areg SDK Tools Overview](./06a-areg-sdk-tools.md)** - Complete toolchain guide
- **[Code Generator](./06b-code-generator.md)** - Generate service interfaces from `.siml` files

**Lusan - Visual Development Tool:**
- **[Building Lusan](./06c-build-lusan.md)** - Build the GUI tool
- **[Setup and Using Lusan](./06d-setup-lusan.md)** - Initial configuration
- **[Service Interface Design](./06e-lusan-service-interface.md)** - Visual interface editor
- **[Live Log Monitoring](./06f-lusan-live-logging.md)** - Real-time distributed logging
- **[Offline Log Analysis](./06g-lusan-offline-logging.md)** - Post-mortem debugging

---

### 🔍 Troubleshooting

Solutions for common issues:

**Platform-Specific:**
- **[WSL Troubleshooting](./07a-troubleshooting-wsl-update.md)** - Windows Subsystem for Linux
- **[CMake Linux Builds](./07b-troubleshooting-cmake-linux-builds.md)** - Linux build problems

**Integration:**
- **[Integration Troubleshooting](./07c-troubleshooting-integration.md)** - SDK integration issues

**Networking:**
- **[Low-Latency TCP Network Tuning](./07d-troubleshooting-network-tunning.md)** - Guide for High-Performance Networking

---

### 📚 Examples and Demos

Hands-on learning resources:

**Official Examples:**
- **[Building and Running Examples](./08a-examples-and-tests.md)** - 17+ example projects
- **[Examples Source Code](https://github.com/aregtech/areg-sdk/tree/master/examples)** - GitHub repository

**Benchmark Examples:**
- **[30_publatency](https://github.com/aregtech/areg-sdk/tree/master/examples/30_publatency)** – Latency benchmark: ping-pong RTT and broadcast OWT across all payload sizes. → [View measured results](./08b-areg-sdk-performance-benchmarks.md)
- **[23_pubdatarate](https://github.com/aregtech/areg-sdk/tree/master/examples/23_pubdatarate)** – Throughput benchmark: data rate and message rate at scale. → [View measured results](./08b-areg-sdk-performance-benchmarks.md)

**Demo Projects:**
- **[Areg SDK Demo](https://github.com/aregtech/areg-sdk-demo)** - Real-world project examples
- **[Edge AI Demo](https://github.com/aregtech/areg-edgeai)** - LLM service provider/consumer
- **[Lusan (Qt Integration)](https://github.com/aregtech/areg-sdk-tools)** - Professional GUI tool

---

### 📊 Performance and Benchmarks

Areg SDK is built on a performance mandate: **latency, throughput, stability, and security are primary design constraints – not secondary concerns.** Every IPC path is measured on production code – service discovery, routing, dispatch, and serialization all active. Nothing is stripped for the benchmark.

> **Run the benchmarks on your own hardware and share results.** Community measurements from diverse hardware help the project and may be published here. If you find a performance regression, open a GitHub issue – regressions are treated as bugs.

**Measured results:**
- **[areg-sdk Performance Benchmarks](./08b-areg-sdk-performance-benchmarks.md)** – Full latency and throughput data across Linux, Windows, and macOS. Includes methodology, timestamp placement, and reproduction instructions.
- **[areg-sdk vs ZMQ / NanoMsg / NNG](./08c-areg-vs-hitachi-benchmark.md)** – Direct TCP comparison against the Hitachi Energy Research benchmark (arXiv:2508.07934v1), using raw CSV data.
- **[areg-sdk Framework Rankings](./08d-areg-framework-rankings.md)** – Competitive position across latency, message rate, and data rate versus all known frameworks with cited sources.

> 📁 *Prefix note:* These files use the `08` prefix alongside examples, since benchmarks run via examples 23 and 30. If additional benchmark documents are added, a dedicated `09` prefix is recommended.

**Reproduce the results:**
- **[30_publatency](https://github.com/aregtech/areg-sdk/tree/master/examples/30_publatency)** – Run latency measurements on your machine
- **[23_pubdatarate](https://github.com/aregtech/areg-sdk/tree/master/examples/23_pubdatarate)** – Run throughput measurements on your machine

---

## Quick Navigation by Task

**I want to...**

### ...get started quickly
→ **[Quick Project Setup](./02a-quick-project-setup.md)** (30 seconds)  
→ **[Build Areg SDK](./01b-cmake-build.md)** (10 minutes)  
→ **[Run First Example](./08a-examples-and-tests.md)**

### ...install Areg SDK
→ **[vcpkg Installation](./01a-areg-package.md)** (package manager)  
→ **[CMake Build](./01b-cmake-build.md)** (from source)  
→ **[MSVC Build](./01c-msvc-build.md)** (Visual Studio)

### ...integrate Areg into my project
→ **[CMake Integration](./02b-cmake-integrate.md)** (CMake projects)  
→ **[MSVC Integration](./02c-msvc-integrate.md)** (Visual Studio projects)  
→ **[CMake Functions](./02e-cmake-functions.md)** (helper functions)

### ...design service interfaces
→ **[Service Interface Design](./06e-lusan-service-interface.md)** (visual editor)  
→ **[Code Generator](./06b-code-generator.md)** (automated code generation)

### ...monitor logs from distributed apps
→ **[Live Log Monitoring](./06f-lusan-live-logging.md)** (Lusan GUI)  
→ **[Log Observer](./04c-logobserver.md)** (command-line tool)

### ...debug a problem
→ **[Log Analysis](./06g-lusan-offline-logging.md)** (offline logs)  
→ **[Troubleshooting Guides](./07c-troubleshooting-integration.md)** (common issues)

### ...configure logging and services
→ **[Logging Configuration](./04a-logging-config.md)** (scopes and priorities)  
→ **[Areg Configuration Reference](./05b-areg-configuration-file.md)** (`areg.init` file)

### ...build for embedded systems
→ **[CMake Cross-Compilation](./01b-cmake-build.md#cross-compilation)** (ARM, etc.)  
→ **[CMake Troubleshooting](./07b-troubleshooting-cmake-linux-builds.md)** (build issues)

### ...increase message and data streaming
→ **[Low-Latency TCP Tuning](./07d-troubleshooting-network-tunning.md)** (GBytes data and 1M messages per second)  

### ...measure or validate performance
→ **[Performance Benchmarks](./08b-areg-sdk-performance-benchmarks.md)** – latency and throughput data with methodology  
→ **[vs ZMQ / NanoMsg / NNG](./08c-areg-vs-hitachi-benchmark.md)** – direct TCP comparison (Hitachi paper)  
→ **[Framework Rankings](./08d-areg-framework-rankings.md)** – competitive position table  
→ **[Run 30_publatency](https://github.com/aregtech/areg-sdk/tree/master/examples/30_publatency)** – reproduce latency measurements  
→ **[Run 23_pubdatarate](https://github.com/aregtech/areg-sdk/tree/master/examples/23_pubdatarate)** – reproduce throughput measurements  

---

## Quick Navigation by Role

### 👨‍💻 Application Developer

**Getting Started:**
1. **[Quick Project Setup](./02a-quick-project-setup.md)** - Create your first project
2. **[Examples](./08a-examples-and-tests.md)** - Learn from working code
3. **[Service Interface Design](./06e-lusan-service-interface.md)** - Create service contracts

**Development:**
- **[CMake Integration](./02b-cmake-integrate.md)** - Add Areg to existing projects
- **[Code Generator](./06b-code-generator.md)** - Generate service code
- **[Logging Development](./04b-logging-develop.md)** - Add logging to apps

**Debugging:**
- **[Live Logging](./06f-lusan-live-logging.md)** - Monitor running applications
- **[Offline Analysis](./06g-lusan-offline-logging.md)** - Debug production issues

---

### 🏗️ System Architect

**Architecture:**
- **[Message Router](./03a-mtrouter.md)** - IPC and network communication
- **[Service Interface Concepts](./06e-lusan-service-interface.md)** - Design patterns
- **[Persistence](./05a-persistence-syntax.md)** - Configuration management

**Integration:**
- **[CMake Configuration](./02d-cmake-config.md)** - Build system setup
- **[CMake Functions](./02e-cmake-functions.md)** - Advanced integration
- **[Preprocessor Definitions](./02f-preprocessor-definitions.md)** - Feature control

**Tools:**
- **[Areg SDK Tools](./06a-areg-sdk-tools.md)** - Complete toolchain overview
- **[Lusan](./06c-build-lusan.md)** - Visual development environment

---

### 🚀 DevOps Engineer

**Build and Deploy:**
- **[CMake Build](./01b-cmake-build.md)** - CI/CD integration
- **[MSVC Build](./01c-msvc-build.md)** - Windows automation
- **[CMake Configuration](./02d-cmake-config.md)** - Build customization

**Monitoring:**
- **[Log Collector](./04d-logcollector.md)** - Centralized logging service
- **[Log Observer](./04c-logobserver.md)** - Runtime log control
- **[Logging Configuration](./04a-logging-config.md)** - Production settings

**Troubleshooting:**
- **[WSL Issues](./07a-troubleshooting-wsl-update.md)** - Windows/Linux integration
- **[CMake Issues](./07b-troubleshooting-cmake-linux-builds.md)** - Build problems
- **[Integration Issues](./07c-troubleshooting-integration.md)** - SDK integration
- **[Low-Latency TCP Network Tuning](./07d-troubleshooting-network-tunning.md)** - Guide for High-Performance Networking

---

### 🧪 QA Engineer

**Testing:**
- **[Examples and Tests](./08a-examples-and-tests.md)** - Unit tests and examples
- **[Build from Source](./01b-cmake-build.md)** - Test environments

**Log Analysis:**
- **[Live Logging](./06f-lusan-live-logging.md)** - Real-time monitoring
- **[Offline Logging](./06g-lusan-offline-logging.md)** - Test result analysis
- **[Log Configuration](./04a-logging-config.md)** - Test logging setup

**Tools:**
- **[Log Observer](./04c-logobserver.md)** - Automated log inspection
- **[Lusan](./06d-setup-lusan.md)** - Visual test monitoring

---

## Architecture and Core Concepts

### Service-Oriented Architecture

Areg SDK implements a **Service Provider-Consumer model**:

- **Service Interface**: Contract defined in `.siml` files
- **Service Provider**: Implements the service (server)
- **Service Consumer**: Uses the service (client)
- **Message Router**: Handles automatic routing and synchronization

### Communication Patterns

**Local Services** (Multithreading):
- Communication between threads in the same process
- Zero-copy optimization
- Automatic synchronization

**IPC Services** (Multiprocessing):
- Communication between processes
- Automatic serialization/deserialization
- Network-transparent

**Remote Services** (Networking):
- Communication across network boundaries
- Same API as local/IPC services
- Automatic message routing

### Key Components

```
Your Application
    ↓ uses
Service Interface (.siml)
    ↓ generates (codegen.jar)
Service Provider/Consumer Code
    ↓ links
Areg Framework (libareg)
    ↓ communicates via
Message Router (mtrouter)
    ↓ monitored by
Log Collector (logcollector)
    ↓ visualized in
Lusan GUI
```

---

## Platform Support

| Platform | Status | Compilers | Architectures |
|----------|--------|-----------|---------------|
| **Linux** | ✅ Fully Supported | GCC, Clang | x86, x86_64, ARM, ARM64 |
| **Windows** | ✅ Fully Supported | MSVC, MinGW, Clang | x86, x86_64 |
| **macOS** | ✅ Fully Supported | Clang, GCC | x86_64, ARM64 |
| **WSL** | ✅ Fully Supported | GCC, Clang | x86_64, ARM64 |
| **Embedded Linux** | ✅ Supported | GCC (cross-compile) | ARM, ARM64, MIPS |

---

## Community and Support

### Get Help

- **GitHub Discussions**: [Ask questions](https://github.com/aregtech/areg-sdk/discussions)
- **GitHub Issues**: [Report bugs](https://github.com/aregtech/areg-sdk/issues)
- **Email**: info[at]areg.tech
- **Website**: [www.areg.tech](https://www.areg.tech)

### Contributing

Help improve the documentation:

**Found an issue?**
- Submit a pull request
- Open a discussion
- Create an issue

**Want to contribute?**
- Improve existing documentation
- Add missing examples
- Translate documentation
- Share your use cases

### Stay Updated

- **GitHub**: [github.com/aregtech/areg-sdk](https://github.com/aregtech/areg-sdk)
- **LinkedIn**: [Aregtech (Artak Avetyan)](https://www.linkedin.com/in/artak-avetyan/)
- **Releases**: [GitHub Releases](https://github.com/aregtech/areg-sdk/releases)

---

## Additional Resources

### Example Projects

- **[Areg SDK Examples](https://github.com/aregtech/areg-sdk/tree/master/examples)** - 17+ examples from basic to advanced
- **[Areg SDK Demo](https://github.com/aregtech/areg-sdk-demo)** - Real-world project templates
- **[Edge AI Demo](https://github.com/aregtech/areg-edgeai)** - LLM inference with Areg SDK

### External Tools

- **[Lusan (areg-sdk-tools)](https://github.com/aregtech/areg-sdk-tools)** - Professional GUI tool
- **[vcpkg Package](https://vcpkg.io/)** - Package manager support

### Documentation

- **[API Reference](https://aregtech.github.io/areg-sdk/)** - Doxygen-generated API docs
- **[Release Notes](https://github.com/aregtech/areg-sdk/releases)** - Version history
- **[License](https://github.com/aregtech/areg-sdk/blob/master/LICENSE.txt)** - Apache License 2.0

---

## Next Steps

### For First-Time Users

1. ✅ **[Quick Project Setup](./02a-quick-project-setup.md)** - Create working project (30 sec)
2. ✅ **[Run Examples](./08a-examples-and-tests.md)** - See Areg SDK in action (5 min)
3. ✅ **[Design Services](./06e-lusan-service-interface.md)** - Create your first service (10 min)
4. ✅ **[Build Edge AI Demo](https://github.com/aregtech/areg-edgeai)** - Real-world application (20 min)

### For Experienced Developers

1. ✅ **[CMake Integration](./02b-cmake-integrate.md)** - Integrate into existing projects
2. ✅ **[CMake Functions](./02e-cmake-functions.md)** - Advanced build configuration
3. ✅ **[Log Monitoring](./06f-lusan-live-logging.md)** - Setup production logging
4. ✅ **[Cross-Compilation](./01b-cmake-build.md#cross-compilation)** - Build for embedded systems

### For Team Leads

1. ✅ **[Architecture Overview](./06e-lusan-service-interface.md)** - Understand service model
2. ✅ **[Toolchain Overview](./06a-areg-sdk-tools.md)** - Complete development tools
3. ✅ **[CI/CD Setup](./01b-cmake-build.md)** - Automated builds
4. ✅ **[Troubleshooting](./07c-troubleshooting-integration.md)** - Common issues

### For Testing

1. ✅ **[Low-Latency TCP Network Tuning](./07d-troubleshooting-network-tunning.md)** - Guide for High-Performance Networking
2. ✅ **[Performance Benchmarks](./08b-areg-sdk-performance-benchmarks.md)** - Reference results and methodology
3. ✅ **[Run 30_publatency](https://github.com/aregtech/areg-sdk/tree/master/examples/30_publatency)** - Reproduce latency on your hardware
4. ✅ **[Run 23_pubdatarate](https://github.com/aregtech/areg-sdk/tree/master/examples/23_pubdatarate)** - Reproduce throughput on your hardware

---

**Ready to build distributed applications?**  
Start with **[Quick Project Setup](./02a-quick-project-setup.md)** or explore **[Examples](./08a-examples-and-tests.md)**!

---

Copyright © 2023–2026 Aregtech (Artak Avetyan)  
https://www.areg.tech  
Contact: info[at]areg.tech
