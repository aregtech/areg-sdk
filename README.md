This file is part of AREG SDK, the multi-tasking application development platform.
Copyright (c) Aregtech, 2019-2021
Contact: info[at]aregtech.com

Current sources of AREG SDK support Windows and POSIX API

areg-sdk
    |
    +- test --> this is a test

File system structure:

areg-sdk            --> the root folder of AREG SDK
    |
    +- dev          --> the source codes of AREG Framework and Multicasting Router
    |   |
    |   +- areg     --> the source codes of AREG Framework
    |   |
    |   +- mcrouter --> the sources of Multicasting Router
    |   |
    |   +- system   --> the development platform specific common codes
    |
    +- examples     --> examples and tests of AREG SDK
    |
    +- tools        --> code generator
    |
    +- License.txt  --> the license. Please read before use or distribute.
    
The compiled codes output in "areg-sdk/product/build/<platform info>" folder

Preprocessor directives:

To compile sources with WIndows API, specify WINDOWS (or _WINDOWS) preprocessor directive.
To compile sources with POSIX API, specify POSIX (or _POSIX) preprocessor directive.

Other preprocessor directives:
- DEBUG to compile debug version
- NDEBUG to compile release version
- ENABLE_TRACES to enable tracing / logging
- EXP_AREG_LIB or EXPORT_STATIC_SYMBOLS to compile SDK sources as static library
- EXP_AREG_DLL or EXPORT_SHARED_SYMBOLS to compile sources as shared / dynamic linked library
- IMP_AREG_LIB or IMPORT_STATIC_SYMBOLS to link SDK static lirary
- IMP_AREG_DLL or IMPORT_SHARED_SYMBOLS to link SDK shared / dynamic linked library
- OUTPUT_DEBUG_LEVEL=N, where N is one of following numbers to enable debug outputs: 
    -- 0 no outputs
    -- 1 output only errors
    -- 2 outpt errors and warnings
    -- 3 output errors, warnings and information
    -- 4 or higher output all


In source codes, all paths are relative to "areg-sdk/dev" folder

To compile with Visual Studio, use areg-sdk.sln
To compile with Eclipse / GCC, include appropriate projects in "areg-sdk/dev/xxx" folders and, by wish, "areg-sdk/examples/xxx" folders.
