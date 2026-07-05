# 30_publatency

**Type:** IPC / Multi-Process

## Overview

**30_publatency** measures the real-world latency of AREG SDK's IPC stack across
two communication patterns and a range of payload sizes.

- **Ping-Pong (RTT)** -- Consumer sends a timestamped request; provider echoes it back.
  RTT = *t4 - t1* on the same consumer clock. Covers the full path: serialization,
  TCP loopback, `mtrouter` routing, thread dispatch, and deserialization on both sides.

- **Broadcast (One-Way)** -- Provider sends timestamped messages; consumer records
  delivery latency (*t4 - t1*). Both processes share the same machine hardware clock --
  no clock synchronization needed.

All measurements run over the full AREG SDK service stack: service discovery,
automatic reconnection, type-safe IPC, and built-in threading are all active.
**What you measure here is what your production system gets.**

> [!NOTE]
> This example requires **Multi-Target Router (`mtrouter`)** running before
> starting the provider or consumer.

---

## Message Format

Every message carries a fixed base header regardless of mode.
The suffix `n` in mode names (`pp64`, `bc128`, etc.) denotes **additional bytes as parameters**.

| Mode        | Extra payload | Request total | Response total |
|:------------|:-------------:|:-------------:|:--------------:|
| `pp0`/`bc0` | 0 B           | 140 B         | 148 B / N/A    |
| `pp64`/`bc64`   | 64 B      | 204 B         | 212 B / N/A    |
| `pp128`/`bc128` | 128 B     | 268 B         | 276 B / N/A    |
| `pp256`/`bc256` | 256 B     | 396 B         | 404 B / N/A    |
| `pp512`/`bc512` | 512 B     | 652 B         | 660 B / N/A    |

Base header: 128 (message header) + 4 (method ID) + 8 (sequence) = **140 bytes**.

---

## Build

Build with logging disabled to eliminate background thread noise from measurements:

```bash
cmake -B ./build -DCMAKE_BUILD_TYPE=Release -DAREG_COMPILER_FAMILY=gnu -DAREG_LOGGING=OFF
cmake --build ./build
```

> [!NOTE]
> Executable extensions differ by platform: `.exe` on Windows, `.elf` on Linux,
> `.mac` on macOS.

---

## Run

Three terminals are required. Start them in this order.

> [!NOTE]
> All binaries are placed in the build output `bin/` directory, e.g.
> `product/build/gnu-g++/linux-64-x86_64-release-shared/bin/` (the exact path
> depends on compiler, platform, and library type). `cd` into that directory
> first, or run each binary with its full path.

**Terminal 1 -- Start `mtrouter`:**
```bash
./mtrouter.elf          # Linux
./mtrouter.mac          # macOS
.\mtrouter.exe          # Windows
```

**Terminal 2 -- Start the provider:**
```bash
./30_pubprovider.elf    # Linux
./30_pubprovider.mac    # macOS
.\30_pubprovider.exe    # Windows
```

**Terminal 3 -- Start the consumer:**
```bash
./30_pubconsumer.elf    # Linux
./30_pubconsumer.mac    # macOS
.\30_pubconsumer.exe    # Windows
```

### Running Tests

All test parameters are set interactively in the consumer console.
**Parameters persist between runs** -- set them once, then use `-s` (or `-s=N`) to start runs.

Recommended session for reproducible results (8 consecutive runs):

```
# Set parameters once:
-m=pp64 -c=10000 -w=1000

# Then run 8 cycles with the default 1000 ms pause between cycles:
-s=8
```

The consumer displays a scrolling table of the last 8 results.
For broadcast (one-way) tests:

```
-m=bc64 -c=5000 -w=1000
-s
...
```

**Three ways to run**, chosen by which of `-c` / `-d` you give on the start line:

- `-c=<N>` only -- **count-bounded**: send at maximum rate, stop after `N` measured samples.
- `-d=<ms>` only -- **duration-bounded**: send at maximum rate for `N` **milliseconds** of measured
  time, then report how many samples ran.
