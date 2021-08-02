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
- **POSIX** to compile sources with _POSIX API_, details are in [POSIX.md](./POSIX.md).
- **WINDOWS** to compile sources with _Win32 API_, details are in [WIN32.md](./WIN32.md).
- **DEBUG** to compile Debug configuration.
- **NDEBUG** to compile Release configuration.
- **ENABLE_TRACES** to enable logging / tracing in sources.
- **EXP_AREG_LIB** or **EXPORT_STATIC_SYMBOLS** to build _AREG framerowk_ static library
- **EXP_AREG_DLL** or **EXPORT_SHARED_SYMBOLS** to build _AREG framerowk_ shared library
- **IMP_AREG_LIB** or **IMPORT_STATIC_SYMBOLS** to link with _AREG framerowk_ static library
- **IMP_AREG_DLL** or **IMPORT_SHARED_SYMBOLS** to link with _AREG framerowk_ shared library

A typical _POSIX_ application usually is compiled with _POSIX, DEBUG (NDEBUG), IMP_AREG_LIB (IMP_AREG_DLL), ENABLE_TRACES_ preprocessor directives.

## How to compile

AREG SDK provides **Eclipse** and **Visual Studio** project files to compile projects. _Eclipe_ projects are used to compile **POSIX** (_Linux_) and _Visual Studio_ project files are used to compile **Windows** versions of projects. By default, there is no need to make additional changes in settings to compile projects. More details of importing and compiling projects are below. To run **IPC** examples, make sure the process **mcrouter** has started.

### POSIX build

To compile _POSIX_ version of projects use _Eclipse for C/C++ Developers_ IDE and _GCC_ compiler version 11 or higher. You need to import to _Eclipse_ workspace the existing projects.

**How to import projects**:
* Open _Eclipse for C/C++ Developers_ IDE.
* Create a _New Workspace_, specify _areg-sdk_ as workspace root.
* In _Eclipse_ IDE select _Import_ menu (_File_ ==> _Import_) to start importing.
* In _Select_ dialog select _General_ ==> _Existing Projects into Workspace_.
* Click _Next_ button, this opens _Import Projects_ dialog.
* In _Import Projects_ dialog click _Select_ ==> _Browse_ and set _areg-sdk_ folder.
* After scan, _Projects_ contains a list of projects.
* Select either areg and mcrouter, or select all to compile all.

The Debug builds are output in _Debug_ and the Release builds are in _Release_ subfolders of each project. By default, _AREG framework_ is compiled as a static library. Change the settings of the framework and project if you need the shared library.

### Windows build

To compile Windows versions of projects, use Visual Studio 12 or higher to open the solution. However, the sources can be compiled with Visual Studio 10 (MSVC v100) compiler or higher version.

**How to load projects**:
* In Visual Studio (VS2012 or higher) open _areg-sdk.sln_ file, located in _areg-sdk_.
* In the toolbar of Visual Studio select _Solution Cofigurations_ and _Solution Platforms_ to compile.
* _Solution Platform_ **Win32** compiles 32-bit and **x64** compiles 64-bit applications.
* _Solution Configuratin_ chose configuration suitable for your Visual Studio.
    * _dbg_vc100_ or _rls_vc100_ use Visual Studio 10 compiler.
    * _dbg_vc110_ or _rls_vc110_ use Visual Studio 2012 compiler.
    * _dbg_vc120_ or _rls_vc120_ use Visual Studio 2013 compiler.
    * _dbg_vc140_ or _rls_vc140_ use Visual Studio 2015 compiler.
    * _dbg_vc141_ or _rls_vc141_ use Visual Studio 2017 compiler.
    * _dbg_vc142_ or _rls_vc142_ use Visual Studio 2019 compiler.

We have taken the advantage of Visual Studio property files and have chosen this behaviour of _Solution Configuration_ to escape changing the PlatformToolset settings in project files if a team works with different versions of Visual Studio. If you need to support newer versions of Visual Studio, register the _Platform_, _Configuration_ and _PlatformToolset_ in the **config_compile.props** file located in _areg-sdk_ and update _areg-sdk_ solution file.

**Note:** _You'll get error if you choose the compiler version, which does not exist in your system!_ 

Example of of building 64-bit Release in VS2019: in _Solution Configurations_ of _Visual Studio 2019_ toolbar select **_rls_v142_** and in _Solution Platform_ select **_x64_**. Compile the Solution. All projects are output in created *_product_* of _areg-sdk_.

### Other builds
Currently the AREG SDK neither contains project files of any other IDE, nor contains make files to build from command line. This addition comes soon.

## How to create a project or integrate in project

