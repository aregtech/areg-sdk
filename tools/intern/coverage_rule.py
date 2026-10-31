#!/usr/bin/env python3
# -*- coding: ascii -*-
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""Counts the line coverage of a Cobertura report the way Codecov counts it.

    coverage_rule.py TOTAL.xml [--root DIR] [--last LAST.json] [--save] [--files REGEX] [--top N]
                               [--summary FILE]

Every source line is one of three states:

- miss:    the line has 0 hits;
- partial: the line is hit and one of its branches is not taken;
- hit:     the line is hit and all its branches, if any, are taken.

The coverage is hits / lines, so a partial line counts as not covered. Codecov does not
count a line that holds only '}', so the sources under '--root' are read to drop them.
The input should be written by 'gcovr --merge-lines', which reports a line once whatever
the number of template instances or constructor variants; lines repeated in the input
are merged here as well.

The report has the total, one row per project ('areg'), one per directory ('areg/base'),
the files matching '--files', and the '--top' files with the most lines not hit. With
'--last', each row also shows the change against that earlier run; '--save' then writes
the current counts to it. The report is printed and appended to '--summary'.

'coverage-report.py' imports 'read_cobertura', 'report', 'load_last' and 'save_last'.
"""

import argparse
import json
import os
import re
import sys
import xml.etree.ElementTree as ElementTree

LINES, HITS, PARTIALS, MISSES = range(4)
CONDITION = re.compile(r'\((\d+)/(\d+)\)')


def closing_braces(root, filename):
    """Returns the numbers of the lines holding only '}' in a source file."""
    try:
        with open(os.path.join(root, filename), 'r', encoding='utf-8', errors='replace') as file:
            return {number for number, text in enumerate(file, 1) if text.strip() == '}'}
    except OSError:
        return set()


def read_cobertura(path, root=None):
    """Returns {file: [lines, hits, partials, misses]} of a Cobertura XML report.

    With 'root', the source tree, a line holding only '}' is not counted, as Codecov does.
    """
    per_line = {}
    filename = None
    in_methods = False
    for event, element in ElementTree.iterparse(path, events=('start', 'end')):
        if event == 'start':
            if element.tag == 'class':
                filename = element.get('filename', '').replace('\\', '/')
            elif element.tag == 'methods':
                in_methods = True
            continue
        if element.tag == 'methods':
            in_methods = False
        elif element.tag == 'line' and filename is not None and not in_methods:
            key = (filename, int(element.get('number', '0')))
            hits = int(element.get('hits', '0'))
            covered = total = 0
            if element.get('branch') == 'true':
                match = CONDITION.search(element.get('condition-coverage', ''))
                if match:
                    covered, total = int(match.group(1)), int(match.group(2))
            known = per_line.get(key)
            if known is None:
                per_line[key] = [hits, covered, total]
            else:
                known[0] += hits
                known[1] += covered
                known[2] += total
        elif element.tag == 'class':
            filename = None
            element.clear()

    braces = {}
    files = {}
    for (name, number), (hits, covered, total) in per_line.items():
        if root is not None:
            if name not in braces:
                braces[name] = closing_braces(root, name)
            if number in braces[name]:
                continue
        counts = files.setdefault(name, [0, 0, 0, 0])
        counts[LINES] += 1
        if hits == 0:
            counts[MISSES] += 1
        elif covered < total:
            counts[PARTIALS] += 1
        else:
            counts[HITS] += 1
    return files


def group_of(filename, depth):
    """Returns the project (depth 1) or directory (depth 2) of a 'framework/' file."""
    parts = filename.split('/')
    if parts and parts[0] == 'framework':
        parts = parts[1:]
    parts = parts[:-1]
    return '/'.join(parts[:depth]) if parts else '.'


def aggregate(files, depth):
    """Returns {group: counts} summed over the files; depth 0 gives the single group ''."""
    groups = {}
    for name, counts in files.items():
        key = group_of(name, depth) if depth > 0 else ''
        total = groups.setdefault(key, [0, 0, 0, 0])
        for index in range(4):
            total[index] += counts[index]
    return groups


def percent(counts):
    """Returns the Codecov-rule coverage in percent."""
    return 100.0 * counts[HITS] / counts[LINES] if counts[LINES] else 0.0


def delta(counts, before):
    """Returns the change of coverage and of non-hit lines against an earlier run."""
    if before is None:
        return 'new'
    change = percent(counts) - percent(before)
    nonhit = (counts[LINES] - counts[HITS]) - (before[LINES] - before[HITS])
    return '%+.2f%% (%+d non-hit)' % (change, nonhit)


def table(title, rows, last_rows):
    """Returns a Markdown table of the given (name, counts) rows."""
    header = '| %s | Lines | Hits | Partial | Miss | Cover |' % title
    align = '|---|---:|---:|---:|---:|---:|'
    if last_rows is not None:
        header += ' Change |'
        align += '---:|'
    text = [header, align]
    for name, counts in rows:
        row = '| `%s` | %d | %d | %d | %d | %.2f%% |' % (
            name, counts[LINES], counts[HITS], counts[PARTIALS], counts[MISSES], percent(counts))
        if last_rows is not None:
            row += ' %s |' % delta(counts, last_rows.get(name))
        text.append(row)
    return '\n'.join(text)


def load_last(path):
    """Returns the file counts saved by an earlier run, or None."""
    if not path or not os.path.isfile(path):
        return None
    try:
        with open(path, 'r', encoding='utf-8') as file:
            return json.load(file).get('files', {})
    except (OSError, ValueError) as error:
        print('cannot read %s: %s' % (path, error), file=sys.stderr)
        return None


def report(files, last, pattern, top, heading='## Coverage, Codecov rule (partial = not covered)', runs=()):
    """Returns the whole Markdown report.

    'runs' is a sequence of (name, files) of the runs that make up 'files'; each gets a row
    of the 'Run' table placed before the total.
    """
    sections = [heading]
    if runs:
        rows = [(name, aggregate(run, 0).get('', [0, 0, 0, 0])) for name, run in runs]
        sections.append(table('Run', rows, None))
    for title, depth in (('Total', 0), ('Project', 1), ('Directory', 2)):
        now = aggregate(files, depth)
        before = aggregate(last, depth) if last is not None else None
        if depth == 0:
            now = {'framework': now.get('', [0, 0, 0, 0])}
            if before is not None:
                before = {'framework': before.get('', [0, 0, 0, 0])}
        sections.append(table(title, sorted(now.items()), before))

    if pattern:
        expression = re.compile(pattern)
        chosen = sorted((name, counts) for name, counts in files.items() if expression.search(name))
        sections.append(table('File matching `%s`' % pattern, chosen, last))

    if top > 0:
        worst = sorted(files.items(), key=lambda item: (item[1][HITS] - item[1][LINES], item[0]))
        sections.append(table('Top %d files by non-hit lines' % top, worst[:top], last))
    return '\n\n'.join(sections) + '\n'


def save_last(path, source, files):
    """Writes the counts of a run to 'path'; returns False when the file cannot be written."""
    try:
        with open(path, 'w', encoding='utf-8') as file:
            json.dump({'source': os.path.abspath(source), 'files': files}, file, indent=0, sort_keys=True)
    except OSError as error:
        print('cannot write %s: %s' % (path, error), file=sys.stderr)
        return False
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n', 1)[0])
    parser.add_argument('cobertura', help="Cobertura XML written by 'gcovr --merge-lines'")
    parser.add_argument('--root', default='.', help="source tree the report's file names are relative to")
    parser.add_argument('--last', default=None, help='counts of an earlier run, to compare with')
    parser.add_argument('--save', action='store_true', help="write the counts to '--last'")
    parser.add_argument('--files', default=None, help='regular expression of files to list')
    parser.add_argument('--top', type=int, default=25, help='number of files listed by non-hit lines')
    parser.add_argument('--summary', default=None, help='file the report is appended to')
    args = parser.parse_args()

    try:
        files = read_cobertura(args.cobertura, args.root)
    except (OSError, ElementTree.ParseError) as error:
        print('cannot read %s: %s' % (args.cobertura, error), file=sys.stderr)
        return 1
    if not files:
        print('no source lines in %s' % args.cobertura, file=sys.stderr)
        return 1

    text = report(files, load_last(args.last), args.files, args.top)
    print(text)
    if args.summary:
        try:
            with open(args.summary, 'a', encoding='utf-8') as file:
                file.write(text + '\n')
        except OSError as error:
            print('cannot write %s: %s' % (args.summary, error), file=sys.stderr)

    if args.save and args.last and not save_last(args.last, args.cobertura, files):
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
