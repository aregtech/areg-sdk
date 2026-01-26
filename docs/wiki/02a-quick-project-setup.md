# Quick Project Setup with Areg SDK

Create ready-to-build Areg SDK projects in 30 seconds using the interactive project setup tool.

> [!TIP]
> This is the fastest way to start with Areg SDK. For manual integration methods, see [CMake Integration Guide](./02b-cmake-integrate.md).

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Tool Overview](#tool-overview)
3. [Interactive Setup](#interactive-setup)
4. [Project Modes](#project-modes)
5. [Generated Project Structure](#generated-project-structure)
6. [Building and Running](#building-and-running)
7. [Customizing Your Project](#customizing-your-project)
8. [Troubleshooting](#troubleshooting)

---

## Quick Start

### 30-Second Project Creation

**Linux/macOS:**
```bash
cd areg-sdk
./areg-sdk/tools/project-setup.sh
# Answer 3 prompts (or press Enter for defaults)
cd areg_hello
cmake -B ./build
cmake --build ./build
./build/bin/areg_hello
```

**Windows:**
```powershell
.\areg-sdk\tools\project-setup.bat
# Answer 3 prompts (or press Enter for defaults)
cd areg_hello
cmake -B .\build
cmake --build .\build
.\build\bin\areg_hello.exe
```

**Expected output:**
```
Specify the name of your new project [default: areg_hello]: 
Specify the root directory of your new project [default: ./areg_hello]: 
Choose mode - (1) 'multiprocessing' or (2) 'multithreading' [default: 1]: 2

AREG 2 project created at: ./areg_hello
Build instructions:
  cd ./areg_hello
  cmake -B ./build
  cmake --build ./build
```

**Running the application:**
```
'Hello Service!'
Received response, end application
```

**Setup time:** ~30 seconds

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Tool Overview

### What It Does

The project setup tool creates a complete, ready-to-build Areg SDK project with:

- ✅ Configured CMakeLists.txt (automatically fetches Areg SDK)
- ✅ Example service interface (HelloService.siml)
- ✅ Provider implementation (service implementation)
- ✅ Consumer implementation (service client)
- ✅ Complete source files with working ORPC example
- ✅ Build scripts

### What It Generates

**All projects include:**
- Top-level `CMakeLists.txt` with Areg SDK integration
- `src/services/HelloService.siml` - Service Interface document
- `src/CMakeLists.txt` - Source build configuration
- Source files - Provider and/or consumer implementation

**Mode-specific files:**
- **Multithreading:** `src/main.cpp` (single executable)
- **Multiprocessing:** `src/provider.cpp` + `src/consumer.cpp` (two executables)

### When to Use It

**Use project-setup when:**
- ✅ Starting a new Areg SDK project from scratch
- ✅ Learning Areg SDK (provides working example)
- ✅ Prototyping quickly
- ✅ Need a reference project structure

**Don't use when:**
- ❌ Integrating into existing project (use [CMake Integration](./02b-cmake-integrate.md))
- ❌ Need custom project structure
- ❌ Building from Visual Studio (use [MSVC Integration](./02c-msvc-integrate.md))

### Prerequisites

- **Git** - To clone Areg SDK repository
- **CMake 3.20+** - Build system
- **C++17 Compiler** - GCC, Clang, or MSVC
- **Java 17+** - For code generation (automatic during build)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Interactive Setup

The tool asks three questions with sensible defaults.

### Prompt 1: Project Name

```
Specify the name of your new project [default: areg_hello]:
```

**Options:**
- Press **Enter** - Use default name (`areg_hello`)
- Type name - Use custom name (e.g., `my_service_app`)

**Example:**
```
Specify the name of your new project [default: areg_hello]: my_messenger
```

**Used for:**
- Directory name (if root not specified)
- CMake project name
- Executable name(s)

### Prompt 2: Root Directory

```
Specify the root directory of your new project [default: ./areg_hello]:
```

**Options:**
- Press **Enter** - Use `./[project_name]`
- Type path - Use custom location (e.g., `~/projects/my_app`)

**Example:**
```
Specify the root directory of your new project [default: ./my_messenger]: ~/dev/my_messenger
```

**Notes:**
- Directory created if it doesn't exist
- Relative or absolute paths supported
- Parent directory must exist

### Prompt 3: Project Mode

```
Choose mode - (1) 'multiprocessing' or (2) 'multithreading' [default: 1]:
```

**Options:**
- **1** or **Enter** - Multiprocessing (two executables)
- **2** - Multithreading (single executable)

**Example:**
```
Choose mode - (1) 'multiprocessing' or (2) 'multithreading' [default: 1]: 2
```

**See:** [Project Modes](#project-modes) for detailed comparison.

### Complete Example Session

```bash
$ ./areg-sdk/tools/project-setup.sh

Specify the name of your new project [default: areg_hello]: chat_service
Specify the root directory of your new project [default: ./chat_service]: 
Choose mode - (1) 'multiprocessing' or (2) 'multithreading' [default: 1]: 2

AREG 2 project created at: ./chat_service
Build instructions:
  cd chat_service
  cmake -B ./build
  cmake --build ./build
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Project Modes

Choose between multithreading and multiprocessing based on your architecture.

### Mode Comparison

| Aspect | Multithreading (Mode 2) | Multiprocessing (Mode 1) |
|--------|-------------------------|--------------------------|
| **Executables** | 1 (single process) | 2 (provider + consumer) |
| **Communication** | In-process (fast) | Inter-process via `mtrouter` |
| **Threads** | Multiple threads | Each process can be multi-threaded |
| **Deployment** | Single binary | Distributed binaries |
| **Use Cases** | Desktop apps, embedded | Microservices, distributed systems |
| **Complexity** | Lower | Higher |
| **Requires mtrouter** | No | Yes (for IPC) |

---

### Mode 1: Multiprocessing

**Architecture:** Two separate executables communicating via Areg Message Router.

```
┌─────────────┐         ┌──────────┐         ┌─────────────┐
│  Provider   │────────►│ mtrouter │◄────────│  Consumer   │
│  Process    │         │ (Router) │         │  Process    │
└─────────────┘         └──────────┘         └─────────────┘
```

**Generated executables:**
- `[project_name]_provider` - Service implementation
- `[project_name]_consumer` - Service client

**Generated files:**
- `src/provider.cpp` - ServiceProvider class
- `src/consumer.cpp` - ServiceConsumer class
- `src/CMakeLists.txt` - Builds both executables

**When to use:**
- Distributed systems
- Microservices architecture
- Services on different machines
- Independent deployment of components
- Scalable architectures

**Example use cases:**
- IoT gateway with remote sensors
- Client-server applications
- Multi-node systems
- Service mesh architectures

**Running:**
```bash
# Terminal 1 - Start provider
./build/my_app_provider

# Terminal 2 - Start consumer
./build/my_app_consumer
```

> [!IMPORTANT]
> For inter-process communication (IPC), `mtrouter` must be running. Start `mtrouter` before or after start provider and consumer processes. There is no starting order of processes. For network communication, configure `mtrouter` address in application configuration file (`areg.init`). By default, both processes communicate via `localhost`.

---

### Mode 2: Multithreading

**Architecture:** Single executable with provider and consumer in different threads.

```
┌────────────────────────────┐
│     Single Process         │
│  ┌──────────┐ ┌──────────┐ │
│  │ Provider │ │ Consumer │ │
│  │ Thread   │ │ Thread   │ │
│  └──────────┘ └──────────┘ │
└────────────────────────────┘
```

**Generated executable:**
- `[project_name]` - Single application

**Generated files:**
- `src/main.cpp` - ServiceProvider and ServiceConsumer classes
- `src/CMakeLists.txt` - Builds single executable

**When to use:**
- Desktop applications
- Embedded systems
- Single-machine deployment
- Learning Areg SDK
- Quick prototyping

**Example use cases:**
- Desktop GUI with background services
- Embedded device firmware
- Test applications
- Demo projects

**Running:**
```bash
./build/my_app
```

> [!NOTE]
> No `mtrouter` needed - all communication is in-process.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Generated Project Structure

### Directory Layout

```
areg_hello/                          # Project root
├── CMakeLists.txt                   # Top-level build configuration
└── src/
    ├── CMakeLists.txt               # Source build configuration
    ├── services/
    │   └── HelloService.siml        # Service interface definition
    ├── main.cpp                     # (Mode 2) Single executable
    ├── provider.cpp                 # (Mode 1) Provider executable
    └── consumer.cpp                 # (Mode 1) Consumer executable
```

---

### Top-Level CMakeLists.txt

**Purpose:** Project configuration and Areg SDK integration

**Key features:**
1. **Package detection** - Tries to find installed Areg SDK
2. **Automatic fetch** - Downloads from GitHub if not found
3. **Configuration** - Disables examples/tests for faster builds
4. **Integration** - Includes Areg CMake utilities

**Generated content:**

```cmake
cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME    "areg_hello")
set(PROJECT_VERSION "1.0.0")
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

# Try to find Areg SDK as installed package
find_package(areg CONFIG)

if (NOT areg_FOUND)
    # Areg SDK not found, fetch from GitHub
    
    # Configuration for faster builds
    set(AREG_BUILD_ROOT "${CMAKE_BINARY_DIR}")
    set(AREG_PACKAGES   "${CMAKE_BINARY_DIR}/packages")
    set(AREG_BINARY    shared)
    
    # Disable examples and tests
    option(AREG_BUILD_TESTS    "Build areg-sdk tests"    OFF)
    option(AREG_BUILD_EXAMPLES "Build areg-sdk examples" OFF)
    option(AREG_GTEST_PACKAGE  "Build GTest"             OFF)
    option(AREG_ENABLE_OUTPUTS "AREG build structure"    OFF)
    
    # Fetch Areg SDK from GitHub
    include(FetchContent)
    set(FETCHCONTENT_BASE_DIR "${AREG_PACKAGES}")
    
    FetchContent_Declare(
        areg
        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
        GIT_TAG "master"
    )
    FetchContent_MakeAvailable(areg)
    
    set(AREG_SDK_ROOT        "${areg_SOURCE_DIR}")
    set(AREG_CMAKE_CONFIG_DIR "${AREG_SDK_ROOT}/conf/cmake")
    set(AREG_CMAKE          "${AREG_SDK_ROOT}/areg.cmake")
else()
    # Areg SDK package found (via vcpkg or system install)
    message(STATUS ">>> Found AREG package at '${areg_DIR}'")
endif()

# Include Areg CMake utilities
include(${AREG_CMAKE})

# Build sources
add_subdirectory(src)
```

**Smart integration:**
- Works with vcpkg-installed Areg SDK
- Falls back to fetching from GitHub
- Optimized for quick builds

---

### HelloService.siml

**Purpose:** Service interface definition in Service Interface Markup Language  
**Location:** `src/services/HelloService.siml`  
**Generated content:**

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<ServiceInterface FormatVersion="1.1.0">
    <Overview ID="1" Name="HelloService" Version="1.0.0" Category="Public">
        <Description>The hello service minimal RPC application with request and response</Description>
    </Overview>
    <MethodList>
        <Method ID="2" Name="HelloService" MethodType="Request" Response="HelloService">
           <Description>The request to output Hello Service!</Description>
        </Method>
        <Method ID="4" Name="HelloService" MethodType="Response">
           <Description>The response indicating success request has been executed.</Description>
        </Method>
    </MethodList>
</ServiceInterface>
```

**Key elements:**
- **Overview** - Service metadata (name, version, category)
- **Request Method** - Client calls this method
- **Response Method** - Server responds via this method

**Code generation:**
During build, `codegen.jar` processes this file to generate:
- `HelloServiceStub.hpp/cpp` - Server-side implementation base
- `HelloServiceProxy.hpp/cpp` - Client-side proxy
- `HelloServiceClientBase.hpp/cpp` - Client convenience base
- `NEHelloService.hpp` - Namespace and constants

> [!TIP]
> Learn more about service interfaces: [Code Generator Guide](./06b-code-generator.md)

---

### Source Files (Multithreading Mode)

**File:** `src/main.cpp`  
**Key components:**  
**1. ServiceProvider class:**
```cpp
class ServiceProvider : public Component, protected HelloServiceStub
{
    virtual void requestHelloService(void) override
    {
        std::cout << "'Hello Service!'" << std::endl;
        responseHelloService();  // Send response
    }
};
```

**2. ServiceConsumer class:**
```cpp
class ServiceConsumer : public Component, protected HelloServiceClientBase
{
    virtual bool serviceConnected(NEService::eServiceConnection status, ProxyBase& proxy) override
    {
        if (NEService::isServiceConnected(status))
            requestHelloService();  // Call service
        return true;
    }
    
    virtual void responseHelloService(void) override
    {
        std::cout << "Received response, end application" << std::endl;
        Application::signalAppQuit();  // Exit gracefully
    }
};
```

**3. Model registration:**
```cpp
BEGIN_MODEL("ServiceModel")
    BEGIN_REGISTER_THREAD("Thread1")
        BEGIN_REGISTER_COMPONENT("ServiceProvider", ServiceProvider)
            REGISTER_IMPLEMENT_SERVICE(NEHelloService::ServiceName, NEHelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceProvider")
    END_REGISTER_THREAD("Thread1")
    
    BEGIN_REGISTER_THREAD("Thread2")
        BEGIN_REGISTER_COMPONENT("ServiceConsumer", ServiceConsumer)
            REGISTER_DEPENDENCY("ServiceProvider")
        END_REGISTER_COMPONENT("ServiceConsumer")
    END_REGISTER_THREAD("Thread2")
END_MODEL("ServiceModel")
```

**4. Application entry point:**
```cpp
int main(void)
{
    Application::initApplication();
    Application::loadModel("ServiceModel");
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    Application::releaseApplication();
    return 0;
}
```

**Model explanation:**
- Two threads created (`Thread1`, `Thread2`)
- Provider implements service in Thread1
- Consumer depends on provider in Thread2
- Automatic service discovery and connection

---

### Source Files (Multiprocessing Mode)

**Provider:** `src/provider.cpp`  
**Key differences:**
- Separate executable
- Provider-only model
- Calls `Application::signalAppQuit()` after response

```cpp
virtual void requestHelloService(void) override
{
    std::cout << "'Hello Service!'" << std::endl;
    responseHelloService();
    Application::signalAppQuit();  // Exit after handling
}

BEGIN_MODEL("ProviderModel")
    BEGIN_REGISTER_THREAD("Thread1")
        BEGIN_REGISTER_COMPONENT("ServiceProvider", ServiceProvider)
            REGISTER_IMPLEMENT_SERVICE(NEHelloService::ServiceName, NEHelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceProvider")
    END_REGISTER_THREAD("Thread1")
END_MODEL("ProviderModel")
```

**Consumer:** `src/consumer.cpp`  
**Key differences:**
- Separate executable
- Consumer-only model
- Declares dependency on "ServiceProvider" role

```cpp
BEGIN_MODEL("ConsumerModel")
    BEGIN_REGISTER_THREAD("Thread1")
        BEGIN_REGISTER_COMPONENT("ServiceConsumer", ServiceConsumer)
            REGISTER_DEPENDENCY("ServiceProvider")
        END_REGISTER_COMPONENT("ServiceConsumer")
    END_REGISTER_THREAD("Thread1")
END_MODEL("ConsumerModel")
```

---

### Source CMakeLists.txt

**Multithreading mode:**
```cmake
addServiceInterface(gen_areg_hello services/HelloService.siml)
macro_declare_executable(areg_hello gen_areg_hello main.cpp)
```

**Multiprocessing mode:**
```cmake
addServiceInterface(gen_areg_hello src/services/HelloService.siml)
macro_declare_executable(areg_hello_provider gen_areg_hello provider.cpp)
macro_declare_executable(areg_hello_consumer gen_areg_hello consumer.cpp)
```

**Functions used:**
- `addServiceInterface` - Generates code from `.siml` file
- `macro_declare_executable` - Creates executable with automatic Areg linkage

> [!NOTE]
> These are Areg SDK CMake helper functions. See [CMake Functions Reference](./02e-cmake-functions.md).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Building and Running

### Building Multithreading Project

**Step 1: Navigate to project**
```bash
cd areg_hello
```

**Step 2: Configure build**
```bash
cmake -B ./build
```

**Expected output (first time):**
```
-- The C compiler identification is GNU 11.4.0
-- The CXX compiler identification is GNU 11.4.0
-- >>> Fetched Areg SDK from GitHub to /path/to/build/packages
-- >>> Location of 'areg.cmake' /path/to/build/packages/areg-src/areg.cmake
-- Configuring done
-- Generating done
-- Build files written to: /path/to/areg_hello/build
```

**Time:** 10-60 seconds (first time), 1-2 seconds (subsequent)

**Step 3: Build**
```bash
cmake --build ./build -j20
```

**Expected output:**
```
[  5%] Generating gen_areg_hello
[ 10%] Building CXX object src/CMakeFiles/areg_hello.dir/main.cpp.o
[ 95%] Linking CXX executable areg_hello
[100%] Built target areg_hello
```
**Time:** 3-8 minutes (first time with Areg SDK), 2-5 seconds (rebuild)

**Step 4: Run**
```bash
./build/bin/areg_hello
```

**Expected output:**
```
'Hello Service!'
Received response, end application
```

---

### Building Multiprocessing Project

**Build (same as multithreading):**
```bash
cd areg_hello
cmake -B ./build
cmake --build ./build -j20
```

**Executables created:**
- `./build/bin/areg_hello_provider`
- `./build/bin/areg_hello_consumer`

**Step 1: Start provider (Terminal 1)**
```bash
./build/bin/areg_hello_provider
```

**Step 2: Start consumer (Terminal 2)**
```bash
./build/bin/areg_hello_consumer
```

**Expected output:**

**Terminal 1 (provider):**
```
'Hello Service!'
(Process exits)
```

**Terminal 2 (consumer):**
```
'Good bye Service!'
(Process exits)
```

> [!IMPORTANT]
> For inter-process communication, both processes must be on the same machine or `mtrouter` must be configured for network communication.

---

### Message Router Setup

For **multiprocessing** projects, `mtrouter` is required for inter-process communication (IPC).  
**Starting mtrouter:**
```bash
# From Areg SDK or areg_hello build
./build/bin/mtrouter
```

Without `mtrouter`, multiprocessing applications will run as isolated multithreading applications - internal services work within each process, but public services cannot communicate across processes. For network communication across machines, configure mtrouter address in your application's configuration file. See [Areg SDK Multitarget Router (mtrouter)](./03a-mtrouter.md) for details.

> [!NOTE]
> Multithreading projects do not need `mtrouter` - all communication is in-process.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Customizing Your Project

### Modifying the Service Interface

**Add a new method to HelloService.siml:**

```xml
<Method ID="6" Name="Goodbye" MethodType="Request" Response="Goodbye">
    <Description>Say goodbye</Description>
</Method>
<Method ID="8" Name="Goodbye" MethodType="Response">
    <Description>Goodbye response</Description>
</Method>
```

> [!TIP]
> You can use [`Lusan`](https://github.com/aregtech/areg-sdk-tools) to edit `.siml` files.  
> See details in [Creating Service Interface Documents with Lusan](./06e-lusan-service-interface.md)

**Implement in provider:**

```cpp
virtual void requestHelloService(void) override
{
    std::cout << "\'Hello Service!\'" << std::endl;
    responseHelloService();
}

virtual void requestGoodbye(void) override
{
    std::cout << "\'Goodbye!\'" << std::endl;
    responseGoodbye();
}
```

**Call from consumer:**

```cpp
virtual bool serviceConnected(NEService::eServiceConnection status, ProxyBase& proxy) override
{
    if (NEService::isServiceConnected(status))
        requestHelloService();
    return true;
}

virtual void responseHelloService(void) override
{
        requestGoodbye();  // New method
}

virtual void responseGoodbye(void) override
{
    std::cout << "Received goodbye response" << std::endl;
    Application::signalAppQuit();
}
```

**Rebuild:**
```bash
cmake --build ./build
```

---

### Adding More Services

**Create new service interface:**

```bash
# Create ChatService.siml in src/services/
```

**Update src/CMakeLists.txt:**

```cmake
# Generate both services
addServiceInterface(gen_hello services/HelloService.siml)
addServiceInterface(gen_chat services/ChatService.siml)

# Link both to executable
macro_declare_executable(areg_hello gen_hello gen_chat main.cpp chat.cpp)
```

---

### Changing Project Structure

**Option 1: Multiple components**

Organize by component:
```
src/
├── hello/
│   ├── HelloProvider.hpp
│   ├── HelloProvider.cpp
│   └── HelloConsumer.cpp
├── chat/
│   ├── ChatProvider.hpp
│   ├── ChatProvider.cpp
│   └── ChatConsumer.cpp
└── main.cpp
```

**Option 2: Reusable library**

Create a static library for reusable components. One static library may contains objects of several Servce Interfaces:
```cmake
# Create service library
addServiceInterface(services_lib services/HelloService.siml)
addServiceInterface(services_lib services/ChatService.siml)

# Use in executable
macro_declare_executable(areg_hello services_lib \
    main.cpp                \
    hello/HelloProvider.cpp \
    hello/HelloConsumer.cpp \
    chat/ChatProvider.cpp   \
    chat/ChatConsumer.cpp
    )
```

---

### Configuration Options

**Modify top-level CMakeLists.txt:**

**Enable logging:**
```cmake
set(AREG_LOGS ON)
```

**Use Areg static library, if needed:**
```cmake
set(AREG_BINARY static)
```

**Complete options:** [CMake Configuration Guide](./02d-cmake-config.md)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### Script Permission Denied (Linux/macOS)

**Problem:** `bash: ./project-setup.sh: Permission denied`  
**Solution:**

```bash
chmod +x ./tools/project-setup.sh
./areg-sdk/tools/project-setup.sh
```

---

### Git Not Found

**Problem:** Script fails with "git: command not found"  
**Solution:**  

*Ubuntu/Debian:*
```bash
sudo apt-get install git
```

*macOS:*
```bash
brew install git
```

*Windows:*  
Download from [git-scm.com](https://git-scm.com/)

---

### CMake Version Too Old

**Problem:** `CMake 3.10 or higher is required.  You are running version 2.8.12`  
**Solution:**

*Ubuntu/Debian:*
```bash
sudo apt-get install cmake
# Or from snap for latest
sudo snap install cmake --classic
```

*macOS:*
```bash
brew install cmake
```

*Windows:*  
Download from [cmake.org](https://cmake.org/download/)

---

### Build Fails with "Java not found"

**Problem:** Code generation fails during build.  
**Solution:**  
Install Java 17+:

*Ubuntu/Debian:*
```bash
sudo apt-get install openjdk-17-jre
```

*macOS:*
```bash
brew install openjdk@17
```

*Windows:*  
Download from [Adoptium](https://adoptium.net/)

**Verify:**
```bash
java -version
```

---

### FetchContent Download Fails

**Problem:** Cannot download Areg SDK from GitHub.  
**Solution:**

*Check network:*
```bash
ping github.com
```

*Use local Areg SDK:*  
Edit generated `CMakeLists.txt`:
```cmake
# Instead of FetchContent
set(AREG_SDK_ROOT "/path/to/local/areg-sdk")
add_subdirectory(${AREG_SDK_ROOT} areg)
```

---

### Consumer Cannot Connect (Multiprocessing)

**Problem:** Consumer exits immediately without connecting.  
**Cause:** Provider not running or wrong configuration.  
**Solution:**

*1. Start provider first:*
```bash
./build/my_app_provider
```

*2. Then start consumer:*
```bash
./build/my_app_consumer
```

*3. Check both are running:*
```bash
ps aux | grep my_app
```

*4. Check consumer is connected - in `mtrouter` console type `-n` or `--instances`:*
```bash
Type '-q' or '--quit' to quit the application ...: --instances
---------------------------------------------------------------------------------------------
   Nr. |  Instance ID  |  Bitness  |  Name
---------------------------------------------------------------------------------------------
    1. |          256  |    64     |  areg_hello_consumer.elf
---------------------------------------------------------------------------------------------
Type '-q' or '--quit' to quit the application ...:
```

---

### Wrong Executable Built (Windows)

**Problem:** Looking for `areg_hello.exe` but only `areg_hello_provider.exe` exists. 
**Cause:** Built multiprocessing project but expected multithreading. 
**Solution:**

**Check mode in script output:**
```
Areg 1 project created...  # 1 = multiprocessing
Areg 2 project created...  # 2 = multithreading
```

**Re-run with correct mode:**
```powershell
.\areg-sdk\tools\project-setup.bat
# Choose mode 2 for single executable
```

---

### Compile Error: "Cannot find HelloServiceStub.hpp"

**Problem:** Build fails with missing generated headers.  
**Cause:** Code generation step failed.  
**Solution:**

**1. Check Java installation:**
```bash
java -version  # Should be 17+
```

**2. Clean and rebuild:**
```bash
rm -rf build
cmake -B ./build
cmake --build ./build -j20
```

**3. Check build output for codegen errors:**
Look for:
```
Generating gen_areg_hello
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Next Steps

### Learn More About Integration

**Manual integration methods:**
- [CMake Integration](./02b-cmake-integrate.md) - FetchContent, vcpkg, submodule
- [Visual Studio Integration](./02c-msvc-integrate.md) - MSVC-specific setup

**Configuration:**
- [CMake Configuration Options](./02d-cmake-config.md) - All build options
- [CMake Functions Reference](./02e-cmake-functions.md) - Helper functions
- [Preprocessor Definitions](./02f-preprocessor-definitions.md) - Compile-time config

### Develop Your Service

**Service design:**
- [Code Generator Guide](./06b-code-generator.md) - Generate from `.siml` files
- [Service Interface Design](./06e-lusan-service-interface.md) - Visual design with Lusan

**Testing and debugging:**
- [Live Log Viewer](./06f-lusan-live-logging.md) - Real-time log monitoring
- [Offline Log Viewer](./06g-lusan-offline-logging.md) - Post-mortem analysis

### Explore Examples

**Areg SDK Examples:**
- [Examples Directory](../../examples/README.md) - Sample applications
- [Areg SDK Demo](https://github.com/aregtech/areg-sdk-demo) - Integration examples

**Help:**
For questions, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech
