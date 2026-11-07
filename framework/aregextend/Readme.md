# Directory Overview: Areg Extend Library

This directory contains the source code for the Areg Extend library (`aregextend`), a collection of optional objects that extend `areg`: a console, SQLite database helpers and OS-managed services. None of them is required by the framework core. The library is compiled as a static library and linked only by the projects that use these objects.

### Directory Contents
- **[console](./console)**
  Implements a thread safe console that outputs messages at given positions and reads user input. It draws with ANSI escape sequences on Linux, macOS and Windows: output is clipped to the terminal, never wraps or scrolls, and the cursor stays on the input line. The input line is editable, and Up and Down recall the entered lines (16 by default, at most 64, set with `set_history_size()`). When the output is redirected to a file or a pipe, it writes plain lines.

- **[db](./db)**
  Provides basic objects for creating and managing SQLite databases.

- **[service](./service)**
  Includes objects for creating and running operating system-managed services.

- **[resources](./resources)**
  Contains resources specific to the `aregextend` library.

### Platform Support and Internal Headers
Where the implementations for `Win32` and `POSIX` systems differ, the behavior of objects is consistent across platforms.

Header files located in the `private` subdirectories are intended for internal use only and should not be included directly in external projects.
