# 30_publatency

🔬 **Type:** IPC / Multi-Process

## Overview

Real-world distributed systems care about two numbers: **throughput** (how much data per second)
and **latency** (how long a single message takes). **30_publatency** measures both the
**round-trip time** and **one-way delivery latency** of AREG SDK's IPC stack across a
range of payload sizes — from a bare timestamp (0 bytes of extra data) to a 64 KB block.

Two measurement modes are available:

- **Ping-Pong (Request / Response)** — The consumer sends a timestamped request; the
  provider echoes it back immediately. Round-trip time (RTT = *t4 − t1*) reflects the
  full stack: serialization, TCP loopback, routing, dispatch, and deserialization on both sides.

- **Broadcast (One-Way)** — The provider fires timestamped messages at a configurable
  rate; the consumer records one-way delivery latency (*t4 − t1*). Both processes share
  the same machine hardware clock, making the measurement accurate without clock
  synchronization.

All benchmarks run over AREG SDK's full service-oriented stack — service discovery,
automatic reconnection, type-safe IPC, and built-in threading are all active.
**What you measure here is what your production system gets.**

---

### Ping-Pong — RTT at P50 (µs)

| Payload        | Windows 11 | WSL2 ¹ | macOS | Linux |
|:---------------|:----------:|:------:|:-----:|:-----:|
| 0 B  (`pp0`)   | — | — | — | — |
| 8 B  (`pp8`)   | — | — | — | — |
| 64 B (`pp64`)  | — | — | — | — |
| 512 B (`pp512`)| — | — | — | — |
| 1 KB (`pp1024`)| — | — | — | — |
| 4 KB (`pp4096`)| — | — | — | — |
| 64 KB (`pp65536`)| — | — | — | — |

---

### Ping-Pong — RTT at P99 (µs)

| Payload        | Windows 11 | WSL2 ¹ | macOS | Linux |
|:---------------|:----------:|:------:|:-----:|:-----:|
| 0 B  (`pp0`)   | — | — | — | — |
| 8 B  (`pp8`)   | — | — | — | — |
| 64 B (`pp64`)  | — | — | — | — |
| 512 B (`pp512`)| — | — | — | — |
| 1 KB (`pp1024`)| — | — | — | — |
| 4 KB (`pp4096`)| — | — | — | — |
| 64 KB (`pp65536`)| — | — | — | — |

---

### Broadcast — One-Way Latency at P50 (µs)

| Payload          | Windows 11 | WSL2 ¹ | macOS | Linux |
|:-----------------|:----------:|:------:|:-----:|:-----:|
| 0 B  (`bc0`)     | — | — | — | — |
| 8 B  (`bc8`)     | — | — | — | — |
| 64 B (`bc64`)    | — | — | — | — |
| 512 B (`bc512`)  | — | — | — | — |
| 1 KB (`bc1024`)  | — | — | — | — |
| 4 KB (`bc4096`)  | — | — | — | — |
| 64 KB (`bc65536`)| — | — | — | — |

---

### Broadcast — One-Way Latency at P99 (µs)

| Payload          | Windows 11 | WSL2 ¹ | macOS | Linux |
|:-----------------|:----------:|:------:|:-----:|:-----:|
| 0 B  (`bc0`)     | — | — | — | — |
| 8 B  (`bc8`)     | — | — | — | — |
| 64 B (`bc64`)    | — | — | — | — |
| 512 B (`bc512`)  | — | — | — | — |
| 1 KB (`bc1024`)  | — | — | — | — |
| 4 KB (`bc4096`)  | — | — | — | — |
| 64 KB (`bc65536`)| — | — | — | — |

---

¹ Requires [network tuning](../../docs/wiki/07d-troubleshooting-network-tunning.md).
Default WSL2 settings yield approximately 60–70% of the values shown.

**Methodology:**
- TCP `localhost`, 1:1 connection (single provider → single consumer) via `mtrouter`.
- Ping-pong: 1,000 samples after 50-message warmup; RTT = *t4 − t1* (same consumer-process clock).
- Broadcast: 5-second burst at 1,000 msg/s; one-way = *t4 − t1* (shared machine hardware clock).
- All tests use AREG SDK with full service dispatch, built-in threading, and automatic framing.
- Reported values are medians (P50) and 99th percentiles (P99) over the sample population.

---

**Use this example to:**

