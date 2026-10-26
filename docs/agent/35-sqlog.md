# Reading a `.sqlog` file

A `.sqlog` file is an ordinary **SQLite 3 database**. Open it with any SQLite client,
or with Python's built-in `sqlite3` -- no areg tooling is needed to read a log after
the fact.

Two programs write one, with the same schema:

- `logcollector`, when started with `--log=db` or with `log::logcollector::enable::db = true`.
  It records every connected process. This is the one to use.
- `logobserver`, by default (`log::logobserver::enable::db = true`), into
  `./logs/log_<time>.sqlog`. It records what it receives from the collector.

How to configure logging is in `34-logging.md`.

```bash
sqlite3 ./logs/logcollector_20260831_2130.sqlog
```

A running session can be read in place; a reader does not stall the writer.

A complete project that produces one -- two processes, `mtrouter`, `logcollector` and
the `areg.init` that points them at each other -- is `recipes/08-observability/`, and
`query_sqlog.py` beside it reads the file back by process or by scope.

---

## 1. The tables

| Table | One row per |
|---|---|
| `version` | the session that wrote the file |
| `instances` | a process that connected to the collector |
| `scopes` | a scope of a connected process, per priority change |
| `logs` | a log message |
| `markers` | a marker a viewer set on log row `master_id` |

`sqlite_sequence` is SQLite's own and holds no log data.

### `logs` -- the one that matters

| Column | Meaning |
|---|---|
| `id` | integer primary key, in the order the rows were written |
| `cookie_id` | which process, joins `instances.cookie_id` |
| `scope_id` | which scope, joins `scopes.scope_id`; `0` when the row has none |
| `session_id` | one entry into a scope: its `ScopeEnter`, its messages and its `ScopeExit` share it |
| `msg_type` | `1` enter, `2` message, `4` exit (`0` undefined) |
| `msg_prio` | priority **bit flag**, see below |
| `msg_log` | the message text; the scope name on an enter or exit row |
| `msg_module` | executable name |
| `msg_thread` | thread name, empty when none was sent |
| `msg_module_id`, `msg_thread_id`, `msg_len` | process id, thread id, text length |
| `time_created` | when the application logged it |
| `time_received` | when the writer got it |
| `time_duration` | microseconds since scope entry; the whole scope on an exit, `0` on an enter and outside a scope |

Rows arrive in `time_received` order but read correctly only in `time_created` order.
Order by `time_created, id` -- the `id` breaks ties so paging never repeats or drops a row.

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
| `IGNORE` | 1024 | `0x0400` |

Useful combinations: `0x03E0` (992) is every real log priority, `0x03F0` (1008) adds
scope enter/exit. Errors and worse is `96` (`FATAL | ERROR`).

### Rows no application wrote

The writer adds rows of its own. They belong to no process under test, join neither
`instances` nor `scopes`, and have `scope_id` `0`:

| Row | `msg_prio` | `cookie_id` | Written by |
|---|---|---|---|
| `Starting database logging...`, the first row | `1024` | `1` | every writer |
| a note on a connection or on registered scopes | `4080` (`0x0FF0`, every bit) | not an application's | `logobserver` only |
| `Dropped [ N ] log messages, ...` | `64` (`ERROR`) | `1` | a writer that could not keep up |

`4080` holds the error bits too. Application rows only:

```sql
SELECT * FROM logs WHERE (msg_prio & 1024) = 0 AND cookie_id <> 1;
```

A `Dropped [` row means rows are missing: check for it before trusting a count.

`query_sqlog.py` drops all three kinds unless `--collector` asks.

---

## 2. Queries that answer real questions

**Everything, in the order it happened**

```sql
SELECT datetime(l.time_created/1000000,'unixepoch') AS ts,
       l.msg_module, s.scope_name, l.msg_log
FROM logs l LEFT JOIN scopes s ON s.scope_id = l.scope_id AND s.cookie_id = l.cookie_id
WHERE (l.msg_prio & 1024) = 0 AND l.cookie_id <> 1
ORDER BY l.time_created, l.id;
```

`scopes` is joined on **both** `scope_id` and `cookie_id`: scope ids are only unique
within one process, so joining on `scope_id` alone multiplies rows once a second
process connects.

**Errors and worse**

```sql
SELECT datetime(time_created/1000000,'unixepoch'), msg_module, msg_log
FROM logs WHERE (msg_prio & 96) != 0 AND (msg_prio & 1024) = 0
ORDER BY time_created, id;
```

It keeps the `Dropped` row.

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
syntax is in `34-logging.md` section 2.

**A scope's own timeline across processes**

```sql
SELECT l.msg_module, datetime(l.time_created/1000000,'unixepoch'), l.msg_log
FROM logs l JOIN scopes s ON s.scope_id = l.scope_id AND s.cookie_id = l.cookie_id
WHERE s.scope_name = 'myapp_Worker.process_timer'
ORDER BY l.time_created, l.id;
```

Scope names are dot-joined from the `DEF_LOG_SCOPE` arguments -- see `34-logging.md`
section 2.

---

## 3. Extracting for a script

One line, no client needed, tab separated:

```bash
sqlite3 -separator $'\t' logs/session.sqlog \
  "SELECT time_created, msg_module, msg_log FROM logs WHERE (msg_prio & 96) != 0 AND (msg_prio & 1024) = 0 ORDER BY time_created, id;"
```

From Python, with nothing installed:

```python
import sqlite3
db = sqlite3.connect("logs/session.sqlog")
for ts, mod, msg in db.execute(
        "SELECT time_created, msg_module, msg_log FROM logs "
        "WHERE (msg_prio & 96) != 0 AND (msg_prio & 1024) = 0 "
        "ORDER BY time_created, id"):
    print(ts // 1000000, mod, msg)
```

---

## 4. Before you move on

- [ ] Timestamps divided by **1000000**, not 1000.
- [ ] Priority filtered with `&`, not `=`.
- [ ] `scopes` joined on `scope_id` **and** `cookie_id`.
- [ ] Ordered by `time_created, id`, not by `id` alone.
- [ ] Writer rows excluded with `(msg_prio & 1024) = 0 AND cookie_id <> 1`, or counted apart.
- [ ] Checked for a `Dropped [` row before trusting a count.

Schema source of truth, if a column here ever disagrees:
`framework/aregextend/db/private/LogSqliteDatabase.cpp`.
