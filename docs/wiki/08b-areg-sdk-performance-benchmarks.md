# areg-sdk – Performance Benchmarks
## Latency, Message Rate, and Data Rate

> **Scope:** All measurements are same-machine TCP loopback (`127.0.0.1`) only.
> No cross-machine or real-network numbers appear in this document.
> All benchmark code is open-source and reproducible. Two raw-output datasets are published:
> [`areg-latency-benchmark-20260629.txt`](./areg-latency-benchmark-20260629.txt) (gnome-terminal, June 2026) and
> [`areg-latency-benchmark-20260705.csv`](./areg-latency-benchmark-20260705.csv) (bare TTY, July 2026,
> the current lowest-latency reference). See Section 2.1 for why the two differ.

---

## 1. Benchmark Examples

Two examples are used for performance measurement:

| Example            | Purpose                                   | Source                                            |
|--------------------|-------------------------------------------|---------------------------------------------------|
| **23_pubdatarate** | Message- and data rate (high-throughput)  | [23_pubdatarate](./../../examples/23_pubdatarate) |
| **30_publatency**  | OWT and RTT latency across payload sizes  | [30_publatency](./../../examples/30_publatency)   |

Both require **mtrouter** to be running, there is no startup order.
Build with logging disabled for clean measurements:

```bash
cmake -B ./build -DCMAKE_BUILD_TYPE=Release -DAREG_COMPILER_FAMILY=gnu -DAREG_LOGGING=OFF
cmake --build ./build -j$(nproc)
```

---

## 2. Test Hardware

| Platform    | CPU                                                     | RAM          | OS                                     |
|-------------|---------------------------------------------------------|--------------|----------------------------------------|
| **Linux**   | Intel i7-13700H (mobile, 14C/20T, 5.0 GHz P-core boost) | 32 GB DDR4   | Ubuntu 26.04, Performance power mode   |
| **Windows** | Same i7-13700H (same physical machine)                  | 32 GB DDR4   | Windows 11                             |
| **macOS**   | Apple M3 Pro                                            | 32 GB LPDDR5 | macOS 26 Tahoe                         |

> **Linux note:** Latency figures in this document were measured on Ubuntu 26.04
> with the **Performance** power profile active. `mtrouter`, provider, and consumer
> are each pinned to a dedicated physical core via `taskset` for the latency results
> in Sections 4–6 and 9.1. Throughput figures (Section 7) were not re-tested with pinning.
---

### 2.1 Measurement Environment: Terminal, Restart Protocol, and Absolute Floor

Two independently measured Linux latency datasets exist for this hardware, and they
produce **different absolute numbers** for a documented reason – stated here explicitly
so a third party reproducing these results on their own machine is not confused or made
skeptical by a mismatch.

| Dataset | Terminal | Core pinning | Restart protocol | Result |
|---------|----------|--------------|-------------------|--------|
| [`areg-latency-benchmark-20260629.txt`](./areg-latency-benchmark-20260629.txt) (June 2026) | `gnome-terminal` (desktop GUI) | Pinned via `taskset` | No restart between the 8 runs of a session | Higher baseline – used in Sections 4.1/5.1 as "gnome-terminal" table |
| [`areg-latency-benchmark-20260705.csv`](./areg-latency-benchmark-20260705.csv) (July 2026) | Bare TTY (`Ctrl+Alt+F4`, no desktop session running) | Pinned via `taskset` | Full process restart (`mtrouter`, provider, consumer) before switching to a new mode | **Lower baseline – current reference, used as primary in Sections 4.1/5.1** |

**Why the TTY figures are lower:** a bare TTY has no desktop compositor, window manager,
or GUI terminal emulator process competing for scheduler attention and cache space on the
same machine. `gnome-terminal` itself is a running process with its own rendering and event
loop – it does not touch the benchmark's pinned cores directly, but it adds background
scheduler and memory-bus pressure that a bare TTY session does not have. The gap is small
but consistent: roughly **0.9–1.4 μs** on OWT Min/P50 and a similar amount on RTT, comparing
the two datasets at the same mode (e.g. `bc64` Min: 10.5 μs gnome-terminal vs 9.6 μs TTY).

**Why the TTY dataset restarts all three processes before every mode switch:** repeated
cycles of the *same* mode, run back-to-back without a process restart, show a small but
real upward drift. Within the TTY dataset's own 10-cycle blocks (no restart *between*
cycles of the same mode, only *before* switching to the next mode), Min drifts upward by
typically **+0.1 to +0.3 μs** from the first cycle to the tenth (e.g. `bc0` Min: 9.26 μs on
cycle 1 vs 9.47 μs on cycle 10; `bc65536` Min: 25.96 μs vs 26.36 μs). Left to run for many
more cycles without a restart, this drift is expected to continue climbing – restarting all
three processes before each new mode is what keeps every mode's measurement anchored to a
fresh baseline rather than an already-drifted one.

**Absolute floor observed to date:** the single lowest OWT sample across all TTY runs is
**9.14 μs** (`bc0` and `bc16`, best of 10 cycles each); the single lowest RTT sample is
**18.34 μs** (`pp0`). These are best-single-sample figures, distinct from the *representative*
averaged Min/P50 figures used in the tables below – both are reported so neither is mistaken
for the other.

**Unpinned cores:** per the author's own testing, not pinning `mtrouter`, provider, and
consumer to dedicated cores adds roughly **+2 to +4 μs** on top of the TTY-pinned baseline
above. This has not been captured in a separately published raw file as of this writing;
it is reported here as a measured observation, not a formal dataset.

**Pinning's cost to throughput:** the same core pinning that lowers latency measurably
*reduces* `23_pubdatarate` throughput by roughly **3×** on both data rate and message rate
compared to the unpinned figures in Section 7 – pinning three benchmark processes to three
cores leaves the throughput test far less parallelism to work with than an unpinned run
using all available cores. Section 7's throughput figures are deliberately **unpinned** for
this reason; they are not simultaneously achievable alongside the pinned latency figures
above on the same running configuration.

---

## 3. Measurement Methodology

### 3.1 Transport Architecture

```
[Provider process]          [mtrouter]          [Consumer process]
        │                       │                        │
        │── TCP send ──────────►│── TCP send ───────────►│
        │                       │                        │
        │◄─ TCP send ───────────│◄─ TCP send ────────────│
```

