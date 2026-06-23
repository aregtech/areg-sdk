# 23_pubdatarate

🚀 **Type:** IPC / Multi-Process

## Overview

Distributed systems demand frameworks that stay out of the way when data moves fast.
**23_pubdatarate** stress-tests exactly that: a **Public Service Provider** streams
continuous bitmap payloads to one or more **Service Consumers** across process
boundaries, while the framework routes, delivers, and tracks every message – all
without a dedicated networking library underneath.

> [!TIP]
> The `mtrouter` console shows **network-layer throughput** – the most accurate signal.
> The provider and consumer consoles show send/receive rates at the application level,
> which differ slightly. See [Interpreting Results](#interpreting-results).

---

### Data Rate – Large Payload (~3 MB per message)

| Platform | Peak | Sustained |
|---|---|---|
| Linux ¹ | **~8.0 GB/s** | ~6.0 GB/s ² |
| macOS M3 Pro | – | ~6.7–7.0 GB/s |
| Windows 11 ³ | **~3.0 GB/s** | ~2.5 GB/s |
| WSL2 (Win11) ⁴ | – | ~4.0–4.5 GB/s |

---

### Message Rate – Very Small Payload (~0.5 KB per message)

| Platform | Peak | Sustained |
|---|---|---|
| Linux ¹ | **~2.5M msg/s** | ~1.5M msg/s ² |
| macOS M3 Pro | – | ~2.5M msg/s |
| Windows 11 ³ | **~2.8M msg/s** | ~1.1M msg/s ⁵ |
| WSL2 (Win11) | – | ~1.5M msg/s |

All x86 rows: same physical machine, Intel i7-13700H, 32 GB DDR4. macOS: Apple M3 Pro, 32 GB LPDDR5.

¹ Ubuntu 26.04, **Performance** power mode.

² Sustained (5-minute) figure carried over from a prior measurement round taken in
**Balanced** power mode; not yet re-verified under Performance mode. A re-test is
recommended to confirm whether sustained throughput improved alongside the peak.

³ Updated peak reading; supersedes the previous burst figures for Windows.

⁴ Without [network tuning](../../docs/wiki/07d-troubleshooting-network-tunning.md): ~4.0–4.5 GB/s.
With tuning: ~5.0–5.6 GB/s. See [network tuning guide](../../docs/wiki/07d-troubleshooting-network-tunning.md).

⁵ Windows message rate at ~0.5 KB declines over time: peak → ~1.12M at 2 min (test stopped).
No 5-minute stable figure measured; extrapolated sustained ~1.0–1.1M msg/s.

**Methodology:**
- TCP `localhost`, 1:1 connection (single provider → single consumer) via `mtrouter`.
- All tests use Areg SDK with full service dispatch, built-in threading, and automatic framing.
- Measured at `mtrouter` – the highest-accuracy measurement point (see [Interpreting Results](#interpreting-results)).
- Reported ranges reflect the most frequently observed values; occasional peak readings may exceed the upper bound.

These numbers come from a full-stack, service-oriented pipeline – location-transparent
service discovery, type-safe IPC, automatic reconnection – none of which is stripped
away for the benchmark. **What you measure here is what your production system gets.**

---

**Use this example to:**

- **Stress-test** your hardware ceiling – when the consumer dispatch saturates, memory
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
- [Building the project](#building-the-project)
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

- **Real-Time Data Rate Monitoring** – Measure and display network throughput for
  communications between provider and consumers.
- **Service Interface Automation** – Uses the Areg SDK code generator to automate
  **Object RPC** message creation and dispatching.
- **Efficient Large Data Transfer** – Demonstrates sending and receiving bitmap
  images over IPC using the full service stack.
- **Fault-Tolerant Communication** – Ensures reliable delivery of messages even
  under process restart or network interruption.

---

## Sub-Projects

| Sub-Project | Role | Description |
|---|---|---|
| **[23_generated](./services/LargeData.siml)** | Generated code | Object RPC stubs auto-generated from `LargeData.siml` – provides the full IPC infrastructure |
| **[23_pubservice](./pubservice/)** | Provider | Generates bitmap frames, streams them to consumers, displays real-time send rates |
| **[23_pubclient](./pubclient/)** | Consumer | Receives bitmap frames, dispatches RPC calls, displays real-time receive rates |

---

## How It Works

```
23_pubservice                mtrouter               23_pubclient
      |                          |                        |
      |-- TCP send() ----------->|                        |
      |   (raw buffer, no        |-- TCP send() --------->|
      |    re-serialization)     |   (same raw buffer)    |
      |                          |                        |
      |<-- scope control --------|<-- connect/subscribe --|
```

The provider serializes a bitmap frame once and sends it to `mtrouter` as a raw buffer.
The router forwards the **same buffer** to every connected consumer without re-serialization –
it is a buffer relay, not a message broker. The consumer's socket pump receives the raw
buffer and queues it for the dispatch thread, which deserializes and executes the RPC call.

Both the provider and the consumer independently measure and display transfer rates.
The `mtrouter` console shows incoming and outgoing byte rates simultaneously – the
most accurate throughput signal, because it uses a high-precision timer and sees
both directions at the socket boundary.

The Areg Framework handles reconnection after process restart and thread dispatch on the consumer side automatically.

---

## Building the Project

For best benchmark results, build in `Release` configuration with no logging:
```bash
cmake -B ./build -DCMAKE_BUILD_TYPE=Release -DAREG_COMPILER_FAMILY=gnu -DAREG_LOGGING=OFF -DAREG_TESTS=OFF
cmake --build ./build --parallel
```

---

## Running the Benchmark

> [!IMPORTANT]
> Executable extensions differ by platform: `.exe` on Windows, `.elf` on Linux,
> `.mac` on macOS. For example: `mtrouter.exe` / `mtrouter.elf` / `mtrouter.mac`.
> The commands below include extensions – adjust if your build uses different conventions.

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

Leave the `mtrouter` console visible – it shows live byte rates and provides
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

> [!WARNING]
> **At high throughput rates, `23_pubclient` will exhaust memory and be killed by the OS.**
> This is expected – it defines the hardware ceiling. Before this happens:
> - Type **`-p`** in the `23_pubservice` console to **pause** the stream, or
> - Type **`-q`** to **quit** the provider entirely.
> Stop the stream before `23_pubclient` runs out of memory, especially when scaling
> channels (`-c`) beyond the stable dispatch ceiling.

---

### Provider Command Reference

All commands are entered interactively in the `23_pubservice` console while running.

| Command    | Long form        | Range       | Description                                                                         |
|------------|------------------|-------------|-------------------------------------------------------------------------------------|
| `-w=<N>`   | `--width=<N>`    | 32 – 32768  | Frame width in pixels                                                               |
| `-h=<N>`   | `--height=<N>`   | 32 – 32768  | Frame height in pixels                                                              |
| `-l=<N>`   | `--lines=<N>`    | 1 – height  | Lines per transmitted block; set equal to height to send one full frame per message |
| `-t=<N>`   | `--time=<N>`     | 0 – 100000  | Time in nanoseconds to generate one pixel; `0` = full speed                         |
| `-c=<N>`   | `--channels=<N>` | 1 – 96      | Parallel channels – primary multiplier for throughput and message rate              |
| `-i`       | `--info`         | –           | Display current configuration                                                       |
| `-h`       | `--help`         | –           | Display help message                                                                |
| `-s`       | `--start`        | –           | Start streaming                                                                     |
| `-p`       | `--stop`         | –           | Stop streaming                                                                      |
| `-q`       | `--quit`         | –           | Quit the application                                                                |

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

> [!IMPORTANT]
> **Monitor `23_pubclient` memory usage during high-rate tests.** When message rate
> exceeds the stable dispatch ceiling (~300–400K msg/s on Windows, ~500–600K on macOS),
> the consumer queue grows unbounded and memory climbs until the OS terminates the process.
> **Before this happens, type `-p` (pause) or `-q` (quit) in the `23_pubservice` console.**
> Read `mtrouter` output just before stopping – that is your hardware's throughput ceiling.

Enter the parameters in the `23_pubservice` console, then type `-s` to start.

> [!TIP]
> Watch all three consoles simultaneously: provider shows send rate, consumer
> shows receive rate, `mtrouter` shows network-layer rate. The three values
> differ slightly – see [Interpreting Results](#interpreting-results).
>
> The `23_pubservice` application prints the computed theoretical rates whenever
> you change parameters, before you type `-s`. Use this to confirm your
> configuration matches the expected values.

---

#### Recipe 1 – Large payload data rate (~3 MB per message)

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

#### Recipe 2 – Small payload message rate (~3 KB per message)

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

#### Recipe 3 – Very small payload message rate (~0.5 KB per message)

> **Target:** Measure per-message overhead with minimal payload.

```
-w=128 -h=128 -l=1 -t=25 -c=1
```

**What this does:** Sends single-line slices of a 128×128 frame.
Each block is `128 × 1 × 3 = 384 bytes` (~0.5KB message is `image data` + `message header` + `routing info`). `-t=25` with `-c=1` produces ~312K messages/sec.

**Scaling with `-c` and `-t`:**

Tune the message rate by changing number of channels and pixel time:

| `-c` | `-t` | 128 x 128 (~0.5 KB) |
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
| `23_pubservice`  | Bytes/messages queued for send                | Slightly optimistic – leads actual socket |
| `23_pubclient`   | Bytes/messages delivered to dispatch thread   | Lags socket receipt by one dispatch cycle |
| `mtrouter`       | Raw bytes at the socket boundary, both directions | **Most accurate** – high-precision timer, full duplex |

---

### Network Layer vs Application Layer

The tables in this README report **network-layer throughput** measured at `mtrouter`.
The consumer process receives data at this rate at the socket level, but the
**dispatch thread** – which deserializes and executes the RPC call – has a lower
stable throughput ceiling.

**Stable dispatch ceiling: ~300–400K msg/s on Windows (i7-13700H), ~500–600K msg/s on macOS (M3 Pro).**

Above this rate, the consumer's internal queue between the socket pump and the
dispatch thread grows faster than it drains. Memory climbs steadily and the OS
will eventually terminate the consumer. This is not a network or transport
limitation – it is a serialization and dispatch throughput characteristic.
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

- **Throughput validation** – Confirm the framework imposes negligible overhead
  before committing to a service-oriented architecture for data-intensive pipelines.
- **Hardware profiling** – Determine the practical IPC ceiling of your target
  hardware before production deployment.
- **Architecture sizing** – Choose payload size, channel count, and consumer
  topology based on measured rather than theoretical numbers.
- **Scientific and industrial imaging** – Directly applicable to laser microscopy,
  X-ray, electron microscopy, and machine vision pipelines that move multi-MB
  frames continuously between acquisition and processing processes.

---

## Takeaway

**Transport ceiling (measured at `mtrouter`):**

| Platform | ~3 MB peak/sustained | ~0.5 KB peak/sustained |
|---|---|---|
| Linux ¹ | ~8.0 / ~6.0 GB/s | ~2.5M / ~1.5M msg/s |
| macOS M3 Pro | – / ~6.7–7.0 GB/s | – / ~2.5M msg/s |
| Windows 11 ² | ~3.0 / ~2.5 GB/s | ~2.8M / ~1.1M msg/s |
| WSL2 (Win11) | – / 4.0–4.5 GB/s (untuned), 5.0–5.6 (tuned) | – / ~1.5M msg/s |

¹ Performance power mode (peak); sustained figure from a prior Balanced-mode round, pending re-test.
² Updated peak readings for Windows 11.

**Stable end-to-end consumer dispatch:**

- Windows (i7-13700H): **300–400K msg/s**
- macOS (M3 Pro): **500–600K msg/s**

Above these rates the dispatch thread becomes the bottleneck – serialization- and dispatch-bound, not transport-bound. Improvements are planned.

These are not raw socket numbers. They include message routing, event dispatching, multithreading, and automatic framing.
**This is what your production system gets.**

> For full latency data, competitive analysis, and methodology:
> → **[areg-sdk Performance Benchmarks](../../docs/wiki/08b-areg-sdk-performance-benchmarks.md)**
> → **[areg-sdk vs ZMQ / NanoMsg / NNG](../../docs/wiki/08c-areg-vs-hitachi-benchmark.md)**
> → **[areg-sdk Framework Rankings](../../docs/wiki/08d-areg-framework-rankings.md)**
