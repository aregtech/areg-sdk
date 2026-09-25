#!/usr/bin/env python3
# -*- coding: ascii -*-
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""
Converts CRLF line endings to LF in the working tree.

The repository stores text with LF. An editor on Windows may save a file with CRLF, and
the file then shows every line as changed. This tool rewrites such files with LF.

A file is left as it is when:
  - '.gitattributes' gives it 'eol=crlf' ('.bat', '.ps1');
  - it is binary (contains a NUL byte, or '.gitattributes' marks it '-text');
  - it matches KEEP_AS_IS below.

Usage
-----
    python3 tools/fix-eol.py                    # files changed against HEAD, and untracked ones
    python3 tools/fix-eol.py --check            # report only; exit 1 when a file has CRLF
    python3 tools/fix-eol.py framework/areg     # these files or directories
    python3 tools/fix-eol.py --all              # every tracked file

Exit status is 0 when nothing is left with CRLF, and 1 when '--check' found a file or a
file could not be rewritten. Paths are relative to the repository root.
"""

import argparse
import fnmatch
import os
import subprocess
import sys

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Files kept byte for byte as they are mirrored from codegen. Glob patterns, relative to
# the repository root.
KEEP_AS_IS = (
    'tools/schema/*',
)

SKIPPED_DIRS = {
    '.git', 'build', 'product', 'thirdparty', 'node_modules',
    '__pycache__', '.vs', '.vscode', '.idea', 'out', 'bin', 'obj',
}
SKIPPED_DIR_PREFIXES = ('build-', 'build_')


def git_lines(*args):
    """Runs git in the repository root and returns its output as a list of lines."""
    out = subprocess.check_output(['git', '-C', REPO_ROOT] + list(args), text=True)
    return [line for line in out.splitlines() if line]


def changed_files():
    """Tracked files that differ from HEAD, and untracked files that are not ignored."""
    files = git_lines('diff', '--name-only', '--diff-filter=ACMR', 'HEAD')
    files += git_lines('ls-files', '--others', '--exclude-standard')
    return files


def walk(paths):
    """Expands files and directories into repository-relative file paths."""
    found = []
    for path in paths:
        full = os.path.abspath(os.path.join(REPO_ROOT, path)) if not os.path.isabs(path) else path
        if os.path.isfile(full):
            found.append(full)
            continue
        for root, dirs, files in os.walk(full):
            dirs[:] = [d for d in dirs if d not in SKIPPED_DIRS and not d.startswith(SKIPPED_DIR_PREFIXES)]
            found.extend(os.path.join(root, name) for name in files)
    return [os.path.relpath(f, REPO_ROOT).replace(os.sep, '/') for f in found]


def attributes(files):
    """Returns {path: (eol, text)} as git reads them from '.gitattributes'."""
    result = {path: ('unspecified', 'unspecified') for path in files}
    if not files:
        return result
    proc = subprocess.run(['git', '-C', REPO_ROOT, 'check-attr', '--stdin', 'eol', 'text'],
                          input='\n'.join(files) + '\n', capture_output=True, text=True)
    for line in proc.stdout.splitlines():
        path, attr, value = line.rsplit(': ', 2)
        eol, text = result.get(path, ('unspecified', 'unspecified'))
        result[path] = (value, text) if attr == 'eol' else (eol, value)
    return result


def is_kept(path, eol, text):
    """Tells whether the file must keep its line endings."""
    if eol == 'crlf' or text == 'unset':
        return True
    return any(fnmatch.fnmatch(path, pattern) for pattern in KEEP_AS_IS)


def main():
    parser = argparse.ArgumentParser(description='Converts CRLF line endings to LF.')
    parser.add_argument('paths', nargs='*', help='files or directories; default: files changed against HEAD')
    parser.add_argument('--all', action='store_true', help='every tracked file')
    parser.add_argument('--check', action='store_true', help='report only, change nothing')
    args = parser.parse_args()

    if args.paths:
        files = walk(args.paths)
    elif args.all:
        files = git_lines('ls-files')
    else:
        files = changed_files()

    attrs = attributes(files)
    found = 0
    failed = 0
    for path in sorted(set(files)):
        full = os.path.join(REPO_ROOT, path)
        if not os.path.isfile(full) or is_kept(path, *attrs.get(path, ('unspecified', 'unspecified'))):
            continue
        try:
            with open(full, 'rb') as src:
                data = src.read()
        except OSError as err:
            print('cannot read %s: %s' % (path, err), file=sys.stderr)
            failed += 1
            continue
        if b'\0' in data:
            continue
        count = data.count(b'\r\n')
        if count == 0:
            continue

        found += 1
        if args.check:
            print('CRLF  %s  (%d lines)' % (path, count))
            continue
        try:
            with open(full, 'r+b') as dst:
                dst.write(data.replace(b'\r\n', b'\n'))
                dst.truncate()
        except OSError as err:
            print('cannot write %s: %s' % (path, err), file=sys.stderr)
            failed += 1
            continue
        print('LF    %s  (%d lines)' % (path, count))

    verb = 'have CRLF' if args.check else 'converted to LF'
    print('%d file(s) checked, %d %s' % (len(set(files)), found, verb))
    if failed:
        return 1
    return 1 if args.check and found else 0


if __name__ == '__main__':
    sys.exit(main())
