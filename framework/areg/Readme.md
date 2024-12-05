# Directory Overview: AREG Framework

This directory contains the source code for the AREG Framework, also known as the AREG communication engine. It is built as the `areg` library, available in both shared and static configurations. Below is an overview of the directory's contents:

### Directory Contents

- **[appbase](./appbase)**
  Provides base classes for initializing and managing application lifecycles.

- **[base](./base)**
  Contains foundational helper objects for general-purpose functionality.

- **[component](./component)**
  Implements the core business logic of the communication engine, enabling the creation of service components and automation processes.

- **[ipc](./ipc)**
  Includes components for inter-process communication (IPC).

- **[logging](./logging)**
  Manages the logging functionality of the AREG engine.

- **[persist](./persist)**
  Provides utilities for persisting data as key-value pairs in files.

- **[resources](./resources)**
  Contains resources utilized by the AREG engine.

- **[system](./system)**
  Houses platform-dependent preprocessor definitions.

### Platform Support and Usage
While the implementation of certain objects varies between POSIX and Win32 APIs, their behavior is consistent across platforms.

### Internal Headers
Header files located in the `private` subdirectories are for internal use only and should not be included directly in external projects.
