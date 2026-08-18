# 31_loclatency - Local latency benchmark

## What this example measures

This example answers one question: **how long does one message need inside a single
process?**

The Areg SDK promises that the same service code runs unchanged whether the two sides sit
in one thread, in two threads, in two processes or on two machines. Each of those cases
costs a different amount of time. This example measures the two cheapest cases:

| Topology  | Where provider and consumer run          | What happens to a message |
|-----------|------------------------------------------|---------------------------|
| `same`    | Two components in **one** component thread | The message is put into the event queue of the thread and taken out of it by that very same thread. The thread never falls asleep during the test, so no thread wake-up happens at all. |
| `cross`   | Two components in **two** component threads | The message crosses one thread boundary: the sending thread puts it into the queue of the receiving thread and wakes that thread up. |

The difference between the two rows is the price of **one thread wake-up**.

Example [30_publatency](../30_publatency/ReadMe.md) measures the same messages between two
**processes**. The difference between example 30 and the `cross` row of example 31 is the
price of **leaving the process**: two sockets and the message router.

Together the two examples cover the whole ladder:

```
same thread   ->  cross thread   ->  two processes  ->  two machines
(example 31)      (example 31)       (example 30)       (example 30)
```

## Why it needs nothing else to run

The service interface of this example is **Private** (see `services/LocalLatency.siml`).
A private service never leaves its process, so the message router is not involved and does
not have to be started. Just run the program.

## What one sample is

The benchmark keeps **one message on the way at a time**. The next message is sent only
after the previous one has arrived. That is what makes the number a latency and not a
throughput: no queue can build up and hide the cost of a single message.

**One-way trip (`bc*` modes).** The consumer asks the provider for one message
(`message_next`). The provider takes a timestamp and sends one broadcast. The consumer
takes its own timestamp when the broadcast arrives. The sample is the difference. The
asking call happens *before* the provider's timestamp, so it is not part of the sample.

```
consumer --- message_next ---> provider     (not measured)
                               [t0]
consumer <--- message_N ------ provider     (measured: t1 - t0)
[t1]
```

**Round trip (`pp*` modes).** The consumer takes a timestamp and sends a request. The
provider takes its own timestamp and answers straight away. The consumer takes a second
timestamp when the answer arrives.

```
[t0]
consumer --- ping_pong_N ----> provider     (measured as "in-leg": t1 - t0)
                               [t1]
consumer <--- response -------- provider    (measured together: t2 - t0)
[t2]
```

The `in-leg` column of the result table is the median of `t1 - t0`, so you can see how the
round trip splits into the way there and the way back.

## Running it

The program is **not interactive**. It runs, prints the result and exits. This is on
purpose: measurements are taken by scripts, and a benchmark that waits for a key press
cannot be scripted.

```bash
# defaults: same thread, modes pp0 and bc0, 100000 measured messages, 10000 warm-up
31_loclatency

# one thread wake-up per message, five modes, results into a CSV
31_loclatency -t=cross -m=bc0,pp0,bc64,pp64,bc1024 -c=200000 -w=20000 -o=result.csv

# every mode, three times, so you can see how steady the machine is
31_loclatency -t=same -m=all -r=3 -l=before-T1
```

### Options

| Option | Meaning | Default |
|--------|---------|---------|
| `-t`, `--topology=same\|cross` | Where provider and consumer run | `same` |
| `-m`, `--mode=<list>` | Comma separated mode names, or the group names `all`, `bc`, `pp` | `pp0,bc0` |
| `-c`, `--count=<number>` | Measured messages per run | `100000` |
| `-w`, `--warmup=<number>` | Messages sent before measuring starts | `10000` |
| `-r`, `--repeat=<number>` | How often the whole mode list is measured | `1` |
| `-o`, `--csv=<path>` | Append the results to this CSV file | none |
| `-l`, `--label=<text>` | Free text written into every CSV line | empty |
| `--list` | Print the accepted mode names and exit | |
| `-q`, `--quiet` | Print only the result table | off |
| `-h`, `--help` | Print the usage text and exit | |

### Modes

The number in a mode name is the size of the **extra payload** in bytes, on top of the
identifier and the timestamps.

- One-way trip: `bc0 bc8 bc16 bc32 bc64 bc128 bc256 bc512 bc1024 bc4096 bc65536`
- Round trip: `pp0 pp8 pp16 pp32 pp64 pp128 pp256 pp512 pp1024 pp4096 pp65536`