Here is information to integrate _AREG framework_ in the existing project. 
1. Include **areg-sdk/framework** in the _include path_ list of the project.
2. Set preprocessor directive **POSIX** or **WINDOWS** to specify _POSIX_ or _Win32_ build of applications.
3. Set preprocessor directive **DEBUG** for Debug and **NDEBUG** for Release build of applications.
4. Set preprocessor directive **IMP_AREG_DLL** for the shared library or **IMP_AREG_LIB** for the static library of _AREG framework_.
5. Optionally, set preprocessor directive **ENABLE_TRACES** if going to use logs / traces.
6. Include library output directory in library search path and specify _AREG framework_ library to link.

Now the project is ready to use _AREG framework_.

The examples of _AREG SDK_ for **POSIX** in Eclipse IDE are built having following settings:
- _areg-sdk/framework_ path is set in the include paths of projects.
- The library search path has _areg-sdk/framework/areg/Debug_ path for Debug and _areg-sdk/framework/areg/Release_ path for Release configuration.
- _areg_, _pthread_ and _rt_ libraries are listed in the list of libraries to link.
- Preprocessor directives _POSIX, DEBUG / NDEBUG, IMP_AREG_LIB, ENABLE_TRACES_ are set for Debug and Release versions.


## How to use logging

#### Creating logs

Use predefined _MACRO_ to configure, enable and output logs. Tracing / Logging MACRO are declared in the [_GTrace.h_](./../framework/areg/trace/GETrace.h) header file.

- To compile sources with logging, compile the application with _ENABLE_TRACES_ preprocessor directive. If this is not specified, the logs will not exist in binaries. 
- There must be declared _scopes_ to generate logs. 
- Use MACRO _**DEF_TRACE_SCOPE**(some_unique_name_of_scope)_ to declare scope.
- Use macro _**TRACE_SCOPE**(some_unique_name_of_scope)_ in methods to prepare tracing. 
- As soon as the method is executed, the scope generates _"Enter"_ message to log.
- Use MACRO _TRACE_DBG, TRACE_INFO, TRACE_WARN, TRACE_ERR, TRACE_FATAL_ to log messages by priority.
- Use standard string formatting flags to log messages (similar to _printf()_).
- No need to set _end-of-line_ ('\n') at the end of the message. The tracer automatically adds.
- When exit the method, the scope generates _"Exit"_ message to log.

#### Enable logging

The application(s) must be compiled with the _TRACE_ENABLE_ preprocessor directive. The _lotgging service_ is enabled in 2 steps:
1. In log initialization file (by default, **log.init** file) set _log.enable_ flag to _true_ to enable logging service for all applications or _log.enable.**appName**_ flag for a specific application.
2. Use _TRACER_START_LOGGING_ macro and specify the log initialization file path to start logging service. If path is NULL, the default relative path _./config/log.init_ is used.

See the details of initialization in [log.init](./../framework/areg/resources/log.init) file. In _log.init_ file the logs can be filtered by scopes and priorities. 

Browse [**04_trace**](./../examples/04_trace/) and other following examples to see how logs are initialized and used. Use macro _TRACER_FORCE_LOGGING_ to force starting logging service with default settings (enable all log scopes and priorities of the application).

_In the current version of log service, the logs output messages in the file. It is planned to extend log service with more dynamic and powerful features._

#### Configuring logging

The logs are configured in [_initialization file_](./../framework/areg/resources/log.init), which has simple syntax (the parser is part of the framework). Change appropriate sections to filter log priorities and log scopes, or change the name of the output log file.

An example of filtering logs and scopes:
```
scope.appName.some_unique_scope_name_in_method = SCOPE | DEBUG
scope.appName.some_unique_scope_name_in_other_method = ERROR
scope.appName.some_unique_scope_name_in_somewhere = NOTSET
```
In this example, in application _appName_:
* _some_unique_scope_name_in_method_ scope enables all logs, including _Enter_ and _Exit_ messages of scope.
* _some_unique_scope_name_in_other_method_ scope enables only logs with ERROR and FATAL priority. All other logs, including _Enter_ and _Exit_ messages of the scope, are filtered.
* _some_unique_scope_name_in_somewhere_ scope disables any log.

## How to use multicast router

Multicast Router (MCR) is a part of AREG framework and it is used for inter-process communication (IPC). When start an application with public service, make sure that the **mcrouter** also has been started. Otherwise, the servicing will not work. The local services do not need to use multicast routing **mcrouter** process.

The MCR options are set in [_router.init_](./../framework/areg/resources/router.init) file. Developers mainly need to change IP-address and the port number of the router in this file. By default, the relative path of _router.init_ file is _./config/router.init_.
