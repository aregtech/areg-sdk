#!/usr/bin/env python3
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""Runs a selection of ctest cases N times and counts the failures of each case.

    repeat-ctest.py <ctest build directory> --regex <ctest -R regex> [--repeat 30]
                    [--config Release] [--timeout 120] [--title name] [--summary file]

Every round is one ctest call with its own JUnit report. The result is a table of each
case with its failed, skipped and run counts, followed by the output of the first failure
of each case. '--summary' appends the table as Markdown, for GITHUB_STEP_SUMMARY.

Exits 1 when a case failed, a round produced no report, or no case ran.
"""

import argparse
import os
import subprocess
import sys
import tempfile
import time
import xml.etree.ElementTree as ET

OUTPUT_LINES = 40


def run_round(args, junit):
    """Runs one ctest round. Returns the ctest output and the seconds it took."""
    cmd = ['ctest', '--test-dir', args.build_dir, '-R', args.regex, '--timeout', str(args.timeout),
           '--no-tests=error', '--output-junit', junit]
    if args.config:
        cmd += ['-C', args.config]
    started = time.monotonic()
    proc = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, errors='replace')
    return proc.stdout, time.monotonic() - started


def read_junit(path):
    """Returns {case: (state, output)} with state 'passed', 'failed' or 'skipped', or None."""
    try:
        root = ET.parse(path).getroot()
    except (OSError, ET.ParseError):
        return None
    results = {}
    for case in root.iter('testcase'):
        name = case.get('name', '')
        if (case.find('failure') is not None) or (case.find('error') is not None):
            results[name] = ('failed', case.findtext('system-out') or '')
        elif (case.get('status') == 'notrun') or (case.find('skipped') is not None):
            results[name] = ('skipped', '')
        else:
            results[name] = ('passed', '')
    return results


def write_summary(path, title, rounds, counts, errors):
    lines = ['### %s: %d rounds' % (title, rounds), '',
             '| Case | Failed | Skipped | Run |', '|---|---|---|---|']
    for name in sorted(counts):
        c = counts[name]
        mark = ' **FAIL**' if c['failed'] else ''
        lines.append('| `%s` | %d%s | %d | %d |' % (name, c['failed'], mark, c['skipped'], c['run']))
    if errors:
        lines += ['', '%d round(s) produced no report.' % errors]
    with open(path, 'a', encoding='utf-8') as out:
        out.write('\n'.join(lines) + '\n\n')


def main():
    parser = argparse.ArgumentParser(description='Repeat ctest cases and count failures per case.')
    parser.add_argument('build_dir', help='ctest build directory')
    parser.add_argument('--regex', required=True, help='ctest -R regular expression')
    parser.add_argument('--repeat', type=int, default=30, help='rounds (default: 30)')
    parser.add_argument('--config', default=None, help='ctest -C configuration')
    parser.add_argument('--timeout', type=int, default=120, help='seconds per case (default: 120)')
    parser.add_argument('--title', default=None, help='name of the table (default: the regex)')
    parser.add_argument('--summary', default=None, help='Markdown file the table is appended to')
    args = parser.parse_args()
    title = args.title or args.regex

    counts = {}
    first_output = {}
    errors = 0
    with tempfile.TemporaryDirectory() as tmp:
        for rnd in range(1, args.repeat + 1):
            junit = os.path.join(tmp, 'round-%d.xml' % rnd)
            output, seconds = run_round(args, junit)
            results = read_junit(junit)
            if results is None:
                errors += 1
                print('round %d/%d: no report (%.1f s)' % (rnd, args.repeat, seconds))
                print('\n'.join(output.splitlines()[-OUTPUT_LINES:]))
                continue
            failed = []
            for name, (state, text) in results.items():
                c = counts.setdefault(name, {'failed': 0, 'skipped': 0, 'run': 0})
                c['run'] += 1
                if state == 'failed':
                    c['failed'] += 1
                    failed.append(name)
                    first_output.setdefault(name, (rnd, text))
                elif state == 'skipped':
                    c['skipped'] += 1
            print('round %d/%d: %d cases, %d failed (%.1f s)%s'
                  % (rnd, args.repeat, len(results), len(failed), seconds,
                     (': ' + ', '.join(sorted(failed))) if failed else ''), flush=True)

    print('\n%s: %d rounds' % (title, args.repeat))
    width = max([len(n) for n in counts] + [4])
    print('%-*s  %6s  %7s  %4s' % (width, 'case', 'failed', 'skipped', 'run'))
    for name in sorted(counts):
        c = counts[name]
        print('%-*s  %6d  %7d  %4d' % (width, name, c['failed'], c['skipped'], c['run']))
    for name in sorted(first_output):
        rnd, text = first_output[name]
        print('\n----- first failure of %s, round %d -----' % (name, rnd))
        print('\n'.join(text.splitlines()[-OUTPUT_LINES:]))

    if args.summary:
        write_summary(args.summary, title, args.repeat, counts, errors)

    failed_cases = sum(1 for c in counts.values() if c['failed'])
    if not counts:
        print('\nFAILED: no case matched %s' % args.regex)
        return 1
    if errors or failed_cases:
        print('\nFAILED: %d case(s) failed, %d round(s) without a report' % (failed_cases, errors))
        return 1
    print('\nPASSED')
    return 0


if __name__ == '__main__':
    sys.exit(main())
