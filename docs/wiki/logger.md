# AREG SDK Log Collector Service

The **AREG SDK Log Collector** is a centralized service designed to gather, manage, and route log data from distributed applications within the AREG SDK environment. It forwards collected log entries to designated log observers for streamlined analysis and handling, enabling real-time monitoring, troubleshooting, and performance optimization in complex multiprocess systems.

## Table of Contents
1. [General Information](#general-information)
2. [Log Collector Setup](#log-collector-setup)
3. [Log Collector Configuration](#log-collector-configuration)
4. [Command Line Options](#command-line-options)
5. [Connection Initialization](#connection-initialization)
   - [Logger Initialization](#logger-initialization)
   - [Application Setup](#application-setup)

---

## General Information

The AREG SDK Log Collector acts as a networking service within the AREG SDK's logging ecosystem. It facilitates centralized log collection by receiving logs from multiple applications, making them accessible to log observers for in-depth analysis and management. This service enables tailored log configurations, including specific scopes and priority levels, promoting efficient tracking and debugging across distributed applications. By integrating with the AREG SDK’s logging tools, the Log Collector enhances monitoring capabilities and provides an efficient pathway for real-time diagnostics in complex multiprocess scenarios.

---

## Log Collector Setup

The Log Collector is implemented within the AREG SDK [logger module](./../../framework/logger/) and compiled as a standalone executable that can run as a console application or a managed service on Windows and Linux. Deployment can occur on any networked machine with a **General Purpose Operating System (GPOS)**.

**Running as a Service:**

- **On Windows:**
  - Execute `logger.service.install.bat`, ensuring the correct path to `logger`, and restart if needed.
  - Alternatively, use `logger.exe --install` in Terminal or PowerShell.

- **On Linux:**
  - Configure the `logger.service` file with the correct `ExecStart` path.
  - Use `sudo systemctl enable logger.service` and `sudo systemctl start logger.service` to enable and start the service.

To stop the service:
- **On Windows:** Run `logger.service.uninstall.bat`, or use `logger.exe --uninstall`, and restart or manage through the Services menu.
- **On Linux:** Run `sudo systemctl stop logger.service` and `sudo systemctl disable logger.service`.

---

## Log Collector Configuration

The **logger** settings are managed within a configuration file, typically found as [areg.init](./../../framework/areg/resources/areg.init). Settings use key-value pairs in the format `section::(module|*)::property[::(position|*)]`. Example configurations for the Log Collector:

```plaintext
logger::*::service          = logger        # Log Collector process name
logger::*::connect          = tcpip         # Communication protocol
logger::*::enable::tcpip    = true          # Enable TCP/IP
logger::*::address::tcpip   = 172.23.96.1   # IP address (default: 127.0.0.1)
logger::*::port::tcpip      = 8282          # Port number (default: 8282)
```

These configurations enable applications to establish network connections with the Log Collector for logging. Applications can log messages, and log observers receive the data for real-time insights. The Log Collector can also route commands from observers, such as adjusting log scopes and filters for dynamic, application-specific logging.

> [!TIP]
> To customize settings for individual applications, replace the wildcard `*` with the application's name (e.g., `logger::someapp::enable::tcpip = false` for disabling network logging on `someapp`).

Setting descriptions:
- `logger::*::service` .......: Specifies the log collector process name.
- `logger::*::connect` .......: Lists supported protocols (TCP/IP in this example).
- `logger::*::enable::tcpip` .: Activates or deactivates the protocol.
- `logger::*::address::tcpip` : Specifies the Log Collector’s network-accessible IP.
- `logger::*::port::tcpip` ...: Assigns the port number.

For further details, refer to the [AREG SDK Persistence Syntax documentation](./persistence-syntax.md).

> [!NOTE]
> AREG SDK includes a simple console application, `logobserver`, for capturing logs, adjusting log scopes, filtering priorities, and saving logs to a file or SQLite database. More details are available in the [logobserver documentation](./logobserver.md).

---

## Command Line Options

The Log Collector can run as a console application or OS-managed service. Key command-line options are listed below for starting, configuring, and controlling its operation. Some options, marked as **Console**, can be entered while `logger` is active; others start the program.

| Command               | Platform  | Description                   | Usage Example                 |
|-----------------------|-----------|-------------------------------|-------------------------------|
| `-a, --save`          | Console   | Save logs in a text file      | `-a`                          |
| `-b, --unsave`        | Console   | Stop saving logs              | `-b`                          |
| `-c, --console`       | All       | Run as a console application  | `logger --console`            |
| `-e, --query`         | Console   | Display list of log scopes    | `-e someapp`                  |
| `-f, --config`        | Console   | Save current configuration    | `-f`                          |
| `-h, --help`          | All       | Display help message          | `logger --help`               |
| `-i, --install`       | Windows   | Install as a service          | `logger --install`            |
| `-l, --load`          | All       | Load configuration file       | `logger -l="config.init"`     |
| `-n, --instances`     | Console   | Show connected instances      | `-n`                          |
| `-o, --scope`         | Console   | Set scope priority            | `-o someapp::scope_*=DEBUG`   |
| `-p, --pause`         | Console   | Pause log collector           | `-p`                          |
| `-q, --quit`          | Console   | Quit console application      | `-q`                          |
| `-r, --restart`       | Console   | Restart connections           | `-r`                          |
| `-s, --service`       | Linux     | Start as a service            | `logger --service`            |
| `-t, --silent`        | Console   | Run silently                  | `-t`                          |
| `-u, --uninstall`     | Windows   | Uninstall service             | `logger --uninstall`          |
| `-v, --verbose`       | Console   | Show data rate                | `-v`                          |

Commands may vary with version updates.

---

## Connection Initialization

### Logger Initialization

By default, `logger` loads settings from `areg.init`. Specify a different configuration file as follows:

```bash
logger --load="./my/config/file.init"
```

> [!NOTE]
> By default, in `areg.init` configuration file the logs of `logger` and `logobserver` are deactivated:
> ```plaintext
> log::logger::scope::* = NOTSET
> log::logobserver::scope::* = NOTSET
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
