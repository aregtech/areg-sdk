# AREG SDK Code Generator (`codegen.jar`)

The `codegen.jar` tool in the AREG SDK is a robust utility that automates the generation of essential code for both **Remote** (Public) and **Local** (Private) Service Interfaces, simplifying Inter-Process Communication (IPC) and Remote Procedure Call (RPC) development. By managing repetitive coding tasks, `codegen.jar` enables developers to concentrate on core application logic while ensuring consistency and efficiency in the codebase.

## Key Advantages of `codegen.jar`

1. **Streamlined Code Automation**: Automatically generates Service Interface code to support object RPC communication, minimizing manual coding and potential errors. The generated code adheres to AREG SDK standards, improving maintainability and scalability.

2. **Efficient Service Interface Setup**: Generates all necessary files for service interfaces, enabling seamless IPC/RPC setup between components. This allows developers to focus on higher-level design without getting involved in IPC/RPC complexities.

3. **Enhanced Productivity**: Reduces boilerplate code, speeding up development cycles and supporting modular architectures that can easily adapt to changing requirements.

4. **Consistency and Reliability**: Ensures that all service interfaces align with AREG SDK standards, which is essential for applications that require high reliability, such as real-time multiprocessing and multithreaded systems.

5. **Secure Outsourcing Capabilities**: Enables secure collaboration by allowing external teams to generate code from provided Service Interface documents, eliminating the need to share full source code.

## How to Use `codegen.jar`

1. **Define Service Interfaces**: [Specify service interface](./ServiceInterface.md) in a `.siml` file, including data structures, attributes, and methods (requests, responses, broadcasts).

2. **Generate Service Code**: Run `codegen.jar` with the `.siml` file as input to produce communication events, stubs, and proxies, providing a structured foundation for IPC.

3. **Integrate Generated Code**: Incorporate the generated code as a static library within your project and link it with other components. This code handles message management, allowing developers to focus on application logic.

> [!NOTE]
> The tool [Lusan](https://github.com/aregtech/areg-sdk-tools/), currently under development, will offer a visual interface to simplify `.siml` document creation.

## Integration Tips

Use AREG **CMake functions** from [functions.cmake](./../../conf/cmake/functions.cmake) to automate file generation and inclusion:
- **addServiceInterface**: Generates files in `${AREG_GENERATE_DIR}` with a directory structure based on the `.siml` file’s parent path.
- **addServiceInterfaceEx**: Allows files to be generated in any specified location.
- **macro_add_service_interface**: Uses the specified code generator and location for generated files.

For automated code generation and inclusions **with Microsoft Visual Studio**, refer to [Integrating AREG Framework with Microsoft Visual Studio](./msvc-integrate.md).

## Example Workflow

### Workflow with Microsoft Visual Studio

1. **Create Service Interface Document**: Define a `.siml` XML file following the [structure](./../ServiceInterface.md).

2. **Run Code Generator**:
   ```bash
   java -jar codegen.jar --doc=<path-to-interface-document> --root=<path-to-generated-root-directory> --target=<relative-output-path>
   ```
   - Replace `<path-to-interface-document>`, `<path-to-generated-root-directory>`, and `<relative-output-path>` as needed.

3. **Add Generated Files to Project**: Add generated files to the static library project’s source directory, compile, and link them with AREG SDK.

4. **Implement Application Logic**: Use generated stubs and clients to manage requests, responses, broadcasts and other events to develop core functionality.

### Workflow with CMake

1. **Create Service Interface Document**: Define a `.siml` XML file following the [structure](./../ServiceInterface.md).

2. **Add Service Interface in the build**: in the CMake scripts, include `<areg-sdk>/areg.cmake` to enable AREG functions, and add a script like this:
   ```cmake
   addServiceInterface(<static-lib-name> <path-to-interface-document>)
   macro_declare_executable(<executable-name> <static-lib-name> <list-of-source-codes>) 
   ```
   - Replace `<path-to-interface-document>`, `<static-lib-name>`, and `<executable-name>`, `<list-of-source-codes>` as needed.

3. **Implement Application Logic**: Use generated stubs and clients to manage requests, responses, broadcasts and other events to develop core functionality.

## Conclusion

The `codegen.jar` tool is an essential asset in the AREG SDK, automating IPC and RPC setups, and promoting code consistency. It enhances productivity, modularity, and quality in distributed applications, enabling developers to build reliable, streamlined communication across components and focus on delivering high-quality software.