# 32_pubmixed - what a bulk stream costs the small messages beside it

## Why this example exists

Every other example hides the case that matters most in a real system:

* examples **30** and **31** send only small messages, so the send queue never fills;
* example **23** sends only large ones, so no small message ever waits behind a big one.

A real service does both on the same connection: control traffic and data traffic. This example
runs them together and measures what the bulk stream does to the round-trip time of the small
requests.

## What it does

**Provider** (`32_pubservice`) starts a configurable number of worker threads that all broadcast
large blocks on the same connection, and answers small `ping` requests on that same connection.

**Consumer** (`32_pubclient`) receives the bulk stream and, at a fixed rate, sends `ping` and
measures the round trip. It prints the bulk rate and the round-trip percentiles once per second,
and a summary at the end.

## Running it

Start `mtrouter`, then:

```
32_pubservice -b=262144 -p=4 -g=2000      # 4 threads, 256 KB blocks, 2 ms apart
32_pubclient  -i=5000 -r=30               # ping every 5 ms, stop after 30 s
```

Measure the reference first, with no bulk traffic at all:

```
32_pubservice -q
32_pubclient  -i=5000 -r=30
```

### Provider options

| Option | Meaning | Default |
|---|---|---|
| `-b=BYTES` | size of one bulk block | 262144 |
| `-p=COUNT` | bulk producer threads, max 16 | 4 |
| `-g=USEC` | gap between two blocks of one thread | 2000 |
| `-q` | no bulk traffic - use it for the reference round-trip | off |
| `-n` | headless | off |

### Consumer options

| Option | Meaning | Default |
|---|---|---|
| `-i=USEC` | gap between two pings | 5000 |
| `-r=SEC` | stop after this many seconds | run until quit |
| `-n` | headless | off |

## What it shows

Measured on one development machine - relative numbers only, never publish them as the
performance of areg-sdk.

**A moderate bulk stream already costs the tail.** 4 threads, 256 KB blocks, about 457 MB/s:

| | p50 us | p95 us | p99 us | max us |
|---|---|---|---|---|
| no bulk (`-q`) | 643 | 1172 | 1596 | 3629 |
| with bulk | 402 | 1511 | **2951** | **9669** |

The median is not the story. The 99th percentile nearly doubles and the maximum nearly triples.

**A heavy bulk stream destroys it, and the queue capacity is the reason.**
8 threads, 1 MB blocks, no gap:

| `config::*::queue::capacity` | ring holds | bulk | ping p50 |
|---|---|---|---|
| 1024 (default) | 1024 x 1 MB = 1 GB | 3859 MB/s | **258 ms** |
| 64 | 64 x 1 MB = 64 MB | 3555 MB/s | **39 ms** |

A small message waits for everything already in the ring to reach the socket. With 1024 slots of
1 MB that is a gigabyte ahead of it - and 0.93 GB is exactly what 258 ms at 3859 MB/s works out
to. Cutting the ring to 64 slots cuts the wait by 6.6x and costs 7.9 % of the bulk rate.

**The tuning rule this gives you:** for a service that sends large messages on a connection that
also carries small ones, the worst-case delay of a small message is about
`queue::capacity x message size / throughput`. Lower `config::*::queue::capacity` until that
number is acceptable. It is a latency setting as much as a memory one.

## Seeing the back-pressure

When producers outrun the socket they wait for a free queue slot. The framework reports a wait
longer than one second, but framework scopes are off by default. To see it, enable them:

```
log::*::enable::file  = true
log::*::scope::areg_* = WARN | SCOPE ;
```

Then a run of `-b=1048576 -p=8 -g=0` produces lines such as:

```
WARN >>> Send queue was full: a producer waited [ 4973 ] ms for a free slot
```
