# How To
```
This file is part of AREG SDK
Copyright (c) 2017-2021, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document replies to several howto questions.

## Table of content

1. [How to use preprocessor directives](#how-to-use-preprocessor-directives).
2. [How to compile](#how-to-compile).
    - [POSIX build](#posix-build).
    - [make](#make).
    - [Eclipse for C/C++ Developer](#eclipse-for-cc-developer).
    - [Windows build](#windows-build).
    - [Other builds](#other-builds).
3. [How to create a project or integrate in project](#how-to-create-a-project-or-integrate-in-project).
4. [How to use logging](#how-to-use-logging).
    - [Enable logging](#enable-logging).
    - [Configure logging](#configure-logging)
5. [How to use multicast router](#how-to-use-multicast-router)

## How to use preprocessor directives

AREG SDK uses a few preprocessor directives to compile **POSIX** and **Windows** versions of codes. Here is the list:
| Preprocessor | Description and meaning |
| --- | --- |
| **POSIX** | Compile sources with _POSIX API_, details are in [POSIX.md](./POSIX.md). |
| **WINDOWS** | Compile sources with _Win32 API_, details are in [WIN32.md](./WIN32.md). |
| **DEBUG** | Compile Debug configuration. |
| **NDEBUG** | Compile Release configuration. |
| **ENABLE_TRACES** | Enable logging / tracing in sources. |
| **EXP_AREG_LIB** | Build AREG _static_ library (also can use EXPORT_STATIC_SYMBOLS). |
| **EXP_AREG_DLL** | Build AREG _shared_ library (also can use EXPORT_SHARED_SYMBOLS). |
| **IMP_AREG_LIB** | Link with AREG _static_ library (also can use IMPORT_STATIC_SYMBOLS). |
| **IMP_AREG_DLL** | Link with AREG _shared_ library (also can use IMPORT_SHARED_SYMBOLS). |

A _POSIX_ application usually is compiled with _POSIX, DEBUG (NDEBUG), IMP_AREG_LIB (IMP_AREG_DLL), ENABLE_TRACES_ preprocessor directives.

## How to compile

AREG SDK provides **Makefile**, **Eclipse** and **Visual Studio** project files to compile sources and examples. _Makefile_ and _Eclipse_ projects are to build software with _POSIX API_, and _Visual Studio_ project files are to build software with _Win32 API_. By default, there is no need to make additional changes in settings to compile projects. To run **IPC** examples, make sure the process **mcrouter** has started as a console application or as a service handled by the system.

### POSIX build

To build the _POSIX_ version of projects use _make_ or _Eclipse for C/C++ Developers_ IDE to compile with _GCC_ or _clang_ compilers. AREG framework and examples require C++ 17 or higher.

#### make

_make_ and _Makefile_ is used to build AREG framework with _POSIX API_. To build projects, open command line terminal in _AREG SDK root_ directory and do one of these actions:

| Description | Command |
| --- | --- |
| Compile all projects: | `$ make [all]` |
| Compile _framework_ only: | `$ make framework` |
| Compile _examples_ only: | `$ make examples` |

All builds are located in the created `./product/build/<compiler-platform-path>` sub-folder located in the _areg-sdk root_ directory.

To clean an existing build call `$ make clean` command terminal. This removes the entire _'product'_ output directory.

In all cases, parallel compilation with `-j` can be used to speedup the compilation. For example, to compile `areg-sdk` sources with 8 threads:`$ make -j 8`

**Change the compiler settings:**

To change compiler settings, use [user.mk](../conf/make/user.mk) file and edit. Do not commit the developer specific file if other developers use different settings.
* To set the compiler, change _Toolset_. Supported values: _g++_ (default compiler), _gcc_, _clang++-13_. Example: `Toolset      := clang++-13`
* To set the hardware platform, change _Platform_. Supported values: &lt;_no value_&gt; (take default), _x86_, _x86_64_, _arm_, _aarch64_. Example: `Platform := _x86_`.
* To set the cross-compile, change _CrossCompile_. Supported values: &lt;_no value_&gt; (take default), _arm-linux-gnueabihf-g++_ (Linux g++ compiler for arm32), _arm-linux-gnueabihf-gcc_ (Linux GCC compiler for arm32), _aarch64-linux-gnu-g++_ (Linux g++ compiler for arm64), _aarch64-linux-gnu-gcc_ (Linux GCC compiler for arm64). Example: `CrossCompile := arm-linux-gnueabihf-g++`.
* To set the AREG framework library type, change _areg_. Supported values: **_static_** (build and link AREG framework as a static library), **_shared_** (build and linke AREG framework as a shared library). Example: `areg   := static`.
* To set build configuration, change _Config_. Supported values: **_Debug_** (build debug version) and **_Release_** (build release version). Example: `Config := Debug`.
* To set additional compiler preprocessor directives, change _UserDefines_. Example: `UserDefines     := -DENABLE_TRACES`.
* To set additional include directories, change _UserDefIncludes_.
* To set additional library directories, change _UserDefLibPaths_.
* To set additional libraries, change _UserDefLibs_.
* To set the build output directory, change _UserDefOutput_. By default, the binaries are compiled in the `./product` sub-folder created in _AREG SDK root_.
* To set the binary output directory, change _ProjBuildPath_. By default, the path includes compile, hardware, operating system and configuration information.

Examples for the command to use for different settings:

```shell
# Compile areg-sdk with clang using 8 threads
$ make -j 8 Toolset=clang++-13
```
```shell
# Compile areg-sdk with arm-linux-gnueabihf-g++ for arm32
$ make -j 8 CrossCompile=arm-linux-gnueabihf-
```

```shell
# Compile areg-sdk as Debug shared library with aarch64-linux-gnu-gcc
$ make -j 8 Config=Debug CrossCompile=aarch64-linux-gnu- Toolset=gcc
```

#### Eclipse for C/C++ Developer

_Eclipse_ IDE is used to build software with _POSIX API_ in Linux or Windows OS. You need to import the existing _Eclipse_ projects to workspace created in _AREG SDK root_ directory and specify OS suitable _Toolchain_ in the settings. For example, for Windows use _cygwin GCC_ Toolchain.

**How to import projects**:
* Open _Eclipse for C/C++ Developers_ IDE.
* Create a _New Workspace_, specify _areg-sdk_ as workspace root.
* In _Eclipse_ IDE select _Import_ menu (_File_ ==> _Import_) to start importing.
* In the _Select_ dialog select _General_ ==> _Existing Projects into Workspace_.
* Click the _Next_ button, this opens the _Import Projects_ dialog.
* In the _Import Projects_ dialog click _Select_ ==> _Browse_ and set _areg-sdk_ folder.
* After scanning, _Projects_ contains a list of projects.
* Select either areg and mcrouter, or select all to compile all.

The Debug builds are output in _Debug_ and the Release builds are in _Release_ subfolders of each project. By default, _AREG framework_ is compiled as a static library. Change the settings of the framework and project if you need the shared library.

### Windows build

To compile Windows versions of projects, use Visual Studio 2019 or higher to open the solution.

**How to load projects**:
* In Visual Studio open _areg-sdk.sln_ file, located in _areg-sdk_ root folder.
* In the toolbar of Visual Studio select:
    - **Win32** or **x64** in _Solution Platform_ to build 32- or 64-bit binaries.
    - **dbg_vc142** or **rls_vc142** in  _Solution Configuration_ to build Debug or Release configuration.

We have taken the advantage of Visual Studio property files and have chosen this behaviour of _Solution Configuration_ to escape changing the PlatformToolset settings in project files if a team works with different versions of Visual Studio. If you need to support newer versions of Visual Studio, register the _Platform_, _Configuration_ and _PlatformToolset_ in the **config_compile.props** file located in _areg-sdk_ and update _areg-sdk_ solution file.

> 💡 **Note:** _You'll get error if you choose the compiler version, which does not exist in your system!_ 

Example to building 64-bit Release in VS2019: in the _Solution Configurations_ of VS2019 toolbar select **rls_v142** and in the _Solution Platform_ select **x64**. Compile the Solution. All projects are output in the created `./product` subfolder of the _areg-sdk_ root.

### Other builds
Currently the AREG SDK does not contain project files of any other IDE or other scripts like CMake to build from the command line.

## How to create a project or integrate in project

Here is information to integrate the AREG framework in the existing project. 
1. Include **&lt;areg-sdk-root&gt;/framework** in the _include path_ list of the project.
2. Set preprocessor directive **POSIX** or **WINDOWS** to specify _POSIX_ or _Win32_ API build of `areg-sdk`.
3. Set preprocessor directive **DEBUG** for Debug or **NDEBUG** for Release build of applications.
4. Set preprocessor directive **IMP_AREG_DLL** for the `areg` framework shared library or **IMP_AREG_LIB** for the `areg` framework static library.
5. Optionally, set preprocessor directive **ENABLE_TRACES** if need logs / traces.
6. Include library output directory in library search path and specify _AREG framework_ library to link with.

Now your project is ready to use the AREG framework.

The examples of _AREG SDK_ for **POSIX** in Eclipse IDE are built having following settings:
- _areg-sdk/framework_ path is set in the include paths of projects.
- The library search path has _areg-sdk/framework/areg/Debug_ path for Debug and _areg-sdk/framework/areg/Release_ path for Release configuration.
- _areg_, _pthread_ and _rt_ libraries are listed in the list of libraries to link.
- Preprocessor directives for Debug build `POSIX, DEBUG, IMP_AREG_LIB, ENABLE_TRACES` and for Release build `POSIX, NDEBUG, IMP_AREG_LIB, ENABLE_TRACES`.

## How to use logging

#### Creating logs

Tracing / Logging MACRO are declared in the [_GTrace.h_](./../framework/areg/trace/GETrace.h) header file. Use predefined _MACRO_ to configure, enable and output logs. 

- To compile sources with logging, compile the application with _ENABLE_TRACES_ preprocessor directive. If this is not specified, the logs will not exist in binaries. Example: `UserDefines := -DENABLE_TRACES`
- There must be declared _scopes_ to generate logs. 
- Use MACRO _**DEF_TRACE_SCOPE**(some_unique_name_of_scope)_ to declare scope. The scopes must have unique names.
- Use macro _**TRACE_SCOPE**(some_unique_name_of_scope)_ in methods to prepare tracing.
- As soon as the method is executed, the scope generates an _"Enter"_ message to log.
- Use MACRO _TRACE_DBG, TRACE_INFO, TRACE_WARN, TRACE_ERR, TRACE_FATAL_ to log messages by priority.
- Use standard string formatting flags to log messages (similar to _printf()_).
- No need to set _end-of-line_ ('\n') at the end of the message. The tracer automatically adds.
- When exit the method, the scope generates _"Exit"_ message to log.

Example:
```cpp
// A class in project 'something'
class SomeObject
{
public:
    void foo();
    void bar();
};

