# AREG SDK Multicast Router

The **Multicast Router (mcrouter)** in AREG SDK enables efficient communication across Service Provider and Consumer nodes within a networked environment. 

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

The AREG Framework uses an Object Remote Procedure Call (ORPC) protocol to enable seamless data exchanges between software components. The **Multicast Message Router** (or **Router**), establishes a communication network between interconnected applications, efficiently routing messages to their intended targets. This guide covers configuration and setup instructions for the Multicast Router.

---

## Router Setup

The Router is implemented in the [mcrouter](./../../framework/mcrouter) module and compiled as a standalone executable, allowing it to run either as a console application or a service managed by Windows or Linux operating systems. It can be deployed on any networked machine running a **General Purpose Operating System (GPOS)**.

**To run as a service:**

- **On Windows:** 
  - Run the `mcrouter.service.install.bat` file, ensure the `mcrouter` path is correct, and restart the system.
  - Alternatively, run `mcrouter.exe --install` on Terminal or Powershell.
  - To uninstall, use `mcrouter.service.uninstall.bat` or call `mcrouter --uninstall`.

- **On Linux:**
  - Use `mcrouter.service` file with the correct path in `ExecStart`.
  - Run `sudo systemctl enable mcrouter.service` and `sudo systemctl start mcrouter.service` to start the service.
  - To stop and disable service, runt `sudo systemctl stop mcrouter.service` and `sudo systemctl disable mcrouter.service`

Since AREG SDK has fault-tolerant design, the `mcrouter` can be started at any time. But it is recommended to start the Multicast Router before other applications to allow for immediate connection establishment, enabling efficient **Inter-Process Communication (IPC)**.

> [!NOTE]
> The Multicast Router is necessary for applications requiring **Inter-Process Communication (IPC)** for *Public* services. For *Local* (**intra-process**) services, the Router is not required.

---

## Router Configuration

The **mcrouter** uses a configuration file, typically [areg.init](./../../framework/areg/resources/areg.init), to define key settings as key-value pairs. These pairs have a structure of `section::(module|*)::property[::position]`. Below is a sample configuration setup for the router in `areg.init`:

```text
router::*::service          = mcrouter      # Router process name
router::*::connect          = tcpip		    # Supported communication protocols
router::*::enable::tcpip    = true		    # Enable/disable communication protocol
router::*::address::tcpip   = 172.23.96.1   # Connection IP address (default: 127.0.0.1)
router::*::port::tcpip      = 8181		    # Connection port number (default: 8181)
```

Each setting has the following function:
- `router::*::service` specifies the router process name.
- `router::*::connect` lists supported protocols (TCP/IP in this example).
- `router::*::enable::tcpip` enables/disables the protocol.
- `router::*::address::tcpip` provides the router’s network-accessible IP.
- `router::*::port::tcpip` assigns the port number.

For details on persistence syntax, refer to [AREG SDK Persistence Syntax](./persistence-syntax.md).

---

## Command Line Options

The `mcrouter` can run either as a console application or an OS-managed service. It has various commands for starting, configuring, and controlling its operation. Some commands are entered in the `mcrouter` console while it is running, indicated with **Console** in the options list. All others are command line options to start the program.

The `mcrouter` can be managed via console or OS service commands:

| Command               | Platform  | Description                   | Usage Example                 |
|-----------------------|-----------|-------------------------------|-------------------------------|
| `-c, --console`       | All       | Run as console application    | `mcrouter --console`          |
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

By default, `mcrouter` loads its settings from `areg.init`. For custom configurations, specify a different configuration file:

```bash
mcrouter --load="./my/config/file.init"
```

> **Note:** Disable `mcrouter` logs by setting `log::mcrouter::scope::* = NOTSET` in `areg.init`.

### Application Setup

Applications configure their mcrouter connection by specifying the path to the configuration file, as shown below:

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

By calling `Application::initApplication()` with the appropriate parameters, the application connects to the Multicast Router and is ready for service-based communication. Finally, `Application::releaseApplication()` cleans up resources.