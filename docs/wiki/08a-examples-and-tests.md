# Building and Running AREG SDK Examples and Unit Tests

The AREG SDK provides a comprehensive set of examples and unit tests to demonstrate its features and ensure reliability. This guide explains how to include or exclude Examples and Unit Tests during the build process, as well as how to execute them after building.

---

## Table of Contents

- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
- [Configuring the Build Process](#configuring-the-build-process)
  - [Including Examples and Unit Tests](#including-examples-and-unit-tests)
  - [Excluding Examples and Unit Tests](#excluding-examples-and-unit-tests)
- [Building the AREG SDK](#building-the-areg-sdk)
- [Running AREG SDK Examples](#running-areg-sdk-examples)
- [Running AREG SDK Google Unit Tests](#running-areg-sdk-google-unit-tests)
- [Running AREG SDK Tests with CTest](#running-areg-sdk-tests-with-ctest)
  - [Running Tests with `ctest`](#running-tests-with-ctest)
  - [Running Tests with `make test`](#running-tests-with-make-test)
- [Try Edge AI Demo](#try-edge-ai-demo)
- [Notes](#notes)

---

## Introduction

By default, the AREG SDK build process includes Examples and Unit Tests to facilitate learning and testing.
Developers can exclude these components to streamline the build process or focus solely on the SDK's core functionality.
This document provides step-by-step instructions for configuring, building, and running the Examples and Unit Tests of the AREG SDK.

---

## Prerequisites

Before proceeding, ensure the following requirements are met:

- **AREG SDK Source Code**: Ensure the source code is available on your system.
- **CMake**: Version 3.20 or newer.
- **Java**: Version 17 or newer.
- **C++ Compiler**: Supported options include GCC, Clang, or MSVC.
- **Build Environment**: Set up a terminal or command prompt with the necessary tools.

> [!NOTE]
> The AREG SDK Examples and Unit Tests are included in the `areg-sdk.sln` Solution file of the Microsoft Visual Studio and are built automatically.

---

## Configuring the Build Process

### Including Examples and Unit Tests

To include **Examples** and **Unit Tests** in the build process, use the default configuration. No additional options are required:

```bash
cmake -B ./build
cmake --build ./build
```

This configuration builds the AREG SDK core along with all Examples and Unit Tests.

### Excluding Examples and Unit Tests

To exclude specific components, use the following CMake options:

- `AREG_BUILD_TESTS`: Controls the inclusion of Unit Tests.
- `AREG_BUILD_EXAMPLES`: Controls the inclusion of Examples.

Set these options to `OFF` to exclude them from the build process.

**Example**: Exclude both Unit Tests and Examples:

```bash
cmake -B ./build -DAREG_BUILD_TESTS=OFF -DAREG_BUILD_EXAMPLES=OFF
cmake --build ./build
```

---

## Building the AREG SDK

After configuring the build process, proceed as follows:

1. **Generate Build Files**: Run the `cmake` command with appropriate options.
   
   **Example**: Build the SDK with Clang, excluding Unit Tests but including Examples:
   ```bash
   cmake -B ./build -DAREG_COMPILER_FAMILY=llvm -DAREG_BUILD_TESTS=OFF -DAREG_BUILD_EXAMPLES=ON
   ```

2. **Build the Project**: Execute the build command:
   ```bash
   cmake --build ./build
   ```

The binaries and artifacts will be available in the `./product/build/...` directory structure.

---

## Running AREG SDK Examples

To execute Examples:

1. Navigate to the `bin` directory inside `./product/build/...`.
2. Locate the desired example binary.
3. Run the binary from the terminal.

**Key Notes**:
- Example sources are located in the [`./examples`](./../../examples/) directory, with descriptions in the [README](./../../examples/README.md).
- **IPC Examples**: Projects utilizing Inter-Process Communication (IPC) require the **mtrouter** service for process communication. Run **mtrouter** from the build directory before testing. These **IPC** projects are categorized in the `Multitasking` section of each example description.
- **Generated Projects**: Projects like `xx_generate` are created from **Service Interface** document files (`.siml`). For CMake builds, these files are generated during configuration. In Visual Studio, generation occurs as a pre-build event of the `dummy` project.
- **MFC-Based Projects**: The [17_winchat](./17_winchat/) project builds only on Windows using Microsoft Compilers (*MSVC* or *ClangCL*) and requires *Microsoft Foundation Classes (MFC)*.

---

## Running AREG SDK Google Unit Tests

To execute Unit Tests:

1. Navigate to the `bin` directory inside `./product/build/...`.
2. Locate the `areg-unit-tests` binary.
3. Run the binary:

**Example**:
```bash
./areg-unit-tests
```

**Filtering Tests**: Run specific tests or suites using filters. For example, to run only `DateTimeTest` tests:
```bash
./areg-unit-tests --gtest_filter=*DateTimeTest*
```

**Successful Test Output**:
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

For additional options, refer to [Google Test Documentation](https://google.github.io/googletest/advanced.html#running-test-programs-advanced-options).

---

## Running AREG SDK Tests with CTest

In addition to running tests directly through the test binaries, you can use `ctest` or `make test` for an organized and versatile testing experience. These tools simplify the process of running specific tests or suites and provide structured output for analysis.

### Running Tests with `ctest`

The `ctest` tool allows you to execute tests directly from the build directory. For instance, to run only the `DateTimeTest` suite, use the following command:

```bash
ctest --test-dir ./build -R DateTimeTest
```

**Example of Successful Test Output**

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

Alternatively, if using a `make`-based build system, you can run tests with the `make test` command. To execute a specific test suite like `DateTimeTest`, use the following:

```bash
cd ./build
make test ARGS="-R 'DateTimeTest'"
```

Both methods offer flexibility and provide a detailed report on test results, including pass/fail statuses and execution times. Choose the one that fits best with your workflow.

For additional options, refer to [CTest Documentation](https://cmake.org/cmake/help/latest/manual/ctest.1.html).

---

### Try Edge AI Demo

Experience Areg SDK's power with a real LLM inference system:
```bash
# Clone the Edge AI demo
git clone https://github.com/aregtech/areg-edgeai.git
cd areg-edgeai
# Follow setup in README.md
```

This demo shows how Areg SDK orchestrates:
- **LLM inference service** (Llama.cpp integration)
- **Multiple concurrent clients** sending prompts
- **Automatic request queuing** and response routing
- **Zero manual threading** or synchronization code

**Case:** A single AI agent processes text requests from multiple clients connected over a local network. Clients may join or leave at any time. When the AI agent is online, it accepts requests, queues them internally, and returns each response to the correct client without mixing results.

📄 See [areg-edgeai documentation](https://github.com/aregtech/areg-edgeai) for use cases that Areg SDK can manage, complete setup and build.

---

## Notes

- Examples and Unit Tests are enabled by default for a comprehensive development experience.
- Excluding these components reduces build time and output size, which is useful for production environments.
- Detailed descriptions of Examples and Test Cases are in the [AREG SDK Example Documentation](./../../examples/README.md).

---