// define unique scopes
DEF_TRACE_SCOPE(something_SomeObject_foo);
DEF_TRACE_SCOPE(something_SomeObject_bar);

void SomeObject::foo()
{
    TRACE_SCOPE(something_SomeObject_foo);
    TRACE_DBG("Now we can output logs with arguments %s and there is no need %s.", "just like printf() method", "to set new line");
}

void SomeObject::bar()
{
    TRACE_SCOPE(something_SomeObject_bar);
    TRACE_WARN("You may as well output logs by priority");
}

```

#### Enable logging

The application(s) must be compiled with the _TRACE_ENABLE_ preprocessor directive. The _logging service_ is enabled in 2 steps:
1. In the log initialization file (by default, **log.init**) set `log.enable = true` to enable logging service for all applications or `log.enable.appName = true` to enable logging only for a specific application(s).
2. Use `TRACER_START_LOGGING()` macro and specify the log initialization file path to start logging service. If `nullptr`, the default relative path _./config/log.init_ is used.
3. The tracing as well is enabled by calling `Application::initApplication()` method.
4. You may as well filter logs by scopes and priorities in the `log.init` file.

Example of enabling logs:

In `log.init` file
```
log.enable      = true  # enable global logging
log.file        = %home%/logs/%appname%_%time%.log          # create logs in 'log' subfolder of user home 

