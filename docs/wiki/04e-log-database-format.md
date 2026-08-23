# Areg SDK Log Database Format (`.sqlog`)

A `.sqlog` file is a plain **SQLite 3 database** that holds recorded Areg SDK logs. Three components write and read this format, so a file produced by one of them can always be opened by the others.

> [!TIP]
> There is nothing special about the file. Any SQLite client opens it: the `sqlite3` command line tool, a GUI browser, Python, or your own program.

---

## Table of Contents

1. [Who Writes and Reads the File](#1-who-writes-and-reads-the-file)
2. [Quick Start](#2-quick-start)
3. [Table `version`](#3-table-version)
4. [Table `instances`](#4-table-instances)
5. [Table `scopes`](#5-table-scopes)
6. [Table `logs`](#6-table-logs)
7. [Value Reference](#7-value-reference)
8. [Timestamps](#8-timestamps)
9. [Ready Made Queries](#9-ready-made-queries)
10. [Notes and Limits](#10-notes-and-limits)

---

## 1. Who Writes and Reads the File

| Component | Role | How to switch recording on |
|---|---|---|
| `logcollector` | Writes everything it collects from all connected applications. | `log::logcollector::enable::db = true`, or `--log=db`, or the console command `--save`. See [Log Collector](./04d-logcollector.md#saving-collected-logs-in-a-database). |
| `logobserver` | Writes the logs it receives from the collector. | `log::logobserver::enable::db = true`. See [Log Observer](./04c-logobserver.md). |
| Lusan | Writes while connected live, and reads files for offline analysis. | See [Lusan Offline Logging](./06g-lusan-offline-logging.md). |

All three use the same writer, so the schema below is identical in every case.

**Which one should record?**

| Situation | Recommended writer |
|---|---|
| Logs must be kept even when nobody is watching | `logcollector` |
| High log rate, many messages per second | `logcollector`, it writes in batched transactions |
| Only one session, from a workstation | `logobserver` or Lusan |
| Analysis with a GUI right after recording | Lusan |

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 2. Quick Start

```bash
# What is in the file?
sqlite3 ./logs/logcollector_2026_08_18_11_20_35_120.sqlog ".tables"
# instances  logs  scopes  sqlite_sequence  version
# (sqlite_sequence is created by SQLite itself, it is not part of the log data)

# Which applications were recorded?
sqlite3 ./logs/logcollector_2026_08_18_11_20_35_120.sqlog \
        "SELECT cookie_id, inst_name FROM instances;"

# The last 20 messages
sqlite3 -header -column ./logs/logcollector_2026_08_18_11_20_35_120.sqlog \
        "SELECT msg_module, msg_thread, msg_log FROM logs ORDER BY id DESC LIMIT 20;"
```

**Schema overview**

```
version     one row, describes the file itself
   │
instances   one row per recorded application (identified by cookie_id)
   │
   ├── scopes   the log scopes of an application, and their priority over time
   │
   └── logs     the log messages themselves
```

`cookie_id` is the join key. It is the ID that the log collector assigns to an application when it connects.
`scope_id` joins `logs` to `scopes`, and is unique **per application**, not globally.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 3. Table `version`

Describes the file. Written once, when the file is created.

| Column | Type | Content |
|---|---|---|
| `name` | TEXT | File name of the process that created the file, including the extension, for example `logcollector.exe`. |
| `version` | TEXT | Schema version of the log data. Currently `3.0.0`. |
| `describe` | TEXT | Fixed description text. |
| `created_by` | TEXT | Fixed creator text. |
| `db_name` | TEXT | Full path of the file at the moment it was created. |
| `time_created` | NUMERIC | When the file was created. |
| `time_closed` | NUMERIC | Reserved. **Always `NULL`**, the writers do not fill it in. |

```sql
SELECT name, version, datetime(time_created / 1000000, 'unixepoch') AS created FROM version;
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 4. Table `instances`

One row per application that was connected while the recording ran. A new row is added every time an application connects, so an application that reconnects appears more than once, each time with a different `cookie_id`.

| Column | Type | Content |
|---|---|---|
| `cookie_id` | INTEGER | ID assigned to the application by the log collector. The join key of the file. |
| `inst_connect` | INTEGER | `1` while connected, set to `0` on disconnect. |
| `inst_type` | INTEGER | Kind of the application, see [Value Reference](#7-value-reference). |
| `inst_bits` | INTEGER | Bitness of the process: `32`, `64`, or `0` if unknown. |
| `inst_name` | TEXT | Name of the application, for example `myapp`. |
| `inst_location` | TEXT | Path of the executable. |
| `time_connected` | NUMERIC | Timestamp reported by the application itself when it connected. |
| `time_disconnected` | NUMERIC | When the disconnect was recorded. `NULL` while still connected, and for every application that was still running when the recording stopped. |
| `time_updated` | NUMERIC | When the writer recorded the row. |

> [!NOTE]
> An open `time_disconnected` is normal. It means the application outlived the recording, not that something failed.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 5. Table `scopes`

The log scopes of every application, and the priority each scope had. A scope that changes priority produces a **new row**; the previous row is marked inactive. This keeps the history of the whole session.

| Column | Type | Content |
|---|---|---|
| `scope_id` | INTEGER | ID of the scope. Unique per application, not across applications. |
| `cookie_id` | INTEGER | The application that owns the scope. |
| `scope_is_active` | INTEGER | `1` while this row is the current priority of the scope, `0` once it was replaced or the application disconnected. |
| `scope_prio` | INTEGER | Priority bit mask, see [Value Reference](#7-value-reference). |
| `scope_name` | TEXT | Full scope name, for example `myapp_Component_start`. |
| `time_received` | NUMERIC | When this priority became effective. |
| `time_inactivated` | NUMERIC | When it was replaced. `NULL` while active. |

Unique constraint: `scope_id` + `cookie_id` + `time_received`.

```sql
-- Current scope priorities of every application
SELECT cookie_id, scope_name, scope_prio FROM scopes WHERE scope_is_active = 1 ORDER BY cookie_id, scope_name;
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 6. Table `logs`

The log messages. This is the table that grows.

| Column | Type | Content |
|---|---|---|
| `id` | INTEGER | Primary key, auto increment. Row order equals write order. |
| `cookie_id` | INTEGER | The application that produced the message. |
| `scope_id` | INTEGER | The scope that produced the message. `0` when the message has no scope. |
| `session_id` | INTEGER | Session ID of the message. Set only for remote logs. |
| `msg_type` | INTEGER | Scope enter, message, or scope exit. See [Value Reference](#7-value-reference). |
| `msg_prio` | INTEGER | Priority of the message. See [Value Reference](#7-value-reference). |
| `msg_module_id` | INTEGER | Process ID of the application. |
| `msg_thread_id` | INTEGER | Thread ID that produced the message. |
| `msg_log` | TEXT | The message text. For a scope enter or exit row this is the scope name. |
| `msg_thread` | TEXT | Name of the thread. |
| `msg_module` | TEXT | Name of the application. |
| `time_created` | NUMERIC | When the application produced the message. |
| `time_received` | NUMERIC | When the recorder received it. The difference to `time_created` is the transport delay. |
| `time_duration` | NUMERIC | For a scope exit row, the time spent in the scope, in microseconds. `0` otherwise. |

Index: `idx_logs` on `(scope_id, msg_prio, cookie_id)`.

> [!IMPORTANT]
> `msg_module`, `msg_thread` and `cookie_id` always identify the **application that produced the log**, never the recorder. A log written by `myapp` keeps the name `myapp` in a file written by `logcollector`.

**The first row** of every file is written by the recorder itself and reads `Starting database logging...`, with priority `1024` (ignore). Filter it out when you count messages.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 7. Value Reference

### Message priority: `logs.msg_prio` and `scopes.scope_prio`

The value is a **bit mask**. A scope priority usually combines a level with the scope boundary bit, for example `DEBUG | SCOPE` is `512 + 16 = 528`.

| Name | Hex | Decimal | Meaning |
|---|---|---|---|
| `NOTSET` | `0x0001` | 1 | Logging disabled for the scope |
| `SCOPE` | `0x0010` | 16 | Scope enter and exit boundaries |
| `FATAL` | `0x0020` | 32 | Fatal error |
| `ERROR` | `0x0040` | 64 | Error |
| `WARN` | `0x0080` | 128 | Warning |
| `INFO` | `0x0100` | 256 | Information |
| `DEBUG` | `0x0200` | 512 | Debug |
| *(ignore)* | `0x0400` | 1024 | Internal message of the recorder, not produced by an application |

```sql
-- Errors and fatals only
SELECT msg_module, msg_log FROM logs WHERE (msg_prio & 96) != 0;   -- 96 = FATAL | ERROR
```

### Message type: `logs.msg_type`

| Value | Meaning |
|---|---|
| `0` | Undefined |
| `1` | Scope enter. `msg_log` holds the scope name. |
| `2` | Log message. `msg_log` holds the text. |
| `4` | Scope exit. `msg_log` holds the scope name, `time_duration` holds the time spent inside. |

### Application kind: `instances.inst_type`

| Value | Meaning |
|---|---|
| `0` | Undefined |
| `1` | Client application, the usual case |
| `2` | System service |
| `4` | Observer or viewer |
| `8` | Test application |
| `16` | Simulation application |

A file written by `logcollector` contains only log sources, that is values `1`, `8` and `16`.

### Bitness: `instances.inst_bits`

`32`, `64`, or `0` when unknown.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 8. Timestamps

Every `time_*` column holds **microseconds since the Unix epoch (1 January 1970, UTC)**. This is not the SQLite default of seconds, so a conversion is always needed:

```sql
-- Readable date and time, to the second
SELECT datetime(time_created / 1000000, 'unixepoch') FROM logs LIMIT 5;

-- Readable, with milliseconds, in local time
SELECT strftime('%Y-%m-%d %H:%M:%f', time_created / 1000000.0, 'unixepoch', 'localtime') FROM logs LIMIT 5;
```

```python
# Python
import datetime
readable = datetime.datetime.fromtimestamp(time_created / 1_000_000, datetime.timezone.utc)
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 9. Ready Made Queries

**How many messages per application**

```sql
SELECT i.inst_name, COUNT(*) AS messages
FROM logs AS l JOIN instances AS i ON l.cookie_id = i.cookie_id
GROUP BY i.inst_name
ORDER BY messages DESC;
```

**Full text of one application, in order, with a readable time**

```sql
SELECT datetime(l.time_created / 1000000, 'unixepoch') AS time, l.msg_thread, l.msg_log
FROM logs AS l JOIN instances AS i ON l.cookie_id = i.cookie_id
WHERE i.inst_name = 'myapp'
ORDER BY l.id;
```

**All errors and warnings, with the scope that produced them**

```sql
SELECT l.msg_module, s.scope_name, l.msg_log
FROM logs AS l LEFT JOIN scopes AS s
     ON l.scope_id = s.scope_id AND l.cookie_id = s.cookie_id AND s.scope_is_active = 1
WHERE (l.msg_prio & 224) != 0          -- 224 = FATAL | ERROR | WARN
ORDER BY l.id;
```

**The slowest scope calls**

```sql
SELECT msg_module, msg_log AS scope, time_duration AS microseconds
FROM logs
WHERE msg_type = 4 AND time_duration > 0
ORDER BY time_duration DESC
LIMIT 20;
```

**Transport delay between the application and the recorder**

```sql
SELECT msg_module,
       MAX(time_received - time_created) AS worst_microseconds,
       AVG(time_received - time_created) AS average_microseconds
FROM logs
GROUP BY msg_module;
```

**Were messages lost?**

```sql
SELECT msg_log FROM logs WHERE msg_log LIKE 'Dropped%';
```

A row here means the recorder could not keep up with the incoming rate and says how many messages it gave up. An empty result means nothing was dropped.

**When was each application connected**

```sql
SELECT inst_name,
       datetime(time_connected / 1000000, 'unixepoch')    AS connected,
       datetime(time_disconnected / 1000000, 'unixepoch') AS disconnected
FROM instances
ORDER BY time_connected;
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 10. Notes and Limits

- **The file is a normal SQLite database.** You may copy, compress, or archive it. Copy it only while the recorder is stopped, or use `sqlite3 <file> ".backup copy.sqlog"`.
- **`scope_id` is unique per application only.** Always join `scopes` on `scope_id` **and** `cookie_id`. Joining on `scope_id` alone mixes the scopes of different applications.
- **A `filter_rules` table may appear** while a file is open in Lusan or the log observer. It is a temporary working table for view filtering and is not part of a closed file.
- **No retention or rotation is built in.** The file grows until the recorder is stopped. Because the shipped file names contain the `%time%` mask, each start creates a new file, which is the intended way to keep files small.
- **Reading while writing is allowed**, but a reader sees only what the writer has already committed. The log collector commits in batches, so the newest messages may be a moment behind.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Related Guides:**
- [Log Collector Guide](./04d-logcollector.md) - Recording on the collector side
- [Log Observer Guide](./04c-logobserver.md) - Recording on the observer side
- [Lusan Offline Logging](./06g-lusan-offline-logging.md) - Reading the file in a GUI
- [Logging Configuration Guide](./04a-logging-config.md) - Scopes, priorities, and targets
- [Areg Configuration Reference](./05b-areg-configuration-file.md) - The `log::*::db::*` properties

**Source Code:**
- [LogSqliteDatabase](../../framework/aregextend/db/) - The writer used by all components
- [logcollector Module](../../framework/logcollector/) - Collector side implementation

**Help:**
For questions, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech (Artak Avetyan), www.areg.tech, email: info[at]areg.tech
