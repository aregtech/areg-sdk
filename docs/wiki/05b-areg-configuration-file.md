# Areg Configuration File Reference (`areg.init`)

## Overview

`areg.init` is the single configuration file that controls the behavior of every Areg Framework application — logging, the message router and log collector endpoints, shared-buffer and message-queue defaults, and TCP/IP network tuning. It is a plain-text key-value file built on the Areg [persistence module](./../../framework/areg/persist).

This document is the **complete property reference**: every section and key, its purpose, type, valid values, default, and a usage example. If you are new to the file *syntax* and *storage model* (the `::`-separated keys, the read-only / writable / temporary tiers, wildcard resolution), read **[Key-Value Data Persistence](./05a-persistence-syntax.md)** first — this page assumes that foundation and focuses on *what each property does*.

The canonical, fully-commented default file ships at [`framework/areg/resources/areg.init`](./../../framework/areg/resources/areg.init).

---

## Table of Contents
1. [File Location and Loading](#1-file-location-and-loading)
2. [Key Anatomy and the Module Field](#2-key-anatomy-and-the-module-field)
3. [Quick Reference: All Properties](#3-quick-reference-all-properties)
4. [`config` Section — Global Runtime Settings](#4-config-section--global-runtime-settings)
5. [`log` Section — Logging](#5-log-section--logging)
6. [`service` / `router` / `logger` — Remote Services](#6-service--router--logger--remote-services)
7. [`net` Section — Network Tuning](#7-net-section--network-tuning)
8. [Worked Examples / Recipes](#8-worked-examples--recipes)
9. [Programmatic Access](#9-programmatic-access)
10. [Troubleshooting and Gotchas](#10-troubleshooting-and-gotchas)

---

## 1. File Location and Loading

By default the framework loads `./config/areg.init` (relative to the working directory) — the constant `areg::DEFAULT_CONFIG_FILE` in [`AppDefs.hpp`](./../../framework/areg/appbase/AppDefs.hpp).

Loading happens during application startup:

```cpp
Application::setup( /*startTracing*/ true
                  , /*startServicing*/ true
                  , /*startRouting*/ true
                  , /*startTimer*/ true
                  , /*startWatchdog*/ true
                  , /*configFile*/ areg::DEFAULT_CONFIG_FILE.data() );   // pass a custom path here
```

`Application::setup()` calls `Application::load_configuration()`, which reads the file into a [`ConfigManager`](./../../framework/areg/persist/ConfigManager.hpp). The parsed values are then consumed by:

| Consumer | Reads | Drives |
|----------|-------|--------|
| `LogConfiguration` | `log::…` | logging targets, layouts, scopes, file/remote settings |
| `ConnectionConfiguration` | `service::…`, `router::…`, `logger::…`, `net::…` | IPC endpoints, socket buffers |
| `Application` / dispatchers | `config::…` | shared-buffer block size, message-queue limits |

If the file is missing, applications fall back to the hard-coded compile-time defaults noted throughout this document, so an Areg application still runs without an `areg.init`.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 2. Key Anatomy and the Module Field

Every key is `section :: module :: property [:: position]`. The **module** field decides scope:

- `module = *` → **read-only global default**, shared by all processes.
- `module = <process-name>` → **writable, process-specific** value that overrides the global default for that one executable.
- A process loads only its own module entries plus the `*` entries; entries for *other* modules are ignored.

```text
log::*::scope::*           = DEBUG | SCOPE   # default for every application
log::mtrouter::scope::*    = NOTSET          # override: silence the router only
```

The `*` module is used for almost every key in the shipped file. You add process-specific overrides (`log::myapp::…`, `net::mtrouter::…`) below the globals. See [05a §4–5](./05a-persistence-syntax.md#4-storage-model-read-only-writable-temporary) for the precise resolution rules.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 3. Quick Reference: All Properties

| Key (with `*` module) | Value / type | Default | Purpose |
|---|---|---|---|
| `config::*::version` | version `x.y.z` | `2.0.0` | Configuration schema version |
| `config::*::default::blocksize` | bytes | `64` (0 = built-in) | SharedBuffer growth granularity |
| `config::*::default::messagequeue` | count | `0` (unlimited) | Dispatcher message-queue size limit |
| `log::*::version` | version `x.y.z` | `2.0.0` | Logging schema version |
| `log::*::target` | list of `remote\|file\|debug\|db` | `remote\|file\|debug\|db` | Known/available log targets |
| `log::*::enable` | bool | `true` | Master logging switch for the module |
| `log::*::enable::remote` | bool | `true` | Enable remote (TCP) logging |
| `log::*::enable::file` | bool | `false` | Enable file logging |
| `log::*::enable::debug` | bool | `false` | Enable debug-console output |
| `log::*::enable::db` | bool | `false` | Enable database logging (*not implemented in core*) |
| `log::*::file::location` | path + masks | `./logs/%appname%_%time%.log` | Log file path |
| `log::*::file::append` | bool | `false` | Append vs. create-new on open |
| `log::*::remote::queue` | count | `100` (0 = no queue) | Buffered messages while collector offline |
| `log::*::remote::service` | service alias | `logger` | Which `service` block names the collector |
| `log::*::db::engine` … `password` | strings | (empty) | Database logging connection (see §5.7) |
| `log::*::layout::enter` | format string | see §5.8 | Scope-entry line format |
| `log::*::layout::message` | format string | see §5.8 | Message line format |
| `log::*::layout::exit` | format string | see §5.8 | Scope-exit line format |
| `log::*::scope::<pattern>` | priorities | `DEBUG\|SCOPE` | Enable/disable scopes (see §5.9) |
| `service::*::list` | list of aliases | `router\|logger` | Remote services available to apps |
| `router::*::service` | executable name | `mtrouter` | Message-router process name |
| `router::*::connect` | transport list | `tcpip` | Supported router transports |
| `router::*::enable::tcpip` | bool | `true` | Enable router TCP/IP transport |
| `router::*::address::tcpip` | host/IP | `localhost` | Router address |
| `router::*::port::tcpip` | port | `8181` | Router TCP port |
| `logger::*::service` | executable name | `logcollector` | Log-collector process name |
| `logger::*::connect` | transport list | `tcpip` | Supported collector transports |
| `logger::*::enable::tcpip` | bool | `true` | Enable collector TCP/IP transport |
| `logger::*::address::tcpip` | host/IP | `localhost` | Collector address |
| `logger::*::port::tcpip` | port | `8282` | Collector TCP port |
| `net::MODULE::tcpip::sndbuf` | KB | 4096 (4 MB) | `SO_SNDBUF` size |
| `net::MODULE::tcpip::rcvbuf` | KB | 4096 (4 MB) | `SO_RCVBUF` size |
| `net::MODULE::tcpip::drain` | count | `128` | Send batch size per dispatcher wake-up |
| `net::MODULE::tcpip::pairs` | count | `0` (disabled) | Dedicated send/recv thread-pool pairs |
| `net::MODULE::tcpip::timeout` | ms | `2500` | `SO_SNDTIMEO` send timeout |
| `net::MODULE::tcpip::cache` | KB | `256` | Per-socket send/recv cache size |

> "Default" is the value in the shipped `areg.init`; where the key is **absent**, the compile-time fallback (in parentheses) applies.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 4. `config` Section — Global Runtime Settings

### `config::*::version`
Schema version of the configuration file. Read with `ConfigManager::config_version()`. Used to validate/upgrade the file format. Current: `2.0.0`.

### `config::*::default::blocksize`
Growth granularity, **in bytes**, for [`SharedBuffer`](./../../framework/areg/base/SharedBuffer.hpp) allocations — the unit a buffer grows by, which reduces reallocation and fragmentation as event payloads are serialized.

- Shipped default: `64`.
- `0` → use the framework's built-in default block alignment.
- Accessor: `ConfigManager::buffer_block_size(module)`.

```text
config::*::default::blocksize  = 64    # align buffer growth to 64-byte blocks
```

### `config::*::default::messagequeue`
Default maximum number of events a dispatcher thread's message queue may hold.

- Shipped default: `0`.
- `0` → **no limit** (the queue grows as needed). A positive value caps the queue.
- Accessor: `ConfigManager::message_queue_size(module)`.

```text
config::*::default::messagequeue = 0   # unbounded dispatcher queues (grow on demand)
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 5. `log` Section — Logging

Logging is the largest section. Settings combine a **target** model (where logs go), **layouts** (how file lines are formatted), and **scopes** (which categories log, at what priority). For a task-oriented logging guide see **[Logging Configuration](./04a-logging-config.md)**.

### 5.1 `log::*::version`
Logging configuration schema version (`log_version()`). Current: `2.0.0`.

### 5.2 `log::*::target`
The set of log **targets** the build is aware of, as a `|`-list. Values: `remote`, `file`, `debug`, `db`. Read with `log_targets()`. This advertises the available outputs; whether each is *active* is controlled by the `enable::*` flags below.

```text
log::*::target = remote | file | debug | db
```

### 5.3 `log::*::enable` and `log::*::enable::<target>`

| Key | Meaning |
|---|---|
| `log::*::enable` | **Master** logging switch for the module (`logging_status()`). `false` silences everything. |
| `log::*::enable::remote` | Send logs to the remote log collector over TCP (`log_enabled(LogTarget::Remote)`). |
| `log::*::enable::file` | Write logs to a file (`log_enabled(LogTarget::File)`). |
| `log::*::enable::debug` | Emit to the platform debug console (Windows `OutputDebugString`, etc.). |
| `log::*::enable::db` | Database target — **not implemented** for the core logger; reserved. |

```text
log::*::enable          = true     # master on
log::*::enable::remote  = true     # to log collector
log::*::enable::file    = false    # not to file
log::*::enable::debug   = false
log::*::enable::db      = false
```

Each flag is settable at runtime with `set_log_enabled(target, value, temporary)`.

### 5.4 `log::*::file::location`
Path of the log file when file logging is enabled. Supports **runtime masks** (see [§5.10](#510-file-path-masks)).

```text
log::*::file::location = ./logs/%appname%_%time%.log
```

### 5.5 `log::*::file::append`
`true` → append to an existing log file; `false` → create a fresh file on each start (`log_file_append()`).

### 5.6 Remote logging: `log::*::remote::queue`, `log::*::remote::service`

| Key | Default | Meaning |
|---|---|---|
| `log::*::remote::queue` | `100` | How many messages to buffer while the collector is unreachable. `0` = no queuing (drop/forward immediately). `remote_queue_size()`. |
| `log::*::remote::service` | `logger` | The `service` alias whose block (`logger::*::…`) holds the collector's address/port. |

### 5.7 Database logging: `log::*::db::*`

> **Not implemented for the core file/remote logger** — these keys are reserved/planned. The shipped file leaves them empty for `*`, and uses them only under the `logobserver` module, where SQLite-backed storage *is* available (see [Log Collector](./04d-logcollector.md) / [Log Observer](./04c-logobserver.md)).

| Position | Meaning |
|---|---|
| `engine` | DB engine (`sqlite3`, …) |
| `name` | Database name (supports `%time%` mask, e.g. `log_%time%.sqlog`) |
| `location` | Directory / connection string |
| `driver` | Driver library |
| `address` / `port` | DB server endpoint |
| `username` / `password` | Credentials |

Accessors: `log_database_property(position)` / `set_db_property(position, value)`.

```text
# Example (log observer storing to a local SQLite file):
log::logobserver::enable::db   = true
log::logobserver::db::engine   = sqlite3
log::logobserver::db::name     = log_%time%.sqlog
log::logobserver::db::location = ./logs
```

### 5.8 Layouts: `log::*::layout::{enter,message,exit}`

Layouts format **file** log output (they do not affect remote logging). Three layouts cover the message kinds: scope **enter**, normal **message**, scope **exit**.

```text
log::*::layout::enter   = %d: [ %t  %x.%z: Enter -->]%n
log::*::layout::message = %d: [ %t  %p >>> ] %m%n
log::*::layout::exit    = %d: [ %t  %x.%z: Exit <-- ]%n
```

**Format tokens**

| Token | Meaning | Example |
|------|---------|---------|
| `%a` | Logging object ID (assigned by collector) | `0x1234` |
| `%c` | Tick-count since process start | `123456` |
| `%d` | Date and time (full timestamp) | `2026-06-16 14:32:45` |
| `%e` | Module (process) ID | `5678` |
| `%m` | Message text (mutually exclusive with `%z`) | `Connection established` |
| `%n` | End-of-line (platform newline) | |
| `%p` | Priority | `INFO`, `DEBUG`, `WARN` |
| `%s` | Scope ID (numeric) | `42` |
| `%t` | Thread ID | `0x7f8a` |
| `%x` | Module / process name | `myapp` |
| `%y` | Thread name | `WorkerThread` |
| `%z` | Scope name (mutually exclusive with `%m`) | `myapp_Connection_connect` |

> If a layout is left unset, messages of that kind are not printed. The `logobserver` block in the shipped file uses an extended layout with `%a` to disambiguate sources when aggregating logs.

### 5.9 Log scopes and priorities: `log::*::scope::<pattern>`

A **scope** is a named logging category (typically `module_Class_method`). A scope key assigns one or more **priorities** to a scope or a family of scopes.

```text
log::*::scope::*                  = DEBUG | SCOPE   # all scopes, all apps
log::*::scope::areg_*             = NOTSET          # silence framework-internal scopes
log::myapp::scope::database_*     = INFO            # 'myapp' database scopes at INFO
log::myapp::scope::myapp_Conn_run = DEBUG | SCOPE   # one exact scope
```

**Priorities** (combine with `|`):

| Priority | Logs |
|---|---|
| `DEBUG` | DEBUG, INFO, WARN, ERROR, FATAL |
| `INFO` | INFO, WARN, ERROR, FATAL |
| `WARN` | WARN, ERROR, FATAL |
| `ERROR` | ERROR, FATAL |
| `FATAL` | FATAL only |
| `SCOPE` | Scope enter/exit boundaries (combine with a level, e.g. `DEBUG \| SCOPE`) |
| `NOTSET` | Disabled — silence the scope/group |

**Scope-name resolution** (most specific wins). For a scope `a_b_c_method`, `ScopeController` resolves its priority in this order:

1. **Exact** match — `log::*::scope::a_b_c_method`.
2. **Leaf-method group** (new `.`-form) — `log::*::scope::a_b_c.*`, where `.` separates the path from a snake_case method name so the method may itself contain underscores.
3. **Node groups**, right-to-left on `_` — `a_b_c_*`, then `a_b_*`, then `a_*`.
4. **Global** — `*`.

This means `areg_*` matches every scope beginning with `areg_`, and `*` is the catch-all default. Manage scopes programmatically with `module_log_scopes()`, `add_log_scope(name, prio)`, `remove_scope(name)`.

### 5.10 File path masks

Paths and file names in `file::location` (and DB `name`) expand these at runtime:

| Mask | Expands to |
|---|---|
| `%appname%` | Application/module name |
| `%time%` | Current timestamp |
| `%user%` | User profile directory |

```text
%user%/logs/%appname%_%time%.log     # per-user, timestamped
/var/log/areg/%appname%.log          # absolute, per-app
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 6. `service` / `router` / `logger` — Remote Services

Areg's IPC relies on two infrastructure services: the **message router** (`mtrouter`, routes events between processes) and the **log collector** (`logcollector`, aggregates logs). Each is described by a *service block* referenced by an alias.

### `service::*::list`
The aliases of the remote services available to applications (`service_list()`).

```text
service::*::list = router | logger
```

### Service block shape
For each alias (`router`, `logger`, or your own), the following keys define the endpoint. These use the framework's generic `*::*::{service,connect,enable::*,address::*,port::*}` key shapes, so the same pattern works for any service alias:

| Key | router default | logger default | Meaning |
|---|---|---|---|
| `<alias>::*::service` | `mtrouter` | `logcollector` | Executable/process name of the service |
| `<alias>::*::connect` | `tcpip` | `tcpip` | Supported transport list (`tcpip` only today) |
| `<alias>::*::enable::tcpip` | `true` | `true` | Enable that transport |
| `<alias>::*::address::tcpip` | `localhost` | `localhost` | Host/IP of the service |
| `<alias>::*::port::tcpip` | `8181` | `8282` | TCP port |

```text
router::*::service        = mtrouter
router::*::connect        = tcpip
router::*::enable::tcpip  = true
router::*::address::tcpip = localhost     # 127.0.0.1 — change for a remote router
router::*::port::tcpip    = 8181

logger::*::service        = logcollector
logger::*::address::tcpip = localhost
logger::*::port::tcpip    = 8282
```

Accessors: `remote_service_name()`, `remote_service_connections()`, `remote_service_enable()`, `remote_service_address()`, `remote_service_port()` (and `set_service_*` mutators), addressable either by string alias or by `RemoteServiceKind` / `ConnectionType`. See **[Message Router](./03a-mtrouter.md)** for deployment details.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 7. `net` Section — Network Tuning

The `net` section tunes the TCP/IP transport. The key shape is:

```text
net :: MODULE :: TRANSPORT :: <knob>
```

- **MODULE** — a process name (`mtrouter`, `logcollector`) or `*` for all processes.
- **TRANSPORT** — `tcpip` (future: `udp`, `sm`). `*` matches any transport.

**Lookup order** for every `net` knob: module-specific entry → wildcard `*` entry → compile-time default.

| Knob | Unit | Shipped default | Compile-time fallback | Meaning |
|---|---|---|---|---|
| `sndbuf` | KB | `8192` (8 MB) for `mtrouter`/`*`; `2048` for `logcollector` | 4 MB | Socket `SO_SNDBUF` |
| `rcvbuf` | KB | same as `sndbuf` | 4 MB | Socket `SO_RCVBUF` |
| `drain` | count | `128` | `128` (`DEFAULT_DRAIN_LIMIT`) | Messages drained/sent per dispatcher wake-up |
| `pairs` | count | `0` | `0` | Dedicated send/recv thread-pair pool; `0` = shared threads |
| `timeout` | ms | `2500` | `2500` (`SOCKET_SEND_TIMEOUT_MS`) | `SO_SNDTIMEO` send timeout |
| `cache` | KB | `256` | `256` (`DEFAULT_THREAD_CACHE`) | Per-socket send/recv cache size |

```text
net::mtrouter::tcpip::sndbuf     = 8192    # router 8 MB send buffer (raise for 3 MB+ frames)
net::mtrouter::tcpip::rcvbuf     = 8192
net::logcollector::tcpip::sndbuf = 2048    # log traffic is bursty but small
net::logcollector::tcpip::rcvbuf = 2048
net::*::tcpip::sndbuf            = 8192    # default for all other processes
net::*::tcpip::rcvbuf            = 8192
net::*::tcpip::drain             = 128
net::*::tcpip::pairs             = 0
net::*::tcpip::timeout           = 2500
net::*::tcpip::cache             = 256
```

**Platform notes**

- `sndbuf`/`rcvbuf` are **not applied on Windows** — Windows TCP autotuning is used instead.
- On **Linux**, the kernel **doubles** the requested `SO_SNDBUF`/`SO_RCVBUF` internally; the value you set is the pre-doubling request.
- `timeout` is worth raising (e.g. `30000`) when debugging on Windows so a breakpoint pause does not trip a send-timeout disconnect.

Accessors: `network_sndbuf()`, `network_rcvbuf()`, `network_drain_limit()`, `network_pool_pairs()`, `network_timeout()`, `network_cache()`. See **[Network Tuning Troubleshooting](./07d-troubleshooting-network-tunning.md)**.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 8. Worked Examples / Recipes

### Enable file logging for one application
```text
# Globals keep file logging off; turn it on for 'myapp' only.
log::*::enable::file        = false
log::myapp::enable::file    = true
log::myapp::file::location  = ./logs/myapp_%time%.log
log::myapp::file::append    = false
```

### Quiet the framework, verbose your code
```text
log::*::scope::*            = NOTSET            # silence everything by default
log::myapp::scope::myapp_*  = DEBUG | SCOPE     # full detail for your own scopes
log::myapp::scope::areg_*   = NOTSET            # keep framework internals quiet
```

### Point a process at a router on another host
```text
# Only this process talks to a remote router; others keep the localhost default.
router::myclient::address::tcpip = 192.168.1.50
router::myclient::port::tcpip    = 8181
```

### Tune socket buffers for large (multi-MB) frames
```text
net::mtrouter::tcpip::sndbuf = 16384   # 16 MB
net::mtrouter::tcpip::rcvbuf = 16384
net::myapp::tcpip::sndbuf    = 16384
net::myapp::tcpip::rcvbuf    = 16384
```

### Shared config for several processes
```text
# One file, conservative globals, selective per-app overrides.
log::*::scope::*           = NOTSET
log::*::enable::remote     = true
log::server::scope::*      = INFO | SCOPE
log::worker::scope::*      = WARN
log::mtrouter::scope::*    = NOTSET     # infrastructure stays silent
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 9. Programmatic Access

Every property has a typed accessor on [`ConfigManager`](./../../framework/areg/persist/ConfigManager.hpp), reachable via `Application::config_manager()`:

| Property family | Read | Write |
|---|---|---|
| Versions | `config_version()`, `log_version()` | — |
| Buffer / queue defaults | `buffer_block_size()`, `message_queue_size()` | — |
| Logging master / targets | `logging_status()`, `log_targets()`, `log_enabled()` | `set_logging_status()`, `set_log_enabled()` |
| Log file | `log_file_location()`, `log_file_append()` | `set_file_location()`, `set_file_append()` |
| Remote logging | `remote_queue_size()` | `set_remote_queue_size()` |
| Layouts | `log_layout_enter/message/exit()` | `set_layout_enter/message/exit()` |
| Scopes | `module_log_scopes()` | `add_log_scope()`, `remove_scope()` |
| DB | `log_database_property()` | `set_db_property()` |
| Services | `service_list()`, `remote_service_name/address/port/enable()` | `set_service_address/port/enable()` |
| Network | `network_sndbuf/rcvbuf/drain_limit/pool_pairs/timeout/cache()` | — |

For arbitrary keys (including your own), use the generic `property_value()` / `set_module_property()` calls and pass `temporary = true` for session-only changes. See **[05a §6–7](./05a-persistence-syntax.md#6-reading-and-writing-with-configmanager)**.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 10. Troubleshooting and Gotchas

- **Comment not recognized / line ignored** — comments must be `# ` (hash **+ space**). `#comment` is treated as a blank line. Inline comments need a space too: `port = 8181 # ok`.
- **My module's setting is ignored** — only `*` (global) and entries for the *running* process are loaded. A `log::otherapp::…` line is invisible to every process except `otherapp`.
- **My change wasn't saved** — entries set with `temporary = true` are intentionally excluded from `save_config()`. Also, setting a value *equal* to the read-only default removes the override (override-collapse) — that is expected and keeps the file minimal.
- **`db` logging does nothing** — database logging is not implemented for the core logger; the `log::*::db::*` keys are reserved (the log observer's SQLite storage is separate).
- **Socket buffer size seems wrong on Windows** — `sndbuf`/`rcvbuf` are ignored there (OS autotuning). On Linux the kernel doubles your value.
- **Disconnects while debugging on Windows** — raise `net::*::tcpip::timeout` so a paused breakpoint doesn't trip `SO_SNDTIMEO`.
- **Values as lists** — use `|` for multi-value entries (`DEBUG | SCOPE`, `remote | file`). A trailing `;` is allowed but optional.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Further Reading

- **[Key-Value Data Persistence](./05a-persistence-syntax.md)** — syntax, storage model, and the `ConfigManager` API
- **[Logging Configuration](./04a-logging-config.md)** — scopes and priorities in practice
- **[Message Router (`mtrouter`)](./03a-mtrouter.md)** — IPC routing service
- **[Log Collector](./04d-logcollector.md)** · **[Log Observer](./04c-logobserver.md)** — log aggregation and monitoring
- **[Network Tuning](./07d-troubleshooting-network-tunning.md)** — socket buffer and throughput tuning
- **Default file**: [framework/areg/resources/areg.init](./../../framework/areg/resources/areg.init)
- **Keys source of truth**: [PersistenceDefs.hpp](./../../framework/areg/persist/PersistenceDefs.hpp)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
