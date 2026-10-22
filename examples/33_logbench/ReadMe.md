# Logging Overhead Latency Benchmark

This example measures the overhead latency of different logging operations in the areg SDK, including varying argument counts, argument types and message lengths.

## Building

From the root of the areg SDK repository:

```bash
cmake -S . -B build
cmake --build build --target 33_logbench
```

## Running

Run the `33_logbench` executable from the appropriate build output directory.

```bash
./33_logbench [options]
```

### Configuration Options

| Option | Description | Default | Valid values |
|---|---|---|---|
| `-t` | Number of threads | 8 | 1, 2, 4, 8, 16 |
| `-l` | Number of loops | 10000 | 1000–100000 |
| `-a` | Maximum number of logging arguments | 8 | 1–12 |
| `-af` | Number of floating-point arguments | 2 | 0–12 |
| `-ad` | Number of integer arguments | 4 | 0–12 |
| `-as` | Number of string arguments | 2 | 0–12 |
| `-sl` | Length of string arguments | 16 | 4–64 |
| `-s` | Logging destination | `collect` | `file`, `db`, `collect`, `all` |

The combined number of floating-point, integer and string arguments must not exceed the maximum specified by `-a`.

### Example

Run the benchmark with 4 threads, 1000 loops and file logging:

```bash
./33_logbench -t 4 -l 1000 -s file
```

Run a benchmark with 6 mixed arguments:

```bash
./33_logbench -a 6 -af 2 -ad 2 -as 2
```

## Benchmark Cases

The benchmark covers the following logging scenarios:

| Case | Description |
|---|---|
| `LgScp` | Logging scope without a log message |
| `PureStr` | Logging a constant string |
| `Dbg1a` | Logging one argument |
| `Dbg2a` | Logging two arguments |
| `Dbg3ma` | Logging three mixed-type arguments |
| `Dbg10ma` | Logging ten mixed-type arguments |
| `DbgLngStr` | Logging a long string |
| `DynStr` | Logging a dynamically configured message |

## Output

Results are displayed in a console table, with all latency measurements reported in microseconds (µs).

| Metric | Description |
|---|---|
| Min / Max | Minimum and maximum latency |
| Mean | Average latency |
| P50 / P90 / P99 / P999 | 50th, 90th, 99th and 99.9th percentile latency |
| SD | Standard deviation |

## Verification

To verify the benchmark:

1. Build the `33_logbench` target.
2. Run the benchmark with its default configuration.
3. Run the benchmark with custom command-line options.
4. Confirm that the program completes successfully and displays the latency statistics table.