#!/usr/bin/env python3
# -*- coding: ascii -*-
"""
Benchmark measurement support for tools/run-all-examples.py.

The two performance examples print their numbers into a full screen console that repaints
itself in place, so the captured output is a stream of ANSI escapes with the same lines
rewritten hundreds of times. This module turns that stream into a small table of numbers,
prints it where a build server log will show it, and writes it to files that can be picked
up later.

What is measured
----------------
23_pubdatarate  data rate    the peak network send rate reached while the channel count
                             is ramped up, in bytes per second
                message rate the peak block (message) rate, in blocks per second
30_publatency   OWT          one way trip, broadcast mode, the P50/P95/P99 of the run
                RTT          round trip time, ping pong mode, the same percentiles

Nothing here is specific to a machine or a checkout: every path is derived from the
arguments, and the module is imported, never executed on its own.

Usage from the driver:

    import areg_benchmarks
    measures = areg_benchmarks.extract(scenario_name, [(binary_name, captured_bytes), ...])
    areg_benchmarks.report(all_measures, out_dir)
"""

import os
import re

# ---------------------------------------------------------------------------
# Parsing.
# ---------------------------------------------------------------------------

# The console rewrites the same screen position over and over, so the captured stream holds
# every intermediate value. The escapes are stripped first, then every occurrence is read and
# the peak is kept: the channel count is ramped up during the run and the peak is the answer
# the benchmark is asked for.
_ANSI = re.compile(rb'\x1b\[[0-9;?]*[a-zA-Z]')

# " Network sent rate ..: data   [ %8.2f ] %s / sec, [ %u ] blocks/sec."
_NET_RATE = re.compile(
    r'Network sent rate[ .]*:\s*data\s*\[\s*([0-9.]+)\s*\]\s*(\w+)\s*/\s*sec,\s*\[\s*(\d+)\s*\]\s*blocks/sec')

# " Broadcast rate .....: sent   [ %8.2f ] %s / sec, [ %u ] blocks/sec."
_SVC_RATE = re.compile(
    r'Broadcast rate[ .]*:\s*sent\s*\[\s*([0-9.]+)\s*\]\s*(\w+)\s*/\s*sec,\s*\[\s*(\d+)\s*\]\s*blocks/sec')

# " Theoretical rate ...: ideal  [ %8.2f ] %s / sec, [ %8u ] blocks/sec."
_IDEAL_RATE = re.compile(
    r'Theoretical rate[ .]*:\s*ideal\s*\[\s*([0-9.]+)\s*\]\s*(\w+)\s*/\s*sec')

# " Min:%9.3f  P50:%9.3f  P95:%9.3f  P99:%9.3f  Max:%9.3f  Mean:%9.3f  (us)"
_LATENCY = re.compile(
    r'Min:\s*([0-9.]+)\s+P50:\s*([0-9.]+)\s+P95:\s*([0-9.]+)\s+P99:\s*([0-9.]+)'
    r'\s+Max:\s*([0-9.]+)\s+Mean:\s*([0-9.]+)')

# The unit names come from areg::STR_ONE_BYTE and friends in CommonDefs.hpp, which are
# SINGULAR ("Byte", "KByte", "MByte", "GByte"). The plural and short spellings are accepted
# too so that a change there cannot silently reduce a GByte/sec reading to a Byte/sec one.
_UNITS = {}
for _index, _names in enumerate((('Byte', 'Bytes', 'B', 'byte', 'bytes'),
                                 ('KByte', 'KBytes', 'KB', 'Kb'),
                                 ('MByte', 'MBytes', 'MB', 'Mb'),
                                 ('GByte', 'GBytes', 'GB', 'Gb'),
                                 ('TByte', 'TBytes', 'TB'))):
    for _name in _names:
        _UNITS[_name] = 1024.0 ** _index


def _plain(data):
    """Strips the ANSI escapes and returns text, so the regexes see the printed characters."""
    if isinstance(data, bytes):
        data = _ANSI.sub(b'', data)
        return data.decode('utf-8', 'replace')
    return _ANSI.sub('', data)


def _as_bytes_per_sec(value, unit):
    return float(value) * _UNITS.get(unit, 1.0)


def human_rate(bytes_per_sec):
    """Formats a byte rate the way the examples do, so the two can be compared by eye."""
    step = 1024.0
    for unit in ('Bytes', 'KB', 'MB', 'GB', 'TB'):
        if bytes_per_sec < step or unit == 'TB':
            return '%.2f %s/sec' % (bytes_per_sec, unit)
        bytes_per_sec /= step
    return '%.2f TB/sec' % bytes_per_sec


def human_count(value):
    """Formats a message rate with thousands separators."""
    return '{:,}'.format(int(value)).replace(',', ' ')


def _measure(scenario, metric, unit, value, display):
    return {'scenario': scenario, 'metric': metric, 'unit': unit,
            'value': value, 'display': display}


def _select(logs, keyword):
    """
    Keeps only the console of the process that reports the measurement.

    Both processes of a scenario print similar looking lines -- in ping pong mode the
    30_publatency provider prints a live latency line of its own, and reading it multiplied
    the five reported runs into three hundred. Only one of the two is the measuring end.
    """
    picked = [(name, data) for name, data in logs if keyword in name]
    return picked if picked else logs


