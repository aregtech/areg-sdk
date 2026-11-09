#!/usr/bin/env python3
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""Reports the edge cases of the critical algorithms against the last ctest run.

    check-edge-cases.py --junit FILE [--register FILE] [--text FILE]

The register (default: tests/edge-cases.json) lists each critical algorithm, its edge cases,
and the ctest tests that prove each case, as regular expressions matched against the whole
test name. The names and results are read from the JUnit file of the run
('ctest --output-junit', written by 'coverage.sh unit' as unit-junit.xml).

A case is proven when its patterns match at least one test and every matched test passed.
A case without patterns is a known gap. A pattern that matches no test is stale.
Exit code 1 when a matched test failed or a pattern is stale, else 0.
"""

import argparse
import json
import os
import re
import sys
import xml.etree.ElementTree as ElementTree

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def read_results(path):
    """Returns {test name: passed} of a ctest JUnit file, or None when it cannot be read."""
    try:
        tree = ElementTree.parse(path)
    except (OSError, ElementTree.ParseError) as error:
        print('cannot read %s: %s' % (path, error), file=sys.stderr)
        return None
    results = {}
    for case in tree.iter('testcase'):
        bad = (case.find('failure') is not None) or (case.find('error') is not None) or (case.find('skipped') is not None)
        results[case.get('name', '')] = (case.get('status', 'run') == 'run') and not bad
    return results


def main():
    parser = argparse.ArgumentParser(description='Reports the edge cases of the critical algorithms.')
    parser.add_argument('--junit', required=True, help="JUnit file of the ctest run ('ctest --output-junit')")
    parser.add_argument('--register', default=os.path.join(ROOT, 'tests', 'edge-cases.json'), help='the register')
    parser.add_argument('--text', default=None, help='file the report is written to')
    args = parser.parse_args()

    with open(args.register, 'r', encoding='utf-8') as file:
        register = json.load(file)
    results = read_results(args.junit)
    if not results:
        return 1
    names = sorted(results)

    rows = ['| Algorithm | Cases | Proven | Failed | No test |', '|---|---:|---:|---:|---:|']
    details = []
    totals = [0, 0, 0, 0]
    errors = 0
    for algorithm in register['algorithms']:
        counts = [0, 0, 0, 0]
        for entry in algorithm['cases']:
            counts[0] += 1
            matched = set()
            for pattern in entry['tests']:
                found = [name for name in names if re.fullmatch(pattern, name)]
                if not found:
                    details.append('- stale pattern `%s` (%s: %s)' % (pattern, algorithm['name'], entry['case']))
                    errors += 1
                matched.update(found)
            bad = sorted(name for name in matched if not results[name])
            if not entry['tests']:
                counts[3] += 1
                details.append('- no test: %s: %s' % (algorithm['name'], entry['case']))
            elif bad:
                counts[2] += 1
                errors += 1
                details.append('- failed: %s: %s (%s)' % (algorithm['name'], entry['case'], ', '.join(bad)))
            elif matched:
                counts[1] += 1
        totals = [a + b for a, b in zip(totals, counts)]
        rows.append('| %s | %d | %d | %d | %d |' % (algorithm['name'], *counts))
    rows.append('| **total** | %d | %d | %d | %d |' % tuple(totals))

    text = '\n'.join(['### Edge cases of the critical algorithms', '',
                      'A case is proven when every test that proves it passed in the run. '
                      'The register is `tests/edge-cases.json`.', ''] + rows + [''] + details) + '\n'
    print(text)
    if args.text:
        with open(args.text, 'w', encoding='utf-8') as file:
            file.write(text)
    return 1 if errors else 0


if __name__ == '__main__':
    sys.exit(main())
