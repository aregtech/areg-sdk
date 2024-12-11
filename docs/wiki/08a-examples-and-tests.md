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
- [Running AREG SDK Unit Tests](#running-areg-sdk-unit-tests)
- [Notes](#notes)

---

## Introduction

By default, the AREG SDK build process includes Examples and Unit Tests to facilitate learning and testing. Developers can exclude these components to streamline the build process or focus solely on the SDK's core functionality. This document provides step-by-step instructions for configuring, building, and running the Examples and Unit Tests of the AREG SDK.

---

## Prerequisites

Before proceeding, ensure the following requirements are met:

- AREG SDK source code is available on your system.
- A compatible CMake version is installed (CMake 3.20 or newer).
- A compatible Java version is installed (Java 17 or newer).
- A C++ compiler (e.g., GCC, Clang, or MSVC) is installed and configured.
- A build environment is set up (e.g., a terminal or command prompt with necessary tools).

> [!NOTE]
> The example and unit tests Microsoft Visual Studio projects are included in the `areg-sdk.sln` Solution file and they are automaticall build.

---

## Configuring the Build Process

### Including Examples and Unit Tests

To include **Examples** and **Unit Tests** in the build process, use the default configuration. No additional options are required.

```bash
cmake -B ./build
cmake --build ./build
```

This configuration builds the core AREG SDK along with all Examples and Unit Tests.

### Excluding Examples and Unit Tests

To exclude specific components, use the following CMake options:

- `AREG_BUILD_TESTS`: Controls the inclusion of Unit Tests.
- `AREG_BUILD_EXAMPLES`: Controls the inclusion of Examples.

Set these options to `OFF` to exclude them from the build process.

**Example:** Exclude both Unit Tests and Examples:

```bash
cmake -B ./build -DAREG_BUILD_TESTS=OFF -DAREG_BUILD_EXAMPLES=OFF
cmake --build ./build
```

---

## Building the AREG SDK

Once the configuration is complete, initiate the build process using CMake. The following steps outline the process:

1. **Generate Build Files**: Run the `cmake` command with appropriate options to generate build files.
   
   **Example building sources with Clang compiler and examples, but without unit tests**:
   ```bash
   cmake -B ./build -DAREG_COMPILER_FAMILY=llvm -DAREG_BUILD_TESTS=OFF -DAREG_BUILD_EXAMPLES=ON
   ```

2. **Build the Project**: Execute the build command:
   ```bash
   cmake --build ./build
   ```

The `./product/build` directory will contain the compiled binaries and artifacts.

---

## Running AREG SDK Examples

To execute the Examples built with the SDK:

1. Navigate to the binary `bin` directory inside of `./product/build` directory tree.
2. Locate the desired example binary.
3. Run the binary from the terminal.

**Important Notes**:

- **Example Sources and Descriptions**: All sources of examples are located in the [`./examples`](./../../examples/) directory. The quick description of each example is in the [README](./../../examples/README.md) document inside `example` directory.
- **IPC Examples**: Projects that use **inter-process communication (IPC)** require the **mcrouter** service to facilitate communication between different processes. The classification of such projects is described in the **Multitasking** section of example description. Run **mcrouter** from the build directory for testing.
- **Generated projects:** All **xx_generate** projects are generated from **Service Interface** document files (`.siml`). For CMake build these projects and source files are generated during configuration. For Microsoft Visual Studio, these projects are predefined and the sources are generated as a pre-build event of `dummy` project, which exists in this `example` directory.
- **MFC based projects:** The project [17_winchat](./17_winchat/) is build only under Windows system with Microsoft Compilers (*MSVC* and *ClangCL*) and it requires *Microsoft Foundation Classes (MFC)*.

---

## Running AREG SDK Unit Tests

Unit Tests are executed to verify the functionality and reliability of the AREG SDK.

1. Navigate to the binary `bin` directory inside of `./product/build` directory tree.
2. Locate the test runner binary `areg-unit-tests`.
3. Execute the test runner from the terminal.

**Example:**
```bash
./areg-unit-tests
```

The test results, including pass/fail statuses, will be displayed in the terminal. For additional debugging, enable verbose mode.

You may as well filter the unit tests to run specific tests or test suite. For example, this command will run only `DateTimeTest` tests:
```bash
./areg-unit-tests.elf --gtest_filter=*DateTimeTest*
```

If the tests succeeded you should have following message on console:
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

For details of options to run unit tests see appropriate [Advanced Options](https://google.github.io/googletest/advanced.html#running-test-programs-advanced-options) topic of Google Tests document.

---

## Notes

- By default, Examples and Unit Tests are enabled in the build process to provide a comprehensive development experience.
- Excluding these components reduces build time and output size, which is useful for production deployments.
- Refer to the [AREG SDK Example](./../../examples/README.md) documentation for details describing available test cases.

---
