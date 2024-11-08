# AREG SDK Code Generator (`codegen.jar`)

The `codegen.jar` tool in the AREG SDK is a robust utility that automates the generation of essential code for both **Remote** (Public) and **Local** (Private) Service Interfaces, simplifying Inter-Process Communication (IPC) and Remote Procedure Call (RPC) development. By handling boilerplate tasks, `codegen.jar` enables developers to focus on application logic while ensuring consistency and efficiency in the codebase.

## Key Advantages of `codegen.jar`

1. **Streamlined Code Automation**: Automatically generates Service Interface code, supporting RPC and object communication. This reduces manual coding and potential errors, while ensuring code adheres to AREG SDK standards for easier maintenance and scalability.

2. **Efficient Service Interface Setup**: Quickly generates all necessary files for service interfaces, facilitating seamless setup for IPC/RPC between components. This lets developers concentrate on higher-level design without dealing with IPC/RPC complexities.

3. **Enhanced Productivity**: Reduces boilerplate code, accelerating development cycles and supporting modular architectures that easily adapt to evolving requirements.

4. **Consistency and Reliability**: Ensures all service interfaces meet AREG SDK standards, which is essential for applications that demand high reliability, such as real-time and multi-threaded systems.

5. **Secure Outsourcing Capabilities**: Facilitates secure collaboration by allowing external teams to generate necessary code from provided Service Interface documents, eliminating the need to share full source code.

## How to Use `codegen.jar`

1. **Define Service Interfaces**: Specify service interface requirements in a `.siml` file, including data structures, attributes, and methods (requests, responses, broadcasts).

2. **Generate Service Code**: Run `codegen.jar` with the `.siml` file as input to produce communication events, stubs, and proxies, creating a structured foundation for IPC.

3. **Integrate Generated Code**: Incorporate generated code as a static library within your project, linking it to other components. This generated code manages message handling, allowing developers to focus on application logic.

> **Note**: [Lusan](https://github.com/aregtech/areg-sdk-tools/) (currently in development) will soon offer a visual interface to simplify `.siml` document creation.

> **Tip**: Use AREG CMake functions from [functions.cmake](./../../conf/cmake/functions.cmake) to automate file generation and inclusion:
> - **addServiceInterface**: Generates files under `${AREG_GENERATE_DIR}` with a directory structure based on the `.siml` file’s parent path.
> - **addServiceInterfaceEx**: Allows generating files in any custom location.
> - **macro_add_service_interface**: Uses specified code-generator and location for generated files.

For Microsoft Visual Studio integration, refer to [Integrating AREG Framework with Microsoft Visual Studio](./msvc-integrate.md).

## Example Workflow

1. **Create Service Interface Document**: Define a `.siml` XML file according to the [structure](./../ServiceInterface.md).
2. **Run Code Generator**:
   ```bash
   java -jar codegen.jar --doc=<path-to-interface-document> --root=<path-to-generated-root-directory> --target=<relative-output-path>
   ```
   - Replace `<path-to-interface-document>`, `<path-to-generated-root-directory>`, and `<relative-output-path>` as needed.

3. **Add Generated Files to Project**: Include generated files in the project’s source directory, compile, and link with AREG SDK.
4. **Implement Application Logic**: Use generated interfaces and proxies to handle component communication while developing core functionality.

## Integration Tips

- **Custom Configuration**: Adjust `codegen.jar` settings to align with project coding practices.
- **Automate Regeneration**: Consider an automated build step to run `codegen.jar` when interfaces change.
- **Review Documentation**: Understand the structure and behavior of generated classes, especially if custom modifications are needed.

## Conclusion

The `codegen.jar` tool is a powerful asset in the AREG SDK, automating IPC and RPC setups and enhancing code consistency. It supports productivity, modularity, and high standards in distributed applications, enabling developers to create reliable, error-free communication across components while focusing on delivering high-quality software.
