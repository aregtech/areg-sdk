# Key-Value Data Persistence in Areg Framework

## Overview

The Areg Framework provides a lightweight key-value persistence mechanism designed for system configuration and runtime parameters. Unlike heavyweight structured formats (XML, JSON, YAML), the Areg [persistence module](./../../framework/areg/persist) prioritizes parsing speed, minimal memory footprint, and developer simplicity — making it ideal for embedded systems, real-time applications, and performance-critical software.

This document describes the **generic persistence mechanism**: the file syntax, the storage model, the wildcard resolution rules, and the `ConfigManager` API you can use to store your *own* application data.

> 💡 The framework's own configuration file, `areg.init`, is built on this exact mechanism. For a complete reference of every `areg.init` section and property (logging, router, log collector, network tuning), see **[Areg Configuration File Reference (`areg.init`)](./05b-areg-configuration-file.md)**.

**When to use**: configuration files, initialization parameters, logging settings, runtime tuning, simple per-application key-value state.  
**When *not* to use**: complex hierarchical data, large datasets, or application-specific data serialization (use [`SharedBuffer`](./../../framework/areg/base/SharedBuffer.hpp) / streaming instead).

---

## Table of Contents
1. [Why Key-Value Persistence](#1-why-key-value-persistence)
2. [The Property Model](#2-the-property-model)
3. [Syntax Reference](#3-syntax-reference)
4. [Storage Model: Read-Only, Writable, Temporary](#4-storage-model-read-only-writable-temporary)
5. [Wildcard Resolution](#5-wildcard-resolution)
6. [Reading and Writing with ConfigManager](#6-reading-and-writing-with-configmanager)
7. [Storing Your Own Data](#7-storing-your-own-data)
8. [Best Practices](#8-best-practices)

---

## 1. Why Key-Value Persistence

**Design philosophy**

The Areg persistence module solves three problems in system-level C++ development:

- **Fast initialization** — parse configuration in microseconds, not milliseconds; critical for real-time systems.
- **Zero dependencies** — pure C++ implementation, no external parser libraries.
- **Human-readable** — engineers edit the files directly with any text editor; no tooling required.

**One file, many processes**

A key advantage is **single-file, multi-process configuration**. Several applications can share one file that holds both global defaults and per-application overrides:

```text
log::*::scope::*            = NOTSET            # Default: no logging for any application
log::myapp::scope::*        = DEBUG | SCOPE     # Override: enable logging only for 'myapp'
```

When `myapp` queries `log::myapp::scope::*`, it receives `DEBUG | SCOPE`; every other application receives `NOTSET` from the wildcard. This removes the need to maintain separate configuration files for distributed processes — ideal for IPC deployments.

**Typical use cases**

- Logging verbosity and output formatting
- Network and IPC endpoint settings
- Feature flags and runtime behavior switches
- Multi-process application coordination

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 2. The Property Model

Each line of a configuration file defines one **property** — a key-value pair. The key is a four-part, `::`-separated path:

```text
section :: module :: property [ :: position ]   =   value
```

| Component | Required | Meaning | Examples |
|-----------|----------|---------|----------|
| **section** | Yes | Top-level configuration domain | `log`, `router`, `net`, `config`, or your own |
| **module** | Yes | Target executable (process) name, or `*` for *all* processes | `myapp`, `mtrouter`, `*` |
| **property** | Yes | Setting identifier within the section | `scope`, `enable`, `address`, `port` |
| **position** | No | Sub-qualifier for compound settings | `tcpip`, `enter`, `areg_*`, `database_*` |

```text
log::*::scope::*                    # section=log  module=*       property=scope    position=*
log::myapp::scope::database_*       # section=log  module=myapp   property=scope    position=database_*
router::*::address::tcpip           # section=router module=*     property=address  position=tcpip
net::mtrouter::tcpip::sndbuf        # section=net  module=mtrouter property=tcpip    position=sndbuf
config::*::version                  # section=config module=*     property=version  (no position)
```

The **module** field is the linchpin of the whole model — it decides whether a property is a shared global default or a process-specific value. See [Storage Model](#4-storage-model-read-only-writable-temporary).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 3. Syntax Reference

### Basic format

```text
<property_key> = <property_value>
```

### Rules

| Element | Description | Example |
|---------|-------------|---------|
| **Assignment** | Key and value separated by `=` | `log::*::enable = true` |
| **Key separator** | Double colon `::` between key parts | `log::myapp::scope::*` |
| **Whitespace** | Spaces and tabs around `=` and `::` are ignored | `key  =  value` ≡ `key=value` |
| **Terminator** | Optional `;` after the value | `router::*::port::tcpip = 8181;` |
| **Value list** | Pipe `\|` separates multiple values (logical OR) | `DEBUG \| SCOPE` |
| **Comment** | Text starting with `# ` (hash **+ space**) | `# this is a comment` |
| **Inline comment** | A ` # ` after the value ends the value | `port = 8181  # router port` |
| **Boolean** | Literal `true` / `false` | `log::*::enable::file = false` |

> ⚠️ **The space after `#` is significant.** `# comment` is a comment; `#comment` (no space) is treated as a blank/ignored line, *not* a recognized comment. Always write `# `.

The parser is implemented in [`Property::parse()`](./../../framework/areg/persist/Property.hpp); the syntax constants live in [`PersistenceDefs.hpp`](./../../framework/areg/persist/PersistenceDefs.hpp).

### Example

```text
# Global logging configuration
log::*::scope::*            = NOTSET;           # Default: disable all scopes
log::myapp::scope::*        = DEBUG | SCOPE     # Override for 'myapp'
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 4. Storage Model: Read-Only, Writable, Temporary

When `ConfigManager` reads a file, it sorts every property into one of two in-memory lists **based solely on the `module` field**, relative to the current process name:

```
                 ┌──────────────────────────────────────────────┐
  areg.init  ──▶ │  for each property line                       │
                 │    module == "*"            ─▶ READ-ONLY list  │
                 │    module == <this process> ─▶ WRITABLE list   │
                 │    module == <other process>─▶ DISCARDED       │
                 └──────────────────────────────────────────────┘
```

This is the single most important rule of the model:

- **Read-only properties** (`module = *`) are global defaults shared by every process. They are never modified or written back. Conceptually they are the immutable "factory settings".
- **Writable properties** (`module =` the running process's name) are this process's own values. They override the global defaults and can be changed at runtime and saved back to the file.
- **Properties addressed to a *different* module are dropped at load time** — process `myapp` never holds `log::server::scope::*` in memory. This keeps each process's view small and prevents one application from mutating another's settings.

> The exact split happens while `ConfigManager::read_config()` parses the file (the `_read_config` helper in [ConfigManager.cpp](./../../framework/areg/persist/private/ConfigManager.cpp)): `key.is_all_modules()` → read-only; `is_module_property(currentModule)` → writable; otherwise ignored.

### The "temporary" attribute

There is no separate "temporary list". **Temporary is a boolean flag on a writable property.** A writable entry created with `temporary = true`:

- participates in lookups exactly like any other writable entry, **but**
- is **excluded when the configuration is saved** (`save_config`), so it never reaches the file.

Use it for runtime-only state — a debug toggle you flip for the current session, a counter, an ephemeral override — that should vanish on the next start. Persistent writable entries (the default, `temporary = false`) *are* written back and survive restarts.

| | Read-only | Writable (persistent) | Writable (temporary) |
|---|---|---|---|
| Key module field | `*` | this process | this process |
| Modifiable at runtime | No | Yes | Yes |
| Saved to file | Yes (unchanged) | Yes | **No** |
| Survives restart | Yes | Yes | No |
| Typical use | shared defaults | per-app settings | session-only state |

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 5. Wildcard Resolution

When the application queries a key, `ConfigManager` resolves it in two tiers:

1. **Writable list** — look for an *exact* entry for the current process module.
2. **Read-only list** — if not found, fall back to the `*` (wildcard) entry.

```text
log::*::scope::areg_*       = NOTSET    # all apps: disable scopes starting with 'areg_'
log::*::scope::*            = WARN      # all apps: default WARN for everything else
log::myapp::scope::*        = DEBUG     # 'myapp' only: override to DEBUG
```

→ `myapp` resolving `log::myapp::scope::*` gets `DEBUG` (writable wins).  
→ any other app resolving `log::*::scope::*` gets `WARN` (read-only default).

**Position wildcards.** A `*` (or a `prefix_*`) in the *position* field matches a family of keys. For example, `log::*::scope::areg_*` applies to every scope whose name begins with `areg_`. Pattern matching of scope names is hierarchical and is described in the [Areg Configuration Reference — `log` section](./05b-areg-configuration-file.md#5-log-section--logging).

### Override-collapse (set semantics)

When you *set* a module value, `ConfigManager` is smart about not creating redundant overrides:

- If the new value **differs** from the read-only default → a writable override is created/updated.
- If the new value **equals** the read-only default → any existing writable override is **removed** (the global default already provides that value).

This keeps the writable list — and therefore the saved file — minimal: only genuine deviations from the defaults are persisted. (See `_set_position_value()` in `ConfigManager.cpp`.)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 6. Reading and Writing with ConfigManager

[`ConfigManager`](./../../framework/areg/persist/ConfigManager.hpp) is the object that owns the in-memory property store. In an Areg application it is created and loaded for you by `Application::setup()` / `Application::load_configuration()`, and you reach it through `Application::config_manager()`.

### Loading and saving

```cpp
ConfigManager& config = Application::config_manager();

config.read_config("./config/areg.init");   // empty path => areg::DEFAULT_CONFIG_FILE
// ... read / modify properties ...
config.save_config();                        // writes back to the same file
```

`read_config()` and `save_config()` accept an optional [`ConfigListener*`](./../../framework/areg/persist/ConfigListener.hpp) that is notified before and after the operation, letting components reload derived state. `save_config()` rewrites only this process's non-temporary writable entries and leaves all other lines of the source file intact.

### Reading a value

```cpp
// Generic lookup (writable for this module, then read-only wildcard):
const PropertyValue* v = config.property_value("config", "version");
Version ver = v != nullptr ? Version(v->value()) : Version();

// Typed helpers exist for the framework's own keys, e.g.:
Version  cfgVersion = config.config_version();
bool     fileLog    = config.log_enabled(LogTarget::File);
uint16_t routerPort = config.remote_service_port(RemoteServiceKind::Router, ConnectionType::Tcpip);
```

### Writing a value

```cpp
// Persistent: saved on save_config(), survives restart
config.set_module_property("config", "myflag", "", "enabled");

// Temporary: lives only for this run, never written to disk
config.set_module_property("config", "myflag", "", "enabled",
                           ConfigEntry::AnyKey, /*temporary*/ true);
```

### Thread safety

`ConfigManager` guards every operation with an internal mutex, so individual calls are thread-safe. After the initial load the data is effectively read-mostly, which makes concurrent reads cheap. When you need a *sequence* of calls to be atomic, hold the lock yourself:

```cpp
Lock guard(config.lockable());   // RAII: hold across multiple operations
// ... several reads/writes that must be consistent ...
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 7. Storing Your Own Data

The persistence module is not limited to the framework's predefined keys — you can define your own `section`/`property` names and reuse the same file, syntax, and storage model.

**Choose a section that won't collide** with the framework's (`config`, `log`, `service`, `router`, `logger`, `net`). For example use your application name as the section, or a clearly custom one such as `app`:

```text
# In areg.init (or your own .init file)
app::*::theme              = dark            # shared default for all processes
app::myeditor::theme       = solarized       # 'myeditor' override (writable for that process)
app::myeditor::recent::0   = /home/me/a.txt
app::myeditor::recent::1   = /home/me/b.txt
```

Read and write them through the generic accessors:

```cpp
ConfigManager& config = Application::config_manager();

// Read: 'myeditor' gets "solarized"; any other process gets "dark"
String theme = "dark";
if (const PropertyValue* v = config.property_value("app", "theme"))
    theme = v->value();

// Write a persistent per-process value, then save
config.set_module_property("app", "theme", "", "monokai");
config.save_config();
```

Because the module field drives storage, the same line gives every process a sensible shared default (`app::*::theme`) while letting each process keep its own persisted override — with no extra files and no merge logic on your side.

If you do not want to share the framework's `areg.init`, point `read_config()`/`save_config()` at your own file path; the syntax and behavior are identical.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 8. Best Practices

### Key naming

- Use lowercase ASCII letters, digits, and underscores for key parts.
- Group related settings under a common section (`log::`, `net::`, `app::`).
- Use a position wildcard for families of related keys: `scope::database_*`.
- Pick a section name that cannot collide with framework sections when storing your own data.

### Values

- Keep values atomic; do not encode complex structures in a single value.
- Use multi-value lists for enumerations: `DEBUG | SCOPE`.
- Document units in inline comments: `port = 8181  # TCP port`.

### Choosing storage scope

- **Read-only (`module = *`)** — shared defaults that rarely change: framework endpoints, layouts, baseline scopes.
- **Writable (per-module)** — values a process may change and must remember across restarts.
- **Temporary (writable + flag)** — session-only state that must never touch disk; cheap, no file I/O.

### Performance

- Files are parsed once at startup — keep them small (well under 100 KB) for sub-millisecond parsing.
- Read-only properties have no runtime cost after load.
- Prefer temporary properties for frequently changing values to avoid repeated file writes.
- Persisting (`save_config`) rewrites the file — call it deliberately, not on every change.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Further Reading

- **[Areg Configuration File Reference (`areg.init`)](./05b-areg-configuration-file.md)** — every section and property documented, with examples
- **[Logging Configuration](./04a-logging-config.md)** — scopes, priorities, and outputs
- **[Message Router (`mtrouter`)](./03a-mtrouter.md)** — IPC routing and its network settings
- **Persistence module source**: [framework/areg/persist](./../../framework/areg/persist)
- **Example configuration**: [areg.init](./../../framework/areg/resources/areg.init)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech (Artak Avetyan), www.areg.tech, email: info[at]areg.tech
