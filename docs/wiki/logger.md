# Log Collector Service



 Configure [_areg.init_](https://github.com/aregtech/areg-sdk/blob/master/framework/areg/resources/areg.init) file copied into the binary directory after the build to start receiving log messages from applications. Following properties are relevant to `logger` and connected applications:




 (To configure logging for applications based on the AREG framework, you can compile them with or without logging. If you choose to compile with logging, you'll need to configure the [_areg.init_](https://github.com/aregtech/areg-sdk/blob/master/framework/areg/resources/areg.init) file to set logging scopes, priorities, and file names.

Here's an example configuration for the log file:
```
log::*::target              = file                          # Log targets: remote, file, debug output, database
log::*::enable::file        = true                          # File logging enable / disable flag
log::*::file::location      = ./logs/%appname%_%time%.log   # Log file location and masks

log::mcrouter::scope::*                 = NOTSET ;                         # disables logs for mcrouter.
log::my_app::scope::*                   = DEBUG | SCOPE ; # enables all logs of my_app
log::my_app::scope::ignore_this_scope   = NOTSET ;        # disables logs for a certain scope in my_app
log::my_app::scope::ignore_this_group_* = NOTSET ;        # disables logs for a certain scope group in my_app
```

Please refer to the [AREG Logging System](https://github.com/aregtech/areg-sdk/wiki/05.-AREG-Logging-System) Wiki page for detailed instructions on compiling applications with enabled or disabled logging, starting and stopping logs during runtime, creating and using logging scopes, and configuring the `areg.init` file.

> 💡 By default, the `areg.init` files are located in the `config` subfolder of the binaries.<br/>
> 💡 To enable logging for all applications, use `log::*::scope::*  = DEBUG | SCOPE ;`.<br/>
> 💡 Currently, logging is only possible in the file.



Here is the revised document with improved clarity and grammar:

---

# AREG SDK Multicast Router

The **Multicast Router (mcrouter)** in AREG SDK enables efficient communication between Service Provider and Consumer nodes within a networked environment.

## Table of Contents
1. [General Information](#general-information)
2. [Router Setup](#router-setup)
3. [Router Configuration](#router-configuration)
4. [Command Line Options](#command-line-options)
5. [Connection Initialization](#connection-initialization)
   - [mcrouter Initialization](#mcrouter-initialization)
   - [Application Setup](#application-setup)

---

## General Information

The AREG Framework utilizes an **Object Remote Procedure Call (Object RPC)** protocol to facilitate seamless data exchange between software components. The **Multicast Message Router** (or **Router**) establishes a communication network, efficiently routing messages to the correct destinations. This guide outlines configuration and setup instructions for the Multicast Router.

---

## Router Setup

The Router is implemented in the [mcrouter](./../../framework/mcrouter) module and compiled as a standalone executable. It can run either as a console application or as a service managed by Windows or Linux. The router can be deployed on any networked machine with a **General Purpose Operating System (GPOS)**.

**To run as a service:**

- **On Windows:** 
  - Run `mcrouter.service.install.bat`, ensure the correct `mcrouter` path, and restart the system.
  - Alternatively, run `mcrouter.exe --install` in the Terminal or PowerShell.

- **On Linux:**
  - Use `mcrouter.service` file, ensuring the correct path in `ExecStart`.
  - Run `sudo systemctl enable mcrouter.service` and `sudo systemctl start mcrouter.service`.

  To stop running the service:
- **On Windows**: run `mcrouter.service.uninstall.bat` or `mcrouter --uninstall`, and restart system or stop service in the Service List.
- **On Linux**: run `sudo systemctl stop mcrouter.service` and `sudo systemctl disable mcrouter.service`.
 
Regardless AREG SDK is fault-tolerant and order of starting process plays no role, starting the Multicast Router before other applications is recommended for immediate connection establishment and efficient **Inter-Process Communication (IPC)**.

> [!NOTE]
> The Multicast Router is only needed for applications requiring **IPC** for *Public* services. It is not necessary for *Local* (intra-process) services.

---

## Router Configuration

The **mcrouter** uses a configuration file, typically [areg.init](./../../framework/areg/resources/areg.init), with settings defined as key-value pairs. Each setting uses the format `section::(module|*)::property[::position]`. Below is a sample configuration for `mcrouter` in `areg.init`:

```text
router::*::service          = mcrouter      # Router process name
router::*::connect          = tcpip		    # Supported communication protocols
router::*::enable::tcpip    = true		    # Enable/disable protocol
router::*::address::tcpip   = 172.23.96.1   # Connection IP address (default: 127.0.0.1)
router::*::port::tcpip      = 8181		    # Connection port number (default: 8181)
```

Explanation of each setting:
- `router::*::service`: Specifies the router process name.
- `router::*::connect`: Lists supported protocols (TCP/IP in this example).
- `router::*::enable::tcpip`: Enables/disables the protocol.
- `router::*::address::tcpip`: Provides the router’s network-accessible IP.
- `router::*::port::tcpip`: Assigns the port number.

For further details, refer to the [AREG SDK Persistence Syntax](./persistence-syntax.md).

---

## Command Line Options

The `mcrouter` can be run as either a console application or an OS-managed service. It has various commands for starting, configuring, and controlling its operation. Some commands are entered while `mcrouter` is running in the console, marked as **Console** in the options list. All others are command-line options to start the program.

| Command               | Platform  | Description                   | Usage Example                 |
|-----------------------|-----------|-------------------------------|-------------------------------|
| `-c, --console`       | All       | Run as a console application  | `mcrouter --console`          |
| `-h, --help`          | All       | Display help message          | `mcrouter --help`             |
| `-i, --install`       | Windows   | Install as a service          | `mcrouter --install`          |
| `-l, --load`          | All       | Load configuration            | `mcrouter -l="config.init"`   |
| `-n, --instances`     | Console   | Display connected instances   | `-n`                          |
| `-p, --pause`         | Console   | Pause message routing         | `-p`                          |
| `-q, --quit`          | Console   | Quit console application      | `-q`                          |
| `-r, --restart`       | Console   | Restart connections           | `-r`                          |
| `-s, --service`       | Linux     | Start as a service            | `mcrouter --service`          |
| `-t, --silent`        | Console   | Run silently                  | `-t`                          |
| `-u, --uninstall`     | Windows   | Uninstall service             | `mcrouter --uninstall`        |
| `-v, --verbose`       | Console   | Display data rate             | `-v`                          |

Commands may be updated over time.

---

## Connection Initialization

### mcrouter Initialization

By default, `mcrouter` loads settings from `areg.init`. For a custom configuration file, specify it like this:

```bash
mcrouter --load="./my/config/file.init"
```

> [!NOTE]
> To disable `mcrouter` logs, set `log::mcrouter::scope::* = NOTSET` in `areg.init`.

### Application Setup

Applications establish a `mcrouter` connection by specifying the path to their configuration file:

```cpp
#include "areg/appbase/Application.hpp"

int main()
{
    Application::initApplication(true, true, true, true, true, nullptr, "./config/myapp-router.init");
    
    // Application-specific code here

    Application::releaseApplication();
    return 0;
}
```

Using `Application::initApplication()` with the appropriate parameters connects the application to the Multicast Router, making it ready for service-based **Inter-Process Communication (IPC)**. Finally, `Application::releaseApplication()` cleans up resources.