Every message travels **4 TCP hops** per round-trip:
`Consumer → mtrouter → Provider → mtrouter → Consumer`

All service discovery, automatic reconnection, and thread dispatch are active during measurement. **There is no stripped-down or bypass mode. What is measured is what production systems get.**

### 3.2 Communication Patterns

#### Ping-Pong RTT (`ppX` modes)
Closed-loop sequential request/response chain. The consumer sends one request; the provider echoes it back; the consumer records RTT and immediately sends the next. There is **exactly 1 request in flight** and **1 response in flight** at all times. The component thread is idle during transit – no artificial delay is added.

#### Broadcast OWT (`bcX` modes)
Closed-loop, reactive one-way chain. The consumer pushes a `request_message_next()` call; the provider reacts and pushes back one `broadcast_message_X()`. There is **exactly 1 request and 1 broadcast in flight** at all times – both delivered by push, never pull.

> **Throughput implication:** The request call and the broadcast that follows are each
> one-way pushes; together they form one full round trip per measured message – the same
> shape as RTT = 2 × OWT in the ping-pong test. areg-sdk has no pull mechanism: every
> operation, including this pacing call, is reactive and push-based.

**Both modes use T=0 (no artificial inter-message delay).** The next message fires immediately on receipt of the previous – continuous operation, no sleep, no pacing.

### 3.3 Payload Structures

Payload fields are serialized **field-by-field** (typed, parameter-by-parameter), not as flat byte buffers – reflecting real service call cost.

| Mode range | Extra payload structure | Allocation |
|-----------|------------------------|------------|
| `pp0` / `bc0` | None | None |
| `pp8` – `pp64` / `bc8` – `bc64` | 1–8 × `uint64_t` fields | None – stack only |
| `pp128` – `pp512` / `bc128` – `bc512` | `uint64_t` fields + `areg::String` (128 B) | Heap: `String` per message |
| `pp1024` – `pp65536` / `bc1024` – `bc65536` | `uint64_t` + `String` + `areg::SharedBuffer` | Heap: `String` + pre-allocated `SharedBuffer` |

> From `bc1024` / `pp1024` upward, the `SharedBuffer` is **pre-allocated** (static),
> eliminating large buffer malloc/free from the hot path. The `String` field remains
> dynamically allocated for modes that include it.

### 3.4 Timestamp Placement and Measured Scope

This is the critical difference between areg-sdk latency measurements and raw transport benchmarks.

**OWT (`bcX`) – one measurement cycle:**

```
t1 = now_ns()                    ← BEFORE any serialization
│
├── serialize: t1, payload → field-by-field, typed (inside framework call)
├── dispatch: route communication channel → TCP send queue
├── TCP send: consumer → mtrouter
├── mtrouter: TCP receive → identify target
├── mtrouter: route to target → TCP send (→ provider)
├── TCP receive: provider → route to component thread queue
├── dispatch: route raw bytes to component thread queue
├── dispatch: find subscribers and method
├── deserialize: t1, payload on the component thread → cache data
└── method call invoked for each subscriber
    └── t4 = now_ns()             ← AFTER dispatch + deserialization + call

OWT = t4 − t1 = serialize + TCP × 2 + route + dispatch × 3 + deserialize + call
```

**RTT (`ppX`) – full cycle, both directions:**

```
t1 = now_ns()                    ← BEFORE serialization (consumer)
│
├── serialize(t1, payload)
├── TCP × 2 → provider
├── dispatch → deserialize → request_handler()
│   └── t_mid = now_ns()         ← AFTER deserialization (provider)
│   ├── serialize(t1, t_mid, payload)
│   ├── TCP × 2 → consumer
│   ├── dispatch → deserialize → response_handler()
│       └── t4 = now_ns()        ← AFTER deserialization (consumer)

RTT = t4 − t1 = 2 × (serialize + TCP × 2 + route + dispatch × 3 + deserialize + call)
```

> By design, dispatch precedes deserialization: raw bytes are routed to the owning
> component's thread first (preserving thread affinity), and only deserialized there.
> This is how areg-sdk eliminates data races by architecture, not by locking.

**Scope comparison with raw-transport benchmarks:**

| Component | areg-sdk (measured) | Raw-transport benchmark (e.g. ZMQ, NanoMsg) |
|-----------|---------------------|---------------------------------------------|
| Serialize payload at sender | ✅ Inside OWT | ❌ Not measured |
| TCP kernel send | ✅ Inside OWT | ✅ Inside OWT |
| Broker routing (2 hops) | ✅ Inside OWT | ❌ Direct 1-hop only |
| TCP kernel receive | ✅ Inside OWT | ✅ Inside OWT |
| Dispatch to component thread | ✅ Inside OWT | ❌ Not measured |
| Deserialize payload at receiver | ✅ Inside OWT | ❌ Not measured |
| Method / callback invocation | ✅ Inside OWT | ❌ Not measured |

areg-sdk numbers include every component of a production service call: typed,
field-by-field serialization, not a flat `memcpy`.

---

## 4. One-Way Latency Results (OWT, `bcX` modes)

Configuration: `count=5000`, `warmup=1000`, T=0. TTY dataset (primary): 10 consecutive
cycles per mode, full process restart between modes (§2.1). `gnome-terminal` dataset
(secondary): 8 consecutive runs, no restart between runs.
All values in **μs**.

### 4.1 Linux – Ubuntu 26.04 LTS (Performance mode, i7-13700H, 32 GB DDR4, pinned cores)

#### Bare TTY, restart-per-mode (current reference – lowest measured)

