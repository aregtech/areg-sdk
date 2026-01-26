# Areg SDK Log Observer Application

The Areg SDK Log Observer is a console application for real-time log monitoring, management, and dynamic scope control of logs received from distributed applications.

> [!TIP]
> Log Observer connects to Log Collector to receive, store, and control logs from multiple applications across your network.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Overview](#overview)
3. [Installation](#installation)
4. [Running Log Observer](#running-log-observer)
5. [Interactive Commands](#interactive-commands)
6. [Configuration](#configuration)
7. [Common Scenarios](#common-scenarios)
8. [Troubleshooting](#troubleshooting)

---

## Quick Start

### Start Receiving Logs in 1 Minute

**Step 1: Start Log Collector**

```bash
# Terminal 1 - Start log collector service
./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/logcollector --console
```

**Step 2: Start Log Observer**

```bash
# Terminal 2 - Start log observer
./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/logobserver
```

**Expected output:**
```
Areg Log Observer console application ...
---------------------------------------------------------------------------------------------

The log observer is initialized, type '-r' to connect.
Type '-q' or '--quit' to quit the application ...:
```

**Step 3: Start receiving logs** 
Type `-r` or `--restart` in Log Observer console:

```
Areg Log Observer console application ...
---------------------------------------------------------------------------------------------

Log observer triggered connection.
Type '-q' or '--quit' to quit the application ...:
```

**Step 4: Run your application with remote logging**

```bash
# Terminal 3 - Run your application
./myapp
```

**Logs saved to:**
- Text file: `./logs/myapp_2026_01_25_23_39_41_801.log.log`
- Database: `./logs/log_2026_01_26_17_58_32_215.sqlog`

**Setup time:** ~1 minute

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Overview

### What is Log Observer?

The Log Observer is a console application that acts as a client to [Log Collector](./04d-logcollector.md), providing:

**Data Management:**
- ✅ Receive logs from multiple distributed applications
- ✅ Save logs to text files
- ✅ Store logs in SQLite database for analysis
- ✅ Real-time log controlling in console

**Dynamic Control:**
- ✅ Adjust scope activation at runtime
- ✅ Change log priorities without restarting applications
- ✅ Filter log messages dynamically
- ✅ Query connected application instances

---

### Architecture

```
┌─────────────┐         ┌──────────────┐         ┌──────────────┐
│ Application │────────►│     Log      │◄────────│     Log      │
│     A       │         │  Collector   │         │   Observer   │
└─────────────┘         │   (Server)   │         │   (Client)   │
                        │              │         │              │
┌─────────────┐         │   Port 8282  │         │ - Stores     │
│ Application │────────►│              │         │ - Controls   │
│     B       │         │              │         │ - Queries    │
└─────────────┘         └──────────────┘         └──────────────┘
```

**Components:**
1. **Applications** - Send logs to Log Collector
2. **Log Collector** - Central log aggregation service
3. **Log Observer** - Receives, stores, and controls logs

> [!NOTE]
> Multiple Log Observer instances can connect to the same Log Collector simultaneously. The Log Collector broadcasts all log messages to every connected observer, enabling each to independently store logs in files and databases. Log Observer uses SQLite3 as its database engine for structured log storage and analysis.

---

### Key Features

| Feature                    | Description                    | Benefit                   |
|----------------------------|--------------------------------|---------------------------|
| **Real-time Monitoring**   | View logs as they arrive       | Immediate feedback        |
| **Dual Storage**           | Text files + SQLite database   | Flexible analysis         |
| **Dynamic Scope Control**  | Change scopes without restart  | Optimize network traffic  |
| **Multi-application**      | Logs from all connected apps   | Centralized monitoring    |
| **Query Scopes**           | List available scopes          | Discover logging points   |
| **Save Configuration**     | Export current scope settings  | Reproduce setups          |
| **Multi-Observer Support** | Multiple observers can connect | Team collaboration        |

---

### When to Use Log Observer

**Use Log Observer when:**
- ✅ Monitoring distributed applications without [`Lusan`](https://github.com/aregtech/areg-sdk-tools) application
- ✅ Need database storage for analysis
- ✅ Runtime scope control required
- ✅ Centralized log collection needed

**Alternative tools:**
- **Lusan** - GUI log viewer ([Lusan Live Log Viewer](./06f-lusan-live-logging.md))
- **Text files** - Direct file logging ([Logging Configuration](./04a-logging-config.md))

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Installation

### From Build Directory

After building Areg SDK, Log Observer is available in the build output directory.

**Default build paths:**

**Linux:**
```bash
./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/logobserver
```

**Windows:**
```powershell
.\product\build\msvc-v143\windows_nt-x64-release\bin\logobserver.exe
```

---

### From Installed Location

If Areg SDK is installed via CMake:

```bash
# Install Areg SDK
sudo cmake --install ./build

# Log Observer location
/usr/local/tools/areg/logobserver  # Linux
C:\Program Files\areg\tools\areg\logobserver.exe  # Windows
```

---

### Prerequisites

**Required:**
- Log Collector must be running
- Network connectivity to Log Collector
- Applications configured for remote logging

**Optional:**
- SQLite3 (bundled with Areg SDK)
- Write permissions for log directory

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Running Log Observer

### Start in Console Mode

**Basic usage:**

```bash
./logobserver
```

**Expected output:**
```
Areg Log Observer console application ...
---------------------------------------------------------------------------------------------


The log observer is initialized, type '-r' to connect.
Type '-q' or '--quit' to quit the application ...:
```

---

### Start with Custom Configuration

**Specify configuration file:**

```bash
./logobserver --load="./config/custom.init"
```
---

### Connect and Start Receiving

**Once started, activate log reception:**

Type `-r` or `--restart`:

```
Areg Log Observer console application ...
---------------------------------------------------------------------------------------------


Log observer triggered connection.
Type '-q' or '--quit' to quit the application ...:
```

**Check connected applications:**

Type `-n` or `--instances`:

```

Areg Log Observer console application ...
---------------------------------------------------------------------------------------------

List of connected instances ...
Type '-q' or '--quit' to quit the application ...:

---------------------------------------------------------------------------------------------
   Nr. |  Inst. ID  |  Bits |  Scopes  |  Name
---------------------------------------------------------------------------------------------
    1. |        257 |  x64  |      87  |  19_pubclient.exe
---------------------------------------------------------------------------------------------
```

---

### Stop Receiving

**Pause log reception:**

Type `-x` or `--stop`:

```
AREG Log Observer console application ...
---------------------------------------------------------------------------------------------

Log observer stops, type '-r' to resume.
Type '-q' or '--quit' to quit the application ...:
```

Type `-r` again to resume.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Interactive Commands

### Command Reference

Commands available while Log Observer is running:

| Command       | Alias | Description                   | Example                   |
|---------------|-------|-------------------------------|---------------------------|
| `--help`      | `-h`  | Display command list          | `-h`                      |
| `--instances` | `-n`  | Show connected applications   | `-n`                      |
| `--query`     | `-e`  | Display application scopes    | `-e myapp`                |
| `--scope`     | `-o`  | Set scope priority            | `-o myapp::scope_*=DEBUG` |
| `--config`    | `-f`  | Save current configuration    | `-f`                      |
| `--restart`   | `-r`  | Start/restart receiving logs  | `-r`                      |
| `--stop`      | `-x`  | Stop receiving logs           | `-x`                      |
| `--pause`     | `-p`  | Pause log observer            | `-p`                      |
| `--quit`      | `-q`  | Quit application              | `-q`                      |
| `--load`      | `-l`  | Load configuration file       | `-l=config.init`          |

---

### Command Examples

**1. Display help:** type `-h` or `--help``

```
AREG Log Observer console application ...
---------------------------------------------------------------------------------------------

Type '-q' or '--quit' to quit the application ...:

Usage of AREG Log Observer console application :
---------------------------------------------------------------------------------------------
-e, --query     : Query the list of logging scopes. Usage: --query *, '*' can be a cookie ID.
-f, --config    : Save current configuration.       Usage: --config
-h, --help      : Display this message on console.  Usage: --help
-l, --load      : Command line option to configure. Usage: './logobserver --load=<path-to-init-file>'
-n, --instances : Display list of log instances.    Usage: --instances
-o, --scope     : Update log scope priorities.      Usage: --scope *::areg_base_NESocket=NOTSET, '*' can be a cookie.
-p, --pause     : Pause the log observer.           Usage: --pause
-q, --quit      : Stop and quit the log observer.   Usage: --quit
-r, --restart   : Start / continue log observer.    Usage: --restart
-x, --stop      : Stop log observer.                Usage: --stop
---------------------------------------------------------------------------------------------
```

**2. Show connected applications:** type `-n` or `--instances`

```
AREG Log Observer console application ...
---------------------------------------------------------------------------------------------

List of connected instances ...
Type '-q' or '--quit' to quit the application ...:

---------------------------------------------------------------------------------------------
   Nr. |  Inst. ID  |  Bits |  Scopes  |  Name
---------------------------------------------------------------------------------------------
    1. |        257 |  x64  |      87  |  19_pubclient.exe
    2. |        258 |  x64  |     132  |  19_pubservice.exe
---------------------------------------------------------------------------------------------
```

**3. Change scope priority:** type `-o provider::provider_Network_connect=DEBUG`  
     **Enable all scopes with wildcard:** type `-o myapp::*=DEBUG|SCOPE`

**4. Save current configuration:** type `-f`
    **Generated config file:**
    ```ini
    log::provider::scope::provider_main_initialize = DEBUG | SCOPE
    log::provider::scope::provider_ServiceImpl_processData = DEBUG | SCOPE
    log::provider::scope::provider_Network_connect = DEBUG
    ```

**5. Pause and resume:** type `-p` to pause, `-r` to resume`

**6. Quit application:** type `-q` or `--quit`

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Configuration

### Configuration File

**Default:** `areg.init` in Log Observer directory  
**Custom location:**
```bash
logobserver --load="./config/custom.init"
```

### Log Collector Connection

**Configure connection to Log Collector:**

```ini
# Log Collector connection settings
logger::*::service          = logcollector  # Service name
logger::*::connect          = tcpip         # Protocol
logger::*::enable::tcpip    = true          # Enable TCP/IP
logger::*::address::tcpip   = 127.0.0.1     # Log Collector IP
logger::*::port::tcpip      = 8282          # Log Collector port
```

**Configuration breakdown:**

| Property                      | Default           | Description                   |
|-------------------------------|-------------------|-------------------------------|
| `logger::*::service`          | `logcollector`    | Log Collector process name    |
| `logger::*::connect`          | `tcpip`           | Communication protocol        |
| `logger::*::enable::tcpip`    | `true`            | Enable/disable TCP/IP         |
| `logger::*::address::tcpip`   | `127.0.0.1`       | Log Collector IP address      |
| `logger::*::port::tcpip`      | `8282`            | Log Collector port            |

### Log Storage Configuration

**Configure where logs are saved:**

```ini
# Enable file and database logging
log::logobserver::enable::file = true
log::logobserver::enable::db   = true

# Database settings
log::logobserver::db::name     = sqlite3
log::logobserver::db::location = ./logs/log_%time%.sqlite3

# File location (default)
log::logobserver::file::location = ./logs/logobserver.log
```

### Message Layout

**Customize log message format:**

```ini
# Scope entry layout
log::logobserver::layout::enter = %d: [ %a.%x.%t.%z: Enter -->]%n

# Log message layout
log::logobserver::layout::message = %d: [ %a.%x.%t.%p >>> ] %m%n

# Scope exit layout
log::logobserver::layout::exit = %d: [ %a.%x.%t.%z: Exit <-- ]%n
```

**Format specifiers:**
- `%d` - Date and time
- `%a` - Application instance ID
- `%x` - Application name
- `%t` - Thread ID
- `%p` - Priority level
- `%m` - Message text
- `%z` - Scope name
- `%n` - Newline

**See:** [Logging Configuration Guide](./04a-logging-config.md) for complete format reference

### Disable Log Observer's Own Logging

**Prevent Log Observer from logging about itself:**

```ini
# Disable Log Observer self-logging
log::logobserver::scope::*  = NOTSET
log::logcollector::scope::* = NOTSET
```

### Network Deployment

**Scenario: Log Observer on different machine from Log Collector**  
**Machine 1 (192.168.1.100) - Log Collector:**
```bash
./logcollector --console
```

**Machine 2 (192.168.1.101) - Log Observer:**
```ini
# logobserver config
logger::*::address::tcpip = 192.168.1.100
logger::*::port::tcpip    = 8282
```

```bash
./logobserver --console --load="./config/remote.init"
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Common Scenarios

### Scenario 1: Monitor All Applications

**Goal:** Receive logs from all connected applications.  
**Steps:**

1. *Start Log Collector:*
    ```bash
    ./logcollector --console
    ```
2. *Start Log Observer:*
    ```bash
    ./logobserver
    ```
3. *Start receiving:* type `-r`
4. *Check connected apps:* `-n`

### Scenario 2: Debug Specific Module

**Goal:** Enable detailed logging for specific module at runtime.
**Steps:** Set log priority for network module to DEBUG
```
-o myapp::myapp_network_*=DEBUG|SCOPE
```

### Scenario 3: Reduce Network Traffic

**Goal:** Decrease network traffic by filtering low-priority logs.  
**Steps:** Set all scopes to WARN
```
-o myapp::*=WARN
```
**Result:** Only warnings and errors transmitted.

### Scenario 4: Save Configuration for Later

**Goal:** Save current scope settings to reproduce later.  
**Steps:**
1. *Configure scopes as needed:*
    ```
    -o app1::module_*=DEBUG
    -o app2::network_*=INFO
    -o app2::database_*=ERROR
    ```
2. *Save configuration:* type `-f`
3. *Later, load saved configuration:*
    ```bash
    ./logobserver --load="./config/logobserver_config.init"
    ```

**Result:** Same scope settings restored.

### Scenario 5: Database Analysis

**Goal:** Analyze logs using SQL queries.
**Steps:**
1. *Ensure database logging enabled:*
    ```ini
    log::logobserver::enable::db = true
    log::logobserver::db::location = ./logs/log_%time%.sqlite3
    ```
2. *Collect logs:* type `-r`
3. *Open database with SQLite:*
    ```bash
    sqlite3 ./logs/log_20260127_103045.sqlite3
    ```
4. *Query logs:*
    ```sql
    -- Count errors per application
    SELECT app_name, COUNT(*) 
    FROM logs 
    WHERE priority = 'ERROR' 
    GROUP BY app_name;
    ```

**Result:** Powerful SQL-based log analysis.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### Cannot Connect to Log Collector

**Problem:** Log Observer cannot connect to Log Collector.  
**Solution:**
1. *Verify Log Collector is running:*
    ```bash
    # Check process
    ps aux | grep logcollector  # Linux
    tasklist | findstr logcollector  # Windows
    ```
2. *Check network connectivity:*
    ```bash
    telnet 127.0.0.1 8282
    # or
    nc -zv 127.0.0.1 8282
    ```
3. *Verify configuration:*
    ```ini
    logger::*::address::tcpip = 127.0.0.1  # Correct address?
    logger::*::port::tcpip    = 8282       # Correct port?
    ```
4. *Check firewall:*
    ```bash
    # Linux
    sudo ufw allow 8282/tcp

    # Windows
    New-NetFirewallRule -DisplayName "Log Collector" -Direction Inbound -Protocol TCP -LocalPort 8282 -Action Allow
    ```

### No Logs Received

**Problem:** Log Observer connected but no logs appear.  
**Solution:**
1. *Start receiving:* type `-r`
2. *Check connected applications:* type `-n`
3. *Verify applications are configured for remote logging:*
    ```ini
    # In application's areg.init
    log::*::target = remote
    logger::*::address::tcpip = 127.0.0.1
    logger::*::port::tcpip = 8282
    ```
4. *Check application scopes are enabled:*
    ```
    > -e myapp
    Check if scopes are not set to NOTSET
    ```

---

### Database File Not Created

**Problem:** SQLite database file doesn't exist.  
**Solution:**
1. *Check database logging is enabled:*
    ```ini
    log::logobserver::enable::db = true
    ```
2. *Check directory exists:*
    ```bash
    mkdir -p ./logs
    ```
3. *Check write permissions:*
    ```bash
    chmod 755 ./logs
    ```
4. *Verify database location:*
    ```ini
    log::logobserver::db::location = ./logs/log_%time%.sqlite3
    ```
5. *Restart Log Observer:*

---

### Scope Changes Not Working

**Problem:** Changed scope priority but no effect.  
**Solution:**
1. *Verify correct application name:* type `-n`
2. *Use correct syntax:*
    ```
    # Correct
    > -o myapp::module_network=DEBUG
    > -o myapp::module_*=DEBUG|SCOPE

    # Wrong
    > -o myapp module_network DEBUG
    ```
3. *Verify application supports dynamic scope changes:*

Applications must be connected to Log Collector to receive scope updates.

---

### Log File Too Large

**Problem:** Log files growing too large.
**Solution:**
1. *Reduce log level:*
    ```
    > -o *::*=WARN
    All applications set to WARN
    ```
2. *Disable file logging:*
    ```ini
    log::logobserver::enable::file = false
    # Keep only database
    log::logobserver::enable::db = true
    ```
3. *Use separate files per run:*
    ```ini
    log::logobserver::file::location = ./logs/log_%time%.log
    ```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Related Guides:**
- [Log Collector Guide](./04d-logcollector.md) - Central log collection service
- [Logging Configuration Guide](./04a-logging-config.md) - Configure application logging
- [Logging Development Guide](./04b-logging-development.md) - Add logging to code
- [Lusan Live Log Viewer](./06f-lusan-live-logging.md) - GUI log viewer alternative

**Configuration Files:**
- [areg.init](../../framework/areg/resources/areg.init) - Default configuration

**Source Code:**
- [logobserver Module](../../framework/logobserver/) - Implementation
- [areglogger Library](../../framework/areglogger/) - Log observer API

**Help:**
For questions, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
