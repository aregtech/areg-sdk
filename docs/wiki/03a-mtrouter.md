# Areg SDK Multitarget Router (mtrouter) Guide

The Multitarget Router enables inter-process and networked communication between Service Provider and Consumer nodes in distributed Areg SDK applications.

> [!TIP]
> For single-process multithreading applications, mtrouter is not required. See [Quick Project Setup](./02a-quick-project-setup.md) for creating projects.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Overview](#overview)
3. [Running as Console Application](#running-as-console-application)
4. [Running as System Service](#running-as-system-service)
5. [Configuration](#configuration)
6. [Command Reference](#command-reference)
7. [Application Integration](#application-integration)
8. [Troubleshooting](#troubleshooting)

---

## Quick Start

### Run mtrouter in 10 Seconds

If run from `areg-sdk` root build with default settings for **Linux**:
```bash
./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/mtrouter.elf --console
```

If run from `areg-sdk` root build with default settings for **Windows**:
```powershell
.\product\build\msvc-v143\windows_nt-x64-release\bin\mtrouter.exe --console
```

**Expected output:**
```
Areg Message Router console application ...
---------------------------------------------------------------------------------------------
Send data with the rate:    0.00  Bytes / sec.
Recv data with the rate:    0.00  Bytes / sec.

Type '-q' or '--quit' to quit the application ...:
```

**Test connection:** Run executable of any IPC example, for example `15_pubclient.exe`, and type `-n` in `mtrouter` console to see connected clients like this: 
```
Areg Message Router console application ...
---------------------------------------------------------------------------------------------
Send data with the rate:    0.00  Bytes / sec.
Recv data with the rate:    0.00  Bytes / sec.

Type '-q' or '--quit' to quit the application ...:

---------------------------------------------------------------------------------------------
   Nr. |  Instance ID  |  Bitness  |  Name
---------------------------------------------------------------------------------------------
    1. |          256  |    64     |  15_pubclient.exe
    2. |          257  |    64     |  15_pubservice.exe
---------------------------------------------------------------------------------------------
```

**Type '-q' to quit**

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Overview

### What is mtrouter?

The **Multitarget Message Router** (mtrouter) is a message routing service that:

- ✅ Routes messages between distributed processes
- ✅ Enables inter-process communication (IPC)
- ✅ Supports network communication (TCP/IP)
- ✅ Helps to manage service discovery
- ✅ Handles multiple software connections

### When Do You Need mtrouter?

| Application Type                      | Requires mtrouter | Communication     |
|---------------------------------------|-------------------|-------------------|
| **Multithreading** (single process)   | ❌ No              | In-process        |
| **Multiprocessing** (same machine)    | ✅ Yes             | Local IPC         |
| **Distributed** (network)             | ✅ Yes             | Network (TCP/IP)  |

**Without mtrouter:**
- Multiprocessing applications run as isolated multithreading applications
- Internal (local) services work within each process
- External (public) services cannot be accessed by other processes
- No inter-process communication possible

**With mtrouter:**
- Processes can communicate via centralized message routing
- Service discovery enables automatic connection
- Public services become accessible across processes
- Seamless scaling from local to network deployment

### Architecture

**Without `mtrouter` (multithreading):**
```
┌────────────────────────────┐
│     Single Process         │
│  ┌──────────┐ ┌──────────┐ │
│  │ Provider │ │ Consumer │ │
│  └──────────┘ └──────────┘ │
└────────────────────────────┘
```

**With `mtrouter` (multiprocessing/distributed):**
```
┌─────────────┐         ┌──────────┐         ┌─────────────┐
│  Provider   │────────►│ mtrouter │◄────────│  Consumer   │
│  Process    │         │ (Router) │         │  Process    │
└─────────────┘         └──────────┘         └─────────────┘
```

### Deployment Options

**Console Application:**
- Run manually for development/testing
- Easy debugging and monitoring
- Stop with `-q`, `--quit` or Ctrl+C

**System Service:**
- Automatic startup at boot
- Runs in background
- Managed by OS (systemd/Windows Services)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Running as Console Application

### Basic Usage

**Linux/macOS:**
```bash
./mtrouter --console
```

**Windows:**
```powershell
.\mtrouter.exe --console
```

**Expected output:**
```
Areg Message Router console application ...
---------------------------------------------------------------------------------------------
Type '-q' or '--quit' to quit the application ...: -n
```

---

### Console Commands

While mtrouter is running, use these single-key commands:

| Key    | Command   | Description                           |
|--------|-----------|---------------------------------------|
| **-h** | Help      | Display available commands            |
| **-n** | Instances | Show connected clients                |
| **-p** | Pause     | Pause message routing                 |
| **-r** | Restart   | Restart connections                   |
| **-q** | Quit      | Stop mtrouter                         |
| **-t** | Silent    | Toggle silent mode (no output)        |
| **-v** | Verbose   | Toggle verbose mode (detailed output) |

**Example - Check connected clients:**

Press **'-n'**:
```
---------------------------------------------------------------------------------------------
   Nr. |  Instance ID  |  Bitness  |  Name
---------------------------------------------------------------------------------------------
    1. |          256  |    64     |  15_pubclient.exe
    2. |          257  |    64     |  15_pubservice.exe
---------------------------------------------------------------------------------------------
```

---

### Custom Configuration

**Load custom configuration file:**

```bash
./mtrouter --console --load="./config/custom.init"
```

---

### Silent Mode

**Run without console output:**

```bash
./mtrouter --console --silent
```

**Use case:** Background operation during development

---

### Verbose Mode

**Show detailed message routing:**
```bash
./mtrouter --console --verbose
```

**Expected output:** no data rate shown.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Running as System Service

### Linux Service (systemd)

#### Step 1: Prepare Files

**Copy executable and library (if build areg-sdk with default setting):**

```bash
# Copy executable
sudo cp ./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/mtrouter /usr/local/bin/mtrouter

# Copy library (if shared build)
sudo cp ./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/libareg.so /usr/lib/libareg.so

# Make executable
sudo chmod +x /usr/local/bin/mtrouter

# Update library cache
sudo ldconfig
```

**Verify:**
```bash
/usr/local/bin/mtrouter --help
```

#### Step 2: Create Service File

**Copy service configuration:**

```bash
sudo cp ./framework/mtrouter/resources/mtrouter.service /etc/systemd/system/
```

**Or create manually:**

```bash
sudo nano /etc/systemd/system/mtrouter.service
```

**Content:**
```ini
[Unit]
Description=AREG Multitarget Router Service
After=network.target

[Service]
Type=simple
ExecStart=/usr/local/bin/mtrouter --service
Restart=on-failure
RestartSec=5s

[Install]
WantedBy=multi-user.target
```

> [!IMPORTANT]
> The `--service` flag is required for service mode.


> [!TIP]
> You may as well install the `areg-sdk` libraries, executables and resources using the provided CMake install target:
> ```bash
> sudo cmake --install ./build --prefix /usr/local
> ```

#### Step 3: Enable and Start Service

**Reload systemd:**
```bash
sudo systemctl daemon-reload
```

**Enable (start at boot):**
```bash
sudo systemctl enable mtrouter.service
```

**Expected output:**
```
Created symlink /etc/systemd/system/multi-user.target.wants/mtrouter.service → /etc/systemd/system/mtrouter.service
```

**Start service:**
```bash
sudo systemctl start mtrouter.service
```

**Verify status:**
```bash
sudo systemctl status mtrouter.service
```

**Expected output:**
```
● mtrouter.service - AREG Multitarget Router Service
     Loaded: loaded (/etc/systemd/system/mtrouter.service; enabled)
     Active: active (running) since Sun 2026-01-26 10:30:00 UTC; 5s ago
   Main PID: 12345 (mtrouter)
      Tasks: 3
     Memory: 8.5M
     CGroup: /system.slice/mtrouter.service
             └─12345 /usr/local/bin/mtrouter --service
```

#### Step 4: Manage Service

**Stop service:**
```bash
sudo systemctl stop mtrouter.service
```

**Restart service:**
```bash
sudo systemctl restart mtrouter.service
```

**Disable (don't start at boot):**
```bash
sudo systemctl disable mtrouter.service
```

**View logs:**
```bash
sudo journalctl -u mtrouter.service -f
```

**Expected log output:**
```
Jan 26 10:30:00 hostname mtrouter[12345]: Router service started
Jan 26 10:30:01 hostname mtrouter[12345]: Listening on 127.0.0.1:8181
```

<div align="right"><kbd><a href="#running-as-system-service">↑ Back to service setup ↑</a></kbd></div>

---

### Windows Service

#### Step 1: Prepare Binaries

**Copy executable and DLL:**

```powershell
# Create installation directory
New-Item -Path "C:\Program Files\areg\mtrouter" -ItemType Directory -Force

# Copy binaries
Copy-Item .\build\bin\Release\mtrouter.exe "C:\Program Files\areg\mtrouter\"
Copy-Item .\build\bin\Release\areg.dll "C:\Program Files\areg\mtrouter\"
```

**Verify:**
```powershell
& "C:\Program Files\areg\mtrouter\mtrouter.exe" --help
```

#### Step 2: Install Service

**Open PowerShell as Administrator:**

Right-click PowerShell → **Run as Administrator**

**Install service:**

```powershell
cd "C:\Program Files\areg\mtrouter"
.\mtrouter.exe --install
```

**Expected output:**
```
Service 'Areg Multitarget Router' installed successfully
```

**Or use batch file:**

```powershell
# Copy batch file
Copy-Item .\framework\mtrouter\resources\mtrouter.service.install.bat "C:\Program Files\areg\mtrouter\"

# Run as Administrator
.\mtrouter.service.install.bat
```

#### Step 3: Start Service

**Using Services application:**

1. Press `Win + R`, type `services.msc`, press Enter
2. Find **AREG Multitarget Router**
3. Right-click → **Start**

**Using PowerShell:**

```powershell
Start-Service "AREG Multitarget Router"
```

**Verify status:**

```powershell
Get-Service "AREG Multitarget Router"
```

**Expected output:**
```
Status   Name               DisplayName
------   ----               -----------
Running  AREGRouter         AREG Multitarget Router
```

#### Step 4: Configure Startup Type

**Automatic startup:**

```powershell
Set-Service "AREG Multitarget Router" -StartupType Automatic
```

**Manual startup:**

```powershell
Set-Service "AREG Multitarget Router" -StartupType Manual
```

#### Step 5: Manage Service

**Stop service:**

```powershell
Stop-Service "AREG Multitarget Router"
```

**Restart service:**

```powershell
Restart-Service "AREG Multitarget Router"
```

**Uninstall service:**

```powershell
# Stop first
Stop-Service "AREG Multitarget Router"

# Uninstall
cd "C:\Program Files\AREG\mtrouter"
.\mtrouter.exe --uninstall
```

**Expected output:**
```
Service 'AREG Multitarget Router' uninstalled successfully
```

**View service logs:**

Open Event Viewer: `Win + R` → `eventvwr.msc`

Navigate: **Windows Logs → Application**

Filter for **Source: AREGRouter** or **mtrouter**

<div align="right"><kbd><a href="#running-as-system-service">↑ Back to service setup ↑</a></kbd></div>

---

## Configuration

### Configuration File Location

**Default:** `areg.init` in executable directory or Areg SDK resources

**Custom location:**
```bash
mtrouter --load="./path/to/config.init"
```

---

### Configuration Format

Settings use key-value pairs:

**Syntax:**
```
section::(module|*)::property[::(position|*)] = value
```

- **section** - Configuration section (e.g., `router`, `log`)
- **module** - Module name or `*` for all modules
- **property** - Property name
- **value** - Property value

---

### Router Configuration Section

**Complete example:**

```ini
# Router service configuration
router::*::service          = mtrouter      # Service name
router::*::connect          = tcpip         # Protocol (tcpip only)
router::*::enable::tcpip    = true          # Enable TCP/IP
router::*::address::tcpip   = 127.0.0.1     # Listen address
router::*::port::tcpip      = 8181          # Listen port
```

**Configuration breakdown:**

| Property | Default | Description |
|----------|---------|-------------|
| `router::*::service` | `mtrouter` | Router process name |
| `router::*::connect` | `tcpip` | Communication protocol |
| `router::*::enable::tcpip` | `true` | Enable/disable TCP/IP |
| `router::*::address::tcpip` | `127.0.0.1` | IP address to bind |
| `router::*::port::tcpip` | `8181` | Port number |

---

### Common Configurations

**1. Local development (default):**

```ini
router::*::address::tcpip   = 127.0.0.1
router::*::port::tcpip      = 8181
```
**Use case:** Single machine development

---

**2. Network access (all interfaces):**

```ini
router::*::address::tcpip   = 0.0.0.0
router::*::port::tcpip      = 8181
```
**Use case:** Accept connections from any network interface

---

**3. Specific network interface:**

```ini
router::*::address::tcpip   = 192.168.1.100
router::*::port::tcpip      = 8181
```
**Use case:** Bind to specific network interface

---

**4. Custom port:**

```ini
router::*::address::tcpip   = 127.0.0.1
router::*::port::tcpip      = 9090
```
**Use case:** Avoid port conflicts

---

**5. Application-specific routing:**

```ini
# Default for all applications
router::*::address::tcpip   = 127.0.0.1
router::*::port::tcpip      = 8181

# Specific configuration for 'myapp'
router::myapp::address::tcpip = 192.168.1.50
router::myapp::port::tcpip    = 9090
```
**Use case:** Different applications connect to different routers

---

### Disable Logging

**Add to configuration:**

```ini
log::mtrouter::scope::*     = NOTSET
```

**Or disable all logs:**

```ini
log::*::scope::*            = NOTSET
```

> [!TIP]
> For detailed configuration syntax, see [Persistence Syntax Guide](./06a-persistence-syntax.md).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Command Reference

### Command-Line Options

Options used when starting mtrouter:

| Option | Alias | Platform | Description |
|--------|-------|----------|-------------|
| `--console` | `-c` | All | Run as console application |
| `--help` | `-h` | All | Display help message |
| `--install` | `-i` | Windows | Install as Windows service |
| `--load` | `-l` | All | Load custom configuration file |
| `--service` | `-s` | Linux | Run as Linux service |
| `--silent` | `-t` | All | Run without console output |
| `--uninstall` | `-u` | Windows | Uninstall Windows service |
| `--verbose` | `-v` | All | Show detailed output |

**Examples:**

```bash
# Console with default config
mtrouter --console

# Console with custom config
mtrouter -c -l="./my-config.init"

# Silent console mode
mtrouter -c -t

# Verbose console mode
mtrouter -c -v

# Linux service
mtrouter --service

# Windows service install
mtrouter.exe --install
```

---

### Interactive Console Commands

Commands available while mtrouter runs in console mode:

| Key | Command | Description |
|-----|---------|-------------|
| `h` | Help | Display command list |
| `n` | Instances | Show connected clients |
| `p` | Pause | Pause message routing |
| `r` | Restart | Restart all connections |
| `q` | Quit | Stop mtrouter |
| `t` | Silent | Toggle silent mode (no output) |
| `v` | Verbose | Toggle verbose mode (detailed output) |

**Usage:** Press the key while mtrouter is running in console mode.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Application Integration

### Connecting to mtrouter

Applications connect to mtrouter by initializing the Application class with router client enabled.

---

### Basic Connection

**Minimal setup:**

```cpp
#include "areg/appbase/Application.hpp"

int main()
{
    // Initialize with router client enabled
    Application::initApplication(
        true,   // Enable logging
        true,   // Enable service manager
        true,   // Enable router client ← Important
        true,   // Enable timer manager
        true    // Enable watchdog manager
    );
    
    // Load service model
    Application::loadModel("MyModel");
    
    // Wait for application quit signal
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    
    // Cleanup
    Application::releaseApplication();
    return 0;
}
```

> [!IMPORTANT]
> The third parameter (`true`) enables router client connection.

---

### Connection with Custom Configuration

**Specify configuration file:**

```cpp
#include "areg/appbase/Application.hpp"

int main()
{
    // Initialize with custom configuration
    Application::initApplication(
        true,   // Enable logging
        true,   // Enable service manager
        true,   // Enable router client
        true,   // Enable timer manager
        true,   // Enable watchdog manager
        "./config/myapp.init",  // Configuration file
        nullptr // Logging configuration (null = use config file)
    );
    
    Application::loadModel("MyModel");
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    Application::releaseApplication();
    return 0;
}
```

---

### Application Configuration

**Create `myapp.init` configuration:**

```ini
# Application router connection settings
router::myapp::service          = mtrouter
router::myapp::connect          = tcpip
router::myapp::enable::tcpip    = true
router::myapp::address::tcpip   = 127.0.0.1    # mtrouter address
router::myapp::port::tcpip      = 8181         # mtrouter port
```

**Or use wildcard for all applications:**

```ini
# Global router settings (all applications)
router::*::service          = mtrouter
router::*::connect          = tcpip
router::*::enable::tcpip    = true
router::*::address::tcpip   = 127.0.0.1
router::*::port::tcpip      = 8181
```

---

### Connection Verification

**Check connection in application code:**

```cpp
#include "areg/appbase/Application.hpp"
#include "areg/base/NEUtilities.hpp"

int main()
{
    Application::initApplication(true, true, true, true, true);
    
    // Wait for router connection
    NEUtilities::waitTimeout(1000); // Wait 1 second
    
    // Check if connected
    if (Application::isRouterConnected())
    {
        std::cout << "Connected to mtrouter" << std::endl;
    }
    else
    {
        std::cout << "Not connected to mtrouter" << std::endl;
    }
    
    Application::loadModel("MyModel");
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    Application::releaseApplication();
    return 0;
}
```

---

### Network Example

**Distributed system setup:**

**Machine 1 (192.168.1.100) - mtrouter:**
```bash
# config/router.init
router::*::address::tcpip = 192.168.1.100
router::*::port::tcpip    = 8181

# Start router
./mtrouter -c -l="config/router.init"
```

**Machine 2 (192.168.1.101) - Provider:**
```ini
# provider.init
router::provider::address::tcpip = 192.168.1.100
router::provider::port::tcpip    = 8181
```

**Machine 3 (192.168.1.102) - Consumer:**
```ini
# consumer.init
router::consumer::address::tcpip = 192.168.1.100
router::consumer::port::tcpip    = 8181
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### mtrouter Won't Start

**Problem:** mtrouter exits immediately or fails to start.

**Solution:**

**1. Check if port is already in use:**

**Linux:**
```bash
sudo netstat -tlnp | grep 8181
```

**Windows:**
```powershell
netstat -ano | findstr 8181
```

**If port is in use:** Kill the process or change port in configuration

**2. Check configuration file:**

```bash
# Verify file exists
ls -l areg.init

# Check syntax
cat areg.init | grep router
```

**3. Check permissions:**

```bash
# Make executable
chmod +x mtrouter

# Check file permissions
ls -l mtrouter
```

---

### Port Already in Use

**Problem:** "Error: Address already in use"

**Solution:**

**Find process using port:**

**Linux:**
```bash
sudo lsof -i :8181
```

**Windows:**
```powershell
netstat -ano | findstr :8181
```

**Kill process:**

**Linux:**
```bash
sudo kill -9 <PID>
```

**Windows:**
```powershell
taskkill /PID <PID> /F
```

**Or change port:**

Edit `areg.init`:
```ini
router::*::port::tcpip = 9090  # Use different port
```

---

### Application Cannot Connect

**Problem:** Application fails to connect to mtrouter.

**Solution:**

**1. Verify mtrouter is running:**

```bash
# Check process
ps aux | grep mtrouter  # Linux
tasklist | findstr mtrouter  # Windows
```

**2. Check mtrouter configuration:**

```bash
./mtrouter -c -v  # Verbose mode shows connection info
```

**Expected output:**
```
IP address: 127.0.0.1
Port: 8181
Router started
```

**3. Verify application configuration:**

Check `myapp.init`:
```ini
router::*::address::tcpip = 127.0.0.1  # Must match mtrouter
router::*::port::tcpip    = 8181       # Must match mtrouter
```

**4. Test network connectivity:**

**Same machine:**
```bash
telnet 127.0.0.1 8181
```

**Different machine:**
```bash
telnet 192.168.1.100 8181
```

**If telnet fails:** Network/firewall issue

---

### Firewall Blocking Connection

**Problem:** mtrouter running but remote applications can't connect.

**Solution:**

**Linux (ufw):**

```bash
# Allow port 8181
sudo ufw allow 8181/tcp

# Check status
sudo ufw status
```

**Linux (firewalld):**

```bash
# Allow port
sudo firewall-cmd --permanent --add-port=8181/tcp
sudo firewall-cmd --reload
```

**Windows Firewall:**

```powershell
# Add rule
New-NetFirewallRule -DisplayName "AREG mtrouter" -Direction Inbound -Protocol TCP -LocalPort 8181 -Action Allow
```

---

### Service Won't Start (Linux)

**Problem:** `systemctl start` fails

**Solution:**

**Check service status:**

```bash
sudo systemctl status mtrouter.service
```

**Check logs:**

```bash
sudo journalctl -u mtrouter.service -n 50
```

**Common issues:**

**1. Executable not found:**
```bash
# Verify path in service file
grep ExecStart /etc/systemd/system/mtrouter.service

# Verify executable exists
ls -l /usr/local/bin/mtrouter
```

**2. Missing library:**
```bash
# Check dependencies
ldd /usr/local/bin/mtrouter

# Copy missing library
sudo cp ./build/lib/libareg.so /usr/lib/
sudo ldconfig
```

**3. Permissions:**
```bash
# Make executable
sudo chmod +x /usr/local/bin/mtrouter
```

---

### Service Won't Start (Windows)

**Problem:** Service fails to start in Services app

**Solution:**

**Check Event Viewer:**

1. `Win + R` → `eventvwr.msc`
2. **Windows Logs → Application**
3. Look for mtrouter or AREGRouter errors

**Common issues:**

**1. Missing DLL:**
- Ensure `areg.dll` is in same directory as `mtrouter.exe`

**2. Permission issues:**
- Install must be run as Administrator

**3. Service already exists:**
```powershell
# Uninstall first
.\mtrouter.exe --uninstall

# Then reinstall
.\mtrouter.exe --install
```

---

### No Connected Instances Shown

**Problem:** Press 'n' but shows "Connected instances: 0"

**Cause:** No applications connected yet.

**Solution:**

**1. Start an application:**

```bash
# From multiprocessing project
./build/myapp_provider
```

**2. Check connection:**

Press 'n' in mtrouter console:
```
Connected instances: 1
  1. myapp_provider (127.0.0.1:12345)
```

**3. If still shows 0:**

- Check application configuration
- Verify `Application::initApplication()` has router enabled (3rd parameter = true)
- Check application logs for connection errors

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Related Guides:**
- [Quick Project Setup](./02a-quick-project-setup.md) - Create multiprocessing projects
- [CMake Integration](./02b-cmake-integrate.md) - Integration methods
- [Persistence Syntax](./06a-persistence-syntax.md) - Configuration file format

**Configuration Files:**
- [areg.init](../../framework/areg/resources/areg.init) - Default configuration
- [mtrouter.service](../../framework/mtrouter/resources/mtrouter.service) - Linux service file

**Source Code:**
- [mtrouter Module](../../framework/mtrouter/) - Router implementation

**Help:**
For questions, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech
