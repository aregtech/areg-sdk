#!/usr/bin/env python3
"""Check that the citations in a set of engineering notes still resolve.

The local notes an agent keeps between sessions are only worth their bytes while
they are true. A stale one is worse than a missing one, because it is believed:
the agent reads that a fix was applied, or that a constant holds a value, and
stops looking. Nothing in the repository checked that until now.

The notes state where their evidence is. This tool follows every citation and
reports the ones that no longer land:

    `SocketDefs.cpp:701`            a file and a line
    `EventDefs.hpp:52-54`           a file and a range
    `framework/areg/base/Foo.hpp`   a path
    `SO_EXCLUSIVEADDRUSE`           an identifier, on a line that also cites a file

A citation passes when the file exists, the line is inside it, and every plain
identifier named on the same line of the note appears within a window around the
cited line -- or, for an identifier with no line, anywhere in that file.

    python3 tools/agent/check_claims.py                  # .claude/memory and .claude/lessons
    python3 tools/agent/check_claims.py NOTES --window 40
    python3 tools/agent/check_claims.py --verbose

Exit code 0 means every citation resolved. This is a report about notes, not
about the framework: a failure means the note is out of date, and the note is
what gets corrected.
"""

import argparse
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
DEFAULT_DIRS = (os.path.join('.claude', 'memory'), os.path.join('.claude', 'lessons'))
EXTERNAL_FILE = os.path.join(HERE, 'claims_external.txt')

# '.claude' is here because the notes cite each other and cite the skills: a memory
# that points at a lesson, or a lesson that names the skill file it belongs to, is a
# citation like any other and goes stale the same way.
SEARCH_DIRS = ('framework', 'tools', 'tests', 'examples', 'docs', 'conf', '.claude')
# 'archive' and 'scratch' under '.claude' hold what a past session left behind. A note
# that resolves only against those is pointing at something nobody loads.
SKIP_DIRS = {'.git', 'build', 'product', '__pycache__', 'node_modules', 'thirdparty',
             'archive', 'scratch', 'worktrees'}

# `File.ext:12` or `File.ext:12-34`, optionally with directories before it.
LINE_CITE_RE = re.compile(
    r'`([A-Za-z0-9_./+-]+\.(?:hpp|hxx|h|cpp|cxx|cc|py|sh|bat|cmake|md|init|json|xml|xsd))'
    r':(\d+)(?:-(\d+))?`')
# A bare backticked path, recognised by having a directory separator or a suffix.
PATH_CITE_RE = re.compile(
    r'`([A-Za-z0-9_./+-]*/[A-Za-z0-9_./+-]+\.'
    r'(?:hpp|hxx|h|cpp|cxx|cc|py|sh|bat|cmake|md|init|json|xml|xsd))`')
# A backticked C or C++ identifier, with any call parentheses stripped.
IDENT_RE = re.compile(r'`([A-Za-z_][A-Za-z0-9_]*(?:::[A-Za-z_][A-Za-z0-9_]*)*)'
                      r'(?:\(\))?`')

# Words that look like identifiers but name nothing in the tree.
NOT_IDENTIFIERS = frozenset((
    'true', 'false', 'null', 'nullptr', 'void', 'int', 'bool', 'const', 'if',
    'else', 'return', 'ms', 'us', 'ns', 'ON', 'OFF', 'NOTSET', 'TODO', 'N',
))

# A line that proposes something, or records that something is gone, is not a claim
# that it exists today. Neither shape is stale when the name does not resolve.
PROPOSAL_RE = re.compile(
    r'\b(?:create|creating|add|adding|introduce|would|should|must be|write|'
    r'implement|implementing|plan|proposed|TODO)\b', re.IGNORECASE)
ABSENCE_RE = re.compile(
    r'\b(?:is gone|are gone|was removed|were removed|no longer|there is no|'
    r'does not exist|do not exist|deleted|dropped|replaced by|renamed)\b',
    re.IGNORECASE)


def external_names():
    """Names that belong to the OS, the toolchain or a standard library."""
    known = set()
    if not os.path.isfile(EXTERNAL_FILE):
        return known
    with open(EXTERNAL_FILE, 'r', encoding='utf-8') as handle:
        for line in handle:
            entry = line.split('#', 1)[0].strip()
            if entry:
                known.add(entry)
    return known


def index_tree():
    """Every candidate source file by base name and by path, and every word in them.

    The word set is what makes the strongest check cheap: an identifier a note
    puts in backticks that appears nowhere in the tree names nothing, and that is
    the shape a stale note takes -- a constant that was renamed or deleted while
    the note went on describing it.
    """
    by_base = {}
    by_path = {}
    words = set()
    word_re = re.compile(r'[A-Za-z_][A-Za-z0-9_]*')
    for top in SEARCH_DIRS:
        start = os.path.join(ROOT, top)
        if not os.path.isdir(start):
            continue
        for folder, folders, files in os.walk(start):
            folders[:] = [d for d in folders if d not in SKIP_DIRS]
            for entry in files:
                full = os.path.join(folder, entry)
                relative = os.path.relpath(full, ROOT).replace(os.sep, '/')
                by_path[relative] = full
                by_base.setdefault(entry, []).append(full)
                if entry.endswith(('.hpp', '.hxx', '.h', '.cpp', '.cxx', '.cc',
                                   '.py', '.sh', '.cmake', '.txt')):
                    try:
                        with open(full, 'r', encoding='utf-8', errors='replace') as handle:
                            words.update(word_re.findall(handle.read()))
                    except OSError:
                        pass
    for entry in os.listdir(ROOT):
        full = os.path.join(ROOT, entry)
        if os.path.isfile(full):
            by_path[entry] = full
            by_base.setdefault(entry, []).append(full)
    return by_base, by_path, words


