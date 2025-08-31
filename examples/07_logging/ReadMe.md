# 07_logging
🧵 **Type:** Multithreading / Single Process

## Overview
The **07_logging** project demonstrates how to implement and manage **logging** in applications using the AREG SDK. It shows how to enable, configure, and control logs, providing a foundation for **debugging, performance monitoring, and log management** in multithreaded applications.

## Concepts Shown
- **Logging and Tracing**: Capture runtime behavior and performance through scoped logs with configurable priorities.  
- **Preprocessor Control**: Enable or disable logging at build time using `AREG_LOGS=ON/OFF`.  
- **Log Scopes & Priorities**: Categorize logs by functional areas and filter them by priority (Error, Warning, Info, Debug).  
- **Configuration File (`areg.init`)**: Dynamically define active scopes and priorities for runtime logging.  
- **Log Management Tools**: Use `logcollector` and `logobserver` for centralized collection, filtering, and scope adjustments.

## How It Works
Logging in AREG SDK is controlled via a combination of **build-time settings** and **runtime configuration**:

- **Build-Time**: Use the `AREG_LOGS` CMake option to include or exclude logging in the final build.  
- **Runtime**: The `areg.init` configuration file defines active scopes and priorities. Default logging works even if the file is absent.  
- **Log Management**: Developers can view, filter, and store logs using `logcollector` (service application) and `logobserver` (GUI/CLI tool). Logs can be saved as plain text or stored in a SQLite database.  
- **Scoped & Prioritized Logging**: Each log message can belong to a scope and has a priority, enabling flexible filtering and structured debugging.

## Use Cases
- **Debugging and Monitoring**: Track application behavior in real time and identify issues efficiently.  
- **Custom Logging Configurations**: Adjust scopes, priorities, and filters dynamically to fit different environments.  
- **Centralized Log Management**: Aggregate logs from multiple threads or processes for analysis and auditing.

## Takeaway
The **07_logging** project provides a clear, practical example of implementing **scalable and configurable logging** in multithreaded AREG applications, using both build-time controls and runtime management tools.
