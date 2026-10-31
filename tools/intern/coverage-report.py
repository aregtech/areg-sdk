#!/usr/bin/env python3
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""Writes the code coverage of framework/ as a Markdown report.

    coverage-report.py --unit U.json --examples E.json --total T.json --cobertura T.xml
                       [--unit-cobertura U.xml] [--examples-cobertura E.xml] [--root DIR]
                       [--last LAST.json] [--save] [--files REGEX] [--top N] [--summary FILE]

The report has two parts.

1. The Codecov rule, the number of the badge: a line counts once whatever the number of
   template instances or constructor variants, and a hit line with an untaken branch is
   partial and not covered. The tables give the total, one row per run, and the combined
   coverage per project and per directory. The input is a Cobertura report written by
   'gcovr --merge-lines' (see coverage_rule.py); '--last' adds the change against an
   earlier run and '--save' replaces it.
2. The gcovr summary: lines, functions and branches, per run and combined, per project
   and per directory. Each input is a gcovr '--json-summary' file: the unit tests, the
   example applications, and the union of both.

The report is appended to '--summary' (the job summary of a build server) and printed.
"""

import argparse
import json
import os
import sys
import xml.etree.ElementTree as ElementTree

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import coverage_rule  # noqa: E402


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


def rule_part(args):
    """Returns the Markdown of the Codecov-rule tables, or None when the Cobertura report is unusable."""
    try:
        total = coverage_rule.read_cobertura(args.cobertura, args.root)
        runs = []
        for name, path in (('unit tests', args.unit_cobertura), ('examples', args.examples_cobertura)):
            if path:
                runs.append((name, coverage_rule.read_cobertura(path, args.root)))
    except (OSError, ElementTree.ParseError) as error:
        print('cannot read the Cobertura report: %s' % error, file=sys.stderr)
        return None, None
    if not total:
        print('no source lines in %s' % args.cobertura, file=sys.stderr)
        return None, None
    last = coverage_rule.load_last(args.last)
    text = coverage_rule.report(total, last, args.files, args.top,
                                heading='### Codecov rule (a partial line is not covered)', runs=runs)
    return text.rstrip('\n').split('\n'), total


def main():
    parser = argparse.ArgumentParser(description='Writes the framework coverage as Markdown.')
    parser.add_argument('--unit', required=True, help='gcovr JSON summary of the unit tests')
    parser.add_argument('--examples', required=True, help='gcovr JSON summary of the examples')
    parser.add_argument('--total', required=True, help='gcovr JSON summary of both runs')
    parser.add_argument('--cobertura', required=True,
                        help="Cobertura XML of both runs, written by 'gcovr --merge-lines'")
    parser.add_argument('--unit-cobertura', default=None, help="Cobertura XML of the unit tests, merged lines")
    parser.add_argument('--examples-cobertura', default=None, help="Cobertura XML of the examples, merged lines")
    parser.add_argument('--root', default='.', help="source tree the Cobertura file names are relative to")
    parser.add_argument('--last', default=None, help='counts of an earlier run, to compare with')
    parser.add_argument('--save', action='store_true', help="write the counts to '--last'")
    parser.add_argument('--files', default=None, help='regular expression of files to list')
    parser.add_argument('--top', type=int, default=0, help='number of files listed by non-hit lines')
    parser.add_argument('--summary', default=None, help='file the report is appended to')
    args = parser.parse_args()

    unit = load(args.unit)
    examples = load(args.examples)
    total = load(args.total)
    if total is None:
        return 1
    rule, counts = rule_part(args)
    if rule is None:
        return 1

    report = ['## Code coverage of `framework/`', '',
              'GCC `--coverage`, Debug build, Linux. Code built only on other platforms is not counted.', '']
    report += ['The first part counts like the Codecov badge: a line counts once whatever the number of '
               'template instances or constructor variants, a line holding only `}` is not counted, '
               'and a hit line with an untaken branch is partial, which is not covered.', '']
    report += rule
    report += ['', '### Lines, functions and branches (gcovr)', '',
               'Each template instance and constructor variant counts on its own, and a line is covered '
               'when it is executed once, so these numbers are higher than the badge.', '']
    report += totals_table((('unit tests', unit), ('examples', examples), ('**combined**', total)))
    report += ['', '#### Per project', '']
    report += groups_table('Project', 1, total, unit, examples)
    report += ['', '#### Per directory', '']
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
    if args.save and args.last and not coverage_rule.save_last(args.last, args.cobertura, counts):
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
