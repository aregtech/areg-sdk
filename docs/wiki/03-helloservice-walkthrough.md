# One service implementation, three deployments

Run the same C++ provider and consumer in one thread, in two threads, and in separate
processes. This walkthrough uses the existing [`03_helloservice`](../../examples/03_helloservice/)
example; you do not need to edit its sources.

The point is not the greeting. It is that the service implementations stay shared
while the application models change.

## Build the example

Requires Git, CMake 3.20+, a C++17 compiler and Java 17+ for code generation.
See [build prerequisites](./01e-build-environment.md). On Windows, use PowerShell
with a supported Visual Studio C++ installation.

If you already followed the README, start in that `areg-sdk` directory and skip the
clone, `cd` and configure commands. Otherwise start without an existing `areg-sdk` folder:

```sh
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target 03_onethread 03_twothreads 03_pubservice 03_pubclient mtrouter -j4
```

This evaluates `master`, not a pinned release. Only the requested targets and their
dependencies are built. Executables go to `build/bin`, including for Visual Studio
generators; `--config Release` selects that configuration.
Java is a build dependency, not an application runtime dependency.

## 1. One thread, then two

Run both commands for your platform, from the `areg-sdk` directory:

<details open>
<summary>Linux</summary>

```sh
./build/bin/03_onethread.elf
./build/bin/03_twothreads.elf
```

</details>

<details>
<summary>macOS</summary>

```sh
./build/bin/03_onethread.mac
./build/bin/03_twothreads.mac
```

</details>

<details>
<summary>Windows (PowerShell)</summary>

```powershell
.\build\bin\03_onethread.exe
.\build\bin\03_twothreads.exe
```

</details>

Each run prints the greeting:

```text
'Hello Service!' from ServiceClient
```

The consumer's response handler prints `Succeeded to output message.` when it receives
the reply. This deliberately short-lived example signals application shutdown from the
provider as well as the consumer; use the provider's greeting as the basic run check,
not response delivery during teardown as a lifecycle guarantee.

Neither deployment needs a running router. In the first, both components share
`Thread1`; in the second, the consumer belongs to `Thread2`.

## 2. Separate processes

Keep three terminals open, each in the same `areg-sdk` directory. Start the router
first, then the provider, then the consumer. Use an otherwise unused local router
endpoint; the default is `localhost:8181`.

<details open>
<summary>Linux</summary>

Terminal 1:
```sh
./build/bin/mtrouter.elf --console
```

Terminal 2:
```sh
./build/bin/03_pubservice.elf
```

Terminal 3:
```sh
./build/bin/03_pubclient.elf
```

</details>

<details>
<summary>macOS</summary>

Terminal 1:
```sh
./build/bin/mtrouter.mac --console
```

Terminal 2:
```sh
./build/bin/03_pubservice.mac
```

Terminal 3:
```sh
./build/bin/03_pubclient.mac
```

</details>

<details>
<summary>Windows (PowerShell)</summary>

Terminal 1:
```powershell
.\build\bin\mtrouter.exe --console
```

Terminal 2:
```powershell
.\build\bin\03_pubservice.exe
```

Terminal 3:
```powershell
.\build\bin\03_pubclient.exe
```

</details>

The provider prints `'Hello Service!' from ServiceClient_...`; the suffix is generated
for this consumer process. The consumer prints `Succeeded to output message.` when
its response arrives. The example processes exit after the exchange/disconnection;
the router stays running. Type `-q` and press Enter in its terminal when finished.

The provider and consumer have their own process models. They communicate through
`mtrouter`, even on the same machine. The service handlers did not acquire socket code.

For communication between machines, configure the router address and port for each
application and deploy the executables and their runtime libraries.
That is a separate deployment exercise, not something the local commands above demonstrate.
See [router configuration](./03a-mtrouter.md#configuration).
The current transport has no built-in authentication or encryption; use a trusted network.

## What changes

All paths below are relative to [`examples/03_helloservice`](../../examples/03_helloservice/).

| File | Role | Changes between these variants? |
|---|---|---|
| [`services/HelloService.siml`](../../examples/03_helloservice/services/HelloService.siml) | Typed request/response contract, already `Public` | No |
| [`common/src/ServiceComponent.cpp`](../../examples/03_helloservice/common/src/ServiceComponent.cpp) | Provider behavior | No |
| [`common/src/ClientComponent.cpp`](../../examples/03_helloservice/common/src/ClientComponent.cpp) | Consumer behavior | No |
| [`onethread/src/main.cpp`](../../examples/03_helloservice/onethread/src/main.cpp) | Both components inside one thread registration | Selected for the one-thread executable |
| [`twothreads/src/main.cpp`](../../examples/03_helloservice/twothreads/src/main.cpp) | Provider and consumer in separate thread registrations | Selected for the two-thread executable |
| [`multiprocess/serviceproc/src/main.cpp`](../../examples/03_helloservice/multiprocess/serviceproc/src/main.cpp) | Provider-only process model | Selected for the provider executable |
| [`multiprocess/clientproc/src/main.cpp`](../../examples/03_helloservice/multiprocess/clientproc/src/main.cpp) | Consumer-only process model | Selected for the consumer executable |
| [`CMakeLists.txt`](../../examples/03_helloservice/CMakeLists.txt) | Four targets using the same two implementation files | Selects the entry point for each target |

Compare the two-thread model with the two process models. The provider still registers
as `ServiceComponent`; the consumer still declares `add_dependency_service("ServiceComponent")`.
The consumer process generates its own unique component name, but its dependency is unchanged.

In your application, introducing service boundaries is design work. Once those boundaries
exist, changing their compiled model declarations or build targets requires a rebuild,
not a rewrite of the shared service behavior. Editing `areg.init` alone does not move a
component into another process.

`Public` is an interface-design choice: it permits both local and cross-process use.
`Private` restricts the interface to the process. This example starts with `Public`,
so switching among its supplied variants requires no interface edit.

## If it does not connect

- Check that `mtrouter` is running and listening on the endpoint the applications use.
  If another program owns port 8181, resolve that conflict rather than starting a second router.
- Run the provider and consumer from this same build. Check the router's `-n` command
  for connected processes while they are waiting.
- A provider waiting for a consumer is normal. If you rerun the consumer after the
  provider has exited, start the provider again.
- If you changed the configuration, check address/port settings and firewall access.
  Configuration can be deployed beside the executables under `config/areg.init`;
  see the [configuration guide](./02d-cmake-config.md) for build options and
  [router guide](./03a-mtrouter.md#configuration) for runtime settings.

## Make it yours

[Create a new project](./02a-quick-project-setup.md), [integrate with CMake](./02b-cmake-integrate.md),
or explore [application patterns](../USECASES.md).
The setup recipes use application logging; `03_helloservice` is a console demonstration.

[Back to the README](../../README.md#quick-start)
