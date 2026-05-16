# 23_pubdatarate

🚀 **Type:** IPC / Multi-Process

## Overview

Distributed systems demand frameworks that stay out of the way when data moves fast.
**23_pubdatarate** stress-tests exactly that: a **Public Service Provider** streams
continuous bitmap payloads to one or more **Service Consumers** across process
boundaries, while the framework routes, delivers, and tracks every message — all
without a dedicated networking library underneath.

> [!TIP]
> The `mtrouter` console shows **network-layer throughput** — the most accurate signal.
> The provider and consumer consoles show send/receive rates at the application level,
> which differ slightly. See [Interpreting Results](#interpreting-results).

---

### Data Rate — Large Payload (~3 MB per message)

| Platform             | CPU               | RAM       | Payload | Data Rate  | Unit | Status    |
|----------------------|-------------------|-----------|---------|------------|------|-----------|
| Windows 11           | Intel i7-13700H   | DDR4      | ~3 MB   | 2.4 – 2.6  | GB/s | measured  |
| WSL2 (on Win11) ¹    | Intel i7-13700H   | DDR4      | ~3 MB   | 5.0 – 5.6  | GB/s | measured  |
| Linux VM (on macOS) ²| Apple M4 Pro      | LPDDR5    | ~3 MB   | 6.6 – 6.8  | GB/s | measured  |
| macOS native ²       | Apple M4 Pro      | LPDDR5    | ~3 MB   | 6.8 – 7.0  | GB/s | measured  |
| Linux bare-metal     | x86-64            | DDR4/DDR5 | ~3 MB   | 6.0 – 7.5  | GB/s | estimated |

---

### Message Rate — Small Payload (~3 KB per message)

| Platform             | CPU               | RAM       | Payload | Message Rate | Unit  | Status    |
|----------------------|-------------------|-----------|---------|--------------|-------|-----------|
| Windows 11           | Intel i7-13700H   | DDR4      | ~3 KB   | 450 – 520K   | msg/s | measured  |
| WSL2 (on Win11) ¹    | Intel i7-13700H   | DDR4      | ~3 KB   | 330 – 375K   | msg/s | measured  |
| Linux VM (on macOS) ²| Apple M4 Pro      | LPDDR5    | ~3 KB   | 180 – 200K   | msg/s | measured  |
| macOS native ²       | Apple M4 Pro      | LPDDR5    | ~3 KB   | 240 – 290K   | msg/s | measured  |
| Linux bare-metal     | x86-64            | DDR4/DDR5 | ~3 KB   | 500 – 600K   | msg/s | estimated |

---

### Message Rate — Very Small Payload (~0.5 KB per message)

| Platform             | CPU               | RAM       | Payload  | Message Rate | Unit  | Status    |
|----------------------|-------------------|-----------|----------|--------------|-------|-----------|
| Windows 11           | Intel i7-13700H   | DDR4      | ~0.5 KB  | 1.0 – 1.2M   | msg/s | measured  |
| WSL2 (on Win11) ¹    | Intel i7-13700H   | DDR4      | ~0.5 KB  | 450 – 520K   | msg/s | measured  |
| Linux VM (on macOS) ²| Apple M4 Pro      | LPDDR5    | ~0.5 KB  | —            | msg/s | needed    |
| macOS native ²       | Apple M4 Pro      | LPDDR5    | ~0.5 KB  | —            | msg/s | needed    |
| Linux bare-metal     | x86-64            | DDR4/DDR5 | ~0.5 KB  | —            | msg/s | needed    |

¹ Requires [network tuning](../../docs/wiki/07d-troubleshooting-network-tunning.md).
Default WSL2 settings yield approximately 60–70% of the values shown.

² Pre-optimization measurements. Current results on Apple M4 Pro are expected to be higher.

**Methodology:**
- TCP `localhost`, 1:1 connection (single provider → single consumer) via `mtrouter`.
- All tests use Areg SDK with full service dispatch, built-in threading, and automatic framing.
- Measured at `mtrouter` — the highest-accuracy measurement point (see [Interpreting Results](#interpreting-results)).
- Reported ranges reflect the most frequently observed values; occasional peak readings may exceed the upper bound.
- Bare-metal Linux values estimated from WSL2 measurements.

These numbers come from a full-stack, service-oriented pipeline — location-transparent
service discovery, type-safe IPC, automatic reconnection — none of which is stripped
away for the benchmark. **What you measure here is what your production system gets.**

---

**Use this example to:**

- **Stress-test** your hardware ceiling — when the consumer dispatch saturates, memory
  grows unbounded, giving you a clear upper bound for your hardware configuration.
- **Profile** data-intensive pipelines before committing to an architecture.
- **Validate** that the framework overhead is negligible for your use case.
- **Experiment** with payload sizes, timing, and channel counts to find your system's ceiling.

> [!NOTE]
> This example requires **Multi-Target Router (`mtrouter`)** for inter-process message routing.
> Ensure `mtrouter` is running before starting provider or consumer processes.
> The `areg.init` file must have the correct IP address and port configured.

---

## Table of Contents

- [Concepts Shown](#concepts-shown)
- [Sub-Projects](#sub-projects)
- [How It Works](#how-it-works)
- [Running the Benchmark](#running-the-benchmark)
  - [Step 1: Start mtrouter](#step-1-start-mtrouter)
  - [Step 2: Start the Consumer](#step-2-start-the-consumer)
  - [Step 3: Start the Provider](#step-3-start-the-provider)
  - [Provider Command Reference](#provider-command-reference)
  - [Benchmark Recipes](#benchmark-recipes)
- [Interpreting Results](#interpreting-results)
- [Use Cases](#use-cases)

---

## Concepts Shown

- **Real-Time Data Rate Monitoring** — Measure and display network throughput for
  communications between provider and consumers.
- **Service Interface Automation** — Uses the Areg SDK code generator to automate
  **Object RPC** message creation and dispatching.
- **Efficient Large Data Transfer** — Demonstrates sending and receiving bitmap
  images over IPC using the full service stack.
- **Fault-Tolerant Communication** — Ensures reliable delivery of messages even
  under process restart or network interruption.

---

## Sub-Projects

| Sub-Project | Role | Description |
|---|---|---|
| **[23_generated](./services/LargeData.siml)** | Generated code | Object RPC stubs auto-generated from `LargeData.siml` — provides the full IPC infrastructure |
| **[23_pubservice](./pubservice/)** | Provider | Generates bitmap frames, streams them to consumers, displays real-time send rates |
| **[23_pubclient](./pubclient/)** | Consumer | Receives bitmap frames, dispatches RPC calls, displays real-time receive rates |

---

## How It Works

```
23_pubservice                mtrouter               23_pubclient
      |                          |                        |
      |-- TCP send() ----------->|                        |
      |   (raw buffer, no        |-- TCP send() --------->|
      |    re-serialization)     |   (path serialization) |
      |                          |                        |
      |<-- scope control --------|<-- connect/subscribe --|
```

The provider serializes a bitmap frame once and sends it to `mtrouter` as a raw buffer.
The router forwards the **same buffer** to every connected consumer without re-serialization —
it is a buffer relay, not a message broker. The consumer's socket pump receives the raw
buffer and queues it for the dispatch thread, which deserializes and executes the RPC call.

Both the provider and the consumer independently measure and display transfer rates.
The `mtrouter` console shows incoming and outgoing byte rates simultaneously — the
most accurate throughput signal, because it uses a high-precision timer and sees
both directions at the socket boundary.

The Areg Framework handles service discovery automatically via `service_connected()`,
reconnection after process restart, and thread dispatch on the consumer side.

---

## Running the Benchmark

> [!IMPORTANT]
> Executable extensions differ by platform: `.exe` on Windows, `.elf` on Linux,
> `.mac` on macOS. For example: `mtrouter.exe` / `mtrouter.elf` / `mtrouter.mac`.
> The commands below include extensions — adjust if your build uses different conventions.

---

### Step 1: Start `mtrouter`

Start the router first. It must be running before provider or consumer attempt to connect.

```bash
# Linux
./mtrouter.elf

# macOS
./mtrouter.mac

# Windows
.\mtrouter.exe
```

Leave the `mtrouter` console visible — it shows live byte rates and provides
the most accurate throughput signal of the three processes.

---

### Step 2: Start the Consumer

Start one or more consumer instances in separate terminals. The consumer waits
for the provider to connect and begin broadcasting.

```bash
# Linux
./23_pubclient.elf

# macOS
./23_pubclient.mac

# Windows
.\23_pubclient.exe
```

Multiple consumers can connect simultaneously. Each receives the full broadcast
stream independently.

---

### Step 3: Start the Provider

Start the provider in its own terminal. Configure parameters first, then type `-s` to start.

```bash
# Linux
./23_pubservice.elf

# macOS
./23_pubservice.mac

# Windows
.\23_pubservice.exe
```

---

### Provider Command Reference

All commands are entered interactively in the `23_pubservice` console while running.

| Command    | Long form        | Range       | Description                                                                         |
|------------|------------------|-------------|-------------------------------------------------------------------------------------|
| `-w=<N>`   | `--width=<N>`    | 32 – 32768  | Frame width in pixels                                                               |
| `-h=<N>`   | `--height=<N>`   | 32 – 32768  | Frame height in pixels                                                              |
| `-l=<N>`   | `--lines=<N>`    | 1 – height  | Lines per transmitted block; set equal to height to send one full frame per message |
| `-t=<N>`   | `--time=<N>`     | 0 – 100000  | Time in nanoseconds to generate one pixel; `0` = full speed                         |
| `-c=<N>`   | `--channels=<N>` | 1 – 96      | Parallel channels — primary multiplier for throughput and message rate              |
| `-i`       | `--info`         | —           | Display current configuration                                                       |
| `-h`       | `--help`         | —           | Display help message                                                                |
| `-s`       | `--start`        | —           | Start streaming                                                                     |
| `-p`       | `--stop`         | —           | Stop streaming                                                                      |
| `-q`       | `--quit`         | —           | Quit the application                                                                |

> [!NOTE]
> `-h=<N>` (with `=` and a value) sets frame height. `-h` alone (no `=`) displays help.
> Parameters `-w`, `-h=`, `-l`, `-t`, `-c` must be set **before** typing `-s`.
> To change parameters mid-run: stop with `-p`, reconfigure, then restart with `-s`.

**How channels scale throughput:**

Each channel (`-c`) is an independent data stream modeled as a pixel scanner:
one pixel (`bytes_per_pixel` bytes) is generated every `pixel_time_ns` nanoseconds.
Increasing `-c` scales both data rate and message rate proportionally.

Theoretical throughput:

```
data_rate_bytes_per_sec = (1e9 / pixel_time_ns) × bytes_per_pixel × channels

message_rate_per_sec    = (1e9 / (width × lines_per_block × pixel_time_ns)) × channels
```

In practice, measured rates are slightly below theoretical due to scheduling
jitter and socket buffer behavior.

---

### Benchmark Recipes

Enter the parameters in the `23_pubservice` console, then type `-s` to start.

> [!TIP]
> Watch all three consoles simultaneously: provider shows send rate, consumer
> shows receive rate, `mtrouter` shows network-layer rate. The three values
> differ slightly — see [Interpreting Results](#interpreting-results).
>
> The `23_pubservice` application prints the computed theoretical rates whenever
> you change parameters, before you type `-s`. Use this to confirm your
> configuration matches the expected values.

---

#### Recipe 1 — Large payload data rate (~3 MB per message)

> **Target:** Measure maximum data throughput with large bitmap frames.

```
-w=1024 -h=1024 -l=1024 -t=25 -c=20
```

**What this does:** Sends full 1024×1024 frames as single ~3 MB blocks.
One frame takes ~26 ms to generate (1024 × 1024 × 25 ns), producing 38 frames/sec
and ~114 MiB/s per channel. With `-c=20`, the theoretical total is **~2.4 GB/s**
at 762 messages/sec.

**Scaling with `-c`:**

| `-c` | Data Rate   |
|------|-------------|
| 10   | ~1.4 GB/s   |
| 30   | ~3.4 GB/s   |
| 50   | ~5.7 GB/s   |

You can fine-tune the data rate by adjusting `-t`. Smaller `-t` → higher rate.
For example, `-c=25 -t=28` produces ~2.55 GB/s:

```
-w=1024 -h=1024 -l=1024 -c=25 -t=28
```

---

#### Recipe 2 — Small payload message rate (~3 KB per message)

> **Target:** Measure maximum message throughput with small frames.

```
-w=1024 -h=1024 -l=1 -t=25 -c=4
```

**What this does:** Sends single-line slices of a 1024-wide frame (~3 KB each).
`-c=4` produces ~156K messages/sec.

**Scaling with `-c` and `-t`:**

Tune 3MB message rate by changing number of channels and pixel time:

| `-c` | `-t` | 1024 x 1024 (~3MB) |
|------|------|--------------------|
| 4    |  25  | ~156K msg/s        |
| 8    |  26  | ~300K msg/s        |
| 15   |  29  | ~505K msg/s        |

---

#### Recipe 3 — Very small payload message rate (~0.5 KB per message)

> **Target:** Measure per-message overhead with minimal payload.

```
-w=128 -h=128 -l=1 -t=25 -c=1
```

**What this does:** Sends single-line slices of a 128×128 frame.
Each block is `128 × 1 × 3 = 384 bytes` (~0.5KB message is `image data` + `message header` + `routing info`). `-t=25` with `-c=1` produces ~312K messages/sec.

**Scaling with `-c` and `-t`:**

Tune the message rate by changing number of channels and pixel time:

| `-c` | `-t` | 128 x 128 (~0.5MB) |
|------|------|--------------------|
| 1    |  25  | ~312K msg/s        |
| 2    |  31  | ~504K msg/s        |
| 4    |  26  | ~1.2M msg/s        |
> [!NOTE]
> The `23_pubservice` application prints the computed rates whenever you change
> parameters. Example output for `-w=128 -h=128 -l=1 -c=4 -t=26`:
>
> ```
> Input options. Or type '-q' to quit application, or type '-h' to read help:
>
> ---------------------------------------
> Printing image current options:
> The large data state is : STOPPED
>        Width ...........:     128 pix.
>        Height ..........:     128 pix.
>        Lines per Block .:       1 lns.
>        Pixel Time ......:      26 ns.
>        Channels ........:       4 ch.
>        Time per Block ..:    3.33 us.
>        Block Size ......:  384.00  Byte.
>        Block Rate ......:  300480 blocks/sec/ch.
>        Theoretical rate.:  110.04 MByte / sec (per channel)
>        Total Block Rate : 1201923 blocks/sec
>        Total rate ......:  440.16 MByte / sec (all channels)
>        Connected client :       1 clients.
> ---------------------------------------
> ```

---

## Interpreting Results

### Where to Read Throughput

| Console          | What it measures                              | Accuracy note                             |
|------------------|-----------------------------------------------|-------------------------------------------|
| `23_pubservice`  | Bytes/messages queued for send                | Slightly optimistic — leads actual socket |
| `23_pubclient`   | Bytes/messages delivered to dispatch thread   | Lags socket receipt by one dispatch cycle |
| `mtrouter`       | Raw bytes at the socket boundary, both directions | **Most accurate** — high-precision timer, full duplex |

---

### Network Layer vs Application Layer

The tables in this README report **network-layer throughput** measured at `mtrouter`.
The consumer process receives data at this rate at the socket level, but the
**dispatch thread** — which deserializes and executes the RPC call — has a lower
stable throughput ceiling.

**Current stable dispatch ceiling: ~300–350K msg/s on mobile-class hardware.**

Above this rate, the consumer's internal queue between the socket pump and the
dispatch thread grows faster than it drains. Memory climbs steadily and the OS
will eventually terminate the consumer. This is not a network or transport
limitation — it is a serialization and dispatch throughput characteristic.
Improvements to this ceiling are planned.

---

### Understanding the Memory Ceiling

When throughput exceeds the dispatch ceiling, use `mtrouter` to read the actual
network capacity while the consumer is still alive. The value just before the
consumer terminates is your hardware's practical upper bound.

---

### What Is Not Measured Here

This benchmark measures **throughput**, not **latency**. The round-trip time
from the provider calling `broadcast_image_block_acquired()` to the consumer
executing `on_broadcast_image_block_acquired()` involves two TCP loopback hops
(provider → mtrouter → consumer), thread dispatch on both sides, and
deserialization on the consumer. Dedicated latency measurement requires a
separate timestamped test.

---

## Use Cases

- **Throughput validation** — Confirm the framework imposes negligible overhead
  before committing to a service-oriented architecture for data-intensive pipelines.
- **Hardware profiling** — Determine the practical IPC ceiling of your target
  hardware before production deployment.
- **Architecture sizing** — Choose payload size, channel count, and consumer
  topology based on measured rather than theoretical numbers.
- **Scientific and industrial imaging** — Directly applicable to laser microscopy,
  X-ray, electron microscopy, and machine vision pipelines that move multi-MB
  frames continuously between acquisition and processing processes.

---

## Takeaway

On a mobile-class Intel i7-13700H with DDR4, the framework is not your bottleneck:

- **Large blocks (~3 MB):** 2.4–2.6 GB/s on Windows, 5.0–5.6 GB/s on WSL2
- **Small messages (~3 KB):** 450–520K msg/s on Windows, 330–375K msg/s on WSL2
- **Very small messages (~0.5 KB):** 1.0–1.2M msg/s on Windows, 450–520K msg/s on WSL2
- **Stable end-to-end dispatch ceiling:** ~300K msg/s on Windows (serialization- and dispatch-bound, not transport-bound)

On Apple M4 Pro, the ceiling rises to ~7 GB/s for large blocks (pre-optimization;
current results expected higher). Bare-metal x86-64 Linux is estimated at 6–7.5 GB/s.

These are not raw socket numbers. They include full service discovery, type-safe
serialization, automatic reconnection, threading dispatch, and message framing.
**This is what your production system gets.**