| Mode    | Payload struct                | Total msg size | Min  | P50  | P95  | P99  | Mean |
|---------|-------------------------------|----------------|------|------|------|------|------|
| bc0     | None                          | 140 B          | 9.4  | 9.7  | 9.8  | 10.8 | 9.7  |
| bc8     | Latency8 (1×u64)              | 148 B          | 9.4  | 9.7  | 9.9  | 10.9 | 9.8  |
| bc16    | Latency16 (2×u64)             | 156 B          | 9.4  | 9.7  | 9.9  | 10.8 | 9.8  |
| bc32    | Latency32 (4×u64)             | 172 B          | 9.4  | 9.7  | 9.9  | 10.8 | 9.8  |
| bc64    | Latency64 (8×u64)             | 204 B          | 9.6  | 9.9  | 10.1 | 11.0 | 9.9  |
| bc128   | Latency128 (8×u64 + String)   | 268 B          | 9.6  | 9.9  | 10.1 | 11.0 | 9.9  |
| bc256   | Latency256 (2×Lat128)         | 396 B          | 9.8  | 10.1 | 10.4 | 11.2 | 10.1 |
| bc512   | Latency512 (4×Lat128)         | 652 B          | 10.1 | 10.4 | 11.1 | 11.6 | 10.5 |
| bc1024  | Lat64 + Lat128 + SharedBuffer | 1164 B         | 10.0 | 10.4 | 11.4 | 11.6 | 10.5 |
| bc4096  | Lat64 + Lat128 + SharedBuffer | 4236 B         | 10.6 | 11.0 | 12.3 | 13.0 | 11.2 |
| bc65536 | u64 + Lat128 + SharedBuffer   | 65676 B        | 26.0 | 26.7 | 28.2 | 31.0 | 26.9 |

> `mtrouter`, provider, and consumer each pinned to a dedicated physical core via `taskset`,
> measured from a bare TTY session (no desktop GUI), with a full process restart before
> each mode switch – see §2.1. Values are averages across 10 consecutive cycles per mode.
> **Absolute minimum single-sample observed: 9.14 μs** (`bc0`/`bc16`).
> Raw data: [`areg-latency-benchmark-20260705.csv`](./areg-latency-benchmark-20260705.csv).

#### `gnome-terminal` desktop session, pinned cores (earlier dataset, higher baseline)

| Mode    | Payload struct                | Total msg size | Min  | P50  | P95  | P99  | Mean |
|---------|-------------------------------|----------------|------|------|------|------|------|
| bc0     | None                          | 140 B          | 10.4 | 10.7 | 10.9 | 11.9 | 10.8 |
| bc8     | Latency8 (1×u64)              | 148 B          | 10.5 | 10.9 | 11.1 | 12.3 | 11.0 |
| bc16    | Latency16 (2×u64)             | 156 B          | 10.4 | 11.0 | 11.3 | 13.3 | 11.1 |
| bc32    | Latency32 (4×u64)             | 172 B          | 10.4 | 11.1 | 11.5 | 16.3 | 11.3 |
| bc64    | Latency64 (8×u64)             | 204 B          | 10.5 | 11.2 | 11.8 | 21.4 | 11.5 |
| bc128   | Latency128 (8×u64 + String)   | 268 B          | 10.5 | 11.3 | 12.1 | 25.4 | 11.7 |
| bc256   | Latency256 (2×Lat128)         | 396 B          | 10.6 | 11.5 | 13.2 | 30.5 | 12.3 |
| bc512   | Latency512 (4×Lat128)         | 652 B          | 11.0 | 11.8 | 13.0 | 28.1 | 12.3 |
| bc1024  | Lat64 + Lat128 + SharedBuffer | 1164 B         | 11.0 | 11.9 | 20.3 | 31.8 | 13.1 |
| bc4096  | Lat64 + Lat128 + SharedBuffer | 4236 B         | 11.6 | 12.8 | 29.5 | 40.7 | 15.4 |
| bc65536 | u64 + Lat128 + SharedBuffer   | 65676 B        | 28.8 | 29.9 | 32.7 | 34.6 | 30.2 |

> Same hardware and pinning, measured inside a `gnome-terminal` desktop session, no restart
> between the 8 runs of a session – see §2.1 for why this differs from the TTY table above.
> Raw data: [`areg-latency-benchmark-20260629.txt`](./areg-latency-benchmark-20260629.txt).

### 4.2 macOS – Apple M3 Pro, 32 GB LPDDR5

| Mode    | Payload struct                | Total msg size | Min  | P50  | P95  | P99  | Mean |
|---------|-------------------------------|----------------|------|------|------|------|------|
| bc0     | None                          | 140 B          | 21.7 | 31.7 | 38.3 | 41.3 | 31.8 |
| bc32    | Latency32 (4×u64)             | 172 B          | 22.1 | 32.1 | 38.3 | 41.2 | 32.0 |
| bc64    | Latency64 (8×u64)             | 204 B          | 21.6 | 31.4 | 37.8 | 40.6 | 31.6 |
| bc128   | Latency128                    | 268 B          | 22.4 | 32.9 | 38.3 | 41.2 | 32.5 |
| bc512   | Latency512                    | 652 B          | 22.8 | 34.3 | 39.5 | 42.9 | 33.7 |
| bc1024  | Lat64 + Lat128 + SharedBuffer | 1164 B         | 23.0 | 33.4 | 39.3 | 42.6 | 33.2 |
| bc4096  | Lat64 + Lat128 + SharedBuffer | 4236 B         | 23.2 | 34.5 | 40.1 | 43.2 | 34.0 |
| bc65536 | u64 + Lat128 + SharedBuffer   | 65676 B        | 41.8 | 49.5 | 53.8 | 56.6 | 49.7 |

> bc65536 P99 spread across 8 runs = 0.8 μs on macOS. The TTY Linux dataset (§2.1) shows
> a similarly tight bc65536 P99 spread of 1.5 μs across its 10 cycles – both platforms are
> tight at this mode. Higher P50 than Linux reflects macOS TCP stack overhead; macOS was
> not re-tested with core pinning.

### 4.3 Windows 11 – same i7-13700H, 32 GB DDR4

| Mode    | Payload struct                | Total msg size | Min  | P50  | P95  | P99   | Mean |
|---------|-------------------------------|----------------|------|------|------|-------|------|
| bc0     | None                          | 140 B          | 32.0 | 39.5 | 41.9 | 57.1  | 40.1 |
| bc64    | Latency64 (8×u64)             | 204 B          | 32.5 | 40.3 | 43.3 | 60.0  | 41.2 |
| bc128   | Latency128                    | 268 B          | 33.0 | 40.5 | 43.5 | 53.6  | 40.5 |
| bc256   | Latency256                    | 396 B          | 33.4 | 41.6 | 46.3 | 72.0  | 42.2 |
| bc512   | Latency512                    | 652 B          | 33.5 | 41.4 | 46.0 | 68.6  | 42.0 |
| bc1024  | Lat64 + Lat128 + SharedBuffer | 1164 B         | 33.8 | 41.8 | 45.8 | 60.4  | 42.9 |
| bc4096  | Lat64 + Lat128 + SharedBuffer | 4236 B         | 36.4 | 46.7 | 49.8 | 67.4  | 46.1 |
| bc65536 | u64 + Lat128 + SharedBuffer   | 65676 B        | 57.6 | 72.8 | 76.9 | 104.3 | 73.7 |