- `-c=<N>` **and** `-d=<ms>` together -- **paced**: send exactly `N` messages spread across `<ms>`
  milliseconds, i.e. a target rate of `N / <ms>` msg/ms. Example: `-c=1000 -d=1000` ≈ 1 msg/ms
  (≈ 1000 msg/s). Stops by count, like `-c` alone.

In **every** mode the run reports the **actual measured duration** (`Dur(ms)`, derived from the
sample timestamps -- not the requested value -- so it is exact despite OS timer jitter) and the
**throughput** (`Msg/s`) in the live summary, the results table, and the CSV. Latency sampling is
never perturbed: the count/duration stop reuses a timestamp already taken on the receive path (no
extra clock read), and the paced delay is a one-shot timer that waits **between** messages -- it sits
entirely outside the measured round-trip, so it changes the spacing of sends, never the latency of a
sample.

Pacing is **best-effort**: the timer resolution is whole milliseconds, and if the requested rate is
faster than the link can actually sustain (the natural rate is the ceiling) the run simply proceeds
at maximum rate. Either way the reported `Dur(ms)` / `Msg/s` reflect what really happened. Examples:

```
-m=pp64 -d=2000 -w=1000          (duration: run ping-pong for 2 s, then read off the round-trip count)
-s
...
-m=pp64 -c=2000 -d=2000 -w=500   (paced: 2000 messages over 2000 ms = ~1000 msg/s)
-s
```

By default, the CSV output is disabled. Enable it with `-f` (optionally `-f=<file>`),
and use `-o=<path>` to choose the output file path.

For benchmark sessions across parameter changes:

```
-b
-m=bc0 -c=5000 -w=1000
-s=10
-m=pp0 -c=10000 -w=1000
-s=10 -p=2000
-e
```

`-e` (or `end`) exports all collected run summaries into a single CSV file.

---

## How It Works

### Ping-Pong (RTT)

```
30_pubconsumer              mtrouter            30_pubprovider
      |                          |                        |
      |-- request_ping_pong ---->|-- TCP send() --------->|  t1 = consumer send
      |   (id, t1)               |                        |
      |                          |<-- response_ping_pong--|
      |<-- TCP send() -----------|   (id, t1, t2)         |
      |                          |                        |  t4 = consumer receive
      |                          |                        |
      |   RTT   = t4 - t1  (exact: same consumer clock)   |
```

The consumer sends one request at a time. The full 4-hop path (Client->Router->Service->Router->Client)
is measured in every RTT sample.

### Broadcast (One-Way)

```
30_pubprovider              mtrouter            30_pubconsumer
      |                          |                        |
      |-- broadcast_message ---->|-- TCP send() --------->|  t1 = provider send
      |   (id, t1)               |                        |  t4 = consumer receive
      |                          |                        |
      |   one-way = t4 - t1  (shared machine hardware clock)
```

---

## Consumer Commands

| Command        | Values           | Description                                              |
|:---------------|:-----------------|:---------------------------------------------------------|
| `-m=<mode>`    | See modes below  | Test mode: ping-pong or broadcast, with payload size     |
| `-c=<N>`       | > 0              | Sample count. Alone: **count-bounded** run (collect exactly `N` samples at max rate). With `-d`: paced run of `N` messages |
| `-d=<ms>`      | > 0              | Duration in **milliseconds**. Alone: **duration-bounded** run (max rate for `N` ms). With `-c`: **paced** run -- `-c` messages spread across `N` ms (target rate = `-c`/`N` msg/ms, best-effort) |
| `-w=<N>`       | >= 0             | Warmup count -- excluded from statistics                 |
| `-s[=N]`       | `N > 0`          | Start test with current settings (`-s` == `-s=1`, `-s=0` == `-s=1`) |
| `-p=<ms>`      | `ms >= 0`        | Pause between batch cycles (`-s=N`)                      |
| `-p`           | --               | Stop running test                                        |
| `-i`           | --               | Show current settings                                    |
| `-o=<path>`    | path             | Set the CSV output file path (does not enable writing)   |
| `-f[=<file>]`  | path (optional)  | Enable CSV file output; optionally set the file name     |
| `-b` / `begin` | --       | Start benchmark collection mode (store completed runs in memory) |
| `-e` / `end`   | --       | Stop benchmark mode and export all collected runs        |
| `-h`           | --               | Show help / redraw the console layout                    |
| `-q`           | --               | Quit consumer and provider                               |
| `-q=1`         | --               | Quit consumer only                                       |

