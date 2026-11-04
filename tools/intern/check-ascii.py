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

1. The encoding rule (default). Every text file of the repository is one of these:
     ASCII        everything that is not named below; only bytes 0x00-0x7F.
     UTF-8        documents ('.md', '.txt') and the character data of ASCII_WAIVED,
                  which is every '*String*.*' file. No byte order mark, except
                  in a C or C++ source: there the mark is mandatory, because the
                  compilers of the SDK read a file without one in the code page
                  of the machine, so the same UTF-8 dash becomes different bytes
                  on different machines, silently, inside a string literal.
     ASCII + BOM  the project files Visual Studio writes (BOM_ALLOWED). It puts the
                  mark back on every save, so the mark is accepted, not required.
   UTF-16, UTF-32 and bytes that are not UTF-8 are never accepted. Visual Studio saves
   a resource script as UTF-16, and clang cannot read that. '--fix-encoding' rewrites
   a file that breaks the rule; a non ASCII byte in an ASCII file stays for a person.

2. A byte hunt ('--find'). Reports every occurrence of the given byte values, whatever
   they are. Written for the control characters that reach a file through a text
   pipeline: '\\brief' typed into a context that expands escapes becomes byte 0x08
   followed by 'rief', which no compiler and no reviewer sees.

Usage
-----
    python3 tools/intern/check-ascii.py                     # encoding rule, whole repository
    python3 tools/intern/check-ascii.py framework examples  # encoding rule, only these paths
    python3 tools/intern/check-ascii.py --staged            # only what git has staged
    python3 tools/intern/check-ascii.py --fix-encoding      # rewrites the files that break it

    python3 tools/intern/check-ascii.py --find control      # every C0 control but TAB, LF, CR
    python3 tools/intern/check-ascii.py --find 0x08         # one value
    python3 tools/intern/check-ascii.py --find 0x01-0x12    # a range
    python3 tools/intern/check-ascii.py --find 0x08,0x1B,0x7F
    python3 tools/intern/check-ascii.py --find control --docs   # documents included

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

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# Extensions that must be pure ASCII.
CHECKED_SUFFIXES = (
    '.h', '.hpp', '.c', '.cpp', '.cc', '.cxx', '.inl',
    '.cmake', '.py', '.sh', '.bat', '.cmd', '.ps1',
    '.json', '.yml', '.yaml', '.xml', '.siml', '.fsml', '.dtml', '.xsd', '.qss', '.init', '.in',
    '.rc', '.rc2', '.def', '.service', '.plist', '.svg', '.toml', '.csv', '.supp', '.license',
    '.sln', '.vcxproj', '.filters', '.props', '.config',
)
CHECKED_NAMES = ('CMakeLists.txt', '.editorconfig', '.gitattributes', '.gitignore')

# Documents. Exempt from the ASCII rule, searched by '--find --docs'.
DOC_SUFFIXES = ('.md', '.txt')

# Directories that are never ours to police.
SKIPPED_DIRS = {
    '.git', '.claude', '.github_cache', 'build', 'product', 'thirdparty', 'node_modules',
    '__pycache__', '.vs', '.vscode', '.idea', 'out', 'bin', 'obj',
}

# Directory name prefixes that are never ours to police. A build tree carries the
# sources of the packages it fetched, and those are not subject to the rule.
SKIPPED_DIR_PREFIXES = ('build-', 'build_')

# Files whose content is character data, so non ASCII bytes in them are the subject and
# not a defect: the string classes, the character tables and the string tests. Glob
# patterns, matched against the path relative to the repository root. A waiver applies to
# the ASCII rule only. '--find' ignores this list, because a control character is unwanted
# in a character test as much as anywhere else.
ASCII_WAIVED = (
    '*String*.*',
    'framework/areg/base/TableDefs.hpp',
    'tests/units/*Encod*Test*.cpp',
)

# The files Visual Studio writes with a UTF-8 byte order mark.
BOM_ALLOWED = ('*.sln', '*.vcxproj', '*.filters', '*.props', '*packages.config')

# The sources a compiler reads. Non ASCII text in one of them needs the byte order mark.
COMPILED_SUFFIXES = ('.h', '.hpp', '.c', '.cpp', '.cc', '.cxx', '.inl')

