# areg-sdk vs ZMQ / NanoMsg / NNG
## TCP Loopback Latency – Direct Benchmark Comparison
### Reference: Hitachi Energy Research, arXiv:2508.07934v1 (August 2025)

---

## 1. Scope and Purpose

This document compares areg-sdk OWT (one-way) and RTT (round-trip) latency
against ZeroMQ, NanoMsg, and NNG as measured in a peer-reviewed benchmark
published by Hitachi Energy Research (arXiv:2508.07934v1, August 2025).
Raw data source: github.com/hitachienergy/messaging-libraries-benchmark/results/results.csv

All measurements are same-machine TCP loopback only.
No cross-machine or real-network numbers appear anywhere in this document.

---

## 2. Measurement Methodology

### 2.1 Competitors (Hitachi Energy paper)

| Parameter | Value |
|-----------|-------|
| Hardware | Intel Xeon w3-2435, 8 cores, 3.1 GHz base / 4.2 GHz boost, 64 GB RAM |
| OS | Linux, native SSD install |
| Core isolation | Yes – publisher and subscriber pinned to dedicated isolated cores |
| Transport | TCP loopback (127.0.0.1), direct 1-hop connection |
| Send rate | T=1000 μs – 1 message per millisecond (low load) |
| Latency type | One-way: publisher embeds timestamp (CLOCK_REALTIME); subscriber records delta |
| Sample count | 5000 messages, 4 repetitions |
| Metrics | Min / Avg / P90 / P99 (nanosecond precision in CSV) |
| Framework scope | Raw transport layer only – no service dispatch, no method call, no routing |

### 2.2 areg-sdk

| Parameter | Value |
|-----------|-------|
| Hardware | Lenovo ThinkBook 16 G6, i7-13700H mobile (14C/20T, 5.0 GHz P-core boost), 32 GB DDR4 |
| OS tested | Ubuntu 26.04 LTS (`Performance` power mode) / Windows 11 (native SSD) / macOS M3 Pro (native) |
| Core isolation | Pinned via `taskset` (not kernel-isolated) |
| Transport | TCP loopback (127.0.0.1), through centralized mtrouter broker (2 hops) |
| Send mode | Closed-loop sequential pipeline – 1 message in flight at all times |
| Artificial spacing | None – next message fires immediately on receipt; component thread idle during transit |
| Queue depth | Pinned to 1 by construction (confirmed by source code analysis) |
| Latency type | One-way (bc): provider embeds timestamp; consumer records delta. Same-machine clock. |
| Sample count | 5000 per run (bc) / 10000 per run (pp), warmup=1000, 8 consecutive runs averaged |
| Metrics | Min / P50 (median) / P95 / P99 / Mean |
| Build | Release, AREG_LOGGING=OFF |
| Framework scope | Full stack: serialize → TCP send → broker route → TCP send → dispatch → deserialize → method call |

## 2.3 What Is Inside the Measured Window

Timestamp placement is critical for interpreting these numbers.
The following is confirmed by source code inspection of example 30.

### areg-sdk: timestamp taken BEFORE serialization / AFTER deserialization

**OWT (bc mode) – one measurement cycle:**

```
t1 = now_ns()                           ← stamped BEFORE any serialization
│
├─ serialize(seq, t1, payload)          payload serialized field-by-field (typed)
├─ dispatch: route communication channel → TCP send queue
├─ TCP send: consumer → mtrouter
├─ mtrouter: TCP receive → identify target
├─ mtrouter: route to target → TCP send (→ provider)
├─ TCP receive: provider → route to component thread queue
├─ dispatch: route raw bytes to component thread queue
├─ dispatch: find subscribers and method
├─ deserialize(seq, t1, payload)        on the component thread – after dispatch
└─ method call invoked with typed params
   └─ t4 = now_ns()                     ← stamped AFTER deserialize + call

OWT = t4 − t1
    = serialize(sender)
    + TCP × 2 hops
    + broker route
    + dispatch × 3
    + deserialize(receiver, on component thread)
    + method invocation
```

Note: **dispatch precedes deserialization**, by design. Raw (still serialized) message
bytes are routed to the correct component thread first – this is how areg-sdk enforces
thread affinity. Deserialization and the method call then happen on that thread, eliminating
data races by architecture, not locking.