**Available modes:**

| RTT modes | Extra payload | | OWT modes | Extra payload |
|:----------|:-------------:|-|:----------|:-------------:|
| `pp0`     | 0 B           | | `bc0`     | 0 B           |
| `pp8`     | 8 B           | | `bc8`     | 8 B           |
| `pp16`    | 16 B          | | `bc16`    | 16 B          |
| `pp32`    | 32 B          | | `bc32`    | 32 B          |
| `pp64`    | 64 B          | | `bc64`    | 64 B          |
| `pp128`   | 128 B         | | `bc128`   | 128 B         |
| `pp256`   | 256 B         | | `bc256`   | 256 B         |
| `pp512`   | 512 B         | | `bc512`   | 512 B         |
| `pp1024`  | 1 024 B       | | `bc1024`  | 1 024 B       |
| `pp4096`  | 4 096 B       | | `bc4096`  | 4 096 B       |
| `pp65536` | 65 536 B      | | `bc65536` | 65 536 B      |

---

## Benchmark Results

> Test configuration: `count=10000 warmup=1000` for RTT (`ppX`); `count=5000 warmup=1000` for OWT (`bcX`).
> Values in **μs** (microseconds). Each row is the average of 8 consecutive runs.
> Build: `Release`, `AREG_LOGGING=OFF`. Linux figures: `mtrouter`, provider, and consumer
> each pinned to a dedicated physical core via `taskset`.
>
> Raw output of every run behind these numbers, including the exact `taskset` commands:
> → **[benchmark-test-results.txt](../../docs/wiki/benchmark-test-results.txt)**
>
> Full methodology, timestamp placement, and interpretation:
> → **[areg-sdk Performance Benchmarks](../../docs/wiki/08b-areg-sdk-performance-benchmarks.md)**
> → **[areg-sdk vs ZMQ / NanoMsg / NNG](../../docs/wiki/08c-areg-vs-hitachi-benchmark.md)**
> → **[areg-sdk Framework Rankings](../../docs/wiki/08d-areg-framework-rankings.md)**

### Cross-Platform Summary

> bc64 = 204 B one-way (OWT). pp64 = 204+212 B round-trip (RTT). Full service stack active on all platforms.

| Platform | OWT Min | OWT P50 | RTT Min | RTT P50 | RTT P99 |
|:---------|:-------:|:-------:|:-------:|:-------:|:-------:|
| **Linux** ¹ | **10.5 μs** | **~11.2 μs** | **21.2 μs** | **~21.7 μs** | ~23.5 μs |
| **macOS M3 Pro** | 21.6 μs | 31.4 μs | 46.0 μs | 62.5 μs | 78.3 μs |
| **Windows 11** | 32.5 μs | 40.3 μs | 64.0 μs | 82.5 μs | 107.8 μs |

