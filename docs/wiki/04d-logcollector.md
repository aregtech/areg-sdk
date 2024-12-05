# AREG SDK Log Collector Service

The **AREG SDK Log Collector** is a centralized service designed to gather, manage, and route log data from distributed applications within the AREG SDK environment. It forwards collected log entries to designated log observers for streamlined analysis and handling, enabling real-time monitoring, troubleshooting, and performance optimization in complex multiprocess systems.

## Table of Contents
1. [General Information](#1-general-information)
2. [Running Log Collector as a Service](#2-running-log-collector-as-a-service)
   - [Linux Service](#linux-service)
   - [Windows Service](#windows-service)
3. [Log Collector Configuration](#3-log-collector-configuration)
4. [Command Line Options](#4-command-line-options)
5. [Connection Initialization](#5-connection-initialization)
   - [Log Collector Initialization](#log-collector-initialization)
   - [Application Setup](#application-setup)

---

## 1. General Information

The AREG SDK Log Collector acts as a networking service within the AREG SDK's logging ecosystem. It facilitates centralized log collection by receiving logs from multiple applications, making them accessible to log observers for in-depth analysis and management. This service enables tailored log configurations, including specific scopes and priority levels, promoting efficient tracking and debugging across distributed applications. By integrating with the AREG SDK's logging tools, the Log Collector enhances monitoring capabilities and provides an efficient pathway for real-time diagnostics in complex multiprocess scenarios.

---

## 2. Running Log Collector as a Service

The Log Collector (`logcollector`) is implemented within the AREG SDK [Log Collector module](./../../framework/logcollector/) and compiled as a standalone executable that can operate as either a console application or an OS-managed service on Windows and Linux platforms. Deployment can occur on any networked machine with a **General Purpose Operating System (GPOS)**.

### Linux Service

To configure and run the `logcollector` application as a Linux-managed service, follow these steps:

1. **Copy Service Configuration File**:
   - Copy the `logcollector.service` file to the `/etc/systemd/system/` directory.

2. **Copy the Executable**:
   - Copy the built `logcollector.elf` executable to the desired location, such as `/usr/local/bin`.

3. **Ensure Library Access (if applicable)**:
   - If the AREG Framework was built as a shared library, ensure that `logcollector` has access to the `libareg.so` library (e.g., located in `/usr/lib`).

4. **Edit the Service Configuration**:
   - Open the `logcollector.service` file and verify that the `ExecStart` path points to the correct location of the `logcollector.elf` executable. For example:
     ```plaintext
     ExecStart=/usr/local/bin/logcollector.elf --service
     ```
   - Ensure the `ExecStart` line includes the `--service` (or `-s`) as a command line option.

5. **Enable and Start the Service**:
   - Enable the service to start automatically at boot:
     ```bash
     sudo systemctl enable logcollector.service
     ```
   - Start the service:
     ```bash
     sudo systemctl start logcollector.service
     ```

6. **Stop or Disable the Service**:
   - To stop the service:
     ```bash
     sudo systemctl stop logcollector.service
     ```
   - To disable the service from starting at boot:
     ```bash
     sudo systemctl disable logcollector.service
     ```

### Windows Service

To configure and run the `logcollector` application as a Windows-managed service, follow these steps:

1. **Copy the Binaries**:
   - Copy the `logcollector.exe` and `areg.dll` binaries to the desired location.

2. **Install the Service**:
   - Open **PowerShell** as the Administrator.
   - Register the executable as a service by running:
     ```powershell
     .\logcollector.exe --install
     ```
   - Alternatively, you can execute `logcollector.service.install.bat` as **Administrator**, ensuring the correct path to `logcollector.exe`.

3. **Start the Service**:
   - Open the **Services** application (or run `services.msc` in the Command Prompt).
   - Locate the service named **AREG Log Collector Service**.
   - Start the service by right-clicking it and selecting **Start**.

4. **Stop and Uninstall the Service**:
   - Stop the service using the **Services** application or the `services.msc` command.
   - Uninstall the service by running the following command in PowerShell:
     ```powershell
     .\logcollector.exe --uninstall
     ```
   - Alternatively, you can execute `logcollector.service.uninstall.bat` as **Administrator**, ensuring the correct path to `logcollector.exe`.

---

## 3. Log Collector Configuration

The **Log Collector** settings are managed within a configuration file, typically found as [areg.init](./../../framework/areg/resources/areg.init). Settings use key-value pairs in the format `section::(module|*)::property[::(position|*)]`. Example configurations for the Log Collector:

```plaintext
logger::*::service          = logcollector  # Log Collector process name
logger::*::connect          = tcpip         # Communication protocol
logger::*::enable::tcpip    = true          # Enable TCP/IP
logger::*::address::tcpip   = 172.23.96.1   # IP address (default: 127.0.0.1)
logger::*::port::tcpip      = 8282          # Port number (default: 8282)
```

These configurations enable applications to establish network connections with the Log Collector for logging. Applications can log messages, and log observers receive the data for real-time insights. The Log Collector can also route commands from observers, such as adjusting log scopes and filters for dynamic, application-specific logging.

> [!TIP]
> To customize settings for individual applications, replace the wildcard `*` with the application's name (e.g., `logcollector::someapp::enable::tcpip = false` for disabling network logging on `someapp`).

Setting descriptions:
|  Property Key Setting:        |   Description:                                        |
|-------------------------------|-------------------------------------------------------|
| `logger::*::service`          | Specifies the log collector process name.             |
| `logger::*::connect`          | Lists supported protocols (TCP/IP in this example).   |
| `logger::*::enable::tcpip`    | Activates or deactivates the protocol.                |
| `logger::*::address::tcpip`   | Specifies the Log Collector's network-accessible IP.  |
| `logger::*::port::tcpip`      | Assigns the port number.                              |

For further details, refer to the [AREG SDK Persistence Syntax documentation](./06a-persistence-syntax.md).

> [!NOTE]
> AREG SDK includes a console tool application, `logobserver`, for capturing logs, adjusting log scopes, filtering priorities, and saving logs to a file or SQLite database. More details are available in the [logobserver documentation](./04c-logobserver.md).

---

## 4. Command Line Options

The Log Collector can run as a console application or OS-managed service. Key command-line options are listed below for starting, configuring, and controlling its operation. Some options, marked as **Console**, can be entered while `logcollector` is active; others start the program.

| Command:              | Platform: | Description:                  | Usage Example:                    |
|-----------------------|-----------|-------------------------------|-----------------------------------|
| `-a, --save`          | Console   | Save logs in a text file      | `-a`                              |
| `-b, --unsave`        | Console   | Stop saving logs              | `-b`                              |
| `-c, --console`       | All       | Run as a console application  | `logcollector --console`          |
| `-e, --query`         | Console   | Display list of log scopes    | `-e someapp`                      |
| `-f, --config`        | Console   | Save current configuration    | `-f`                              |
| `-h, --help`          | All       | Display help message          | `logcollector --help`             |
| `-i, --install`       | Windows   | Install as a service          | `logcollector --install`          |
| `-l, --load`          | All       | Load configuration file       | `logcollector -l="config.init"`   |
| `-n, --instances`     | Console   | Show connected instances      | `-n`                              |
| `-o, --scope`         | Console   | Set scope priority            | `-o someapp::scope_*=DEBUG`       |
| `-p, --pause`         | Console   | Pause log collector           | `-p`                              |
| `-q, --quit`          | Console   | Quit console application      | `-q`                              |
| `-r, --restart`       | Console   | Restart connections           | `-r`                              |
| `-s, --service`       | Linux     | Start as a service            | `logcollector --service`          |
| `-t, --silent`        | Console   | Run silently                  | `-t`                              |
| `-u, --uninstall`     | Windows   | Uninstall service             | `logcollector --uninstall`        |
| `-v, --verbose`       | Console   | Show data rate                | `-v`                              |

Commands may vary with version updates.

---

## 5. Connection Initialization

### Log Collector Initialization

By default, `logcollector` loads settings from `areg.init`. Specify a different configuration file as follows:

```bash
logcollector --load="./my/config/file.init"
```

> [!NOTE]
> By default, in `areg.init` configuration file the logs of `logcollector` and `logobserver` are deactivated:
> ```plaintext
> log::logcollector::scope::*   = NOTSET
> log::logobserver::scope::*    = NOTSET
> ```

### Application Setup

Applications connect to the Log Collector by initializing their log manager and specifying the configuration file path:

```cpp
#include "areg/appbase/Application.hpp"

int main()
{
    // start: logging, service manager, router client, timer manager, watchdog manager
    // configure connections from "./config/myapp-services.init" file
    Application::initApplication(true, true, true, true, true, "./config/myapp-services.init", nullptr);

    // Application-specific operations

    Application::releaseApplication();
    return 0;
}
```

Using `Application::initApplication()`, the application initializes its connection to the Log Collector, enabling log message transfer and allowing real-time adjustments from `logobserver`.
