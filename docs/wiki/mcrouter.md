# Multicast Router

Multicast Router (`mcrouter`) is part of AREG SDK project. It is a tool, which creates a network and provides communication between Service Provier and Service Consumer nodes. 

## Table of contents

1. [General Information](#general-information)
2. [Router Setup](#router-setup)
3. [Router Configuration](#router-configuration)
4. [Connection Initialization](#connection-initialization)

---

## General Information

The AREG Framework leverages the robust Object Remote Procedure Call (ORPC) protocol for seamless communication between its software components. Facilitating this communication is the **Multicast Message Router**, also known as the **Message Router**. This essential component forms a network of interconnected applications and efficiently routes messages to the targets. In this chapter, we will provide an overview of the *Multicast Message Router* and provide guidance on its configuration.

---

## Router Setup

The Multicast Router, an integral part of the AREG Framework, is implemented in the [mcrouter](./../../framework/mcrouter) module. It is compiled as a standalone executable, offering the flexibility to run either as a console application or as a service managed by Windows or Linux operating systems. The Multicast Router can be deployed on any machine operating a General Purpose Operating System (GPOS) within the network.

Multicast Router can run as a console or OS-managed service. To run Multicast Router as a service:

- **On Windows:** 
  - use copied [mcrouter.service.install.bat](./../../framework/mcrouter/resources/mcrouter.service.install.bat) file; 
  - make sure that the path to the `mcrouter` is properly set, and run the script;
  - alternatively, call `mcrouter.exe --install` on command line;
  - restart the system, make sure that Multicast Router is listed in the service list and runs.
  - to uninstall the router use copied [mcrouter.service.uninstall.bat](./../../framework/mcrouter/resources/mcrouter.service.uninstall.bat), which unregisters `mcrouter` as a service.
  
- **On Linux:**
  - use copied `mcrouter.service` file;
  - make sure that the path to `mcrouter` is properly set (see `ExecStart=/usr/local/bin/mcrouter.out --service`);
  - run 'sudo systemctl enable mcrouter.service' in command line to enable Multicast Router;
  - run `sudo systemctl start mcrouter.service` in command line to start Multicast Router.
  - To scopt the `mcrouter` service, run `sudo systemctl stop mcrouter.service` in command line.
  - 

Once the Multicast Router is launched, all other applications should be configured to establish a connection with it. Although the communication engine of AREG-enabled applications will automatically retry establishing a connection, it is advisable to start the Multicast Router beforehand. As soon as the socket connection is successfully established, applications can proceed to register or wait for service availability before initiating communication.

> [!NOTE]
> *Multicast Router* is necessary only for applications that provide or consume _Public_ services (require **Inter-Process Communication (IPC)**). In case of _Local_ services (multithreading communication) the Router can be ignored. 


## Router Configuration

By default, the configuration of the Multicast Router, relies on the [areg.init](./../../framework/mcrouter/resources/router.init) file. This file has to a straightforward syntax, consisting of *key* and *value* pairs. The key has a structure `section::(module|*)::property[::position]`. All keys in the section `router` are relevant to the Multicast Router service, so that all application that use the Router should use this section to establish proper connection. All modules that refer to `mcrouter` are relevant to Multicastin Router process and other applications can ignore.

In `areg.init` file the router is configured in following way:
```text
# ---------------------------------------------------------------------------
# Message router settings
# ---------------------------------------------------------------------------
router::*::service          = mcrouter                      # The name of the router service (process name)
router::*::connect          = tcpip			                # The list of supported communication protocols
router::*::enable::tcpip    = true			                # Communication protocol enable / disable flag
router::*::address::tcpip   = 172.23.96.1                   # Protocol specific connection IP-address, default IP is 127.0.0.1
router::*::port::tcpip      = 8181			                # Protocol specific connection port number, default port is 8181
```

Where the module `*` indicates that all applications should use these data to configure and the meaning of data is following
- `router::*::service` indicates the name of the router process.
- `router::*::connect` indicates the connection type. It can be a list of connections. Since here it is TCP/IP only one entry, currently IPC is done only via TCP/IP connection for all applications.
- `router::*::enable::tcpip` this key should be listed for each connection type. Since there is only one connection type TCP/IP, here it indicates that the connection is enabled for all applications.
- `router::*::address::tcpip` indicates the IP-address for TCP/IP connection, it should be network accessible IP-address for all processes.
- `router::*::port::tcpip` indicates the port number for TCP/IP connection.

To gain a comprehensive understanding of the configuration syntax, please consult the [Persistence Syntax](./persistence-syntax.md) document of AREG SDK Wiki.

---

## Command line options

The Router `mcrouter` runs as a console application and service managed by Operating System. It has a few commands when starting or input commands when runs as a console application. Bellow are all command options to start the router or input on console, commands with remark **Console** are input command on console of `mcrouter`, the remarks with **-** mean the command can be used as a command line option and it is valid for Windows and Linux platforms.

|  Command          | Remark    | Meaning                                   | Usage example               |
|-------------------|-----------|-------------------------------------------|-----------------------------|
| `-c, --console`   | -         | Start `mcrouter` as console application   | `mcrouter --console`        |
| `-h, --help`      | -         | Display help message on console           | `mcrouter --help`           |
| `-i, --install`   | Windows   | Install `mcrouter` as OS managed service  | `mcrouter --install`        |
| `-l, --load`      | -         | Start `mcrouter` and load configuration   | `mcrouter -l="config.init"` |
| `-n, --instances` | Console   | Option to display connected instances     | `-n`                        |
| `-p, --pause`     | Console   | Option to pause routing, stop messaging   | `-p`                        |
| `-q, --quit`      | Console   | Option to quit the console application    | `-q`                        |
| `-r, --restart`   | Console   | Option to restart connection              | `-r`                        |
| `-s, --service`   | Linux     | Start `mcrouter` as a OS manged service   | `mcrouter --service`        |
| `-t, --silent`    | Console   | Option to runs silent                     | `-t`                        |
| `-u, --uninstall` | Windows   | Uninstall `mcrouter` as OS managed service| `mcrouter --uninstall`      |
| `-v, --verbose`   | Console   | Option to display data rate               | `-v`                        |

By time the list can be updated. 

---

## Connection Initialization

### mcrouter initialization

`areg.init` configuration is the default for `mcrouter`. If you need to have another configuration, start mcrouter with the option `--load="/path/my/config/file.init"`.
Like this:
```bash
mcrouter --load="./my/congig/file.init"
```

> [!NOTE]
> To disable the logs of `mcrouter` change in `areg.init` file `log::mcrouter::scope::* = NOTSET`.

### Other applications

Typically, applications make use of the configuration file during setup by passing the file path when initializing the services. In the following code snippet, the application initializes and establishes a connection with the *Multicast Router* by specifying `./config/myapp-router.init` as the path to the application-specific configuration file:

```cpp
#include "areg/appbase/Application.hpp"

int main()
{
    // Initialize services, providing the path to establish a connection with mcrouter
    Application::initApplication(true, true, true, true, true, nullptr, "./config/myapp-router.init");
    
    // ... your code goes here

    // Release resources
    Application::releaseApplication();
    return 0;
}
```

By calling `Application::initApplication()` and providing the appropriate parameters, the application initializes and configures the necessary services. In this case, the last parameter, `./config/myapp-router.init`, specifies the path to the configuration file, enabling the establishment of a connection with the *Multicast Router*. Once the initialization is complete, you can proceed with your application-specific code. Finally, releasing the application resources is done by calling `Application::releaseApplication()`.

---