> Windows OWT is approximately 4.1× higher than Linux at the same hardware (bc64 P50: 40.3 vs 9.9 μs, TTY, pinned cores).
> The Windows TCP loopback stack has higher per-message overhead than Linux.
> No bimodal distribution – all 8 runs land in the same cluster.

---

## 5. Round-Trip Latency Results (RTT, `ppX` modes)

Configuration: `count=5000–10000`, `warmup=1000`, T=0. TTY dataset (primary): 10
consecutive cycles per mode, full process restart between modes (§2.1). `gnome-terminal`
dataset (secondary): 8 consecutive runs, no restart between runs.
All values in **μs**.

### 5.1 Linux – Ubuntu 26.04 LTS (Performance mode, i7-13700H, 32 GB DDR4, pinned cores)

#### Bare TTY, restart-per-mode (current reference – lowest measured)

| Mode    | Payload struct                | Request / Response total | Min  | P50  | P95  | P99  |
|---------|-------------------------------|---------------------------|------|------|------|------|
| pp0     | None                          | 140 / 148 B               | 18.7 | 19.2 | 19.6 | 20.6 |
| pp8     | Latency8                      | 148 / 156 B                | 18.9 | 19.2 | 19.7 | 20.7 |
| pp16    | Latency16                     | 156 / 164 B                | 18.8 | 19.2 | 19.7 | 20.6 |
| pp32    | Latency32                     | 172 / 180 B                | 19.0 | 19.3 | 19.8 | 20.7 |
| pp64    | Latency64                     | 204 / 212 B                | 19.1 | 19.5 | 20.1 | 20.9 |
| pp128   | Latency128                    | 268 / 276 B                | 19.2 | 19.6 | 20.5 | 21.1 |
| pp256   | Latency256                    | 396 / 404 B                | 19.6 | 20.0 | 21.0 | 21.5 |
| pp512   | Latency512                    | 652 / 660 B                | 20.2 | 20.6 | 21.8 | 22.3 |
| pp1024  | Lat64 + Lat128 + SharedBuffer | 1164 / 1172 B              | 19.8 | 20.6 | 21.8 | 22.5 |
| pp4096  | Lat64 + Lat128 + SharedBuffer | 4236 / 4244 B              | 21.3 | 22.2 | 24.0 | 24.6 |
| pp65536 | u64 + Lat128 + SharedBuffer   | 65676 / 65684 B            | 60.7 | 63.4 | 65.0 | 67.3 |

> `mtrouter`, provider, and consumer each pinned to a dedicated physical core via `taskset`,
> measured from a bare TTY session with a full process restart before each mode switch –
> see §2.1. Values are averages across 10 consecutive cycles per mode.
> **Absolute minimum single-sample observed: 18.34 μs** (`pp0`).
> Raw data: [`areg-latency-benchmark-20260705.csv`](./areg-latency-benchmark-20260705.csv).

#### `gnome-terminal` desktop session, pinned cores (earlier dataset, higher baseline)

| Mode    | Payload struct                | Request / Response total | Min  | P50  | P95  | P99  |
|---------|-------------------------------|---------------------------|------|------|------|------|
| pp0     | None                          | 140 / 148 B               | 21.0 | 21.4 | 22.0 | 23.0 |
| pp8     | Latency8                      | 148 / 156 B                | 21.1 | 21.4 | 22.0 | 23.0 |
| pp16    | Latency16                     | 156 / 164 B                | 21.0 | 21.5 | 22.2 | 23.2 |
| pp32    | Latency32                     | 172 / 180 B                | 21.1 | 21.6 | 22.3 | 23.2 |
| pp64    | Latency64                     | 204 / 212 B                | 21.2 | 21.7 | 22.5 | 23.5 |
| pp128   | Latency128                    | 268 / 276 B                | 21.2 | 21.8 | 22.8 | 23.8 |
| pp256   | Latency256                    | 396 / 404 B                | 21.8 | 22.3 | 23.3 | 24.2 |
| pp512   | Latency512                    | 652 / 660 B                | 22.3 | 22.9 | 24.0 | 24.9 |
| pp1024  | Lat64 + Lat128 + SharedBuffer | 1164 / 1172 B              | 22.4 | 22.9 | 24.1 | 25.1 |
| pp4096  | Lat64 + Lat128 + SharedBuffer | 4236 / 4244 B              | 23.4 | 24.5 | 26.1 | 26.9 |
| pp65536 | u64 + Lat128 + SharedBuffer   | 65676 / 65684 B            | 62.6 | 65.5 | 68.7 | 70.9 |

> Same hardware and pinning, measured inside a `gnome-terminal` desktop session, no restart
> between the 8 runs of a session – see §2.1.
> Raw data: [`areg-latency-benchmark-20260629.txt`](./areg-latency-benchmark-20260629.txt).

### 5.2 macOS – Apple M3 Pro, 32 GB LPDDR5

| Mode    | Payload struct                | Request / Response total | Min  | P50  | P95   | P99   |
|---------|-------------------------------|--------------------------|------|------|-------|-------|
| pp0     | None                          | 140 / 148 B              | 46.4 | 63.3 | 74.8  | 78.1  |
| pp64    | Latency64                     | 204 / 212 B              | 46.0 | 62.5 | 74.6  | 78.3  |
| pp128   | Latency128                    | 268 / 276 B              | 48.1 | 65.4 | 75.7  | 79.0  |
| pp256   | Latency256                    | 396 / 404 B              | 47.8 | 66.2 | 76.6  | 80.0  |
| pp512   | Latency512                    | 652 / 660 B              | 48.8 | 69.5 | 78.0  | 81.0  |
| pp1024  | Lat64 + Lat128 + SharedBuffer | 1164 / 1172 B            | 47.9 | 65.5 | 75.7  | 79.0  |
| pp4096  | Lat64 + Lat128 + SharedBuffer | 4236 / 4244 B            | 49.5 | 70.3 | 78.6  | 81.8  |
| pp65536 | u64 + Lat128 + SharedBuffer   | 65676 / 65684 B          | 82.1 | 95.9 | 102.2 | 106.0 |