def index_notes(notes, by_base, by_path):
    """Let one note cite another: the note tree is part of what resolves."""
    for note in notes:
        relative = os.path.relpath(note, ROOT).replace(os.sep, '/')
        by_path[relative] = note
        by_base.setdefault(os.path.basename(note), []).append(note)


def resolve(target, by_base, by_path):
    """The one file a citation names, or None when it is ambiguous or absent."""
    # lstrip() takes a character set, so it would eat the leading dot of a dotted
    # directory and turn '.claude/...' into 'claude/...', which resolves to nothing.
    cleaned = target[2:] if target.startswith('./') else target
    if cleaned in by_path:
        return by_path[cleaned]
    for relative, full in by_path.items():
        if relative.endswith('/' + cleaned):
            return full
    hits = by_base.get(os.path.basename(cleaned), [])
    return hits[0] if len(hits) == 1 else None


def notes_in(folder):
    found = []
    for entry in sorted(os.listdir(folder)):
        if entry.endswith('.md'):
            found.append(os.path.join(folder, entry))
    return found


def check(folders, window, verbose, strict):
    by_base, by_path, words = index_tree()
    external = external_names()
    if not by_path:
        print('nothing to check against under %s' % ROOT, file=sys.stderr)
        return 2

    notes = []
    for folder in folders:
        full = folder if os.path.isabs(folder) else os.path.join(ROOT, folder)
        if os.path.isdir(full):
            notes.extend(notes_in(full))
        elif os.path.isfile(full):
            notes.append(full)
    if not notes:
        print('no notes found in: %s' % ', '.join(folders))
        return 0

    index_notes(notes, by_base, by_path)

    problems = []
    notes_out = []
    passed = 0
    for note in notes:
        shown = os.path.relpath(note, ROOT)
        with open(note, 'r', encoding='utf-8', errors='replace') as handle:
            lines = handle.read().splitlines()

        for number, line in enumerate(lines, 1):
            where = '%s:%d' % (shown, number)
            proposing = (PROPOSAL_RE.search(line) is not None
                         or ABSENCE_RE.search(line) is not None)
            cites = LINE_CITE_RE.findall(line)
            named = [n for n in IDENT_RE.findall(line)
                     if n not in NOT_IDENTIFIERS and '.' not in n]

            # An identifier the note puts in backticks that appears nowhere in the
            # tree names nothing. This is the check that finds a constant which was
            # renamed or deleted while the note went on describing it.
            for name in sorted(set(named)):
                leaf = name.split('::')[-1]
                if leaf in words or leaf in external or name in external:
                    passed += 1
                elif proposing:
                    pass
                else:
                    problems.append('%s: `%s` appears nowhere in the tree'
                                    % (where, name))

            for target, first, last in cites:
                path = resolve(target, by_base, by_path)
                if path is None:
                    if not proposing:
                        problems.append('%s: cites %s, which is not in the tree'
                                        % (where, target))
                    continue
                with open(path, 'r', encoding='utf-8', errors='replace') as handle:
                    body = handle.read().splitlines()
                start_line = int(first)
                end_line = int(last) if last else start_line
                if end_line > len(body):
                    problems.append('%s: cites %s:%s but the file has %d lines'
                                    % (where, target, last or first, len(body)))
                    continue
                passed += 1
                if verbose:
                    print('  ok   %-38s %s:%s' % (where, target, last or first))
                low = max(0, start_line - 1 - window)
                high = min(len(body), end_line + window)
                region = '\n'.join(body[low:high])
                # Soft: the identifier exists somewhere, but not where the note
                # points. Either the note drifted or it names two files at once.
                adrift = [n for n in sorted(set(named))
                          if n.split('::')[-1] in words
                          and n.split('::')[-1] not in region]
                if adrift and len(cites) == 1:
                    notes_out.append(
                        '%s: %s is not within %d lines of %s:%s'
                        % (where, ', '.join(adrift), window, target,
                           last or first))

            # Paths with no line number. A path already carrying one was handled
            # above, so it is not reported twice.
            cited_paths = {target for target, _first, _last in cites}
            for target in PATH_CITE_RE.findall(line):
                if target in cited_paths:
                    continue
                if resolve(target, by_base, by_path) is None:
                    if not proposing:
                        problems.append('%s: names %s, which is not in the tree'
                                        % (where, target))
                else:
                    passed += 1

    for entry in notes_out:
        print('note:  ' + entry)
    for problem in problems:
        print('error: ' + problem)
    print('%d claims checked, %d stale, %d adrift'
          % (passed + len(problems), len(problems), len(notes_out)))
    if problems:
        return 1
    return 1 if (strict and notes_out) else 0


def main():
    parser = argparse.ArgumentParser(
        description='Check that citations in engineering notes still resolve.')
    parser.add_argument('folders', nargs='*', default=list(DEFAULT_DIRS),
                        help='note folders or files (default: .claude/memory, .claude/lessons)')
    parser.add_argument('--window', type=int, default=25,
                        help='lines either side of a cited line to search (default: 25)')
    parser.add_argument('--verbose', action='store_true',
                        help='list the citations that resolved as well')
    parser.add_argument('--strict', action='store_true',
                        help='let an adrift citation fail as well')
    args = parser.parse_args()
    return check(args.folders or list(DEFAULT_DIRS), args.window, args.verbose,
                 args.strict)


if __name__ == '__main__':
    sys.exit(main())
