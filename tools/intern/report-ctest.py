#!/usr/bin/env python3
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""Publishes the ctest cases that did not pass as build server annotations.

The job log of a build server needs a credential to read, annotations do not. So the
names of the cases that did not pass and the output they produced are published as
annotations, which makes a failed matrix entry readable in a browser.

    report-ctest.py <ctest build directory> [--title ctest]

Nothing here is allowed to fail the job: it runs because something else already failed
and its only purpose is to carry the evidence out. A missing file, an unreadable file
and a file without a single failure are all reported and none of them is an error.

Two sources, in this order:

1. 'test_results.xml', the JUnit report that the ctest command line already asks for.
   This is the authority. A case carries 'status="run"' plus a 'failure' element when it
   ran and failed, and 'status="notrun"' plus a 'skipped' element when it never ran.

2. 'Testing/Temporary/LastTest.log', when there is no JUnit report.

The order matters, because LastTest.log does not always tell the truth. A case that
ctest reported on its console as

    Test not available without configuration.  (Missing "-C <config>"?)
    515/515 Test #515: areg-release-unload-test ...***Not Run

is written into LastTest.log as a block ending in 'Test Passed.', with the reason on a
line of its own before the block starts. Reading the verdict alone would call that run
green while ctest exited 8. The JUnit report has it right: status="notrun" with
'Missing Configuration'. Observed with the Visual Studio generator, CMake 3.31.
"""

import argparse
import os
import re
import sys
import xml.etree.ElementTree as ElementTree


ANNOTATION_CHARS = 3200     #!< characters of one annotation message that survive

#!< Head of a block of 'LastTest.log'. The name is what the verdict below it belongs to.
_TEST_HEAD = re.compile(r'^\s*\d+/\d+\s+Test:\s*(?P<name>.+?)\s*$')
#!< Verdict of a block. 'Test Passed.' is the only one that is not a failure.
_TEST_VERDICT = re.compile(r'^\s*(?:Test\s+(?P<verdict>\w+)\.|\*\*\*(?P<crash>[^\r\n]+))\s*$')
#!< A reason ctest prints before the block, where no verdict of the block will show it.
_TEST_UNAVAILABLE = re.compile(r'^\s*Test not available without configuration\..*$')


def annotation_text(text):
    """Encodes a multi line report so that a build server keeps the line breaks."""
    return text.replace('%', '%25').replace('\r', '%0D').replace('\n', '%0A')


def publish(title, level, report, limit):
    """Prints a report and spends at most 'limit' annotations on it."""
    print(report)
    chunks = [report[at:at + ANNOTATION_CHARS]
              for at in range(0, len(report), ANNOTATION_CHARS)]
    shown = chunks[:limit]
    for index, chunk in enumerate(shown):
        print('::%s title=%s %d/%d::%s'
              % (level, title, index + 1, len(shown), annotation_text(chunk)))
    if len(chunks) > limit:
        print('::warning title=%s::%d of %d part(s) are in the artifact only'
              % (title, len(chunks) - limit, len(chunks)))
    sys.stdout.flush()


def read_junit(path):
    """Turns a ctest JUnit report into (name, verdict, output) of every case that failed.

    A case counts as failed when it carries a 'failure' or an 'error' element, or when it
    did not run at all. The last one is not pedantry: 'notrun' is how a configuration that
    ctest was not told about arrives, and it is enough to make ctest exit non-zero.
    """
    try:
        root = ElementTree.parse(path).getroot()
    except (OSError, ElementTree.ParseError) as error:
        return None, '%s cannot be read: %s' % (path, error)

    failures = []
    for case in root.iter('testcase'):
        name = case.attrib.get('name', '<unnamed>')
        status = case.attrib.get('status', 'run')
        problem = case.find('failure')
        if problem is None:
            problem = case.find('error')
        if problem is not None:
            verdict = problem.attrib.get('message') or problem.tag
        elif status != 'run':
            skipped = case.find('skipped')
            reason = skipped.attrib.get('message') if skipped is not None else None
            verdict = '%s%s' % (status, ': %s' % reason if reason else '')
        else:
            continue

        body = []
        for element in list(case) + ([problem] if problem is not None else []):
            if element.tag in ('system-out', 'system-err', 'failure', 'error') and element.text:
                body.extend(element.text.splitlines())
        failures.append((name, verdict, body))
    return failures, None


def read_last_test_log(lines):
    """Turns the lines of 'LastTest.log' into (name, verdict, output) of every failure."""
    failures = []
    name = None
    body = []
    pending = None      #!< a reason printed before the block it belongs to
    for line in lines:
        if _TEST_UNAVAILABLE.match(line):
            pending = line.strip()
            continue
        head = _TEST_HEAD.match(line)
        if head is not None:
            name = head.group('name')
            body = []
            continue
        if name is None:
            continue
        verdict = _TEST_VERDICT.match(line)
        if verdict is None:
            body.append(line)
            continue
        if verdict.group('crash') is not None:
            failures.append((name, verdict.group('crash').strip(), body))
        elif verdict.group('verdict') != 'Passed':
            failures.append((name, 'Test %s' % verdict.group('verdict'), body))
        elif pending is not None:
            # The block says it passed and ctest said it never ran. Believe ctest.
            failures.append((name, pending, body))
        name = None
        pending = None
    return failures


def locate(path, *tails):
    """Accepts the file itself, the ctest build directory or the directory that holds it."""
    if os.path.isfile(path):
        return path
    for tail in tails:
        candidate = os.path.join(path, tail)
        if os.path.isfile(candidate):
            return candidate
    return None


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('path', help='the ctest build directory, or one of its reports')
    parser.add_argument('--title', default='ctest', help='the annotation title')
    parser.add_argument('--tail', type=int, default=60,
                        help='lines kept of the output of one failed case')
    parser.add_argument('--max-cases', type=int, default=6,
                        help='failed cases that get an annotation of their own')
    parser.add_argument('--max-annotations', type=int, default=2,
                        help='annotations spent on the output of one failed case')
    args = parser.parse_args()

    failures = None
    source = locate(args.path, 'test_results.xml')
    if source is not None and source.endswith('.xml'):
        failures, complaint = read_junit(source)
        if complaint is not None:
            print('::warning title=%s::%s' % (args.title, complaint))

    log = locate(args.path, os.path.join('Testing', 'Temporary', 'LastTest.log'),
                 'LastTest.log')
    lines = []
    if log is not None:
        try:
            with open(log, 'r', errors='replace') as handle:
                lines = handle.read().splitlines()
        except OSError as error:
            print('::warning title=%s::%s cannot be read: %s' % (args.title, log, error))

    if failures is None:
        source = log
        failures = read_last_test_log(lines) if lines else None

    if failures is None:
        print('::warning title=%s::no test report under %s' % (args.title, args.path))
        return 0

    if not failures:
        # This runs because the job failed, so a report without a failed case means the
        # report is not the one that belongs to the failure -- a stale file of an earlier
        # run, or a ctest that died before it wrote one. Say so rather than stay silent,
        # and print the tail of the log so that the reason is still visible.
        print('::warning title=%s::%s names no failed case' % (args.title, source))
        if lines:
            publish('%s-tail' % args.title, 'notice',
                    '%s, last %d line(s)\n%s'
                    % (log, min(args.tail, len(lines)), '\n'.join(lines[-args.tail:])),
                    args.max_annotations)
        return 0

    names = '; '.join('%s (%s)' % (name, verdict) for name, verdict, _ in failures)
    print('::error title=%s::%d case(s) did not pass, from %s: %s'
          % (args.title, len(failures), os.path.basename(source), names))

    for name, verdict, body in failures[:args.max_cases]:
        report = ('%s -- %s\n%s' % (name, verdict, '\n'.join(body[-args.tail:]))).strip()
        publish('%s %s' % (args.title, name), 'notice', report, args.max_annotations)

    if len(failures) > args.max_cases:
        print('::warning title=%s::the output of %d further case(s) is in the artifact'
              ' only' % (args.title, len(failures) - args.max_cases))
    return 0


if __name__ == '__main__':
    sys.exit(main())