> macOS P99 is extremely stable: pp65536 P99 spread = 0.8 μs across 8 runs.
> Consistent with native SSD, well-behaved macOS scheduler.
> The TTY Linux dataset (§2.1) is similarly tight at this mode: pp65536 P99 spread = 2.4 μs
> across its 10 cycles.

### 5.3 Windows 11 – i7-13700H, 32 GB DDR4

| Mode    | Payload struct                | Request / Response total | Min   | P50   | P95   | P99   | Mean  |
|---------|-------------------------------|--------------------------|-------|-------|-------|-------|-------|
| pp0     | None                          | 140 / 148 B              | 63.0  | 81.9  | 84.6  | 104.9 | 81.9  |
| pp32    | Latency32                     | 172 / 180 B              | 63.5  | 82.0  | 84.8  | 108.0 | 82.3  |
| pp64    | Latency64                     | 204 / 212 B              | 64.0  | 82.5  | 85.2  | 107.8 | 82.5  |
| pp128   | Latency128                    | 268 / 276 B              | 64.5  | 82.5  | 89.3  | 133.0 | 83.5  |
| pp256   | Latency256                    | 396 / 404 B              | 64.5  | 82.5  | 85.0  | 108.8 | 82.5  |
| pp512   | Latency512                    | 652 / 660 B              | 65.0  | 84.1  | 87.0  | 113.5 | 84.5  |
| pp1024  | Lat64 + Lat128 + SharedBuffer | 1164 / 1172 B            | 65.3  | 83.0  | 85.8  | 112.5 | 83.5  |
| pp4096  | Lat64 + Lat128 + SharedBuffer | 4236 / 4244 B            | 69.1  | 85.3  | 89.6  | 114.1 | 86.7  |
| pp65536 | u64 + Lat128 + SharedBuffer   | 65676 / 65684 B          | 115.6 | 142.5 | 204.8 | 242.9 | 152.8 |

> Windows RTT is approximately 4.2× higher than Linux at the same hardware (pp64 P50: 82.5 vs 19.5 μs, TTY, pinned cores).
> No bimodal distribution – results are stable and predictable across all runs.
> pp65536 shows higher tail (P99 242 μs) due to TCP segmentation overhead on the Windows loopback stack.

---

## 6. Payload Sensitivity

areg-sdk latency is nearly flat from 140 B to 4 KB. Framework overhead (routing,
dispatch, thread wakeup) dominates – payload serialization adds minimal overhead
for messages below the TCP segment boundary (~1460 B).

### Linux OWT (bc modes, Min values) – TTY dataset

| Total msg size    | Min (μs) | Delta from bc0   |
|-------------------|----------|-------------------|
| 140 B (bc0)       | 9.4      | baseline          |
| 148 B (bc8)       | 9.4      | +0.0 μs           |
| 156 B (bc16)      | 9.4      | +0.0 μs           |
| 172 B (bc32)      | 9.4      | +0.0 μs           |
| 204 B (bc64)      | 9.6      | +0.2 μs           |
| 268 B (bc128)     | 9.6      | +0.2 μs           |
| 396 B (bc256)     | 9.8      | +0.4 μs           |
| 652 B (bc512)     | 10.1     | +0.7 μs (+7%)     |
| 1164 B (bc1024)   | 10.0     | +0.6 μs (+6%)     |
| 4236 B (bc4096)   | 10.6     | +1.2 μs (+13%)    |
| 65676 B (bc65536) | 26.0     | +16.6 μs (+177%)  |

From 204 B (bc64) to 4236 B (bc4096) (~21× size increase): Min increases only 1.0 μs.
Below 4 KB, payload size has effectively no measurable impact on Min latency.
The jump at 65 KB is due to TCP segmentation: a 65 KB message requires approximately
45 TCP segments (at 1460 B MTU), each requiring a separate kernel write.
This pattern is identical in shape to the earlier `gnome-terminal` dataset – only the
absolute floor shifts (see §2.1), not the flatness-below-4KB conclusion.

### macOS OWT (bc modes, Min values)

macOS shows similarly flat sensitivity from 140 B to 4 KB (+1.5 μs over 30×),
with the same TCP-segmentation jump at 65 KB (+20.2 μs from bc4096 to bc65536).

---

## 7. Throughput Results (23_pubdatarate)

### 7.1 Linux – Ubuntu 26.04 LTS (Performance mode, i7-13700H, 32 GB DDR4)

| Metric        | Payload | Peak            | Sustained       |
|---------------|---------|-----------------|-----------------|
| Message rate  | ~0.5 KB | **~2.5M msg/s** | ~2.0M msg/s     |
| Data rate     | ~3 MB   | **~8.0 GB/s**   | ~6.0–6.5 GB/s   |

> Peak is the best short-run reading; sustained is the stable 5-minute rate.
> Both measured under Ubuntu 26.04, **Performance** power mode, **cores not pinned**.
> Pinning `mtrouter`/provider/consumer to dedicated cores for latency measurement (§2.1,
> §4.1) reduces these throughput figures by roughly **3×** on both data rate and message
> rate – the two configurations are not simultaneously achievable.

### 7.2 macOS – Apple M3 Pro, 32 GB LPDDR5

| Metric | Payload | Sustained rate |
|--------|---------|---------------|
| Message rate | ~0.5 KB | **~2.5M msg/s** |
| Data rate | ~3 MB | **~6.7–7.0 GB/s** |

Higher message rate than Linux (2.5M vs 2.0M sustained) due to LPDDR5 memory bandwidth
and a consistently active CPU performance state on Apple Silicon.

### 7.3 Windows 11 – i7-13700H, 32 GB DDR4

| Metric        | Payload | Peak            | Sustained   |
|---------------|---------|-----------------|-------------|
| Message rate  | ~0.5 KB | **~2.5M msg/s** | ~1.8M msg/s ² |
| Data rate     | ~3 MB   | **~3.0 GB/s**   | ~2.2 GB/s   |

