#!/usr/bin/env python3
# -*- coding: ascii -*-
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""
Finds virtual methods of non-template classes that a header defines inline.

areg is a shared library. A client compiler that devirtualizes a call to an inline
virtual method may refer to the library's copy of it instead of emitting its own, and
a library built with link time optimization keeps that copy local: the client fails
to link (bug 418, tests/client-link). A virtual method of a non-template class is
therefore defined in its .cpp. Class templates are not checked: every client emits
their methods itself. Virtual destructors are not checked yet.

    python3 tools/intern/check_inline_virtuals.py           check framework/
    python3 tools/intern/check_inline_virtuals.py <dir>...  check other directories

Exit code 0 when nothing is found, 1 otherwise.
"""

import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

CLASS_HEAD = re.compile(r'^\s*(?:class|struct)\s+(?:AREG_API\s+|AREG_EXTEND_API\s+|LOGGER_API\s+)?([A-Za-z_]\w*)\b(?!\s*;)')
VIRTUAL_DECL = re.compile(r'^\s*(?:inline\s+)?(?:virtual\s+)?[^;{}()~]*?\b([A-Za-z_]\w*)\s*\([^;{}]*\)[^;{}]*?\b(?:override|final)\b'
                          r'|^\s*(?:inline\s+)?virtual\s+[^;{}()~]*?\b([A-Za-z_]\w*)\s*\(')
OUT_OF_CLASS = re.compile(r'^inline\s+[^;(]*?\b([A-Za-z_]\w*)::([A-Za-z_]\w*)\s*\(')


def arity(signature):
    """Returns the number of parameters of the first parameter list in the text."""
    inner = signature[signature.index('(') + 1:]
    depth, params, current = 1, 0, ''
    for char in inner:
        if char in '(<':
            depth += 1
        elif char in ')>':
            depth -= 1
            if depth == 0:
                break
        elif char == ',' and depth == 1:
            params += 1
        current += char
    return params + (1 if current.strip() not in ('', 'void') else 0)


def strip_comments(text):
    """Blanks the comments and string literals, keeping the line breaks."""
    def blank(match):
        return re.sub(r'[^\n]', ' ', match.group(0))
    return re.sub(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\\n])*"', blank, text, flags=re.S)


def classes_of(lines):
    """Returns (name, is_template, first_line, last_line) of every class body in the header."""
    result = []
    for index, line in enumerate(lines):
        match = CLASS_HEAD.match(line)
        if match is None:
            continue

        head = index
        while ('{' not in lines[head]) and (';' not in lines[head]) and (head + 1 < len(lines)):
            head += 1
        if '{' not in lines[head]:
            continue

        prev = index - 1
        while (prev >= 0) and (lines[prev].strip() == ''):
            prev -= 1
        is_template = (prev >= 0) and lines[prev].lstrip().startswith('template')

        depth = 0
        end = head
        for end in range(head, len(lines)):
            depth += lines[end].count('{') - lines[end].count('}')
            if depth <= 0:
                break
        result.append((match.group(1), is_template, head, end))
    return result


def check_header(path):
    """Returns the findings of one header as (line number, text)."""
    text = strip_comments(open(path, encoding='utf-8', errors='replace').read())
    lines = text.split('\n')
    findings = []
    virtuals = {}
    for name, is_template, first, last in classes_of(lines):
        if is_template:
            continue
        depth = 0
        for number in range(first, last + 1):
            line = lines[number]
            if number > first and depth == 1:
                match = VIRTUAL_DECL.match(line)
                if match and '~' not in line.split('(')[0]:
                    method = match.group(1) or match.group(2)
                    virtuals.setdefault(name, set()).add((method, arity(line[line.index(method):])))
                    body = line.split(')')[-1]
                    if re.match(r'^\s*inline\b', line) or ('{' in body):
                        findings.append((number + 1, line.strip()))
            depth += line.count('{') - line.count('}')

    for number, line in enumerate(lines):
        match = OUT_OF_CLASS.match(line)
        if match and ((match.group(2), arity(line[match.start(2):])) in virtuals.get(match.group(1), ())):
            findings.append((number + 1, line.strip()))
    return findings


def main(argv):
    dirs = argv[1:] or [os.path.join(ROOT, 'framework')]
    count = 0
    for top in dirs:
        for folder, _, files in os.walk(top):
            for file in sorted(files):
                if not file.endswith(('.hpp', '.h')):
                    continue
                path = os.path.join(folder, file)
                for number, line in check_header(path):
                    print('%s:%d: inline virtual method: %s' % (os.path.relpath(path, ROOT), number, line))
                    count += 1

    if count != 0:
        print('%d inline virtual method(s); define them in the .cpp (CLAUDE.md, "No inline virtual methods")' % count)
        return 1

    print('no inline virtual methods in non-template classes')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
