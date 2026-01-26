# Areg SDK Logging Configuration

The Areg SDK provides a highly configurable logging system via the `areg.init` configuration file, enabling customization of log destinations, formats, scopes, and priorities for enhanced debugging and runtime analysis.

> [!TIP]
> Start with the Quick Start section to see logging in action, then customize based on your needs.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Configuration File Overview](#configuration-file-overview)
3. [Log Destinations](#log-destinations)
4. [Log Message Layout](#log-message-layout)
5. [Remote Log Collector](#remote-log-collector)
6. [Log Viewing with Lusan](#log-viewing-with-lusan)
7. [Scopes and Priorities](#scopes-and-priorities)
8. [Common Configurations](#common-configurations)
9. [Troubleshooting](#troubleshooting)

---

## Quick Start

### Enable Logging in 30 Seconds

**Step 1: Create configuration file**

Create `areg.init` in your application directory:

```ini
# Basic logging configuration
log::*::target = file
log::*::enable::file = true
log::*::file::location = ./logs/%appname%_%time%.log

# Enable all scopes
log::*::scope::* = DEBUG | SCOPE
```

**Step 2: Initialize logging in application**

```cpp
#include "areg/appbase/Application.hpp"

int main()
{
    // Initialize with logging enabled
    Application::initApplication(
        true,   // Enable logging ← Important
        true,   // Enable service manager
        false,  // Disable router client (for this example)
        true,   // Enable timer manager
        false,  // Disable watchdog
        "./areg.init",  // Configuration file
        nullptr
    );
    
    // Your application code
    Application::loadModel("MyModel");
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    
    Application::releaseApplication();
    return 0;
}
```

**Step 3: Run application**

```bash
./myapp
```

**Expected output in `./logs/myapp_20260127_103045.log`:**

```
2026-01-27 10:30:45.123: [ 12345  myapp.exe: Enter -->]
    Entering scope: main
2026-01-27 10:30:45.124: [ 12345  DEBUG >>> ] Application initialized
2026-01-27 10:30:45.125: [ 12345  INFO >>> ] Loading model: MyModel
2026-01-27 10:30:45.126: [ 12345  myapp.exe: Exit <-- ]
    Exiting scope: main
```

**Setup time:** ~30 seconds

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Configuration File Overview

### File Location

**Default name:** `areg.init`

**Common locations:**
- Application directory: `./areg.init`
- Config subdirectory: `./config/areg.init`
- Installed location: `/usr/local/share/areg/areg.init` (Linux)

**Specify custom location:**

```cpp
Application::initApplication(
    true,   // Enable logging
    true, true, true, false,
    "./my-config/custom.init",  // Custom config file
    nullptr
);
```

---

### Configuration Syntax

Settings use key-value pairs:

**Format:**
```
section::(module|*)::property[::position] = value
```

**Components:**
- **section** - Configuration section (e.g., `log`, `logger`)
- **module** - Application name or `*` for all
- **property** - Setting name
- **position** - Optional sub-property
- **value** - Setting value

**Examples:**

```ini
# Global setting (all applications)
log::*::target = file

# Application-specific setting
log::myapp::target = file | remote

# Property with position
log::*::enable::file = true
```

> [!TIP]
> For complete syntax details, see [Persistence Syntax Guide](./06a-persistence-syntax.md).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Log Destinations

### Available Destinations

| Destination | Value | Description | Use Case |
|-------------|-------|-------------|----------|
| **File** | `file` | Local log files | Development, production |
| **Remote** | `remote` | Centralized log collector | Distributed systems |
| **Debug Output** | `debug` | IDE debug console (MSVC) | Visual Studio debugging |
| **Database** | `db` | Database storage | Long-term analysis |

---

### File Logging

**Configuration:**

```ini
# Enable file logging
log::*::target = file
log::*::enable::file = true

# Specify log file location
log::*::file::location = ./logs/%appname%_%time%.log
```

**File naming masks:**

| Mask | Replaced With | Example |
|------|---------------|---------|
| `%appname%` | Application name | `myapp` |
| `%time%` | Timestamp (YYYYMMDD_HHMMSS) | `20260127_103045` |
| `%pid%` | Process ID | `12345` |

**Examples:**

```ini
# Single log file per application
log::*::file::location = ./logs/%appname%.log
Result: ./logs/myapp.log

# Separate log per run
log::*::file::location = ./logs/%appname%_%time%.log
Result: ./logs/myapp_20260127_103045.log

# Include process ID
log::*::file::location = ./logs/%appname%_pid%pid%.log
Result: ./logs/myapp_pid12345.log
```

**Log file output:**

```
2026-01-27 10:30:45.123: [ 12345  DEBUG >>> ] Application started
2026-01-27 10:30:45.124: [ 12345  INFO >>> ] Connecting to service
2026-01-27 10:30:45.125: [ 12345  WARN >>> ] Connection delayed
2026-01-27 10:30:45.126: [ 12345  ERROR >>> ] Connection failed
```

---

### Remote Logging

**Configuration:**

```ini
# Enable remote logging
log::*::target = remote
log::*::enable::remote = true

# Connection settings
logger::*::connect = tcpip
logger::*::address::tcpip = 127.0.0.1
logger::*::port::tcpip = 8282

# Optional: Queue size for buffering
log::*::remote::queue = 100
```

**Use cases:**
- Centralized log collection
- Distributed systems
- Real-time log monitoring
- Multiple applications logging to one collector

**Prerequisites:**
- `logcollector` service must be running
- Network connectivity to log collector

**See:** [Log Collector Guide](./XX-logcollector.md) for setup

---

### Debug Output (Visual Studio)

**Configuration:**

```ini
# Enable debug output
log::*::target = debug
log::*::enable::debug = true
```

**Use cases:**
- Visual Studio debugging
- Real-time log viewing in IDE
- Development on Windows

**Output location:** Visual Studio Debug Output window

**Expected output in Visual Studio:**

```
[12345] DEBUG: Application started
[12345] INFO: Connecting to service
[12345] WARN: Connection delayed
```

---

### Database Logging

**Configuration:**

```ini
# Enable database logging
log::*::target = db
log::*::enable::db = true

# Database connection (example)
log::*::db::connection = ./logs/application.db
```

**Use cases:**
- Structured log analysis
- SQL queries on logs
- Long-term log retention
- Complex filtering and reporting

---

### Multiple Destinations

**Enable multiple targets:**

```ini
# Log to file AND remote collector
log::*::target = file | remote

# Enable both
log::*::enable::file = true
log::*::enable::remote = true

# File configuration
log::*::file::location = ./logs/%appname%.log

# Remote configuration
logger::*::address::tcpip = 192.168.1.100
logger::*::port::tcpip = 8282
```

**Use cases:**
- Development: File + Debug
- Production: File + Remote
- Analysis: File + Database

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Log Message Layout

### Layout Configuration

Define how log messages appear:

```ini
# Scope entry layout
log::*::layout::enter = %d: [ %t  %x.%z: Enter -->]%n

# Log message layout
log::*::layout::message = %d: [ %t  %p >>> ] %m%n

# Scope exit layout
log::*::layout::exit = %d: [ %t  %x.%z: Exit <-- ]%n
```

---

### Format Specifiers

| Specifier | Description | Example Output |
|-----------|-------------|----------------|
| `%a` | Logging object ID | `0x12345678` |
| `%c` | Tick count since start | `1234567` |
| `%d` | Date and time | `2026-01-27 10:30:45.123` |
| `%e` | Module (process) ID | `12345` |
| `%m` | Log message text | `Application started` |
| `%n` | End-of-line | (newline) |
| `%p` | Message priority | `DEBUG`, `INFO`, `WARN` |
| `%s` | Message scope ID | `0x00001234` |
| `%t` | Thread ID | `54321` |
| `%x` | Module (process) name | `myapp.exe` |
| `%y` | Thread name | `WorkerThread` |
| `%z` | Scope name | `main::initialize` |

> [!NOTE]
> `%m` (message) and `%z` (scope name) are mutually exclusive in the same layout.

---

### Layout Examples

**Example 1: Minimal layout**

```ini
log::*::layout::message = %p: %m%n
```

**Output:**
```
DEBUG: Application started
INFO: Connecting to service
WARN: Connection delayed
```

---

**Example 2: Detailed layout**

```ini
log::*::layout::message = [%d] %x (thread %t) %p: %m%n
```

**Output:**
```
[2026-01-27 10:30:45.123] myapp.exe (thread 54321) DEBUG: Application started
[2026-01-27 10:30:45.124] myapp.exe (thread 54321) INFO: Connecting to service
```

---

**Example 3: Custom layout with thread name**

```ini
log::*::layout::message = %d %y [%p] %m%n
```

**Output:**
```
2026-01-27 10:30:45.123 MainThread [DEBUG] Application started
2026-01-27 10:30:45.124 WorkerThread [INFO] Processing request
```

---

**Example 4: Scope entry/exit**

```ini
log::*::layout::enter = >>> Entering %z (thread %t)%n
log::*::layout::exit = <<< Exiting %z (thread %t)%n
```

**Output:**
```
>>> Entering main::initialize (thread 54321)
    ... other log messages ...
<<< Exiting main::initialize (thread 54321)
```

---

### Complete Layout Example

**Configuration:**

```ini
log::*::layout::enter   = %d [%t] --> %z%n
log::*::layout::message = %d [%t] %p: %m%n
log::*::layout::exit    = %d [%t] <-- %z%n
```

**Output:**

```
2026-01-27 10:30:45.123 [54321] --> main
2026-01-27 10:30:45.124 [54321] DEBUG: Initializing application
2026-01-27 10:30:45.125 [54321] INFO: Configuration loaded
2026-01-27 10:30:45.126 [54321] <-- main
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Remote Log Collector

### Configuration

**Enable remote logging:**

```ini
# Set remote as target
log::*::target = remote
log::*::enable::remote = true

# Configure connection to log collector
logger::*::connect = tcpip
logger::*::address::tcpip = 127.0.0.1
logger::*::port::tcpip = 8282

# Optional: Message queue size
log::*::remote::queue = 100
```

---

### Connection Settings

| Property | Default | Description |
|----------|---------|-------------|
| `logger::*::connect` | `tcpip` | Protocol (TCP/IP only) |
| `logger::*::address::tcpip` | `127.0.0.1` | Log collector IP |
| `logger::*::port::tcpip` | `8282` | Log collector port |
| `log::*::remote::queue` | `100` | Message buffer size |

---

### Network Deployment

**Scenario: Central log collector for distributed system**

**Machine 1 (192.168.1.100) - Log Collector:**

```bash
# Start log collector
/usr/local/tools/areg/logcollector --console
```

**Machine 2 - Application A:**

```ini
# app_a.init
logger::*::address::tcpip = 192.168.1.100
logger::*::port::tcpip = 8282
log::*::target = remote
```

**Machine 3 - Application B:**

```ini
# app_b.init
logger::*::address::tcpip = 192.168.1.100
logger::*::port::tcpip = 8282
log::*::target = remote
```

**Result:** Both applications send logs to centralized collector

---

### Runtime Scope Control

With remote logging, use `logobserver` to control scopes and priorities in real-time without restarting applications:

**Features:**
- Change log priorities dynamically
- Enable/disable scopes on the fly
- Configure multiple applications simultaneously

> [!NOTE]
> `logobserver` is a control and log collector tool only - it does not display logs. For log viewing, use Lusan GUI application.

**See:** [Log Observer Guide](./04c-logobserver.md) for details

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Log Viewing with Lusan

**Lusan** is a GUI application for real-time log monitoring and analysis. It connects to `logcollector` to display logs from all connected applications with advanced filtering and visualization features.

**Features:**
- Real-time log viewing from multiple applications
- Advanced filtering by priority, scope, and text
- Color-coded log levels
- Search and export capabilities
- Timeline visualization

**Prerequisites:**
- `logcollector` service running
- Applications configured for remote logging

**See:** [Lusan Live Log Viewer Guide](./06f-lusan-live-logging.md) for complete setup and usage.

> [!TIP]
> Lusan provides a professional GUI alternative to viewing log files. It's part of the Areg SDK and developed in [areg-sdk-tools](https://github.com/aregtech/areg-sdk-tools) repository.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Scopes and Priorities

### What are Scopes?

**Scopes** are logical groupings of code where logging occurs. They represent:
- Functions or methods
- Classes or modules
- Components or subsystems

**Scope configuration:**

```ini
# Enable all scopes
log::*::scope::* = DEBUG | SCOPE

# Application-specific scopes
log::myapp::scope::module_foo = DEBUG | SCOPE
log::myapp::scope::module_bar = INFO
log::myapp::scope::module_baz = NOTSET
```

---

### Priority Levels

| Priority | Value | What Gets Logged |
|----------|-------|------------------|
| **NOTSET** | Disabled | Nothing |
| **FATAL** | Highest | Fatal errors only |
| **ERROR** | High | Errors + Fatal |
| **WARN** | Medium | Warnings + Errors + Fatal |
| **INFO** | Low | Info + Warnings + Errors + Fatal |
| **DEBUG** | Lowest | Everything (Debug + Info + Warn + Error + Fatal) |
| **SCOPE** | Special | Scope entry/exit only |

---

### Priority Combinations

Priorities can be combined with `|` (OR):

**Common combinations:**

```ini
# All messages + scope entry/exit
log::*::scope::* = DEBUG | SCOPE

# Warnings and errors only
log::*::scope::* = WARN

# Errors, fatal, + scope tracking
log::*::scope::* = ERROR | SCOPE

# Info and above, no scope tracking
log::*::scope::* = INFO

# Disable all logging
log::*::scope::* = NOTSET
```

---

### Priority Examples

**Example 1: `DEBUG | SCOPE`**

**Configuration:**
```ini
log::*::scope::mymodule = DEBUG | SCOPE
```

**Logs:**
- ✅ Scope entry/exit
- ✅ DEBUG messages
- ✅ INFO messages
- ✅ WARN messages
- ✅ ERROR messages
- ✅ FATAL messages

**Result:** Everything

---

**Example 2: `WARN | SCOPE`**

**Configuration:**
```ini
log::*::scope::mymodule = WARN | SCOPE
```

**Logs:**
- ✅ Scope entry/exit
- ❌ DEBUG messages
- ❌ INFO messages
- ✅ WARN messages
- ✅ ERROR messages
- ✅ FATAL messages

**Result:** Warnings and errors + scope tracking

---

**Example 3: `ERROR`**

**Configuration:**
```ini
log::*::scope::mymodule = ERROR
```

**Logs:**
- ❌ Scope entry/exit
- ❌ DEBUG messages
- ❌ INFO messages
- ❌ WARN messages
- ✅ ERROR messages
- ✅ FATAL messages

**Result:** Only errors and fatal, no scope tracking

---

**Example 4: `NOTSET`**

**Configuration:**
```ini
log::*::scope::mymodule = NOTSET
```

**Logs:**
- ❌ Everything disabled

**Result:** No logging for this scope

---

### Wildcard Scopes

**Enable groups of scopes:**

```ini
# All scopes starting with "network"
log::*::scope::network_* = DEBUG | SCOPE

# All scopes in myapp
log::myapp::scope::* = INFO

# Specific patterns
log::*::scope::http_* = WARN
log::*::scope::db_* = ERROR
```

---

### Application-Specific Scopes

**Different settings per application:**

```ini
# Global default
log::*::scope::* = WARN

# Application A: verbose logging
log::appA::scope::* = DEBUG | SCOPE

# Application B: errors only
log::appB::scope::* = ERROR

# Application C: specific modules
log::appC::scope::network_* = DEBUG
log::appC::scope::database_* = INFO
log::appC::scope::ui_* = NOTSET
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Common Configurations

### Development Configuration

**Goal:** Maximum visibility for debugging

```ini
# Log to file and debug output
log::*::target = file | debug
log::*::enable::file = true
log::*::enable::debug = true

# Detailed logs with timestamps
log::*::file::location = ./logs/%appname%_%time%.log

# Log everything
log::*::scope::* = DEBUG | SCOPE

# Detailed layout
log::*::layout::enter   = %d [%t] --> %x.%z%n
log::*::layout::message = %d [%t] %p: %m%n
log::*::layout::exit    = %d [%t] <-- %x.%z%n
```

---

### Production Configuration

**Goal:** Performance + essential logs

```ini
# Log to file only
log::*::target = file
log::*::enable::file = true

# Single log file
log::*::file::location = /var/log/myapp/myapp.log

# Warnings and errors only
log::*::scope::* = WARN

# Minimal layout
log::*::layout::message = %d %p: %m%n
```

---

### Distributed System Configuration

**Goal:** Centralized logging across network

```ini
# Remote logging
log::*::target = remote | file
log::*::enable::remote = true
log::*::enable::file = true

# Backup local file
log::*::file::location = ./logs/%appname%.log

# Remote collector settings
logger::*::connect = tcpip
logger::*::address::tcpip = 192.168.1.100
logger::*::port::tcpip = 8282

# Buffer for network delays
log::*::remote::queue = 200

# Info and above
log::*::scope::* = INFO
```

---

### Debugging Specific Module

**Goal:** Focus on one module

```ini
# Log to file
log::*::target = file
log::*::enable::file = true
log::*::file::location = ./logs/%appname%.log

# Most modules: warnings only
log::*::scope::* = WARN

# Target module: verbose
log::*::scope::problematic_module = DEBUG | SCOPE

# Related modules: info
log::*::scope::related_module_* = INFO
```

---

### Performance Testing Configuration

**Goal:** Minimal logging overhead

```ini
# File only
log::*::target = file
log::*::enable::file = true
log::*::file::location = ./logs/%appname%.log

# Errors only
log::*::scope::* = ERROR

# Minimal layout
log::*::layout::message = %p: %m%n

# No scope tracking
# (no SCOPE in priorities)
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### No Log File Created

**Problem:** Application runs but no log file appears.

**Solution:**

**1. Check logging is enabled:**

```cpp
Application::initApplication(
    true,   // Must be true ← Check this
    true, true, true, false,
    "./areg.init",
    nullptr
);
```

**2. Check configuration file is loaded:**

```bash
# Verify file exists
ls -l ./areg.init

# Check file has content
cat areg.init | grep "log::"
```

**3. Check target is enabled:**

```ini
log::*::target = file          # Must include 'file'
log::*::enable::file = true    # Must be true
```

**4. Check directory exists:**

```bash
# If log location is ./logs/app.log
mkdir -p ./logs
```

**5. Check file permissions:**

```bash
# Ensure write permissions
chmod 755 ./logs
```

---

### Empty Log File

**Problem:** Log file created but empty.

**Solution:**

**Check scopes are enabled:**

```ini
# Verify scopes are not NOTSET
log::*::scope::* = DEBUG | SCOPE  # Enable all scopes
```

**Check priority levels:**

```ini
# If set to ERROR, only errors will log
log::*::scope::* = DEBUG | SCOPE  # Change to log everything
```

**Verify logging code:**

```cpp
// Make sure you're actually logging
LOG_DEBUG("This is a debug message");
LOG_INFO("This is an info message");
```

---

### Cannot Connect to Remote Logger

**Problem:** Application cannot connect to log collector.

**Solution:**

**1. Verify logcollector is running:**

```bash
# Check process
ps aux | grep logcollector  # Linux
tasklist | findstr logcollector  # Windows
```

**2. Check network connectivity:**

```bash
telnet 192.168.1.100 8282
# or
nc -zv 192.168.1.100 8282
```

**3. Verify configuration:**

```ini
logger::*::connect = tcpip
logger::*::address::tcpip = 192.168.1.100  # Correct address?
logger::*::port::tcpip = 8282              # Correct port?
```

**4. Check firewall:**

```bash
# Linux
sudo ufw allow 8282/tcp

# Windows
New-NetFirewallRule -DisplayName "Log Collector" -Direction Inbound -Protocol TCP -LocalPort 8282 -Action Allow
```

**5. Fallback to file logging:**

```ini
# Enable both remote and file
log::*::target = remote | file
log::*::enable::file = true
```

---

### Log File Too Large

**Problem:** Log file grows too large.

**Solution:**

**1. Reduce log level:**

```ini
# Change from DEBUG to INFO or WARN
log::*::scope::* = WARN
```

**2. Disable scope tracking:**

```ini
# Remove SCOPE from priorities
log::*::scope::* = WARN  # No | SCOPE
```

**3. Use separate log files:**

```ini
# New file per run
log::*::file::location = ./logs/%appname%_%time%.log
```

**4. Implement log rotation:**

Use external tools:
- Linux: `logrotate`
- Windows: Task Scheduler + script

**5. Disable verbose scopes:**

```ini
# Most scopes: errors only
log::*::scope::* = ERROR

# Critical scopes only: detailed
log::*::scope::critical_module = DEBUG | SCOPE
```

---

### Invalid Format Specifiers

**Problem:** Log output shows `%d`, `%m` literally.

**Solution:**

**Check layout configuration:**

```ini
# Wrong (missing %)
log::*::layout::message = d: p: m%n

# Correct
log::*::layout::message = %d: %p: %m%n
```

**Verify end-of-line:**

```ini
# Always include %n at end
log::*::layout::message = %d %p: %m%n
                                   ^^
```

---

### Logs from Wrong Application

**Problem:** Seeing logs from other applications.

**Solution:**

**Use application-specific configuration:**

```ini
# Instead of wildcard
log::*::scope::* = DEBUG | SCOPE

# Use application name
log::myapp::scope::* = DEBUG | SCOPE
```

**Or use separate configuration files:**

```cpp
// Application A
Application::initApplication(true, true, true, true, false, "./config/appA.init", nullptr);

// Application B
Application::initApplication(true, true, true, true, false, "./config/appB.init", nullptr);
```

---

### Debug Output Not Showing in Visual Studio

**Problem:** Logs not appearing in Output window.

**Solution:**

**1. Enable debug target:**

```ini
log::*::target = debug
log::*::enable::debug = true
```

**2. Check Output window:**

- Open Visual Studio
- View → Output (or Ctrl+Alt+O)
- Select "Debug" from dropdown

**3. Verify running in Debug mode:**

```cpp
#ifdef _DEBUG
    // Debug output only works in Debug builds
#endif
```

**4. Check output window filters:**

- Ensure no filters are active in Output window

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Related Guides:**
- [Log Collector Guide](./XX-logcollector.md) - Remote log collection service
- [Log Observer Guide](./04c-logobserver.md) - Runtime scope control
- [Persistence Syntax](./06a-persistence-syntax.md) - Configuration file format

**Configuration Files:**
- [areg.init](../../framework/areg/resources/areg.init) - Default configuration

**Tools:**
- `logcollector` - Centralized log collection service
- `logobserver` - Runtime scope and priority control
- `Lusan` - GUI log viewer and analysis tool ([areg-sdk-tools](https://github.com/aregtech/areg-sdk-tools) repo)

**Help:**
For questions, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech
