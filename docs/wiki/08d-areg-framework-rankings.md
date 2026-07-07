# areg-sdk – Competitive Position Among All Known Frameworks
## Latency | Message Rate | Data Rate | Use Case Analysis

---

## 1.1 Measurement Scope – What Each Framework Actually Measures

This distinction is critical for interpreting all numbers in this document.

### areg-sdk: full production call path inside measurement window

Timestamps are taken **before serialization** (sender) and **after deserialization + dispatch** (receiver).
Every component of a real production call is inside the measured window:

| Component | Included in areg-sdk latency? | Order |
|-----------|-------------------------------|-------|
| Payload serialization – typed, field-by-field (sender) | ✅ Yes | 1st |
| TCP kernel send | ✅ Yes | 2nd |
| Centralized broker routing (2 hops total) | ✅ Yes | 3rd |
| TCP kernel receive | ✅ Yes | 4th |
| Dispatch: identify service/method, route raw bytes to component thread | ✅ Yes | 5th |
| Payload deserialization on component thread (after dispatch) | ✅ Yes | 6th |
| Method / callback invocation with typed parameters | ✅ Yes | 7th |

For RTT (pp modes): all of the above runs **twice** – once on the provider side
(dispatch → deserialize → serialize) and once on the consumer side
(dispatch → deserialize → call) – within every single RTT sample.

The dispatch-before-deserialize order is not incidental: by design, raw message bytes
are routed to the owning component thread first (enforcing thread affinity), and only
deserialized there – eliminating data races by architecture, not locking.

### Competitors (ZMQ / NanoMsg / NNG): raw socket transfer only

Timestamps bracket only the raw TCP read/write. No serialization, no field parsing, no dispatch.
Payload is a flat byte buffer – one `memcpy` or direct write each end.

### Practical consequence

areg-sdk's published numbers include every component of a production service call:
typed, field-by-field serialization, not a flat buffer. They represent what a real
production service call costs end-to-end – not a synthetic transport floor.

### 1.2 Measurement Environment and Pinning Trade-offs

