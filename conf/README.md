# Configuration Overview

This document provides a brief overview of the configuration directory's contents and instructions for setting up configurations for various toolsets.

## Directory Structure

```
conf/
├── cmake/          # CMake build configuration files
├── exports/        # Installation templates and examples
├── msvc/           # Microsoft Visual Studio property files
└── README.md       # This file
```

---

## CMake

The [./cmake](./cmake/) directory contains project-wide and developer-specific settings for building with CMake.

| File | Purpose |
|------|---------|
| `setup.cmake` | Main setup and initialization |
| `user.cmake` | User-configurable build options |
| `common.cmake` | Common settings across all platforms |
| `functions.cmake` | CMake utility functions and macros |
| `gnu.cmake` | GCC compiler settings |
| `clang.cmake` | Clang/Apple Clang compiler settings |
| `msvc.cmake` | Microsoft Visual C++ settings |
| `install.cmake` | Installation and packaging rules |

Direct modifications to files within this directory are generally unnecessary. Instead, use the options in [user.cmake](./cmake/user.cmake) to customize the build configuration.

---

## Microsoft Visual Studio

The [./msvc](./msvc/) directory includes Visual Studio-specific property files referenced by the project. Directly modifying `.props` files is not recommended. To customize your build, copy the [msvc_setup.props](./../msvc_setup.props) file into your solution directory and apply necessary changes there.

---

## Exports

The [./exports](./exports/) directory contains templates and examples required for packaging, installation, service management, and project integration.

### Directory Contents

| File/Directory | Purpose |
|----------------|---------|
| `*.pc.in` | pkg-config templates for library discovery |
| `config.cmake.in` | CMake package configuration template |
| `*.service.in` | Linux systemd service templates |
| `*.service.*.bat.in` | Windows service install/uninstall scripts |
| `tech.areg.*.plist.in` | macOS launchd service templates |
| `*.service.*.sh.in` | macOS service install/uninstall scripts |
| `example/` | Example project and CMake toolchains |

### Package Configuration Files

| Template | Generated File | Purpose |
|----------|----------------|---------|
| `areg.pc.in` | `areg.pc` | pkg-config for areg library |
| `aregextend.pc.in` | `aregextend.pc` | pkg-config for aregextend library |
| `areglogger.pc.in` | `areglogger.pc` | pkg-config for areglogger library |
| `config.cmake.in` | `areg-config.cmake` | CMake find_package support |

### Service Files by Platform

**Linux (systemd):**
| Template | Purpose |
|----------|---------|
| `mtrouter.service.in` | mtrouter systemd service unit |
| `logcollector.service.in` | logcollector systemd service unit |

**macOS (launchd):**
| Template | Purpose |
|----------|---------|
| `tech.areg.mtrouter.plist.in` | mtrouter launchd plist |
| `tech.areg.logcollector.plist.in` | logcollector launchd plist |
| `mtrouter.service.install.sh.in` | mtrouter install script |
| `mtrouter.service.uninstall.sh.in` | mtrouter uninstall script |
| `logcollector.service.install.sh.in` | logcollector install script |
| `logcollector.service.uninstall.sh.in` | logcollector uninstall script |

**Windows (Services):**
| Template | Purpose |
|----------|---------|
| `mtrouter.service.install.bat.in` | mtrouter install batch script |
| `mtrouter.service.uninstall.bat.in` | mtrouter uninstall batch script |
| `logcollector.service.install.bat.in` | logcollector install batch script |
| `logcollector.service.uninstall.bat.in` | logcollector uninstall batch script |

### Example Project and Toolchains

The `example/` subdirectory contains:
- `CMakeLists.txt` - Example CMake project using Areg SDK
- `example.cpp` - Sample application source code
- `ReadMe.md` - Example project documentation
- `toolchains/` - CMake toolchain files for cross-compilation

**Available Toolchains:**

