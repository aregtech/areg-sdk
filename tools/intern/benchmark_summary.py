#!/usr/bin/env python3
# -*- coding: ascii -*-
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""
Writes the results of a Google Benchmark run as a Markdown table.

The input is the file written by 'areg-benchmarks --benchmark_out=<file>
--benchmark_out_format=json'. The table lists every benchmark with its real time,
CPU time and iteration count, followed by the context of the run (host, CPU count,
build type). The table is a report only: the script never compares the numbers
with a baseline and exits 0 whatever they are.

Usage
-----
    python3 tools/intern/benchmark_summary.py results.json             # print the table
    python3 tools/intern/benchmark_summary.py results.json summary.md  # also append it to a file

On GitHub Actions pass "$GITHUB_STEP_SUMMARY" as the second argument to show the
table on the page of the run.
"""

import json
import sys


def _format_time(value, unit):
    return '{:,.1f} {}'.format(value, unit)


def make_table(data):
    """Returns the Markdown text for the parsed benchmark results."""
    context = data.get('context', {})
    lines = []
    lines.append('## Benchmark results')
    lines.append('')
    lines.append('Measured on a shared build server: use the numbers as a report, not as a baseline.')
    lines.append('')
    lines.append('| Benchmark | Real time | CPU time | Iterations |')
    lines.append('| --------- | --------: | -------: | ---------: |')

    errors = 0
    for bench in data.get('benchmarks', []):
        name = bench.get('name', '?')
        if bench.get('error_occurred', False):
            errors += 1
            lines.append('| `{}` | error: {} | | |'.format(name, bench.get('error_message', '')))
            continue

        unit = bench.get('time_unit', 'ns')
        lines.append('| `{}` | {} | {} | {:,} |'.format(
            name,
            _format_time(bench.get('real_time', 0.0), unit),
            _format_time(bench.get('cpu_time', 0.0), unit),
            int(bench.get('iterations', 0))))

    lines.append('')
    lines.append('Host `{}`, {} CPU(s) at {} MHz, library build `{}`, {} benchmark(s), {} error(s).'.format(
        context.get('host_name', '?'),
        context.get('num_cpus', '?'),
        context.get('mhz_per_cpu', '?'),
        context.get('library_build_type', '?'),
        len(data.get('benchmarks', [])),
        errors))
    lines.append('')
    return '\n'.join(lines)


def main(argv):
    if len(argv) < 2:
        print('usage: benchmark_summary.py <results.json> [<summary.md>]')
        return 2

    with open(argv[1], 'r', encoding='utf-8') as source:
        table = make_table(json.load(source))

    print(table)
    if len(argv) > 2:
        with open(argv[2], 'a', encoding='utf-8') as target:
            target.write(table)
            target.write('\n')

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