The payload types are exactly the ones of example 30 (they are re-used from
`examples/30_publatency/common/common.hpp`), so the two examples can be compared directly.

### Exit codes

| Code | Meaning |
|------|---------|
| `0` | The benchmark finished. |
| `1` | The command line was wrong. Nothing was measured. |
| `2` | The benchmark got stuck: no message arrived for 20 seconds. |

Code `2` exists so that an unattended script always ends instead of waiting for ever.

## Reading the result table

```
 topo  | mode     | bytes | rep |  samples |     min |     p50 |     p90 |     p99 |     p99.9 |       max |    mean |  stddev |   in-leg |      msg/s
 ------+----------+-------+-----+----------+---------+---------+---------+---------+-----------+-----------+---------+---------+----------+-----------
 same  | bc0      |     0 |   1 |    20000 |   0.395 |   0.409 |   0.417 |   0.477 |     1.470 |    14.536 |   0.417 |   0.213 |    0.000 |    1226302
```

All times are **microseconds**.

| Column | Meaning |
|--------|---------|
| `min` | The fastest sample. This is the floor the machine can reach. Use it to compare code changes: it is the least disturbed number. |
| `p50` | The median. Half of the samples were faster. This is the number to quote as "the" latency. |
| `p90`, `p99`, `p99.9` | The tail. `p99.9` shows what a rare slow message costs. |
| `max` | The slowest single sample. Usually an interruption by the operating system, not by the framework. |
| `mean`, `stddev` | Average and spread. A large `stddev` next to a small `p50` means the machine was busy with something else. |
| `in-leg` | Round trip only: the median of the way from the consumer to the provider. |
| `msg/s` | Measured samples divided by the measured wall clock time. With one message at a time this is `1 / latency`, not the maximum throughput of the framework. |

Before the table the program prints the cost of one clock reading, for example
`Clock cost : 21.6 ns per reading`. Every sample contains two such readings. On the
`same` topology, where a sample can be below half a microsecond, this is no longer a
small share, so subtract it when you compare very small numbers.

## Things to keep in mind when you compare numbers

1. **Never compare numbers from different machines, and never from a virtual machine.**
   Under WSL2 a thread wake-up costs many times what it costs on the same machine booted
   natively. The `same` topology, which has no wake-up at all, is barely affected; the
   `cross` topology is affected a lot. Comparing the two across environments is
   meaningless.

2. **Payload size is not the only thing that changes with the mode.** The larger payloads
   are built from different building blocks: up to `512` bytes they are nested structures
   that contain text, from `1024` bytes upwards they are one block of memory. That is why
   `bc512` can be slower than `bc1024`. The cost being measured is the cost of packing the
   payload, not only its size.

3. **Compare `min` and `p50`, not `max`.** A single `max` value tells you what the
   operating system did to the process, not what the framework costs.

4. **Use the same `--count` on both sides of a comparison.** The memory allocator of the
   process is asked to give memory back to the system after every 100000 dispatched
   events. A run that crosses this border and a run that does not are not comparable.

## Running it as part of the test suite

Both examples are benchmark scenarios of the shared example driver, so one command measures
the whole ladder from one thread to two processes:

```bash
python3 tools/run-all-examples.py --tier perf --only 31_locsame,31_loccross,30_owt,30_rtt --perf
```

The driver prints every run and then works out the two steps of the ladder, per mode:

```
ladder price of one thread wake-up, bc64      13.861 us (two threads minus one thread)
ladder price of leaving the process, bc64     65.514 us (two processes minus two threads: ...)
```

Only the same mode is ever compared: a round trip carries two messages and a one-way trip
carries one, so mixing them would invent a number that was never measured. Example 31 needs
no message router, and the driver is told so, which keeps a router from competing for the
processor while the latencies are being taken.

## Files

| File | Content |
|------|---------|
| `services/LocalLatency.siml` | The service interface: one pull request, eleven broadcasts, eleven request and response pairs. |
| `common/common.hpp` | Re-uses the payload types of example 30, so both examples send identical messages. |
| `src/LatencyOptions.hpp/.cpp` | Command line, mode names, clock helpers. |
| `src/LatencyReport.hpp/.cpp` | Sample collection, statistics, printing, CSV export. |
| `src/LocalLatencyProvider.hpp/.cpp` | The service side. Does as little as possible on purpose. |
| `src/LocalLatencyConsumer.hpp/.cpp` | Drives the session, measures, reports, and watches for a stall. |
| `src/main.cpp` | Builds the model that matches the requested topology and runs it. |
