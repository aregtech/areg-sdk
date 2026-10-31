#!/usr/bin/env python3
"""Check docs/agent/members.json against what clang parses out of the public headers.

build_members.py reads the headers line by line. This reads them the way a compiler
does: every public header of framework/areg and framework/aregextend is included in
one translation unit, clang dumps its syntax tree, and every function, method,
scoped enumeration and class declared in a public header is looked up in the
inventory of its own library. A declaration clang sees and the inventory lacks is a
gap of build_members.py and fails the check.

The other direction is not checked: the inventory also holds names an inline body
calls and declarations of other platforms, which one parse never sees.

    python3 tools/intern/check_members_ast.py
    python3 tools/intern/check_members_ast.py --compiler clang++-18
"""
import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
FRAMEWORK = os.path.join(ROOT, 'framework')
TARGET = os.path.join(ROOT, 'docs', 'agent', 'members.json')
LIBRARIES = ('areg', 'aregextend')

# A source location "file:line:col". A "line:" or "col:" one keeps the previous file.
LOCATION = re.compile(r'([^\s:<>,=]+):\d+:\d+')

# A node line: the tree prefix, the node kind, its address.
NODE = re.compile(r'^([\s|`-]*)([A-Za-z]+) 0x[0-9a-f]+\b')

FUNCTIONS = frozenset(('FunctionDecl', 'CXXMethodDecl', 'FunctionTemplateDecl'))
SNAKE = re.compile(r'[a-z][a-z0-9_]*')
RECORD = re.compile(r'\b(?:class|struct) ([A-Z]\w*) definition\b')
SCOPED_ENUM = re.compile(r'\bclass ([A-Za-z_]\w*)(?: \'|$)')


def public_headers():
    """Every public header of both libraries, as an include path below framework/."""
    found = []
    for library in LIBRARIES:
        for base, dirs, files in os.walk(os.path.join(FRAMEWORK, library)):
            dirs[:] = sorted(d for d in dirs if d != 'private')
            for name in sorted(files):
                if name.endswith(('.hpp', '.h')):
                    found.append(os.path.relpath(os.path.join(base, name),
                                                 FRAMEWORK).replace(os.sep, '/'))
    return found


def library_of(path):
    """The library a dumped file belongs to, or None when it is not a public header."""
    path = path.replace('\\', '/')
    if '/private/' in path:
        return None
    for library in LIBRARIES:
        if '/{}/'.format(library) in '/' + path:
            return library
    return None


def declared_name(line):
    """The name of a node: the last word before its quoted type, or at the line end.

    "operator new" and "operator delete" are spelled in two words and return "".
    """
    words = line.split(" '", 1)[0].split()
    if len(words) < 2 or words[-2] == 'operator':
        return ''
    return words[-1]


def parse(lines):
    """Per library: function names, scoped enumerations with enumerators, classes."""
    found = dict((library, {'members': set(), 'enums': {}, 'classes': set()})
                 for library in LIBRARIES)
    current = None
    enum = None
    for line in lines:
        node = NODE.match(line)
        if not node:
            continue
        for location in LOCATION.finditer(re.sub(r"'[^']*'", '', line)):
            if location.group(1) not in ('line', 'col'):
                current = location.group(1)
        depth, kind = len(node.group(1)), node.group(2)
        if enum and depth <= enum[0]:
            enum = None
        library = library_of(current) if current else None
        if library is None:
            continue
        into = found[library]
        if kind in FUNCTIONS:
            if ' implicit ' in line:
                continue
            name = declared_name(line)
            if SNAKE.fullmatch(name):
                into['members'].add(name)
        elif kind == 'CXXRecordDecl':
            record = RECORD.search(line)
            if record and ' implicit ' not in line:
                into['classes'].add(record.group(1))
        elif kind == 'EnumDecl':
            scoped = SCOPED_ENUM.search(line.split(" '", 1)[0] + " '")
            if scoped:
                enum = (depth, into['enums'].setdefault(scoped.group(1), set()))
        elif kind == 'EnumConstantDecl' and enum:
            enum[1].add(declared_name(line))
    return found


def inventory():
    """The library inventories members.json holds, keyed like parse()."""
    with open(TARGET, encoding='utf-8') as handle:
        document = json.load(handle)
    extension = document.get('extension') or {}
    return {
        'areg': {'members': set(document.get('members') or []),
                 'enums': document.get('enumerations') or {},
                 'classes': None},
        'aregextend': {'members': set(extension.get('members') or []),
                       'enums': extension.get('enumerations') or {},
                       'classes': set(extension.get('classes') or [])},
    }


def compare(parsed, listed):
    """Every declaration clang saw that the inventory of its library lacks."""
    gaps = []
    for library in LIBRARIES:
        seen, have = parsed[library], listed[library]
        for name in sorted(seen['members'] - have['members']):
            gaps.append('{}: {}() is declared but not listed'.format(library, name))
        for name, values in sorted(seen['enums'].items()):
            if not values:
                continue
            missing = sorted(values - set(have['enums'].get(name) or []))
            if missing:
                gaps.append('{}: enumeration {} lacks {}'.format(
                    library, name, ', '.join(missing)))
        if have['classes'] is not None:
            for name in sorted(seen['classes'] - have['classes']):
                gaps.append('{}: class {} is defined but not listed'.format(
                    library, name))
    return gaps


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--compiler', default=os.environ.get('CLANGXX', 'clang++'),
                        help='the clang++ to parse with (default: $CLANGXX or clang++)')
    args = parser.parse_args()

    compiler = shutil.which(args.compiler)
    if not compiler:
        sys.stderr.write('error: {} is not on PATH; this check needs clang\n'
                         .format(args.compiler))
        return 2
    try:
        listed = inventory()
    except (OSError, ValueError) as error:
        sys.stderr.write('error: cannot read {}: {}\n'.format(TARGET, error))
        return 1

    headers = public_headers()
    with tempfile.TemporaryDirectory() as scratch:
        unit = os.path.join(scratch, 'all_headers.cpp')
        with open(unit, 'w', encoding='ascii') as handle:
            handle.writelines('#include "{}"\n'.format(path) for path in headers)
        command = [compiler, '-std=c++17', '-fsyntax-only', '-fno-color-diagnostics',
                   '-I' + FRAMEWORK, '-Xclang', '-ast-dump', unit]
        process = subprocess.Popen(command, stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE, text=True,
                                   encoding='utf-8', errors='replace')
        parsed = parse(process.stdout)
        errors = process.stderr.read()
        process.wait()
    if process.returncode != 0:
        sys.stderr.write(errors[-4000:])
        sys.stderr.write('error: clang could not parse the public headers\n')
        return 1

    gaps = compare(parsed, listed)
    for gap in gaps:
        print('MISSING  ' + gap)
    counts = ', '.join('{} {} names, {} enumerations{}'.format(
        library, len(parsed[library]['members']), len(parsed[library]['enums']),
        '' if listed[library]['classes'] is None
        else ', {} classes'.format(len(parsed[library]['classes'])))
        for library in LIBRARIES)
    print('{} header(s) parsed: {}; {} gap(s)'.format(len(headers), counts, len(gaps)))
    if gaps:
        print('fix tools/intern/build_members.py so it finds them, then run it')
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
