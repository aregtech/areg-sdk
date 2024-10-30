# Log Collector Service

The **Log Collector** in AREG SDK is a centralized logging service designed to collect and manage log data from distributed applications. 

## Table of Contents
1. [General Information](#general-information)
2. [Log Collector Setup](#log-collector-setup)
3. [Log Collector Configuration](#log-collector-configuration)
4. [Command Line Options](#command-line-options)
5. [Connection Initialization](#connection-initialization)
   - [logger Initialization](#logger-initialization)
   - [Application Setup](#application-setup)

---

## General Information

The **AREG SDK Log Collector** is a networked service within the AREG SDK logging infrastructure, designed for collecting logs from distributed applications and forwarding them to log observers for analysis and management. It enables centralized log collection, capturing logs across various processes and systems, and offers flexible configuration for log scope and priority settings. By integrating with the AREG SDK logging module, the Log Collector provides a streamlined approach to monitor, debug, and optimize applications, enhancing diagnostic capabilities in real-time for complex, multiprocess environments.

---

## Log Collector Setup

The Log Collector is implemented in the [logger](./../../framework/logger/) module and compiled as a standalone executable. It can run either as a console application or as a service managed by Windows or Linux. The Log Collector can be deployed on any networked machine with a **General Purpose Operating System (GPOS)**.

**To run as a service:**

- **On Windows:** 
  - Run `logger.service.install.bat`, ensure the correct `logger` path, and restart the system.
  - Alternatively, run `logger.exe --install` in the Terminal or PowerShell.

- **On Linux:**
  - Use `logger.service` file, ensuring the correct path in `ExecStart`.
  - Run `sudo systemctl enable logger.service` and `sudo systemctl start logger.service`.

  To stop running the service:
- **On Windows**: run `logger.service.uninstall.bat` or `logger.exe --uninstall`, and restart system or stop service in the Service List.
- **On Linux**: run `sudo systemctl stop logger.service` and `sudo systemctl disable logger.service`.
 
---

## Log Collector Configuration

The **logger** uses a configuration file, typically [areg.init](./../../framework/areg/resources/areg.init), with settings defined as key-value pairs. Each setting uses the format `section::(module|*)::property[::position]`. Below is a sample configuration for `logger` in `areg.init`:

```text
logger::*::service          = logger        # Log Collector process name
logger::*::connect          = tcpip		    # Supported communication protocols
logger::*::enable::tcpip    = true		    # Enable/disable protocol
logger::*::address::tcpip   = 172.23.96.1   # Connection IP address (default: 127.0.0.1)
logger::*::port::tcpip      = 8282		    # Connection port number (default: 8282)
```

All application use these settings to establish the network connection with Log Collector service. The applications that generate logging messages, send the log messages to the log collector and the lob observers such as log viewers receive the messages. The Log Collector also plays a role of message router, where the log observers may send logging scope activation of message filtering instructions to the log generating applications. The developers may replace the wildcard `*` with the name of application if the application should have another setting. For example, if an application `someapp` should not have network accessible logging, add the following property in your configuration file: `logger::someapp::enable::tcpip = false`. This setting will refer only to `someapp` application and will not affect other applications.

Explanation of each setting:
- `logger::*::service` .......: Specifies the log collector process name.
- `logger::*::connect` .......: Lists supported protocols (TCP/IP in this example).
- `logger::*::enable::tcpip` .: Enables or disables the protocol.
- `logger::*::address::tcpip` : Provides the logger’s network-accessible IP.
- `logger::*::port::tcpip` ...: Assigns the port number.

For further details, refer to the [AREG SDK Persistence Syntax](./persistence-syntax.md).

> [!NOTE]
> AREG SDK provides a simple console application `logobserver` to receive the logs from Log Collector, manipulate the scopes, filter priorities and save logs in the file or SQLite database. For more information, refer to the [logobserver](./logobserver.md) AREG SDK Wiki documentation.

---

## Command Line Options

The `logger` Log Collector process can be run as either a console application or an OS-managed service. It has various commands for starting, configuring, and controlling its operation. Some commands are entered while `logger` is running in the console, marked as **Console** in the options list. All others are command-line options to start the program.

| Command               | Platform  | Description                   | Usage Example                 |
|-----------------------|-----------|-------------------------------|-------------------------------|
| `-a, --save`          | Console   | Save logs in the text file    | `-a`                          |
| `-b, --unsave`        | Console   | Stop saving logs in the file  | `-b`                          |
| `-c, --console`       | All       | Run as a console application  | `logger --console`            |
| `-e, --query`         | Console   | Query the list of scopes      | `-e someapp`                  |
| `-f, --config`        | Console   | Save current configuration    | `-f`                          |
| `-h, --help`          | All       | Display help message          | `logger --help`               |
| `-i, --install`       | Windows   | Install as a service          | `logger --install`            |
| `-l, --load`          | All       | Load configuration            | `logger -l="config.init"`     |
| `-n, --instances`     | Console   | Display connected instances   | `-n`                          |
| `-o, --scope`         | Console   | Update log scope priority     | `-o someapp::some_obj_*=DEBUG`|
| `-p, --pause`         | Console   | Pause log collector           | `-p`                          |
| `-q, --quit`          | Console   | Quit console application      | `-q`                          |
| `-r, --restart`       | Console   | Restart connections           | `-r`                          |
| `-s, --service`       | Linux     | Start as a service            | `logger --service`            |
| `-t, --silent`        | Console   | Run silently                  | `-t`                          |
| `-u, --uninstall`     | Windows   | Uninstall service             | `logger --uninstall`          |
| `-v, --verbose`       | Console   | Display data rate             | `-v`                          |

Commands may be updated over time.

---

## Connection Initialization

### logger Initialization

By default, `logger` loads settings from `areg.init`. For a custom configuration file, specify it like this:

```bash
logger --load="./my/config/file.init"
```

> [!NOTE]
> In `areg.init` configuration file the logs of `logger` and `logobserver` are disabled:
> ```text
> log::logger::scope::* = NOTSET
> log::logobserver::scope::* = NOTSET
> ```

### Application Setup

Applications establish a `logger` connection by starting logging manager and specifying the path to their configuration file:

```cpp
#include "areg/appbase/Application.hpp"

int main()
{
    // start: logging, service manager, router client, timer manager, watchdog manager
    // configure connections from "./config/myapp-services.init" file
    Application::initApplication(true, true, true, true, true, "./config/myapp-services.init", nullptr);
    
    // Application-specific code here

    Application::releaseApplication();
    return 0;
}
```

Using `Application::initApplication()` with the appropriate parameters the application starts the log manager and connects to the Log Collector, making it ready for sending message and receiving log filtering instructions from Lob Observer. Finally, `Application::releaseApplication()` cleans up resources.
