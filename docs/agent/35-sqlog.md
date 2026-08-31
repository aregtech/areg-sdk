# Reading a `.sqlog` file

A `.sqlog` file is an ordinary **SQLite 3 database**. `logcollector` writes one when
it is started with `--log=db`; nothing else produces it. Open it with any SQLite
client, or with Python's built-in `sqlite3` -- no areg tooling is needed to read a
log after the fact.

How to produce one, and the rest of the logging configuration, is in `34-logging.md`.

```bash
sqlite3 ./logs/logcollector_20260831_2130.sqlog
```

Copy the file before querying a session that is still running: the collector holds it
open, and a reader that locks it can stall the writer.

---

## 1. The four tables

A file at rest has exactly four tables. Two more, `filter_rules` and
`refused_spans`, are `CREATE TEMP TABLE` -- they belong to a live viewer and vanish
with the connection, so **never expect them in a file** and never join against them.

| Table | One row per |
|---|---|
| `version` | the collector session that wrote the file |
| `instances` | a process that connected to the collector |
| `scopes` | a scope of a connected process, per priority change |
| `logs` | a log message |

### `logs` -- the one that matters

| Column | Meaning |
|---|---|
| `id` | autoincrement primary key; ties to `time_created` for stable paging |
| `cookie_id` | which process, joins `instances.cookie_id` |
| `scope_id` | which scope, joins `scopes.scope_id` |
| `session_id` | pairs a `ScopeEnter` with its `ScopeExit` in the same scope |
| `msg_type` | `1` enter, `2` message, `4` exit (`0` undefined) |
| `msg_prio` | priority **bit flag**, see below |
| `msg_module_id` | process id on its own machine |
| `msg_thread_id` | thread id |
| `msg_log` | the message text |
| `msg_len` | its length |
| `msg_thread` | thread name |
| `msg_module` | executable name |
| `time_created` | when the application logged it |
| `time_received` | when the collector got it |
| `time_duration` | microseconds the scope took; set on exit, `0` otherwise |

`time_created` is the interesting one: rows arrive in `time_received` order but read
correctly only in `time_created` order, which is why `idx_logs_time` exists. Order by
`time_created, id` -- the `id` breaks ties so paging never repeats or drops a row.

### Timestamps

All `time_*` columns are **microseconds since the Unix epoch**, not seconds and not
milliseconds. SQLite's date functions need seconds:

```sql
SELECT datetime(time_created / 1000000, 'unixepoch') AS ts,
       (time_created % 1000000) AS usec
FROM logs LIMIT 5;
```

Dividing by 1000 gives dates in the year 57000 and is the usual first mistake.

### Priorities are bit flags

`msg_prio` holds one bit, so filtering uses `&`, not `=` or `>=`.

| Priority | Value | Hex |
|---|---|---|
| `NOTSET` | 1 | `0x0001` |
| `SCOPE` | 16 | `0x0010` |
| `FATAL` | 32 | `0x0020` |
| `ERROR` | 64 | `0x0040` |
| `WARNING` | 128 | `0x0080` |
| `INFO` | 256 | `0x0100` |
| `DEBUG` | 512 | `0x0200` |

Useful combinations: `0x03E0` (992) is every real log priority, `0x03F0` (1008) adds
scope enter/exit and is the viewer's default mask. Errors and worse is `96`
(`FATAL | ERROR`).

---

## 2. Queries that answer real questions

**Everything, in the order it happened**

```sql
SELECT datetime(l.time_created/1000000,'unixepoch') AS ts,
       l.msg_module, s.scope_name, l.msg_log
FROM logs l LEFT JOIN scopes s ON s.scope_id = l.scope_id AND s.cookie_id = l.cookie_id
ORDER BY l.time_created, l.id;
```

`scopes` is joined on **both** `scope_id` and `cookie_id`: scope ids are only unique
within one process, so joining on `scope_id` alone multiplies rows once a second
process connects.

**Errors and worse**

```sql
SELECT datetime(time_created/1000000,'unixepoch'), msg_module, msg_log
FROM logs WHERE (msg_prio & 96) != 0 ORDER BY time_created, id;
```

**What one process logged**

```sql
SELECT datetime(l.time_created/1000000,'unixepoch'), l.msg_log
FROM logs l JOIN instances i ON i.cookie_id = l.cookie_id
WHERE i.inst_name = 'myapp' ORDER BY l.time_created, l.id;
```

**Which processes took part, and how long each stayed**

```sql
SELECT cookie_id, inst_name, inst_location,
       datetime(time_connected/1000000,'unixepoch') AS connected,
       CASE WHEN time_disconnected IS NULL THEN 'still connected'
            ELSE datetime(time_disconnected/1000000,'unixepoch') END AS disconnected
FROM instances ORDER BY time_connected;
```

A `NULL` `time_disconnected` means the process never said goodbye -- it is still
running, or it crashed. That distinction is often the answer on its own.

**The slowest scope calls** (needs `SCOPE` priority enabled when logging)

```sql
SELECT s.scope_name, l.time_duration/1000.0 AS ms, l.msg_module
FROM logs l JOIN scopes s ON s.scope_id = l.scope_id AND s.cookie_id = l.cookie_id
WHERE l.msg_type = 4 AND l.time_duration > 0
ORDER BY l.time_duration DESC LIMIT 20;
```

**Where the noise is**

```sql
SELECT s.scope_name, COUNT(*) AS n
FROM logs l JOIN scopes s ON s.scope_id = l.scope_id AND s.cookie_id = l.cookie_id
GROUP BY s.scope_name ORDER BY n DESC LIMIT 20;
```

Feed the top rows back into `areg.init` as `NOTSET` to quieten them; the scope-key
syntax is in `34-logging.md` section 3.

**A scope's own timeline across processes**

```sql
SELECT l.msg_module, datetime(l.time_created/1000000,'unixepoch'), l.msg_log
FROM logs l JOIN scopes s ON s.scope_id = l.scope_id AND s.cookie_id = l.cookie_id
WHERE s.scope_name = 'myapp_Worker.process_timer'
ORDER BY l.time_created, l.id;
```

Scope names are dot-joined from the `DEF_LOG_SCOPE` arguments -- see `34-logging.md`
section 3.

---

## 3. Extracting for a script

One line, no client needed, tab separated:

```bash
sqlite3 -separator $'\t' logs/session.sqlog \
  "SELECT time_created, msg_module, msg_log FROM logs WHERE (msg_prio & 96) != 0 ORDER BY time_created, id;"
```

From Python, with nothing installed:

```python
import sqlite3
db = sqlite3.connect("logs/session.sqlog")
for ts, mod, msg in db.execute(
        "SELECT time_created, msg_module, msg_log FROM logs "
        "WHERE (msg_prio & 96) != 0 ORDER BY time_created, id"):
    print(ts // 1000000, mod, msg)
```

---

## 4. Before you move on

- [ ] The file came from `logcollector --log=db`; `logobserver` does not write one.
- [ ] Timestamps divided by **1000000**, not 1000.
- [ ] Priority filtered with `&`, not `=`.
- [ ] `scopes` joined on `scope_id` **and** `cookie_id`.
- [ ] Ordered by `time_created, id`, not by `id` alone.
- [ ] No query references `filter_rules` or `refused_spans`; they are not in the file.

Schema source of truth, if a column here ever disagrees:
`framework/aregextend/db/private/LogSqliteDatabase.cpp`.
