# Quick Project Setup with Areg SDK

Create a ready-to-build Areg SDK project with one command, then build and run it.

> [!TIP]
> This is the fastest way to start with Areg SDK. To add Areg to a project you already have, see the [CMake Integration Guide](./02b-cmake-integrate.md).

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [The Three Setup Tools](#the-three-setup-tools)
3. [Options](#options)
4. [Project Modes](#project-modes)
5. [Generated Project Structure](#generated-project-structure)
6. [Building and Running](#building-and-running)
7. [Building with an AI Coding Agent](#building-with-an-ai-coding-agent)
8. [Customizing Your Project](#customizing-your-project)
9. [Troubleshooting](#troubleshooting)

---

## Quick Start

Run the command from the directory that contains your `areg-sdk` clone.

**Linux / macOS:**
```bash
sh areg-sdk/tools/setup-project.sh --name areg_hello --mode local
cd areg_hello
cmake -B build
cmake --build build -j
# run areg_hello from build/bin/
```

**Windows:**
```powershell
areg-sdk\tools\setup-project.bat --name areg_hello --mode local
cd areg_hello
cmake -B build
cmake --build build -j
# run areg_hello from build\bin\
```

**With Python 3, on any platform:**
```bash
python3 areg-sdk/tools/agent/setup_project.py --name areg_hello --mode local
```

Run without options on a terminal and the tool asks for the project name, the mode and
the directory, with a default for each.

**Running the application prints:**
```
provider: hello, ServiceConsumer
consumer: greeted
```

Executable names carry a platform suffix: `.elf` on Linux, `.mac` on macOS, `.exe` on Windows.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## The Three Setup Tools

All three create the **same project**: the same sources, the same CMake files, the same
modes. Each copies a working recipe from `docs/agent/recipes/` and renames it after your
project, so a new project starts from code that the SDK's own checks build and run.

| Tool | Needs | Also writes |
|------|-------|-------------|
| `tools/setup-project.sh` | a POSIX shell (Linux, macOS, WSL, Git Bash) | -- |
| `tools/setup-project.bat` | Windows; runs `setup-project.ps1` beside it | -- |
| `tools/setup-project.ps1` | Windows PowerShell 5.1 or PowerShell 7 | -- |
| `tools/agent/setup_project.py` | Python 3 | the files an AI coding agent works from: `AGENTS.md`, `design.json`, `scenarios.json` and the startup file of each common agent |

Use the shell script or the batch file when you write the code yourself. Use the Python
tool when an AI coding agent writes it; see [Building with an AI Coding Agent](#building-with-an-ai-coding-agent).

### Prerequisites

- **CMake 3.20+**
- **C++17 compiler**: GCC, Clang or MSVC
- **Java 17+**: runs the code generator during the build
- **Git**: fetches Areg SDK during the configure step, unless `--sdk-root` names a local copy

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Options

The three tools take the same options.

| Option | Meaning | Default |
|--------|---------|---------|
| `--name NAME` | Project name, a C identifier. It becomes the directory, the CMake project and the executable names | asked on a terminal, otherwise required |
| `--mode MODE` | `local`: one process, two threads. `ipc`: two processes. `pubsub`: attributes and broadcasts, one process | `local` |
| `--root DIR` | Directory to create | `./NAME` |
| `--sdk-root DIR` | Build against this local Areg SDK copy instead of fetching it from GitHub | fetch from GitHub |
| `--tag TAG` | The Areg SDK git tag to fetch | the tag this SDK documents |
| `--force` | Scaffold into a directory that is not empty. Files of the same name are overwritten; nothing is deleted | refuse |

`setup_project.py` has a few more options for the agent files: `--no-agents`,
`--harness`, `--no-harness`, `--quiet`. Run it with `--help`.

**Example: two processes, built against a local SDK copy:**
```bash
sh areg-sdk/tools/setup-project.sh --name chat --mode ipc --root ~/dev/chat --sdk-root ./areg-sdk
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Project Modes

| Aspect                | `local`                     | `ipc`                                 | `pubsub`                            |
| --------------------- | --------------------------- | ------------------------------------- | ----------------------------------- |
| **Executables**       | 1                           | 2: `NAME_provider`, `NAME_consumer`   | 1                                   |
| **Communication**     | in-process, between threads | between processes, through `mtrouter` | in-process, between threads         |
| **Service shows**     | request and response        | request and response                  | attributes and broadcasts           |
| **Service interface** | `Category="Private"`        | `Category="Public"`                   | `Category="Private"`                |
| **Requires mtrouter** | No                          | Yes                                   | No                                  |
| **Start script**      | --                          | `run.sh` and `run.bat`                | --                                  |
| **Recipe**            | `01-local-single-process`   | `02-ipc-two-processes`                | `03-attributes-and-broadcast`       |

### `local`: one process

```
┌────────────────────────────┐
│     Single Process         │
│  ┌──────────┐ ┌──────────┐ │
│  │ Provider │ │ Consumer │ │
│  │ Thread   │ │ Thread   │ │
│  └──────────┘ └──────────┘ │
└────────────────────────────┘
```

The provider and the consumer run in two threads of one executable. No `mtrouter`.

### `ipc`: two processes

```
┌─────────────┐         ┌──────────┐         ┌─────────────┐
│  Provider   │────────►│ mtrouter │◄────────│  Consumer   │
│  Process    │         │ (Router) │         │  Process    │
└─────────────┘         └──────────┘         └─────────────┘
```

The provider and the consumer are separate executables connected by `mtrouter`, on one
machine or across a network.

### `pubsub`: attributes and broadcasts

One executable with a provider and a consumer thread, like `local`. The provider publishes
attributes and broadcasts; the consumer subscribes to them and reacts to every update.

> [!NOTE]
> The component code of `local` and `ipc` is the same. What differs is the model in each
> `main()`, which says which components run in which thread of which process, and the
> interface category: an interface must be `Public` to be reached from another process.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Generated Project Structure

### Directory Layout

```
areg_hello/
├── CMakeLists.txt              # Areg SDK integration
├── .gitignore
├── src/
│   ├── CMakeLists.txt          # generates the service code, declares the executables
│   ├── services/
│   │   └── HelloService.siml   # service interface
│   ├── main.cpp                # local: provider, consumer and model
│   ├── provider.cpp            # ipc: provider process
│   └── consumer.cpp            # ipc: consumer process
├── run.sh                      # ipc: starts mtrouter, provider, consumer
└── run.bat                     # ipc: the same on Windows
```

### Top-Level CMakeLists.txt

It uses an installed Areg SDK package when CMake finds one. Otherwise it fetches the SDK
with `FetchContent`, or uses the local copy given with `--sdk-root`. The build output goes
to `build/bin/`.

```cmake
find_package(areg CONFIG)

set(AREG_BUILD_DIR "${CMAKE_BINARY_DIR}")
option(AREG_OUTPUT_LAYOUT "Areg build structure" OFF)

if (NOT areg_FOUND)
    set(AREG_DEPS_DIR  "${CMAKE_BINARY_DIR}/packages")
    set(AREG_LIB_TYPE  shared)
    option(AREG_TESTS         "Build areg-sdk tests"    OFF)
    option(AREG_EXAMPLES      "Build areg-sdk examples" OFF)
    option(AREG_SYSTEM_GTEST  "Build GTest"             OFF)

    include(FetchContent)
    set(FETCHCONTENT_BASE_DIR "${AREG_DEPS_DIR}")
    FetchContent_Declare(areg
        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
        GIT_TAG "master")
    FetchContent_MakeAvailable(areg)

    set(AREG_SDK_ROOT         "${areg_SOURCE_DIR}")
    set(AREG_CMAKE_CONFIG_DIR "${AREG_SDK_ROOT}/conf/cmake")
    set(AREG_CMAKE            "${AREG_SDK_ROOT}/areg.cmake")
endif()

include(${AREG_CMAKE})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory(src)
```

With `--sdk-root`, the `FetchContent_Declare` line becomes
`FetchContent_Declare(areg SOURCE_DIR "/path/to/areg-sdk")`.

### HelloService.siml

One request with a `client` parameter and one response with a `success` flag:

```xml
<Method ID="2" Name="hello_service" MethodType="Request" Response="hello_service">
    <ParamList><Parameter ID="51" Name="client" DataType="String"/></ParamList>
</Method>
<Method ID="4" Name="hello_service" MethodType="Response">
    <ParamList><Parameter ID="52" Name="success" DataType="bool"/></ParamList>
</Method>
```

During the build, `codegen.jar` generates `HelloServiceProviderBase`,
`HelloServiceConsumerBase`, the proxy and the constants from this file.

> [!TIP]
> Learn more about service interfaces: [Code Generator Guide](./06b-code-generator.md)

### Provider

```cpp
void request_hello_service(const areg::String & client) final
{
    std::cout << "provider: hello, " << client << std::endl;
    response_hello_service(true);
}
```

### Consumer

```cpp
bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final
{
    bool result{ false };
    if (HelloServiceConsumerBase::service_connected(status, proxy))
    {
        result = true;
        if (areg::is_service_connected(status))
        {
            request_hello_service(role_name());
        }
    }

    return result;
}

void response_hello_service(bool success) final
{
    std::cout << "consumer: " << (success ? "greeted" : "failed") << std::endl;
    areg::Application::signal_quit();
}
```

### Model (`local`)

```cpp
BEGIN_MODEL(_modelName)
    BEGIN_REGISTER_THREAD("ProviderThread")
        BEGIN_REGISTER_COMPONENT("ServiceProvider", ServiceProvider)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceProvider")
    END_REGISTER_THREAD("ProviderThread")

    BEGIN_REGISTER_THREAD("ConsumerThread")
        BEGIN_REGISTER_COMPONENT("ServiceConsumer", ServiceConsumer)
            REGISTER_DEPENDENCY("ServiceProvider")
        END_REGISTER_COMPONENT("ServiceConsumer")
    END_REGISTER_THREAD("ConsumerThread")
END_MODEL(_modelName)
```

In `ipc` mode, `provider.cpp` holds only the provider thread and `consumer.cpp` only the
consumer thread. The consumer registers under a generated unique name, so several consumer
processes can run at the same time.

### Source CMakeLists.txt

**`local`:**
```cmake
addServiceInterface(gen_areg_hello src/services/HelloService.siml)
macro_declare_executable(areg_hello gen_areg_hello main.cpp)
```

**`ipc`:**
```cmake
addServiceInterface(gen_areg_hello src/services/HelloService.siml)
macro_declare_executable(areg_hello_provider gen_areg_hello provider.cpp)
macro_declare_executable(areg_hello_consumer gen_areg_hello consumer.cpp)
```

`addServiceInterface` generates the code from the `.siml` file, and
`macro_declare_executable` creates an executable linked with Areg. See the
[CMake Functions Reference](./02e-cmake-functions.md).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Building and Running

### Build

```bash
cd areg_hello
cmake -B build
cmake --build build -j
```

The first configure fetches Areg SDK, unless `--sdk-root` names a local copy, and the first
build compiles the framework: a few minutes. A rebuild takes seconds.

### Run a `local` project

Run `areg_hello` from `build/bin/`. It prints:

```
provider: hello, ServiceConsumer
consumer: greeted
```

and exits.

### Run an `ipc` project

The project carries a start script:

```bash
./run.sh        # Linux / macOS
run.bat         # Windows
```

It starts `mtrouter`, waits until it listens, starts the provider, then the consumer,
and stops the router and the provider when the consumer ends. The provider prints
`provider: hello, ServiceConsumer...` and the consumer prints `consumer: greeted`.

To start the pieces by hand, run `mtrouter`, then `areg_hello_provider`, then
`areg_hello_consumer`, each from `build/bin/`. For a network deployment, set the
`mtrouter` address in the application configuration file; see
[Areg SDK Multitarget Router (mtrouter)](./03a-mtrouter.md).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Building with an AI Coding Agent

Python 3 is **not** needed to build or run an Areg application. It **is** needed for
effective agentic coding, because the tools an agent works with are Python scripts:

| Tool | What it does for the agent |
|------|----------------------------|
| `setup_project.py` | writes `AGENTS.md`, which routes the agent to the one page its task needs, and the startup file of each common agent |
| `build_project.py` | turns one `design.json` into the service documents, the C++ skeleton, the model and the test scenarios, then builds and runs them in one command |
| `check_contract.py` | catches the defects that compile cleanly and fail silently, before the build |
| `run_scenarios.py` | runs the application and exits 0 only when every scenario passes |

Without these tools an agent writes the communication code, the model and the tests by
hand, from memory. That means more requests, more build-and-fix cycles, more tokens and
more defects. In a measured comparison, an agent using these tools wrote about 2.8x less
C++ by hand and needed 3x fewer requests than an agent using gRPC:
[AI benchmark](../../examples/ai-benchmark/baseline-2026-09-13.md).

**Start an agent project:**
```bash
python3 areg-sdk/tools/agent/setup_project.py --name myapp --mode ipc --sdk-root ./areg-sdk
```

Then open your coding agent in `myapp/` and describe the application. The agent finds
`AGENTS.md` on its own. The full agent guide is [`AGENTS.md`](../../AGENTS.md) in the SDK.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Customizing Your Project

### Modifying the Service Interface

Add a method to `src/services/HelloService.siml`:

```xml
<Method ID="6" Name="goodbye" MethodType="Request" Response="goodbye"/>
<Method ID="8" Name="goodbye" MethodType="Response"/>
```

> [!TIP]
> You can use [`Lusan`](https://github.com/aregtech/areg-sdk-tools) to edit `.siml` files.
> See [Creating Service Interface Documents with Lusan](./06e-lusan-service-interface.md).

Implement it in the provider:

```cpp
void request_goodbye() final
{
    std::cout << "provider: goodbye" << std::endl;
    response_goodbye();
}
```

Call it from the consumer:

```cpp
void response_hello_service(bool success) final
{
    request_goodbye();
}

void response_goodbye() final
{
    std::cout << "consumer: goodbye received" << std::endl;
    areg::Application::signal_quit();
}
```

Rebuild with `cmake --build build`. The generated base classes are regenerated from the
changed document.

### Adding More Services

```cmake
addServiceInterface(gen_areg_hello src/services/HelloService.siml)
addServiceInterface(gen_areg_hello src/services/ChatService.siml)

macro_declare_executable(areg_hello gen_areg_hello main.cpp chat.cpp)
```

One generated library can hold the code of several service interfaces.

### Configuration Options

Areg is configured when `FetchContent_MakeAvailable(areg)` runs, so its options go inside
the `if (NOT areg_FOUND)` block of the top-level `CMakeLists.txt`, before that call:

```cmake
    set(AREG_LIB_TYPE  static)    # instead of shared: link Areg statically
    set(AREG_LOGGING   OFF)       # compile without logs; the default is ON
```

An installed Areg package was built with its own options, and these lines do not change it.

All options: [CMake Configuration Guide](./02d-cmake-config.md).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### `error: ... exists and is not empty`

The target directory already holds files. Choose another `--root`, or pass `--force` to
write the project files over it; nothing else in the directory is touched.

### `error: not found on PATH: cmake` or `git`

Install the missing tool. `git` is needed only when the SDK is fetched; `--sdk-root`
avoids it.

*Ubuntu/Debian:* `sudo apt-get install cmake git`
*macOS:* `brew install cmake git`
*Windows:* [cmake.org](https://cmake.org/download/) and [git-scm.com](https://git-scm.com/)

### Script Permission Denied (Linux/macOS)

**Problem:** `Permission denied` when running `./areg-sdk/tools/setup-project.sh`
**Solution:** run it through the shell: `sh areg-sdk/tools/setup-project.sh`

### Scripts Are Blocked (Windows)

**Problem:** PowerShell refuses to run `setup-project.ps1`.
**Solution:** use `setup-project.bat`, which starts it with the execution policy bypassed
for that one run.

### Build Fails with "Java not found"

Code generation needs Java 17+.

*Ubuntu/Debian:* `sudo apt-get install openjdk-17-jre`
*macOS:* `brew install openjdk@17`
*Windows:* [Adoptium](https://adoptium.net/)

Verify with `java -version`.

### FetchContent Download Fails

The configure step cannot reach GitHub. Create the project against a local clone instead:

```bash
sh areg-sdk/tools/setup-project.sh --name areg_hello --sdk-root ./areg-sdk --force
```

### The Consumer Waits and Never Prints (`ipc`)

The consumer connects through `mtrouter`. Use `run.sh` or `run.bat`, or start `mtrouter`
before the processes. In the `mtrouter` console, `--instances` lists the connected
processes.

### Compile Error: "Cannot find HelloServiceProviderBase.hpp"

Code generation failed. Check `java -version` (17+), delete `build/`, configure and build
again, and look for codegen errors in the build output.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Next Steps

**Integration:**
- [CMake Integration](./02b-cmake-integrate.md): FetchContent, vcpkg, submodule
- [Visual Studio Integration](./02c-msvc-integrate.md): MSVC-specific setup
- [CMake Configuration Options](./02d-cmake-config.md) and [CMake Functions Reference](./02e-cmake-functions.md)

**Service design:**
- [Code Generator Guide](./06b-code-generator.md)
- [Service Interface Design with Lusan](./06e-lusan-service-interface.md)

**Examples:**
- [Examples Directory](../../examples/README.md)
- [Agent recipes](../agent/recipes/README.md): complete projects for more shapes, including attributes and broadcasts, timers and state machines

**Help:**
Open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or an [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech (Artak Avetyan), www.areg.tech, email: info[at]areg.tech
