# Areg SDK Tools

Everything the SDK ships to generate code, build, measure, debug and verify. The
directory has two levels, and which level a tool sits in tells you who runs it.

> **Areg SDK** is a multitasking application development platform for building modular, distributed, and IPC/RPC-based systems.

---

## Layout

```
tools/            what a developer runs: the generator, the environment check,
                  the project scaffolder, sanitizers, profilers, benchmarks
tools/agent/      what an AI coding agent runs, and what verifies the corpus
                  that agent reads: docs/agent/, api.json and the eval bank
```

**The rule:** a tool belongs in `tools/agent/` when it exists because of the agent
documentation -- it reads `docs/agent/`, `AGENTS.md`, `docs/agent/api.json` or the
eval bank, or it scaffolds for an agent. Everything else stays in `tools/`.

Nothing in `tools/agent/` is required to develop with the SDK. It is there so that a
developer reading this directory can tell at a glance which scripts are part of their
job and which belong to the agent workflow.

### Do I need Python?

**No.** Generating, building and running an Areg application needs a C++17 compiler,
CMake 3.20+ and a Java 17+ runtime -- nothing else. Every tool in `tools/agent/` is
Python, and every one of them is optional.

The two places a developer might reach for Python have a Python-free equivalent:

| Task | Without Python | With Python |
|---|---|---|
| create a project | `setup-project.sh` / `.bat` (prompts) | `agent/setup_project.py` (unattended) |
| generate from a document | `codegenerate.sh` / `.bat`, or `addServiceInterface()` in CMake | -- |
| check the environment | `check-env.sh` / `.bat` | -- |

---

## `tools/` -- developer tools

