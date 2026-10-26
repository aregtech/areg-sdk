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
| Logs per second delivered **without loss**, saved in a database | **200 000** (higher not measured) | **250 000** from 5 applications (~88 MB/s per hop); the database takes up to ~270 000 |
| Logs per second delivered without loss, no database | **400 000** | 50 000 (higher not measured) |
| Time of one log call in the application, median | 0.25-0.4 µs (about 1.3 µs when the logging thread is idle) | not measured |
| Time from the log call until the observer has it, median, light load | ~120 µs | ~1.4 ms |

- The **database** is the slowest step: every message becomes one row in a `.sqlog` file.
- One of five runs at 50 000 msg/s with the database lost 17%; the cause is not found yet ([Results](#results)).
- The Linux numbers come from **WSL2**, not from Linux on bare metal. Bare metal is expected to be faster; it is not measured yet.
- Both systems ran on the same laptop: Intel i7-13700H, 20 threads.
- The Windows database numbers are from a newer build (2026-10-26) that saves on its own writer thread with larger transactions and fewer indexes. The Linux numbers are from an older build and are not re-measured yet.

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

### Windows 11, with the database

Several applications log at the same time; each writes about 50 000 messages per second (`-p=10 -b=800`) for 30 seconds. **Per hop** is the data on each TCP connection at ~350 bytes per message; **Disk** is what the observer writes to the drive (database and its write-ahead log).

| Applications | Steady rate, msg/s | Saved | Per hop | Trip P50 | Trip P99 | Disk |
|---:|---:|---|---:|---:|---:|---:|
| 1 | 50 500 | 99.98% (371 of 1.5 million did not reach the observer, 1 run) | 18 MB/s | 1.3 ms | 6.6 ms | 17 MB/s |
| 2 | 100 800 | 100% | 35 MB/s | 1.2 ms | 6.3 ms | 36 MB/s |
| 3 | 151 000 | 100% | 53 MB/s | 1.3 ms | 7.3 ms | 53 MB/s |
| 4 | 201 100 | 100% | 70 MB/s | 1.5 ms | 7.5 ms | 74 MB/s |
| 5 | 251 200 | **100% (3 runs)** | 88 MB/s | 1.5-1.6 ms | 6.7 ms | 92-93 MB/s |
| 6 | ~301 000 | 88-91%, saved 265 000-273 000 msg/s (3 runs) | 105 MB/s | 1.8 ms | 7.6-8.1 ms | 91-96 MB/s |

- **The limit is the database writer**, one thread in the observer: it uses 53% of a core at 151 000 msg/s, 88% at 251 000 and 96% at 301 000. Above ~270 000 msg/s the observer drops the rows it cannot save, **counts them**, and writes a row `Dropped [ N ] log messages` into the file. Its queue holds up to 256 MB, which absorbs bursts of a few seconds.
- The same rates were also saved without loss by Lusan as the observer (2-5 applications, up to 252 000 msg/s), with the live window open.
- These runs write a fresh file. On long runs the drive's sustained write speed becomes the limit: this laptop's drive wrote ~1 GB/s for the first ~9 GB, then 40-75 MB/s. At ~90 MB/s of disk writes, that is the case after a few minutes at the top rates.
- The single-application loss came before the observer (its queue dropped nothing). The cause is not found yet; it was not seen with 2 to 6 applications.

### Windows 11, without the database

| Steady rate, msg/s | Delivered | Trip P50 |
|---:|---|---:|
| 50 000 | 100% | ~1.4 ms |
| 100 000, one application | 84% | -- |

These rows are from the older build. The loss at 100 000 msg/s from one application comes from the test, not from the observer. A Windows timer fires about every 16 ms, so the test application writes 1 600 messages at once. That is more than the 1 024 messages the logging queue holds. Use several applications for higher rates, as in the table above.

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

On Windows nothing was lost with the database up to 250 000 messages per second from 5 applications; one run with a single application missed 371 of 1.5 million. On Linux nothing was lost without the database at 50 000 and below; with the database, one of five runs at 50 000 lost 17%. The causes of these two are not found yet.

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

On Windows a timer fires about every 16 ms, so use a longer period: `-p=10 -b=800` holds about 50 000 messages per second. For higher rates start several sources at once and give the sink their sum (`-c=<sources x count>`); 5 sources write about 250 000 per second. The source prints the rate it really reached. All options: [example ReadMe](../../examples/35_loglatency/ReadMe.md).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## How the Numbers Were Measured

- **Machine:** laptop, Intel i7-13700H, 20 threads, otherwise idle.
  - Linux: Ubuntu 26.04 in WSL2 (kernel 6.18), files on the ext4 disk of WSL2.
  - Windows: Windows 11 (build 26300), MSVC x64, on mains power. The database rows: 2026-10-26 build, 1 to 6 sources started together from PowerShell, output to files, the sink pinned to the performance cores, the `.sqlog` file on the internal NVMe drive, fresh for every run.
- **Build:** Release, examples on, default `areg.init`.
- **Processes:** `logcollector`, `35_logsink` and `35_logsource` on the same machine, connected over TCP `localhost`. One thread writing logs per application; one application on Linux, 1 to 6 on Windows with the database.
- **Message:** `msg alpha bravo charlie <counter> 1024 65535`, the default of `35_logsource` (`-s=3 -n=3`).
- **Steady rate:** an areg timer in the source fires every period and writes the burst (`-b`) each time. A run lasts 5 seconds or more.
- **Log call time:** measured around each `LOG_DBG` call with `std::chrono::steady_clock`. The source prints the cost of one clock reading; each sample contains it.
- **Trip time:** the arrival time at the sink minus the time stamp the log call wrote into the message. Both processes read the same system clock.
- **Rate at the sink:** messages received divided by the time from the first to the last arrival.
- **Lost:** expected messages minus received messages. The sink stops when all messages arrived, or 3 seconds after the last one.
- **Database:** the sink saves every message in a `.sqlog` file the same way `logobserver` does, in WAL mode. In the Linux rows it commits every 1 000 messages or 100 ms, whichever comes first. In the Windows database rows a writer thread of its own commits up to 16 384 messages or every 100 ms.
- **Repeats:** each rate was run at least once; the rates near a limit were run 2 to 5 times, and the tables give the range.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Limits of These Numbers

- **Linux was measured in WSL2, not on bare metal.** WSL2 runs Linux in a virtual machine, with a virtual disk and virtual network. Bare-metal Linux on the same CPU is expected to be faster, the database rate especially. A bare-metal measurement is planned.
- **The Windows rows with the database were measured with the applications started from PowerShell, output to files; the Windows rows without the database from a script through `cmd.exe`, output to pipes, with an older build.** The two methods are not compared.
- **Windows rows are runs of 30 seconds on a fresh file.** Long runs at the top rates are limited by the drive's sustained write speed (see the Windows results).
- **One machine, `localhost`.** A real network adds its own delay. At ~350 bytes per message, a 100 Mbit/s link carries at most about 35 000 messages per second, and 1 Gbit/s about 357 000.
- **One writing thread per application.** On Linux one application was measured; on Windows 1 to 6 at the same time.
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
windows,windows11,off,100000,84,,,,no
windows,windows11,on,50511,99.98,50511,1305,6621,no
windows,windows11,on,100802,100,100802,1153,6250,yes
windows,windows11,on,150990,100,150990,1334,7329,yes
windows,windows11,on,201114,100,201114,1512,7495,yes
windows,windows11,on,251300,100,251161-251374,1540-1556,6716-6744,yes
windows,windows11,on,301000,88-91,264500-273400,1798-1845,7602-8093,no
```

The Windows rows with `database=on` come from 1 to 6 applications at ~50 000 msg/s each (`offered_msg_s` is their sum), build of 2026-10-26.

```text
message_size_wire_bytes = 350      # per TCP hop: 116 header + 192 fixed entry + ~42 text
message_size_db_bytes   = 207      # per .sqlog row, indexes included
log_call_p50_ns         = 240-390  # steady load, Linux in WSL2
logging_queue_messages  = 1024     # per application; full queue drops, never blocks
db_commit               = 1000 messages or 100 ms     # Linux rows
db_commit_windows       = 16384 messages or 100 ms    # Windows database rows, own writer thread
db_writer_limit_windows = 265000-273000 msg/s         # above it rows are dropped and counted
cpu                     = Intel i7-13700H, 20 threads
build                   = Release
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>