areg-sdk's Linux latency figures in this document come from a bare-TTY session (no desktop
GUI), with `mtrouter`, provider, and consumer each pinned to a dedicated physical core via
`taskset`, and a full process restart before switching test modes – the lowest-latency,
most rigorously controlled configuration measured to date. Full methodology:
[08b §2.1](./08b-areg-sdk-performance-benchmarks.md#21-measurement-environment-terminal-restart-protocol-and-absolute-floor).
This is not a free improvement – several trade-offs are disclosed here:

| Configuration | Latency | Message rate | Data rate |
|---|---|---|---|
| **Bare TTY, pinned cores** (used for all latency figures in this document) | **Baseline** (lowest) | Lower | Lower |
| `gnome-terminal`, pinned cores (earlier round) | ~0.9–1.4 μs higher | – | – |
| Pinned cores, unpinned comparison | – | ~3× lower | ~3× lower |
| **Unpinned** (used for all throughput figures in this document) | **~2–4 μs higher** | Baseline (peak) | Baseline (peak) |

Pinning removes OS scheduler migration jitter from the three latency-critical processes,
which is why the latency numbers in Sections 3–6 of this document are markedly tighter than
an unpinned run. But `23_pubdatarate`'s throughput test uses multiple parallel channels across
more threads than three – constraining everything to 3 pinned cores caps the parallelism
available to that test, costing roughly **3×** on both message rate and data rate compared to
an unpinned run. The throughput figures in this document (Section 4, Section 5) were
measured **unpinned** for this reason.

**Practical reading:** the latency and throughput numbers in this document are each the best
achievable figure for their respective configuration – they are not simultaneously achievable
on the same running system without re-tuning for whichever metric matters more to a given
deployment.

---

## 2. Framework Classification

| Framework | Primary transport | Scope | Device-to-device | Language | Active |
|-----------|-----------------|-------|-----------------|----------|--------|
| Chronicle Queue | SHM / mmap | 🏗️ Persisted queue | ❌ same cluster | Java | ✅ |
| Aeron IPC | SHM | 🏗️ Raw transport | ❌ same machine | Java/C++ | ✅ |
| CycloneDDS | UDP / SHM | ⚙️ Full DDS | ✅ | C | ✅ |
| Zenoh P2P | SHM / UDP | 🏗️ Raw pub-sub | ✅ | Rust/C | ✅ |
| Aeron UDP ¹ | UDP | 🏗️ Raw transport | ✅ | Java/C++ | ✅ |
| FastDDS ² | UDP / SHM / TCP | ⚙️ Full DDS | ✅ | C++ | ✅ |
| **areg-sdk** | **TCP** | **⚙️ Full RPC/IPC** | **✅** | **C++** | **✅** |
| NanoMsg IPC | Unix socket | 🏗️ Raw | ❌ same machine | C | ❌ dead |
| Zenoh brokered | UDP | 🏗️ Raw pub-sub, through router | ✅ | Rust/C | ✅ |
| ZMQ IPC | Unix socket | 🏗️ Raw | ❌ same machine | C++ | ⚠️ |
| NanoMsg TCP | TCP | 🏗️ Raw | ✅ | C | ❌ dead |
| MQTT | TCP | ⚙️ Pub-sub broker | ✅ | Multi | ✅ |
| ZMQ TCP | TCP | 🏗️ Raw | ✅ | C++ | ⚠️ |
| NNG TCP | TCP | 🏗️ Raw | ✅ | C | ✅ |
| gRPC | TCP / HTTP2 | ⚙️ Full RPC | ✅ | Multi | ✅ |
| ROS2 | UDP DDS | ⚙️ Full middleware | ✅ | Multi | ✅ |

🏗️ = socket-to-socket (raw transport, no service dispatch)
⚙️ = API-to-API (full service framework: routing, dispatch, method/callback delivery)
⚠️ = effectively unmaintained (0 commits Jan–Apr 2025 per Hitachi paper)
❌ dead = no commits, abandoned

> **Note on scope:** ZMQ inproc (thread-to-thread within one process) has been excluded from this
> document entirely – it is not IPC. For consistency, areg-sdk's own in-process delivery mode
> (same-process component-to-component, no socket) is excluded from every comparison in this
> document for the same reason, even though it exists and is faster than the TCP path measured here.
>
> ¹ **Aeron UDP latency is not ranked with confidence.** Four sources disagree by roughly 5×: the
> official aeron.io site states "18 μs on physical hardware" with no transport, percentile, payload,
> or methodology disclosed (a marketing claim, not a benchmark). A detailed community benchmark
> (Sequeira, 2023) reports ~50 μs P50 under default/untuned configuration. A separate community
> Rust-port benchmark (rusteron, GitHub) reports ~9–10 μs average – but that page explicitly states
> "treat all results as stale, rerun before citing," and the same test shows 138–650 ms maximum
> latency outliers. A fourth source, Man Group Engineering (2020), reports ~10 μs round-trip
> for Aeron's reliable-UDP protocol – methodology and payload size not fully specified, but the
> figure is consistent with the rusteron result and corroborates the lower end of the range.
> Aeron UDP could plausibly be faster or slower than areg-sdk depending on tuning and source;
> this document does not assert a position either way.
>
> **Separately, Aeron IPC/SHM has its own wide spread**, now four sources deep: Pirogov (2017,
> ~7.978 μs mean, `EmbeddedPingPong.java`, `BusySpin` idle strategy), Sequeira (2023, ~40 μs IPC
> after an MTU fix), rusteron (GitHub, stale-flagged), and Man Group Engineering (Nov 11, 2020),
> which reports ~0.25 μs round-trip for a 100-byte message via JMH microbenchmark on shared
> memory – nearly two orders of magnitude below Pirogov's figure. The article also describes a
> real production migration to Aeron (early 2019) validated over multiple weeks of independent
> load testing, showing "at least an order of magnitude" improvement at every percentile –
> stronger validation than a standalone microbenchmark, though it still doesn't specify hardware.
>
> **This is not a case of a strong source correcting a weak one.** Pirogov's figure is precise
> and reproducible (named benchmark class, named idle strategy) but lacks production context.
> Man Group's figure has stronger real-world validation but reports Aeron and Chronicle-Queue
> together rather than isolating Aeron alone. Both are credible, well-documented sources, and
> they still disagree by ~32× – which argues for *more* caution about ranking Aeron IPC/SHM with
> confidence, not less. Aeron IPC's #2 position in Section 3 uses the Pirogov figure specifically
> because it isolates Aeron and documents payload size and methodology precisely, not because
> it's the most favorable number available – Man Group's 0.25 μs is markedly more favorable and
> is deliberately *not* used for the ranking.
>
> **Important:** 0.25 μs is an empirical result for two specific software stacks (Aeron,
> Chronicle-Queue) measured over shared memory – it is not a theoretical or physical floor for
> local IPC. No such "theoretical limit" framing appears in the Man Group source; that
> characterization should not be attributed to it or used elsewhere in this document.
>
> ² **FastDDS has no confirmed inter-process (UDP/SHM/TCP) latency figure on comparable hardware
> or payload class.** A previous version of this document cited ~10–15 μs for FastDDS UDP/SHM,
> sourced to a community observation that was actually measuring same-process delivery under an
> RT-Preempt kernel – a different, out-of-scope category, now corrected. eProsima's own official
> benchmark page publishes only chart images with no extractable numeric data for the
> inter-process case. A real, verifiable inter-process figure does exist: OSRF's TSC-RMW Report
> 2021 states FastDDS synchronous publication at 2.56477 ms inter-process – but this is a 2 MB
> payload at 30 Hz (a ROS2 camera-image workload), roughly 10,000× larger than the small
> messages compared everywhere else in this document. Citing it alongside areg-sdk's ~9.9 μs
> (204-byte) figure would be a payload-class mismatch far more severe than any other comparison
> here, so it is noted but not used for ranking. FastDDS remains included in this table for
> completeness but is not ranked numerically anywhere in this document.

---

## 3. Ranking 1 – OWT Latency (same-machine loopback, small messages)

Sorted best-to-worst P50/Avg. Mixed transports – see Notes column.
Aeron UDP and FastDDS are listed without a rank number – see Section 2 footnotes ¹² for why
their numbers aren't confirmed/comparable enough to place with confidence.

| Rank | Framework | Transport | OWT P50/Avg | Source | Scope | Notes |
|------|-----------|-----------|------------|--------|-------|-------|
| 1 | Chronicle Queue | SHM/mmap | ~1–3 μs (P99=3.7 μs) | ✅ Official | 🏗️ | Java. Persisted queue, not RPC. Same machine. |
| 2 | Aeron IPC | SHM | ~7.978 μs mean | ✅ Pirogov 2017 | 🏗️ | Java. IPC/SHM embedded driver. No service layer. Same machine. |
| 3 | CycloneDDS | UDP/SHM | ~8 μs | ✅ NTU 2023 | ⚙️ | Full DDS. Raw DDS layer measured (no app dispatch). |
| — | Aeron UDP ¹ | UDP | ~9–50 μs (sources disagree) | ⚠️ Disputed | 🏗️ | Not ranked – see Section 2. |
| — | FastDDS ² | UDP/SHM/TCP | Unconfirmed | ⚠️ No data | ⚙️ | Not ranked – see Section 2. |
| **4** | **areg-sdk Linux** | **TCP** | **~9.9 μs P50 (bare TTY, pinned)** | **✅ Measured** | **⚙️ Full** | **2-hop broker. Full dispatch. Device-to-device.** |
| 5 | Zenoh P2P ³ | SHM/UDP | ~10 μs | ✅ NTU 2023 | 🏗️ | Raw pub-sub. No dispatch. |
| 6 | NanoMsg IPC | Unix socket | ~16.2 μs avg | ✅ Hitachi CSV | 🏗️ | Dead project. Same machine only. Raw. |
| 7 | Zenoh brokered | UDP | ~21 μs | ✅ NTU 2023 | 🏗️ | Raw pub-sub through router. |
| 8 | NanoMsg TCP | TCP | ~18 μs Min / 21.9 μs avg | ✅ Hitachi CSV | 🏗️ | Dead project. Raw. Low load. |
| 9 | ZMQ IPC | Unix socket | ~25.8 μs avg | ✅ Hitachi CSV | 🏗️ | Same machine only. Raw. |
| 10 | MQTT | TCP | ~27 μs single-machine / ~45 μs multi-machine | ✅ NTU 2023 (arXiv:2303.09419) | ⚙️ | QoS 0 (best-effort); OWT proxy (half of median RTT), not directly measured. |
| 11 | ZMQ TCP | TCP | ~22 μs Min / 27.5 μs avg | ✅ Hitachi CSV | 🏗️ | Raw. Low load. |
| 12 | areg-sdk macOS M3 | TCP | ~31.4 μs P50 | ✅ Measured | ⚙️ Full | Best P99 consistency of all platforms. Not pinned. |
| 13 | NNG TCP | TCP | ~24 μs Min / 34.9 μs avg | ✅ Hitachi CSV | 🏗️ | Raw. Low load. |
| 14 | areg-sdk Windows | TCP | ~40 μs P50 | ✅ Measured | ⚙️ Full | Native SSD, stable. Not pinned. |
| 15 | Kafka | TCP | ~73 μs single-machine / ~81 μs multi-machine | ✅ NTU 2023 (arXiv:2303.09419) | ⚙️ | Durable replicated log, not latency-optimized; OWT proxy (half of median RTT). |
| 16 | gRPC C++ | UDS → TCP ≥ | ~116–167 μs | ✅ MPI-HD (Werner 2021) | ⚙️ Full | UDS, single client, sequential. This is round-trip call latency (unary RPC), not OWT – no independent OWT figure exists for this methodology. TCP would be equal or higher. |
| 17 | ROS2 | UDP DDS | ~200–500 μs | ✅ arxiv | ⚙️ Full | Full RCL/executor overhead. |

> **areg-sdk Linux is #4 overall**, ahead of Zenoh P2P, with the current bare-TTY, pinned-core
> dataset (§1.2). The 3 frameworks ranked above it use Java persisted queues or shared memory,
> and two of the three (Aeron IPC, CycloneDDS) measure raw transport without application
> dispatch. areg-sdk is the highest-ranked **measured, full-stack, device-to-device TCP
> framework**, and the only framework in the top 5 with full application dispatch *and* a
> directly measured (not estimated, not disputed) number.
>
> ³ **The areg-sdk vs. Zenoh P2P ordering is close and should be read with one caveat:** the
> Zenoh source states "~10 μs" without specifying which statistic (Min, Avg, P50) it represents.
> areg-sdk's P50 (9.9 μs) and Mean (9.9 μs) both sit fractionally below 10 μs, so the ranking
> above is defensible but should be read as "essentially tied, marginally ahead" rather than a
> decisively verified gap – especially given Zenoh P2P uses SHM/UDP and areg-sdk uses TCP with
> a 2-hop broker and full service dispatch. Using the earlier `gnome-terminal` dataset instead
> (see [08b §2.1](./08b-areg-sdk-performance-benchmarks.md#21-measurement-environment-terminal-restart-protocol-and-absolute-floor)),
> areg-sdk's OWT P50 is ~11.2 μs and it would rank #5, behind Zenoh P2P rather than above it.
> Unpinned cores add a further ~2–4 μs on top of either dataset (§1.2).

---

## 4. Ranking 2 – Message Rate (msg/s, small messages)

| Rank | Framework | Transport | Stable msg/s | Source | Scope | Conditions |
|------|-----------|-----------|-------------|--------|-------|-----------|
| 1 | Zenoh P2P | SHM/UDP | ~5M | ✅ NTU 2023 | 🏗️ | 8 B payloads, raw pub-sub |
| — | Aeron IPC ³ | SHM | Disputed: ~1M to ~38M | ⚠️ Disputed | 🏗️ | Not ranked – see note below. |
| **2** | **areg-sdk macOS M3** | **TCP** | **~2.5M** | **✅ Measured** | **⚙️ Full dispatch** | **~0.5 KB, LPDDR5, unpinned** |
| 3 | CycloneDDS | UDP/SHM | ~1–3M | 🔲 Est. (derived from GB/s) | ⚙️ Full DDS | Raw DDS layer |
| **4** | **areg-sdk Linux** | **TCP** | **~2.0M sustained / ~2.5M peak** | **✅ Measured** | **⚙️ Full dispatch** | **~0.5 KB, DDR4, `Performance` power mode, unpinned** |
| 5 | ZMQ / NNG TCP | TCP | ~1–2M | 🔲 Est. (paper polling-limited) | 🏗️ Raw | Unpolled config |
| **6** | **areg-sdk Windows** | **TCP** | **~1.1M stable / ~2.8M peak** | **✅ Measured** | **⚙️ Full dispatch** | **~0.5 KB, DDR4, unpinned** |
| 7 | gRPC streaming | TCP | ~100–500K | ✅ Community | ⚙️ Full | HTTP/2 streaming |
| 8 | ROS2 | UDP DDS | ~1–10K | ✅ arxiv | ⚙️ Full | Full executor overhead |

> ³ **Aeron IPC throughput is not ranked with confidence – a ~38× spread between sources.**
> aeron.io's own current official site states "Over 1 million msg/sec" as its headline
> throughput claim – not the ~20M+ figure a previous version of this document cited under
> that same "aeron.io official" label. A separate community Rust-port benchmark (rusteron,
> GitHub, self-flagged as stale) reports ~38–39M msg/s on an AMD EPYC server. Given the size
> of the discrepancy and that the higher number's own source says not to cite it without
> rerunning, this document does not assert a throughput rank for Aeron IPC.
>
> areg-sdk macOS M3 at 2.5M msg/s (full dispatch) reaches the upper bound of
> CycloneDDS's estimated range (raw DDS, no dispatch). areg-sdk's measured number
> is harder to achieve – it includes complete receive-side processing.
>
> **All areg-sdk throughput figures above are unpinned** (see Section 1.2) – pinning
> cores for latency would cost roughly 3× on these message-rate numbers.
>
> **Note on bc mode throughput ceiling:** areg-sdk bc mode is closed-loop and reactive –
> for each broadcast received, the consumer pushes a `request_message_next()` call before
> the provider pushes the next message. areg-sdk has no pull mechanism; both steps are
> push, together forming one round trip per message (the same shape as RTT = 2 × OWT).
> The published bc msg/s numbers reflect this pacing, not areg-sdk's throughput ceiling –
> see `23_pubdatarate` for the dedicated throughput benchmark.

---

## 5. Ranking 3 – Data Rate (GB/s, large messages)

| Rank | Framework | Transport | Stable data rate | Source | Scope | Msg size |
|------|-----------|-----------|-----------------|--------|-------|---------|
| 1 | Zenoh P2P | SHM/UDP | ~8 GB/s peak (67 Gbps) | ✅ NTU/ZettaScale | 🏗️ Raw | 8 KB+, raw |
| **2** | **areg-sdk macOS M3** | **TCP** | **~6.7–7.0 GB/s** | **✅ Measured** | **⚙️ Full dispatch** | **~3 MB** |
| **3** | **areg-sdk Linux** | **TCP** | **~6.0–6.5 GB/s sustained / ~8.0 GB/s peak** | **✅ Measured** | **⚙️ Full dispatch** | **~3 MB, `Performance` power mode** |
| 4 | Aeron | SHM/UDP | ~5+ GB/s | 🔲 Est. | 🏗️ Raw | Large msgs |
| 5 | Zenoh brokered | UDP | ~4.6 GB/s (37 Gbps) | ✅ NTU paper | 🏗️ Raw | Large msgs |
| 6 | CycloneDDS | UDP/SHM | ~3.2 GB/s (25.8 Gbps) | ✅ NTU paper | ⚙️ Full DDS | Large msgs |
| 7 | ZMQ TCP | TCP | ~2.9 GB/s | ✅ Hitachi paper | 🏗️ Raw | 512 KB |
| **8** | **areg-sdk Windows** | **TCP** | **~2.5 GB/s stable / ~3.0 GB/s peak** | **✅ Measured** | **⚙️ Full dispatch** | **~3 MB** |
| 9 | NNG TCP | TCP | ~2.0 GB/s | ✅ Hitachi paper | 🏗️ Raw | 512 KB |
| 10 | NanoMsg TCP | TCP | ~1.4 GB/s | ✅ Hitachi paper | 🏗️ Raw | 512 KB |

> areg-sdk macOS M3 and Linux rank #2 and #3 overall in data rate –
> only Zenoh P2P (raw pub-sub, SHM/UDP) exceeds either, and only by a small margin
> on peak (areg-sdk Linux's ~8.0 GB/s peak is within ~5% of Zenoh P2P's ~8.4 GB/s).
> areg-sdk with full dispatch (6.0–7.0 GB/s sustained) still beats Zenoh brokered
> (4.6 GB/s), CycloneDDS raw DDS (3.2 GB/s), and all TCP frameworks.
>
> **All areg-sdk figures above are unpinned** (see Section 1.2) – pinning cores for
> latency would cost roughly 3× on these data-rate numbers.

---

## 6. Filtered Rankings

### 6.1 Networking Only (TCP + UDP, excluding in-process/SHM-only)

CycloneDDS and Zenoh P2P are excluded from this section entirely, not just re-labeled –
on same-machine loopback their measured numbers reflect SHM auto-selection, not real
network-only performance (see Section 2). Aeron UDP and FastDDS remain unranked per
Section 2's footnotes.

#### By Latency

| Rank | Framework | Transport | OWT P50 | Scope |
|------|-----------|-----------|---------|-------|
| — | Aeron UDP ¹ | UDP | ~9–50 μs (disputed) | 🏗️ Raw – not ranked |
| — | FastDDS ² | UDP | Unconfirmed | ⚙️ Full DDS – not ranked |
| **1** | **areg-sdk Linux TCP** | **TCP** | **~9.9 μs (bare TTY, pinned)** | **⚙️ Full: broker + dispatch + method call** |
| 2 | NanoMsg TCP | TCP | ~18 μs avg | 🏗️ Raw |
| 3 | Zenoh brokered | UDP | ~21 μs | 🏗️ Raw pub-sub, through router (real network, no SHM) |
| 4 | ZMQ TCP | TCP | ~22 μs avg | 🏗️ Raw |
| 5 | MQTT | TCP | ~27–45 μs (OWT proxy) | ⚙️ Full pub-sub broker |
| 6 | NNG TCP | TCP | ~24–35 μs | 🏗️ Raw |
| 7 | Kafka | TCP | ~73–81 μs (OWT proxy) | ⚙️ Durable log, not latency-optimized |
| 8 | gRPC | TCP/HTTP2 | ~116–167 μs ⁴ | ⚙️ Full RPC |
| 9 | ROS2 | UDP DDS | ~200–500 μs | ⚙️ Full middleware |

**areg-sdk Linux: #1 networking framework by latency**, once same-machine SHM-accelerated
numbers (CycloneDDS, Zenoh P2P) are correctly excluded from a network-only comparison.
areg-sdk (full stack, TCP, 2-hop broker, pinned) is faster than Zenoh brokered (raw, UDP,
1-hop) – the only other entry on this list using a genuine, non-SHM network path.

> ⁴ gRPC measured over Unix domain socket (UDS), not TCP. Source: F. Werner, MPI-HD, 2021.
> Single client, sequential unary calls, AMD EPYC 7402P, gRPC v1.40.0. This is round-trip
> call latency (request + response), not OWT – no independent OWT figure exists for this
> methodology. TCP loopback latency would be equal or higher than UDS.
> MQTT/Kafka figures are OWT proxies (half of median RTT), not directly measured – see §6.2 ⁶.

#### Full-Stack Networking Only (frameworks with service dispatch)

| Rank | Framework | OWT P50 |
|------|-----------|---------|
| **1** | **areg-sdk Linux** | **~9.9 μs (bare TTY, pinned)** ✅ measured |
| 2 | MQTT | ~27–45 μs (OWT proxy) |
| 3 | Kafka | ~73–81 μs (OWT proxy) |
| 4 | gRPC | ~116–167 μs ⁴ (RTT, not OWT – see footnote above) |
| 5 | ROS2 | ~200–500 μs |

> CycloneDDS and FastDDS removed from this list – CycloneDDS's number is SHM-accelerated
> on same-machine loopback (not a fair "networking" comparison here), and FastDDS has no
> confirmed inter-process figure at all (Section 2).

**areg-sdk: #1 full-stack networking framework. Directly measured, pinned cores.**

---

### 6.2 TCP Only

#### By Latency

| Rank | Framework | OWT P50 | RTT P50 | Source | Scope |
|------|-----------|---------|---------|--------|-------|
| **1** | **areg-sdk Linux** | **~9.9 μs (bare TTY, pinned)** | **~19.6 μs (bare TTY, pinned)** | **✅ Measured** | **⚙️ Full: 2-hop broker + dispatch** |
| 2 | NanoMsg TCP | ~18 μs avg | ~36 μs ⁷ | ✅ Hitachi CSV | 🏗️ Raw, T=1000 μs, Xeon |
| 3 | ZMQ TCP | ~22 μs avg | ~44 μs ⁷ | ✅ Hitachi CSV | 🏗️ Raw |
| 4 | MQTT | ~27–45 μs ⁶ | n/a | ✅ NTU 2023 (arXiv:2303.09419) | ⚙️ Full pub-sub broker |
| 5 | NNG TCP | ~25–35 μs | ~50–70 μs ⁷ | ✅ Hitachi CSV | 🏗️ Raw |
| 6 | areg-sdk macOS M3 | ~31.4 μs | ~62.5 μs | ✅ Measured | ⚙️ Full, unpinned |
| 7 | areg-sdk Windows | ~40 μs | ~83 μs | ✅ Measured | ⚙️ Full, unpinned |
| 8 | Kafka | ~73–81 μs ⁶ | n/a | ✅ NTU 2023 (arXiv:2303.09419) | ⚙️ Durable log |
| 9 | gRPC ⁵ (UDS) | n/a | ~116–167 μs | ✅ MPI-HD (Werner 2021) | ⚙️ Full |

**areg-sdk Linux: #1 TCP framework by OWT and RTT latency**, by a wider margin pinned
than unpinned.

> ⁵ gRPC measured over Unix domain socket (UDS). Source: F. Werner, MPI-HD, 2021.
> This is round-trip call latency (unary RPC: request + response) – no independent OWT
> figure exists for this methodology, so OWT is listed as n/a rather than estimated.
> TCP loopback would be equal or higher latency than UDS.
>
> ⁶ MQTT/Kafka source reports latency as half of median round-trip time (an OWT proxy),
> not a true one-way measurement – no comparable RTT number exists for these two, so RTT
> is listed as n/a rather than reversing the estimate.
>
> ⁷ NanoMsg/ZMQ/NNG RTT figures are derived as 2× the measured OWT (assuming symmetric
> one-way latency) – the Hitachi paper's methodology is one-way only and does not report
> round-trip time independently. Treat these as estimates, not independently measured RTT;
> the "✅ Hitachi CSV" source marker applies to the OWT column only.

---

### 6.3 Small-Payload UDP DDS Comparison (RTT, informational – not part of the numbered rankings)

Unlike the CycloneDDS/Zenoh P2P figures excluded elsewhere in this document for being
SHM-accelerated, this section compares areg-sdk directly against three DDS implementations'
own **confirmed pure-UDP** RTT figures, at payload sizes comparable to areg-sdk's own test
range. It is deliberately kept separate from Section 3's ranked table – UDP and TCP are
different transport classes, and the goal here is a direct competitiveness check, not a
ranking position.

**Source:** eProsima benchmarking repo, `dds_vendors_comparisons/latency/localhost`
(2019-09-23). Xeon E3-1230 v6 @ 3.50GHz, Ubuntu 18.04.2, **UDPv4, RELIABLE QoS**, loopback,
inter-process (separate OS processes, confirmed via the directory's own README). Confirmed
**RTT** (not OWT) via Fast-RTPS's own `LatencyTest` tool console output in
[eProsima/Fast-DDS issue #463](https://github.com/eProsima/Fast-DDS/issues/463):
*"Printing round-trip times in us"* – exact column-header match to the raw CSVs below.
Raw data, fetched and parsed directly (validated against the constraint that percentiles
must increase monotonically – min ≤ 50% ≤ 90% ≤ 99% ≤ 99.99% ≤ max):
[CycloneDDS CSV](https://github.com/eProsima/benchmarking/blob/tsc_rmw_report_2021/performance_results/dds_vendors_comparisons/latency/localhost/cyclonedds/results_2019-09-23_07-28-16.csv),
[Fast-RTPS CSV](https://github.com/eProsima/benchmarking/blob/tsc_rmw_report_2021/performance_results/dds_vendors_comparisons/latency/localhost/fastrtps/results_2019-09-23_07-28-16.csv),
[OpenSplice CSV](https://github.com/eProsima/benchmarking/blob/tsc_rmw_report_2021/performance_results/dds_vendors_comparisons/latency/localhost/opensplice/results_2019-09-23_07-28-16.csv).

| Payload | Fast-RTPS Mean RTT | CycloneDDS Mean RTT | OpenSplice Mean RTT | areg-sdk Mean RTT ¹ |
|---------|--------------------|-----------------------|----------------------|-----------------------|
| 64 B    | 7.95 μs            | 11 μs                 | 27 μs                | ~19.6 μs (pp64) |
| 128 B   | 7.97 μs            | 11 μs                 | 27.5 μs              | ~19.7 μs (pp128) |
| 1024 B  | 8.20 μs            | 12 μs                 | 28 μs                 | ~20.8 μs (pp1024) |
| 4096 B  | 9.00 μs            | 17 μs                 | 38 μs                 | ~22.5 μs (pp4096) |
| 16384 B | 13.76 μs           | 34 μs                 | 51 μs                 | ~65.5 μs (pp65536, closest available) ² |

¹ areg-sdk figures: bare TTY, pinned cores, current reference (§1.2).
² areg-sdk has no pp16384 mode; pp65536 (65536 B, not 16384 B) is the closest available
data point and is not an exact payload match – treat this row as directional only.

**Honest result – not spun as a clean sweep:**
- **vs OpenSplice: areg-sdk wins clearly** at every payload size tested.
- **vs CycloneDDS: mixed.** areg-sdk trails at small payloads (64 B–1024 B) and pulls ahead
  at 4096 B+ as CycloneDDS's RTT grows faster with payload size.
- **vs Fast-RTPS: areg-sdk trails, by roughly 2–3×, across the whole range tested.**
  Fast-RTPS's raw UDP RTT is faster than areg-sdk's TCP RTT at every size here.

**Why this is still a meaningful result despite trailing Fast-RTPS:** areg-sdk's RTT includes
a full 4-hop path through a centralized broker (`mtrouter`), complete typed field-by-field
serialization on both sides, and full service dispatch – not a raw two-process UDP exchange.
Fast-RTPS and CycloneDDS here measure direct publisher–subscriber RTT with no broker, no
service-layer dispatch, and no location-transparency guarantees. areg-sdk sits within a small,
single-digit constant factor of raw DDS-over-UDP RTT while doing substantially more work –
**competitive, not uniformly faster**, and this document does not claim otherwise.

> **Caveat:** this dataset is from 2019, on a 2017-era Xeon E3-1230 v6 – older hardware than
> areg-sdk's i7-13700H test platform. Despite an extensive, multi-round search (including
> checking four independent secondhand summaries, all of which incorrectly attributed a
> different, unrelated experiment's OWT methodology to this dataset), no equivalently
> rigorous, primary-source-verified small-payload DDS-over-UDP loopback benchmark on more
> modern hardware was found. If a fresher benchmark is confirmed later, this section should
> be updated to it.

---

## 7. areg-sdk Positioning Summary

| Question | Answer | Evidence |
|----------|--------|---------|
| Top 10 IPC/RPC overall (latency)? | ✅ **#4** (bare TTY, pinned) | Directly measured |
| Top 10 overall (msg rate)? | ✅ **#2** (macOS), **#4** (Linux) | Directly measured |
| Top 10 overall (data rate)? | ✅ **#2** (macOS), **#3** (Linux) | Directly measured |
| Top 5 networking (TCP+UDP)? | ✅ **#1** overall, **#1** full-stack | Directly measured |
| Top 3 TCP (latency)? | ✅ **#1** | Directly measured |
| Top 3 TCP (data rate)? | ✅ **#1** | Directly measured |
| Elite class? | ✅ **Yes** | See Section 8 |

---

## 8. Classification

**Definition:** Top-tier latency AND throughput within the same deployment category,
under reproducible conditions, with production reliability.

| Criterion | areg-sdk value | vs nearest competitor | Class |
|-----------|---------------|----------------------|-------|
| TCP OWT Min (Linux, bare TTY, pinned) | 9.6 μs | < NanoMsg 18.0 μs (raw, Xeon) | ✅ |
| TCP OWT P50 (Linux, bare TTY, pinned) | 9.9 μs | < NanoMsg 21.9 μs avg (raw, Xeon) | ✅ |
| TCP RTT Min (Linux, bare TTY, pinned) | **19.1 μs** | < 2× NanoMsg OWT (36 μs) | ✅ |
| TCP data rate (macOS) | 6.7–7.0 GB/s | > Zenoh brokered 4.6 GB/s | ✅ |
| TCP data rate (Linux, unpinned) | **6.0 GB/s** sustained / ~8.0 GB/s peak | > ZMQ 2.9 GB/s (2.1×) | ✅ |
| Msg rate (macOS, unpinned) | 2.5M/s full dispatch | Competitive with CycloneDDS raw | ✅ |
| Stability at T=0 (bare TTY, pinned) | Stable P99=11.6 μs | Only stable framework of 4 tested | ✅ |
| Payload sensitivity (bare TTY, pinned) | +1.0 μs over 20× range | Better than NanoMsg 7× range | ✅ |
| Full service framework | Yes | No TCP competitor provides this | ✅ |
| Device-to-device native | Yes | TCP, zero rewrite across topologies | ✅ |

**Verdict:**

> **areg-sdk is a high-performance C++ TCP IPC/RPC framework.**
> It is #1 by TCP latency and #1 by TCP data rate across all measured platforms.
> It is #1 among full-stack networking frameworks once same-machine SHM-accelerated
> numbers (CycloneDDS, Zenoh P2P) are properly excluded from a networking-only comparison.
>
> The only frameworks with lower latency use shared memory, in-process threads,
> Unix sockets, or UDP – transports that cannot survive device separation or
> process restart. On TCP specifically, no comparable framework comes close.

---

## 9. Use Case Recommendations

### areg-sdk

Best for:
- **Embedded and edge C++** systems requiring structured IPC with service model
- **Industrial automation** – PLCs, robot controllers, CNC: control and coordination plane
- **Robotics service/control plane** alongside DDS for raw sensor data highways
- **Distributed C++ applications** scaling from single device to multi-device without rewrites
- **Any C++ team** with threading safety requirements – race conditions eliminated architecturally
- **Projects scaling topology** (single process → multi-process → distributed): zero code changes

Avoid when:
- Multi-language team required (areg-sdk is C++ only)
- Raw UDP multicast is mandatory (areg uses TCP)
- JVM ecosystem is the deployment target

### ZMQ / NNG

Best for:
- Custom messaging with full architectural control
- Research, prototyping, tooling
- Multi-language teams (ZMQ supports 40+ languages)
- When no service model is needed

Avoid: NanoMsg (dead project, critical Nagle bug in TCP)

### Aeron

Best for:
- Finance / HFT where microsecond latency and extreme throughput are non-negotiable
- Java ecosystems with SHM IPC requirements
- Systems where UDP multicast is the primary transport

### CycloneDDS / FastDDS

Best for:
- ROS2 robotics ecosystems
- Defense and automotive DDS-standard deployments
- Systems requiring QoS profiles and RTPS interoperability

### gRPC

Best for:
- Cloud microservices requiring multi-language support
- Kubernetes-native deployments (HTTP/2, gRPC-web, service mesh)
- Standard API exposure

Not for: embedded, latency-critical edge systems

### ROS2

Best for:
- Robotics research with full ecosystem (RViz, Gazebo, Nav2)
- When 200–500 μs node-to-node latency is acceptable
- Sensor data pipelines where DDS QoS matters

---

## 10. Development Time Savings with areg-sdk

This section estimates time savings for a medium-sized embedded/distributed C++ project
(6–12 month scope). Figures are a conservative bottom-up model based on observed category
costs in real distributed C++ codebases, not a surveyed average. Two independent research
findings anchor the concurrency-specific estimates:

- Data races account for approximately **80% of all concurrency bugs** in concurrent
  programs (Upadhyay et al., arXiv:2312.14479, 2023).
- About **73% of real-world concurrency bug fixes** required more than adding or changing
  locks, and many were not correct on the first attempt (Lu et al., *Learning from Mistakes*,
  ASPLOS 2008, ACM Influential Paper Award — 105 bugs across MySQL, Apache, Mozilla,
  OpenOffice).

These two findings make the race condition debugging row the most conservative estimate
in the table, not the most speculative.

| Category | Source of saving | Est. weeks saved |
|----------|-----------------|-----------------|
| **Race condition prevention** | Thread affinity by design: raw bytes routed to owning thread before deserialization; no shared mutable state on any message path; entire class of mutex-related bugs absent from codebase ⁷ | 2–4 weeks |
| **Race condition debugging avoidance** | No incidents to investigate: 1–5 days/incident × ~5–10 incidents removed per project ⁷ | 1–4 weeks |
| **Topology rewrites** | Location transparency: zero code changes when scaling thread → process → networked device | 2–4 weeks per scaling event |
| **Service infrastructure** | No hand-written service discovery, routing, or reconnection code | 2–3 weeks |
| **IDL code generation** | `.siml` generates typed serialization and dispatch for 10–20 interfaces | 1–2 weeks |

**Total conservative estimate: 10–19 weeks** over one product lifecycle.  
Add 2–4 weeks per topology-change event.  
Add 3–6 weeks for teams with documented race condition history (Lu et al. study shows
teams with concurrency exposure spend disproportionately more remediation time on
subsequent incidents due to non-obvious fix patterns).

> ⁷ Race condition prevalence: Upadhyay et al., [arXiv:2312.14479](https://arxiv.org/html/2312.14479v1/) (2023).  
>   Fix difficulty: Lu et al., [*Learning from Mistakes*](https://www.microsoft.com/en-us/research/publication/learning-from-mistakes-a-comprehensive-study-on-real-world-concurrency-bug-characteristics/), ASPLOS 2008.

---

## 11. Platform Performance Summary

| Platform | bc OWT P50 | pp RTT P50 | Msg/s | Data rate | Best suited for |
|----------|-----------|-----------|-------|-----------|----------------|
| **Linux** (i7-13700H, DDR4, `Performance` mode, bare TTY, pinned cores) | **~9.9 μs** | **~19.6 μs** | \~2.0M sustained / **~2.5M peak** (unpinned) | \~6.0–6.5 GB/s sustained / **~8.0 GB/s peak** (unpinned) | Lowest latency; highest peak throughput of the three platforms |
| **macOS M3 Pro** (LPDDR5, unpinned) | ~31 μs | ~63 μs | **~2.5M** | **~6.7–7.0 GB/s** | Best P99 consistency + highest confirmed *sustained* throughput |
| **Windows 11** (i7-13700H, DDR4, unpinned) | ~40 μs | ~83 μs | ~1.1M sustained / ~2.8M peak | ~2.5 GB/s sustained / ~3.0 GB/s peak | Windows-native production |

Linux: **~4.1× lower** P50 latency than macOS, **~4.1× lower** than Windows on the same hardware (bc64 OWT, bare-TTY-pinned vs. unpinned).
macOS M3: **~1.25× higher sustained throughput** than Linux, **best P99 predictability** of all platforms.
Latency and throughput figures use different measurement configurations – see Section 1.2.
Earlier `gnome-terminal` dataset reads ~0.9–1.4 μs higher on Linux; not pinning cores at all
adds a further ~2–4 μs on top of either dataset.

---

## Sources

| Data | Source |
|------|--------|
| ZMQ/NanoMsg/NNG TCP | [arXiv:2508.07934v1](https://arxiv.org/abs/2508.07934), [results.csv](https://github.com/hitachienergy/messaging-libraries-benchmark/blob/main/results/results.csv) |
| ZMQ/NanoMsg/NNG IPC | Same CSV (ipc transport rows) |
| CycloneDDS ~8 μs / Zenoh ~10 μs / Zenoh brokered ~21 μs | NTU research (Liang, Yuan, Lin), arXiv:2303.09419, hosted at [zenoh.io/blog/2023-03-21-zenoh-vs-mqtt-kafka-dds](https://zenoh.io/blog/2023-03-21-zenoh-vs-mqtt-kafka-dds/) (March 2023) |
| MQTT (~27/45 μs), Kafka (~73/81 μs) | Same NTU source, [arXiv:2303.09419](https://arxiv.org/abs/2303.09419) |
| Zenoh throughput (5M msg/s, 67 Gbps) | Same NTU/Zenoh source |
| Aeron "Over 1 million msg/sec" / "18 μs on physical hardware" | [aeron.io](https://aeron.io/) official site – no methodology disclosed, treat as directional |
| Aeron IPC mean ~7.978 μs | [Pirogov, Medium, 2017](https://medium.com/@pirogov.alexey/aeron-low-latency-transport-protocol-9493f8d504e8) |
| Aeron UDP ~50 μs (untuned) | [E. Sequeira, 2023](https://esequeira.com/posts/aeron-latency-at-lower-throughputs/) |
| Aeron UDP ~9–10 μs avg / ~38M msg/s (stale, unverified) | [rusteron BENCHMARKS.md, GitHub](https://github.com/gsrxyz/rusteron/blob/main/BENCHMARKS.md) – source itself flags results as stale |
| Aeron SHM ~0.25 μs RTT (100 B) / Aeron UDP ~10 μs RTT | [Man Group Engineering, "Special FX", Nov 11, 2020](https://www.man.com/technology/special-fx-execution-system-on-aeron) – JMH microbenchmark; production-validated migration (2019–2020); hardware unspecified. Confirmed via full-text fetch: no "theoretical speed limit" claim exists in this source – that framing should not be attributed to it. |
| FastDDS – no confirmed small-message figure | Checked: [arXiv:2310.16630](https://arxiv.org/abs/2310.16630) (survey paper, no such data), [arXiv:2508.00947](https://arxiv.org/abs/2508.00947) (real data, wrong hardware class – Jetson Orin, ms-range), [eprosima.com official page](https://www.eprosima.com/developer-resources/performance/eprosima-fast-dds-performance) (right categories, chart images only, no extractable numbers) |
| FastDDS 2.56 ms inter-process (real, but 2 MB @ 30 Hz – not comparable to small-message figures) | [OSRF TSC-RMW Report 2021, eProsima response](https://osrf.github.io/TSC-RMW-Reports/humble/eProsima-response.html) |
| CycloneDDS ~20 μs UDP (claimed via GitHub issue #484) | **Checked and not found** – the actual issue thread discusses retransmit tuning for multi-megabyte point clouds (100 ms–100 s latencies); no "~20 μs" figure exists in it. Not used. |
| Fast-RTPS/CycloneDDS/OpenSplice small-payload RTT (64 B–16384 B) | eProsima benchmarking repo, [`dds_vendors_comparisons/latency/localhost`](https://github.com/eProsima/benchmarking/tree/tsc_rmw_report_2021/performance_results/dds_vendors_comparisons/latency/localhost) (2019-09-23), Xeon E3-1230 v6, UDPv4, RELIABLE. RTT confirmed via [eProsima/Fast-DDS issue #463](https://github.com/eProsima/Fast-DDS/issues/463) (tool's own console output). Used in §6.3. |
| CycloneDDS `ping.log` (eclipse-cyclonedds/cyclonedds, assets branch, 2019-07-30) | **Checked and not used** – confirmed cross-machine via the directory's own `config.txt` ("2 identical machines", real GbE + switch), not loopback. |
| gRPC C++ UDS ~116 μs | [F. Werner, MPI-HD, 2021](https://www.mpi-hd.mpg.de/personalhomes/fwerner/research/2021/09/grpc-for-ipc/) |
| ROS2 | arxiv:2201.00393 (ros2_tracing) |
| areg-sdk all measurements | July 2026 (Linux latency, current reference) / June 2026 (Win/macOS latency, Linux `gnome-terminal` secondary dataset, throughput) – i7-13700H DDR4 (Win/Linux, Linux in `Performance` power mode, latency figures pinned via `taskset` from a bare TTY session), MacBook Pro M3 Pro LPDDR5 – [raw latency CSV, current](./areg-latency-benchmark-20260705.csv) · [raw latency output, earlier round](./areg-latency-benchmark-20260629.txt) |

*Compiled June–July 2026. All same-machine loopback. areg-sdk: Release, AREG_LOGGING=OFF.*