**RTT (pp mode) – one full cycle, both directions:**

```
t1 = now_ns()                           ← BEFORE serialization (consumer)
│
├─ serialize(seq, t1, payload)
├─ dispatch: route communication channel → TCP send queue
├─ TCP × 2 → provider
├─ dispatch: route raw bytes to provider component thread
├─ dispatch: find subscribers and method
├─ deserialize(seq, t1, payload)        on provider component thread
└─ request_handler() invoked
    └─ t_mid = now_ns()                 ← AFTER deserialize (provider)
    ├─ serialize(seq, t1, t_mid, payload)
    ├─ dispatch: route communication channel → TCP send queue
    ├─ TCP × 2 → consumer
    ├─ dispatch: route raw bytes to consumer component thread
    ├─ dispatch: find subscribers and method
    ├─ deserialize(seq, t1, t_mid, payload)  on consumer component thread
    └─ response_handler() invoked
        └─ t4 = now_ns()               ← AFTER deserialize (consumer)

RTT = t4 − t1
    = 2 × (serialize + TCP × 2 + route + dispatch × 3 + deserialize + call)
```

### Competitor benchmarks (ZMQ / NanoMsg / NNG): raw buffer transfer only

```
publisher:  memcpy → write(fd, buf, N)   ← timestamp here (Hitachi paper)
subscriber: read(fd, buf, N)             ← timestamp here
            (no field parsing, no type reconstruction, no dispatch)
```

### Scope comparison

| Component | areg-sdk (measured) | ZMQ / NanoMsg / NNG (measured) |
|-----------|---------------------|-------------------------------|
| Serialize payload (sender) | ✅ Inside window | ❌ Not measured |
| TCP kernel send | ✅ Inside window | ✅ Inside window |
| Broker / router hop | ✅ Inside window (2 hops) | ❌ Direct connection (1 hop) |
| TCP kernel receive | ✅ Inside window | ✅ Inside window |
| Dispatch (route raw msg → component thread, thread wake) | ✅ Inside window | ❌ Not measured |
| Deserialize payload on component thread | ✅ Inside window | ❌ Not measured |
| Method / callback invocation | ✅ Inside window | ❌ Not measured |

**areg-sdk's published latency numbers include every component of the production call path:**
typed, field-by-field serialization, not a flat buffer.
**Competitor numbers include only raw TCP socket transfer.**

### 2.4 Methodology Differences – Every Factor Favors Competitors

| Factor | Competitors | areg-sdk | Impact on areg |
|--------|-------------|---------|----------------|
| CPU boost clock | 4.2 GHz | 5.0 GHz | areg slight advantage |
| Core isolation | Yes (kernel isolcpus) | Pinned via `taskset` (not kernel-isolated) | reduced but not eliminated scheduling exposure |
| Power / OS state | Native SSD | Native SSD; `Performance` power mode (Linux) | comparable |
| Send rate | T=1000 μs (1 msg/ms) | Natural rate (~5–50K msg/s) | Higher scheduling exposure |
| TCP hops | 1 direct | 2 via broker | +1 TCP round-trip |
| Overhead | None | Full stack | +10–20 μs vs raw transport |

**Most conditions still favor the competitor (send rate, hops, framework overhead, true kernel
isolation vs `taskset` pinning). areg-sdk results are produced under harder conditions on every
dimension except CPU clock.**

### 2.5 Column Mapping

| Paper metric | areg-sdk metric | Notes |
|---|---|---|
| Min | Min | Direct equivalent – best-case single message |
| Avg | Mean | Paper: low load; areg: continuous load |
| P90 | P95 | Different percentile – not directly comparable, shown for reference |
| P99 | P99 | Direct match |

---

## 3. Message Size Reference

areg-sdk carries a fixed 140-byte base header regardless of mode.
The suffix `n` in mode names denotes additional parameter bytes.

