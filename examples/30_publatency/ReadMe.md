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
**Parameters persist between runs** -- set them once, then use `-s` to start each run.

Recommended session for reproducible results (8 consecutive runs):

```
# Set parameters once:
-m=pp64 -c=10000 -w=1000

# Then start 8 runs by typing -s and pressing Enter each time:
-s
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
| `-s`           | --               | Start test with current settings                         |
| `-p`           | --               | Stop running test                                        |
| `-i`           | --               | Show current settings                                    |
| `-o=<path>`    | path             | Set the CSV output file path (does not enable writing)   |
| `-f[=<file>]`  | path (optional)  | Enable CSV file output; optionally set the file name     |
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

> Test configuration: `count=10000 warmup=1000` for RTT; `count=5000 warmup=1000` for OWT.
> Values in **μs** (microseconds). Each table is the average of 8 consecutive runs.
> Build: `Release`, `AREG_LOGGING=OFF`.

---

### Linux Ubuntu 26.04 -- Intel i7-13700H (mobile), 32 GB DDR4

> Tested from USB boot. P50 uses median across runs due to bimodal distribution
> caused by USB I/O interference (~25% of runs show elevated P50). Native SSD
> installation expected to show lower variance and improved P95/P99.

#### Ping-Pong RTT (μs)

| Mode  | Total msg size  | Min   | P50   | P95   | P99    | Mean  |
|:------|:---------------:|:-----:|:-----:|:-----:|:------:|:-----:|
| pp0   | 140 / 148 B     | ~30   | ~34   | ~81   | ~106   | ~47   |
| pp64  | 204 / 212 B     | ~31   | ~33   | ~74   | ~88    | ~45   |
| pp128 | 268 / 276 B     | ~31   | ~33   | ~80   | ~99    | ~48   |
| pp512 | 652 / 660 B     | ~32   | ~34   | ~80   | ~96    | ~47   |

#### Broadcast One-Way (μs)

| Mode   | Total msg size | Min   | P50   | P95   | P99   | Mean  |
|:-------|:--------------:|:-----:|:-----:|:-----:|:-----:|:-----:|
| bc0    | 140 B          | ~15   | ~16   | ~33   | ~42   | ~22   |
| bc64   | 204 B          | ~16   | ~17   | ~36   | ~44   | ~23   |
| bc128  | 268 B          | ~15   | ~17   | ~34   | ~49   | ~20   |
| bc512  | 652 B          | ~16   | ~19   | ~38   | ~51   | ~22   |

---

### Windows 11 -- Intel i7-13700H (mobile), 32 GB DDR4

> Native SSD install. Results are stable -- no bimodal distribution.

#### Ping-Pong RTT (μs)

| Mode  | Total msg size  | Min   | P50   | P95   | P99    | Mean  |
|:------|:---------------:|:-----:|:-----:|:-----:|:------:|:-----:|
| pp64  | 204 / 212 B     | ~63   | ~83   | ~88   | ~117   | ~84   |

#### Broadcast One-Way (μs)

| Mode  | Total msg size | Min   | P50   | P95   | P99   | Mean  |
|:------|:--------------:|:-----:|:-----:|:-----:|:-----:|:-----:|
| bc64  | 204 B          | ~32   | ~40   | ~44   | ~60   | ~41   |

---

### macOS -- Apple M4, LPDDR5

> ⚠️ These numbers are **outdated** -- measured before the latest optimizations.
> Re-measurement is pending. Expected to match or slightly exceed Linux numbers.

| Mode  | P50 (pre-opt) | P95 (pre-opt) |
|:------|:-------------:|:-------------:|
| pp64 RTT  | ~69 μs    | ~102 μs       |
| bc64 OWT  | ~21 μs    | ~44 μs        |

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

These numbers are produced by a **full-stack framework** -- centralized broker routing,
guaranteed thread affinity, and service dispatch are all active and included in every
measurement. This is not a raw transport benchmark.

| Framework       | Transport    | RTT P50  | Scope                          |
|:----------------|:------------:|:--------:|:-------------------------------|
| **areg-sdk**    | TCP (Linux)  | **~33 μs** | Full: broker + dispatch + thread affinity |
| **areg-sdk**    | TCP (Windows)| **~83 μs** | Full: broker + dispatch + thread affinity |
| ZMQ (tuned)     | TCP (Linux)  | ~40-50 μs | Raw transport only, 1-byte messages |
| nng req/rep     | TCP (Linux)  | ~44 μs   | Minimal req/rep, no framework  |
| gRPC C++        | TCP (Linux)  | ~200-300 μs | Full RPC, HTTP/2 + protobuf  |

areg-sdk on Linux approaches published ZMQ latency figures on a mobile CPU (2023)
while routing through a centralized broker and carrying 204-byte messages through
a complete service framework. gRPC, the de facto industry standard for C++ RPC,
is 2.4-3.5x slower on equivalent hardware.
