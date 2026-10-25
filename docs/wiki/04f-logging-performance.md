# Areg SDK Logging Performance: Logs per Second, Latency and Loss

How many log messages areg can deliver per second from an application to a log observer, how long one log call takes, and how to measure it on your own machine with the example [`35_loglatency`](../../examples/35_loglatency/).

> [!NOTE]
> Logging is a debugging and testing aid in areg, not a messaging channel. It never blocks the application: when a queue on the way is full, a log message is dropped. The numbers below show the rates at which nothing is dropped.

---

## Table of Contents

1. [The Short Answer](#the-short-answer)
2. [What Is Measured](#what-is-measured)
3. [Results](#results)
4. [Why Messages Are Lost and What Helps](#why-messages-are-lost-and-what-helps)
5. [Measure It Yourself](#measure-it-yourself)
6. [How the Numbers Were Measured](#how-the-numbers-were-measured)
7. [Limits of These Numbers](#limits-of-these-numbers)
8. [Machine-Readable Summary](#machine-readable-summary)

---

## The Short Answer

| Question | Linux (in WSL2) | Windows 11 |
|---|---|---|
| Logs per second delivered **without loss**, saved in a database | **200 000** (higher not measured) | **50 000** |
| Logs per second delivered without loss, no database | **400 000** | 50 000 (higher not measured) |
| Time of one log call in the application, median | 0.25-0.4 µs (about 1.3 µs when the logging thread is idle) | not measured |
| Time from the log call until the observer has it, median, light load | ~120 µs | ~1.4 ms |

- The **database** is the slowest step: every message becomes one row in a `.sqlog` file.
- One of five runs at 50 000 msg/s with the database lost 17%; the cause is not found yet ([Results](#results)).
- The Linux numbers come from **WSL2**, not from Linux on bare metal. Bare metal is expected to be faster; it is not measured yet.
- Both systems ran on the same laptop: Intel i7-13700H, 20 threads.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## What Is Measured

One log message travels this way:

```text
application ──TCP──► logcollector ──TCP──► log observer ──► .sqlog database (optional)
 (LOG_DBG call)       (forwards)          (35_logsink, same library as logobserver and Lusan)
```

- **Log call:** the time `LOG_DBG` takes in the application thread. This is what logging costs the application.
- **Trip time:** the time from the `LOG_DBG` call until the observer receives the message. With the database on, the time includes the database write.
- **Delivered / lost:** how many messages arrived at the observer, out of how many were written.
- **Rate:** messages per second that arrived at the observer.

**One message** is the text plus a fixed part with time stamps, ids, the thread name and the module name:

| Part | Size |
|---|---|
| Text of the test message, for example `msg alpha bravo charlie 4936 1024 65535` | ~41 bytes |
| One message on the network, per hop | ~350 bytes |
| One row in the `.sqlog` database, with indexes | ~207 bytes |

At 150 000 messages per second, that is about **52 MB/s** on each TCP hop and about **31 MB/s** written to the database.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Results

A **steady rate** means the application writes a fixed number of messages every millisecond, for 5 seconds or more. "Lossless" means every message arrived.

### Linux (Ubuntu 26.04 in WSL2), with the database

| Steady rate, msg/s | Delivered | Trip P50 | Trip P99 |
|---:|---|---:|---:|
| 50 000 | 100% in 4 of 5 runs; one run lost 17%, not repeated since | ~220 µs | ~4 ms |
| 100 000 | 100% | 480 µs | 4.6 ms |
| 150 000 | 100% (4 runs) | 0.36-0.40 ms | 8-10 ms |
| 200 000 | 100% (4 runs) | 0.8-1.6 ms | 11-12 ms |

Higher rates with the database are not measured yet. The database file is written in WAL mode; every few thousand rows SQLite moves the log into the file, and that commit takes about 10 ms instead of well under 1 ms. These pauses set the trip P99 with the database.

### Linux (Ubuntu 26.04 in WSL2), without the database

| Steady rate, msg/s | Delivered | Trip P50 | Trip P99 |
|---:|---|---:|---:|
| 100 000 | 100% | 189 µs | 604 µs |
| 200 000 | 99.9% | 258 µs | 1.4 ms |
| 400 000 | 100% | 242 µs | 905 µs |
| ~790 000 | 94.7% (arrive at ~745 000 msg/s) | 0.9 ms | 8 ms |

One message per millisecond (a quiet system): trip P50 ~117 µs, P99 ~340 µs.

### Windows 11

| Steady rate, msg/s | Database | Delivered | Trip P50 |
|---:|---|---|---:|
| 50 000 | off | 100% | ~1.4 ms |
| 50 000 | on | 100% | ~2.0 ms (P99 ~14 ms) |
| full speed | on | ~20%, the observer drains ~270 000 msg/s | 13-17 ms |
| 100 000 | off or on | 84% | -- |

The loss at 100 000 msg/s on Windows comes from the test, not from the database. A Windows timer fires about every 16 ms, so the test application writes 1 600 messages at once. That is more than the 1 024 messages the logging queue holds.

### The time of one log call (Linux in WSL2)

| Load | P50 | P99 |
|---|---:|---:|
| 50 000-400 000 msg/s, steady | 0.24-0.39 µs | 0.8-2.6 µs |
| One message per millisecond | ~1.3 µs | -- |

A log call is faster under load. At a low rate the logging thread is asleep, and the application thread pays about 1 µs to wake it up.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Why Messages Are Lost and What Helps

A log message is dropped in two cases:

1. **The rate stays above what the slowest step can do.** With the database on, the slowest step is the observer writing the `.sqlog` file. A bigger buffer does not help here: it only fills up later.
2. **A short burst is larger than the logging queue** (1 024 messages) of the application. The Windows case above is an example.

What helps today:

- **Log less in hot loops.** Lower the priority of busy scopes, or switch them off in `areg.init` ([Logging Configuration](./04a-logging-config.md)). Lusan can change scope priorities while the application runs ([Live Logging](./06f-lusan-live-logging.md)).
- **Put the `.sqlog` file on a fast local disk.** The database rate depends mostly on how fast the disk confirms a write.
- **Do not raise `log::*::remote::queue` for this.** That queue only keeps messages while the log collector is not connected yet, or after the connection is lost. It does not change the loss during normal work.

At 50 000 messages per second and below, nothing was lost on Windows, nor on Linux without the database. On Linux with the database, one of five runs at 50 000 lost 17%. The cause is not found yet.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Measure It Yourself

Build areg with the examples ([Examples and Tests](./08a-examples-and-tests.md)), go to the `bin/` directory of the build, and use three terminals. Executables end in `.elf` on Linux, `.exe` on Windows, `.mac` on macOS.

**1. Start the log collector:**
```bash
./logcollector.elf --console
```

**2. Start the observer (sink).** `-c` is the number of messages to expect; `-d` saves them in a database file:
```bash
./35_logsink.elf -c=750000 -d=test.sqlog
```

**3. When the sink prints `connected`, start the application (source).** `-p=1 -b=150` writes 150 messages every millisecond, which is 150 000 per second; `-c=750000` runs for 5 seconds; `-w=0` sends no warm-up messages:
```bash
./35_logsource.elf -c=750000 -w=0 -p=1 -b=150
```

**Read the result** in the sink: `received` and `lost` say whether the rate was lossless, `rate` is how fast the messages arrived, `log trip` is the trip time. The source prints the time of one log call.

| What you want to know | Source options | Sink options |
|---|---|---|
| Highest steady rate with a database | `-p=1 -b=<n>`, `-c=<5000*n>`, `-w=0` | `-c=<same> -d=<file>` |
| Highest steady rate without a database | the same | `-c=<same>` |
| Trip time of a quiet system | `-p=1 -b=1 -c=10000` | `-c=11000` (adds the 1 000 warm-up messages) |
| Cost of a log call at full speed | `-c=100000` | `-c=101000` |

On Windows a timer fires about every 16 ms, so use a longer period: `-p=10 -b=800` holds about 50 000 messages per second. The source prints the rate it really reached. All options: [example ReadMe](../../examples/35_loglatency/ReadMe.md).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## How the Numbers Were Measured

- **Machine:** laptop, Intel i7-13700H, 20 threads, otherwise idle.
  - Linux: Ubuntu 26.04 in WSL2 (kernel 6.18), files on the ext4 disk of WSL2.
  - Windows: Windows 11 (build 26300), MSVC x64.
- **Build:** Release, examples on, default `areg.init`.
- **Processes:** `logcollector`, `35_logsink` and `35_logsource` on the same machine, connected over TCP `localhost`. One application, one thread writing logs.
- **Message:** `msg alpha bravo charlie <counter> 1024 65535`, the default of `35_logsource` (`-s=3 -n=3`).
- **Steady rate:** an areg timer in the source fires every period and writes the burst (`-b`) each time. A run lasts 5 seconds or more.
- **Log call time:** measured around each `LOG_DBG` call with `std::chrono::steady_clock`. The source prints the cost of one clock reading; each sample contains it.
- **Trip time:** the arrival time at the sink minus the time stamp the log call wrote into the message. Both processes read the same system clock.
- **Rate at the sink:** messages received divided by the time from the first to the last arrival.
- **Lost:** expected messages minus received messages. The sink stops when all messages arrived, or 3 seconds after the last one.
- **Database:** the sink saves every message in a `.sqlog` file the same way `logobserver` does, in WAL mode. It commits every 1 000 messages or 100 ms, whichever comes first.
- **Repeats:** each rate was run at least once; the rates near a limit were run 2 to 5 times, and the tables give the range.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Limits of These Numbers

- **Linux was measured in WSL2, not on bare metal.** WSL2 runs Linux in a virtual machine, with a virtual disk and virtual network. Bare-metal Linux on the same CPU is expected to be faster, the database rate especially. A bare-metal measurement is planned.
- **The Windows rows with the database were measured with the applications started from PowerShell, output to files; the other Windows rows from a script through `cmd.exe`, output to pipes.** The two methods are not compared.
- **One machine, `localhost`.** A real network adds its own delay. At ~350 bytes per message, a 100 Mbit/s link carries at most about 35 000 messages per second, and 1 Gbit/s about 357 000.
- **One application and one writing thread.** Several applications logging at the same time are not measured.
- **macOS is not measured.**

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Machine-Readable Summary

One row per measured case. `delivered_pct` is a range when the case was run more than once. `trip_*_us` is empty when it was not recorded.

```csv
platform,environment,database,offered_msg_s,delivered_pct,arrival_msg_s,trip_p50_us,trip_p99_us,lossless
linux,wsl2,on,50000,83-100,41429-50006,209-245,3975-4817,4 of 5 runs
linux,wsl2,on,100000,100,100000,480,4622,yes
linux,wsl2,on,150000,100,149972-150023,363-395,8443-10361,yes
linux,wsl2,on,200000,100,199887-200033,759-1607,11203-12050,yes
linux,wsl2,off,1000,100,1000,117,340,yes
linux,wsl2,off,100000,100,100000,189,604,yes
linux,wsl2,off,200000,99.9,199829,258,1428,no
linux,wsl2,off,400000,100,400066,242,905,yes
linux,wsl2,off,786000,94.7,744678,937,8044,no
windows,windows11,off,50000,100,50000,1400,,yes
windows,windows11,on,50000,100,49643-50781,1900-2044,13837-45598,yes
windows,windows11,on,full,18-21,263845-279434,13037-16706,35650-40808,no
windows,windows11,off,100000,84,,,,no
```

```text
message_size_wire_bytes = 350      # per TCP hop: 116 header + 192 fixed entry + ~42 text
message_size_db_bytes   = 207      # per .sqlog row, indexes included
log_call_p50_ns         = 240-390  # steady load, Linux in WSL2
logging_queue_messages  = 1024     # per application; full queue drops, never blocks
db_commit               = 1000 messages or 100 ms
cpu                     = Intel i7-13700H, 20 threads
build                   = Release
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>