Message rate at small payloads declines over the first 2 minutes before stabilizing;
Windows sustained data rate is approximately 2.7–3.0× lower than Linux sustained (2.2 vs 6.0–6.5 GB/s)
due to higher per-message TCP overhead on the Windows loopback stack.

---

## 8. Cross-Platform Summary

### OWT Latency (bc64, 204 B)

| Platform         | Min         | P50         | P95         | P99         | Mean        |
|------------------|-------------|-------------|-------------|-------------|-------------|
| **Linux Ubuntu** ¹ | **9.6 μs** | **9.9 μs** | **10.1 μs** | **11.0 μs** | **9.9 μs** |
| **macOS M3 Pro** | 21.6 μs     | 31.4 μs     | 37.8 μs     | 40.6 μs     | 31.6 μs     |
| **Windows 11**   | 32.5 μs     | 40.3 μs     | 43.3 μs     | 60.0 μs     | 41.2 μs     |

¹ TTY dataset, restart-per-mode (§2.1) – the current reference. `gnome-terminal` dataset
reads ~0.9–1.4 μs higher; see §4.1.

### RTT Latency (pp64, 204+212 B)

| Platform         | Min         | P50         | P95         | P99         |
|------------------|-------------|-------------|-------------|-------------|
| **Linux Ubuntu** ¹ | **19.1 μs** | **19.5 μs** | **20.1 μs** | **20.9 μs** |
| **macOS M3 Pro** | 46.0 μs     | 62.5 μs     | 74.6 μs     | 78.3 μs     |
| **Windows 11**   | 64.0 μs     | 82.5 μs     | 85.2 μs     | 107.8 μs    |

¹ TTY dataset, restart-per-mode (§2.1) – the current reference.

### Throughput

| Platform         | Msg/s peak | Msg/s sustained | Data rate peak | Data rate sustained |
|------------------|------------|-----------------|----------------|---------------------|
| **Linux Ubuntu** | ~2.5M      | ~2.0M           | **~8.0 GB/s**  | ~6.0–6.5 GB/s     |
| **macOS M3 Pro** | **~3.0M**  | **~2.5M**       | ~7.0 GB/s      | **~6.5 GB/s**       |
| **Windows 11**   | ~2.5M      | ~1.8M           | ~3.0 GB/s      | ~2.2 GB/s           |

### Platform Profiles

**Linux** (TTY, pinned cores) offers the lowest OWT Min and RTT Min latency (9.6 μs / 19.1 μs),
the lowest P99 of all three platforms (11.0 μs bc64 OWT), and the
highest peak data rate and message rate of all three platforms (unpinned, see §2.1).

**macOS M3 Pro** offers the highest confirmed *sustained* message rate (2.5M msg/s) versus
Linux's 2.0M. The higher P50 latency vs Linux reflects macOS TCP stack per-packet overhead;
macOS was not re-tested with core pinning.

**Windows 11** has the highest OWT and RTT latency and the lowest *sustained* data rate.
Sustained throughput is stable at ~2.2 GB/s; peak readings now reach ~3.0 GB/s / ~2.5M msg/s.
Message rate declines over time on long runs. Suitable for Windows-native production deployments.

---

## 9. Brief Competitive Context

The following comparisons use **only publicly available, measured benchmark data**.
All areg-sdk numbers above include full service dispatch, routing, serialization, and deserialization.

### 9.1 ZeroMQ, NanoMsg, NNG – TCP Loopback

**Source:** Hitachi Energy Research, *"Performance Evaluation of Brokerless Messaging Libraries"*,
arXiv:2508.07934v1 (August 2025).
Raw CSV data: [github.com/hitachienergy/messaging-libraries-benchmark/blob/main/results/results.csv](https://github.com/hitachienergy/messaging-libraries-benchmark/blob/main/results/results.csv)

**Test conditions (competitors):** Intel Xeon w3-2435, 8 cores, 3.1 GHz / 4.2 GHz boost,
64 GB RAM, Linux native, publisher and subscriber pinned to isolated cores.
T=1000 μs publishing interval (1 message per millisecond, low load).
PUB/SUB pattern, 1 subscriber, 5000 messages. Raw transport only – no service dispatch.

**TCP OWT comparison – 1 KB payload:**

| Framework          | Min         | Avg         | P90/P95     | P99         | Transport        | Scope      |
|--------------------|-------------|-------------|-------------|-------------|------------------|------------|
| **areg-sdk Linux** | **10.0 μs** | **10.5 μs** | **11.4 μs** | **11.6 μs** | TCP 2-hop broker | Full stack |
| NanoMsg            | 18.0 μs     | 21.9 μs     | 22.3 μs     | 24.8 μs     | TCP direct       | Raw        |
| ZMQ                | 22.0 μs     | 27.5 μs     | 28.5 μs     | 31.6 μs     | TCP direct       | Raw        |
| NNG                | 24.3 μs     | 34.9 μs     | 39.7 μs     | 48.4 μs     | TCP direct       | Raw        |

**TCP OWT comparison – 4 KB payload:**

| Framework | Min | Avg | P90/P95 | P99 |
|-----------|-----|-----|-----|-----|
| **areg-sdk Linux** | **10.6 μs** | **11.2 μs** | **12.3 μs** | **13.0 μs** |
| NanoMsg | 19.3 μs | 24.5 μs | 26.3 μs | 28.0 μs |
| ZMQ | 23.9 μs | 29.3 μs | 31.0 μs | 34.1 μs |
| NNG | 27.0 μs | 35.6 μs | 39.8 μs | 50.2 μs |

**TCP OWT comparison – 64 KB payload:**

| Framework | Min | Avg | P90/P95 | P99 |
|-----------|-----|-----|-----|-----|
| **areg-sdk Linux** | **26.0 μs** | **26.9 μs** | **28.2 μs** | **31.0 μs** |
| NNG | 39.9 μs | 49.4 μs | 55.6 μs | 65.2 μs |
| ZMQ | 43.6 μs | 52.6 μs | 53.4 μs | 59.5 μs |
| NanoMsg | 32.1 μs | 171.1 μs ⚠️ | 303.9 μs ⚠️ | 307.3 μs ⚠️ |

> Figures use the TTY dataset (§2.1), the current reference. Using the `gnome-terminal`
> dataset instead, areg-sdk still wins Min/Avg at every size but trails NanoMsg's P99 at
> 1 KB and 4 KB – see the archived comparison in this document's revision history if needed.
>
> **NanoMsg 64 KB anomaly (T=1000 μs):** Nagle's algorithm fires at this payload size.
> Min (32.1 μs) remains clean but Avg jumps to 171 μs – a 5.3× degradation.
> areg-sdk shows near-flat latency across all sizes (no Nagle issue).
>
> **areg-sdk now wins every metric – Min, Avg, P90/P95, and P99 – at all three payload sizes**,
> under conditions still harder than the competitors': max send rate (T=0) vs T=1000 μs
> (1 msg/ms), 2-hop broker vs direct, full dispatch vs raw, mobile CPU vs Xeon workstation,
> pinned-but-not-isolated cores vs isolated cores. No caveat remains on the tail-latency
> comparison at any size.
>
> **RTT floor vs 2× NanoMsg OWT:**
> areg-sdk pp64 RTT Min (19.1 μs) < 2 × NanoMsg TCP OWT Min (18.0 × 2 = 36.0 μs).
> The complete areg-sdk service framework – 4-hop brokered RTT, both-sides serialization,
> dispatch, and method call – adds less overhead than a second raw NanoMsg TCP one-way hop.

**Reliability at maximum send rate (T=0) – from the same Hitachi CSV:**

| Framework          | 1 KB T=0 Avg | 1 KB T=0 P99  | Stable?            |
|--------------------|--------------|---------------|--------------------|
| **areg-sdk Linux** | **10.5 μs**  | **11.6 μs**   | **✅**             |
| ZMQ                | 25.3 μs      | 30.3 μs       | ✅                 |
| NNG                | 42.8 μs      | 542.8 μs      | ❌ P99 collapsed   |
| NanoMsg            | 452.8 μs     | 867.1 μs      | ❌ Broken (Nagle)  |

At the same send rate as areg-sdk: NanoMsg and NNG become unreliable. areg-sdk now beats
ZMQ on both Mean (10.5 vs 25.3 μs) and P99 (11.6 vs 30.3 μs) – no caveat remains at equal load.

> **NanoMsg maintenance status:** 0 commits and 2 open issues recorded
> January–April 2025 per the Hitachi Energy paper. The library is effectively unmaintained.

### 9.2 Zenoh – UDP / SHM, same machine

**Source:** ZettaScale, *"Zenoh vs MQTT, Kafka, and DDS"* blog post, March 2023.
[zenoh.io/blog/2023-03-21-zenoh-vs-mqtt-kafka-dds](https://zenoh.io/blog/2023-03-21-zenoh-vs-mqtt-kafka-dds/)

Single-machine latency (same hardware, NTU test environment):
- Zenoh P2P (direct, SHM/UDP): **~10 μs OWT**
- Zenoh brokered (through zenoh router, UDP): **~21 μs OWT**

The same post reports CycloneDDS single-machine OWT at **~8 μs** (auto-selects SHM).

These are raw pub-sub transport numbers (no service dispatch, no method call).
Zenoh P2P uses shared memory when publisher and subscriber are on the same machine.
areg-sdk uses TCP through a centralized broker regardless of topology.

**Context:** areg-sdk Linux bc64 OWT Mean (9.9 μs, TTY, pinned cores – §2.1) is now lower
than Zenoh brokered (21 μs raw) by a wide margin, and is essentially at parity with –
fractionally below – Zenoh P2P's own **~10 μs** figure, despite running on TCP with full
service dispatch against Zenoh P2P's raw SHM/UDP pub-sub path. One caveat on precision:
the source for Zenoh's "~10 μs" does not state which statistic (Min, Avg, P50) it
represents, so this should be read as "now essentially tied, marginally ahead" rather than
a decisively verified rank flip. CycloneDDS (~8 μs) and Aeron IPC (~7.978 μs mean) – both
also SHM – remain measurably lower than areg-sdk's TCP path.


### 9.3 Aeron – UDP loopback

**Source:** E. Sequeira, *"Aeron Latency at Lower Throughputs"*, May 2023.
[esequeira.com/posts/aeron-latency-at-lower-throughputs](https://esequeira.com/posts/aeron-latency-at-lower-throughputs/)

Personal blog observation (hardware not specified). Aeron configured in `DEDICATED`
threading mode (recommended for low latency), default stock configuration.
Test rate: up to 10K msg/s. Raw transport, no service dispatch.

| Transport | P50 baseline | P90 (default config) | P90 (spin idle strategy) |
|-----------|-------------|---------------------|--------------------------|
| Aeron UDP | **~50 μs** | **> 6 ms** | **< 120 μs** |
| Aeron IPC | **~40 μs** | – | ~120 μs (after MTU fix) |

> The large P90 gap with default config is caused by the `backoff` idle strategy, which
> uses OS yields and sleeps between sends. Switching to `spin` eliminates this jitter.
> Aeron IPC default also showed max latencies >100 ms (fixed by increasing `IPC_MTU_LENGTH`).
>
> **Context:** areg-sdk Linux bc64 P50 (~9.9 μs, full TCP dispatch, TTY, pinned cores) is
> approximately 5.1× lower than Aeron UDP P50 (~50 μs, raw transport) at comparable message
> rates. Hardware is different and tuning matters significantly for Aeron.
> Comparison is indicative, not conclusive.

### 9.4 gRPC C++ – Unix domain socket, sequential single-client

**Source:** Felix Werner, Max Planck Institute for Nuclear Physics, September 2021.
[mpi-hd.mpg.de/personalhomes/fwerner/research/2021/09/grpc-for-ipc](https://www.mpi-hd.mpg.de/personalhomes/fwerner/research/2021/09/grpc-for-ipc/)

**Hardware:** AMD EPYC 7402P server, CentOS 8, "latency-performance" profile, gRPC v1.40.0.
**Methodology:** Pure synchronous unary call, 1 million iterations per configuration,
single client thread, no concurrency. Sequential – one call completes before the next starts.
**Transport:** Unix domain socket (`unix://` address). Not TCP.

| Configuration | Median | P95 | P99 |
|--------------|--------|-----|-----|
| gRPC, same CPU core | 167 μs | 178 μs | 200 μs |
| gRPC, different CPU cores | **116 μs** | 129 μs | 142 μs |
| Plain UDS blocking I/O, same core | 4 μs | 5 μs | 6 μs |
| Plain UDS blocking I/O, other core | 11 μs | 12 μs | 13 μs |

> Transport note: Unix domain sockets are faster than TCP for local IPC (UDS bypasses
> TCP header processing). gRPC over TCP loopback would be equal or higher than these values.
>
> **areg-sdk RTT comparison (pp64, TCP, 4-hop broker, full dispatch):**
> areg-sdk Linux P50 ~19.5 μs (TTY, pinned cores) vs gRPC UDS median ~116 μs (other core) –
> areg-sdk full-stack RTT is approximately 6.0× lower despite more hops,
> full typed serialization both sides, and TCP vs Unix domain socket.

### 9.5 Notes on Other Frameworks

The following frameworks have public performance claims but specific loopback latency
numbers could not be extracted from their published pages for inclusion here:

- **Aeron IPC/SHM (~7.978 μs mean):** [Pirogov, Medium, Sept 2017](https://medium.com/@pirogov.alexey/aeron-low-latency-transport-protocol-9493f8d504e8)
  reports `EmbeddedPingPong.java` Mean=7.978 μs with BusySpin. This is Aeron **IPC
  (shared memory)**, not UDP loopback; for Aeron UDP see the ~50 μs baseline in section 9.3.

- **FastDDS (eProsima):** Performance data at
  [eprosima.com/developer-resources/performance/eprosima-fast-dds-performance](https://www.eprosima.com/developer-resources/performance/eprosima-fast-dds-performance)
  is rendered as charts. One community observation in the
  [OpenRobotics discourse](https://discourse.openrobotics.org/t/dds-implementation-performance-benchmark/19343)
  notes FastDDS at ~11 μs for **intraprocess** (same process) communication
  with an RT-Preempt kernel – not inter-process UDP. Inter-process latency
  from the eProsima benchmark exists but is in chart images only.

- **Chronicle Queue (OpenHFT):** Uses memory-mapped OS page cache for IPC –
  a fundamentally different transport from TCP. Not directly comparable to areg-sdk.
  Source: [github.com/OpenHFT/Chronicle-Queue-Demo](https://github.com/OpenHFT/Chronicle-Queue-Demo).

- **gRPC (grpc_bench):** Community benchmark at
  [github.com/LesnyRumcajs/grpc_bench](https://github.com/LesnyRumcajs/grpc_bench)
  runs with 1000 concurrent in-flight requests. These are concurrency/throughput
  numbers, not sequential latency. Not comparable to single-message-in-flight methodology.

---

## 10. Interpreting Results

| Metric | What it means | Design guidance |
|--------|--------------|-----------------|
| **Min** | Best-case – OS scheduler and TCP cooperated perfectly | Hardware/stack floor; use for theoretical planning |
| **P50** | Median – the typical delivery time | Primary latency metric for most workloads |
| **P95** | 5% of messages took longer than this | Soft real-time deadline planning |
| **P99** | Tail latency – 1% worst case | Hard real-time deadline planning |
| **Mean** | Arithmetic average; skewed by outliers | Use P50 instead for typical behaviour |

**For real-time systems:** design to P99, not P50. The gap between P50 and P99 reveals OS
scheduler and TCP stack jitter on your specific hardware. macOS M3 Pro shows the tightest
P50–P99 gap (best determinism).

**For throughput-sensitive systems:** use the `23_pubdatarate` example to measure your
specific payload size and channel count on your target hardware, as throughput scales
with both message size and available memory bandwidth.

---

## 11. Reproducing the Results

### 30_publatency – Latency

Two raw datasets are published, reflecting the two environments in §2.1:
→ **[areg-latency-benchmark-20260705.csv](./areg-latency-benchmark-20260705.csv)** (bare TTY, current reference)
→ **[areg-latency-benchmark-20260629.txt](./areg-latency-benchmark-20260629.txt)** (gnome-terminal, earlier baseline)

Both include the exact `taskset` commands used.

Start three processes in order. On Linux, pin each to a dedicated physical core
(not hyperthread siblings) to match the published Linux numbers. For the lowest published
numbers, run from a bare TTY (`Ctrl+Alt+F2`–`F6` on most distros, no desktop session active)
rather than a desktop terminal emulator – see §2.1 for the measured difference:

```bash
# Terminal 1
taskset -c 10 ./mtrouter.elf    # Linux – pin to a P-core
./mtrouter.mac                   # macOS – no pinning
.\mtrouter.exe                   # Windows – no pinning

# Terminal 2
taskset -c 6 ./30_pubprovider.elf  # Linux
./30_pubprovider.elf               # macOS / Windows (adjust extension per platform)

# Terminal 3
taskset -c 1 ./30_pubconsumer.elf  # Linux
./30_pubconsumer.elf               # macOS / Windows
```

In the consumer console, set parameters once and run 8–10 times with `-s`. If reproducing
the TTY methodology, restart all three processes before switching to a different `-m` mode
(§2.1) – do not just change `-m` and continue in the same session:

```
# OWT (broadcast)
-m=bc64 -c=5000 -w=1000
-s   (repeat 8 times)

# RTT (ping-pong)
-m=pp64 -c=10000 -w=1000
-s   (repeat 8 times)
```

Results appear in a scrolling table of the last 8 runs.

### 23_pubdatarate – Throughput

```bash
# Terminal 1: mtrouter
# Terminal 2: 23_pubclient.elf
# Terminal 3: 23_pubservice.elf
```

In the provider console, configure then start:

```
# ~3 MB messages, data rate
-w=1024 -h=1024 -l=1024 -t=25 -c=55
-s
# Read mtrouter console for network-layer throughput
# Run for 5+ minutes for sustained figure; read mtrouter before stopping
-p
```

Watch `mtrouter` output – it shows the highest-accuracy throughput signal
(raw bytes at the socket boundary, both directions).

---

*Measurements taken June 2026 (`gnome-terminal` dataset) and July 2026 (bare TTY dataset, current reference – see §2.1).*
*areg-sdk build: Release, `AREG_LOGGING=OFF`.*
*All latency values: TCP loopback (127.0.0.1) only. No cross-machine data.*
