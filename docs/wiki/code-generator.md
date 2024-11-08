# AREG SDK Code Generator (`codegen.jar`)

The `codegen.jar` tool in the AREG SDK is a powerful utility for automating code generation, helping developers streamline the creation of **Remote** (*Public*) and **Local** (*Private*) Service Interfaces, and associated boilerplate code. Designed with flexibility and efficiency in mind, `codegen.jar` enables developers to focus on core logic while handling the repetitive tasks of generating code for Inter-Process Communication (IPC) and Remote Procedure Calls (RPC) between components.

## Benefits of Using `codegen.jar`

1. **Efficient Code Automation**: 
   - The code generator automates the creation of Service Interface code for Remote Procedure Calls (RPC) and object communication. By reducing the amount of manual coding, `codegen.jar` saves time and reduces the risk of human error.
   - It produces consistent, standardized code structures, ensuring that all generated code adheres to AREG SDK conventions, which enhances maintainability and scalability.
2. **Simplified Service Interface Setup**
   - With `codegen.jar`, you can quickly define Service Interfaces and generate the necessary code files, streamlining communication setup between local and remote components.
   - The tool automatically generates interface files and associated classes, encapsulating the complex details of IPC and RPC handling. This makes integration smoother and allows developers to focus on higher-level design and application logic.
3. **Enhanced Productivity**
   - `codegen.jar` drastically cuts down on the amount of repetitive boilerplate code, which would otherwise need to be written manually. This results in faster development cycles, enabling teams to build robust applications more efficiently.
   - It promotes a modular architecture, making it easier to add, modify, or remove service interfaces in response to changing requirements.
4. **Improved Reliability and Consistency**
   - The generated code follows AREG SDK standards, ensuring that all service interface implementations are consistent, reliable, and less prone to errors. This consistency is crucial in applications requiring high reliability, such as real-time or multi-threaded systems.
5. **Simplified and Secure Outsourcing**
   - `codegen.jar` enables safer and more efficient outsourcing by reducing the need to share full source code or detailed API documentation with external teams. Instead, you can simply provide a binary that either supplies or consumes a service, along with the associated Service Interface document file.
   - This allows the external team to generate the necessary code using `codegen.jar`, integrate it into their build, and either consume the provided service or implement a compatible service. This approach preserves intellectual property, minimizes dependency on extensive documentation, and simplifies the collaboration process, ensuring a faster and more secure integration.

## Use of `codegen.jar`

1. **Defining Service Interfaces**: Define your service interface requirements in a file. The interface definitions include the necessary data types, data, called Attributes, and methods (requests, responses and broadcasts to deliver multiple data at once).
2. **Generating Service Code**: Run `codegen.jar` with the defined service interface document as input. This produces a set of files that include communication events, stubs and proxies, creating a structured foundation for IPC.
3. **Integrating the Generated Code**: Integrate the generated code files into your project as a static library to link them with other application components as required. The generated classes and methods handle the low-level details of sending, receiving, and processing messages between components, allowing developers to focus on implementing the business logic.

> [!NOTE]
> The UI design tool [Lusan](https://github.com/aregtech/areg-sdk-tools/) is currently under the development. It will help to visualize the Service Interface document and simplify creating file.

> [!TIP]
> AREG CMake functions and macro, declared in [functions.cmake](./../../conf/cmake/functions.cmake) simplify generating and including generated files in the static library. See the detailed [description of CMake functions](./cmake-functions.md):
>  - **addServiceInterface**: generates files in the location set in variable `AREG_GENERATE_DIR` and  a directory structure mirroring the parent path of Service Interface document file.
>  - **addServiceInterfaceEx**: generates files in any location.
>  - **macro_add_service_interface**: generates files using given code-generator, given Service Interface document file and the generated files are located in the given location.
> These functions and macro automatically trigger the code generator and add generated files in the given static library, letting developer to link with other binaries. 

> [!TIP]
> Integration of generated code in the Microsoft Visual Studio is slightly mode complex and described in the [Integrating AREG Framework with Microsoft Visual Studio](./msvc-integrate.md).

## Example Workflow for Using `codegen.jar`

1. **Define Service Interface Document**: Create a `.siml` XML file based on the [supported structure](./../ServiceInterface.md) to specify the details of the service interfaces, methods, and data structures needed for IPC/RPC.
2. **Run Code Generator**: Use the following command to run `codegen.jar` on the interface document:
   ```bash
   java -jar codegen.jar --doc=<path-to-interface-document> --root=<path-to-generated-root-directory> --target=
   ```
   - Replace `<path-to-interface-document>` with the path to your service interface document like `/home/dev/project/my/component/Service.siml`
   - Replace `<path-to-generated-root-directory>` with the desired output path to generate files like `/home/dev/project/product/generate`
   - Replace `<relative-path-to-generated-root>` with the relative path to generate code like `my/component/interface`

3. **Integrate Generated Files**: Add the generated files to your project’s source directory. Compile and link these files with the AREG SDK to enable communication with other components.

4. **Implement Business Logic**: With the generated service interface and proxy classes in place, you can focus on implementing the core functionality of your application, leveraging the generated code to handle communication.

## Integration Tips

- **Configuration Options**: Customize the output with configuration options in `codegen.jar` to fit your project’s organizational standards or coding practices.
- **Automatic Re-Generation**: For larger projects, consider setting up an automated build step to run `codegen.jar` whenever the service interface definitions change, ensuring your code remains synchronized with the latest interface specifications.
- **Documentation**: Review the generated code documentation to understand the structure and functionality of the generated classes, especially if your application requires custom modifications to the default behavior.

## Conclusion

The `codegen.jar` tool is an invaluable asset in the AREG SDK, enabling efficient, consistent, and reliable code generation for service interfaces. By simplifying complex IPC and RPC setups, it enhances productivity, reduces development time, and ensures that applications built with the AREG SDK maintain high standards of reliability and modularity. With `codegen.jar`, developers can focus on delivering high-quality software with streamlined, error-free communication between components, making it an ideal choice for building robust, distributed applications.
