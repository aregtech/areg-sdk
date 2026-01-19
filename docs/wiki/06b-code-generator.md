# AREG SDK Code Generator (`codegen.jar`)

The `codegen.jar` tool in the AREG SDK is a powerful utility that automates the generation of essential code for both **Remote** (Public) and **Local** (Private) Service Interfaces, streamlining Inter-Process Communication (IPC) and Remote Procedure Call (RPC) development. By managing repetitive coding tasks, `codegen.jar` enables developers to focus on core application logic while ensuring codebase consistency and efficiency.

# Table of Contents

1. [Key Advantages of `codegen.jar`](#1-key-advantages-of-codegenjar)
2. [How to Use `codegen.jar`](#2-how-to-use-codegenjar)
3. [Integration Tips](#3-integration-tips)
4. [Example Workflow](#4-example-workflow)
   - [Workflow with Microsoft Visual Studio](#workflow-with-microsoft-visual-studio)
   - [Workflow with CMake](#workflow-with-cmake)
5. [Conclusion](#5-conclusion)

## 1. Key Advantages of `codegen.jar`

1. **Streamlined Code Automation**: Automatically generates Service Interface code to support object-based RPC communication, minimizing manual coding and potential errors. The generated code adheres to AREG SDK standards, which improves maintainability and scalability.

2. **Efficient Service Interface Setup**: Produces all necessary files for service interfaces, enabling smooth IPC/RPC setup between components. This allows developers to concentrate on higher-level design without handling IPC/RPC complexities.

3. **Enhanced Productivity**: Reduces boilerplate code, accelerating development cycles and supporting modular architectures that can easily adapt to changing requirements.

4. **Consistency and Reliability**: Ensures that all service interfaces conform to AREG SDK standards, which is essential for applications that require high reliability, such as those with real-time multiprocessing and multithreading needs.

5. **Secure Outsourcing Capabilities**: Enables secure collaboration by allowing external teams to generate code from provided Service Interface documents, removing the need to share full source code.

## 2. How to Use `codegen.jar`

1. **Define Service Interfaces**: [Specify the service interface](./ServiceInterface.md) in a `.siml` file, including data structures, attributes, and methods (requests, responses, broadcasts).

2. **Generate Service Code**: Run `codegen.jar` with the `.siml` file as input to produce communication events, stubs, and proxies, providing a structured foundation for IPC. Command line to run `codegen.jar`:
   ```bash
   java -jar codegen.jar --doc=<path-to-interface-document> --root=<path-to-generated-root-directory> --target=<relative-output-path>
   ```
   Run `java -jar codegen.jar --help` to display the list of possible options.

3. **Integrate Generated Code**: Add the generated code as a static library within your project and link it with other components. This code manages message handling, allowing developers to focus on application logic.

> [!NOTE]
> The tool [Lusan](https://github.com/aregtech/areg-sdk-tools/), currently under development, will soon provide a visual interface to simplify `.siml` document creation.

## 3. Integration Tips

Use AREG **CMake functions** from [functions.cmake](./../../conf/cmake/functions.cmake) to automate file generation and inclusion:
- **addServiceInterface**: Generates files in `${AREG_GENERATE_DIR}`, following a directory structure based on the `.siml` file's parent path.
- **addServiceInterfaceEx**: Allows files to be generated in any specified location.
- **macro_add_service_interface**: Uses the specified code generator and location for generated files.

For automated code generation and inclusion **with Microsoft Visual Studio**, refer to [Integrating AREG Framework with Microsoft Visual Studio](./02d-msvc-integrate.md).

## 4. Example Workflow

### Workflow with CMake

1. **Create Service Interface Document**: Define a `.siml` XML file following the [structure](./../ServiceInterface.md).

2. **Add Service Interface to the Build**: In the CMake scripts, include `<areg-sdk>/areg.cmake` to enable AREG functions, and add the following script:
   ```cmake
   addServiceInterface(<static-lib-name> <path-to-interface-document>)
   macro_declare_executable(<executable-name> <static-lib-name> <list-of-source-codes>) 
   ```
   - Replace `<path-to-interface-document>`, `<static-lib-name>`, `<executable-name>`, and `<list-of-source-codes>` as needed.

3. **Implement Application Logic**: Use the generated stubs and clients to handle requests, responses, broadcasts, and other events, supporting core functionality development.

### Workflow with Microsoft Visual Studio

1. **Create Service Interface Document**: Define a `.siml` XML file following the [structure](./../ServiceInterface.md).

2. **Run Code Generator**:
   ```bash
   java -jar codegen.jar --doc=<path-to-interface-document> --root=<path-to-generated-root-directory> --target=<relative-output-path>
   ```
   - Replace `<path-to-interface-document>`, `<path-to-generated-root-directory>`, and `<relative-output-path>` as appropriate.

3. **Add Generated Files to Project**: Place the generated files in the static library project's source directory, compile, and link them with the AREG SDK.

4. **Implement Application Logic**: Use the generated stubs and clients to manage requests, responses, broadcasts, and other events, enabling the development of core functionality.

## 5. Conclusion

The `codegen.jar` tool is an essential asset in the AREG SDK, automating IPC and RPC setups and promoting code consistency. It enhances productivity, modularity, and quality in distributed applications, enabling developers to build reliable, streamlined communication across components and focus on delivering high-quality software.
