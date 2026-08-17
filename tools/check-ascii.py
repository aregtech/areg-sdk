#!/usr/bin/env python3
# -*- coding: ascii -*-
"""
Verifies the ASCII-only rule of the coding standard.

Every source, script, CMake and configuration file of the SDK must contain only bytes
0x00-0x7F. Documents ('.md', '.txt') are exempt, because they are allowed to carry UTF-8.
The rule exists because the sources are compiled by six toolchains on three platforms with
different default code pages, and a stray en-dash or non breaking space in a comment has
already broken a build more than once.

The check is cheap enough to run before every commit and in CI.

Usage
-----
    python3 tools/check-ascii.py                     # the whole repository
    python3 tools/check-ascii.py framework examples  # only these paths
    python3 tools/check-ascii.py --staged            # only what git has staged
    python3 tools/check-ascii.py --fix-report out.txt

Exit status is 0 when everything is clean and 1 when at least one file is not, so it can be
used directly as a gate.

Paths are resolved relative to the repository root, which is found from this file's location,
so the tool works from any working directory and on any checkout.
"""

import argparse
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

# Directories that are never ours to police.
SKIPPED_DIRS = {
    '.git', '.github_cache', 'build', 'product', 'thirdparty', 'node_modules',
    '__pycache__', '.vs', '.vscode', '.idea', 'out', 'bin', 'obj',
}

# Known, accepted exceptions. Keep this list short and justified.
ALLOWED = {
    # Documented in CLAUDE.md: the only source file allowed to carry non ASCII comments.
    'framework/areg/base/TableDefs.hpp',
}


def is_checked(path):
    name = os.path.basename(path)
    if name in CHECKED_NAMES:
        return True
    return path.endswith(CHECKED_SUFFIXES)


def relative(path):
    return os.path.relpath(path, REPO_ROOT).replace(os.sep, '/')


def offending_bytes(path, limit=10):
    """Returns [(line, column, byte)] for the first `limit` non ASCII bytes of the file."""
    try:
        with open(path, 'rb') as src:
            data = src.read()
    except OSError as err:
        return [(0, 0, str(err))]

    found = []
    line, column = 1, 1
    for byte in data:
        if byte == 0x0A:
            line += 1
            column = 1
            continue
        if byte > 0x7F:
            found.append((line, column, '0x%02X' % byte))
            if len(found) >= limit:
                break
        column += 1
    return found


def collect(paths, staged):
    if staged:
        try:
            out = subprocess.check_output(
                ['git', '-C', REPO_ROOT, 'diff', '--cached', '--name-only', '--diff-filter=ACMR'],
                text=True)
        except (OSError, subprocess.CalledProcessError) as err:
            print('cannot read the staged files: %s' % err, file=sys.stderr)
            return []
        for name in out.splitlines():
            candidate = os.path.join(REPO_ROOT, name)
            if name and is_checked(name) and os.path.isfile(candidate):
                yield candidate
        return

    roots = [os.path.join(REPO_ROOT, p) for p in paths] if paths else [REPO_ROOT]
    for root in roots:
        if os.path.isfile(root):
            if is_checked(root):
                yield root
            continue
        for directory, subdirs, files in os.walk(root):
            subdirs[:] = [d for d in subdirs if d not in SKIPPED_DIRS]
            for name in files:
                full = os.path.join(directory, name)
                if is_checked(full):
                    yield full


def main():
    parser = argparse.ArgumentParser(description='Checks that the sources are ASCII only.')
    parser.add_argument('paths', nargs='*',
                        help='paths relative to the repository root; default is everything')
    parser.add_argument('--staged', action='store_true',
                        help='check only the files staged in git, for a pre-commit hook')
    parser.add_argument('--quiet', action='store_true',
                        help='print only the offending files, no summary')
    parser.add_argument('--fix-report', metavar='FILE',
                        help='also write the findings to this file')
    args = parser.parse_args()

    checked = 0
    bad = []
    for path in collect(args.paths, args.staged):
        rel = relative(path)
        if rel in ALLOWED:
            continue
        checked += 1
        found = offending_bytes(path)
        if found:
            bad.append((rel, found))

    lines = []
    for rel, found in sorted(bad):
        for line, column, value in found:
            lines.append('%s:%d:%d: non ASCII byte %s' % (rel, line, column, value))
        if len(found) >= 10:
            lines.append('%s: ... more non ASCII bytes follow' % rel)

    for line in lines:
        print(line)

    if args.fix_report:
        with open(args.fix_report, 'w') as report:
            report.write('\n'.join(lines) + ('\n' if lines else ''))

    if not args.quiet:
        if bad:
            print('')
            print('%d of %d checked file(s) contain non ASCII bytes.' % (len(bad), checked))
            print('Replace them with ASCII: "(c)" not the copyright sign, "--" not an en dash,')
            print('"..." not an ellipsis, "->" not an arrow, and plain quotes.')
        else:
            print('%d file(s) checked, all ASCII.' % checked)

    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
