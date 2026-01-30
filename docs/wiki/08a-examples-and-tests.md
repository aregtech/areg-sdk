# Building and Running Areg SDK Examples and Unit Tests

The Areg SDK provides a comprehensive set of examples and unit tests to demonstrate its features and ensure reliability. This guide explains how to include or exclude examples and unit tests during the build process, as well as how to execute them after building.

---

## Table of Contents

- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
- [Configuring the Build Process](#configuring-the-build-process)
- [Building the Areg SDK](#building-the-areg-sdk)
- [Running Areg SDK Examples](#running-areg-sdk-examples)
- [Running Areg SDK Google Unit Tests](#running-areg-sdk-google-unit-tests)
- [Running Areg SDK Tests with CTest](#running-areg-sdk-tests-with-ctest)
- [Try Edge AI Demo](#try-edge-ai-demo)
- [Summary](#summary)

---

## Introduction

By default, the Areg SDK build process includes examples and unit tests to facilitate learning and testing. Developers can exclude these components to streamline the build process or focus solely on the SDK's core functionality.

This document provides step-by-step instructions for configuring, building, and running the examples and unit tests of the Areg SDK.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Prerequisites

Before proceeding, ensure the following requirements are met:

| Requirement | Version | Notes |
|-------------|---------|-------|
| **Areg SDK Source Code** | Latest | Clone from [GitHub repository](https://github.com/aregtech/areg-sdk) |
| **CMake** | 3.20 or newer | Build system generator |
| **Java** | 17 or newer | Required for code generation |
| **C++ Compiler** | C++17 compatible | GCC, Clang, MSVC, or MinGW |
| **Build Environment** | Terminal/Command Prompt | With necessary development tools |

> [!NOTE]
> **Visual Studio Users**: The Areg SDK examples and unit tests are included in the `areg-sdk.sln` solution file and are built automatically when you build the solution.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Configuring the Build Process

### Including Examples and Unit Tests

To include **examples** and **unit tests** in the build process, use the default configuration. No additional options are required:

```bash
cmake -B ./build
cmake --build ./build -j
```

This configuration builds the Areg SDK core along with all examples and unit tests.

### Excluding Examples and Unit Tests

To exclude specific components, use the following CMake options:

| Option | Description | Default |
|--------|-------------|---------|
| `AREG_BUILD_TESTS` | Controls inclusion of unit tests | `ON` |
| `AREG_BUILD_EXAMPLES` | Controls inclusion of examples | `ON` |

Set these options to `OFF` to exclude them from the build process.

**Example 1: Exclude Both Unit Tests and Examples**

```bash
cmake -B ./build \
  -DAREG_BUILD_TESTS=OFF \
  -DAREG_BUILD_EXAMPLES=OFF

cmake --build ./build -j
```

**Example 2: Include Examples Only (Exclude Tests)**

```bash
cmake -B ./build \
  -DAREG_BUILD_TESTS=OFF \
  -DAREG_BUILD_EXAMPLES=ON

cmake --build ./build -j
```

**Example 3: Include Tests Only (Exclude Examples)**

```bash
cmake -B ./build \
  -DAREG_BUILD_TESTS=ON \
  -DAREG_BUILD_EXAMPLES=OFF

cmake --build ./build -j
```

### Benefits of Excluding Components

**Faster Build Times**
- Reduced compilation time for production builds
- Smaller build output directory

**Production Deployment**
- Minimal binary footprint
- Only core SDK libraries included

**CI/CD Optimization**
- Separate test and production build pipelines
- Parallel build configuration testing

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Building the Areg SDK

After configuring the build process, proceed with the following steps:

### Step 1: Generate Build Files

Run the `cmake` command with appropriate options.

**Example: Build with Clang, Excluding Tests but Including Examples**

```bash
cmake -B ./build \
  -DAREG_COMPILER_FAMILY=llvm \
  -DAREG_BUILD_TESTS=OFF \
  -DAREG_BUILD_EXAMPLES=ON
```

**Example: Build with GCC in Release Mode**

```bash
cmake -B ./build \
  -DCMAKE_BUILD_TYPE=Release \
  -DAREG_BUILD_TESTS=ON \
  -DAREG_BUILD_EXAMPLES=ON
```

### Step 2: Build the Project

Execute the build command:

```bash
cmake --build ./build -j
```

**Alternative: Specify Number of Parallel Jobs**

```bash
cmake --build ./build -j8
```

### Build Output Location

The binaries and artifacts will be available in the `./product/build/` directory structure:

```
product/
└── build/
    ├── <compiler>-<processor>-<config>/
    │   └── bin/           # Executables and libraries
    │       ├── areg        # Core library
    │       ├── aregextend  # Extended library
    │       ├── examples/   # Example binaries
    │       └── tests/      # Unit test binaries
    └── obj/               # Object files (intermediate)
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Running Areg SDK Examples

To execute examples:

### Step 1: Navigate to Binary Directory

Navigate to the `bin` directory inside `./product/build/`:

```bash
cd ./product/build/<compiler>-<processor>-<config>/bin
```

**Example**:
```bash
cd ./product/build/gnu-x86_64-release/bin
```

### Step 2: Locate and Run Example Binary

Locate the desired example binary and run it:

```bash
./00_helloservice
```

### Important Notes for Examples

**Example Documentation**
- Example sources are located in the [`./examples`](./../../examples/) directory
- Detailed descriptions available in [Examples README](./../../examples/README.md)

**IPC Examples Requirement**

Projects utilizing Inter-Process Communication (IPC) require the **mtrouter** service for process communication.

**Steps to Run IPC Examples**:

1. **Start Message Router** (in separate terminal):
   ```bash
   cd ./product/build/<compiler>-<processor>-<config>/bin
   ./mtrouter
   ```

2. **Run IPC Example** (in another terminal):
   ```bash
   cd ./product/build/<compiler>-<processor>-<config>/bin
   ./12_pubsvc
   ```

> [!NOTE]
> IPC examples are categorized in the `Multitasking` section of each example description. Look for examples that demonstrate inter-process communication.

**Generated Projects**

Projects like `xx_generate` are created from **Service Interface** document files (`.siml`):

- **CMake Builds**: Files are generated during CMake configuration
- **Visual Studio Builds**: Generation occurs as a pre-build event of the `dummy` project

**Platform-Specific Examples**

**MFC-Based Projects**:
- The [17_winchat](./../../examples/17_winchat/) project builds only on Windows
- Requires Microsoft Compilers (MSVC or ClangCL)
- Requires Microsoft Foundation Classes (MFC)

### Example Categories

Examples are organized by complexity and features:

| Category | Description | Examples |
|----------|-------------|----------|
| **Basic** | Simple service communication | 00_helloservice, 01_hello |
| **Multithreading** | Thread-based communication | 10_locsvc, 11_locmesh |
| **Multitasking (IPC)** | Process-based communication | 12_pubsvc, 13_pubmesh |
| **Advanced** | Complex scenarios | 16_pubfsm, 17_winchat |

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Running Areg SDK Google Unit Tests

To execute unit tests:

### Step 1: Navigate to Binary Directory

Navigate to the `bin` directory:

```bash
cd ./product/build/<compiler>-<processor>-<config>/bin
```

### Step 2: Locate Unit Test Binary

Locate the `areg-unit-tests` binary.

### Step 3: Run Unit Tests

**Run All Tests**:

```bash
./areg-unit-tests
```

**Run Specific Test Suite**:

Use filters to run specific tests or suites. For example, to run only `DateTimeTest` tests:

```bash
./areg-unit-tests --gtest_filter=*DateTimeTest*
```

**Run Multiple Test Suites**:

```bash
./areg-unit-tests --gtest_filter=*DateTimeTest*:*StringTest*
```

**Exclude Specific Tests**:

```bash
./areg-unit-tests --gtest_filter=-*SlowTest*
```

### Successful Test Output Example

```plaintext
Running main() from <areg-sdk-root>/build/packages/googletest-src/googletest/src/gtest_main.cc
Note: Google Test filter = *DateTimeTest*
[==========] Running 4 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 4 tests from DateTimeTest
[ RUN      ] DateTimeTest.TestNow
[       OK ] DateTimeTest.TestNow (0 ms)
[ RUN      ] DateTimeTest.TestLocalTimeWin32
[       OK ] DateTimeTest.TestLocalTimeWin32 (0 ms)
[ RUN      ] DateTimeTest.TestLocalTime
[       OK ] DateTimeTest.TestLocalTime (0 ms)
[ RUN      ] DateTimeTest.TestFormatISO8601
[       OK ] DateTimeTest.TestFormatISO8601 (0 ms)
[----------] 4 tests from DateTimeTest (0 ms total)

[----------] Global test environment tear-down
[==========] 4 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 4 tests.
```

### Additional Google Test Options

**Verbose Output**:
```bash
./areg-unit-tests --gtest_print_time=1
```

**Repeat Tests**:
```bash
./areg-unit-tests --gtest_repeat=3
```

**Shuffle Tests**:
```bash
./areg-unit-tests --gtest_shuffle
```

**List All Tests**:
```bash
./areg-unit-tests --gtest_list_tests
```

For additional options, refer to [Google Test Documentation](https://google.github.io/googletest/advanced.html#running-test-programs-advanced-options).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Running Areg SDK Tests with CTest

In addition to running tests directly through the test binaries, you can use `ctest` or `make test` for an organized and versatile testing experience. These tools simplify the process of running specific tests or suites and provide structured output for analysis.

### Running Tests with `ctest`

The `ctest` tool allows you to execute tests directly from the build directory.

**Run All Tests**:

```bash
ctest --test-dir ./build
```

**Run Specific Test Suite**:

For instance, to run only the `DateTimeTest` suite:

```bash
ctest --test-dir ./build -R DateTimeTest
```

**Verbose Output**:

```bash
ctest --test-dir ./build -V
```

**Run Tests in Parallel**:

```bash
ctest --test-dir ./build -j8
```

### Example of Successful Test Output

```plaintext
Internal ctest changing into directory: <areg-sdk-root>/build
<areg-sdk-root>/build
    Start 3: DateTimeTest.TestNow
1/5 Test #3: DateTimeTest.TestNow ..............   Passed    0.41 sec
    Start 4: DateTimeTest.TestOperators
2/5 Test #4: DateTimeTest.TestOperators ........   Passed    0.52 sec
    Start 5: DateTimeTest.TestLocalTimeWin32
3/5 Test #5: DateTimeTest.TestLocalTimeWin32 ...   Passed    0.47 sec
    Start 6: DateTimeTest.TestLocalTime
4/5 Test #6: DateTimeTest.TestLocalTime ........   Passed    0.54 sec
    Start 7: DateTimeTest.TestFormatISO8601
5/5 Test #7: DateTimeTest.TestFormatISO8601 ....   Passed    0.44 sec

100% tests passed, 0 tests failed out of 5

Total Test time (real) =   2.84 sec
```

### Running Tests with `make test`

Alternatively, if using a `make`-based build system, you can run tests with the `make test` command.

**Run All Tests**:

```bash
cd ./build
make test
```

**Run Specific Test Suite**:

To execute a specific test suite like `DateTimeTest`:

```bash
cd ./build
make test ARGS="-R DateTimeTest"
```

**Verbose Output**:

```bash
make test ARGS="-V"
```

### CTest Command-Line Options

| Option | Description |
|--------|-------------|
| `-R <regex>` | Run tests matching regular expression |
| `-E <regex>` | Exclude tests matching regular expression |
| `-V` | Verbose output |
| `-VV` | Very verbose output (shows test command) |
| `-j <N>` | Run tests in parallel (N jobs) |
| `--output-on-failure` | Show test output only for failed tests |
| `--rerun-failed` | Rerun only tests that failed in previous run |

### Benefits of Using CTest

**Organized Testing**
- Structured test execution and reporting
- Clear pass/fail status for each test

**Flexibility**
- Run all tests or specific subsets
- Regular expression filtering
- Parallel test execution

**Integration**
- Works seamlessly with CMake build system
- Standard tool for CMake-based projects
- CI/CD pipeline integration

For additional options, refer to [CTest Documentation](https://cmake.org/cmake/help/latest/manual/ctest.1.html).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Try Edge AI Demo

Experience Areg SDK's power with a real LLM inference system that demonstrates practical distributed application architecture.

### Clone and Setup

```bash
# Clone the Edge AI demo
git clone https://github.com/aregtech/areg-edgeai.git
cd areg-edgeai

# Follow setup instructions in README.md
```

### What the Demo Demonstrates

This demo showcases how Areg SDK orchestrates distributed AI services:

**LLM Inference Service**
- Integration with Llama.cpp for local LLM inference
- Centralized AI processing service

**Multiple Concurrent Clients**
- Multiple clients send prompts simultaneously
- Clients can join or leave at any time
- Dynamic client management

**Automatic Request Handling**
- Automatic request queuing and prioritization
- Response routing to correct client
- No manual threading or synchronization code required

**Zero Manual Synchronization**
- Service Provider-Consumer model handles all synchronization
- No mutex, semaphore, or condition variable code
- Thread-safe by design

### Use Case Overview

**Scenario**: A single AI agent processes text requests from multiple clients connected over a local network.

**Features**:
- Clients may join or leave dynamically
- AI agent accepts requests when online
- Requests queued internally
- Responses returned to correct client without mixing results

**Areg SDK Advantages**:
- Automatic message routing
- Built-in request queuing
- Type-safe communication
- No manual thread management

For complete setup, build instructions, and additional use cases that Areg SDK can manage, see the [areg-edgeai documentation](https://github.com/aregtech/areg-edgeai).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Summary

This guide covered building and running Areg SDK examples and unit tests:

### Key Points

- ✅ **Default Build**: Examples and unit tests included by default
- ✅ **Configurable**: Use `AREG_BUILD_TESTS` and `AREG_BUILD_EXAMPLES` to control inclusion
- ✅ **Multiple Test Methods**: Direct execution, Google Test filters, or CTest
- ✅ **IPC Examples**: Require `mtrouter` service to be running
- ✅ **Platform Support**: Cross-platform with some platform-specific examples

### Build Configuration Options

| Configuration | Command | Use Case |
|---------------|---------|----------|
| **Full Build** | `cmake -B ./build` | Development and testing |
| **Core Only** | `cmake -B ./build -DAREG_BUILD_TESTS=OFF -DAREG_BUILD_EXAMPLES=OFF` | Production deployment |
| **Examples Only** | `cmake -B ./build -DAREG_BUILD_TESTS=OFF` | Learning and demonstrations |
| **Tests Only** | `cmake -B ./build -DAREG_BUILD_EXAMPLES=OFF` | CI/CD testing pipelines |

### Testing Options

| Method | Command | Best For |
|--------|---------|----------|
| **Direct Execution** | `./areg-unit-tests` | Development, debugging |
| **Google Test Filter** | `./areg-unit-tests --gtest_filter=*TestName*` | Specific test debugging |
| **CTest** | `ctest --test-dir ./build` | CI/CD, automation |
| **Make Test** | `make test` | Make-based workflows |

### Next Steps

**Learn by Example**:
- Explore examples in [`./examples`](./../../examples/) directory
- Read [Examples README](./../../examples/README.md) for detailed descriptions
- Start with basic examples (00_helloservice) and progress to advanced ones

**Test and Validate**:
- Run unit tests to verify SDK functionality
- Use Google Test filters to focus on specific areas
- Integrate CTest into CI/CD pipelines

**Build Real Applications**:
- Try the [Edge AI Demo](https://github.com/aregtech/areg-edgeai) for practical example
- Review [Areg SDK Demo repository](https://github.com/aregtech/areg-sdk-demo)
- Design your own distributed services

### Additional Resources

- [Areg SDK Examples Documentation](./../../examples/README.md)
- [Google Test Documentation](https://google.github.io/googletest/advanced.html)
- [CTest Documentation](https://cmake.org/cmake/help/latest/manual/ctest.1.html)
- [Edge AI Demo Repository](https://github.com/aregtech/areg-edgeai)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
