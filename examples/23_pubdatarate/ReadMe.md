# 23_pubdatarate

🚀 **Type:** IPC / Multi-Process

## Overview

Distributed systems demand frameworks that stay out of the way when data moves fast.
**23_pubdatarate** stress-tests exactly that: a **Public Service Provider** streams
continuous bitmap payloads to one or more **Service Consumers** across process
boundaries, while the framework routes, delivers, and tracks every message — all
without a dedicated networking library underneath.

The result on a single machine with a **mobile-class CPU**:

<style>
* { box-sizing: border-box; }
.wrap { padding: 1rem 0; font-size: 12px; color: var(--color-text-primary); }
table { width: 100%; border-collapse: collapse; table-layout: fixed; }
col.c1 { width: 20%; }
col.c2 { width: 14%; }
col.c3 { width: 12%; }
col.c4 { width: 10%; }
col.c5 { width: 13%; }
col.c6 { width: 13%; }
col.c7 { width: 18%; }
th { font-size: 11px; font-weight: 500; color: var(--color-text-secondary); text-align: left; padding: 5px 7px; border-bottom: 0.5px solid var(--color-border-secondary); white-space: nowrap; }
td { padding: 6px 7px; vertical-align: middle; border-bottom: 0.5px solid var(--color-border-tertiary); line-height: 1.4; }
tr:hover td { background: var(--color-background-secondary); }
.section td { background: var(--color-background-tertiary); font-size: 10px; font-weight: 500; color: var(--color-text-secondary); padding: 3px 7px; letter-spacing: 0.04em; text-transform: uppercase; border-bottom: 0.5px solid var(--color-border-secondary); }
.num { font-weight: 500; }
.muted { color: var(--color-text-secondary); font-size: 11px; }
.badge { display: inline-block; font-size: 10px; padding: 1px 5px; border-radius: 3px; font-weight: 500; white-space: nowrap; }
.b-m  { background: #EAF3DE; color: #3B6D11; }
.b-e  { background: #FAEEDA; color: #854F0B; }
.b-tbd { background: #E6F1FB; color: #185FA5; }
@media (prefers-color-scheme: dark) {
  .b-m  { background: #173404; color: #C0DD97; }
  .b-e  { background: #412402; color: #FAC775; }
  .b-tbd { background: #042C53; color: #B5D4F4; }
}
</style>
<div class="wrap">
<table>
<colgroup><col class="c1"><col class="c2"><col class="c3"><col class="c4"><col class="c5"><col class="c6"><col class="c7"></colgroup>
<thead>
<tr>
  <th>Platform</th>
  <th>CPU</th>
  <th>RAM</th>
  <th>Payload</th>
  <th>Throughput / Rate</th>
  <th>Unit</th>
  <th>Status</th>
</tr>
</thead>
<tbody>

<tr class="section"><td colspan="7">— data rate (large payload ~3 MB) —</td></tr>

<tr>
  <td>Windows 11</td>
  <td class="muted">Intel i7-13700H</td>
  <td class="muted">DDR4</td>
  <td>~3 MB</td>
  <td class="num">1.7 – 2.1</td>
  <td>GB/s</td>
  <td><span class="badge b-m">measured</span></td>
</tr>
<tr>
  <td>WSL2 <span class="muted">(on Win11)</span></td>
  <td class="muted">Intel i7-13700H</td>
  <td class="muted">DDR4</td>
  <td>~3 MB</td>
  <td class="num">3.8 – 4.0</td>
  <td>GB/s</td>
  <td><span class="badge b-m">measured</span></td>
</tr>
<tr>
  <td>Linux VM <span class="muted">(on macOS)</span></td>
  <td class="muted">Apple M4 Pro</td>
  <td class="muted">LPDDR5</td>
  <td>~3 MB</td>
  <td class="num">~6.8</td>
  <td>GB/s</td>
  <td><span class="badge b-m">measured</span></td>
</tr>
<tr>
  <td>macOS native</td>
  <td class="muted">Apple M4 Pro</td>
  <td class="muted">LPDDR5</td>
  <td>~3 MB</td>
  <td class="num">~7.0</td>
  <td>GB/s</td>
  <td><span class="badge b-m">measured</span></td>
</tr>
<tr>
  <td>Linux bare-metal</td>
  <td class="muted">x86-64 desktop/server</td>
  <td class="muted">DDR4/DDR5</td>
  <td>~3 MB</td>
  <td class="num">6.0 – 7.5</td>
  <td>GB/s</td>
  <td><span class="badge b-e">estimated</span></td>
</tr>

<tr class="section"><td colspan="7">— message rate (small payload ~3 KB) —</td></tr>

<tr>
  <td>Windows 11</td>
  <td class="muted">Intel i7-13700H</td>
  <td class="muted">DDR4</td>
  <td>~3 KB</td>
  <td class="num">120 – 135K</td>
  <td>msg/s</td>
  <td><span class="badge b-m">measured</span></td>
</tr>
<tr>
  <td>WSL2 <span class="muted">(on Win11)</span></td>
  <td class="muted">Intel i7-13700H</td>
  <td class="muted">DDR4</td>
  <td>~3 KB</td>
  <td class="num">170 – 190K</td>
  <td>msg/s</td>
  <td><span class="badge b-m">measured</span></td>
</tr>
<tr>
  <td>Linux VM <span class="muted">(on macOS)</span></td>
  <td class="muted">Apple M4 Pro</td>
  <td class="muted">LPDDR5</td>
  <td>~3 KB</td>
  <td class="num">180 – 200K</td>
  <td>msg/s</td>
  <td><span class="badge b-m">measured</span></td>
</tr>
<tr>
  <td>macOS native</td>
  <td class="muted">Apple M4 Pro</td>
  <td class="muted">LPDDR5</td>
  <td>~3 KB</td>
  <td class="num">240 – 290K</td>
  <td>msg/s</td>
  <td><span class="badge b-m">measured</span></td>
</tr>
<tr>
  <td>Linux bare-metal</td>
  <td class="muted">x86-64 desktop/server</td>
  <td class="muted">DDR4/DDR5</td>
  <td>~3 KB</td>
  <td class="num">300 – 340K</td>
  <td>msg/s</td>
  <td><span class="badge b-e">estimated</span></td>
</tr>

<tr class="section"><td colspan="7">— message rate (very small payload ~0.5 KB) —</td></tr>

<tr>
  <td>Windows 11</td>
  <td class="muted">Intel i7-13700H</td>
  <td class="muted">DDR4</td>
  <td>~0.5 KB</td>
  <td class="num">150 – 160K</td>
  <td>msg/s</td>
  <td><span class="badge b-m">measured</span></td>
</tr>
<tr>
  <td>WSL2 <span class="muted">(on Win11)</span></td>
  <td class="muted">Intel i7-13700H</td>
  <td class="muted">DDR4</td>
  <td>~0.5 KB</td>
  <td class="num">210 – 220K</td>
  <td>msg/s</td>
  <td><span class="badge b-m">measured</span></td>
</tr>
<tr>
  <td>Linux VM <span class="muted">(on macOS)</span></td>
  <td class="muted">Apple M4 Pro</td>
  <td class="muted">LPDDR5</td>
  <td>~0.5 KB</td>
  <td class="num">—</td>
  <td class="muted">not yet run</td>
  <td><span class="badge b-tbd">needed</span></td>
</tr>
<tr>
  <td>macOS native</td>
  <td class="muted">Apple M4 Pro</td>
  <td class="muted">LPDDR5</td>
  <td>~0.5 KB</td>
  <td class="num">—</td>
  <td class="muted">not yet run</td>
  <td><span class="badge b-tbd">needed</span></td>
</tr>
<tr>
  <td>Linux bare-metal</td>
  <td class="muted">x86-64 desktop/server</td>
  <td class="muted">DDR4/DDR5</td>
  <td>~0.5 KB</td>
  <td class="num">—</td>
  <td class="muted">community target</td>
  <td><span class="badge b-tbd">needed</span></td>
</tr>

</tbody>
</table>
<p style="font-size:11px;color:var(--color-text-tertiary);margin-top:10px;line-height:1.5;">
TCP localhost, 1:1 connection (single provider → single consumer) via <code>mtrouter</code>.
macOS and Linux VM results use Apple M4 Pro unified LPDDR5 memory.
Windows and WSL2 results use Intel i7-13700H with DDR4.
Bare-metal Linux values are estimated from WSL2 measurements.
All tests: Areg SDK with full service dispatch, built-in threading, automatic framing.
Reported ranges reflect the most frequently observed values; peak readings may be higher.
</p>
</div>

These numbers come from a full-stack, service-oriented pipeline — location-transparent
service discovery, type-safe IPC, automatic reconnection — none of which is stripped
away for the benchmark. What you measure here is what your production system gets.

**Use this example to:**

- **Stress-test** your hardware ceiling — when the consumer pipeline saturates, memory grows unbounded, giving you a clear upper bound.
- **Profile** data-intensive pipelines before committing to an architecture.
- **Validate** that the framework overhead is negligible for your use case.
- **Experiment** with subscriber counts, payload sizes, and channel counts to find your system's throughput ceiling.

> [!NOTE]
> This example requires **Multi-Target Router (`mtrouter`)** for inter-process message
> routing. Ensure `mtrouter` is running before starting provider or consumer processes.
> The `areg.init` file must have the correct IP address and port configured.

> [!TIP]
> **Monitor real network traffic** in the `mtrouter` console — it displays live
> incoming and outgoing byte rates, giving you the ground truth of what actually
> travels over the socket.

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
| **[23_pubclient](./pubclient/)** | Consumer | Receives bitmap frames, displays real-time receive rates |

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

The provider generates grayscale bitmap frames based on user parameters and
publishes them as **Object RPC broadcasts** via `mtrouter`. The router forwards
the raw buffer to every connected consumer without re-serialization — it is a
buffer relay, not a message broker.

Both the provider and the consumer independently measure and display transfer
rates in their consoles. The provider shows **send rate**; the consumer shows
**receive rate**. The `mtrouter` console shows both simultaneously as
**incoming and outgoing byte rates** — the most reliable real-world throughput
indicator.

The Areg Framework handles service discovery automatically via `service_connected()`,
reconnection after process restart, and thread dispatch on the consumer side.

---

## Running the Benchmark

> [!IMPORTANT]
> Executable extensions differ by platform: `.exe` on Windows, `.elf` on Linux,
> `.mac` on macOS. For example: `mtrouter.exe` / `mtrouter.elf` / `mtrouter.mac`.
> The commands below omit extensions for brevity — apply the correct one for your platform.

### Step 1: Start `mtrouter`
Start the router first. It must be running before provider or consumer attempt
to connect.

```bash
# Linux
./mtrouter.elf --console

#  macOS
./mtrouter.mac --console

# Windows
.\mtrouter.exe --console
```

Leave the `mtrouter` console visible — it shows live byte rates that confirm
actual socket-level throughput.

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

Start the provider in its own terminal. Once started, use the interactive
commands below to configure and start the stream.

```bash
# Linux
./23_pubservice.elf

# macOS
./23_pubservice.mac

# Windows
.\23_pubservice.exe
```

The provider starts in idle state. Use the commands below to configure payload
size and rate, then type `-s` to begin streaming.

---

### Provider Command Reference

All commands are entered interactively in the `23_pubservice` console while
the application is running.

| Command | Long form | Range | Description |
|---|---|---|---|
| `-w=<N>` | `--width=<N>` | 32 – 32768 | Frame width in pixels |
| `-h=<N>` | `--height=<N>` | 32 – 32768 | Frame height in pixels |
| `-l=<N>` | `--lines=<N>` | 1 – height | Lines per transmitted block; `height` = one full frame per message |
| `-t=<N>` | `--time=<N>` | 0 – 100000 | Inter-message delay in nanoseconds; `0` = full speed |
| `-c=<N>` | `--channels=<N>` | 1 – 96 | Parallel channels (multiplier for throughput and message rate) |
| `-i` | `--info` | — | Display current configuration |
| `--help` | `--help` | — | Display command help |
| `-s` | `--start` | — | Start streaming |
| `-p` | `--stop` | — | Stop streaming |
| `-q` | `--quit` | — | Quit the application |

> [!NOTE]
> Parameters `-w`, `-h`, `-l`, `-t`, `-c` must be set **before** typing `-s`.
> Changing them while streaming requires `-p` (stop), reconfigure, then `-s` (start).

**How channels affect throughput:**

Each channel (`-c`) is an independent data stream. Increasing `-c` scales both
data rate and message rate proportionally. The theoretical throughput is:

```
data_rate  = (width × height × bytes_per_pixel) / pixel_time_ns × channels × 1e9
```

In practice, measured rates are slightly below theoretical due to scheduling and
socket buffering.

---

### Benchmark Recipes

The following commands reproduce the measurements in the table above.
Enter them in the `23_pubservice` console, then type `-s` to start.

---

#### Large payload — data rate benchmark (~3 MB per message)

```
-w=1024 -h=1024 -l=1024 -t=25 -c=20
```

**What this does:** Sends full 1024×1024 frames as single blocks (~3 MB each).
`-t=25` sets a 25 ns inter-message delay. `-c=20` runs 20 parallel channels,
creating a theoretical rate of ~2.4 GB/s (typically ~2.1–2.3 GB/s measured).

**To push higher:** Increase channels. `-c=30` targets ~3.6 GB/s (typically
~3.5 GB/s measured). The ceiling is when the consumer runs out of memory and
is killed by the OS — that is your hardware's upper bound.

| `-c` value | Theoretical rate | Typical measured |
|---|---|---|
| 10 | ~1.2 GB/s | ~1.1 GB/s |
| 20 | ~2.4 GB/s | ~2.1–2.3 GB/s |
| 30 | ~3.6 GB/s | ~3.4–3.5 GB/s |

---

#### Small payload — message rate benchmark (~3 KB per message)

```
-w=1024 -h=1024 -l=1 -t=25 -c=4
```

**What this does:** Sends single-line slices of a 1024-wide frame (~3 KB each).
`-c=4` creates ~155–156K messages per second.

**To scale:** Adjust `-c` to increase message rate, or `-t` to throttle.
Increasing `-t` reduces rate; decreasing it (toward 0) increases rate up to the
hardware ceiling.

| `-c` value | Typical rate |
|---|---|
| 2 | ~75–80K msg/s |
| 4 | ~150–160K msg/s |
| 6 | ~220–240K msg/s |

---

#### Very small payload — high message rate benchmark (~0.5 KB per message)

```
-w=128 -h=128 -l=1 -t=38 -c=1
```

**What this does:** Sends single-line slices of a 128×128 frame (~500 bytes each).
`-t=38` with `-c=1` produces ~200K messages per second.

**To scale:** Increase `-c` to multiply message rate, or reduce `-t` to push
faster. This recipe isolates per-message overhead from payload size.

---

## Interpreting Results

**Where to read throughput:**

| Source | What it shows |
|---|---|
| `23_pubservice` console | Send rate (framework → socket) |
| `23_pubclient` console | Receive rate (socket → application) |
| `mtrouter` console | Raw socket throughput — most accurate ground truth |

**Why the numbers differ slightly:**

- The provider measures bytes queued for send, which may briefly lead the actual
  socket transmission.
- The consumer measures bytes delivered to the application thread, which lags
  socket receipt by one dispatch cycle.
- `mtrouter` measures bytes crossing the socket boundary in both directions —
  this is the cleanest throughput signal.

**Understanding the ceiling:**

When throughput approaches the hardware ceiling, the consumer's internal queue
grows faster than it can drain. Memory usage climbs steadily. The OS eventually
kills the consumer process when memory is exhausted. This is expected behavior —
it defines the upper bound for your specific hardware configuration.

**What is NOT measured here:**

This benchmark measures socket-level throughput, not end-to-end application
latency. The time from `broadcast_image_block_acquired()`/ `broadcast_image_block_acquired` on the provider to `on_broadcast_image_block_acquired()`
on the consumer is higher than the raw socket rate implies, due to the two-hop
path (provider → mtrouter → consumer) and thread dispatch on both sides.
Dedicated latency measurement requires a separate timestamped test.

---

## Use Cases

- **Throughput validation** — Confirm the framework imposes negligible overhead
  before committing to a service-oriented architecture for data-intensive pipelines.
- **Hardware profiling** — Determine the practical IPC ceiling of your target
  hardware before production deployment.
- **Architecture sizing** — Choose payload size, channel count, and consumer
  topology based on measured rather than theoretical numbers.
- **Scientific / industrial imaging** — Directly applicable to laser microscopy,
  X-ray, electron microscopy, and machine vision pipelines that move multi-MB
  frames continuously between acquisition and processing processes.

---

## Takeaway

If framework transport overhead is your concern, this example answers it directly.
On a mobile-class CPU (Intel i7-13700H), the overhead is negligible up to ~4 GB/s
on Linux. Beyond that, the bottleneck is your hardware — not the framework. On
Apple M4 Pro, the ceiling rises to ~7 GB/s. Bare-metal x86-64 Linux is estimated
at 6–7.5 GB/s.

These are not raw socket numbers. They include full service discovery, type-safe
serialization, automatic reconnection, threading dispatch, and message framing.
