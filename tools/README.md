```
This file is part of AREG SDK, the multi-tasking application development platform.
Copyright (c) Aregtech, 2017-2021
Contact: info[at]aregtech.com
```
To run a code generator:
1. Create a Service Interface XML file ( _.siml_ ) in your project;
2. Include code genrator **codegen.jar** library to your _Java path_.
3. Call from command line:
java com.aregtech.CMFMain _--root=_**<project_root>** _--doc=_**<relative_path_to_siml>** _--target=_**<relative_path_to_target_location>**

where
- **<project_root>**                  -- is a project root folder. _**All generated paths and includes are relative to this path!**_
- **<relative_path_to_siml>**   -- the relative path to the _SIML_ document that contains Service Interface description.
- **<relative_path_to_target>** -- the relative path the target location to output generated files.

If there are more than one projects that use the generated service interface objects, the recommended solution is to generate files in a common location and include them in a statically linked library that all other projects can be linked with a libraryl. For example: if an applicatio consists of 2 processes and each process is a separate project (for instance, "main" and "secondary"), create third project (for intance, "generated") as a static library and include this project in first two (for instance, link "main" and "secondary" with "generated").