- **Measure latency** of your target hardware and OS through AREG SDK's IPC stack.
- **Baseline request/response cycles** before committing to a service-oriented architecture.
- **Evaluate payload impact** — understand how serialization and TCP framing costs grow
  from a zero-byte message to a 64 KB block.
- **Compare broadcast vs. request/response** delivery latency trade-offs.
- **Export raw samples** to CSV for offline percentile analysis and regression tracking.

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
  - [Step 2: Start the Provider](#step-2-start-the-provider)
  - [Step 3: Start the Consumer](#step-3-start-the-consumer)
  - [Consumer Command Reference](#consumer-command-reference)
  - [Provider Command Reference](#provider-command-reference)
  - [Benchmark Recipes](#benchmark-recipes)
- [Detailed Results by Platform](#detailed-results-by-platform)
- [Interpreting Results](#interpreting-results)
- [Use Cases](#use-cases)

---

## Concepts Shown

- **Round-Trip Latency Measurement** — Timestamped ping-pong requests expose the full IPC
  round-trip cost: serialization, TCP loopback, message routing, thread dispatch, and
  deserialization on both sides.
- **One-Way Broadcast Latency** — Provider-side timestamps on broadcast messages quantify
  delivery latency independently of the response path.
- **Payload-Size Sensitivity** — Eleven payload sizes (0 B – 64 KB) reveal how
  serialization cost and TCP segment boundaries affect latency.
- **Service Interface Automation** — Uses the AREG SDK code generator to auto-generate
  all Object RPC stubs from `Latency.siml`.
- **Warmup-Aware Sampling** — A configurable warmup count discards OS scheduling and
  connection-setup effects before recording statistics.
- **CSV Export** — Every test writes a raw sample file with per-message timestamps
  (*t1*, *t2*, *t4*) for offline analysis.

---

## Sub-Projects

| Sub-Project | Role | Description |
|---|---|---|
| **[30_generated](./services/Latency.siml)** | Generated code | Object RPC stubs auto-generated from `Latency.siml` — provides the full IPC infrastructure |
| **[30_pubprovider](./pubprovider/)** | Provider | Serves ping-pong requests, drives broadcast bursts on a 1-second timer, displays live stats |
| **[30_pubconsumer](./pubconsumer/)** | Consumer | Configures and starts tests, collects timestamped samples, computes percentiles, exports CSV |

---

## How It Works

### Ping-Pong (Request / Response)

```
30_pubconsumer              mtrouter            30_pubprovider
      |                          |                        |
      |-- request_ping_pong ---->|-- TCP send() --------->|  t1 = consumer send
      |   (id, t1)               |                        |
      |                          |   records one-way:     |
      |                          |   t2 = now_ns()        |
      |<-- TCP send() -----------|<-- response_ping_pong--|  provider replies (t1, t2)
      |   (id, t1, t2)           |                        |
      |                          |                        |  t4 = consumer receive
      |                          |                        |
      |   RTT    = t4 - t1  (exact: same consumer clock)  |
      |   server ≈ t2 - t1  (approx: shared machine clock)|
```

The consumer sends one request at a time and waits for the response before sending the
next. This fully serializes the pipeline and yields clean RTT measurements without
pipelining effects.

### Broadcast (One-Way)

```
30_pubprovider              mtrouter            30_pubconsumer
      |                          |                        |
      |-- broadcast_message ---->|                        |  t1 = provider send
      |   (id, t1)               |-- TCP send() --------->|
      |                          |                        |  t4 = consumer receive
      |                          |                        |
      |   one-way = t4 - t1  (shared machine hardware clock)
```

The provider fires messages at the configured rate (messages/second) using a 1-second
burst timer. The consumer passively receives and timestamps each delivery.

Both processes read the same machine hardware clock through
`std::chrono::steady_clock`, so the inter-process time delta is accurate without
any clock synchronization step.

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

Leave the `mtrouter` console visible — it confirms that both processes have connected.

---

### Step 2: Start the Provider

Start the provider in its own terminal. It waits for consumers to connect and serves
requests automatically — no configuration is required at the provider side.

```bash
# Linux
./30_pubprovider.elf

# macOS
./30_pubprovider.mac

# Windows
.\30_pubprovider.exe
```

The provider console shows live statistics: current mode, status, total messages served,
and throughput rate.

---

### Step 3: Start the Consumer

Start the consumer in its own terminal. Once connected, configure the test parameters
and type `-s` to start. Results appear after the test completes and are saved to a CSV file.

```bash
# Linux
./30_pubconsumer.elf

# macOS
./30_pubconsumer.mac

# Windows
.\30_pubconsumer.exe
```

---

### Consumer Command Reference

All commands are entered interactively in the `30_pubconsumer` console while running.
Multiple options can be combined on a single line: `-s -m=pp0 -c=1000 -w=50`.

| Command      | Long form      | Values / Range                 | Description                                                                  |
|:-------------|:---------------|:-------------------------------|:-----------------------------------------------------------------------------|
| `-m=<mode>`  | `mode=<mode>`  | See [Modes](#modes) below      | Latency test mode: ping-pong or broadcast, with payload size                 |
| `-c=<N>`     | `count=<N>`    | > 0                            | Ping-pong: total samples to collect. Broadcast: messages per second          |
| `-w=<N>`     | `warmup=<N>`   | ≥ 0                            | Messages excluded from statistics at the start of each test (warmup count)   |
| `-d=<N>`     | `dur=<N>`      | ≥ 0 (0 = run until `-p`)      | Broadcast duration in seconds; `0` runs continuously until stopped           |
| `-o=<path>`  | `out=<path>`   | File path                      | CSV output path; auto-generated from mode and timestamp when omitted         |
| `-s`         | `start`        | —                              | Start the benchmark with current settings                                    |
| `-p`         | `stop`         | —                              | Stop the running test and compute final statistics                           |
| `-i`         | `info`         | —                              | Display current settings                                                     |
| `-h`         | `help`         | —                              | Display help and examples                                                    |
| `-q`         | `quit`         | —                              | Quit both consumer and provider                                              |
| `-q=1`       | `quit=1`       | —                              | Quit the local consumer only (provider keeps running)                        |

> [!NOTE]
> Parameters `-m`, `-c`, `-w`, `-d`, `-o` can be set independently at any time and
> remain in effect for subsequent `-s` calls. To change a parameter mid-run: stop
> with `-p`, update the parameter, then restart with `-s`.

#### Modes

**Ping-Pong modes** — consumer drives one request at a time, measures RTT:

| Mode      | Extra payload |
|:----------|:-------------:|
| `pp0`     | 0 bytes       |
| `pp8`     | 8 bytes       |
| `pp16`    | 16 bytes      |
| `pp32`    | 32 bytes      |
| `pp64`    | 64 bytes      |
| `pp128`   | 128 bytes     |
| `pp256`   | 256 bytes     |
| `pp512`   | 512 bytes     |
| `pp1024`  | 1 024 bytes   |
| `pp4096`  | 4 096 bytes   |
| `pp65536` | 65 536 bytes  |

**Broadcast modes** — provider drives messages at a fixed rate, consumer measures one-way latency:

| Mode      | Extra payload |
|:----------|:-------------:|
| `bc0`     | 0 bytes       |
| `bc8`     | 8 bytes       |
| `bc16`    | 16 bytes      |
| `bc32`    | 32 bytes      |
| `bc64`    | 64 bytes      |
| `bc128`   | 128 bytes     |
| `bc256`   | 256 bytes     |
| `bc512`   | 512 bytes     |
| `bc1024`  | 1 024 bytes   |
| `bc4096`  | 4 096 bytes   |
| `bc65536` | 65 536 bytes  |

Every message in both modes carries an `id` (sequence number) and a `begin` timestamp
in addition to the extra payload bytes shown above.

---

### Provider Command Reference

Commands are entered interactively in the `30_pubprovider` console.

| Command | Long form | Description                          |
|:--------|:----------|:-------------------------------------|
| `-q`    | `quit`    | Quit the application                 |
| `-i`    | `info`    | Redraw the live stats display        |
| `-h`    | `help`    | Redraw the layout and help text      |

The provider is fully passive — it responds to requests and starts broadcasting
as directed by the consumer's `start_mode` call. No upfront configuration is needed.

---

### Benchmark Recipes

> [!TIP]
> Type multiple options on a single line. After each test, results appear on the consumer
> console and are saved to a CSV file. The last 8 test results accumulate in a scrolling
> table — compare rows directly to see how RTT or one-way latency changes across modes.

---

#### Recipe 1 — Baseline ping-pong RTT (0-byte payload)

> **Target:** Measure the irreducible IPC round-trip cost with no user payload.

```
-s -m=pp0 -c=1000 -w=50
```

**What this does:** Sends 1,000 ping-pong requests (after a 50-message warmup) with no
extra data. The measured RTT contains only AREG SDK framing, TCP loopback, `mtrouter`
routing, and dispatch overhead — the **baseline latency** of the IPC stack.

---

#### Recipe 2 — Payload sweep (ping-pong)

> **Target:** Measure how RTT grows with payload size.

Run each mode in sequence and compare P50 / P99 in the results table:

```
-s -m=pp0    -c=1000 -w=50
-s -m=pp8    -c=1000 -w=50
-s -m=pp64   -c=1000 -w=50
-s -m=pp512  -c=1000 -w=50
-s -m=pp1024 -c=1000 -w=50
-s -m=pp4096 -c=1000 -w=50
-s -m=pp65536 -c=500 -w=20
```

The consumer's results table retains the last 8 runs — compare P50 and P99 rows to
see whether extra payload bytes shift the distribution or only raise the tail.

---

#### Recipe 3 — Baseline broadcast one-way latency

> **Target:** Measure end-to-end delivery time in fire-and-forget mode.

```
-s -m=bc0 -c=1000 -d=5
```

**What this does:** Requests the provider to broadcast 1,000 messages per second for
5 seconds (5,000 samples). One-way latency is timestamped at the provider's
`broadcast_message` call and at the consumer's handler entry.

---

#### Recipe 4 — High-rate broadcast (scheduler jitter test)

> **Target:** Find where scheduling jitter starts dominating one-way latency.

```
-s -m=bc0 -c=5000 -d=5
```

At 5,000 msg/s the inter-message interval is 200 µs. Compare P50 and P99 against
Recipe 3 (1,000 msg/s, interval = 1 ms). Rising P99 at higher rates indicates that
the OS scheduler cannot maintain the requested interval precisely.

---

#### Recipe 5 — Large-payload broadcast (serialization cost)

> **Target:** Isolate the serialization and framing cost at 64 KB.

```
-s -m=bc65536 -c=100 -d=5
```

At 100 msg/s the inter-message interval is 10 ms — comfortably above scheduler noise —
so the latency increase relative to `bc0` (Recipe 3) isolates the cost of serializing,
transmitting, and deserializing a 64 KB payload through the full IPC stack.

---

## Detailed Results by Platform

> [!NOTE]
> Tables below will be populated after optimization is complete.
> All latency values are in **microseconds (µs)**.
> Test configuration: ping-pong — 1,000 samples, 50 warmup;
> broadcast — 1,000 msg/s, 5 seconds.

---

### Windows 11 — Intel i7-13700H, DDR4

#### Ping-Pong RTT (µs)

| Payload   | Min | P50 | P95 | P99 | Max | Mean |
|:----------|:---:|:---:|:---:|:---:|:---:|:----:|
| 0 B       | — | — | — | — | — | — |
| 8 B       | — | — | — | — | — | — |
| 16 B      | — | — | — | — | — | — |
| 32 B      | — | — | — | — | — | — |
| 64 B      | — | — | — | — | — | — |
| 128 B     | — | — | — | — | — | — |
| 256 B     | — | — | — | — | — | — |
| 512 B     | — | — | — | — | — | — |
| 1 KB      | — | — | — | — | — | — |
| 4 KB      | — | — | — | — | — | — |
| 64 KB     | — | — | — | — | — | — |

#### Broadcast One-Way Latency (µs)

| Payload   | Min | P50 | P95 | P99 | Max | Mean |
|:----------|:---:|:---:|:---:|:---:|:---:|:----:|
| 0 B       | — | — | — | — | — | — |
| 8 B       | — | — | — | — | — | — |
| 16 B      | — | — | — | — | — | — |
| 32 B      | — | — | — | — | — | — |
| 64 B      | — | — | — | — | — | — |
| 128 B     | — | — | — | — | — | — |
| 256 B     | — | — | — | — | — | — |
| 512 B     | — | — | — | — | — | — |
| 1 KB      | — | — | — | — | — | — |
| 4 KB      | — | — | — | — | — | — |
| 64 KB     | — | — | — | — | — | — |

---

### WSL2 Ubuntu — Intel i7-13700H, DDR4 ¹

#### Ping-Pong RTT (µs)

| Payload   | Min | P50 | P95 | P99 | Max | Mean |
|:----------|:---:|:---:|:---:|:---:|:---:|:----:|
| 0 B       | — | — | — | — | — | — |
| 8 B       | — | — | — | — | — | — |
| 16 B      | — | — | — | — | — | — |
| 32 B      | — | — | — | — | — | — |
| 64 B      | — | — | — | — | — | — |
| 128 B     | — | — | — | — | — | — |
| 256 B     | — | — | — | — | — | — |
| 512 B     | — | — | — | — | — | — |
| 1 KB      | — | — | — | — | — | — |
| 4 KB      | — | — | — | — | — | — |
| 64 KB     | — | — | — | — | — | — |

#### Broadcast One-Way Latency (µs)

| Payload   | Min | P50 | P95 | P99 | Max | Mean |
|:----------|:---:|:---:|:---:|:---:|:---:|:----:|
| 0 B       | — | — | — | — | — | — |
| 8 B       | — | — | — | — | — | — |
| 16 B      | — | — | — | — | — | — |
| 32 B      | — | — | — | — | — | — |
| 64 B      | — | — | — | — | — | — |
| 128 B     | — | — | — | — | — | — |
| 256 B     | — | — | — | — | — | — |
| 512 B     | — | — | — | — | — | — |
| 1 KB      | — | — | — | — | — | — |
| 4 KB      | — | — | — | — | — | — |
| 64 KB     | — | — | — | — | — | — |

---

### macOS — Apple M3 Pro, LPDDR5

#### Ping-Pong RTT (µs)

| Payload   | Min | P50 | P95 | P99 | Max | Mean |
|:----------|:---:|:---:|:---:|:---:|:---:|:----:|
| 0 B       | — | — | — | — | — | — |
| 8 B       | — | — | — | — | — | — |
| 16 B      | — | — | — | — | — | — |
| 32 B      | — | — | — | — | — | — |
| 64 B      | — | — | — | — | — | — |
| 128 B     | — | — | — | — | — | — |
| 256 B     | — | — | — | — | — | — |
| 512 B     | — | — | — | — | — | — |
| 1 KB      | — | — | — | — | — | — |
| 4 KB      | — | — | — | — | — | — |
| 64 KB     | — | — | — | — | — | — |

#### Broadcast One-Way Latency (µs)

| Payload   | Min | P50 | P95 | P99 | Max | Mean |
|:----------|:---:|:---:|:---:|:---:|:---:|:----:|
| 0 B       | — | — | — | — | — | — |
| 8 B       | — | — | — | — | — | — |
| 16 B      | — | — | — | — | — | — |
| 32 B      | — | — | — | — | — | — |
| 64 B      | — | — | — | — | — | — |
| 128 B     | — | — | — | — | — | — |
| 256 B     | — | — | — | — | — | — |
| 512 B     | — | — | — | — | — | — |
| 1 KB      | — | — | — | — | — | — |
| 4 KB      | — | — | — | — | — | — |
| 64 KB     | — | — | — | — | — | — |

---

### Linux bare-metal — x86-64, DDR4/DDR5

#### Ping-Pong RTT (µs)

| Payload   | Min | P50 | P95 | P99 | Max | Mean |
|:----------|:---:|:---:|:---:|:---:|:---:|:----:|
| 0 B       | — | — | — | — | — | — |
| 8 B       | — | — | — | — | — | — |
| 16 B      | — | — | — | — | — | — |
| 32 B      | — | — | — | — | — | — |
| 64 B      | — | — | — | — | — | — |
| 128 B     | — | — | — | — | — | — |
| 256 B     | — | — | — | — | — | — |
| 512 B     | — | — | — | — | — | — |
| 1 KB      | — | — | — | — | — | — |
| 4 KB      | — | — | — | — | — | — |
| 64 KB     | — | — | — | — | — | — |

#### Broadcast One-Way Latency (µs)

| Payload   | Min | P50 | P95 | P99 | Max | Mean |
|:----------|:---:|:---:|:---:|:---:|:---:|:----:|
| 0 B       | — | — | — | — | — | — |
| 8 B       | — | — | — | — | — | — |
| 16 B      | — | — | — | — | — | — |
| 32 B      | — | — | — | — | — | — |
| 64 B      | — | — | — | — | — | — |
| 128 B     | — | — | — | — | — | — |
| 256 B     | — | — | — | — | — | — |
| 512 B     | — | — | — | — | — | — |
| 1 KB      | — | — | — | — | — | — |
| 4 KB      | — | — | — | — | — | — |
| 64 KB     | — | — | — | — | — | — |

---

## Interpreting Results

### Ping-Pong RTT

RTT = *t4 − t1*, where:
- *t1* — captured on the consumer immediately before calling `request_ping_pong_*`
- *t2* — captured on the provider inside the request handler, immediately before calling `response_ping_pong_*`
- *t4* — captured on the consumer immediately upon entering `response_ping_pong_*`

Both *t1* and *t4* are from the same consumer process's `std::chrono::steady_clock`,
so RTT = *t4 − t1* is exact. The intermediate value *t2 − t1* approximates the
client-to-server one-way time using the shared machine hardware clock.

The RTT covers the full nine-step path:

```
consumer serialize → TCP write → mtrouter receive → mtrouter route → TCP write →
provider receive → provider deserialize → handler → serialize response →
TCP write → mtrouter receive → mtrouter route → TCP write →
consumer receive → consumer deserialize → response handler
```

### Broadcast One-Way Latency

One-way = *t4 − t1*, where:
- *t1* — captured on the provider immediately before calling `broadcast_message_*`
- *t4* — captured on the consumer immediately upon entering `broadcast_message_*`

Both processes run on the same physical machine. `std::chrono::steady_clock` reads
the same underlying hardware clock (TSC on x86-64, `mach_absolute_time` on Apple
Silicon) without inter-process clock drift, making the *t4 − t1* delta accurate.

### How to Read the Percentile Tables

| Metric   | Meaning                                                                                    |
|:---------|:------------------------------------------------------------------------------------------|
| **Min**  | Best-case delivery — OS scheduler and TCP cooperated perfectly                             |
| **P50**  | Median latency — half of all messages were faster; the "typical" case                     |
| **P95**  | 95th percentile — only 5% of messages took longer than this                               |
| **P99**  | 99th percentile — tail latency; the value to use for real-time deadline analysis           |
| **Max**  | Worst-case delivery — a single OS scheduling or TCP ACK hiccup                            |
| **Mean** | Arithmetic average; skewed upward when outliers are present; less representative than P50 |

> [!TIP]
> For real-time systems, design to **P99**, not P50. The gap between P50 and P99
> reveals how much jitter the OS scheduler and TCP stack introduce on your platform.

---

## Use Cases

- **IPC latency baseline** — Establish the minimum round-trip cost of service-oriented
  communication on your target hardware before designing time-sensitive pipelines.
- **Payload sensitivity analysis** — Determine whether your message size meaningfully
  increases latency or whether the constant framing and routing overhead dominates
  across the full 0 B – 64 KB range.
- **Broadcast vs. request/response trade-off** — Compare one-way broadcast delivery
  latency against the corresponding ping-pong RTT to choose the right communication
  pattern for your application's timing requirements.
- **Latency regression testing** — Re-run the benchmark suite after upgrading hardware,
  OS kernel, or AREG SDK to detect latency regressions before they reach production.
- **Real-time system validation** — Use P99 results to confirm whether AREG SDK IPC
  meets the soft deadlines of your application: control loops, sensor fusion,
  UI refresh cycles, or safety-critical notification paths.

---

## Takeaway

> Tables will be populated after optimization is complete.

**Baseline IPC latency (0-byte payload, 1:1 via `mtrouter`):**

| Platform                      | RTT P50 | RTT P99 | One-Way P50 | One-Way P99 |
|:------------------------------|:-------:|:-------:|:-----------:|:-----------:|
| Windows 11 (i7-13700H, DDR4)  | — µs | — µs | — µs | — µs |
| WSL2 Ubuntu (i7-13700H, DDR4) | — µs | — µs | — µs | — µs |
| macOS (M3 Pro, LPDDR5)        | — µs | — µs | — µs | — µs |
| Linux bare-metal (x86-64)     | — µs | — µs | — µs | — µs |

These numbers include the full AREG SDK service stack — service discovery, message
routing, event dispatching, and multithreading — with no shortcuts taken.
**This is what your production system gets.**
