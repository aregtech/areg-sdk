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
