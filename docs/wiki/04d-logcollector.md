# Areg SDK Log Collector Service

The Areg SDK Log Collector is a centralized service that gathers and routes log data from distributed applications to log observers for real-time monitoring and analysis.

> [!TIP]
> Log Collector acts as the central hub: applications send logs to it, and log observers receive logs from it.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Overview](#overview)
3. [Installation](#installation)
4. [Running as System Service](#running-as-system-service)
5. [Configuration](#configuration)
6. [Saving Collected Logs in a Database](#saving-collected-logs-in-a-database)
7. [Command Line and Console Commands](#command-line-and-console-commands)
8. [Application Integration](#application-integration)
9. [Troubleshooting](#troubleshooting)

---

## Quick Start

### Deploy Log Collector in 2 Minutes

Log collector is available when building Areg SDK sources. The `logcollector` binary is available:

**Linux:** built with GNU g++ compiler
```bash
./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/logcollector.elf
```

**Windows:**
```powershell
.\product\build\msvc-v143\windows_nt-x64-release\bin\logcollector.exe
```


**Step 1: Install Areg SDK (if not already installed)**

```bash
# Linux
sudo cmake --install ./build

# Log Collector location
/usr/local/tools/areg/logcollector
```

**Step 2: Install as system service**

**Linux:**
```bash
# Copy service file
sudo cp /usr/local/share/areg/service/logcollector.service /etc/systemd/system/

# Enable and start
sudo systemctl daemon-reload
sudo systemctl enable logcollector.service
sudo systemctl start logcollector.service
```

**Windows:**
```powershell
# Run as Administrator
cd "C:\Program Files\areg\tools\areg"
.\logcollector.exe --install
Start-Service "logger.service"
```

**Step 3: Verify service is running**

**Linux:**
```bash
sudo systemctl status logcollector.service
```

**Expected output:**
```
● logcollector.service - Areg Log Collector Service
     Loaded: loaded (/etc/systemd/system/logcollector.service; enabled)
     Active: active (running) since Mon 2026-01-27 10:30:00 UTC; 5s ago
```

**Windows:**
```powershell
Get-Service "logger.service"
```

**Expected output:**
```
Status   Name             DisplayName
------   ----             -----------
Running  logger.service   Areg Log Collector Service
```

**Setup time:** ~2 minutes

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Overview

### What is Log Collector?

The Log Collector is a centralized network service that:

**Data Collection:**
- ✅ Receives logs from multiple distributed applications
- ✅ Aggregates logs from all sources
- ✅ Routes logs to connected log observers

**Persistence (optional):**
- ✅ Saves the collected logs in an SQLite database file (`.sqlog`)
- ✅ Records the connect and disconnect time of every log source
- ✅ Records the log scopes and priorities of every log source

**Dynamic Control:**
- ✅ Forwards scope control commands from observers to applications
- ✅ Enables runtime log filtering
- ✅ Optimizes network traffic

---

### Architecture

```
┌─────────────┐         ┌──────────────┐         ┌──────────────┐
│ Application │────────►│     Log      │────────►│     Log      │
│     A       │  Logs   │  Collector   │  Logs   │   Observer   │
│             │         │   (Server)   │         │   (Client)   │
└─────────────┘         │              │         └──────────────┘
                        │   Port 8282  │
┌─────────────┐         │              │         ┌──────────────┐
│ Application │────────►│              │────────►│    Lusan     │
│     B       │  Logs   │              │  Logs   │  Application │
└─────────────┘         └──────────────┘         └──────────────┘
                         ▲          │
                 Control │          │ optional
                commands │          ▼
            (scope and   │  ┌──────────────────┐
             priority    │  │  ./logs/*.sqlog  │
             updates)    │  │ SQLite database  │
                         │  └──────────────────┘
```

**Components:**
1. **Applications** - Send logs to Log Collector
2. **Log Collector** - Central aggregation and routing service (this guide)
3. **Log Observers** - Receive logs, store data, control scopes
4. **Database file** - Optional local copy of everything the collector received

> [!NOTE]
> Log Collector supports multiple applications sending logs and multiple log observers receiving logs simultaneously. Each observer receives all log messages, enabling independent storage and analysis.

---

### Communication Flow

**Log Flow:**
```
Application → Log Collector → Log Observer(s) → Storage (File/Database)
                     │
                     └─────────► Database file (.sqlog), if enabled
```

**Control Flow:**
```
Log Observer → Log Collector → Application (scope/priority updates)
```

> [!NOTE]
> Forwarding to the observers always happens first. The database write is queued and performed by a separate thread, so it never delays the delivery of the logs to the observers.

---

### When to Use Log Collector

**Use Log Collector when:**
- ✅ Applications are distributed across network
- ✅ Centralized log collection needed
- ✅ Multiple applications logging simultaneously
- ✅ Runtime scope control required
- ✅ Using Log Observer or Lusan for log analysis
- ✅ Logs must be kept on disk even when no observer is connected

**Not needed when:**
- ❌ Single application logging to file
- ❌ No network distribution
- ❌ Simple file-based logging sufficient

**See:** [Logging Configuration Guide](./04a-logging-config.md) for direct file logging

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Installation

### From Build Directory

After building Areg SDK, Log Collector is available in build output.

**Default build paths:**

**Linux:**
```bash
./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/logcollector
```

**Windows:**
```powershell
.\product\build\msvc-v143\windows_nt-x64-release\bin\logcollector.exe
```

---

### System-Wide Installation

**Install via CMake:**

```bash
# Configure and build
cmake -B ./build
cmake --build ./build

# Install (requires sudo on Linux)
sudo cmake --install ./build
```

**Installation locations:**

**Linux:**
- Executable: `/usr/local/tools/areg/logcollector`
- Library: `/usr/local/lib/libareg.so`
- Config: `/usr/local/share/areg/areg.init`
- Service file: `/usr/local/share/areg/service/logcollector.service`

**Windows:**
- Executable: `C:\Program Files\areg\tools\areg\logcollector.exe`
- Library: `C:\Program Files\areg\bin\areg.dll`
- Config: `C:\Program Files\areg\share\areg\areg.init`
- Service scripts: `C:\Program Files\areg\share\areg\service\logcollector.service.*.bat`

---

### Prerequisites

**Required:**
- Areg SDK libraries (`libareg.so` or `areg.dll`)
- Network port 8282 available (default)
- Write permissions for log directory (if logging enabled)

**Optional:**
- Configuration file (`areg.init`)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Running as System Service

### Linux Service (systemd)

#### Install Service

**Step 1: Prepare files**

```bash
# If installed via CMake, files are already in place
# Service file at: /usr/local/share/areg/service/logcollector.service
# Executable at: /usr/local/tools/areg/logcollector
```

**Or manually copy:**

```bash
# Copy executable
sudo cp ./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/logcollector /usr/local/bin/

# Copy library (if shared build)
sudo cp ./product/build/gnu-g++/linux-64-x86_64-release-shared/lib/libareg.so /usr/lib/
sudo ldconfig

# Copy configuration
sudo mkdir -p /usr/local/share/areg
sudo cp ./framework/areg/resources/areg.init /usr/local/share/areg/
```

**Step 2: Install service file**

```bash
# Copy service file
sudo cp /usr/local/share/areg/service/logcollector.service /etc/systemd/system/

# Or copy from framework
sudo cp ./framework/logcollector/resources/logcollector.service /etc/systemd/system/
```

**Verify service file:**

```bash
cat /etc/systemd/system/logcollector.service
```

**Expected content:**
```ini
[Unit]
Description=Areg Log Collector Service
After=network.target

[Service]
Type=simple
ExecStart=/usr/local/tools/areg/logcollector --service
Restart=on-failure
RestartSec=5s

[Install]
WantedBy=multi-user.target
```

> [!IMPORTANT]
> The `--service` flag is required for service mode.

**Step 3: Enable and start**

```bash
# Reload systemd
sudo systemctl daemon-reload

# Enable (start at boot)
sudo systemctl enable logcollector.service

# Start service
sudo systemctl start logcollector.service
```

**Step 4: Verify status**

```bash
sudo systemctl status logcollector.service
```

**Expected output:**
```
● logcollector.service - Areg Log Collector Service
     Loaded: loaded (/etc/systemd/system/logcollector.service; enabled)
     Active: active (running) since Mon 2026-01-27 10:30:00 UTC; 10s ago
   Main PID: 12345 (logcollector)
      Tasks: 3
     Memory: 12.5M
     CGroup: /system.slice/logcollector.service
             └─12345 /usr/local/tools/areg/logcollector --service
```

---

#### Manage Service

**Stop service:**
```bash
sudo systemctl stop logcollector.service
```

**Restart service:**
```bash
sudo systemctl restart logcollector.service
```

**Disable (don't start at boot):**
```bash
sudo systemctl disable logcollector.service
```

**View logs:**
```bash
sudo journalctl -u logcollector.service -f
```

<div align="right"><kbd><a href="#running-as-system-service">↑ Back to service setup ↑</a></kbd></div>

---

### Windows Service

#### Install Service

**Step 1: Prepare binaries**  
**If installed via CMake:** files are at: `C:\Program Files\areg\tools\areg\`  
**Or manually copy:**
```powershell
# Create directory
New-Item -Path "C:\Program Files\areg\tools\areg" -ItemType Directory -Force

# Copy binaries
Copy-Item .\product\build\msvc-v143\windows_nt-x64-release\bin\logcollector.exe "C:\Program Files\areg\tools\areg\"
Copy-Item .\product\build\msvc-v143\windows_nt-x64-release\bin\areg.dll "C:\Program Files\areg\tools\areg\"
```

**Step 2: Install service**  
**Open PowerShell as Administrator**

**Option A: Using installation script**
```powershell
cd "C:\Program Files\areg\share\areg\service"
.\logcollector.service.install.bat
```

**Option B: Using executable**
```powershell
cd "C:\Program Files\areg\tools\areg"
.\logcollector.exe --install
```

**Expected output:**
```
Service 'Areg Log Collector Service' installed successfully
```

**Step 3: Start service**

**Using Services GUI:**
1. Press `Win + R`, type `services.msc`, press Enter
2. Find **Areg Log Collector Service**
3. Right-click → **Start**

**Using PowerShell:**
```powershell
Start-Service "logger.service"
```

**Or using net command:**
```powershell
net start logger.service
```

**Step 4: Verify status**

```powershell
Get-Service "logger.service"
```

**Expected output:**
```
Status   Name             DisplayName
------   ----             -----------
Running  logger.service   Areg Log Collector Service
```

---

#### Manage Service

**Stop service:**

```powershell
Stop-Service "logger.service"
# or
net stop logger.service
```

**Restart service:**

```powershell
Restart-Service "logger.service"
```

**Set automatic startup:**

```powershell
Set-Service "logger.service" -StartupType Automatic
```

**Uninstall service:**

```powershell
# Stop first
Stop-Service "logger.service"

# Uninstall
cd "C:\Program Files\areg\tools\areg"
.\logcollector.exe --uninstall
```

**View service logs:**

Open Event Viewer: `Win + R` → `eventvwr.msc`  
Navigate: **Windows Logs → Application**  
Filter for **Source: logcollector**

<div align="right"><kbd><a href="#running-as-system-service">↑ Back to service setup ↑</a></kbd></div>

---

### Console Mode (Development/Testing)

For development or testing, run Log Collector in console mode:

```bash
./logcollector --console
```

**Use cases:**
- Development and debugging
- Testing configuration changes
- Temporary log collection

> [!NOTE]
> For production deployment, use system service mode. Console mode is intended for development and testing only.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Configuration

### Configuration File

**Default:** `areg.init` in Areg SDK resources directory

**Installed location:**
- Linux: `/usr/local/share/areg/areg.init`
- Windows: `C:\Program Files\areg\share\areg\areg.init`

**Custom location:**

```bash
logcollector --load="./path/to/config.init"
```

---

### Network Configuration

**Configure Log Collector network settings:**

```ini
# Log Collector service configuration
logger::*::service          = logcollector  # Service name
logger::*::connect          = tcpip         # Protocol
logger::*::enable::tcpip    = true          # Enable TCP/IP
logger::*::address::tcpip   = 127.0.0.1     # Listen address
logger::*::port::tcpip      = 8282          # Listen port
```

**Configuration breakdown:**

| Property                    | Default        | Description                          |
| --------------------------- | -------------- | ------------------------------------ |
| `logger::*::service`        | `logcollector` | Log Collector process name           |
| `logger::*::connect`        | `tcpip`        | Communication protocol (TCP/IP only) |
| `logger::*::enable::tcpip`  | `true`         | Enable/disable TCP/IP                |
| `logger::*::address::tcpip` | `127.0.0.1`    | IP address to bind                   |
| `logger::*::port::tcpip`    | `8282`         | Port number                          |

---

### Common Configurations

1. Local machine (default):*
    ```ini
    logger::*::address::tcpip   = 127.0.0.1
    logger::*::port::tcpip      = 8282
    ```
    **Use case:** All applications and observers on same machine

2. *Network access (all interfaces):*
    ```ini
    logger::*::address::tcpip   = 0.0.0.0
    logger::*::port::tcpip      = 8282
    ```
    **Use case:** Accept connections from any network interface
3. *Specific network interface:*
    ```ini
    logger::*::address::tcpip   = 192.168.1.100
    logger::*::port::tcpip      = 8282
    ```
    **Use case:** Bind to specific network interface
4. *Custom port:**
    ```ini
    logger::*::address::tcpip   = 127.0.0.1
    logger::*::port::tcpip      = 9090
    ```
    **Use case:** Avoid port conflicts

---

### Disable Log Collector's Own Logging

**Prevent Log Collector from logging about itself:**

```ini
# Disable self-logging
log::logcollector::scope::* = NOTSET
log::logobserver::scope::*  = NOTSET
```

> [!TIP]
> This is the default configuration to reduce overhead.

---

### Application-Specific Configuration

**Different settings per application:**

```ini
# Default for all applications
logger::*::address::tcpip   = 127.0.0.1
logger::*::port::tcpip      = 8282

# Disable logging for specific app
logger::someapp::enable::tcpip = false
```

**See:** [Areg Configuration Reference](./05b-areg-configuration-file.md) for the complete property list

**See also:** [Saving Collected Logs in a Database](#saving-collected-logs-in-a-database) for the `log::logcollector::db::*` properties

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Saving Collected Logs in a Database

Forwarding logs to observers is a live stream. If no observer is connected, the logs are gone. The Log Collector can additionally write every collected log into a local **SQLite database file** with the extension `.sqlog`.

The file has the same structure as the database written by the Log Observer, so the same tools read both files, including [Lusan](./06f-lusan-live-logging.md) and the plain `sqlite3` command line tool.

> [!TIP]
> The database is **off by default**. Nothing changes for an existing deployment until you switch it on.

---

### What Is Stored

| Table | Content |
| --- | --- |
| `version` | One row that describes the file: name, schema version, creator, and the time the file was created and closed. |
| `instances` | One row per log source: cookie ID, process name, location, bitness, connect time, disconnect time. |
| `scopes` | The log scopes of every log source: scope ID, scope name, priority, owner cookie, and the time the scope became active or inactive. |
| `logs` | Every collected log message: cookie ID, scope ID, priority, message text, module and thread name and ID, the time the message was created by the source, and the time the collector received it. |

Every log keeps the identity of the **log source that produced it**, not the identity of the collector. The module name, thread name, and cookie stored in the file are the ones of the originating application.

---

### Three Ways to Switch the Database On

| Way | Scope of the decision | Use case |
| --- | --- | --- |
| `log::logcollector::enable::db = true` in `areg.init` | Every start of the collector | Permanent setup, service deployment |
| `logcollector --log=db [<path>]` | This process only | Capture one session, no file editing |
| Console command `--save [<path>]` | Immediately, on a running collector | Start recording while a problem happens |

**The command line overrides the configuration file.** The order of precedence is:

```
--log=db  or  --log=nodb        ← wins, if present
        ↓
log::logcollector::enable::db   ← used when no --log option is given
```

Use `--log=nodb` to make sure that no database is created, even when the configuration file enables it.

---

### Configuration Properties

The properties live in [`areg.init`](../../framework/areg/resources/areg.init):

```ini
# ---------------------------------------------------------------------------
# Log Collector Database Settings
# ---------------------------------------------------------------------------
log::logcollector::enable::db       = false                         # save the collected logs in the database
log::logcollector::db::engine       = sqlite3                       # database engine: SQLite 3
log::logcollector::db::name         = logcollector_%time%.sqlog     # database file name with timestamp mask
log::logcollector::db::location     = ./logs                        # database file directory
```

| Property | Default in the shipped file | Value if the key is missing | Description |
| --- | --- | --- | --- |
| `log::logcollector::enable::db` | `false` | `false` | Switches the database on when the collector starts. |
| `log::logcollector::db::engine` | `sqlite3` | `sqlite3` | The only supported engine. An empty value is treated as `sqlite3`, any other value switches the database off. |
| `log::logcollector::db::name` | `logcollector_%time%.sqlog` | `logcollector_%time%.sqlog` | File name. Masks are allowed. |
| `log::logcollector::db::location` | `./logs` | `./logs` | Directory of the file. Created if it does not exist. |

**File name masks**

| Mask | Expands to |
| --- | --- |
| `%time%` | The current timestamp, in the form `yyyy_mm_dd_hh_mm_ss_ms` |
| `%appname%` | The name of the process, that is `logcollector` |
| `%user%` | The home directory of the account that runs the collector |

Because the shipped file name contains `%time%`, every start writes a new file and no earlier file is overwritten.

> [!IMPORTANT]
> A relative path such as `./logs` is resolved against the **working directory of the collector process**, not against the location of the executable. When the collector runs as a system service, the working directory is set by the service manager. Use an absolute path such as `/var/log/areg` on Linux to avoid surprises. `%user%` is also absolute, but for a system service it resolves to the home of the service account (`SYSTEM` on Windows, `root` under systemd), not of the interactive user.

> [!NOTE]
> The master switch `log::logcollector::enable` must stay `true`. Setting it to `false` disables all log targets of the process, including the database. The command line option `--log=db` starts the database regardless of both switches.

---

### Command Line Examples

```bash
# Save the collected logs, path taken from areg.init
logcollector --log=db

# Save the collected logs in an explicit file, masks are allowed
logcollector --log=db ./logs/session_%time%.sqlog

# Never save, even if areg.init enables the database
logcollector --log=nodb

# No option: areg.init decides
logcollector
```

An unknown value is rejected. The collector prints a message and exits **without starting the service**:

```bash
logcollector --log=text
# Unknown target [ text ] of the option '--log', expected either 'db' or 'nodb'.
```

---

### Controlling the Database on a Running Collector

In console mode, two commands start and stop the recording without restarting the process:

| Command | Short | Effect |
| --- | --- | --- |
| `--save [<path>]` | `-a` | Opens the database file and starts saving. Without a path, the path from `areg.init` is used. |
| `--unsave` | `-b` | Writes what is still pending, closes the file, and stops saving. |

```
--save
Started to save the collected logs in the database [ .../logs/logcollector_2026_08_18_11_20_35_120.sqlog ]

--unsave
Stopped saving the collected logs in the database
```

Two points worth knowing:

- When the database starts on a running collector, the **already connected log sources are written into the new file** at once, so their names are known. Their scope lists are not resent automatically; request them with `--query *`.
- If the configured file name contains `%time%`, calling `--save` again after `--unsave` creates a **new file**. The previous file stays untouched.

---

### Behaviour Under High Load

The collected messages are handed to a dedicated writer thread and are written in batches inside a single SQLite transaction. This keeps the network path free.

If logs arrive faster than the disk can absorb them for a long time, the queue grows. It is limited to **200 000 pending entries**. Beyond that limit:

1. New entries are dropped and counted, instead of growing the memory of the process without limit.
2. As soon as the writer catches up, one row is written into the `logs` table with priority `ERROR` and the text `Dropped [ N ] log messages, the logging database cannot keep up with the incoming data rate.`

So a gap in the file is always visible in the file itself.

---

### Reading the File

```bash
# List the log sources that were recorded
sqlite3 ./logs/logcollector_2026_08_18_11_20_35_120.sqlog \
        "SELECT cookie_id, inst_name, time_connected, time_disconnected FROM instances;"

# Show the last 20 messages with their source
sqlite3 ./logs/logcollector_2026_08_18_11_20_35_120.sqlog \
        "SELECT msg_module, msg_thread, msg_prio, msg_log FROM logs ORDER BY id DESC LIMIT 20;"
```

The file can also be opened with [Lusan](./06f-lusan-live-logging.md), which reads the same format.

---

### Limitations

- ❌ The **collected** logs cannot be written into a plain text file. The property `log::logcollector::enable::file` controls only the own logs of the `logcollector` process, not the logs it receives from other applications.
- ❌ `sqlite3` is the only supported engine. The remaining `db::*` properties (`driver`, `address`, `port`, `username`, `password`) are reserved for future engines and are ignored.
- ❌ The console command `--config`, which is meant to save the current configuration back into the file, is not implemented yet.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Command Line and Console Commands

The Log Collector accepts two different sets of commands:

- **Command line options** are passed when the process starts.
- **Console commands** are typed at the prompt of a running collector in console mode.

Some names appear in both sets, but the meaning is not always the same. `logcollector --help` prints the list and exits.

---

### Command Line Options

| Option | Short | Argument | Description |
| --- | --- | --- | --- |
| `--console` | `-c` | none | Run as a console application. This is the default. |
| `--service` | `-s` | none | Run as an OS managed service (systemd, Windows service). |
| `--install` | `-i` | none | Register as a Windows service. Windows only. |
| `--uninstall` | `-u` | none | Remove the Windows service registration. Windows only. |
| `--load=<path>` | `-l` | path | Read the configuration from the given file instead of the default one. |
| `--log=<target>` | `-d` | `db` or `nodb`, plus an optional file path | Decide whether the collected logs are saved in a database. Overrides `areg.init`. |
| `--help` | `-h` | none | Print the list of options and exit without starting the service. |

```bash
logcollector --console
logcollector --service
logcollector --load=./config/mycollector.init --log=db
```

---

### Console Commands

| Command | Short | Description |
| --- | --- | --- |
| `--save [<path>]` | `-a` | Start saving the collected logs in the database. |
| `--unsave` | `-b` | Stop saving the collected logs in the database. |
| `--query <target>` | `-e` | Ask the log sources for their list of scopes. `*` means all sources, a number means one cookie ID. |
| `--scope <update>` | `-o` | Change the priority of a scope or a group of scopes on a running application. |
| `--instances` | `-n` | Print the list of connected instances with their cookie ID and bitness. |
| `--pause` | `-p` | Close the server connection. Connected applications keep running. |
| `--restart` | `-r` | Open the server connection again. |
| `--verbose` | `-v` | Show the data rate rows. Requires a build with extended features. |
| `--silent` | `-t` | Hide the data rate rows. Requires a build with extended features. |
| `--help` | `-h` | Print the list of commands on the console. |
| `--quit` | `-q` | Stop the Log Collector and exit. |
| `--config` | `-f` | Save the current configuration in the file. **Not implemented yet.** |

**Scope update syntax**

```
--scope <cookie>::<scope-name>=<priority>
```

```
--scope *::areg_base_NESocket=NOTSET     # silence one scope in every application
--scope 257::myapp_Component_*=DEBUG     # full detail for a scope group of one application
--query *                                # list the scopes of every connected application
--query 257                              # list the scopes of one application
```

> [!NOTE]
> The options `--console`, `--service`, `--install`, `--uninstall`, `--load` and `--log` are command line only. Typing them at the console prompt prints a hint instead of doing anything.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Application Integration

### Configure Application to Use Log Collector

**Step 1: Configure remote logging**  
  Create or edit application's `areg.init`:
```ini
# Enable remote logging
log::*::target = remote
log::*::enable::remote = true

# Log Collector connection
logger::*::service          = logcollector
logger::*::connect          = tcpip
logger::*::enable::tcpip    = true
logger::*::address::tcpip   = 127.0.0.1  # Log Collector address
logger::*::port::tcpip      = 8282       # Log Collector port

# Enable scopes
log::*::scope::* = DEBUG | SCOPE
```

**Step 2: Initialize application with logging**
```cpp
#include "areg/appbase/Application.hpp"

int main()
{
    // Initialize with logging and configuration
    Application::setup(
        true,   // Enable logging
        true,   // Enable service manager
        true,   // Enable router client (if needed)
        true,   // Enable timer manager
        true,   // Enable watchdog manager
        "./config/myapp.init",  // Configuration file
        nullptr
    );
    
    // Application code
    Application::load_model("MyServiceModel");
    Application::wait_quit(areg::WAIT_INFINITE);
    
    Application::release();
    return 0;
}
```

> [!NOTE]
> Applications automatically connect to Log Collector when started. No rigid startup order required - apps will reconnect if Log Collector restarts.

---

### Network Deployment Example

**Scenario: Distributed applications with centralized logging**

**Machine 1 (192.168.1.100) - Log Collector:**

```bash
# Configure Log Collector
# Edit /usr/local/share/areg/areg.init
logger::*::address::tcpip = 192.168.1.100
logger::*::port::tcpip    = 8282

# Start service
sudo systemctl start logcollector.service
```

**Machine 2 (192.168.1.101) - Application:**

```ini
# myapp.init
logger::*::address::tcpip = 192.168.1.100
logger::*::port::tcpip    = 8282
log::*::target = remote
```

**Machine 3 (192.168.1.102) - Log Observer:**

```ini
# logobserver.init
logger::*::address::tcpip = 192.168.1.100
logger::*::port::tcpip    = 8282
```

**Result:** All logs flow through centralized Log Collector.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### Service Won't Start (Linux)

**Problem:** `systemctl start` fails  
**Solution:**  
1. *Check service status:*
    ```bash
    sudo systemctl status logcollector.service
    ```
2. *Check logs:*
    ```bash
    sudo journalctl -u logcollector.service -n 50
    ```
3. *Verify executable exists:*
    ```bash
    ls -l /usr/local/tools/areg/logcollector
    # Should exist and be executable
    ```
4. *Check library dependencies:*
    ```bash
    ldd /usr/local/tools/areg/logcollector
    # Verify libareg.so is found
    ```
    *If missing:*
        ```bash
        sudo cp ./product/build/.../lib/libareg.so /usr/lib/
        sudo ldconfig
        ```
5. *Verify service file:*
    ```bash
    cat /etc/systemd/system/logcollector.service
    # Check ExecStart path is correct
    ```
6. *Reload and restart:*
    ```bash
    sudo systemctl daemon-reload
    sudo systemctl restart logcollector.service
    ```

---

### Service Won't Start (Windows)

**Problem:** Service fails to start in Services app  
**Solution:**
1. *Check Event Viewer:*
    1. `Win + R` → `eventvwr.msc`
    2. **Windows Logs → Application**
    3. Look for logcollector or logger.service errors

2. *Verify binaries exist:*
    ```powershell
    Test-Path "C:\Program Files\areg\tools\areg\logcollector.exe"
    Test-Path "C:\Program Files\areg\tools\areg\areg.dll"
    ```
3. *Check service installation:*
    ```powershell
    Get-Service "logger.service"
    # Should exist
    ```
    **If not found, reinstall:**
    ```powershell
    cd "C:\Program Files\areg\tools\areg"
    .\logcollector.exe --install
    ```
4. *Verify permissions:* Service must run with sufficient privileges.
5. *Try manual start:*
    ```powershell
    Start-Service "logger.service"
    # Check error message
    ```

---

### Port Already in Use

**Problem:** "Address already in use" error  
**Solution:**
1. *Check what's using the port:*
    **Linux:**
    ```bash
    sudo lsof -i :8282
    ```
    **Windows:**
    ```powershell
    netstat -ano | findstr :8282
    ```

2. *Kill conflicting process:*
    **Linux:**
    ```bash
    sudo kill -9 <PID>
    ```
    **Windows:**
    ```powershell
    taskkill /PID <PID> /F
    ```
3. *Or change port:* Edit `areg.init`:
    ```ini
    logger::*::port::tcpip = 9090
    ```

Restart service.

---

### Applications Cannot Connect

**Problem:** Applications fail to connect to Log Collector  
**Solution:**
1. *Verify Log Collector is running:*
    **Linux:**
    ```bash
    sudo systemctl status logcollector.service
    ```
    **Windows:**
    ```powershell
    Get-Service "logger.service"
    ```
2. *Check Log Collector is listening:*
    **Linux:**
    ```bash
    sudo netstat -tlnp | grep 8282
    ```
    **Windows:**
    ```powershell
    netstat -ano | findstr :8282
    ```
    Should show LISTENING
3. *Test connectivity:*
    **Same machine:**
    ```bash
    telnet 127.0.0.1 8282
    ```
    **Different machine:**
    ```bash
    telnet 192.168.1.100 8282
    ```
4. *Check application configuration:*
    ```ini
    # In application's areg.init
    logger::*::address::tcpip = 127.0.0.1  # Must match Log Collector
    logger::*::port::tcpip    = 8282       # Must match Log Collector
    log::*::target = remote                # Must include remote
    ```
5. *Check firewall:*
    **Linux:**
    ```bash
    sudo ufw allow 8282/tcp
    ```
    **Windows:**
    ```powershell
    New-NetFirewallRule -DisplayName "Log Collector" -Direction Inbound -Protocol TCP -LocalPort 8282 -Action Allow
    ```

---

### Log Observers Cannot Connect

**Problem:** Log Observer fails to connect to Log Collector  
**Solution:**
  Same steps as "Applications Cannot Connect" above.  
  Additionally verify Log Observer configuration:
  ```ini
  # In logobserver's areg.init
  logger::*::address::tcpip = 127.0.0.1  # Log Collector address
  logger::*::port::tcpip    = 8282       # Log Collector port
  ```

---

### High Memory/CPU Usage

**Problem:** Log Collector consuming excessive resources  
**Solution:**
1. *Reduce log volume at source:* Configure applications to log less:
    ```ini
    # In application's areg.init
    log::*::scope::* = WARN  # Only warnings and errors
    ```
2. *Disable Log Collector's own logging:*
    ```ini
    log::logcollector::scope::* = NOTSET
    ```
3. *Monitor system resources:*
    **Linux:**
    ```bash
    top -p $(pgrep logcollector)
    ```
    **Windows:**
    ```powershell
    Get-Process logcollector
    ```

---

### No Database File Is Created

**Problem:** The collector runs, but no `.sqlog` file appears  
**Solution:**
1. *Check that the database is switched on.* Start the collector with the explicit option, which ignores the configuration file:
    ```bash
    logcollector --console --log=db
    ```
    In console mode the collector prints the path it uses:
    ```
    Saving the collected logs in the database [ .../logs/logcollector_<time>.sqlog ]
    ```
2. *Check the configuration keys.* All four keys belong to the `logcollector` module, not to `*`:
    ```ini
    log::logcollector::enable::db   = true
    log::logcollector::db::engine   = sqlite3
    log::logcollector::db::name     = logcollector_%time%.sqlog
    log::logcollector::db::location = ./logs
    ```
3. *Check the master switch.* `log::logcollector::enable = false` disables every log target of the collector, the database included.
4. *Check the engine value.* Only `sqlite3` is supported. A different value switches the database off silently.
5. *Check where you are looking.* A relative `location` is resolved against the working directory of the process. For a systemd service that is not the directory of the executable:
    ```bash
    sudo find / -name "logcollector_*.sqlog" 2>/dev/null
    ```
    Set an absolute path to make this predictable:
    ```ini
    log::logcollector::db::location = /var/log/areg
    ```
6. *Check the write permission* of the target directory. The service user must be allowed to create files there.

---

### The Database File Exists but Stays Empty

**Problem:** The `.sqlog` file is created, but the `logs` table has no rows  
**Solution:**
1. *Check that log sources are connected:* type `--instances` on the console. An empty list means the applications never reached the collector, see "Applications Cannot Connect" above.
2. *Check that the applications actually log:* their scopes must be enabled.
    ```ini
    # In the application's areg.init
    log::*::enable::remote = true
    log::myapp::scope::*   = DEBUG | SCOPE
    ```
3. *Do not expect the collector's own logs there.* The collector does not send its own logs to itself. Apart from the report about dropped messages, the file contains only the logs it received from other applications.

---

### Messages Are Missing from the Database

**Problem:** Fewer rows than expected  
**Solution:**  
  Search the `logs` table for the report the collector writes itself:
  ```bash
  sqlite3 <file>.sqlog "SELECT msg_log FROM logs WHERE msg_log LIKE 'Dropped%';"
  ```
  A row such as `Dropped [ 1523 ] log messages, ...` means the incoming data rate was higher than the disk could absorb for a long time and the queue limit was reached. Reduce the log volume at the source, or write the file to a faster disk.
  ```ini
  # In the application's areg.init
  log::*::scope::* = WARN
  ```

---

### Service Crashes or Stops Unexpectedly

**Problem:** Service stops running  
**Solution:**
1. *Check service logs:*
    **Linux:**
    ```bash
    sudo journalctl -u logcollector.service -n 100
    ```
    **Windows:**
    Event Viewer → Application logs

2. *Verify auto-restart is configured:*  
    **Linux service file should have:**
    ```ini
    [Service]
    Restart=on-failure
    RestartSec=5s
    ```
3. *Check system resources:*  
    Ensure sufficient memory and disk space.
4. *Review configuration:*
    Invalid configuration can cause crashes.
5. *Test in console mode:*
    ```bash
    # Stop service
    sudo systemctl stop logcollector.service

    # Run in console
    /usr/local/tools/areg/logcollector --console

    # If works in console, issue is with service configuration
    ```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Related Guides:**
- [Log Observer Guide](./04c-logobserver.md) - Log monitoring and control tool
- [Log Database Format](./04e-log-database-format.md) - Schema of the recorded `.sqlog` files and SQL queries
- [Logging Configuration Guide](./04a-logging-config.md) - Configure application logging
- [Logging Development Guide](./04b-logging-develop.md) - Add logging to code
- [Lusan Live Log Viewer](./06f-lusan-live-logging.md) - GUI log viewer

**Configuration:**
- [areg.init](../../framework/areg/resources/areg.init) - Default configuration
- [Areg Configuration Reference](./05b-areg-configuration-file.md) - `areg.init` properties

**Service Files:**
- [logcollector.service](../../framework/logcollector/resources/logcollector.service) - Linux service file

**Source Code:**
- [logcollector Module](../../framework/logcollector/) - Implementation

**Help:**
For questions, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech (Artak Avetyan), www.areg.tech, email: info[at]areg.tech
