# Build with Win32 API
```
This file is part of AREG SDK
Copyright (c) Aregtech, 2017-2021
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document describes the instruction to compile **AREG SDK** and example source codes under _Windows_ 32- or 64-bit platform using Visual Studio 2019 or higher version.

Open **[areg-sdk.sln](./../areg-sdk.sln)** solution file in VS2019 and compile the solution. By default, the _AREG framework_ is compiled as a _dynamically linked library (areg.dll)_, all sources are built in the created `product/build/xxx` subfolder of _areg-sdk_.

Before compiling, in the toolbar of Visual Studio IDE select the appropriate _Solution Configuration_ and _Platform Configuration_ of your compiler. _For example_, 64-bit Release builds using Visual Studio 2019 can be done in the following way: in _Solution Configuration_ select _rls_vc142_ and _x64_ in _Solution Platform_. 

To build _AREG framework_ with _Win32 API_, **WINDOWS** (or **_WINDOWS**) preprocessor directive must be defined. 

**Other preprocessor directives to use to compile:**
- **DEBUG** to compile the debug version.
- **NDEBUG** to compile the release version.
- **ENABLE_TRACES** to compile sources with tracing / logging.
- **EXP_AREG_LIB** to compile the _AREG framework_ as a static library.
- **EXP_AREG_DLL** to compile the _AREG framework_ as a dynamically linked library.
- **IMP_AREG_LIB** to link a project with _AREG framework_ static library.
- **IMP_AREG_DLL** to link a project with _AREG framework_ dynamically linked library.

The [HOWTO.md](./HOWTO.md) document contains description [to select solution configuration and the platform](./HOWTO.md#windows-build) and [to integrate _AREG framework_](./HOWTO.md#how-to-create-a-project-or-integrate-in-project) in the existing or new project.
