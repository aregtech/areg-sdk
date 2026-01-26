# Areg SDK Multitarget Router (mtrouter)

The Multitarget Router enables inter-process and networked communication between Service Provider and Consumer nodes in distributed Areg SDK applications.

> [!TIP]
> For single-process multithreading applications, `mtrouter` is not required. See [Quick Project Setup](./02a-quick-project-setup.md) for creating projects.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Overview](#overview)
3. [Installation](#installation)
4. [Running as Console Application](#running-as-console-application)
5. [Running as System Service](#running-as-system-service)
6. [Configuration](#configuration)
7. [Command Reference](#command-reference)
8. [Application Integration](#application-integration)
9. [Troubleshooting](#troubleshooting)

---

## Quick Start

### Run mtrouter in 10 Seconds

**Linux/macOS:**
```bash
cd areg-sdk
./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/mtrouter.elf --console
```

**Windows:**
```powershell
cd areg-sdk
.\product\build\msvc-v143\windows_nt-x64-release\bin\mtrouter.exe --console
```

**Expected output:**
```
Areg Multi-target Message Router console application ...
---------------------------------------------------------------------------------------------
Send data with the rate:    0.00  Bytes / sec.
Recv data with the rate:    0.00  Bytes / sec.

Type '-q' or '--quit' to quit the application ...:
```

**Test connection:** Run executable of any IPC example, for example `15_pubclient.exe`, and type `-n` in `mtrouter` console to see connected clients like this: 
```
Areg Multi-target Message Router console application ...
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

**Type `-q` to quit**

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Overview

### What is mtrouter?

The **Multitarget Message Router** (mtrouter) is a message routing service that:

- ✅ Routes messages between distributed processes
- ✅ Enables inter-process communication (IPC)
- ✅ Supports network communication (TCP/IP)
- ✅ Manages service discovery
- ✅ Handles multiple client connections

### When Do You Need mtrouter?

| Application Type | Requires mtrouter | Communication |
|------------------|-------------------|---------------|
| **Multithreading** (single process) | ❌ No | In-process |
| **Multiprocessing** (same machine) | ✅ Yes | Inter-process (IPC) |
| **Distributed** (network) | ✅ Yes | Network (TCP/IP) |

> [!IMPORTANT]
> Without mtrouter, multiprocessing applications cannot communicate with each other. They will run as standalone multithreading applications where only internal (local) services work within each process, but external (public) services remain inaccessible to other processes.

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
- Stop with `-q` or `--quit`

**System Service:**
- Automatic startup at boot
- Runs in background
- Managed by OS (systemd/Windows Services)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Installation

### Option 1: Use from Build Directory

After building Areg SDK, `mtrouter` is available in the build output directory.

**Default build paths:**

**Linux:**
```bash
./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/mtrouter.elf
```

**Windows:**
```powershell
.\product\build\msvc-v143\windows_nt-x64-release\bin\mtrouter.exe
```

> [!NOTE]
> Build output paths follow pattern: `product/build/<compiler>/<platform>-<arch>-<config>/bin/`

---

### Option 2: Install via CMake

**Install Areg SDK system-wide:**

```bash
# Configure and build
cmake -B ./build
cmake --build ./build

# Install (requires sudo on Linux)
sudo cmake --install ./build
```

**Default installation locations:**

**Linux:**
- Executables: `/usr/local/tools/areg/mtrouter`
- Libraries: `/usr/local/lib/libareg.so`
- Headers: `/usr/local/include/areg/`
- Config: `/usr/local/share/areg/areg.init`
- Service files: `/usr/local/share/areg/service/`

**Windows:**
- Executables: `C:\Program Files\areg\tools\areg\mtrouter.exe`
- Libraries: `C:\Program Files\areg\bin\areg.dll`
- Headers: `C:\Program Files\areg\include\areg\`
- Config: `C:\Program Files\areg\share\areg\areg.init`
- Service files: `C:\Program Files\areg\share\areg\service\`

**Custom installation prefix:**

```bash
cmake --install ./build --prefix /opt/areg
```

**What gets installed:**

**Libraries:**
- `areg` - Core framework library
- `aregextend` - Extended utilities library
- `areglogger` - Log observer API library

**Executables:**
- `mtrouter` - Message router
- `logcollector` - Log collector service
- `logobserver` - Log observer application

**Tools:**
- `codegen.jar` - Service interface code generator
- `project-setup.sh` / `project-setup.bat` - Project creation scripts

**Configuration:**
- `areg.init` - Default configuration file
- Service installation scripts (Linux/Windows)

**Development files:**
- Headers (`.h`, `.hpp`)
- CMake configuration files
- pkg-config files (`.pc`)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Running as Console Application

### Basic Usage

**From build directory:**

**Linux/macOS:**
```bash
./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/mtrouter.elf --console
```

**Windows:**
```powershell
.\product\build\msvc-v143\windows_nt-x64-release\bin\mtrouter.exe --console
```

**From installed location:**

**Linux:**
```bash
/usr/local/tools/areg/mtrouter --console
```

**Windows:**
```powershell
& "C:\Program Files\areg\tools\areg\mtrouter.exe" --console
```

**Expected output:**
```
Areg Multi-target Message Router console application ...
---------------------------------------------------------------------------------------------
Send data with the rate:    0.00  Bytes / sec.
Recv data with the rate:    0.00  Bytes / sec.

Type '-q' or '--quit' to quit the application ...:
```

---

### Console Commands

While mtrouter is running, type commands starting with `-` (short) or `--` (long):

| Command | Alias | Description |
|---------|-------|-------------|
| `--help` | `-h` | Display command list |
| `--instances` | `-n` | Show connected clients |
| `--pause` | `-p` | Pause message routing |
| `--restart` | `-r` | Restart all connections |
| `--quit` | `-q` | Stop mtrouter |
| `--silent` | `-t` | Toggle silent mode (hide data rates) |
| `--verbose` | `-v` | Toggle verbose mode (show data rates) |

**Example - Check connected clients:**

Type `--instances` or `-n`:
```
Areg Multi-target Message Router console application ...
---------------------------------------------------------------------------------------------
Send data with the rate:    0.00  Bytes / sec.
Recv data with the rate:    0.00  Bytes / sec.

Type '-q' or '--quit' to quit the application ...:

---------------------------------------------------------------------------------------------
   Nr. |  Instance ID  |  Bitness  |  Name
---------------------------------------------------------------------------------------------
    1. |          256  |    64     |  provider.exe
    2. |          257  |    64     |  consumer.exe
---------------------------------------------------------------------------------------------
```

---

### Verbose Mode

**Toggle verbose mode:**

Type `--verbose` or `-v`:

**When verbose ON (default):**
```
Send data with the rate:    1.23  Bytes / sec.
Recv data with the rate:    0.87  Bytes / sec.
```

**When verbose OFF (silent):**
```
Type '-q' or '--quit' to quit the application ...:
```

> [!NOTE]
> Verbose mode shows data transfer rates, not individual messages.

---

### Custom Configuration

**Load custom configuration file:**

```bash
./mtrouter.elf --console --load="./config/custom.init"
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Running as System Service

### Linux Service (systemd)

#### Step 1: Prepare Files

**Option A: Install via CMake (recommended)**

```bash
# Install Areg SDK system-wide
sudo cmake --install ./build

# Service file location
ls -l /usr/local/share/areg/service/mtrouter.service
```

**Option B: Manual installation**

```bash
# Copy executable
sudo cp ./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/mtrouter.elf /usr/local/bin/mtrouter

# Copy library (if shared build)
sudo cp ./product/build/gnu-g++/linux-64-x86_64-release-shared/lib/libareg.so /usr/lib/libareg.so

# Make executable
sudo chmod +x /usr/local/bin/mtrouter

# Update library cache
sudo ldconfig
```

**Verify:**
```bash
/usr/local/bin/mtrouter --help
```

#### Step 2: Install Service File

**If installed via CMake:**

```bash
sudo cp /usr/local/share/areg/service/mtrouter.service /etc/systemd/system/
```

**If manual installation:**

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
Description=Areg Multitarget Router Service
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
● mtrouter.service - Areg Multitarget Router Service
     Loaded: loaded (/etc/systemd/system/mtrouter.service; enabled)
     Active: active (running) since Mon 2026-01-27 10:30:00 UTC; 5s ago
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
Jan 27 10:30:00 hostname mtrouter[12345]: Router service started
Jan 27 10:30:01 hostname mtrouter[12345]: Listening on 127.0.0.1:8181
```

<div align="right"><kbd><a href="#running-as-system-service">↑ Back to service setup ↑</a></kbd></div>

---

### Windows Service

#### Step 1: Prepare Binaries

**Option A: Install via CMake (recommended)**

```powershell
# Open PowerShell as Administrator
# Install Areg SDK
cmake --install .\build --prefix "C:\Program Files\areg"

# Service installation script location
dir "C:\Program Files\areg\share\areg\service\mtrouter.service.install.bat"
```

**Option B: Manual installation**

```powershell
# Create installation directory
New-Item -Path "C:\Program Files\areg\tools\areg" -ItemType Directory -Force

# Copy binaries
Copy-Item .\product\build\msvc-v143\windows_nt-x64-release\bin\mtrouter.exe "C:\Program Files\areg\tools\areg\"
Copy-Item .\product\build\msvc-v143\windows_nt-x64-release\bin\areg.dll "C:\Program Files\areg\tools\areg\"
```

**Verify:**
```powershell
& "C:\Program Files\areg\tools\areg\mtrouter.exe" --help
```

#### Step 2: Install Service

**Open PowerShell as Administrator:**

Right-click PowerShell → **Run as Administrator**

**Option A: Using installation script (if CMake installed)**

```powershell
cd "C:\Program Files\areg\share\areg\service"
.\mtrouter.service.install.bat
```

**Option B: Using mtrouter executable**

```powershell
cd "C:\Program Files\areg\tools\areg"
.\mtrouter.exe --install
```

**Expected output:**
```
Service 'Areg Multitarget Router Service' installed successfully
```

#### Step 3: Start Service

**Using Services GUI:**

1. Press `Win + R`, type `services.msc`, press Enter
2. Find **Areg Multitarget Router Service**
3. Right-click → **Start**

**Using PowerShell:**

```powershell
Start-Service "mtrouter.service"
```

**Verify status:**

```powershell
Get-Service "mtrouter.service"
```

**Expected output:**
```
Status   Name               DisplayName
------   ----               -----------
Running  mtrouter.service   Areg Multitarget Router Service
```

#### Step 4: Configure Startup Type

**Automatic startup:**

```powershell
Set-Service "mtrouter.service" -StartupType Automatic
```

**Manual startup:**

```powershell
Set-Service "mtrouter.service" -StartupType Manual
```

#### Step 5: Manage Service

**Stop service:**

```powershell
Stop-Service "mtrouter.service"
```

**Restart service:**

```powershell
Restart-Service "mtrouter.service"
```

**Uninstall service:**

**Option A: Using uninstall script**

```powershell
cd "C:\Program Files\areg\share\areg\service"
.\mtrouter.service.uninstall.bat
```

**Option B: Using mtrouter executable**

```powershell
# Stop first
Stop-Service "mtrouter.service"

# Uninstall
cd "C:\Program Files\areg\tools\areg"
.\mtrouter.exe --uninstall
```

**Expected output:**
```
Service 'Areg Multitarget Router Service' uninstalled successfully
```

**View service logs:**

Open Event Viewer: `Win + R` → `eventvwr.msc`

Navigate: **Windows Logs → Application**

Filter for **Source: mtrouter**

<div align="right"><kbd><a href="#running-as-system-service">↑ Back to service setup ↑</a></kbd></div>

---

## Configuration

### Configuration File Location

**Default:** `areg.init` in Areg SDK resources directory

**Installed location:**
- Linux: `/usr/local/share/areg/areg.init`
- Windows: `C:\Program Files\areg\share\areg\areg.init`

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
| `--silent` | `-t` | All | Run without showing data rates |
| `--uninstall` | `-u` | Windows | Uninstall Windows service |
| `--verbose` | `-v` | All | Show data transfer rates |

**Examples:**

```bash
# Console with default config
mtrouter --console

# Console with custom config
mtrouter -c -l="./my-config.init"

# Silent console mode (no data rates)
mtrouter -c -t

# Verbose console mode (show data rates)
mtrouter -c -v

# Linux service
mtrouter --service

# Windows service install
mtrouter.exe --install
```

---

### Interactive Console Commands

Commands available while mtrouter runs in console mode (must start with `-` or `--`):

| Command | Alias | Description |
|---------|-------|-------------|
| `--help` | `-h` | Display command list |
| `--instances` | `-n` | Show connected clients |
| `--pause` | `-p` | Pause message routing |
| `--restart` | `-r` | Restart all connections |
| `--quit` | `-q` | Stop mtrouter |
| `--silent` | `-t` | Toggle silent mode (hide data rates) |
| `--verbose` | `-v` | Toggle verbose mode (show data rates) |

**Usage:** Type the command (e.g., `-n` or `--instances`) and press Enter.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Application Integration

### Connecting to mtrouter

Applications connect to `mtrouter` by initializing the Application class with router client enabled.

> [!IMPORTANT]
> For inter-process communication, `mtrouter` must be running. Start `mtrouter` before or after starting provider and consumer processes. There is no required startup order - applications will automatically connect when `mtrouter` becomes available. For network communication, configure `mtrouter` address in application configuration file (`areg.init`). By default, applications communicate via `localhost`.

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

> [!NOTE]
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
./mtrouter --console --load="config/router.init"
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
chmod +x mtrouter.elf

# Check file permissions
ls -l mtrouter.elf
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

**2. Check mtrouter console for connections:**

Type `--instances` or `-n` in mtrouter console to see connected applications.

**3. Verify application configuration:**

Check `myapp.init`:
```ini
router::*::address::tcpip = 127.0.0.1  # Must match mtrouter
router::*::port::tcpip    = 8181       # Must match mtrouter
```

**4. Verify router client is enabled:**

Check application code:
```cpp
Application::initApplication(
    true,   // logging
    true,   // service manager
    true,   // router client ← Must be true
    true,   // timer
    true    // watchdog
);
```

**5. Test network connectivity:**

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
New-NetFirewallRule -DisplayName "Areg mtrouter" -Direction Inbound -Protocol TCP -LocalPort 8181 -Action Allow
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
sudo cp ./product/build/gnu-g++/linux-64-x86_64-release-shared/lib/libareg.so /usr/lib/
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
3. Look for mtrouter errors

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

**Problem:** Type `-n` but shows no instances

**Cause:** No applications connected yet.

**Solution:**

**1. Start an application:**

```bash
# From multiprocessing project
./build/myapp_provider
```

**2. Check connection in mtrouter:**

Type `-n` or `--instances` in mtrouter console.

**3. If still shows 0 instances:**

- Check application configuration (mtrouter address/port)
- Verify `Application::initApplication()` has router enabled (3rd parameter = `true`)
- Check application logs for connection errors
- Verify mtrouter is listening on correct address/port

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

**Installation:**
- [install.cmake](../../conf/cmake/install.cmake) - CMake installation script

**Source Code:**
- [mtrouter Module](../../framework/mtrouter/) - Router implementation

**Help:**
For questions, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech
