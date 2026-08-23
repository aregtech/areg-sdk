#!/usr/bin/env python3
# -*- coding: ascii -*-
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""
Finds unwanted bytes in the sources of the SDK.

Two checks live here, and a run performs one of them.

1. The ASCII-only rule (default). Every source, script, CMake and configuration file
   must contain only bytes 0x00-0x7F. Documents ('.md', '.txt') are exempt, because
   they may carry UTF-8. The compilers of the SDK read a file without a byte order
   mark in the code page of the machine, so the same UTF-8 dash becomes different
   bytes on different machines, silently, inside a string literal.

2. A byte hunt ('--find'). Reports every occurrence of the given byte values, whatever
   they are. Written for the control characters that reach a file through a text
   pipeline: '\\brief' typed into a context that expands escapes becomes byte 0x08
   followed by 'rief', which no compiler and no reviewer sees.

Usage
-----
    python3 tools/check-ascii.py                     # ASCII rule, whole repository
    python3 tools/check-ascii.py framework examples  # ASCII rule, only these paths
    python3 tools/check-ascii.py --staged            # only what git has staged

    python3 tools/check-ascii.py --find control      # every C0 control but TAB, LF, CR
    python3 tools/check-ascii.py --find 0x08         # one value
    python3 tools/check-ascii.py --find 0x01-0x12    # a range
    python3 tools/check-ascii.py --find 0x08,0x1B,0x7F
    python3 tools/check-ascii.py --find control --docs   # documents included

Exit status is 0 when nothing was found and 1 when something was, so either check can
be used directly as a gate.

