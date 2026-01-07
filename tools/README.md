
# AREG SDK Tools

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
  --root=<project-root> \
  --doc=<relative-path-to-siml> \
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
java -jar ~/areg-sdk/tools/codegen.jar \
  --root=./my_project \
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
├── HelloServiceClientBase.cpp
├── HelloServiceEvents.cpp
├── HelloServiceProxy.cpp
├── HelloServiceStub.cpp
└── NEHelloService.cpp
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

From the `tools/` directory:`
a. Run on Linux/macOS:

   ```bash
   sh ./setup-project.sh
   ```
b.Or run on Windows:
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

## 7. Summary

* Use `.siml` to define services
* Prefer **CMake integration** over manual code generation
* Keep generated code in **static libraries** for modularity
* Use setup scripts to get started instantly
* Leverage **Lusan** for visual service interface design and logging
* Follow examples for real-world IPC patterns

This workflow scales from a **hello-service example** to **full distributed production systems**.
