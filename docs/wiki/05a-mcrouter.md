# AREG SDK Multicast Router

The **Multicast Router (mcrouter)** in AREG SDK enables efficient communication between Service Provider and Consumer nodes within a networked environment.

## Table of Contents
- [AREG SDK Multicast Router](#areg-sdk-multicast-router)
  - [Table of Contents](#table-of-contents)
  - [General Information](#general-information)
  - [Router Setup](#router-setup)
  - [Router Configuration](#router-configuration)
  - [Command Line Options](#command-line-options)
  - [Connection Initialization](#connection-initialization)
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
  - Run `mcrouter.service.install.bat`, ensure the correct `mcrouter` path, and restart the system if needed.
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

The **mcrouter** uses a configuration file, typically [areg.init](./../../framework/areg/resources/areg.init), with settings defined as key-value pairs. Each setting uses the format `section::(module|*)::property[::(position|*)] = value`. Below is a sample configuration for `mcrouter` in `areg.init`:

```plaintext
router::*::service          = mcrouter      # Router process name
router::*::connect          = tcpip		    # Supported communication protocols
router::*::enable::tcpip    = true		    # Enable/disable protocol
router::*::address::tcpip   = 172.23.96.1   # Connection IP address (default: 127.0.0.1)
router::*::port::tcpip      = 8181		    # Connection port number (default: 8181)
```

Applications use router settings in their configuration files to establish a network connection and initiate Inter-Process Communication (IPC). 

> [!TIP]
> For custom configurations, replace the wildcard `*` in properties with the specific application name (e.g., `router::someapp::address::tcpip = localhost`).

Bellow is the explanation of each setting:
|  Property Key Setting:        |   Description:                                            |
|-------------------------------|-----------------------------------------------------------|
| `router::*::service`          | Specifies the message router process name.                |
| `router::*::connect`          | Lists supported protocols (**TCP/IP** in this example).   |
| `router::*::enable::tcpip`    | Enables or disables the protocol.                         |
| `router::*::address::tcpip`   | Provides the router's network-accessible IP.              |
| `router::*::port::tcpip`      | Assigns the port number.                                  |

For further details, refer to the [AREG SDK Persistence Syntax](./06a-persistence-syntax.md).

---

## Command Line Options

The `mcrouter` can be run as either a console application or an OS-managed service. It has various commands for starting, configuring, and controlling its operation. Some commands are entered while `mcrouter` is running in the console, marked as **Console** in the options list. All others are command-line options to start the program.

| Command:              | Platform: | Description:                  | Usage Example:                |
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

Commands may vary with version updates.

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

Applications establish a `mcrouter` connection by starting message router client and specifying the path to their configuration file:

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

Using `Application::initApplication()` with the appropriate parameters connects the application to the Multicast Router, making it ready for service-based **Inter-Process Communication (IPC)**. Finally, `Application::releaseApplication()` cleans up resources.