scope.something.something_SomeObject_bar = DEBUG | SCOPE ;  # enable all logs for specified scope
scope.something.something_SomeObject_foo = NOTSET ;         # disable any log of specific scope
```

in `main.cpp` file:
```cpp
// main.cpp file

DEF_TRACE_SCOPE(something_main);

int main()
{
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, true, true, nullptr, nullptr );
    do
    {
        // initialize scope after logging service is started
        TRACE_SCOPE(something_main);
        TRACE_INFO("Now I can log!");

        SomeObject obj;
        obj.foo(); // <== the log scope is disabled, no logging
        obj.bar(); // <== logs scope "Enter", log message, log "Exit"
    } while (false);

    return 0;
}
```

See the details of initialization in the [log.init](./../framework/areg/resources/log.init) file. Browse [04_trace](./../examples/04_trace/) and other following examples to see how logs are initialized and used. Use macro `TRACER_FORCE_LOGGING()` to force the logging service to start with default settings (enable all log scopes and priorities of the application).

> 💡 Currently logs output messages in the file. Other output settings are reserved and currently have no effect. It is planned to extend log service with more features.

#### Configure logging

The logs are configured in [log.init](./../framework/areg/resources/log.init), which syntax parser is part of the framework. Change appropriate sections to filter log priorities and log scopes, or change the name of the output log file.

An example of filtering logs and scopes:
```
scope.appName.some_unique_scope_name_in_method       = SCOPE | DEBUG # Enable all logs for the scope
scope.appName.some_unique_scope_name_in_other_method = ERROR         # Enable only error message, not scope Enter and Exit
scope.appName.some_unique_scope_name_in_somewhere    = NOTSET        # Disable any log
```
In this example:
* _some_unique_scope_name_in_method_ scope enables all logs, including _Enter_ and _Exit_ messages of scope.
* _some_unique_scope_name_in_other_method_ scope enables only ERROR and FATAL logs. All other logs, including scope _Enter_ and _Exit_ are filtered out.
* _some_unique_scope_name_in_somewhere_ scope disables any log.

## How to use multicast router

Multicast Router (MCR) is a part of AREG SDK and it is used for inter-process communication (IPC). The multicast router and the applications must have a configured [_router.init_](./.../framework/areg/resources/router.init) file located in the `./config` subfolder of binaries. Set the IP-address and the port number of the multicast router to connect. 

```
connection.address.tcpip    = 127.0.0.1	# the address of mcrouter host
connection.port.tcpip       = 8181      # the connection port of mcrouter
```
When starting an application with _Public_ service(s), make sure that the **mcrouter** is started. If your application uses only _Local_ services, it does not need the multicast router.