UTF8_BOM = b'\xef\xbb\xbf'
WIDE_BOMS = (
    (b'\xff\xfe\x00\x00', 'utf-32', 'UTF-32 LE'),
    (b'\x00\x00\xfe\xff', 'utf-32', 'UTF-32 BE'),
    (b'\xff\xfe', 'utf-16', 'UTF-16 LE'),
    (b'\xfe\xff', 'utf-16', 'UTF-16 BE'),
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


def is_skipped_dir(name):
    """Tells whether a directory is outside the reach of the rule."""
    return name in SKIPPED_DIRS or name.startswith(SKIPPED_DIR_PREFIXES)


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


def matches(rel, patterns):
    """Tells whether the path, or its file name, matches one of the glob patterns."""
    name = os.path.basename(rel)
    return any(fnmatch.fnmatch(rel, pattern) or fnmatch.fnmatch(name, pattern) for pattern in patterns)


def is_waived(rel):
    return matches(rel, ASCII_WAIVED)


def is_document(rel):
    return rel.endswith(DOC_SUFFIXES) and os.path.basename(rel) not in CHECKED_NAMES


def encoding_of(data):
    """Returns the name of the encoding of the file content."""
    for mark, _, name in WIDE_BOMS:
        if data.startswith(mark):
            return name
    if b'\0' in data:
        return 'NUL bytes'
    bom = data.startswith(UTF8_BOM)
    body = data[len(UTF8_BOM):] if bom else data
    try:
        body.decode('utf-8')
    except UnicodeDecodeError:
        return 'not UTF-8'
    if all(byte < 0x80 for byte in body):
        return 'ASCII + BOM' if bom else 'ASCII'
    return 'UTF-8 + BOM' if bom else 'UTF-8'


def encoding_problem(rel, encoding):
    """Returns what is wrong with the encoding of the file, or None."""
    if encoding in ('NUL bytes', 'not UTF-8') or encoding.startswith('UTF-16') or encoding.startswith('UTF-32'):
        return 'is %s, it must be %s' % (encoding, 'UTF-8' if is_document(rel) or is_waived(rel) else 'ASCII')

    compiled = rel.endswith(COMPILED_SUFFIXES)
    if encoding == 'UTF-8' and compiled:
        return 'holds non ASCII text without a byte order mark, a compiler reads it in the code page of the machine'
    if encoding == 'UTF-8 + BOM' and compiled:
        return None
    if encoding.endswith('+ BOM') and not matches(rel, BOM_ALLOWED):
        return 'has a byte order mark'
    return None


def fix_encoding(path, rel, data):
    """Rewrites the file to the encoding the rule wants. Returns True if it was written."""
    for mark, codec, _ in WIDE_BOMS:
        if data.startswith(mark):
            data = data.decode(codec).encode('utf-8')
            break
    else:
        if encoding_of(data) in ('NUL bytes', 'not UTF-8'):
            return False

    body = data[len(UTF8_BOM):] if data.startswith(UTF8_BOM) else data
    ascii_only = all(byte < 0x80 for byte in body)
    if rel.endswith(COMPILED_SUFFIXES) and not ascii_only:
        body = UTF8_BOM + body
    elif matches(rel, BOM_ALLOWED) and data.startswith(UTF8_BOM):
        body = UTF8_BOM + body
    with open(path, 'wb') as out:
        out.write(body)
    return True


def offending_bytes(path, wanted, limit=10, data=None):
    """Returns [(line, column, byte, context)] for the first 'limit' wanted bytes."""
    if data is None:
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
            subdirs[:] = [d for d in subdirs if not is_skipped_dir(d)]
            for name in files:
                full = os.path.join(directory, name)
                if is_checked(full, docs):
                    yield full


def main():
    parser = argparse.ArgumentParser(
        description='Checks the encoding of the text files, or finds the byte values given by --find.')
    parser.add_argument('paths', nargs='*',
                        help='paths relative to the repository root; default is everything')
    parser.add_argument('--find', metavar='SPEC',
                        help='hunt these byte values instead of checking the encoding rule: '
                             'a value (0x08), a range (0x01-0x12), a list (0x08,0x1B), '
                             'or a name ("control", "non-ascii")')
    parser.add_argument('--docs', action='store_true',
                        help='let --find search the .md and .txt documents too; the encoding '
                             'rule always covers them')
    parser.add_argument('--staged', action='store_true',
                        help='check only the files staged in git, for a pre-commit hook')
    parser.add_argument('--fix-encoding', action='store_true',
                        help='rewrite the files that break the encoding rule: UTF-16 and UTF-32 '
                             'become ASCII or UTF-8, a byte order mark is removed or added')
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
    fixed = 0
    bad = []
    wrong_encoding = []
    encodings = {}
    for path in collect(args.paths, args.staged, args.docs or not hunting):
        rel = relative(path)
        checked += 1
        if hunting:
            found = offending_bytes(path, wanted)
            if found:
                bad.append((rel, found))
            continue

        try:
            with open(path, 'rb') as src:
                data = src.read()
        except OSError as err:
            bad.append((rel, [(0, 0, None, str(err))]))
            continue

        encoding = encoding_of(data)
        problem = encoding_problem(rel, encoding)
        if (problem is not None) and args.fix_encoding and fix_encoding(path, rel, data):
            with open(path, 'rb') as src:
                data = src.read()
            encoding = encoding_of(data)
            problem = encoding_problem(rel, encoding)
            fixed += 1
            print('%s: rewritten as %s' % (rel, encoding))
        encodings[encoding] = encodings.get(encoding, 0) + 1
        if problem is not None:
            wrong_encoding.append('%s: %s' % (rel, problem))
        elif (encoding in ('ASCII', 'ASCII + BOM')) or is_document(rel) or is_waived(rel):
            continue
        else:
            found = offending_bytes(path, wanted, data=data[len(UTF8_BOM):] if data.startswith(UTF8_BOM) else data)
            if found:
                bad.append((rel, found))

    lines = sorted(wrong_encoding)
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
        if (not hunting) and encodings:
            print('Encodings: ' + ', '.join('%s %d' % item for item in sorted(encodings.items())))
        if fixed:
            print('%d file(s) rewritten.' % fixed)
        if wrong_encoding:
            print('')
            print('%d of %d checked file(s) break the encoding rule.' % (len(wrong_encoding), checked))
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
        elif not wrong_encoding:
            print('%d file(s) checked, the encodings are right.' % checked)

    return 1 if (bad or wrong_encoding) else 0


if __name__ == '__main__':
    sys.exit(main())
