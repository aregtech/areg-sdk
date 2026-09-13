# Installing gRPC for the comparison arm

Do this once, outside any measured run. The agent never installs anything itself: an
install inside a run is billed as framework cost and makes the two arms incomparable.
`run-benchmark.sh --framework grpc` and `run-benchmark.ps1 --framework grpc` refuse to
start while `protoc` or `grpc_cpp_plugin` is missing.

Every route below installs **prebuilt packages**: nothing is cloned and nothing is
compiled. What the arm needs, whichever route:

| Needed | Why |
|---|---|
| `protoc` on the `PATH` | generates the message code from the `.proto` |
| `grpc_cpp_plugin` on the `PATH` | generates the service code |
| gRPC and protobuf headers and libraries | the two programs link them |
| CMake config packages (`gRPCConfig.cmake`, `protobuf-config.cmake`) | `find_package(gRPC CONFIG)` in the agent's `CMakeLists.txt` |
| CMake 3.20 or newer and a C++17 compiler | the build |

---

## Linux

**Debian and Ubuntu**

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config \
                    libgrpc++-dev protobuf-compiler-grpc libprotobuf-dev protobuf-compiler
```

**Fedora, RHEL and derivatives**

```bash
sudo dnf install -y gcc-c++ cmake pkgconf-pkg-config \
                    grpc-devel grpc-plugins protobuf-devel protobuf-compiler
```

**Arch Linux**

```bash
sudo pacman -S --needed base-devel cmake grpc protobuf
```

Distribution packages lag the gRPC release by months; the benchmark does not need a
recent one. Some distributions ship pkg-config files but no CMake config package -- the
check [below](#check-it) says so. If yours does, use the conda route, which works the
same way on Linux.

---

## macOS

With [Homebrew](https://brew.sh):

```bash
xcode-select --install          # the compiler, once
brew install cmake grpc protobuf
```

Homebrew installs bottles (prebuilt binaries) for current macOS versions on both Apple
Silicon and Intel. `grpc` brings `grpc_cpp_plugin`; `protobuf` brings `protoc`.

---

## Windows

A Windows run needs the compiler and the gRPC libraries to agree, so pick **one** route
and use its compiler for the build.

### Route 1: conda-forge, with MSVC (recommended)

Install Visual Studio 2022 (or its Build Tools) with the "Desktop development with C++"
workload, then [Miniforge](https://github.com/conda-forge/miniforge), and in a
**Developer PowerShell for VS 2022**:

```powershell
conda create -y -n grpc -c conda-forge libgrpc libprotobuf cmake
conda activate grpc
$env:CMAKE_PREFIX_PATH = "$env:CONDA_PREFIX\Library"
```

conda-forge ships MSVC-built binaries; `protoc.exe` and `grpc_cpp_plugin.exe` land in
`%CONDA_PREFIX%\Library\bin`, which `conda activate` puts on the `PATH`. Start
`run-benchmark.ps1` from that same shell, so the agent inherits the `PATH` and
`CMAKE_PREFIX_PATH`.

### Route 2: MSYS2, with MinGW-w64 GCC

Install [MSYS2](https://www.msys2.org), then in the **MSYS2 UCRT64** shell:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake \
                   mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-grpc \
                   mingw-w64-ucrt-x86_64-protobuf
```

Add `C:\msys64\ucrt64\bin` to the `PATH` of the PowerShell that starts the run, and build
with the `Ninja` or `MinGW Makefiles` generator. Do not mix this route with MSVC.

### Not a route: vcpkg

`vcpkg install grpc` compiles gRPC and all its dependencies from source on the first
install -- an hour or more. It works, but it is the from-source route under another name.

---

## Check it

`protoc` and the plugin first:

```bash
protoc --version
grpc_cpp_plugin --version        # some builds print "Unknown option" and still work
```

Then that CMake finds the packages, because a project that cannot configure turns the
run into a build-system exercise and measures nothing. In an empty directory, write this
`CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20)
project(probe CXX)
find_package(Protobuf CONFIG)
find_package(gRPC CONFIG)
message(NOTICE "Protobuf_FOUND=${Protobuf_FOUND} gRPC_FOUND=${gRPC_FOUND}")
```

and configure it, from the same shell that will start the run:

```bash
cmake -S . -B build
```

Both values must print `1`. If `gRPC_FOUND` is empty, the packages ship pkg-config files
but no CMake config package: use another route above, or record in the run's notes that
the arm was told to use pkg-config, because it changes what the agent has to do.

---

## What must not be done

- **Never `FetchContent` gRPC inside the measured project.** It builds gRPC during the
  agent's own build, adding tens of minutes and thousands of lines of unrelated compiler
  output to the agent's context. The areg arm links a framework it builds once; the gRPC
  arm links an installed one.
- **Never let the agent run a package manager.** If a header is missing the run is void:
  fix the machine and start a fresh run.
