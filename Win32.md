```
This file is part of AREG SDK, the multi-tasking application development platform.
Copyright (c) Aregtech, 2017-2021
Contact: info[at]aregtech.com
```

This file describes the instruction to compile **AREG SDK** and example source codes under _Windows_ platform with the help of _Visual Studio_. The minimum requirement to compile sources is **Visual Studio 10 or higher** version. Open **areg-sdk.sln** Visual Studio solution file in your preferred Visual Studio IDE and compile **areg** or all projects. By default, the **AREG** connectivity framework is compiled as _dynamically linked library (areg.dll)_. BY default, all sources are build in **”product/build/xxx””** folders.

**AREG SDK** and examples can be build in 32- or 64-bit environment. To change the configuration, select _Solution Configuration_ and _Solution Platform_ in Visual Studio. All Debug builds start with prefix **’dbg_’** and all Release build start with prefix **’rls_’**. The output folder of Debug builds are located in folders with prefix **'dbg_'** folled with tool version (for example, **‘dbg_vc110’** indicates that it is _Debug_ build used VS2012 compiler, or **’rls_v142’** is _Release_ build used VS2019 compiler).


To build **AREG SDK** connectivity framework, use **WINDOWS** (or **_WINDOWS**) preprocessor directive. 

**Other preprocessor directives:**
- **DEBUG** to compile debug version
- **NDEBUG** to compile release version
- **ENABLE_TRACES** to enable tracing / logging (requires additional space)
- **EXP_AREG_LIB** or **EXPORT_STATIC_SYMBOLS** to compile sources as static library
- **EXP_AREG_DLL** or **EXPORT_SHARED_SYMBOLS** to compile sources as dynamically linked library (.dll)
- **IMP_AREG_LIB** or **IMPORT_STATIC_SYMBOLS** to link static library
- **IMP_AREG_DLL** or **IMPORT_SHARED_SYMBOLS** to link with dynamically linked library (.dll)