> ¹ Same i7-13700H / 32 GB DDR4 laptop as Windows, Ubuntu 26.04, **Performance** power mode,
> cores pinned via `taskset`. Unpinned, Linux latency is ~30% higher.
> macOS: Apple M3 Pro, 32 GB LPDDR5. All three platforms tested on native SSD.
>
> Latency is **payload-insensitive up to 4 KB**: Linux Min increases only 1.1 μs over a
> 20× size range (bc64 to bc4096).
>
> For comparison: gRPC C++ sequential RTT **~116–167 μs** over Unix domain socket (no service dispatch).
> Source: [MPI-HD, F. Werner, 2021](https://www.mpi-hd.mpg.de/personalhomes/fwerner/research/2021/09/grpc-for-ipc/)

---

### Linux – Ubuntu 26.04, Intel i7-13700H (mobile), 32 GB DDR4

> **Performance** power mode, native SSD, cores pinned via `taskset`. Values are averages
> across 8 consecutive runs per mode. Min and P50 are tight and stable across runs.

#### Ping-Pong RTT (μs)

| Mode    | Total msg size       | Min    | P50    | P95    | P99    | Mean   |
|:--------|:--------------------:|:------:|:------:|:------:|:------:|:------:|
| `pp0`   | 140 / 148 B          | 21.0   | 21.4   | 22.0   | 23.0   | 21.5   |
| `pp8`   | 148 / 156 B          | 21.1   | 21.4   | 22.0   | 23.0   | 21.5   |
| `pp16`  | 156 / 164 B          | 21.0   | 21.5   | 22.2   | 23.2   | 21.6   |
| `pp32`  | 172 / 180 B          | 21.1   | 21.6   | 22.3   | 23.2   | 21.7   |
| `pp64`  | 204 / 212 B          | 21.2   | 21.7   | 22.5   | 23.5   | 22.0   |
| `pp128` | 268 / 276 B          | 21.2   | 21.8   | 22.8   | 23.8   | 22.0   |
| `pp256` | 396 / 404 B          | 21.8   | 22.3   | 23.3   | 24.2   | 22.4   |
| `pp512` | 652 / 660 B          | 22.3   | 22.9   | 24.0   | 24.9   | 23.1   |
| `pp1024`| 1 164 / 1 172 B      | 22.4   | 22.9   | 24.1   | 25.1   | 23.1   |
| `pp4096`| 4 236 / 4 244 B      | 23.4   | 24.5   | 26.1   | 26.9   | 24.7   |
| `pp65536`| 65 676 / 65 684 B   | 62.6   | 65.5   | 68.7   | 70.9   | 65.3   |

#### Broadcast One-Way (μs)

| Mode      | Total msg size | Min    | P50   | P95   | P99   | Mean  |
|:----------|:--------------:|:------:|:-----:|:-----:|:-----:|:-----:|
| `bc0`     | 140 B          | 10.4   | 10.7  | 10.9  | 11.9  | 10.8  |
| `bc8`     | 148 B          | 10.5   | 10.9  | 11.1  | 12.3  | 11.0  |
| `bc16`    | 156 B          | 10.4   | 11.0  | 11.3  | 13.3  | 11.1  |
| `bc32`    | 172 B          | 10.4   | 11.1  | 11.5  | 16.3  | 11.3  |
| `bc64`    | 204 B          | 10.5   | 11.2  | 11.8  | 21.4  | 11.5  |
| `bc128`   | 268 B          | 10.5   | 11.3  | 12.1  | 25.4  | 11.7  |
| `bc256`   | 396 B          | 10.6   | 11.5  | 13.2  | 30.5  | 12.3  |
| `bc512`   | 652 B          | 11.0   | 11.8  | 13.0  | 28.1  | 12.3  |
| `bc1024`  | 1 164 B        | 11.0   | 11.9  | 20.3  | 31.8  | 13.1  |
| `bc4096`  | 4 236 B        | 11.6   | 12.8  | 29.5  | 40.7  | 15.4  |
| `bc65536` | 65 676 B       | 28.8   | 29.9  | 32.7  | 34.6  | 30.2  |

---

### Windows 11 – Intel i7-13700H (mobile), 32 GB DDR4

> Native SSD install. Results are stable – no bimodal distribution.

#### Ping-Pong RTT (μs)

| Mode      | Total msg size       | Min    | P50    | P95    | P99    | Mean   |
|:----------|:--------------------:|:------:|:------:|:------:|:------:|:------:|
| `pp0`     | 140 / 148 B          | 63.0   | 81.9   | 84.6   | 104.9  | 81.9   |
| `pp32`    | 172 / 180 B          | 63.5   | 82.0   | 84.8   | 108.0  | 82.3   |
| `pp64`    | 204 / 212 B          | 64.0   | 82.5   | 85.2   | 107.8  | 82.5   |
| `pp128`   | 268 / 276 B          | 64.5   | 82.5   | 89.3   | 133.0  | 83.5   |
| `pp256`   | 396 / 404 B          | 64.5   | 82.5   | 85.0   | 108.8  | 82.5   |
| `pp512`   | 652 / 660 B          | 65.0   | 84.1   | 87.0   | 113.5  | 84.5   |
| `pp1024`  | 1 164 / 1 172 B      | 65.3   | 83.0   | 85.8   | 112.5  | 83.5   |
| `pp4096`  | 4 236 / 4 244 B      | 69.1   | 85.3   | 89.6   | 114.1  | 86.7   |
| `pp65536` | 65 676 / 65 684 B    | 115.6  | 142.5  | 204.8  | 242.9  | 152.8  |

#### Broadcast One-Way (μs)

| Mode      | Total msg size | Min    | P50    | P95    | P99    | Mean   |
|:----------|:--------------:|:------:|:------:|:------:|:------:|:------:|
| `bc0`     | 140 B          | 32.0   | 39.5   | 41.9   | 57.1   | 40.1   |
| `bc64`    | 204 B          | 32.5   | 40.3   | 43.3   | 60.0   | 41.2   |
| `bc128`   | 268 B          | 33.0   | 40.5   | 43.5   | 53.6   | 40.5   |
| `bc256`   | 396 B          | 33.4   | 41.6   | 46.3   | 72.0   | 42.2   |
| `bc512`   | 652 B          | 33.5   | 41.4   | 46.0   | 68.6   | 42.0   |
| `bc1024`  | 1 164 B        | 33.8   | 41.8   | 45.8   | 60.4   | 42.9   |
| `bc4096`  | 4 236 B        | 36.4   | 46.7   | 49.8   | 67.4   | 46.1   |
| `bc65536` | 65 676 B       | 57.6   | 72.8   | 76.9   | 104.3  | 73.7   |

---

### macOS – Apple M3 Pro, 32 GB LPDDR5 (native SSD)

> P99 consistency is exceptional: bc65536 P99 spread = 0.8 μs across 8 runs.

#### Ping-Pong RTT (μs)

| Mode      | Total msg size       | Min    | P50    | P95    | P99    |
|:----------|:--------------------:|:------:|:------:|:------:|:------:|
| `pp0`     | 140 / 148 B          | 46.4   | 63.3   | 74.8   | 78.1   |
| `pp64`    | 204 / 212 B          | 46.0   | 62.5   | 74.6   | 78.3   |
| `pp128`   | 268 / 276 B          | 48.1   | 65.4   | 75.7   | 79.0   |
| `pp256`   | 396 / 404 B          | 47.8   | 66.2   | 76.6   | 80.0   |
| `pp512`   | 652 / 660 B          | 48.8   | 69.5   | 78.0   | 81.0   |
| `pp1024`  | 1 164 / 1 172 B      | 47.9   | 65.5   | 75.7   | 79.0   |
| `pp4096`  | 4 236 / 4 244 B      | 49.5   | 70.3   | 78.6   | 81.8   |
| `pp65536` | 65 676 / 65 684 B    | 82.1   | 95.9   | 102.2  | 106.0  |

#### Broadcast One-Way (μs)

| Mode      | Total msg size | Min    | P50    | P95    | P99    |
|:----------|:--------------:|:------:|:------:|:------:|:------:|
| `bc0`     | 140 B          | 21.7   | 31.7   | 38.3   | 41.3   |
| `bc64`    | 204 B          | 21.6   | 31.4   | 37.8   | 40.6   |
| `bc128`   | 268 B          | 22.4   | 32.9   | 38.3   | 41.2   |
| `bc512`   | 652 B          | 22.8   | 34.3   | 39.5   | 42.9   |
| `bc1024`  | 1 164 B        | 23.0   | 33.4   | 39.3   | 42.6   |
| `bc4096`  | 4 236 B        | 23.2   | 34.5   | 40.1   | 43.2   |
| `bc65536` | 65 676 B       | 41.8   | 49.5   | 53.8   | 56.6   |

---

## Interpreting Results

| Metric   | Meaning                                                                 |
|:---------|:------------------------------------------------------------------------|
| **Min**  | Best-case -- OS scheduler and TCP cooperated perfectly                  |
| **P50**  | Median -- the typical case; half of all messages were faster            |
| **P95**  | 5% of messages took longer than this                                    |
| **P99**  | Tail latency -- use this for real-time deadline analysis                |
| **Max**  | Worst-case -- single OS scheduling or TCP ACK hiccup                   |
| **Mean** | Arithmetic average; skewed by outliers; less useful than P50            |

> [!TIP]
> Design real-time systems to **P99**, not P50. The gap between P50 and P99
> reveals how much jitter the OS scheduler and TCP stack introduce on your platform.

The RTT covers the full 4-hop path through `mtrouter`:

```
consumer serialize -> TCP write -> mtrouter receive -> mtrouter route -> TCP write ->
provider receive -> provider deserialize -> handler -> response serialize ->
TCP write -> mtrouter receive -> mtrouter route -> TCP write ->
consumer receive -> consumer deserialize -> response handler
```

Payload size has near-zero impact on latency up to at least 512 bytes --
the constant framing, routing, and dispatch overhead dominates.

---

## Competitive Context

These numbers are produced by a **full-stack framework** – centralized broker routing,
guaranteed thread affinity, and service dispatch are all active and included in every
measurement. This is not a raw transport benchmark.

Timestamps are taken **before serialization** (sender) and **after deserialization + dispatch**
(receiver) – the full production call path is inside the measured window.

| Framework | Transport | OWT / RTT | Metric | Source |
|:----------|:---------:|:---------:|:------:|:-------|
| **areg-sdk** | TCP, 2-hop broker (Linux, pinned) | OWT **10.5 μs** Min / **~11.2 μs** P50 | Full: serialization + routing + dispatch | Measured – this example |
| **areg-sdk** | TCP, 2-hop broker (Linux, pinned) | RTT **21.2 μs** Min / **~21.7 μs** P50 | Full: broker + dispatch + thread affinity | Measured – this example |
| **areg-sdk** | TCP, 2-hop broker (Windows) | RTT **82.5 μs** P50 | Full: broker + dispatch + thread affinity | Measured – this example |
| **areg-sdk** | TCP, 2-hop broker (macOS M3) | RTT **62.5 μs** P50 | Full: broker + dispatch + thread affinity | Measured – this example |
| NanoMsg | TCP, direct (Linux) | OWT **18.0 μs** Min / **21.9 μs** Avg | Raw transport, no dispatch | [Hitachi Energy, arXiv:2508.07934v1](https://arxiv.org/abs/2508.07934) |
| ZMQ | TCP, direct (Linux) | OWT **22.0 μs** Min / **27.5 μs** Avg | Raw transport, no dispatch | [Hitachi Energy, arXiv:2508.07934v1](https://arxiv.org/abs/2508.07934) |
| NNG | TCP, direct (Linux) | OWT **24.3 μs** Min / **34.9 μs** Avg | Raw transport, no dispatch | [Hitachi Energy, arXiv:2508.07934v1](https://arxiv.org/abs/2508.07934) |
| gRPC C++ | UDS, direct (Linux) | RTT **116 μs** median (other core) | Full RPC, HTTP/2 + protobuf | [MPI-HD, F. Werner, 2021](https://www.mpi-hd.mpg.de/personalhomes/fwerner/research/2021/09/grpc-for-ipc/) |

> **areg-sdk on Linux (OWT, TCP, 2-hop, pinned cores)** achieves lower Min and Mean than NanoMsg,
> ZMQ, and NNG (raw TCP, direct, isolated cores, low load) – while routing through a centralized
> broker and including full typed serialization and dispatch on both sides.
>
> **RTT floor claim:** areg-sdk pp64 RTT Min (21.2 μs) < 2 × NanoMsg direct OWT Min (18.0 × 2 = 36 μs).
> The complete service framework adds less overhead than a second raw TCP hop.
>
> Raw output of these exact runs: [benchmark-test-results.txt](../../docs/wiki/benchmark-test-results.txt)
>
> For the full competitive analysis, see:
> → **[areg-sdk vs ZMQ / NanoMsg / NNG](../../docs/wiki/08c-areg-vs-hitachi-benchmark.md)**
> → **[areg-sdk Framework Rankings](../../docs/wiki/08d-areg-framework-rankings.md)**