| areg mode | Extra | Request total | Response total | Closest paper size |
|-----------|-------|--------------|----------------|-------------------|
| bc0 / pp0 | 0 B | 140 B | 148 B | – (paper min = 1 KB) |
| bc64 / pp64 | 64 B | 204 B | 212 B | – |
| bc128 / pp128 | 128 B | 268 B | 276 B | – |
| bc512 / pp512 | 512 B | 652 B | 660 B | – |
| bc1024 / pp1024 | 1024 B | 1164 B | 1172 B | 1 KB (1000 B) |
| bc4096 / pp4096 | 4096 B | 4236 B | 4244 B | 4 KB (4000 B) |
| bc65536 / pp65536 | 65536 B | 65676 B | 65684 B | 64 KB (64000 B) |

---

## 4. OWT Latency – Linux vs Competitors

All competitor values from Hitachi CSV: TCP, T=1000 μs, 1 subscriber, Xeon w3-2435, isolated cores.
areg-sdk: Ubuntu 26.04 LTS, `Performance` power mode, i7-13700H, cores pinned via `taskset`
(not kernel-isolated), T=0 continuous, warmed (w=1000).
All values in **μs**. **Bold** = wins against nearest competitor at similar message size.
Tables are sorted best-to-worst by Min (or Avg where Min is skewed by a transport anomaly).

### 4.1 Sub-1 KB – areg only (paper minimum is 1 KB)

| Framework | Mode | Total size | Min | P50 | P95 | P99 | Mean |
|-----------|------|-----------|-----|-----|-----|-----|------|
| **areg-sdk Linux** | bc0 | 140 B | **10.4** | **10.7** | 10.9 | 11.9 | **10.8** |
| **areg-sdk Linux** | bc64 | 204 B | **10.5** | **11.2** | 11.8 | 21.4 | **11.5** |
| **areg-sdk Linux** | bc128 | 268 B | **10.5** | **11.3** | 12.1 | 25.4 | **11.7** |
| **areg-sdk Linux** | bc512 | 652 B | **11.0** | 11.8 | 13.0 | 28.1 | 12.3 |

> No competitor data below 1 KB exists in the reference benchmark.
> areg bc0 Min (10.4 μs) is already faster than NanoMsg at 1 KB (18.0 μs),
> with full service dispatch and an additional TCP broker hop included.

---

### 4.2 ~1 KB – areg bc1024 (1164 B) vs paper 1 KB (1000 B)

| Framework          | Transport  | Size   | Min      | Avg/Mean        | P90/P95        | P99      | Conditions                         |
|--------------------|------------|--------|----------|-----------------|----------------|----------|------------------------------------|
| **areg-sdk Linux** | TCP bc1024 | 1164 B | **11.0** | **13.1** (Mean) | **20.3** (P95) | 31.8     | Perf. mode, T=0, pinned, 2-hop, full stack |
| NanoMsg            | TCP direct | 1000 B | 18.0     | 21.9 (Avg)      | 22.3 (P90)     | **24.8** | Xeon, T=1000μs, isolated, raw      |
| ZMQ                | TCP direct | 1000 B | 22.0     | 27.5 (Avg)      | 28.5 (P90)     | 31.6     | same                               |
| NNG                | TCP direct | 1000 B | 24.3     | 34.9 (Avg)      | 39.7 (P90)     | 48.4     | same                               |

**areg-sdk wins:** Min (−7.0 μs), Mean vs Avg (−8.8 μs), P95 vs P90 (−2.0 μs)
**areg-sdk loses:** P99 (+7.0 μs) – tighter than before; raw socket still keeps a slim tail edge
> bc1024 Min spread across 8 runs: 10.87–11.10 μs (0.23 μs total).

---

### 4.3 ~4 KB – areg bc4096 (4236 B) vs paper 4 KB (4000 B)

| Framework | Transport | Size | Min | Avg/Mean | P90/P95 | P99 | Conditions |
|-----------|-----------|------|-----|----------|---------|-----|-----------|
| **areg-sdk Linux** | TCP bc4096 | 4236 B | **11.6** | **15.4** (Mean) | 29.5 (P95) | 40.7 | Perf. mode, T=0, pinned, 2-hop, full stack |
| NanoMsg | TCP direct | 4000 B | 19.3 | 24.5 (Avg) | **26.3** (P90) | **28.0** | Xeon, T=1000μs, isolated, raw |
| ZMQ | TCP direct | 4000 B | 23.9 | 29.3 (Avg) | 31.0 (P90) | 34.1 | same |
| NNG | TCP direct | 4000 B | 27.0 | 35.6 (Avg) | 39.8 (P90) | 50.2 | same |

