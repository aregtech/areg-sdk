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
6. [Application Integration](#application-integration)
7. [Troubleshooting](#troubleshooting)

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
                               ▲
                               │ Control
                               │ Commands
                               │
                        (Scope changes,
                         priority updates)
```

**Components:**
1. **Applications** - Send logs to Log Collector
2. **Log Collector** - Central aggregation and routing service (this guide)
3. **Log Observers** - Receive logs, store data, control scopes

> [!NOTE]
> Log Collector supports multiple applications sending logs and multiple log observers receiving logs simultaneously. Each observer receives all log messages, enabling independent storage and analysis.

---

### Communication Flow

**Log Flow:**
```
Application → Log Collector → Log Observer(s) → Storage (File/Database)
```

**Control Flow:**
```
Log Observer → Log Collector → Application (scope/priority updates)
```

---

### When to Use Log Collector

**Use Log Collector when:**
- ✅ Applications are distributed across network
- ✅ Centralized log collection needed
- ✅ Multiple applications logging simultaneously
- ✅ Runtime scope control required
- ✅ Using Log Observer or Lusan for log analysis

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

| Property | Default | Description |
|----------|---------|-------------|
| `logger::*::service` | `logcollector` | Log Collector process name |
| `logger::*::connect` | `tcpip` | Communication protocol (TCP/IP only) |
| `logger::*::enable::tcpip` | `true` | Enable/disable TCP/IP |
| `logger::*::address::tcpip` | `127.0.0.1` | IP address to bind |
| `logger::*::port::tcpip` | `8282` | Port number |

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

**See:** [Persistence Syntax Guide](./06a-persistence-syntax.md) for complete syntax

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
    Application::initApplication(
        true,   // Enable logging
        true,   // Enable service manager
        true,   // Enable router client (if needed)
        true,   // Enable timer manager
        true,   // Enable watchdog manager
        "./config/myapp.init",  // Configuration file
        nullptr
    );
    
    // Application code
    Application::loadModel("MyServiceModel");
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    
    Application::releaseApplication();
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
- [Logging Configuration Guide](./04a-logging-config.md) - Configure application logging
- [Logging Development Guide](./04b-logging-development.md) - Add logging to code
- [Lusan Live Log Viewer](./06f-lusan-live-logging.md) - GUI log viewer

**Configuration:**
- [areg.init](../../framework/areg/resources/areg.init) - Default configuration
- [Persistence Syntax](./06a-persistence-syntax.md) - Configuration file format

**Service Files:**
- [logcollector.service](../../framework/logcollector/resources/logcollector.service) - Linux service file

**Source Code:**
- [logcollector Module](../../framework/logcollector/) - Implementation

**Help:**
For questions, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
