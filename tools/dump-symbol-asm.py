#!/usr/bin/env python3
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""Prints the machine code a compiler produced for named functions.

TEMPORARY, bug-fixing phase. Delete together with the defect it was written for.

A failure that appears on a build server and on no development machine is either a
race or a difference in code generation. The second kind cannot be argued about from
source: it has to be read out of the binary that actually failed. This prints the
disassembly of the functions whose demangled name contains one of the given patterns
and publishes it as build server annotations, which are readable without a token,
unlike the job log and the artifacts.

    dump-symbol-asm.py <binary> <pattern> [<pattern> ...] [--title <name>]
    dump-symbol-asm.py --tool objdump ...

Nothing here may fail the build: a missing binary, a missing disassembler or a symbol
that was inlined away are all reported and none of them is an error.
"""

import argparse
import os
import re
import shutil
import subprocess
import sys


ANNOTATION_CHARS = 3200     #!< characters of one annotation message that survive
ANNOTATION_COUNT = 4        #!< annotations one invocation may spend


def annotation_text(text):
    """Encodes a multi line report so that a build server keeps the line breaks."""
    return text.replace('%', '%25').replace('\r', '%0D').replace('\n', '%0A')


def disassemble(binary, tool):
    """Returns the demangled disassembly of a binary, or an empty string."""
    if shutil.which(tool) is None:
        print('::warning title=asm::%s is not installed' % tool)
        return ''
    command = [tool, '-d', '-C', '--no-show-raw-insn', binary]
    try:
        done = subprocess.run(command, stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT, timeout=600)
    except Exception as error:
        print('::warning title=asm::%s failed: %s' % (tool, error))
        return ''
    return done.stdout.decode('utf-8', 'replace')


def functions(text):
    """Splits a disassembly into (name, body) pairs, one per function."""
    found = []
    name = None
    body = []
    for line in text.splitlines():
        match = re.match(r'^[0-9a-f]+ <(.*)>:$', line)
        if match is not None:
            if name is not None:
                found.append((name, '\n'.join(body)))
            name = match.group(1)
            body = [line]
        elif name is not None:
            if line.strip():
                body.append(line)
            else:
                found.append((name, '\n'.join(body)))
                name = None
                body = []
    if name is not None:
        found.append((name, '\n'.join(body)))
    return found


def call_digest(wanted):
    """Names every function the given functions call or jump out to.

    The listing itself may be cut off by the annotation limit; this cannot. A dispatch
    that was compiled into a call to something it has no business calling shows up here
    in one line, which is the question this script exists to answer.
    """
    lines = []
    for name, body in wanted:
        targets = []
        for match in re.finditer(r'^\s+[0-9a-f]+:\s+(call|jmp|bl|b)\s+.*?<(.+?)>\s*$',
                                 body, flags=re.M):
            target = match.group(2)
            if target.startswith(name):      # a jump inside the function itself
                continue
            if target not in targets:
                targets.append(target)
        lines.append('%s\n    calls: %s' % (name, ', '.join(targets) or '(none)'))
    return '\n'.join(lines)


def publish(title, report):
    """Prints a report and spends as many annotations on it as it needs."""
    print(report)
    sys.stdout.flush()
    chunks = [report[at:at + ANNOTATION_CHARS]
              for at in range(0, len(report), ANNOTATION_CHARS)]
    for index, chunk in enumerate(chunks[:ANNOTATION_COUNT]):
        print('::notice title=%s %d/%d::%s'
              % (title, index + 1, min(len(chunks), ANNOTATION_COUNT),
                 annotation_text(chunk)))
    if len(chunks) > ANNOTATION_COUNT:
        print('::warning title=%s::%d of %d parts did not fit into annotations'
              % (title, len(chunks) - ANNOTATION_COUNT, len(chunks)))
    sys.stdout.flush()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('binary', help='the executable or library to read')
    parser.add_argument('patterns', nargs='+', help='substrings of demangled names')
    parser.add_argument('--tool', default='objdump', help='the disassembler to use')
    parser.add_argument('--title', default='asm', help='the annotation title')
    args = parser.parse_args()

    if not os.path.isfile(args.binary):
        print('::warning title=%s::no %s' % (args.title, args.binary))
        return 0

    text = disassemble(args.binary, args.tool)
    if not text:
        return 0

    wanted = []
    for name, body in functions(text):
        if any(pattern in name for pattern in args.patterns):
            wanted.append((name, body))

    if not wanted:
        print('::warning title=%s::none of %s is a symbol of %s -- inlined or absent'
              % (args.title, ', '.join(args.patterns), os.path.basename(args.binary)))
        return 0

    header = '%s, %d matching function(s)\n' % (os.path.basename(args.binary), len(wanted))
    digest = call_digest(wanted)
    print('::notice title=%s calls::%s' % (args.title, annotation_text(digest)))
    publish(args.title, header + digest + '\n\n'
            + '\n\n'.join(body for _, body in wanted))
    return 0


if __name__ == '__main__':
    sys.exit(main())
