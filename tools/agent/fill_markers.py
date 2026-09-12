#!/usr/bin/env python3
"""Fill every TODO(you) marker of a project from one bodies file.

    python3 fill_markers.py --bodies bodies.txt [--src src] [--dry-run]

The bodies file names a marker and gives the code that replaces its line:

    == provider_state
        uint32_t mCredit{ 0 };
    == request_insert_coin
        set_credit( credit() + coinValue );

Nothing is escaped, so quotes and backslashes travel as written. A name that
matches no marker, a marker named twice, and an ambiguous name are all refused
before anything is written: either every section applies or none does.
"""
import argparse
import difflib
import os
import re
import sys

HEADER = re.compile(r'^==\s+([A-Za-z_][\w]*(?:\.[\w]+)?(?::[A-Za-z_][\w]*)?)\s*$')
MARKER = re.compile(r'//\s*TODO\(you\)\s+([A-Za-z_][\w]*)\s*:\s*(.*?)\s*$')

# A run that filled most of its markers does not need the rest listed in full.
SHOWN = 8


def fail(message):
    sys.stderr.write('fill_markers: {}\n'.format(message))
    sys.exit(2)


def read_bodies(path):
    """The bodies file as [(name, [line, ...])], in the order it names them."""
    try:
        with open(path, encoding='utf-8') as handle:
            lines = handle.read().splitlines()
    except (IOError, OSError) as problem:
        fail('cannot read {}: {}'.format(path, problem))
    sections = []
    current = None
    for number, line in enumerate(lines, 1):
        found = HEADER.match(line)
        if found:
            current = (found.group(1), [], number)
            sections.append(current)
        elif current is None:
            if line.strip():
                fail('{} line {}: a body must follow a "== <marker>" line, and this '
                     'one follows nothing'.format(path, number))
        else:
            current[1].append(line)
    if not sections:
        fail('{} names no marker. A section starts with "== " and the marker name'
             .format(path))
    seen = {}
    for name, _, number in sections:
        if name in seen:
            fail('{} names "{}" twice, at line {} and line {}. One section per marker'
                 .format(path, name, seen[name], number))
        seen[name] = number
    return [(name, trim(body)) for name, body, _ in sections]


def trim(body):
    """The body without the blank lines that only separate one section from the next."""
    while body and not body[0].strip():
        body.pop(0)
    while body and not body[-1].strip():
        body.pop()
    return body


def markers_of(root):
    """Every marker in the project: {name: [(path, index, line), ...]}."""
    found = {}
    if not os.path.isdir(root):
        fail('{} is not a directory. Pass --src if the sources are elsewhere'.format(root))
    for folder, dirs, files in os.walk(root):
        dirs[:] = [d for d in dirs if d not in ('services', 'build')]
        for name in sorted(files):
            if not name.endswith(('.hpp', '.cpp')):
                continue
            path = os.path.join(folder, name)
            with open(path, encoding='utf-8') as handle:
                lines = handle.read().splitlines()
            for index, line in enumerate(lines):
                hit = MARKER.search(line)
                if hit:
                    found.setdefault(hit.group(1), []).append((path, index, line))
    return found


def resolve(name, markers, where):
    """The one marker this section names, or a refusal that says how to say it."""
    wanted, _, bare = name.rpartition(':')
    places = markers.get(bare or name)
    if not places:
        near = difflib.get_close_matches(bare or name, sorted(markers), 3, 0.6)
        fail('{} names "{}", which is no open marker of this project -- it is spelt '
             'differently, or it is filled already.{}'
             .format(where, name,
                     ' Did you mean: {}?'.format(', '.join(near)) if near else
                     ' gen_skeleton.py --todos lists every marker left.'))
    if wanted:
        places = [p for p in places
                  if os.path.basename(p[0]) == wanted or
                  p[0].replace(os.sep, '/').endswith('/' + wanted)]
        if not places:
            fail('{} names "{}", and no {} carries a marker "{}"'
                 .format(where, name, wanted, bare))
    if len(places) > 1:
        fail('{} names "{}", which {} files carry: {}. Name one as '
             '"<file>:{}"'.format(where, name, len(places),
                                  ', '.join(os.path.basename(p[0]) for p in places),
                                  bare or name))
    return places[0]


def indent_of(line):
    return line[:len(line) - len(line.lstrip())]


def place(body, line):
    """The body as it will stand in the file, under the marker's own indentation.

    A body written from column one is shifted right to where the marker stood, so
    the code keeps the shape it was written in. One already indented is taken as it
    is: the author has placed it.
    """
    if not body:
        return []
    if body[0][:1].isspace():
        return list(body)
    pad = indent_of(line)
    return [pad + text if text.strip() else text for text in body]


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--bodies', required=True,
                        help='the file naming each marker and the code that fills it')
    parser.add_argument('--src', default='src',
                        help='the directory holding the sources (default: src)')
    parser.add_argument('--dry-run', action='store_true',
                        help='report what would change and write nothing')
    args = parser.parse_args()

    sections = read_bodies(args.bodies)
    markers = markers_of(args.src)
    if not markers:
        fail('no TODO(you) marker is left in {}. There is nothing to fill'
             .format(args.src))

    # Resolve every section before writing any file, so a refusal leaves the tree
    # exactly as it was rather than half filled.
    planned = []
    claimed = {}
    for name, body in sections:
        path, index, line = resolve(name, markers, args.bodies)
        if (path, index) in claimed:
            fail('{} fills the same marker twice, as "{}" and as "{}"'
                 .format(args.bodies, claimed[(path, index)], name))
        claimed[(path, index)] = name
        planned.append((path, index, line, name, place(body, line)))

    edits = {}
    for path, index, _, _, body in planned:
        edits.setdefault(path, {})[index] = body

    for path in sorted(edits):
        with open(path, encoding='utf-8') as handle:
            lines = handle.read().splitlines()
        out = []
        for index, line in enumerate(lines):
            if index in edits[path]:
                out.extend(edits[path][index])
            else:
                out.append(line)
        if not args.dry_run:
            with open(path, 'w', encoding='utf-8') as handle:
                handle.write('\n'.join(out) + '\n')
        print('{} {}: {} marker(s) filled'
              .format('would fill' if args.dry_run else 'filled',
                      path.replace(os.sep, '/'), len(edits[path])))

    filled = set(claimed)
    left = [(name, path, index) for name, places in sorted(markers.items())
            for path, index, _ in places if (path, index) not in filled]
    print('{} of {} marker(s) filled'.format(len(planned), len(planned) + len(left)))
    for name, path, _ in left[:SHOWN]:
        print('  still open: {} in {}'.format(name, path.replace(os.sep, '/')))
    if len(left) > SHOWN:
        print('  and {} more; gen_skeleton.py --todos lists them all'
              .format(len(left) - SHOWN))
    if left:
        print('  Give them to this tool in one more bodies file.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
