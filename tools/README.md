
# Areg SDK Tools

This directory contains **developer tools** shipped with the **Areg SDK**, designed to help you:

- Generate RPC service code from `.siml` files
- Integrate generated code into CMake-based projects
- Quickly bootstrap a working Areg example project

> **Areg SDK** is a multitasking application development platform for building modular, distributed, and IPC/RPC-based systems.

---

## Contents Overview

| Tool / Script        | Purpose                                                |
|----------------------|--------------------------------------------------------|
| `codegen.jar`        | Generates C++ service code from `.siml` files          |
| `setup-project.bat`  | Creates a ready-to-build Areg project on Windows       |
| `setup-project.sh`   | Creates a ready-to-build Areg project on Linux/macOS   |
| `sanitize.sh`        | Builds/runs the SDK under a sanitizer or profiler      |
| `sanitizer/*.supp`   | Suppression files for LeakSanitizer / ThreadSanitizer  |
| `conf/cmake/*.cmake` | CMake helper functions used by Areg projects           |

---

## 1. Designing and Generating Service Code from `.siml`

The **Areg Code Generator** converts a Service Interface Definition (`.siml`) into C++ source files for Areg components.

> [!TIP]
> For designing and testing your service interfaces visually, you can use the **Lusan project**. Lusan helps you create, validate, and log `.siml` service definitions, making development faster and less error-prone.  
> Lusan Project Repository: [https://github.com/aregtech/areg-sdk-tools](https://github.com/aregtech/areg-sdk-tools)

### Step 1: Create a `.siml` File

A `.siml` file defines your service interface: data types, data attributes, requests, responses, and broadcasts.  
Place it in your project, e.g.:

```
<project-root>/src/services/HelloService.siml
```

---

### Step 2: Run the Code Generator

From a terminal:

```bash
java -jar <areg-sdk-root>/tools/codegen.jar \
    --root=<project-root>                   \
    --doc=<relative-path-to-siml>           \
    --target=<relative-path-to-output>
```

#### Parameters

| Parameter                   | Description                                       |
| --------------------------- | ------------------------------------------------- |
| `<areg-sdk-root>`           | Root directory of the Areg SDK source             |
| `<project-root>`            | Root directory of your application                |
| `<relative-path-to-siml>`   | Path to `.siml` file **relative to project root** |
| `<relative-path-to-output>` | Directory where generated code will be placed     |

#### Example

```bash
java -jar ~/areg-sdk/tools/codegen.jar   \
    --root=./my_project                  \
    --doc=src/services/HelloService.siml \
    --target=src/services
```

This generates C++ headers and sources next to your `.siml` file.

---

## 2. Using `.siml` Files in CMake (Recommended)

Instead of running the generator manually, Areg recommends **using CMake integration**.

### Add Service Interface in `CMakeLists.txt`

```cmake
addServiceInterface(
    gen_my_project
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services/HelloService.siml
)
```

This:

* Runs the code generator automatically
* Produces a **static library** (`gen_my_project`)
* Ensures consistency and reproducibility across builds

---

### Link Generated Code to Your Executable

```cmake
macro_declare_executable(
    my_project
    gen_my_project
    src/main.cpp
)
```

The executable automatically links against Areg and the generated service code.

📌 **Reference:** See `conf/cmake/functions.cmake` for full details on `addServiceInterface()` and related macros.

---

## 3. Best Practices for Generated Code

### ✔ Use Static Libraries

Always place generated service code into one or more **dedicated static libraries**.
This ensures code modularity, reuse, and consistency across processes.

Example:

```
gen_services (static library)
├── HelloServiceConsumerBase.cpp
├── HelloServiceProviderBase.cpp
├── HelloServiceEvents.cpp
├── HelloServiceProxy.cpp
└── HelloService.cpp
```

### ✔ Share Between Processes

If your system has multiple processes:

* Create one or more static libraries for the generated code
* Link these libraries into applications by need
* Guarantees consistent RPC interfaces across IPC boundaries

---

## 4. Project Setup Scripts (Quick Start)

Areg provides scripts to bootstrap a **working example project** in one step.

### Available Scripts

| Platform      | Script              |
| ------------- | ------------------- |
| Windows       | `setup-project.bat` |
| Linux / macOS | `setup-project.sh`  |

### What the Script Does

* Prompts for **project name** (or uses default)

* Prompts for **project root folder** (or uses default)

* Creates project directories:

  ```
  src/
  src/services/
  ```

* Generates:

  * Top-level `CMakeLists.txt`
  * `src/CMakeLists.txt`
  * Example `src/services/HelloService.siml` Service Interface file
  * Working `src/main.cpp` file with Service Provider and Service Consumer

* Configures **Areg dependency automatically**

---

### How to Run

From the `tools/` directory:
* Run on Linux/macOS:

   ```bash
   sh ./setup-project.sh
   ```
* Run on Windows:
   ```bat
   ./setup-project.bat
   ```

Follow the interactive prompts to set the project name and location.

---

## 5. Building the Generated Project

After project creation:

```bash
cd <project-root>
cmake -B build
cmake --build build
```

This produces a **fully functional Areg RPC example**.

---

## 6. Examples and Advanced Usage

* **RPC (single process)**: Generated by setup scripts
* **IPC (multi-process)**: See `examples/` folder in the Areg SDK root
* **Custom build flows**: Inspect `conf/cmake/` for advanced control

---

## 7. Quality & Performance: Sanitizers and Profilers

`sanitize.sh` builds the SDK in an **isolated, throw-away build tree** (`build-asan/`,
`build-tsan/`, `build-perf/`) and runs a target under a compiler sanitizer or a
profiler. It never edits the build-system files and never touches your normal
`build/` tree — every flag is passed on the CMake command line only.

> Sanitizers ship **inside** GCC/Clang, so nothing extra needs to be installed.
> Profilers (`perf`, `heaptrack`, `valgrind`) are optional system tools; the
> script detects a missing one and prints the exact `apt-get` line.

### Prerequisites and environment setup

`sanitize.sh` **does not install anything**. It only verifies that a tool is
present and, if not, prints the exact install command and stops. You install the
prerequisites once, up front. There is no separate "setup" step beyond this — the
script configures and builds an isolated CMake tree itself.

**1. Compiler with sanitizer runtimes (required).** The sanitizers are part of the
compiler toolchain, not a separate download:

| Toolchain | What provides the sanitizers | Install on Debian/Ubuntu | Install on Fedora/RHEL |
|-----------|------------------------------|--------------------------|------------------------|
| GCC (`--compiler gnu`, default) | `libasan`, `libubsan`, `libtsan` runtimes shipped with `g++` | `sudo apt-get install g++` (runtimes included) | `sudo dnf install gcc-c++ libasan libubsan libtsan` |
| Clang (`--compiler llvm`) | `compiler-rt` shipped with `clang` | `sudo apt-get install clang` | `sudo dnf install clang compiler-rt` |

Verify the runtime is actually available before the first run:

```bash
g++     -print-file-name=libasan.so      # prints a real path, not just "libasan.so"
g++     -print-file-name=libtsan.so      # TSan runtime
clang++ -print-file-name=libclang_rt.asan-x86_64.so   # for --compiler llvm
```

If a command echoes the bare file name, the runtime package is missing — install it
from the table above.

**2. CMake 3.20+ (required)** — the same version the SDK already needs. No extra
build dependency is introduced by the script.

**3. Profilers (optional, only for the `perf` / `heaptrack` / `callgrind` modes).**
These are real system packages; install only the one you intend to use:

| Mode | Tool | Debian/Ubuntu package |
|------|------|-----------------------|
| `perf` | `perf` | `sudo apt-get install linux-tools-common linux-tools-$(uname -r)` |
| `heaptrack` | `heaptrack` | `sudo apt-get install heaptrack` |
| `callgrind` | `valgrind` | `sudo apt-get install valgrind` |

> `perf` usually also needs `linux-tools-$(uname -r)` (the kernel-matched package),
> and `kernel.perf_event_paranoid` low enough to record:
> `sudo sysctl kernel.perf_event_paranoid=1`. Under WSL2 `perf` requires a kernel
> built with perf support.

**4. ThreadSanitizer needs a compatible address-space layout.** See the dedicated
note below — `sanitize.sh tsan` handles it automatically with `setarch -R` (provided
by `util-linux`, already present on any normal Linux). No root required.

**5. WSL2 / `/mnt/c` (drvfs) caveat.** Building the SDK on a Windows-mounted drive
works, but the large (>150 MB) instrumented `libareg.a` can intermittently fail its
**first** link with `file format not recognized` — a drvfs write-visibility race, not
a real corruption. Re-run the same `sanitize.sh` command (or build on a native Linux
filesystem such as `~/`) and it links cleanly.

### Usage

```bash
tools/sanitize.sh <mode> [options] [-- <args forwarded to the binary>]
```

| Mode         | What it catches / does                                                      |
|--------------|-----------------------------------------------------------------------------|
| `asan`       | **AddressSanitizer + LeakSanitizer + UBSan** — use-after-free/scope, heap/stack overflow, leaks, undefined behavior. *Primary memory tool.* |
| `tsan`       | **ThreadSanitizer** — data races and lock-order inversions (separate build; cannot be combined with `asan`). |
| `ubsan`      | UndefinedBehaviorSanitizer only — cheap, no ASan slowdown.                  |
| `build-only` | Configure + build the instrumented target without running.                 |
| `perf`       | `perf stat -d -d` on an optimized (`RelWithDebInfo`) build — IPC/CPU profiling. |
| `heaptrack`  | Allocation profiling: peak RSS and allocation hot spots — the memory-retention lens. |
| `callgrind`  | `valgrind --tool=callgrind` instruction/call-graph profile.                 |

| Option            | Default            | Meaning                                          |
|-------------------|--------------------|--------------------------------------------------|
| `--compiler`      | `gnu`              | `gnu` (g++) or `llvm` (clang++).                 |
| `--target`        | `areg-unit-tests`  | CMake target to build.                           |
| `--run`           | `auto`             | `ctest`, an explicit binary path, `auto`, or `none`. |
| `--lib`           | `static`           | `static` (most robust for ASan) or `shared`.     |
| `--examples`      | off                | Also build the `examples/` tree.                 |
| `--jobs N`        | `nproc`            | Parallel build jobs.                             |
| `--keep`          | off                | Reuse the existing build dir (skip reconfigure). |

### Examples

```bash
# Run the whole unit-test suite under ASan/LSan/UBSan:
tools/sanitize.sh asan --run ctest

# Hunt data races across the test suite:
tools/sanitize.sh tsan --run ctest

# Memory + concurrency check of a live IPC client (needs mtrouter running):
tools/sanitize.sh asan --target 23_pubclient --examples -- <client args>

# Allocation / peak-RSS profile of the router under load:
tools/sanitize.sh heaptrack --target mtrouter --examples -- -e
```

### Latency-focused profiling tips

For the sub-millisecond IPC dispatch work, the highest-value `perf` views are:

```bash
perf stat -d -d   <bin>          # IPC, cache-miss and branch-miss rates
perf c2c record   <bin>          # cache-line contention / false sharing (MPSC queues, refcounts)
perf record -e cache-misses <bin>; perf report
perf sched record <bin>; perf sched latency   # scheduler wake-up latency
```

`perf c2c` is especially relevant here: it pinpoints cache lines bounced
between cores — the classic cost in lock-free queues and intrusive refcounts.

### Note: ThreadSanitizer and ASLR

On modern kernels (Ubuntu 24.04 / WSL2 default `vm.mmap_rnd_bits=32`) TSan
aborts at startup with `FATAL: ThreadSanitizer: unexpected memory mapping`
because the high-entropy address layout does not fit its shadow memory.
`sanitize.sh tsan` works around this automatically by running the target under
`setarch -R` (disables ASLR for that process — no root needed). If you run a
TSan binary by hand and hit this, either prefix it with `setarch "$(uname -m)" -R`
or lower the entropy once with `sudo sysctl vm.mmap_rnd_bits=28`.

---

## 8. Summary

* Use `.siml` to define services
* Prefer **CMake integration** over manual code generation
* Keep generated code in **static libraries** for modularity
* Use setup scripts to get started instantly
* Leverage **Lusan** for visual service interface design and logging
* Follow examples for real-world IPC patterns

This workflow scales from a **hello-service example** to **full distributed production systems**.
