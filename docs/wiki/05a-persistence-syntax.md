# Key-Value Data Persistence in Areg Framework

## Overview

The Areg Framework provides a lightweight key-value persistence mechanism designed specifically for system configuration and runtime parameters. Unlike heavyweight structured formats (XML, JSON, YAML), the Areg [persistence module](./../../framework/areg/persist) prioritizes parsing speed, minimal memory footprint, and developer simplicity—making it ideal for embedded systems, real-time applications, and performance-critical software.

**When to use**: Configuration files, initialization parameters, logging settings, runtime tuning.  
**When not to use**: Complex hierarchical data, large datasets, or application-specific data serialization.

---

## Table of Contents
1. [Why Key-Value Persistence](#1-why-key-value-persistence)
2. [How It Works](#2-how-it-works)
3. [Syntax Reference](#3-syntax-reference)
4. [Property Keys](#4-property-keys)
5. [Property Values](#5-property-values)
6. [Practical Examples](#6-practical-examples)
7. [Best Practices](#7-best-practices)

---

## 1. Why Key-Value Persistence

**Design Philosophy**

The Areg SDK persistence module solves three critical problems in system-level C++ development:

- **Fast Initialization**: Parse configuration in microseconds, not milliseconds—critical for real-time systems.
- **Zero Dependencies**: No external libraries required; pure C++ implementation.
- **Human-Readable**: Engineers can edit configuration files directly without specialized tools.

**Shared Configuration for IPC**

A key advantage is **single-file, multi-process configuration**. Multiple applications can share one configuration file with global defaults and per-application overrides:

```text
log::*::scope::*            = NOTSET            # Default: no logging for any application
log::myapp::scope::*        = DEBUG | SCOPE     # Override: enable logging only for 'myapp'
```

When `myapp` queries `log::myapp::scope::*`, it receives `DEBUG | SCOPE`. Other applications receive `NOTSET` from the wildcard. This eliminates the need to maintain separate configuration files in multiple locations—ideal for IPC scenarios where distributed processes need coordinated configuration.

**Use Cases**

- Logging verbosity and output formatting
- Network and IPC endpoint settings
- Feature flags and runtime behavior switches
- Multi-process application coordination

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 2. How It Works

The persistence module reads plain-text configuration files (e.g., [areg.init](./../../framework/areg/resources/areg.init)) and stores key-value pairs with wildcard resolution support. Applications query this store during initialization to configure components, logging, and runtime behavior.

**Processing Pipeline**

1. **Load**: Read configuration file from disk or embedded resource
2. **Parse**: Extract key-value pairs using syntax rules defined below
3. **Store**: Populate internal key-value store with section-based organization
4. **Resolve**: When queried, return module-specific value or fall back to wildcard (`*`) default
5. **Query**: Application components retrieve resolved values by key

**Wildcard Resolution**: When an application queries a key, the persistence module first searches for a module-specific entry (e.g., `log::myapp::scope::*`). If not found, it falls back to the wildcard entry (e.g., `log::*::scope::*`). This enables global defaults with selective overrides.

### Storage Architecture

The Areg persistence module organizes key-value pairs into three distinct storage sections, each with different mutability and lifetime characteristics:

#### Read-Only Section

**Purpose**: Global configuration applicable to all modules.

- **Scope**: Keys with wildcard module (`*`) — e.g., `log::*::scope::areg_*`
- **Mutability**: Immutable after initialization
- **Persistence**: Fixed values that remain constant across process restarts
- **Use Case**: Framework-wide defaults, system-level settings, shared logging scopes

**Example**:
```text
log::*::scope::areg_*       = NOTSET            # Fixed for all applications
log::*::layout::enter       = %d: [ %t  %x.%z: Enter -->]%n
router::*::address::tcpip   = localhost         # Fixed router address
```

When any module queries `log::*::scope::areg_*`, it receives the same immutable value (`NOTSET`).

#### Writable Section

**Purpose**: Module-specific configuration that can be modified and persisted.

- **Scope**: Keys with specific module names — e.g., `log::myapp::scope::*`
- **Mutability**: Applications can modify values at runtime
- **Persistence**: Changes are written to the file system and survive process restarts
- **Use Case**: User preferences, feature toggles, runtime-adjustable settings

**Example**:
```text
log::myapp::scope::*        = INFO | SCOPE      # Initial value
# Application changes to DEBUG | SCOPE at runtime
# Next startup reads: log::myapp::scope::* = DEBUG | SCOPE
```

Writable properties enable applications to store configuration state between sessions without external databases.

#### Temporary Section

**Purpose**: Runtime-only module-specific properties.

- **Scope**: Keys with specific module names, marked as temporary
- **Mutability**: Applications can modify values at runtime
- **Persistence**: **Not persisted**—changes exist only during process lifetime
- **Lifecycle**: Created on startup, destroyed on shutdown
- **Use Case**: Session state, runtime flags, ephemeral counters, dynamic behavior switches

**Example**:
```text
# Process 'myapp' sets temporary property at runtime (not in areg.init file)
log::myapp::scope::temp_debug = DEBUG | SCOPE   # Exists only while process runs
# After shutdown, this property is lost
```

Temporary properties provide fast, memory-only storage for transient state without file I/O overhead.

### Section Interaction

When a module queries a key, the resolution order follows this priority:

1. **Temporary section** (module-specific, runtime-modified, not persisted)
2. **Writable section** (module-specific, runtime-modified, persisted)
3. **Read-only section** (wildcard, immutable, persisted)

**Example Resolution**:
```text
# Configuration file areg.init (read-only + writable sections)
log::*::scope::*            = WARN              # Read-only default
log::myapp::scope::*        = INFO | SCOPE      # Writable initial value

# Runtime (temporary section)
# Application 'myapp' sets temporary override:
#   log::myapp::scope::* = DEBUG | SCOPE

# Query result for 'myapp': DEBUG | SCOPE (temporary takes precedence)
# After restart: INFO | SCOPE (temporary lost, writable persists)
# Other apps query result: WARN (read-only default)
```

This three-tier architecture balances flexibility (writable + temporary) with safety (read-only defaults) while minimizing file system writes (temporary properties never touch disk).

**Thread Safety**: The persistence module is read-only after initialization, allowing lock-free concurrent access from multiple threads.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 3. Syntax Reference

### Basic Format

```text
<property_key> = <property_value>
```

### Rules

| Element | Description | Example |
|---------|-------------|---------|
| **Assignment** | Key and value separated by `=` | `log::*::enable = true` |
| **Terminator** | Optional `;` at end of value | `router::*::port::tcpip = 8181;` |
| **Comments** | Lines starting with `# ` (hash + space) | `# Logging configuration` |
| **Whitespace** | Spaces and tabs ignored around `=` and `::` | `key  =  value` → `key=value` |
| **Key Separator** | Double colon `::` for hierarchical keys | `log::myapp::scope::*` |
| **Value Delimiter** | Pipe `|` for multi-value lists | `DEBUG | SCOPE` |

**Important**: Comments require a space after `#`. The sequence `#comment` is not recognized; use `# comment` instead.

### Example

```text
# Global logging configuration
log::*::scope::*            = NOTSET;           # Default: disable all scopes
log::myapp::scope::*        = DEBUG | SCOPE     # Override for 'myapp'
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 4. Property Keys

### Anatomy of a Key

```text
section::module::property[::position]
```

| Component | Required | Description | Examples |
|-----------|----------|-------------|----------|
| **section** | Yes | Configuration domain | `log`, `router`, `logger`, `config` |
| **module** | Yes | Target executable name or `*` for all | `myapp`, `mtrouter`, `*` |
| **property** | Yes | Setting identifier | `scope`, `enable`, `layout`, `address` |
| **position** | No | Sub-category for complex settings | `areg_*`, `tcpip`, `enter`, `message` |

### Key Hierarchy Examples

```text
log::*::scope::*                    # Global scope settings for all executables
log::myapp::scope::*                # Scope settings specific to 'myapp' executable
log::myapp::scope::database_*       # Database-related scopes in 'myapp'
router::*::address::tcpip           # TCP/IP address for message router
```

### Wildcard Behavior

Using `*` in the module position creates **global defaults** that apply to all executables unless overridden by module-specific entries. Wildcards in the position field match patterns (e.g., `areg_*` matches all scopes starting with `areg_`).

**Resolution Priority**: Module-specific keys take precedence over wildcard keys.

```text
log::*::scope::areg_*       = NOTSET    # All apps: disable scopes starting with 'areg_'
log::*::scope::*            = WARN      # All apps: default WARN for other scopes
log::myapp::scope::*        = DEBUG     # 'myapp': override to DEBUG
log::mtrouter::scope::*     = NOTSET    # 'mtrouter': no logging
```

**Resolution Algorithm**:
1. Query for `log::<module>::scope::*` where `<module>` is the requesting application
2. If module-specific key exists, return its value
3. If not found, search for `log::*::scope::*` (wildcard)
4. If wildcard exists, return its value
5. If neither exists, return default/empty value

This mechanism enables **one configuration file, many applications**—critical for IPC deployments where updating multiple config files is error-prone.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 5. Property Values

### Value Syntax

Values begin immediately after `=` and terminate at:
- Semicolon (optional) `;`
- Comment marker `# `
- End of line

### Single Value

```text
log::*::enable              = true
router::*::port::tcpip      = 8181;
log::*::file::append        = false
```

### Multi-Value Lists

Use pipe `|` to specify multiple values (logical OR relationship):

```text
log::*::scope::*            = DEBUG | SCOPE
log::*::target              = remote | file | debug | db
router::*::connect          = tcpip
```

**Parsing Behavior**: The persistence module returns multi-value entries as an array or list type (implementation-dependent).

### Special Characters

Values support all printable ASCII characters. To include literal `#` or `;` in a value, ensure no space precedes `#`, or use `;` only as a terminator.

```text
config::*::version          = 2.0.0         # Valid: version number
log::*::file::location      = ./logs/%appname%_%time%.log   # Valid: masks
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 6. Practical Examples

### Complete Configuration File

Wildcard in position (`database_*`) matches any scope starting with `database_`.

```text
# Areg Framework Configuration - areg.init
# Shared by multiple processes in IPC deployment

# ===== Global Defaults (Read-Only Section) =====
log::*::scope::*            = NOTSET            # Disable all scopes by default
log::*::scope::areg_*       = NOTSET            # Framework scopes disabled
log::*::layout::enter       = %d: [ %t  %x.%z: Enter -->]%n
log::*::layout::message     = %d: [ %t  %p >>> ] %m%n
log::*::layout::exit        = %d: [ %t  %x.%z: Exit <-- ]%n
log::*::enable              = true              # Global logging enabled
log::*::enable::remote      = true              # Remote logging enabled
log::*::enable::file        = false             # File logging disabled
log::*::file::location      = ./logs/%appname%_%time%.log

# ===== Application-Specific Overrides (Writable Section) =====
log::myapp::scope::*        = DEBUG | SCOPE     # Enable all scopes for 'myapp'
log::myapp::scope::ui_*     = WARN | SCOPE      # UI scopes at WARN level

log::server::scope::*       = INFO | SCOPE      # 'server' uses INFO level
log::server::scope::database_* = DEBUG | SCOPE  # Database scopes at DEBUG

log::mtrouter::scope::*     = NOTSET            # Message router: no logging
log::logcollector::scope::* = NOTSET            # Log collector: no logging

# Note: 'client' app will use global default (NOTSET) - no logging

# ===== Remote Service Configuration =====
router::*::service          = mtrouter          # Message router service name
router::*::connect          = tcpip             # Supported protocol
router::*::enable::tcpip    = true              # TCP/IP enabled
router::*::address::tcpip   = localhost         # Router IP address
router::*::port::tcpip      = 8181              # Router port number

logger::*::service          = logcollector      # Log collector service name
logger::*::connect          = tcpip             # Supported protocol
logger::*::enable::tcpip    = true              # TCP/IP enabled
logger::*::address::tcpip   = localhost         # Log collector IP address
logger::*::port::tcpip      = 8282              # Log collector port number
```

**Section Classification**:
- **Read-only**: All `log::*::...` (wildcard module), `router::*::...`, `logger::*::...`
- **Writable**: `log::myapp::...`, `log::server::...`, `log::mtrouter::...`
- **Temporary**: Not shown in file (created at runtime programmatically)

### Log Layout Tokens

Format specifiers in `log::*::layout::*` properties:

| Token | Meaning               | Example Output                |
|-------|-----------------------|-------------------------------|
| `%a`  | Logging object ID     | `0x1234`                      |
| `%c`  | Tick-count            | `123456`                      |
| `%d`  | Date/time             | `2025-01-26 14:32:45`         |
| `%e`  | Module (process) ID   | `5678`                        |
| `%m`  | Message text          | `Connection established`      |
| `%n`  | Newline               | (platform-specific)           |
| `%p`  | Priority (log level)  | `INFO`, `DEBUG`, `WARN`       |
| `%s`  | Scope ID              | `42`                          |
| `%t`  | Thread ID             | `0x7f8a`                      |
| `%x`  | Module/Process name   | `myapp`                       |
| `%y`  | Thread name           | `WorkerThread`                |
| `%z`  | Scope name            | `myapp_Connection_connect`    |

### Scope Priority Values

Valid values for `log::*::scope::*` properties:

| Priority | Description | Logs Output |
|----------|-------------|-------------|
| `DEBUG` | Debug level (lowest) | DEBUG, INFO, WARN, ERROR, FATAL |
| `INFO` | Information level | INFO, WARN, ERROR, FATAL |
| `WARN` | Warning level | WARN, ERROR, FATAL |
| `ERROR` | Error level | ERROR, FATAL |
| `FATAL` | Fatal errors (highest) | FATAL only |
| `SCOPE` | Enter/exit messages | Scope boundaries (combine with others) |
| `NOTSET` | Disabled | No logging for this scope |

**Combinations**: Use `|` to combine priorities: `DEBUG | SCOPE`, `WARN | SCOPE`

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 7. Best Practices

### Key Naming Conventions

- Use lowercase ASCII letters, digits, and underscores only
- Prefer descriptive names: `database_connection` over `db_conn`
- Group related settings under common sections: `log::`, `router::`, `logger::`
- Use wildcards in position for pattern matching: `scope::database_*` for all database scopes

### Value Guidelines

- Keep values simple and atomic (avoid encoding complex structures)
- Use multi-value lists for enumeration sets: `DEBUG | SCOPE`
- Document value units in comments: `port = 8181 # Port number`
- Use masks in file paths: `%appname%_%time%.log` for timestamped files

### File Organization

```text
# Structure configuration files in sections:
# 1. Global defaults (using *) - Read-Only Section
# 2. Application-specific overrides - Writable Section
# 3. Remote service configuration
# 4. Module-specific settings
```

### Choosing Storage Types

**Use Read-Only (wildcard keys) for:**
- Framework defaults that never change: `log::*::layout::*`
- System-wide logging configurations: `log::*::scope::areg_*`
- Service endpoints: `router::*::address::tcpip`
- Constants shared across all applications

**Use Writable (module-specific) for:**
- Application-specific scope settings: `log::myapp::scope::*`
- Per-module log levels that may change between sessions
- Feature flags that persist: `log::myapp::scope::ui_*`
- Runtime-adjustable settings that should survive restarts

**Use Temporary (runtime-only) for:**
- Session tokens and authentication state
- Real-time counters and metrics: `log::myapp::scope::temp_counter`
- Debug flags enabled at runtime without file modification
- Transient feature switches for testing

### Shared Configuration Strategy

When multiple applications share one `areg.init` file:
- Set conservative defaults with wildcards: `log::*::scope::* = NOTSET`
- Enable features selectively per application: `log::myapp::scope::* = DEBUG | SCOPE`
- Disable logging for system services: `log::mtrouter::scope::* = NOTSET`
- Place the shared file in a common location: `/etc/areg/areg.init` or `~/.config/areg/areg.init`
- Use environment variables to override the configuration file path if needed

### Performance Considerations

- Configuration files are parsed once at startup—keep files under 100KB for sub-millisecond parsing
- Use temporary properties for frequently changing values to avoid file I/O
- Writable properties trigger file updates on change—use sparingly for critical data only
- Read-only properties (wildcards) have no runtime overhead after initialization

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Further Reading

- **Areg SDK Repository**: [github.com/aregtech/areg-sdk](https://github.com/aregtech/areg-sdk)
- **Persistence Module Source**: [framework/areg/persist](./../../framework/areg/persist)
- **Example Configuration**: [areg.init](./../../framework/areg/resources/areg.init)
- **Logging Documentation**: [Areg Logging Guide](../logging/README.md)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