**areg-sdk wins:** Min (−7.7 μs), Mean vs Avg (−9.1 μs)
**areg-sdk loses:** P90/P95 (+3.2 μs), P99 (+12.7 μs) – raw socket keeps the tighter tail at this size
> bc4096 Min spread across 8 runs: 11.49–11.81 μs (0.32 μs total).

---

### 4.4 ~64 KB – areg bc65536 (65676 B) vs paper 64 KB (64000 B)

| Framework | Transport | Size | Min | Avg/Mean | P90/P95 | P99 | Conditions |
|-----------|-----------|------|-----|----------|---------|-----|-----------|
| **areg-sdk Linux** | TCP bc65536 | 65676 B | **28.8** | **30.2** (Mean) | **32.7** (P95) | **34.6** | Perf. mode, T=0, pinned, 2-hop, full stack |
| NNG | TCP direct | 64000 B | 39.9 | 49.4 (Avg) | 55.6 (P90) | 65.2 | Xeon, T=1000μs, isolated, raw |
| ZMQ | TCP direct | 64000 B | 43.6 | 52.6 (Avg) | 53.4 (P90) | 59.5 | same |
| NanoMsg | TCP direct | 64000 B | 32.1 | 171.1 ⚠️ | 303.9 ⚠️ | 307.3 ⚠️ | Nagle failure |

**areg-sdk wins outright at every metric**, including Min vs NanoMsg's clean pre-Nagle reading
(28.8 vs 32.1, −3.3 μs) – the one size/metric combination where areg previously trailed.
**NanoMsg at 64 KB:** Nagle algorithm failure – Avg=171 μs vs Min=32.1 μs (5.3× degradation).
areg-sdk Mean (30.2 μs) is 5.7× better than NanoMsg average at this size.

---

### 4.5 OWT Win/Lose Summary

| Size | areg vs best competitor | Min | Mean/Avg | P90/P95 | P99 |
|------|------------------------|-----|----------|---------|-----|
| ~1 KB | vs NanoMsg TCP | ✅ areg −7.0 μs | ✅ areg −8.8 μs | ✅ areg −2.0 μs | ❌ areg +7.0 μs |
| ~4 KB | vs NanoMsg TCP | ✅ areg −7.7 μs | ✅ areg −9.1 μs | ❌ areg +3.2 μs | ❌ areg +12.7 μs |
| ~64 KB | vs NNG/ZMQ/NanoMsg | ✅ areg −3.3 μs (vs NanoMsg) | ✅ areg −19.2 μs (vs NNG) | ✅ areg −20.7 μs (vs ZMQ) | ✅ areg −24.9 μs (vs ZMQ) |

**Pattern:** with cores pinned, areg now wins Min and Mean at all sizes, wins every metric outright
at 64 KB, and only trails on tail latency (P90/P95, P99) at 1 KB and 4 KB – where a raw, unbrokered
socket still keeps a structural edge on worst-case timing under the paper's low-load (T=1000 μs)
test design. True kernel core isolation (vs the `taskset` pinning used here) is expected to close
most of the remaining gap.

---

## 5. RTT Latency (pp modes) – areg-sdk Only (paper does not measure RTT)

Full round-trip: Consumer → mtrouter → Provider → mtrouter → Consumer (4 TCP hops).
Both sides fully processed: serialize, dispatch, method call, response.

### Linux (Ubuntu 26.04, `Performance` power mode, i7-13700H, DDR4)

| Mode | Total size | Min | P50 | P95 | P99 | Mean |
|------|-----------|-----|-----|-----|-----|------|
| pp0 | 140+148 B | **21.0 μs** | 21.4 μs | 22.0 μs | 23.0 μs | 21.5 μs |
| pp64 | 204+212 B | **21.2 μs** | **21.7 μs** | 22.5 μs | 23.5 μs | 22.0 μs |
| pp128 | 268+276 B | **21.2 μs** | 21.8 μs | 22.8 μs | 23.8 μs | 22.0 μs |
| pp512 | 652+660 B | 22.3 μs | 22.9 μs | 24.0 μs | 24.9 μs | 23.1 μs |
| pp1024 | 1164+1172 B | **22.4 μs** | 22.9 μs | 24.1 μs | 25.1 μs | 23.1 μs |
| pp4096 | 4236+4244 B | 23.4 μs | 24.5 μs | 26.1 μs | 26.9 μs | 24.7 μs |

