# Directory Overview: AREG Extend Library

This directory contains the source code for the AREG Extend library (`aregextend`), which provides optional extended features. These features may depend on additional libraries and resources, enabling enhanced functionality. The library is compiled as a static library to link with projects requiring these extensions.

### Compilation Options
To include extended features, define `AREG_EXTENDED=1` during compilation (CMake option `-DAREG_EXTENDED=ON`). If omitted or set to `AREG_EXTENDED=0` (CMake option `-DAREG_EXTENDED=OFF`), the library will be compiled with limited functionality. In this case, while the library can be linked, extended features may not function as originally intended.

### Directory Contents
- **[console](./console)**
  Implements console extensions for advanced cursor manipulation and message output at specific positions.
  - **Linux**: Uses `ncurses` for enhanced cursor management.
  - **Windows**: Uses cursor positioning `Win32 API` for better control over cursor behavior.
  - **Fallback**: If `AREG_EXTENDED=0` (or `-DAREG_EXTENDED=OFF`), the implementation relies on ANSI C APIs and escape codes. 
  > [!NOTE]
  > Concurrent querying of cursor position and waiting for user input in a multithreaded environment may result in unpredictable behavior. Use cursor save/restore functions to mitigate issues.

- **[db](./db)**
  Provides basic objects for creating and managing SQLite databases.

- **[service](./service)**
  Includes objects for creating and running operating system-managed services.

- **[resources](./resources)**
  Contains resources specific to the `aregextend` library.

### Platform Support and Internal Headers
While the implementations for `Win32` and `POSIX` systems differ, the behavior of objects is consistent across platforms.

Header files located in the `private` subdirectories are intended for internal use only and should not be included directly in external projects.
