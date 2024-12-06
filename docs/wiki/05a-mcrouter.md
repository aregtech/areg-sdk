# AREG SDK Multicast Router

The **Multicast Router (mcrouter)** in AREG SDK enables efficient communication between Service Provider and Consumer nodes within a networked environment.

## Table of Contents
1. [General Information](#1-general-information)
2. [Running Router as a Service](#2-running-router-as-a-service)
   - [Linux Service](#linux-service)
   - [Windows Service](#windows-service)
3. [Router Configuration](#3-router-configuration)
4. [Command Line Options](#4-command-line-options)
5. [Connection Initialization](#5-connection-initialization)
   - [mcrouter Initialization](#mcrouter-initialization)
   - [Application Setup](#application-setup)

---

## 1. General Information

The AREG Framework utilizes an **Object Remote Procedure Call (Object RPC)** protocol to facilitate seamless data exchange between software components. The **Multicast Message Router** (or **Router**) establishes a communication network, efficiently routing messages to the correct destinations. This guide outlines configuration and setup instructions for the Multicast Router.

---

## 2. Running Router as a Service

The Multicast Router (`mcrouter`) is implemented within the AREG SDK [Multicast Router module](./../../framework/mcrouter/) and compiled as a standalone executable that can operate as either a console application or an OS-managed service on Windows and Linux platforms. Deployment can occur on any networked machine with a **General Purpose Operating System (GPOS)**.

### Linux Service

To configure and run the `mcrouter` application as a Linux-managed service, follow these steps:

1. **Copy Service Configuration File**:
   - Copy the `mcrouter.service` file to the `/etc/systemd/system/` directory.

2. **Copy the Executable**:
   - Copy the built `mcrouter.elf` executable to the desired location, such as `/usr/local/bin`.

3. **Ensure Library Access (if applicable)**:
   - If the AREG Framework was built as a shared library, ensure that `mcrouter` has access to the `libareg.so` library (e.g., located in `/usr/lib`).

4. **Edit the Service Configuration**:
   - Open the `mcrouter.service` file and verify that the `ExecStart` path points to the correct location of the `mcrouter.elf` executable. For example:
     ```plaintext
     ExecStart=/usr/local/bin/mcrouter.elf --service
     ```
   - Ensure the `ExecStart` line includes the `--service` (or `-s`) as a command line option.

5. **Enable and Start the Service**:
   - Enable the service to start automatically at boot:
     ```bash
     sudo systemctl enable mcrouter.service
     ```
   - Start the service:
     ```bash
     sudo systemctl start mcrouter.service
     ```

6. **Stop or Disable the Service**:
   - To stop the service:
     ```bash
     sudo systemctl stop mcrouter.service
     ```
   - To disable the service from starting at boot:
     ```bash
     sudo systemctl disable mcrouter.service
     ```

### Windows Service

To configure and run the `mcrouter` application as a Windows-managed service, follow these steps:

1. **Copy the Binaries**:
   - Copy the `mcrouter.exe` and `areg.dll` binaries to the desired location.

2. **Install the Service**:
   - Open **PowerShell** as the Administrator.
   - Register the executable as a service by running:
     ```powershell
     .\mcrouter.exe --install
     ```
   - Alternatively, you can execute `mcrouter.service.install.bat` as **Administrator**, ensuring the correct path to `mcrouter.exe`.

3. **Start the Service**:
   - Open the **Services** application (or run `services.msc` in the Command Prompt).
   - Locate the service named **AREG Log Collector Service**.
   - Start the service by right-clicking it and selecting **Start**.

4. **Stop and Uninstall the Service**:
   - Stop the service using the **Services** application or the `services.msc` command.
   - Uninstall the service by running the following command in PowerShell:
     ```powershell
     .\mcrouter.exe --uninstall
     ```
   - Alternatively, you can execute `mcrouter.service.uninstall.bat` as **Administrator**, ensuring the correct path to `mcrouter.exe`.

---

## 3. Router Configuration

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

## 4. Command Line Options

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

## 5. Connection Initialization

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
