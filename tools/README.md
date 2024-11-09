# Tools

```
This file is part of AREG SDK, the multi-tasking application development platform.
Copyright (c) Aregtech, 2017-2023
Contact: info[at]aregtech.com
```
To run a code generator:
1. Create a Service Interface XML file ( _.siml_ ) in your project;
2. Include code genrator **codegen.jar** library to your _Java path_.
3. Call from command line:
If `codegen.jar` is in `CLASSPATH` run it calling
```bash
$ java -jar codegen.jar --root=<project_root> --doc=<relative_path_to_siml> --target=<relative_path_to_target_location>
```
or
```bash
$ java com.aregtech.CMFMain --root=<project_root> --doc=<relative_path_to_siml> --target=<relative_path_to_target_location>
```
If `codegen.jar` is not in `CLASSPATH` excplicitly specify the path of `codegen.jar`
```bash
$ java -jar <areg-sdk-root>/tools/codegen.jar --root=<project_root> --doc=<relative_path_to_siml> --target=<relative_path_to_target_location>
```

Where:
- `<project_root>` if the path of your project, for example `~/aregtech/areg-sdk/examples`;
- `<relative_path_to_siml>` the service interface prototype file path relative to the project root, for example `12_pubsvc/services/SystemShutdown.siml`
- `<relative_path_to_target_location>` the generated code output folder path relative to the project root, for example `12_pubsvc/generated`
- `<areg-sdk-root>` is the path to `areg-sdk` sources.

We recommend include generated files in a static library, so that all projects can link. For example: if an applicatios consists of 2 processes (2 separate project), create third static library project called, for instance, `generated` that is linked in these two processes.