Paths are resolved relative to the repository root, which is found from this file's
location, so the tool works from any working directory and on any checkout.
"""

import argparse
import fnmatch
import os
import subprocess
import sys

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Extensions that must be pure ASCII.
CHECKED_SUFFIXES = (
    '.h', '.hpp', '.c', '.cpp', '.cc', '.cxx', '.inl',
    '.cmake', '.py', '.sh', '.bat', '.cmd', '.ps1',
    '.json', '.yml', '.yaml', '.xml', '.siml', '.fsml', '.qss', '.init', '.in',
)
CHECKED_NAMES = ('CMakeLists.txt',)

# Documents. Exempt from the ASCII rule, searched by '--find --docs'.
DOC_SUFFIXES = ('.md', '.txt')

# Directories that are never ours to police.
SKIPPED_DIRS = {
    '.git', '.github_cache', 'build', 'product', 'thirdparty', 'node_modules',
    '__pycache__', '.vs', '.vscode', '.idea', 'out', 'bin', 'obj',
}

# Files whose content is character data, so non ASCII bytes in them are the subject and
# not a defect. Glob patterns, matched against the path relative to the repository root.
# A waiver applies to the ASCII rule only. '--find' ignores this list, because a control
# character is unwanted in a character test as much as anywhere else.
ASCII_WAIVED = (
    'framework/areg/base/TableDefs.hpp',
    'tests/units/String*Test.cpp',
    'tests/units/*Encod*Test.cpp',
)

# Named byte sets accepted by '--find'.
CONTROL_BYTES = (set(range(0x00, 0x20)) - {0x09, 0x0A, 0x0D}) | {0x7F}
NON_ASCII_BYTES = set(range(0x80, 0x100))

NAMED_SETS = {
    'control': CONTROL_BYTES,
    'non-ascii': NON_ASCII_BYTES,
}

# How a found byte is spelled in the report, when it has a name worth printing.
BYTE_NAMES = {
    0x00: 'NUL', 0x07: 'BEL', 0x08: 'BS', 0x0B: 'VT', 0x0C: 'FF',
    0x1A: 'SUB', 0x1B: 'ESC', 0x7F: 'DEL',
}

CONTEXT_CHARS = 72      #!< characters of the offending line that are printed


def parse_byte_set(spec):
    """Turns '0x08', '0x01-0x12', '0x08,0x1B' or a named set into a set of byte values."""
    if spec in NAMED_SETS:
        return set(NAMED_SETS[spec])

    wanted = set()
    for part in spec.split(','):
        part = part.strip()
        if not part:
            continue
        if part in NAMED_SETS:
            wanted |= NAMED_SETS[part]
            continue
        bounds = [item.strip() for item in part.split('-') if item.strip()]
        if len(bounds) not in (1, 2):
            raise ValueError('cannot read "%s"' % part)
        try:
            values = [int(item, 16) for item in bounds]
        except ValueError:
            raise ValueError('"%s" is not a hexadecimal value' % part)
        for value in values:
            if not 0x00 <= value <= 0xFF:
                raise ValueError('0x%02X is outside 0x00-0xFF' % value)
        low, high = (values[0], values[-1])
        if low > high:
            raise ValueError('the range "%s" starts above it ends' % part)
        wanted |= set(range(low, high + 1))

    if not wanted:
        raise ValueError('no byte value given')
    return wanted


def spell(byte):
    """The report spelling of one byte value."""
    name = BYTE_NAMES.get(byte)
    return '0x%02X (%s)' % (byte, name) if name else '0x%02X' % byte


def render(raw, column):
    """The offending line, with every unprintable byte written out, cut around 'column'."""
    start = max(0, column - 1 - CONTEXT_CHARS // 3)
    text = []
    for index in range(start, min(len(raw), start + CONTEXT_CHARS)):
        byte = raw[index]
        text.append(chr(byte) if 0x20 <= byte <= 0x7E else '<0x%02X>' % byte)
    return ('...' if start > 0 else '') + ''.join(text)


def is_checked(path, docs):
    name = os.path.basename(path)
    if name in CHECKED_NAMES:
        return True
    if path.endswith(CHECKED_SUFFIXES):
        return True
    return docs and path.endswith(DOC_SUFFIXES)


def relative(path):
    """The path as the report shows it: relative to the root, absolute when outside it."""
    rel = os.path.relpath(path, REPO_ROOT).replace(os.sep, '/')
    return path.replace(os.sep, '/') if rel.startswith('../') else rel


def is_waived(rel):
    return any(fnmatch.fnmatch(rel, pattern) for pattern in ASCII_WAIVED)


def offending_bytes(path, wanted, limit=10):
    """Returns [(line, column, byte, context)] for the first 'limit' wanted bytes."""
    try:
        with open(path, 'rb') as src:
            data = src.read()
    except OSError as err:
        return [(0, 0, None, str(err))]

    found = []
    for number, raw in enumerate(data.split(b'\n'), 1):
        for column, byte in enumerate(raw, 1):
            if byte in wanted:
                found.append((number, column, byte, render(raw, column)))
                if len(found) >= limit:
                    return found
    return found


def collect(paths, staged, docs):
    if staged:
        try:
            out = subprocess.check_output(
                ['git', '-C', REPO_ROOT, 'diff', '--cached', '--name-only', '--diff-filter=ACMR'],
                text=True)
        except (OSError, subprocess.CalledProcessError) as err:
            print('cannot read the staged files: %s' % err, file=sys.stderr)
            return
        for name in out.splitlines():
            candidate = os.path.join(REPO_ROOT, name)
            if name and is_checked(name, docs) and os.path.isfile(candidate):
                yield candidate
        return

    roots = [os.path.join(REPO_ROOT, p) for p in paths] if paths else [REPO_ROOT]
    for root in roots:
        if os.path.isfile(root):
            if is_checked(root, docs):
                yield root
            continue
        for directory, subdirs, files in os.walk(root):
            subdirs[:] = [d for d in subdirs if d not in SKIPPED_DIRS]
            for name in files:
                full = os.path.join(directory, name)
                if is_checked(full, docs):
                    yield full


def main():
    parser = argparse.ArgumentParser(
        description='Finds non ASCII bytes, or the byte values given by --find, in the sources.')
    parser.add_argument('paths', nargs='*',
                        help='paths relative to the repository root; default is everything')
    parser.add_argument('--find', metavar='SPEC',
                        help='hunt these byte values instead of checking the ASCII rule: '
                             'a value (0x08), a range (0x01-0x12), a list (0x08,0x1B), '
                             'or a name ("control", "non-ascii")')
    parser.add_argument('--docs', action='store_true',
                        help='search the .md and .txt documents too; they are always exempt '
                             'from the ASCII rule, but not from --find')
    parser.add_argument('--staged', action='store_true',
                        help='check only the files staged in git, for a pre-commit hook')
    parser.add_argument('--quiet', action='store_true',
                        help='print only the offending files, no summary')
    parser.add_argument('--fix-report', metavar='FILE',
                        help='also write the findings to this file')
    args = parser.parse_args()

    hunting = args.find is not None
    if hunting:
        try:
            wanted = parse_byte_set(args.find)
        except ValueError as err:
            parser.error('--find: %s' % err)
    else:
        wanted = NON_ASCII_BYTES

    checked = 0
    bad = []
    for path in collect(args.paths, args.staged, args.docs):
        rel = relative(path)
        if not hunting:
            if rel.endswith(DOC_SUFFIXES) or is_waived(rel):
                continue
        checked += 1
        found = offending_bytes(path, wanted)
        if found:
            bad.append((rel, found))

    lines = []
    for rel, found in sorted(bad):
        for number, column, byte, context in found:
            if byte is None:
                lines.append('%s: %s' % (rel, context))
                continue
            lines.append('%s:%d:%d: %s' % (rel, number, column, spell(byte)))
            lines.append('    %s' % context)
        if len(found) >= 10:
            lines.append('%s: ... more occurrences follow' % rel)

    for line in lines:
        print(line)

    if args.fix_report:
        with open(args.fix_report, 'w') as report:
            report.write('\n'.join(lines) + ('\n' if lines else ''))

    if not args.quiet:
        if bad and hunting:
            print('')
            print('%d of %d searched file(s) contain the requested bytes.' % (len(bad), checked))
        elif bad:
            print('')
            print('%d of %d checked file(s) contain non ASCII bytes.' % (len(bad), checked))
            print('Replace them with ASCII: "(c)" not the copyright sign, "--" not an en dash,')
            print('"..." not an ellipsis, "->" not an arrow, and plain quotes.')
        elif hunting:
            print('%d file(s) searched, none contains the requested bytes.' % checked)
        else:
            print('%d file(s) checked, all ASCII.' % checked)

    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