| Tool / Script | Needs | Purpose | Section |
|---|---|---|---|
| `codegen.jar` | Java | Generates C++ service code from `.siml`, `.fsml` and `.dtml` | [1](#1-designing-and-generating-service-code-from-siml) |
| `codegenerate.sh` | Java | Ready-made generator invocation for Linux/macOS | [1](#1-designing-and-generating-service-code-from-siml) |
| `codegenerate.bat` | Java | Ready-made generator invocation for Windows | [1](#1-designing-and-generating-service-code-from-siml) |
| `schema/*.xsd` | -- | Grammars of the service, state machine and data type documents | [2](#2-document-schemas-and-the-rule-registry) |
| `schema/rules.xml` | -- | The registry of validation rule numbers, shared with Lusan | [2](#2-document-schemas-and-the-rule-registry) |
| `explain_rule.py` | Python | Explains a generator validation finding, by number or `--search` | [2](#2-document-schemas-and-the-rule-registry) |
| `check-env.sh` | -- | Reports whether CMake, Java and a compiler are present | [5](#5-project-setup-scripts-quick-start) |
| `check-env.bat` | -- | The same check on Windows | [5](#5-project-setup-scripts-quick-start) |
| `setup-project.sh` | -- | Creates a project by asking three questions; no interpreter | [5](#5-project-setup-scripts-quick-start) |
| `setup-project.bat` | -- | The same on Windows | [5](#5-project-setup-scripts-quick-start) |
| `sanitize.sh` | -- | Builds and runs a target under a sanitizer or a profiler | [8](#8-sanitizers-and-profilers) |
| `sanitizer/*.supp` | -- | Suppression files for LeakSanitizer and ThreadSanitizer | [8](#8-sanitizers-and-profilers) |
| `run-all-examples.py` | Python | Runs the built examples as pass/fail scenarios | [9](#9-running-the-examples-as-a-test-suite) |
| `run-all-examples.sh` | Python | Wrapper around the driver for Linux/macOS | [9](#9-running-the-examples-as-a-test-suite) |
| `run-all-examples.bat` | Python | Wrapper around the driver for Windows | [9](#9-running-the-examples-as-a-test-suite) |
| `report-ctest.py` | Python | Republishes failed ctest cases as build server annotations | [9](#9-running-the-examples-as-a-test-suite) |
| `areg_benchmarks.py` | Python | Turns the console output of the benchmarks into numbers | [10](#10-measuring-throughput-and-latency) |
| `latency/*` | Python | Unattended latency measurement and A/B comparison | [10](#10-measuring-throughput-and-latency) |
| `check-ascii.py` | Python | Finds non ASCII bytes and unwanted control characters | [11](#11-source-hygiene-check-asciipy) |
| `hunt-crash.py` | Python | Repeats a run under a debugger until it crashes, saves stacks | [12](#12-debugging-a-rare-crash) |
| `check_invariants.py` | Python | Seeds a defect per framework invariant, rebuilds, and asks whether the test suite notices. `--dry-run` is one second; `--restore` undoes a seed a killed run left | -- |

`explain_rule.py` reads `schema/rules.xml`, so it lives beside it. It is the one
Python script here a developer is likely to want: it turns a generator refusal into
the rule that caused it and the correction. The generator itself needs no Python.

---

## `tools/agent/` -- AI agent tools

Every script here is Python, and none is needed to build or run anything. They fall
into three jobs.

**Scaffolding an agent uses while writing an application:**

| Tool / Script | Purpose |
|---|---|
| `setup_project.py` | Creates a ready-to-build Areg project, unattended |
| `gen_skeleton.py` | Writes provider and consumer components from a `.siml` or `.fsml` |
| `fsml_layout.py` | Writes the `<Layout>` of a `.fsml` so the machine opens laid out |
| `run_scenarios.py` | Runs an application and checks its output; exit 0 is one verdict |
| `service_ports.py` | Shared helper: the router and collector ports, and waiting on them |

**Checking that application code obeys the contract:**

| Tool / Script | Purpose |
|---|---|
| `check_contract.py` | Checks sources against `docs/agent/api.json`, before a build. `--audit-prohibitions` compares the three lists of rules; `--audit-legacy` checks the removed-name lists |
| `check_symbols.py` | Checks every `areg::` name, framework macro and `Header.hpp:line` citation in the agent pages against `framework/` |
| `check_agent_docs.py` | Verifies every path the agent documentation names resolves |
| `check_commands.py` | Verifies every command the documentation gives resolves, and runs the read-only ones |

**Proving the corpus is still true:**

The rules these check against are `../../docs/ai-readiness.md`.

| Tool / Script | Purpose |
|---|---|
| `check_corpus.py` | Asks every rule the agent corpus rests on and prints a finding per rule that does not hold; `--strict` fails on a warning too |
| `run_evals.py` | Grades an application against an evaluation bank task; `--self-check` grades the reference recipes |
| `evals/tasks.json` | The bank: build tasks and repair tasks, each bound to a rule id |
| `check_recipes.py` | Generates, builds and runs every recipe under `docs/agent/recipes/` |
| `check_mutations.py` | Breaks a good recipe the way the eval bank says, and asserts the documented diagnostic still fires |
| `check_observability.py` | Runs two processes through `mtrouter` and `logcollector`, then queries the `.sqlog` database |
| `check_doc_config.py` | Runs the configuration block `docs/agent/00-cheatsheet.md` gives, and asserts it logs what the page promises |
| `setup_agent_redirect.py` | Writes the local, untracked redirect a coding agent looks for |
| `setup_agent_memory.py` | Writes the local memory files an agent keeps between sessions |

### Running them

Every one takes `--help`. From the repository root:

```bash
python3 tools/agent/check_agent_docs.py            # seconds, no build needed
python3 tools/agent/check_symbols.py
python3 tools/agent/check_contract.py <project> --strict
python3 tools/agent/check_mutations.py --static
python3 tools/agent/check_corpus.py --verbose

cmake -B build -DAREG_OUTPUT_LAYOUT=OFF && cmake --build build -j
python3 tools/agent/check_recipes.py --lib build/bin
python3 tools/agent/check_observability.py --lib build/bin
```

The first group needs no compiler. The second builds every recipe, so it takes
minutes rather than seconds. `.github/workflows/agent-docs.yml` runs all of them.

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

| Parameter                   | Description                                                                     |
| --------------------------- | ------------------------------------------------------------------------------- |
| `<areg-sdk-root>`           | Root directory of the Areg SDK source                                           |
| `<project-root>`            | Root directory of your application. **Absolute**                                |
| `<path-to-document>`        | Path to the `.siml`, `.fsml` or `.dtml` document. Absolute, or relative to `--root` |
| `<path-to-output>`          | The generate root, one folder for all generated code. Absolute, or relative to `--root` |

Inside the generate root, each document gets the folder its own path names. A document at
`src/services/HelloService.siml` with `--target=generated` is written to `generated/src/services`,
with the public headers there and the matching sources in a `private` subfolder. The generate root
is what belongs on the compiler's include path.

#### Example

```bash
java -jar ~/areg-sdk/tools/codegen.jar   \
    --root=/home/dev/my_project          \
    --doc=src/services/HelloService.siml \
    --target=generated
```

This writes the generated headers and sources into `/home/dev/my_project/generated/src/services`.

> [!TIP]
> A document that **includes** another one is generated together with it. Name the `.siml` or the
> `.fsml` and the `.dtml` it includes comes with it, into the folder that `.dtml`'s own path names --
> so two service interfaces sharing one data type document produce a single copy of it.

Ready-made templates for both platforms are in this directory: `codegenerate.sh` and
`codegenerate.bat`. Copy one into your project, set `AREG_SDK_ROOT` and `DOCUMENT`, and run it.

---

## 2. Document Schemas and the Rule Registry

`tools/schema/` holds the definitions that the code generator and **Lusan** both read, so that
the two agree on what a document may contain and on how a mistake in it is reported.

| File | Contents |
|------|----------|
| `siml.xsd` | Grammar of a service interface document (`.siml`) |
| `fsml.xsd` | Grammar of a state machine document (`.fsml`) |
| `dtml.xsd` | Grammar of a data type document (`.dtml`) |
| `datatype.xml` | The predefined data types every document may use without declaring them |
| `rules.xml` | The registry of validation rule numbers |

### Validating a document before generating

Any XML validator accepts these schemas, which is the quickest way to find a malformed document
without running the generator:

```bash
xmllint --noout --schema tools/schema/siml.xsd src/services/HelloService.siml
```

Editors that support XML catalogs can be pointed at the same files to get completion and
validation while a document is written. Lusan does this for you.

### `rules.xml`

A validation rule is identified by its **number together with the severity band** it is reported
in: an error keeps the bare number, a warning adds 100, information adds 200. So `4`, `104` and
`204` are three unrelated rules, and a number taken in one band is still free in another.

Lusan generates its rule constants from this file and the code generator generates its own, and
each build compares what it generated against what is checked in. A rule is therefore added by
adding a row here first. Removing a row fails a build instead of quietly dropping a check.

---

## 3. Using `.siml` Files in CMake (Recommended)

Instead of running the generator manually, Areg recommends **using CMake integration**.

### Add Service Interface in `CMakeLists.txt`

```cmake
addServiceInterface(gen_my_project src/services/HelloService.siml)
```

The path is relative to `${PROJECT_SOURCE_DIR}`; an absolute one works too.

This:

* Runs the code generator automatically
* Produces a **static library** (`gen_my_project`)
* Ensures consistency and reproducibility across builds

### State Machines and Data Types

The `.fsml` and `.dtml` documents have the same call shapes:

```cmake
addStateMachine(gen_my_project src/fsm/TrafficLight.fsml)
addDataType(gen_my_project     src/common/SharedTypes.dtml)
```

> [!IMPORTANT]
> A `.dtml` that a `.siml` or a `.fsml` **includes** must not be listed separately: the generator
> produces it together with the document that includes it. Call `addDataType` only for a data type
> document that nothing includes.

Each of the three has an `Ex` form taking an explicit source root, for sources that do not live under
`${PROJECT_SOURCE_DIR}`:

```cmake
addServiceInterfaceEx(gen_my_project "/home/dev/shared" "services/HelloService.siml")
```

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

## 4. Best Practices for Generated Code

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

## 5. Project Setup Scripts (Quick Start)

Areg provides scripts to bootstrap a **working example project** in one step.

### Available Scripts

| Script | Needs | Use it when |
| --- | --- | --- |
| `setup-project.sh` (Linux/macOS) | nothing | you are at a terminal; it asks three questions |
| `setup-project.bat` (Windows) | nothing | the same, on Windows |
| `agent/setup_project.py` | Python 3 | you are scripting it, in CI or from a coding agent |

The shell and batch scripts need no interpreter beyond the shell you are already in,
so they add no requirement to the C++ compiler, CMake and Java the SDK already asks
for. They prompt, so they cannot run unattended.

`agent/setup_project.py` takes its answers on the command line instead. It copies one
of the recipes in `docs/agent/recipes/`, renames it, and writes an `AGENTS.md` into
the new project.

```bash
python3 tools/agent/setup_project.py --name myapp --root ~/myapp --mode local
python3 tools/agent/setup_project.py --name myapp --root ~/myapp --mode ipc --sdk-root /opt/areg-sdk
```

| Option | Meaning |
|---|---|
| `--mode local` | One process, provider and consumer in two threads |
| `--mode ipc` | Two processes; needs `mtrouter` |
| `--mode pubsub` | Attributes and broadcasts in one process |
| `--sdk-root` | Build against a local SDK copy instead of fetching from GitHub |
| `--tag` | The SDK tag to fetch |
| `--force` | Overwrite an existing directory |
| `--no-agents` | Do not write `AGENTS.md` |


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

## 6. Building the Generated Project

After project creation:

```bash
cd <project-root>
cmake -B build
cmake --build build
```

This produces a **fully functional Areg RPC example**.

---

## 7. Examples and Advanced Usage

* **RPC (single process)**: Generated by setup scripts
* **IPC (multi-process)**: See `examples/` folder in the Areg SDK root
* **Custom build flows**: Inspect `conf/cmake/` for advanced control

---

## 8. Sanitizers and Profilers

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

## 9. Running the Examples as a Test Suite

The unit tests cover the framework in isolation. The examples cover it as it is actually used:
several processes started together, a message router between them, and a result that depends on
all of them behaving. `run-all-examples.py` runs them as **scenarios** -- the processes that
belong together are started together, the driving process is given a deadline, and a scenario
passes only if every process ends the way it is supposed to end.

```bash
tools/run-all-examples.sh                     # Linux, macOS
tools\run-all-examples.bat                    # Windows
python3 tools/run-all-examples.py --list      # what would run, without running it
```

Both wrappers pass every argument through to the driver.

Without `--bin-dir` the driver searches `build/bin` and every other CMake build tree in
the repository, plus `product/build/<compiler>/<config>/bin`, keeps only the directories
holding binaries the current platform can start, and takes the most recently built one.

| Option | Default | Meaning |
|--------|---------|---------|
| `--tier smoke\|ipc\|all\|perf` | `all` | `smoke` is single process only, `ipc` needs the router |
| `--bin-dir DIR` | `$AREG_BIN_DIR` | The build to test. Always pass it on a build server |
| `--only NAME[,NAME...]` | all | Run only the named scenarios |
| `--repeat N` | `1` | Run the selection N times and report every failure |
| `--timeout SEC` | `120` | Per-scenario deadline |
| `--perf` | off | Append the benchmarks and print the measured rates |
| `--no-pty` | off | Capture into a log file instead of a terminal |
| `--junit FILE` | none | Write the results as a JUnit report |
| `--out-dir DIR` | derived | Where the captured output is written |

Exit code 0 means every scenario passed.

```bash
# A single flaky scenario, twenty times, to see how often it really fails:
tools/run-all-examples.sh --only 12_svcmulti --repeat 20

# Everything, against an explicitly chosen build:
tools/run-all-examples.sh --bin-dir product/build/gnu-g++/linux-64-x86_64-release-shared/bin
```

> [!IMPORTANT]
> The `ipc` and `all` tiers start and stop the message router themselves. Do not leave your own
> `mtrouter` running while the suite runs: the examples would connect to yours instead, and the
> run would measure and prove nothing.

### `report-ctest.py`

The job log of a build server needs a credential to read; its annotations do not. After a failed
`ctest` run this reads the JUnit report (or `Testing/Temporary/LastTest.log` when there is none)
and republishes the cases that did not pass as annotations, so a red matrix entry is readable in
a browser.

```bash
python3 tools/report-ctest.py <ctest build directory> [--title ctest]
```

It never fails the job: a missing file, an unreadable file and a file without a single failure
are all reported and none of them is an error. `.github/workflows/cmake.yml` calls it.

---

## 10. Measuring Throughput and Latency

Examples 23, 30 and 31 are the benchmarks of the SDK:

| Example | Measures |
|---------|----------|
| `23_pubdatarate` | Peak network send rate and peak block rate, while the channel count is ramped up |
| `30_publatency` | One way trip and round trip time between **two processes**, through the message router |
| `31_loclatency` | The same trips inside **one process**: provider and consumer in one thread, then in two |

The difference between the two topologies of example 31 is the price of one thread wake-up; the
difference to example 30 is the price of leaving the process.

### `areg_benchmarks.py`

A module, not a script. The benchmarks print into a full screen console that repaints itself in
place, so the captured output is a stream of ANSI escapes with the same lines rewritten hundreds
of times. This turns that stream into a table of numbers. `run-all-examples.py --perf` imports
it; there is nothing to run by hand.

### `latency/` -- unattended measurement

| Script | Platform | What it does |
|--------|----------|--------------|
| `run-local-latency.sh` | Linux, macOS | Example 31 in both topologies, all modes, into one CSV and one log |
| `run-pub-latency.py` | Linux, macOS | Example 30 through the router; starts and stops the router itself |
| `run-latency-trace.sh` | Linux, macOS | A per-stage trace build, to see where the time inside a trip goes |
| `run-win-abba.py` | Windows | Runs two builds alternately in ABBA order and compares them |
| `win_latency_trace.ps1` | Windows | The Windows counterpart of `run-latency-trace.sh` |

```bash
tools/latency/run-local-latency.sh --repeat 3 --label baseline
python3 tools/latency/run-pub-latency.py --modes pp0,pp64 --count 20000
```

Results are written to `product/tasks/measurements`, which is not under version control.

### The per-stage trace

Building the framework with `AREG_LATENCY_TRACE=1` turns on the instrumentation in
`framework/areg/base/private/DebugDefs.hpp`. Every instrumented stage accumulates count, minimum,
mean and maximum, and the table is printed to standard error when the process exits:

| Stage | What it times |
|-------|---------------|
| `SendNode` | The send thread: drain, serialize and write the batch |
| `SendSyscall` | The send thread: only the write syscall |
| `RecvNode` | The receive thread: deserialize and route one message |
| `CompDispatch` | The dispatcher: the component's own handler |
| `MpscHandoff` | The event queue, from enqueue to dequeue: handing a message to another thread, wake-up included |

Instrument a new stage by adding an entry to `areg::LtStage` before `Count` and placing an
`AREG_LT_SCOPE(areg::LtStage::YourStage)` in the scope to be timed. The macros compile to nothing
when the option is off.

> [!WARNING]
> Three rules, or the numbers are worthless.
> 1. The table is printed by `std::atexit`. It appears **only** when the process ends through its
>    own exit path. Never kill the processes -- quit them with their console `-q` command.
> 2. The instrumentation itself costs time. A traced build is slower than a normal one. Read the
>    **share** of each stage, never its absolute value, and never mix the two builds in one table.
> 3. Measure on a quiet machine. A build, a browser or a second benchmark running at the same time
>    changes the result more than most of the changes worth measuring.

---

## 11. Source Hygiene: `check-ascii.py`

Two different checks live in this one tool. Both exit 0 when nothing was found and 1 when
something was, so either can be used as a gate.

### The ASCII rule

Every source, script, CMake and configuration file of the SDK must contain only bytes `0x00-0x7F`.
Documents (`.md`, `.txt`) are exempt.

```bash
python3 tools/check-ascii.py                     # the whole repository
python3 tools/check-ascii.py framework examples  # only these paths
python3 tools/check-ascii.py --staged            # only what git has staged
```

The reason is the compilers, not taste. A source file without a byte order mark is read in the
**code page of the machine that compiles it**. The same UTF-8 dash is therefore one thing on your
machine and something else on a build server or on a user's machine in another locale -- silently,
with no diagnostic, and inside a string literal that difference reaches the output of the program.

Files whose content is character data are waived, because there the non ASCII bytes are the
subject and not a defect. The list is `ASCII_WAIVED` at the top of the script and it is short on
purpose:

```
framework/areg/base/TableDefs.hpp
tests/units/String*Test.cpp
tests/units/*Encod*Test.cpp
```

### The byte hunt

`--find` reports every occurrence of the byte values given, whatever they are, and ignores the
waiver list -- a control character is unwanted in a character test as much as anywhere else.

```bash
python3 tools/check-ascii.py --find control      # every C0 control but TAB, LF, CR, plus DEL
python3 tools/check-ascii.py --find 0x08         # one value
python3 tools/check-ascii.py --find 0x01-0x12    # a range
python3 tools/check-ascii.py --find 0x08,0x1B,0x7F
python3 tools/check-ascii.py --find control --docs   # documents searched too
```

Values are hexadecimal. `control` and `non-ascii` are accepted as names.

This exists for the control characters that arrive through a text pipeline rather than through a
keyboard. `\brief` written into a context that expands escape sequences becomes byte `0x08`
followed by `rief`; the file still compiles, the comment still looks right in most editors, and
Doxygen silently drops the command. The report prints the offending line with every unprintable
byte written out, which makes the damage obvious:

```
framework/areg/base/Sample.hpp:2:4: 0x08 (BS)
     * <0x08>rief   Does the thing.
```

### As a pre-commit hook

`--staged` checks only what is about to be committed, which is the fast path -- a full walk of the
repository is dominated by the file system, not by the check.

```bash
cat > .git/hooks/pre-commit <<'EOF'
#!/bin/sh
python3 tools/check-ascii.py --staged --quiet || exit 1
python3 tools/check-ascii.py --staged --quiet --find control || exit 1
EOF
chmod +x .git/hooks/pre-commit
```

---

## 12. Debugging a Rare Crash

`hunt-crash.py` is for the defect that appears once in a few dozen runs. A core file is not always
available -- `ptrace` is restricted in containers, macOS writes crash reports instead of cores, and
WSL often writes neither -- so the program is started **under the debugger from the beginning** and
the debugger is asked for the stacks at the moment it stops.

```bash
python3 tools/hunt-crash.py --runs 200 14_locmesh.elf
python3 tools/hunt-crash.py --bin-dir <dir> --runs 50 --timeout 120 22_pubservice.elf
python3 tools/hunt-crash.py --runs 100 --out crash.txt 30_pubprovider.elf -- --some-arg
```

| Option | Default | Meaning |
|--------|---------|---------|
| `--runs N` | `100` | How many times to try |
| `--timeout SEC` | `120` | Deadline for one run |
| `--bin-dir DIR` | newest build | Where the executable is |
| `--out FILE` | `crash-stacks.txt` | Where the stacks are written |

It uses `gdb` on Linux and cygwin and `lldb` on macOS, both in batch mode, so nothing is
interactive and it can be left running. Exit status is 0 when a crash was captured, 1 when the runs
finished without one, and 2 when the tool could not run at all.

Stops on the first fatal signal: `SIGSEGV`, `SIGABRT`, `SIGBUS`, `SIGILL`, `SIGFPE` and, on macOS,
`EXC_BAD_ACCESS`. A hang is not a crash -- for that, run the scenario under
`tools/run-all-examples.py --repeat N`, which photographs a process it has to kill.

> [!TIP]
> Build with `-DCMAKE_BUILD_TYPE=RelWithDebInfo` before hunting. A Debug build changes the timing
> enough that a race often stops reproducing, and a plain Release build has no symbols to print.
> If the defect is a memory error rather than a race, `tools/sanitize.sh asan` finds it in one run
> instead of fifty.

---

## 13. Summary

Building an application with the SDK:

* Use `.siml` to define services
* Prefer **CMake integration** over manual code generation
* Keep generated code in **static libraries** for modularity
* Use setup scripts to get started instantly
* Leverage **Lusan** for visual service interface design and logging
* Follow examples for real-world IPC patterns

Working on the SDK itself:

* `run-all-examples.sh` after every build -- it covers what the unit tests cannot
* `sanitize.sh asan` before trusting a change that touches memory or lifetime
* `check-ascii.py --staged` from a pre-commit hook, both checks
* `hunt-crash.py` for a defect that reproduces rarely, `sanitize.sh` for one that reproduces
* Measure with `latency/` on a quiet machine, and compare shares rather than absolute numbers

This workflow scales from a **hello-service example** to **full distributed production systems**.