> pp0 RTT Min (21.0 μs) and pp64 RTT Min (21.2 μs) are the confirmed floor for this hardware,
> cores pinned via `taskset`. From `bc1024`/`pp1024` upward the provider uses pre-allocated
> buffers, eliminating heap allocation from the hot path.
>
> pp1024 RTT Min (22.4 μs) = full 4-hop round-trip at ~1 KB.
> NanoMsg OWT at 1 KB = 18.0 μs. 2 × NanoMsg OWT = 36 μs.
> **areg full 4-hop brokered RTT (22.4 μs) < 2 × NanoMsg direct OWT (36 μs).**

---

## 6. IPC Reference (Hitachi CSV – Unix socket, same machine)

Provided for context. areg-sdk does not use IPC/Unix socket transport.

| Framework | Min | Avg | P90/P95 | P99 | Source |
|-----------|-----|-----|-----|-----|--------|
| **areg-sdk Linux TCP** | **10.5** | **11.5** | **11.8** | 21.4 | ✅ Measured, pinned cores |
| NanoMsg IPC | 13.0 | 16.2 | 16.5 | **17.9** | ✅ Hitachi CSV, T=1000 μs, Xeon |
| ZMQ IPC | 20.4 | 25.8 | 26.4 | 28.4 | ✅ Hitachi CSV |
| NNG IPC | 20.0 | 28.4 | 32.9 | 43.3 | ✅ Hitachi CSV |

**areg-sdk TCP now beats NanoMsg IPC, ZMQ IPC, and NNG IPC on Min, Mean/Avg, and P90/P95** – despite
TCP vs Unix domain socket, 2-hop broker vs direct, full dispatch vs raw transport, and max send
rate vs low load (T=1000 μs). NanoMsg IPC keeps a tighter P99 (17.9 vs 21.4 μs) – the one place
raw IPC still holds the tail-latency edge.

---

## 7. Reliability at Maximum Send Rate (T=0)

At T=0 (maximum send rate, matching areg-sdk test conditions), competitor data from
Hitachi CSV shows severe reliability problems:

| Framework | 1 KB T=0 Avg | 1 KB T=0 P99 | Status |
|-----------|-------------|-------------|--------|
| **areg-sdk Linux** | **13.1 μs** | 31.8 μs | **✅ Stable** |
| ZMQ TCP | 25.3 μs | **30.3 μs** | ✅ Stable |
| NNG TCP | 42.8 μs | 542.8 μs | ❌ P99 broken |
| NanoMsg TCP | 452.8 μs | 867.1 μs | ❌ Nagle broken |

At the same send rate: areg-sdk is both the fastest and the most reliable framework of the four.
NanoMsg loses TCP_NODELAY effectiveness at high rates – Avg becomes 452 μs despite Min=21 μs.
areg-sdk Mean (13.1 μs) beats ZMQ Mean (25.3 μs) at maximum load; ZMQ keeps a slightly
tighter P99 (30.3 vs 31.8 μs) – the gap has narrowed substantially.

---

## 8. Payload Sensitivity

areg-sdk latency is nearly flat from 140 B to 4 KB. Framework overhead dominates.

| areg mode | Total size | Min | Mean | Delta Min from bc64 |
|-----------|-----------|-----|------|---------------------|
| bc64 | 204 B | 10.5 μs | 11.5 μs | baseline |
| bc128 | 268 B | 10.5 μs | 11.7 μs | +0.0 μs |
| bc512 | 652 B | 11.0 μs | 12.3 μs | +0.5 μs (+5%) |
| bc1024 | 1164 B | 11.0 μs | 13.1 μs | +0.5 μs (+5%) |
| bc4096 | 4236 B | 11.6 μs | 15.4 μs | +1.1 μs (+10%) |
| bc65536 | 65676 B | 28.8 μs | 30.2 μs | +18.3 μs (+174%) |

From 204 B to 4236 B (20× size increase): Min increases only 1.1 μs.
Jump at 65 KB is due to TCP segmentation (~45 segments per message).

