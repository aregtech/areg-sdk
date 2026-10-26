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
6. [Method and Limits](#method-and-limits)
7. [Machine-Readable Summary](#machine-readable-summary)

---

## The Short Answer

| Question | Linux (in WSL2) | Windows 11 |
|---|---|---|
| Logs per second delivered **without loss**, saved in a database | **200 000** (higher not measured) | **250 000** from 5 applications; the database takes up to ~270 000 |
| Logs per second delivered without loss, no database | **400 000** | 50 000 (100 000 from one application lost 16%) |
| Time of one log call in the application, median | 0.25-0.4 µs (about 1.3 µs when the logging thread is idle) | not measured |
| Time from the log call until the observer has it, median, light load | ~120 µs | ~1.4 ms |

- The **database** is the slowest step: every message becomes one row in a `.sqlog` file.
- Linux was measured in **WSL2**, not on bare metal, and with an older database writer than Windows. Both on the same laptop: Intel i7-13700H, 20 threads.

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

| Size of one message | Bytes |
|---|---|
| Text of the test message, for example `msg alpha bravo charlie 4936 1024 65535` | ~41 |
| On the network, per hop | ~350 |
| One row in the `.sqlog` database, with indexes | ~207 |

At 150 000 messages per second, that is about **52 MB/s** on each TCP hop and about **31 MB/s** written to the database.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Results

A **steady rate** means the application writes a fixed number of messages every period, for 5 seconds or more. "Lossless" means every message arrived.

### Linux (Ubuntu 26.04 in WSL2), with the database

| Steady rate, msg/s | Delivered | Trip P50 | Trip P99 |
|---:|---|---:|---:|
| 50 000 | 100% in 4 of 5 runs; one run lost 17%, cause not found | ~220 µs | ~4 ms |
| 100 000 | 100% | 480 µs | 4.6 ms |
| 150 000 | 100% (4 runs) | 0.36-0.40 ms | 8-10 ms |
| 200 000 | 100% (4 runs) | 0.8-1.6 ms | 11-12 ms |

Every few thousand rows SQLite moves its write-ahead log into the file; that commit takes about 10 ms and sets the trip P99.

### Linux (Ubuntu 26.04 in WSL2), without the database

| Steady rate, msg/s | Delivered | Trip P50 | Trip P99 |
|---:|---|---:|---:|
| 1 000 | 100% | 117 µs | 340 µs |
| 100 000 | 100% | 189 µs | 604 µs |
| 200 000 | 99.9% | 258 µs | 1.4 ms |
| 400 000 | 100% | 242 µs | 905 µs |
| ~790 000 | 94.7% (arrive at ~745 000 msg/s) | 0.9 ms | 8 ms |

### Windows 11, with the database

Several applications log at the same time, each about 50 000 messages per second, for 30 seconds. **Per hop** is the data on each TCP connection; **Disk** is what the observer writes (database and write-ahead log).

| Applications | Steady rate, msg/s | Saved | Per hop | Trip P50 | Trip P99 | Disk |
|---:|---:|---|---:|---:|---:|---:|
| 1 | 50 500 | 99.98% (371 of 1.5 million lost before the observer, cause not found) | 18 MB/s | 1.3 ms | 6.6 ms | 17 MB/s |
| 2 | 100 800 | 100% | 35 MB/s | 1.2 ms | 6.3 ms | 36 MB/s |
| 3 | 151 000 | 100% | 53 MB/s | 1.3 ms | 7.3 ms | 53 MB/s |
| 4 | 201 100 | 100% | 70 MB/s | 1.5 ms | 7.5 ms | 74 MB/s |
| 5 | 251 200 | **100% (3 runs)** | 88 MB/s | 1.5-1.6 ms | 6.7 ms | 92-93 MB/s |
| 6 | ~301 000 | 88-91%, saved 265 000-273 000 msg/s (3 runs) | 105 MB/s | 1.8 ms | 7.6-8.1 ms | 91-96 MB/s |

- **The limit is the database writer**, one thread in the observer: 53% of a core at 151 000 msg/s, 88% at 251 000, 96% at 301 000. Above ~270 000 msg/s it drops the rows it cannot save, counts them, and writes a row `Dropped [ N ] log messages` into the file. Its queue absorbs bursts of a few seconds.
- Lusan as the observer saved the same rates without loss (2-5 applications, up to 252 000 msg/s) with the live window open.
- On long runs the drive's sustained write speed becomes the limit: this laptop's drive wrote ~1 GB/s for the first ~9 GB, then 40-75 MB/s.

### Windows 11, without the database

| Steady rate, msg/s | Delivered | Trip P50 |
|---:|---|---:|
| 50 000 | 100% | ~1.4 ms |
| 100 000, one application | 84% | -- |

The loss at 100 000 msg/s came from the test: the source was then paced by a timer that fired only every ~16 ms on Windows, so it wrote 1 600 messages at once, more than the 1 024 the logging queue holds.

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

1. **The rate stays above what the slowest step can do.** With the database on, that is the observer writing the `.sqlog` file. A bigger buffer does not help: it only fills up later.
2. **A burst is larger than the logging queue** of the application (1 024 messages).

What helps:

- **Log less in hot loops.** Lower the priority of busy scopes, or switch them off in `areg.init` ([Logging Configuration](./04a-logging-config.md)). Lusan can change scope priorities while the application runs ([Live Logging](./06f-lusan-live-logging.md)).
- **Put the `.sqlog` file on a fast local disk.** The database rate depends mostly on how fast the disk confirms a write.
- **Do not raise `log::*::remote::queue` for this.** That queue only keeps messages while the log collector is not connected yet, or after the connection is lost.

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

**Read the result** in the sink: `received` and `lost` say whether the rate was lossless, `rate` is how fast the messages arrived, `log trip` is the trip time. The source prints the time of one log call and the rate it reached.

| What you want to know | Source options | Sink options |
|---|---|---|
| Highest steady rate, with or without a database | `-p=1 -b=<n>`, `-c=<5000*n>`, `-w=0` | `-c=<same>`, plus `-d=<file>` for the database |
| Trip time of a quiet system | `-p=1 -b=1 -c=10000` | `-c=11000` (adds the 1 000 warm-up messages) |
| Cost of a log call at full speed | `-c=100000` | `-c=101000` |

For more load, start several sources at once and give the sink their sum. All options: [example ReadMe](../../examples/35_loglatency/ReadMe.md).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Method and Limits

- **Machine:** laptop, Intel i7-13700H, 20 threads, otherwise idle, on mains power.
  - Linux: Ubuntu 26.04 in WSL2 (kernel 6.18), files on the ext4 disk of WSL2. WSL2 is a virtual machine with a virtual disk and network; bare-metal Linux is expected to be faster, the database rate especially. Not measured yet.
  - Windows: Windows 11 (build 26300), MSVC x64. Database rows: sources started together from PowerShell, output to files, the sink pinned to the performance cores, a fresh `.sqlog` on the internal NVMe drive per run. Rows without the database: an older build, started from `cmd.exe` with output to pipes.
  - macOS: not measured.
- **Build:** Release, examples on, default `areg.init`.
- **Processes:** `logcollector`, `35_logsink` and `35_logsource` on one machine over TCP `localhost`, one writing thread per application. A real network adds its own delay: at ~350 bytes per message, 100 Mbit/s carries at most ~35 000 messages per second, 1 Gbit/s ~357 000.
- **Message:** `msg alpha bravo charlie <counter> 1024 65535`, the default of `35_logsource` (`-s=3 -n=3`).
- **Log call time:** measured around each `LOG_DBG` call with `std::chrono::steady_clock`; each sample contains one clock reading, whose cost the source prints.
- **Trip time:** arrival time at the sink minus the time stamp the log call wrote into the message. Both processes read the same clock.
- **Rate at the sink:** messages received divided by the time from the first to the last arrival.
- **Lost:** expected minus received. The sink stops when all messages arrived, or 3 seconds after the last one.
- **Database:** the sink saves every message the way `logobserver` does, in WAL mode. Linux rows: commit every 1 000 messages or 100 ms. Windows rows: a writer thread of its own commits up to 16 384 messages or every 100 ms.
- **Repeats:** each rate at least once; rates near a limit 2 to 5 times, and the tables give the range.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Machine-Readable Summary

One row per measured case. `delivered_pct` is a range when the case was run more than once. `trip_*_us` is empty when it was not recorded. The Windows rows with `database=on` come from 1 to 6 applications at ~50 000 msg/s each; `offered_msg_s` is their sum.

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

Constants behind the numbers: log call P50 240-390 ns (steady load, Linux in WSL2); logging queue 1 024 messages per application, a full queue drops and never blocks; database writer limit on Windows 265 000-273 000 msg/s.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>
