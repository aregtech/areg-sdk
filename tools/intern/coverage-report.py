#!/usr/bin/env python3
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""Writes the code coverage of framework/ as a Markdown report.

    coverage-report.py --unit U.json --examples E.json --total T.json [--summary FILE]

Each input is a gcovr '--json-summary' file: the unit tests, the example applications,
and the union of both. The report has three tables:

1. the totals of lines, functions and branches, per run and combined;
2. the combined coverage per project, like 'areg' or 'mtrouter';
3. the combined coverage per directory of a project, like 'areg/ipc'.

Tables 2 and 3 also give the line coverage of each run. The report is appended to
'--summary' (the job summary of a build server) and printed.
"""

import argparse
import json
import sys


METRICS = (('line', 'Lines'), ('function', 'Functions'), ('branch', 'Branches'))


def load(path):
    """Returns the parsed gcovr summary, or None when the file is missing or unreadable."""
    try:
        with open(path, 'r', encoding='utf-8') as file:
            return json.load(file)
    except (OSError, ValueError) as error:
        print('cannot read %s: %s' % (path, error), file=sys.stderr)
        return None


def percent(covered, total):
    """Returns the coverage as text, or a dash when there is nothing to cover."""
    if total <= 0:
        return '-'
    return '%.1f%%' % (100.0 * covered / total)


def cell(covered, total):
    """Returns the coverage with its counts, like '61.2% (1200 / 1960)'."""
    if total <= 0:
        return '-'
    return '%s (%d / %d)' % (percent(covered, total), covered, total)


def group_of(filename, depth):
    """Returns the first 'depth' directories of a file below framework/, like 'areg/ipc'."""
    parts = filename.replace('\\', '/').split('/')
    if 'framework' in parts:
        parts = parts[parts.index('framework') + 1:]
    return '/'.join(parts[:min(depth, len(parts) - 1)] or parts[:1])


def by_group(summary, depth):
    """Returns {group: {metric_total: n, metric_covered: n}} summed over the files."""
    groups = {}
    for entry in (summary or {}).get('files', []):
        sums = groups.setdefault(group_of(entry['filename'], depth), {})
        for metric, _ in METRICS:
            for key in (metric + '_total', metric + '_covered'):
                sums[key] = sums.get(key, 0) + entry.get(key, 0)
    return groups


def totals_table(runs):
    """Returns the table of totals, one row per run."""
    lines = ['| Measured by | ' + ' | '.join(title for _, title in METRICS) + ' |',
             '|---|' + '---:|' * len(METRICS)]
    for name, summary in runs:
        if summary is None:
            lines.append('| %s | ' % name + ' | '.join('no data' for _ in METRICS) + ' |')
            continue
        row = [cell(summary.get(m + '_covered', 0), summary.get(m + '_total', 0)) for m, _ in METRICS]
        lines.append('| %s | %s |' % (name, ' | '.join(row)))
    return lines


def groups_table(title, depth, total, unit, examples):
    """Returns the table of the combined coverage per group of the given depth."""
    combined = by_group(total, depth)
    unit_groups = by_group(unit, depth)
    examples_groups = by_group(examples, depth)
    lines = ['| %s | ' % title + ' | '.join(name for _, name in METRICS) + ' | Lines, unit tests | Lines, examples |',
             '|---|' + '---:|' * (len(METRICS) + 2)]
    for name in sorted(combined):
        sums = combined[name]
        row = [cell(sums[m + '_covered'], sums[m + '_total']) for m, _ in METRICS]
        for source in (unit_groups, examples_groups):
            row.append(percent(source.get(name, {}).get('line_covered', 0), sums['line_total']))
        lines.append('| `%s` | %s |' % (name, ' | '.join(row)))
    return lines


def main():
    parser = argparse.ArgumentParser(description='Writes the framework coverage as Markdown.')
    parser.add_argument('--unit', required=True, help='gcovr JSON summary of the unit tests')
    parser.add_argument('--examples', required=True, help='gcovr JSON summary of the examples')
    parser.add_argument('--total', required=True, help='gcovr JSON summary of both runs')
    parser.add_argument('--summary', default=None, help='file the report is appended to')
    args = parser.parse_args()

    unit = load(args.unit)
    examples = load(args.examples)
    total = load(args.total)
    if total is None:
        return 1

    report = ['## Code coverage of `framework/`', '',
              'GCC `--coverage`, Debug build, Linux. Code built only on other platforms is not counted.', '']
    report += totals_table((('unit tests', unit), ('examples', examples), ('**combined**', total)))
    report += ['', '### Per project', '']
    report += groups_table('Project', 1, total, unit, examples)
    report += ['', '### Per directory', '']
    report += groups_table('Directory', 2, total, unit, examples)
    report += ['', 'Per file and line: the artifact `coverage-report`, `html/index.html`.', '']
    text = '\n'.join(report)

    print(text)
    if args.summary:
        try:
            with open(args.summary, 'a', encoding='utf-8') as file:
                file.write(text + '\n')
        except OSError as error:
            print('cannot write %s: %s' % (args.summary, error), file=sys.stderr)
    return 0


if __name__ == '__main__':
    sys.exit(main())