def _extract_datarate(logs, scenario):
    """Peak network and broadcast rate over the whole run, from the provider's console."""
    logs = _select(logs, 'service')
    out = []
    best_net_bytes, best_net_blocks, best_svc_bytes, best_ideal = 0.0, 0, 0.0, 0.0
    channels = set()
    for _, data in logs:
        text = _plain(data)
        for value, unit, blocks in _NET_RATE.findall(text):
            best_net_bytes = max(best_net_bytes, _as_bytes_per_sec(value, unit))
            best_net_blocks = max(best_net_blocks, int(blocks))
        for value, unit, _blocks in _SVC_RATE.findall(text):
            best_svc_bytes = max(best_svc_bytes, _as_bytes_per_sec(value, unit))
        for value, unit in _IDEAL_RATE.findall(text):
            best_ideal = max(best_ideal, _as_bytes_per_sec(value, unit))
        channels.update(int(c) for c in re.findall(r'Channels[ .]*:\s*(\d+) ch', text))

    if best_net_bytes > 0.0:
        out.append(_measure(scenario, 'peak network data rate', 'bytes/sec',
                            best_net_bytes, human_rate(best_net_bytes)))
    if best_svc_bytes > 0.0:
        out.append(_measure(scenario, 'peak broadcast data rate', 'bytes/sec',
                            best_svc_bytes, human_rate(best_svc_bytes)))
    if best_net_blocks > 0:
        out.append(_measure(scenario, 'peak message rate', 'blocks/sec',
                            float(best_net_blocks), human_count(best_net_blocks) + ' msg/sec'))
    if best_ideal > 0.0:
        # How close the measured rate came to what the configuration could produce. A run that
        # stops well short of it means the channel ramp did not go far enough.
        reached = (100.0 * best_net_bytes / best_ideal) if best_net_bytes > 0.0 else 0.0
        out.append(_measure(scenario, 'theoretical rate at peak', 'bytes/sec', best_ideal,
                            '%s (measured is %.0f%% of it)' % (human_rate(best_ideal), reached)))
    if channels:
        out.append(_measure(scenario, 'channels ramped to', 'count', float(max(channels)),
                            '%d channels' % max(channels)))
    return out


def _extract_latency(logs, scenario):
    """Every completed latency run of the consumer, in order, plus the median of their P50s."""
    logs = _select(logs, 'consumer')
    out = []
    runs = []
    for _, data in logs:
        runs.extend(_LATENCY.findall(_plain(data)))

    for index, (mn, p50, p95, p99, mx, mean) in enumerate(runs, start=1):
        out.append(_measure(scenario, 'run %d P50' % index, 'us', float(p50), '%s us' % p50))
        out.append(_measure(scenario, 'run %d P95' % index, 'us', float(p95), '%s us' % p95))
        out.append(_measure(scenario, 'run %d P99' % index, 'us', float(p99), '%s us' % p99))
        out.append(_measure(scenario, 'run %d min/max/mean' % index, 'us', float(mean),
                            'min %s / max %s / mean %s us' % (mn, mx, mean)))

    if runs:
        p50s = sorted(float(r[1]) for r in runs)
        median = p50s[len(p50s) // 2]
        out.append(_measure(scenario, 'median of the run P50s', 'us', median, '%.3f us' % median))
    return out


_EXTRACTORS = {
    'datarate': _extract_datarate,
    'latency': _extract_latency,
}


def extract(kind, scenario, logs):
    """
    Reads the measurements out of the captured console streams of one scenario.

    kind      'datarate' or 'latency', taken from the scenario definition.
    scenario  the scenario name, carried into every measurement.
    logs      list of (binary name, captured bytes) of that scenario.
    Returns a list of measurement dictionaries, empty when nothing was printed.
    """
    handler = _EXTRACTORS.get(kind)
    return handler(logs, scenario) if handler is not None else []


# ---------------------------------------------------------------------------
# Reporting.
# ---------------------------------------------------------------------------

def report(measures, out_dir, annotate=True):
    """
    Prints the measurements, writes them next to the run output, and publishes them as build
    server annotations so that they are readable in a browser without downloading an artifact.

    Writes 'benchmarks.csv' (machine readable) and 'benchmarks.md' (human readable).
    """
    if not measures:
        return

    lines = ['', '=' * 78, 'BENCHMARK RESULTS', '=' * 78]
    width = max(len(m['scenario']) + len(m['metric']) for m in measures) + 4
    for m in measures:
        label = '%s %s' % (m['scenario'], m['metric'])
        lines.append('  %-*s %s' % (width, label, m['display']))
    lines.append('=' * 78)
    lines.append('')
    print('\n'.join(lines))

    os.makedirs(out_dir, exist_ok=True)

    csv_path = os.path.join(out_dir, 'benchmarks.csv')
    with open(csv_path, 'w') as csv:
        csv.write('scenario;metric;value;unit;display\n')
        for m in measures:
            csv.write('%s;%s;%r;%s;%s\n'
                      % (m['scenario'], m['metric'], m['value'], m['unit'], m['display']))

    md_path = os.path.join(out_dir, 'benchmarks.md')
    with open(md_path, 'w') as md:
        md.write('# Benchmark results\n\n')
        md.write('| scenario | metric | result |\n|---|---|---|\n')
        for m in measures:
            md.write('| %s | %s | %s |\n' % (m['scenario'], m['metric'], m['display']))

    print('benchmarks written to %s and %s' % (csv_path, md_path))

    if annotate and os.environ.get('GITHUB_ACTIONS') == 'true':
        # One annotation per scenario keeps within the per-job annotation budget and still
        # puts every number on the summary page.
        by_scenario = {}
        for m in measures:
            by_scenario.setdefault(m['scenario'], []).append(
                '%s = %s' % (m['metric'], m['display']))
        for scenario, items in by_scenario.items():
            print('::notice title=benchmark-%s::%s' % (scenario, ' | '.join(items)))