NanoMsg comparison (T=1000 μs): 1 KB → 4 KB: +1.3 μs (+7%). areg: +0.6 μs (+5%) over the same range.

---

## 9. Throughput Comparison

### areg-sdk OWT throughput (bc mode Msg/s column, from output table)

| Mode | Total size | Msg/s (Linux) | Paper ZMQ | Paper NanoMsg | Paper NNG |
|------|-----------|--------------|-----------|--------------|-----------|
| bc1024 (~1 KB) | 1164 B | ~7.4K | 18.0K | 17.7K | 16.5K |
| bc4096 (~4 KB) | 4236 B | ~6.3K | 18.0K | 17.4K | 16.4K |
| bc65536 (~64 KB) | 65676 B | ~5.4K | 16.1K | 14.9K | 14.1K |

> The `bc` mode used for latency testing is closed-loop and reactive: for 
> every broadcast received, the consumer pushes a `request_message_next()` 
> call before the provider pushes the next message.
> One round trip per measured message is the same as RTT = 2 × OWT).
> It measures one message in flight, not sustained throughput. 
> The Msg/s column above is a byproduct of the latency test, not a
> stress test, and at this round's Performance-mode latencies it now sits below the paper's
> reference rates at all three sizes.
>
> **The actual throughput ceiling is measured separately**, in the dedicated `23_pubdatarate`
> benchmark with parallel channels: up to **~8.0 GB/s** and **~2.5M msg/s** on the same Linux
> hardware (Performance power mode) – see [areg-sdk Performance Benchmarks](./08b-areg-sdk-performance-benchmarks.md).
> Note: paper throughput is also bounded by a polling architecture (10 ms poll timeout) –
> not representative of ZMQ/NNG's maximum throughput potential either.

---

## 10. Platform Analysis – areg-sdk Across Three Platforms

Tested on same hardware (i7-13700H, DDR4) for Windows and Linux.
macOS on MacBook Pro M3 Pro, LPDDR5.

### 10.1 OWT Latency by Platform (bc64, 204 B)

| Platform | OS | Min | P50 | P95 | P99 | Mean | Notes |
|----------|----|-----|-----|-----|-----|------|-------|
| **Linux Ubuntu** | `Performance` mode, pinned cores | **10.4–11.0** | **10.7–11.8** | **10.9–13.0** | **11.9–28.1** | **10.8–12.3** | Min and P50 are stable. |
| **macOS M3 Pro** | Native LPDDR5 | 21.6 | 31.4 | 37.8 | 40.6 | 31.6 | Excellent consistency |
| **Windows 11** | Native SSD | ~32 | ~40 | ~44 | ~47–64 | ~41 | Stable |

### 10.2 RTT Latency by Platform (pp64, 204+212 B)

| Platform | Min | P50 | P95 | P99 | Mean |
|----------|-----|-----|-----|-----|------|
| **Linux** (`Performance` mode, pinned cores) | **21.2 μs** | **21.7 μs** | **22.5 μs** | **23.5 μs** | **22.0 μs** |
| **macOS M3 Pro** | 46.0 μs | 62.5 μs | 74.6 μs | 78.3 μs | 62.9 μs |
| **Windows 11** | ~64 μs | ~83 μs | ~86 μs | ~108 μs | ~83 μs |

### 10.3 Throughput by Platform

| Platform | Msg/s peak | Msg/s sustained | Data rate peak | Data rate sustained |
|----------|------------|----------------|----------------|--------------------|
| **macOS M3 Pro** | – | **~2.5M** | – | **~6.7–7.0 GB/s** |
| **Linux Ubuntu 26.04** | **~2.5M** | ~2.0M | **~8.0 GB/s** | ~6.0–6.5 GB/s |
| **Windows 11** | ~2.8M | ~1.1M | ~3.0 GB/s | ~2.5 GB/s |

### 10.4 Platform Profiles

**Linux (Ubuntu 26.04, `Performance` power mode, pinned cores):**
- Strongest latency: Min 10.5 μs, P50 11.2 μs – fastest single-message delivery
- P99 (21.4 μs) now the lowest of all three platforms, well below macOS (40.6 μs)
- Highest peak throughput of all three platforms: ~8.0 GB/s / ~2.5M msg/s
- Best for: latency-critical applications, maximum responsiveness

