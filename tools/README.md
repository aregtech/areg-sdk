# Tools

```
This file is part of AREG SDK, the multitasking application development platform.  
Copyright (c) Aregtech, 2017-2023  
Contact: info[at]areg.tech  
```

## Running the Code Generator

To generate service interface code using the **AREG SDK Code Generator**, follow these steps:

1. **Create the Service Interface XML File**  
   Prepare a `.siml` file (Service Interface Prototype) in your project directory.

2. **Include the Code Generator Library**  
   Add the `codegen.jar` library to your Java path.

3. **Run the Code Generator**  
   Execute the following command from the command line:  
   ```bash
   $ java -jar <areg-sdk-root>/tools/codegen.jar --root=<project_root> --doc=<relative_path_to_siml> --target=<relative_path_to_target_location>
   ```

### Parameters:
- `<areg-sdk-root>`: The root directory of the AREG SDK source code.  
- `<project_root>`: The root directory of your project.  
- `<relative_path_to_siml>`: The path to the `.siml` file relative to the project root (e.g., `sources/services/SystemShutdown.siml`).  
- `<relative_path_to_target_location>`: The path to the folder where the generated files will be stored, relative to the project root (e.g., `product/generated`).

---

## Best Practices for Managing Generated Files

It is recommended to include the generated files in a static library for better modularity and reuse. For example:  

If your application consists of two processes (two separate projects), create a third static library project (e.g., `generated`) that includes the generated files. This library can then be linked to both processes, ensuring consistent access to the shared service interface.

--- 
