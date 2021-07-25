```
This file is part of AREG IoT SDK
Copyright (c) Aregtech, 2017-2021
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```
============================================
# How To

This document replies to several howto questions.

## How to use preprocessor directives

AREG SDK uses a few preprocessor directives to compile **POSIX** and **Windows** versions of codes. Here is the list:
- **POSIX** to compile sources with _POSIX API_, also see details in [POSIX.md](./POSIX.md)
- **WINDOWS** to compile sources with _Win32 API_, also see details in [WIN32.md](./WIN32.md) 
- **DEBUG** to build debug version of codes
- **NDEBUG** to build release version of codes
- **ENABLE_TRACES** to enable tracing / logging
- **EXP_AREG_LIB** or **EXPORT_STATIC_SYMBOLS** to compile static library of _AREG framerowk_
- **EXP_AREG_DLL** or **EXPORT_SHARED_SYMBOLS** to compile shared library of _AREG framerowk_
- **IMP_AREG_LIB** or **IMPORT_STATIC_SYMBOLS** to link with static library of _AREG framerowk_
- **IMP_AREG_DLL** or **IMPORT_SHARED_SYMBOLS** to link with shared library of _AREG framerowk_

For example, the _POSIX_ version of examples are compiled with preprocessor directives _POSIX, (N)DEBUG, IMP_AREG_LIB, ENABLE_TRACES_.

## How to compile

Currently, AREG SDK provides **Eclipse** and **Visual Studio** project files to compile projects. _Eclipe_ projects are used to compile **POSIX** (_Linux_) versions and _Visual Studio_ project files are used to compile **Windows** versions of projects. By default, there is no need to make additional changes in settings to compile projects. More details of importing and compiling projects are below. After compilation, to run **IPC** related examples, make sure the process **mcrouter** has started.

### POSIX build

To compile _POSIX_ version of projects use _Eclipse for C/C++ Developers_ IDE and _GCC_ compiler version 98 or higher. You need to import to _Eclipse_ the existing project files to compile projects.

**How to import project**:
* Open _Eclipse for C/C++ Developers_ IDE.
* Create a new workspace and specify the workflow root folder as _are-sdk root_.
* In _Eclipse_ IDE select _Import_ menu (_File_ ==> _Import_) to start importing.
* In _Select_ dialog find _General_ and select option _Existing Projects into Workspace_.
* Click _Next_ button of dialog box, this opens _Import Projects_ dialog.
* In _Import Projects_ dialog choose radio _Select_, click _Browse_ button and set _areg-sdk root_ folder.
* After scanning all folders of _areg-sdk root_ are listed in the _Projects_ list box.
* Select either areg and mcrouter projects to compile framework, or select all to compile all.

The Debug builds are output in _Debug_ and the Release builds are in _Release_ subfolders of the projects. If changing settings of the project, make sure _POSIX_ (or __POSIX_) preprocessor directive is set.

### Windows build

To compile Windows versions of projects, use Visual Studio 12 or higher to open the solution. However, the sources can be compiled with Visual Studio 10 (MSVC v100) compiler or higher version.

**How to load project**:
* In your Visual Studio IDE (should be VS2012 or higher) open _areg-sdk.sln_ file located in _areg-sdk root_ folder to load all projects.
* In the toolbar of your Visual Studio select specific _Solution Cofigurations_ version and select _Solution Platforms_ you'd like to compile and build the solution.

For example, to compile sources with Visual Studio 2019 compiler, in the _Solution Configurations_ tool select _dbg_v142_ Debug or _rls_v142_ Release builds. Then select either Win32 for 32-bit or x64 for 64-bit builds and compile the solution. With _Visual Studio_ all projects are output in *_product_* subfolder created in _areg-sdk root_.

### Other builds
Currently AREG SDK does not contain project files of any other IDE. AREG SDK neither contains make files to build on the command line. This comes soon.

## How to create a project or integrate in project