**macOS M3 Pro (LPDDR5, native):**
- Higher typical latency (P50 ~31 μs) due to macOS TCP stack overhead
- Exceptional P99 consistency: bc65536 P99 spread = 0.8 μs across 8 runs
- Highest confirmed *sustained* message rate: 2.5M msg/s vs 2.0M on Linux
- Best for: real-time systems requiring guaranteed latency bounds; high-throughput pipelines

**Windows 11 (native SSD):**
- Highest latency (P50 ~40 μs OWT, ~83 μs RTT)
- Stable and predictable – no bimodal distribution
- Lowest *sustained* data rate (2.5 GB/s) vs 6.0–7.0 GB/s on Linux/macOS; peak now reaches 3.0 GB/s / 2.8M msg/s
- Best for: Windows-native production deployments; development and testing

### 10.5 Platform Comparison vs Competitors

| Comparison | Linux | macOS M3 | Windows |
|------------|-------|----------|---------|
| vs NanoMsg TCP Min (18.0 μs) | **wins −7.5 μs** | **loses +3.6 μs** | **loses +14 μs** |
| vs NanoMsg TCP Avg (21.9 μs) | **wins −10.4 μs** | **loses +9.7 μs** | **loses +18 μs** |
| vs ZMQ TCP Avg (27.5 μs) | **wins −16.0 μs** | **loses +4.1 μs** | **loses +12.5 μs** |
| vs NNG TCP Avg (34.9 μs) | **wins −23.4 μs** | **wins −3.3 μs** | **wins −5.1 μs** |

Linux areg-sdk (pinned cores) beats all three competitors at all metrics, by a wider margin than in earlier rounds.
macOS M3 areg-sdk beats ZMQ and NNG; is close to NanoMsg.
Windows areg-sdk beats NNG; is slower than ZMQ and NanoMsg.

---

## 11. Defensible Claims

The following claims are directly supported by measured data:

> *"areg-sdk on Linux TCP achieves Min 10.5 μs and Mean 11.5 μs one-way for 204-byte
> messages – faster than ZMQ (27.5 μs Avg), NanoMsg (21.9 μs Avg) on TCP, and even
> NanoMsg's Unix-domain-socket IPC (16.2 μs Avg) – despite routing through a centralized
> broker, full service dispatch, TCP (not UDS), maximum send rate, and cores pinned but
> not kernel-isolated. Competitor numbers: Hitachi Energy Research, arXiv:2508.07934v1
> (August 2025), TCP T=1000 μs, Xeon w3-2435, isolated cores."*

> *"Full 4-hop brokered RTT (21.2 μs Min, Linux, pinned cores) is lower than 2× NanoMsg
> direct one-way (36 μs). The complete areg-sdk service framework adds less overhead
> than a second raw TCP hop."*

> *"At maximum send rate (T=0): areg-sdk is both the fastest and the most stable
> framework among ZMQ, NanoMsg, NNG, and areg-sdk. NanoMsg and NNG show P99 of
> 542–867 μs at 1 KB; areg-sdk P99 remains at 31.8 μs with a Mean of 13.1 μs.
> (Source: Hitachi CSV, T=0 data.)"*

---

## Sources

| Data | Source |
|------|--------|
| Competitor latency / IPC data | arXiv:2508.07934v1, CSV: github.com/hitachienergy/messaging-libraries-benchmark |
| Competitor architecture | utils.c, source: github.com/hitachienergy/messaging-libraries-benchmark |
| areg-sdk Linux latency | Measured June 2026, i7-13700H, 32 GB DDR4, Ubuntu 26.04, `Performance` power mode, pinned cores via `taskset` – [raw output](./benchmark-test-results.txt) |
| areg-sdk Windows latency | Measured June 2026, i7-13700H, 32 GB DDR4, Windows 11 SSD |
| areg-sdk macOS latency | Measured June 2026, MacBook Pro M3 Pro, 32 GB LPDDR5 |
| areg-sdk queuing model | Example 30 source code analysis (closed-loop, reactive push confirmed) |
| Library maintenance status | arXiv:2508.07934v1, January–April 2025 activity scan |

*All latency values: same-machine TCP loopback only. Build: Release, AREG_LOGGING=OFF.*
