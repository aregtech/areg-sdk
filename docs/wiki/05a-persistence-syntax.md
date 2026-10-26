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

A process reads **two files**. The shared file `areg.init` is read-only for every process. Each process has its own file `<process>.init` in the same directory (for example `./config/myapp.init` beside `./config/areg.init`), and that is the only file it ever writes:

```
                   ┌───────────────────────────────────────────────────┐
  areg.init    ──▶ │  module == "*"             ─▶ READ-ONLY list       │
                   │  module == <this process>  ─▶ READ-ONLY list       │
                   │  module == <other process> ─▶ DISCARDED            │
                   └───────────────────────────────────────────────────┘
                   ┌───────────────────────────────────────────────────┐
  myapp.init   ──▶ │  module == <this process>  ─▶ WRITABLE list        │
                   │  any other line            ─▶ ignored, kept on save│
                   └───────────────────────────────────────────────────┘
```

This is the single most important rule of the model:

- **`areg.init` is shared and never written.** Its `*` entries are the global defaults; its `<this process>` entries are defaults for one executable. A save refuses to write `areg.init`, whatever path it is given, and returns `false`.
- **Writable properties** are the entries of `<process>.init` plus the changes made at run time. They win over everything in `areg.init`, and a save writes them to `<process>.init`.
- **Properties addressed to a *different* module are dropped at load time** — process `myapp` never holds `log::server::scope::*` in memory. This keeps each process's view small and prevents one application from mutating another's settings.
- **Without `areg.init`** the framework defaults take its place; `<process>.init` still applies on top of them, and `areg.init` is never created.

> The split happens while `ConfigManager::read_config()` parses the shared file, then `ConfigManager::read_process_config()` reads the process file (both in [ConfigManager.cpp](./../../framework/areg/persist/private/ConfigManager.cpp)). `ConfigManager::process_config_file()` returns the path of the process file.

### The "temporary" attribute

There is no separate "temporary list". **Temporary is a boolean flag on a writable property.** A writable entry created with `temporary = true`:

- participates in lookups exactly like any other writable entry, **but**
- is **excluded when the configuration is saved** (`save_config`), so it never reaches the file.

Use it for runtime-only state — a debug toggle you flip for the current session, a counter, an ephemeral override — that should vanish on the next start. Persistent writable entries (the default, `temporary = false`) *are* written back and survive restarts.

| | Read-only | Writable (persistent) | Writable (temporary) |
|---|---|---|---|
| Comes from | `areg.init` (`*` or this process) | `<process>.init`, or set at run time | set at run time |
| Modifiable at runtime | No | Yes | Yes |
| Saved | Never | Yes, to `<process>.init` | **No** |
| Survives restart | Yes | Yes | No |
| Typical use | shared defaults | per-app settings | session-only state |

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 5. Wildcard Resolution

When the application queries a key, `ConfigManager` resolves it in three tiers. The first tier that has the key wins:

1. **`<process>.init` and run-time changes** — the writable list.
2. **`areg.init`, this process** — an entry with the current process name as module.
3. **`areg.init`, all processes** — the `*` (wildcard) entry. Without it, the framework default applies.

```text
# areg.init
log::*::scope::*            = WARN      # all apps: default WARN
log::myapp::scope::*        = INFO      # 'myapp' only

# myapp.init
log::myapp::scope::*        = DEBUG     # saved by 'myapp', wins over both lines above
```

→ `myapp` gets `DEBUG`; without `myapp.init` it gets `INFO`; any other app gets `WARN`.

**Log scopes resolve by tier first, then by name.** A scope takes its priority from the first tier that has *any* entry matching it, exact name or group. Inside that tier the exact name wins, then the closest group. So `log::myapp::scope::abc_* = INFO` in `myapp.init` sets `abc_x` to `INFO` even if `areg.init` names `abc_x` exactly.

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

config.read_config("./config/areg.init");   // also reads ./config/<process>.init
// ... read / modify properties ...
config.save_config();                        // writes ./config/<process>.init
```

`read_config()` and `save_config()` accept an optional [`ConfigListener*`](./../../framework/areg/persist/ConfigListener.hpp) that is notified before and after the operation, letting components reload derived state. `save_config()` writes only this process's non-temporary writable entries and keeps the other lines of the target file.

A save is safe when several processes save at the same time, also two instances of one executable writing the same `<process>.init`:

- Saves of one file take turns through a lock file beside it (`<process>.init.lock`). The operating system releases the lock if a process ends.
- The new content is written to a temporary file in the same directory, flushed, and then replaces the target in one step. A reader sees the old file or the new one, never a mix, and a crash never leaves a half-written file.
- Two instances that save one file leave the content of the save that finished last; each instance keeps its own values in memory until it restarts.

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
app::myeditor::theme       = solarized       # 'myeditor' default (read-only, in areg.init)
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

The shared file gives every process a sensible default (`app::*::theme`) and a per-process default (`app::myeditor::theme`), while the saved `monokai` goes to `myeditor.init` and wins on the next start — with no merge logic on your side.

`save_config()` writes to `<process>.init` by default. Give it another path to save a copy elsewhere; the syntax and behavior are identical, and only the shared `areg.init` is refused.

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

- **Read-only (`areg.init`)** — shared defaults that rarely change: framework endpoints, layouts, baseline scopes. Edit the file by hand; no process writes it.
- **Writable (`<process>.init`)** — values a process may change and must remember across restarts.
- **Temporary (writable + flag)** — session-only state that must never touch disk; cheap, no file I/O.

### Performance

- Files are parsed once at startup — keep them small (well under 100 KB) for sub-millisecond parsing.
- Read-only properties have no runtime cost after load.
- Prefer temporary properties for frequently changing values to avoid repeated file writes.
- Persisting (`save_config`) rewrites `<process>.init` — call it deliberately, not on every change.

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