| Toolchain File | Target Platform |
|----------------|-----------------|
| `gnu-linux-x64.cmake` | Linux x86-64 with GCC |
| `gnu-linux-x86.cmake` | Linux x86 (32-bit) with GCC |
| `gnu-linux-arm32.cmake` | Linux ARM 32-bit with GCC |
| `gnu-linux-arm64.cmake` | Linux ARM 64-bit (AARCH64) with GCC |
| `clang-linux-x64.cmake` | Linux x86-64 with Clang |
| `clang-linux-x86.cmake` | Linux x86 (32-bit) with Clang |
| `clang-linux-arm32.cmake` | Linux ARM 32-bit with Clang |
| `clang-linux-arm64.cmake` | Linux ARM 64-bit with Clang |
| `clang-macos-x64.cmake` | macOS x86-64 (Intel) with Apple Clang |
| `clang-macos-arm64.cmake` | macOS ARM64 (Apple Silicon) with Apple Clang |
| `clang-win-x64.cmake` | Windows x86-64 with Clang |
| `clang-win-x86.cmake` | Windows x86 (32-bit) with Clang |
| `msvc-win-x64.cmake` | Windows x86-64 with MSVC |
| `msvc-win-x86.cmake` | Windows x86 (32-bit) with MSVC |

---

## System Services Management

After installing Areg SDK on the target system, you can run `mtrouter` and `logcollector` as system services.

### Installation

```bash
# Linux/macOS
sudo cmake --install ./build

# Windows (as Administrator)
cmake --install ./build
```

Service files are installed to:
- **Linux:** `/usr/local/share/areg/service/`
- **macOS:** `/usr/local/share/areg/service/`
- **Windows:** `C:\Program Files\areg\share\areg\service\`

---

### Linux (systemd)

**Install and Start:**
```bash
# Copy service file
sudo cp /usr/local/share/areg/service/mtrouter.service /etc/systemd/system/

# Reload systemd
sudo systemctl daemon-reload

# Enable (start at boot)
sudo systemctl enable mtrouter.service

# Start service
sudo systemctl start mtrouter.service
```

**Stop and Uninstall:**
```bash
# Stop service
sudo systemctl stop mtrouter.service

# Disable service
sudo systemctl disable mtrouter.service

# Remove service file
sudo rm /etc/systemd/system/mtrouter.service
sudo systemctl daemon-reload
```

**Check Status:**
```bash
sudo systemctl status mtrouter.service
sudo journalctl -u mtrouter.service -f
```

---

### macOS (launchd)

**Install and Start:**
```bash
# Use provided install script
sudo /usr/local/share/areg/service/mtrouter.service.install.sh

# Or manually:
sudo mkdir -p /var/log/areg
sudo cp /usr/local/share/areg/service/tech.areg.mtrouter.plist /Library/LaunchDaemons/
sudo chown root:wheel /Library/LaunchDaemons/tech.areg.mtrouter.plist
sudo chmod 644 /Library/LaunchDaemons/tech.areg.mtrouter.plist
sudo launchctl load -w /Library/LaunchDaemons/tech.areg.mtrouter.plist
```

**Stop and Uninstall:**
```bash
# Use provided uninstall script
sudo /usr/local/share/areg/service/mtrouter.service.uninstall.sh

# Or manually:
sudo launchctl unload -w /Library/LaunchDaemons/tech.areg.mtrouter.plist
sudo rm /Library/LaunchDaemons/tech.areg.mtrouter.plist
```

**Check Status:**
```bash
sudo launchctl list | grep tech.areg
tail -f /var/log/areg/mtrouter.log
```

---

### Windows (Services)

**Install and Start:**
```powershell
# Run as Administrator
cd "C:\Program Files\areg\share\areg\service"
.\mtrouter.service.install.bat

# Or using executable directly:
cd "C:\Program Files\areg\tools\areg"
.\mtrouter.exe --install

# Start via Services app or:
Start-Service "mtrouter.service"
```

**Stop and Uninstall:**
```powershell
# Stop service
Stop-Service "mtrouter.service"

# Uninstall
cd "C:\Program Files\areg\share\areg\service"
.\mtrouter.service.uninstall.bat

# Or using executable:
.\mtrouter.exe --uninstall
```

**Check Status:**
```powershell
Get-Service "mtrouter.service"
# Or open services.msc GUI
```

---

## Service Summary

| Platform | Service Manager | Config Format | Service Location |
|----------|-----------------|---------------|------------------|
| Linux | systemd | `.service` | `/etc/systemd/system/` |
| macOS | launchd | `.plist` | `/Library/LaunchDaemons/` |
| Windows | Services | Registry | Service Control Manager |

| Service | Linux Name | macOS Label | Windows Name |
|---------|------------|-------------|--------------|
| Message Router | `mtrouter.service` | `tech.areg.mtrouter` | `mtrouter.service` |
| Log Collector | `logcollector.service` | `tech.areg.logcollector` | `logcollector.service` |

---

For further information, consult the [wiki](./../docs/wiki/README.md) documentation.
