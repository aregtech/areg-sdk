# Areg SDK Code Generator (`codegen.jar`)

The Areg SDK Code Generator is a build-time utility that automates the generation of service interface code for **Remote** (Public) and **Local** (Private) services. By transforming `.siml` service interface definitions into production-ready C++ code, `codegen.jar` eliminates repetitive boilerplate, ensures interface consistency, and allows developers to focus on business logic rather than communication infrastructure.

---

## Table of Contents

1. [Key Advantages](#1-key-advantages)
2. [Usage](#2-usage)
3. [Build Integration](#3-build-integration)
4. [Workflow Examples](#4-workflow-examples)
5. [Best Practices](#5-best-practices)

---

## 1. Key Advantages

### Automated Code Generation

Generates complete service interface code for object-based RPC communication, including:
- Service provider stubs and skeletons
- Service consumer proxies
- Request/response handlers
- Event notification dispatchers
- Attribute accessors
- Serialization/deserialization logic

### Consistency and Reliability

- **Standards Compliance**: All generated code adheres to Areg SDK coding standards
- **Type Safety**: Strongly-typed interfaces prevent runtime errors
- **Version Control**: Interface changes automatically propagate to all generated code
- **Documentation**: Self-documenting code with clear method signatures

### Developer Productivity

- **Zero Boilerplate**: Eliminates hundreds of lines of repetitive code per service
- **Rapid Prototyping**: Define interfaces quickly, generate code instantly
- **Modular Architecture**: Clean separation between interface definition and implementation
- **Easy Refactoring**: Update `.siml` file, regenerate—no manual code changes needed

### Secure Collaboration

Enables distributed development by allowing external teams to:
- Generate service implementation code from interface definitions alone
- Implement services without access to proprietary business logic
- Maintain interface contracts across organizational boundaries

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 2. Usage

### Prerequisites

- **Java 17 or newer** installed and available in system PATH
- Areg SDK built (provides `codegen.jar` in the output directory)

### Service Interface Definition

Create a `.siml` file defining your service interface. A service interface consists of:

- **Data Structures**: Custom types, enumerations, imported types
- **Attributes**: Shared state accessible to all consumers
- **Requests**: Service Consumer-initiated RPC calls expecting responses
- **Responses**: Service Provider-side answers to requests
- **Broadcasts**: Service Provider-initiated event to all connected and subscribed consumers

**Example**: `HelloService.siml`
```xml
<?xml version="1.0" encoding="utf-8" standalone="yes"?>
<ServiceInterface FormatVersion="1.0.0">
    <Overview ID="1" Name="HelloService" Version="1.0.0">
        <Description>Simple greeting service</Description>
    </Overview>
    <MethodList>
        <Method ID="2" MethodType="request" Name="Greeting">
            <Description>Request a personalized greeting</Description>
        </Method>
        <Method ID="3" MethodType="response" Name="Greeting">
            <Description>Greeting response</Description>
        </Method>
    </MethodList>
</ServiceInterface>
```

For detailed `.siml` syntax, see [Service Interface Documentation](./ServiceInterface.md).

### Command-Line Invocation

```bash
java -jar codegen.jar --doc=<path-to-siml-file> --root=<output-root> --target=<relative-path>
```

**Parameters**:
- `--doc`: Path to `.siml` service interface definition file
- `--root`: Root directory for generated files (typically project source directory)
- `--target`: Relative path within root for generated code (e.g., `generated/services`)

**Example**:
```bash
java -jar codegen.jar \
    --doc=./services/HelloService.siml \
    --root=./src \
    --target=generated
```

Generated files appear in: `./src/generated/`

### Help and Options

```bash
java -jar codegen.jar --help
```

Displays all available command-line options and usage information.

> [!TIP]
> Use the [Lusan GUI tool](https://github.com/aregtech/areg-sdk-tools) to visually design service interfaces. Lusan provides an intuitive graphical editor for `.siml` files, eliminating the need to write XML manually.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 3. Build Integration

### Automated Integration with CMake

Areg SDK provides CMake functions in [functions.cmake](./../../conf/cmake/functions.cmake) to seamlessly integrate code generation into the build process:

#### `addServiceInterface`

Generates code in `${AREG_GENERATE_DIR}` with automatic directory structure:

```cmake
addServiceInterface(<static-lib-name> <path-to-siml-file>)
```

**Behavior**:
- Creates directory structure based on `.siml` file location
- Generates C++ source and header files
- Compiles files into a static library
- Makes library available for linking

**Example**:
```cmake
addServiceInterface(HelloServiceLib ./services/HelloService.siml)
macro_declare_executable(HelloService HelloServiceLib main.cpp provider.cpp)
```

#### `addServiceInterfaceEx`

Generates code in a custom location:

```cmake
addServiceInterfaceEx(<static-lib-name> <path-to-siml-file> <custom-output-dir>)
```

Use when you need control over the output directory structure.

#### `macro_add_service_interface`

Low-level function for advanced scenarios:

```cmake
macro_add_service_interface(<lib-name> <siml-path> <codegen-path> <output-dir>)
```

Allows specifying custom code generator location and output directory.

### Integration with Microsoft Visual Studio

For Visual Studio projects without CMake, see the dedicated guide:  
📘 [Integrating Areg Framework with Microsoft Visual Studio](./02c-msvc-integrate.md)

Manual integration steps:
1. Run `codegen.jar` from command line or custom build step
2. Add generated files to a static library project
3. Link the library with your application projects

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 4. Workflow Examples

### CMake-Based Workflow

**Step 1: Create Service Interface**

Define `services/HelloService.siml` following the [Service Interface structure](./ServiceInterface.md).

**Step 2: Configure CMake Build**

In your `CMakeLists.txt`:

```cmake
# Include Areg SDK
include(<areg-sdk-path>/areg.cmake)

# Generate service interface code and create static library
addServiceInterface(HelloServiceLib ./services/HelloService.siml)

# Create executable linking the generated library
macro_declare_executable(
    HelloService                # Executable name
    HelloServiceLib             # Generated service library
    main.cpp                    # Application sources
    ServiceProvider.cpp
)
```

**Step 3: Implement Business Logic**

Use generated base classes:

```cpp
#include "generated/HelloServiceStub.hpp"

class ServiceProvider : public HelloServiceStub {
public:
    // Implement request handler
    virtual void requestGreeting(const String& name) override {
        // Business logic here
        String greeting = "Hello, " + name + "!";
        responseGreeting(greeting);
    }
};
```

**Step 4: Build Project**

```bash
cmake -B ./build
cmake --build ./build
```

Code generation happens automatically during the build process.

### Visual Studio Workflow

**Step 1: Create Service Interface**

Define `services/HelloService.siml` following the [Service Interface structure](./ServiceInterface.md).

**Step 2: Run Code Generator**

```bash
java -jar codegen.jar ^
    --doc=services/HelloService.siml ^
    --root=src ^
    --target=generated
```

**Step 3: Add Generated Files to Project**

1. Create a static library project in Visual Studio
2. Add generated files from `src/generated/` to the project
3. Compile the library
4. Link it with your application project

**Step 4: Implement Business Logic**

Same as CMake workflow—inherit from generated stubs and implement service methods.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 5. Best Practices

### Service Interface Design

- **Keep interfaces focused**: One service per functional domain
- **Version interfaces**: Use semantic versioning in `.siml` files
- **Document thoroughly**: Add descriptions to all methods and parameters
- **Plan for evolution**: Design interfaces with future extensions in mind

### Code Generation

- **Automate in build system**: Use CMake functions instead of manual invocation
- **Version control `.siml` files**: Track interface definitions, not generated code
- **Exclude generated code from VCS**: Add `generated/` to `.gitignore`
- **Regenerate on interface changes**: Always rebuild after modifying `.siml` files

### Project Organization

```
project/
├── services/
│   └── HelloService.siml          # Interface definitions (version controlled)
├── src/
│   ├── generated/                 # Generated code (excluded from VCS)
│   │   ├── HelloServiceStub.hpp
│   │   ├── HelloServiceStub.cpp
│   │   ├── HelloServiceClientBase.hpp
│   │   ├── HelloServiceClientBase.cpp
│   │   ├── HelloServiceProxy.hpp
│   │   └── HelloServiceProxy.cpp
│   └── implementation/            # Business logic (version controlled)
│       ├── ServiceProvider.cpp
│       └── ServiceConsumer.cpp
└── CMakeLists.txt
```

### Common Pitfalls

- **Manual edits to generated code**: Never modify generated files—changes will be lost on regeneration
- **Stale generated code**: Always regenerate after updating `.siml` files
- **Missing dependencies**: Ensure Java 17+ is installed and accessible
- **Path issues**: Use absolute paths or CMake variables for reliable builds

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Summary

The `codegen.jar` tool is a cornerstone of Areg SDK development, transforming service interface definitions into production-ready C++ code. By automating boilerplate generation, enforcing interface consistency, and integrating seamlessly with modern build systems, it enables developers to build robust, maintainable distributed applications efficiently.

**Key Takeaways**:
- **Automates** service interface code generation from `.siml` definitions
- **Eliminates** hundreds of lines of boilerplate per service
- **Ensures** type-safe, standards-compliant communication code
- **Integrates** seamlessly with CMake and Visual Studio workflows
- **Accelerates** development cycles and reduces errors

For advanced service interface design and visual editing, explore the [Lusan GUI tool](https://github.com/aregtech/areg-sdk-tools).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
