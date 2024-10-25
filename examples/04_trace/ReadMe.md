# 04_trace Project Overview

The **04_trace** project showcases how to implement **logging (tracing)** in applications using the AREG SDK. This example demonstrates how to enable, configure, and manage logging for monitoring application behavior, which is essential for debugging, performance analysis, and log management.

## Key Concepts

- **Logging and Tracing**: Understand how to set up logging to monitor application runtime behavior and performance using the AREG SDK's logging capabilities.
- **Preprocessor Definitions**: Enable or disable compilation with logs in your application by setting the `AREG_LOGS=ON` or `AREG_LOGS=OFF` preprocessor definition during the build process.
- **Log Scopes and Logging Priorities**: Define **log scopes** to categorize logs and utilize **logging priorities** to filter logs based on their importance (e.g., Error, Warning, Info, Debug).
- **Log Management Tools**: The AREG SDK offers tools like **logger** and **logobserver** for centralized log collection, log management, and scope manipulation. These tools help in **log filtering**, scope adjustments, and log priority settings. For more details on using these tools, refer to the **[AREG SDK documentation](./../../docs/wiki/README.md)**.
- **Configuration File**: Use the `areg.init` configuration file to activate and configure logging, including defining log filters based on scope and priority.

## Features

- **Build with Logs**: Logging can be enabled or disabled during the build process using the `AREG_LOGS` option in CMake. Set it to `ON` to include logging or `OFF` to exclude logs in the final build.
  - Example command to enable logging: `cmake -B ./build -DAREG_LOGS:BOOL=ON`
  - Example command to disable logging: `cmake -B ./build -DAREG_LOGS:BOOL=OFF`
- **Log Scopes**: Log scopes allow you to organize log entries into categories, making it easier to track specific areas of your application, and use them to measure performance. These scopes can be dynamically enabled or disabled, providing flexibility in log management.
- **Log Priorities**: Assign different priorities to log messages (e.g., Error, Warning, Info, Debug) to focus on specific types of logs, especially during troubleshooting and debugging.
- **Log Configuration File**: The `areg.init` file controls logging parameters, including which log scopes are active and which log priorities are captured. The file is usually located in the `./config` directory of the application.
- **Default Logging Behavior**: Even without an `areg.init` file, logging can be enabled with default settings, ensuring that important logs are captured at runtime.
- **Saving Logs**: The logs can be saved in the file as plain text of in the SQLite database if use `logger` service application and `logobserver` tool, which are part of AREG SDK.

## Project Structure

- **Log Activation**: This example shows how to activate logging via CMake options and the `areg.init` configuration file.
- **Pre-Configured Logging**: The project comes with pre-configured logs, allowing you to see how logging is implemented out-of-the-box.
- **Log Scopes**: It demonstrates how to define and utilize log scopes for organizing log entries.
- **Log Message Priorities**: Learn how to log messages with varying priorities, helping you prioritize and filter logs.
- **Log Management Tools**: Explore the AREG SDK's built-in tools for **log collection** and **log management**, such as the `logger` and `logobserver` applications.
- **Log Filtering**: This example shows how to adjust log settings and filters dynamically using either the `areg.init` configuration file or the **logobserver** tool.

## Use Cases

- **Debugging and Application Monitoring**: This project is ideal for tracking application behavior, identifying bugs, and monitoring performance in real-time.
- **Customizable Logging**: Developers can easily customize logging by adjusting scopes, priorities, and formats in the `areg.init` file to meet the needs of different environments and use cases.
- **Centralized Logging Setup**: Learn how to implement centralized logging, where logs from multiple processes or threads can be collected, filtered, and managed in one place.

## Conclusion

The **04_trace** project provides a practical guide for implementing **logging (tracing)** in AREG SDK applications. By utilizing logging scopes, message priorities, and log management tools like `logger` and `logobserver`, developers can efficiently monitor, collect, and filter logs for debugging and performance analysis. With easy setup via CMake and configuration through the `areg.init` file, the AREG SDK makes **log management** simple, powerful, and flexible for any application.