Creating a new project or integrating _AREG framework_ in an existing project have almost the same logic. 
1. Include **areg-sdk/framework** path in the _include path_ list of the project.
2. Declare **POSIX** or **WINDOWS** preprocessor directive to specify _POSIX_ or _Win32_ API use in framework.
3. Declare **DEBUG** or **NDEBUG** preprocessor directive to specify which build configuration to use. 
4. Declare **IMP_AREG_DLL** or **IMP_AREG_LIB** preprocessor to specify shared or static library symbols of framework to import.
5. Declare **ENABLE_TRACES** preprocessor directive if logs / tracing is going to exist in application.
6. Include binary out directory of the library search path list.
7. Make sure to link the project sources with _libareg.a_ for POSIX and _areg.lib_ for Windows version.

Steps (5) is optional, and (6) and (7) can be omitted if a project is a _static library_. Only steps (1) - (4) are obligatorily for all projects.

For example, to create a _POSIX_ application that uses static library of _AREG framework_:
- Specify _areg-sdk/framework_ in source include paths and _areg-sdk/framework/areg/Debug_ path in library search path
- Set linker option to link with _libareg.a_
- Specify _POSIX, DEBUG, IMP_AREG_LIB, ENABLE_TRACES_ preprocessor directive.

After these steps, the POSIX application sources can use _AREG framework_ API and build the project.


## How to use logging

**Creating logs**

Use predefined _MACRO_ to output logs. Tracing / Logging MACRO are declared in [_GTrace.h_](./../framework/areg/trace/GETrace.h) header file.

First of all, to be able to see the logs, the project must be compiled with preprocessor directive _ENABLE_TRACES_. If this is not enabled, the log outputs will not exist in binaries. Second, there must be declared _scopes_, which is possible by using DEF_TRACE_SCOPE(_some_unique_name_of_scope_), and scopes should be used in the method(s) by declaring TRACE_SCOPE(_some_unique_name_of_scope_). As soon as the method is called, the scope generates _"Enter"_ message and then exit method, the scope generates _"Exit"_ message. In between, the developers need to make log output just in the same way as they would do when call _printf()_ except that there is no need to specify _end-of-line_. The logs are output by priority, such as DEBUG, INFO, WARN, ERROR, FATAL. Each of them has predefined MACRO.

**Enable loging**

Once application uses logging. It should be enabled during application startup. An example of starting logging is a project [**04_trace**](./../examples/04_trace/). There are many other example application sthat use logs. As soon as logging is enabled during runtime, the system is ready to output. The logging has intiaial default settings, but it is recommended to use configuration to enable logs and set logging priorities.

_In the current version, all logs are output in the file on file system. It is planned to have remote logging, which is in development phase._

**Configuring logging**

When logging starts, it reads the initial instructions from [_configuration file_](./../framework/areg/resources/log.init), which has simple syntax and the parser of this syntax is part of AREG framework. So that, the projects can use same syntax to configure own components. If after compilation, the binary output folder does not contain _"config/log.init"_ file, create the _config_, copy [_log.init_](./../framework/areg/resources/log.init) located in [_resource_](./../framework/areg/resources/) folder. The configuration file does well documented instructions how to enable / disable loging, how to specify which loging scopes to use and to change the logging priority. 

Please note, it is planned to change slightly the configuration logc.


## How to use multicast router

Multicast Router (MCR) is a part of AREG framework and it is used for inter-process communication (IPC). When start an application with public service, make sure that the **mcrouter** also has been started. Otherwise, the servicing will not work. The local services do not need to use multicast routing **mcrouter** process.

The MCR is options are set in [_configuration file_](./../framework/areg/resources/router.init). Developers mainly need to change IP-address and the port number of router in this file. If after compilation, the binary output folder does not contain _"config/router.init"_ file, create the _config_, copy [_router.init_](./../framework/areg/resources/router.init) located in [_resource_](./../framework/areg/resources/) folder.
