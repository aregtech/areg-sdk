# 35_loglatency

**Type:** Logging / Multi-Process

## Overview

**35_loglatency** measures the cost of AREG SDK logging along the whole path:
the application thread that writes a log, the log collector, and a log observer.

- **Log call** -- `35_logsource` times each `LOG_DBG` call on the writing thread.
  This is the time logging takes away from the application.
- **Log trip** -- `35_logsink` is a log observer. For every message it receives, it
  records the time from the `LOG_DBG` call in the source until the message
  arrives. Both processes read the same machine clock.
- **Delivered and lost** -- logging never blocks the application. When a queue on
  the way is full, a message is dropped. The sink reports how many arrived.

Two load profiles:

- **As fast as possible** (default): each source thread writes its messages in
  a loop. Shows the shortest call time and the highest rate, and where messages
  start to drop.
- **Paced** (`-p=<ms>`, `-b=<n>`): a timer writes `n` messages per period. Shows
  the call time and the trip time of a system that is not saturated, and whether
  a steady rate is delivered without loss.

With `-d=<file>` the sink also saves every message in a database file, the way
`logobserver` does, so the numbers include the database write of a log observer.

Measured results and the method: [Logging Performance](../../docs/wiki/04f-logging-performance.md).

> [!NOTE]
> This example requires the **Log Collector (`logcollector`)** running before
> starting the sink or the source.

---

## Message Format

The message text is `msg` followed by up to 5 strings and up to 5 numbers,
chosen with `-s` and `-n`. With the defaults (`-s=3 -n=3`) a message reads
`msg alpha bravo charlie 1 1024 65535`. The text is sent with its real length;
the log entry adds the fixed part with the time stamps, the thread and the
module name.

---

## Run

Three terminals are required. Start them in this order.

> [!NOTE]
> All binaries are placed in the build output `bin/` directory. `cd` into that
> directory first, or run each binary with its full path. Executable extensions
> are `.exe` on Windows, `.elf` on Linux, `.mac` on macOS.

**Terminal 1 -- Start `logcollector`:**
```bash
./logcollector.elf --console
```

**Terminal 2 -- Start the sink** with the number of messages to expect
(the measured ones plus the warm-up, for every thread):
```bash
./35_logsink.elf -c=101000
```

**Terminal 3 -- Start the source:**
```bash
./35_logsource.elf -c=100000 -w=1000
```

When the sink prints `connected`, start the source. The sink ends when the
expected number of messages arrived, or 3 seconds after the last one.

---

## Options

**35_logsource**

| Option | Default | Meaning |
|:-------|:-------:|:--------|
| `-c=<n>` | 100000 | messages each thread measures |
| `-w=<n>` | 1000 | messages each thread writes before it measures |
| `-t=<n>` | 1 | threads that write at the same time (ignored with `-p`) |
| `-s=<n>` | 3 | strings in a message, 0 to 5 |
| `-n=<n>` | 3 | numbers in a message, 0 to 5 |
| `-p=<ms>` | 0 | a timer writes the messages, one burst per period; 0 writes as fast as possible |
| `-b=<n>` | 1 | messages in each burst of `-p`; `-p=1 -b=50` holds 50 000 messages per second |

**35_logsink**

| Option | Default | Meaning |
|:-------|:-------:|:--------|
| `-c=<n>` | 0 | messages that end the run; 0 waits until the messages stop |
| `-d=<file>` | none | also save every message in this database file, as `logobserver` does; an existing file is replaced |

---

## Output

```text
35 logsource | threads 1 | strings 3 | numbers 3 | period 0 ms | burst 1 | messages 100000 | clock 17.5 ns
35 log call  | P50 ... ns | P90 ... ns | P99 ... ns | P99.9 ... ns | max ... ns | mean ... ns | rate ... msg/s
```

```text
35 logsink   | received ... | lost ... | rate ... msg/s | database off
35 log trip  | P50 ... us | P90 ... us | P99 ... us | P99.9 ... us | max ... us | mean ... us
```

- `clock` is the cost of one clock reading; every call sample contains it.
- `rate` of the source is how fast the threads wrote; `rate` of the sink is how
  fast the messages arrived.
- `lost` is the expected count minus the received count.
- At full speed the trip time is mostly time spent waiting in queues. Use `-p`
  for the trip time of a single message.
- On Windows the clock steps by 100 ns, and a timer period is at least about
  16 ms, so `-p=1` writes about 60 bursts per second there; `-p=10 -b=800` holds
  50 000 messages per second. The source prints the rate it reached.
- A burst larger than about 1000 messages can overflow the queue of the logging
  thread, which drops messages instead of blocking the application.

The scope `examples_35_logsource_*` must be enabled for `DEBUG` in `areg.init`,
which is the default configuration.
