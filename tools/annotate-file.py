#!/usr/bin/env python3
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""Publishes the tail of a text file as build server annotations.

TEMPORARY, bug-fixing phase. Delete together with the defects it was written for.

The job log and the artifacts of a build server need a credential to read; annotations
do not. A log written by an application that hung is exactly the evidence that has to
travel back, so its last lines are published as annotations as well as printed.

    annotate-file.py --title <name> [--tail <lines>] [--level notice] <file> [...]

A glob that matches nothing, an unreadable file and an empty file are all reported and
none of them is an error: this runs because something else already failed.
"""

import argparse
import glob
import os
import sys


ANNOTATION_CHARS = 3200     #!< characters of one annotation message that survive


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
        print('::warning title=%s::%d of %d parts are in the artifact only'
              % (title, len(chunks) - limit, len(chunks)))
    sys.stdout.flush()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('patterns', nargs='+', help='files or glob patterns')
    parser.add_argument('--title', default='log', help='the annotation title')
    parser.add_argument('--tail', type=int, default=120, help='lines kept per file')
    parser.add_argument('--level', default='notice',
                        choices=['notice', 'warning', 'error'])
    parser.add_argument('--max-annotations', type=int, default=4)
    parser.add_argument('--max-files', type=int, default=4)
    args = parser.parse_args()

    paths = []
    for pattern in args.patterns:
        paths.extend(sorted(glob.glob(pattern)))
    if not paths:
        print('::warning title=%s::nothing matches %s'
              % (args.title, ', '.join(args.patterns)))
        return 0

    for path in paths[:args.max_files]:
        try:
            with open(path, 'r', errors='replace') as source:
                lines = source.read().splitlines()
        except OSError as error:
            print('::warning title=%s::%s cannot be read: %s' % (args.title, path, error))
            continue
        if not lines:
            print('::warning title=%s::%s is empty' % (args.title, path))
            continue
        report = ('%s, last %d of %d line(s)\n'
                  % (os.path.basename(path), min(args.tail, len(lines)), len(lines))
                  + '\n'.join(lines[-args.tail:]))
        publish('%s %s' % (args.title, os.path.basename(path)),
                args.level, report, args.max_annotations)

    if len(paths) > args.max_files:
        print('::warning title=%s::%d further file(s) are in the artifact only'
              % (args.title, len(paths) - args.max_files))
    return 0


if __name__ == '__main__':
    sys.exit(main())
