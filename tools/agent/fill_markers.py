#!/usr/bin/env python3
"""Fill every TODO(you) marker of a project from one worksheet.

    python3 fill_markers.py --bodies bodies.txt [--src src] [--dry-run]

gen_skeleton.py writes the worksheet with a section per open marker. Under each
"== <marker>" line goes the code that replaces that marker's line:

    == provider_state
        uint32_t mCredit{ 0 };
    == request_insert_coin
        set_credit( credit() + coinValue );

A line starting with "#|" is furniture of the worksheet and is dropped. Every
other line under a "==" is code and travels as written, so a comment in a body
is "//" and a line starting with "#" has to be a preprocessor directive. A
section with no code under it stays open and nothing is written for it: one pass
fills what it knows, a later pass the rest.

A name that matches no marker, a marker named twice, an ambiguous name and a "#"
line that is no directive are all refused before anything is written: either
every section applies or none does.
"""
import argparse
import difflib
import json
import os
import re
import sys

HEADER = re.compile(r'^==\s+([A-Za-z_][\w]*(?:\.[\w]+)?(?::[A-Za-z_][\w]*)?)\s*$')
MARKER = re.compile(r'//\s*TODO\(you\)\s+([A-Za-z_][\w]*)\s*:\s*(.*?)\s*$')

# A line gen_skeleton.py wrote only so the skeleton runs before its marker is filled.
# It belongs to the marker above it, so filling that marker takes it away too. A line
# after a marker that carries no tag is real code and is left alone.
PLACEHOLDER = re.compile(r'//\s*placeholder\(you\)')

# A run that filled most of its markers does not need the rest listed in full.
SHOWN = 8


def fail(message):
    sys.stderr.write('fill_markers: {}\n'.format(message))
    sys.exit(2)


# The worksheet's own furniture. No C++ line begins this way, so a body never loses
# one, and a note the author writes in the style of these is caught below instead.
NOTE = '#|'

# The heading that says which file the sections below it belong to. It carries no
# marker of its own, so it survives only while a section under it does.
FILE_HEADING = re.compile(r'^#\|\s*----\s')

# What a line of a body may be when it begins with a hash. Anything else there was
# meant as a comment, and C++ has no such comment.
DIRECTIVE = re.compile(r'^\s*#\s*(include|define|undef|if|ifdef|ifndef|elif|else|'
                       r'endif|pragma|error|warning|line)\b')


def note(line):
    """True for a line of the worksheet's own furniture, never code of a body."""
    return line.startswith(NOTE)


def read_bodies(path):
    """The worksheet as [(name, [line, ...])], in the order it names its markers."""
    try:
        with open(path, encoding='utf-8') as handle:
            lines = handle.read().splitlines()
    except (IOError, OSError) as problem:
        fail('cannot read {}: {}'.format(path, problem))
    sections = []
    current = None
    for number, line in enumerate(lines, 1):
        if note(line):
            continue
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
    for name, body, number in sections:
        for offset, line in enumerate(body, 1):
            if line.lstrip().startswith('#') and not DIRECTIVE.match(line):
                fail('{} line {}: section "{}" carries "{}", which is neither code nor '
                     'a "{}" note of this worksheet. A comment in a body is "//"; leave '
                     'a section empty to keep its marker open. Nothing was written'
                     .format(path, number + offset, name, line.strip()[:48], NOTE))
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
                    last = index
                    while last + 1 < len(lines) and PLACEHOLDER.search(lines[last + 1]):
                        last += 1
                    found.setdefault(hit.group(1), []).append(
                        (path, index, line, last - index))
    return found


def expectations_of(path):
    """Every named "expect" hole of a scenario file: {name: (scenario, process)}.

    These are not lines of a source file. The body of such a section is a list of
    regular expressions, written into the JSON as one, so no comma and no quote of
    the file's own syntax is ever the author's to get right.
    """
    found = {}
    if not os.path.exists(path):
        return found
    try:
        with open(path, encoding='utf-8') as handle:
            document = json.load(handle)
        scenarios = document['scenarios']
    except (ValueError, OSError, KeyError, TypeError):
        return found
    for outer, scenario in enumerate(scenarios):
        for inner, spec in enumerate(scenario.get('procs', [])):
            for entry in spec.get('expect', []):
                hit = MARKER.search('// ' + entry)
                if hit:
                    found[hit.group(1)] = (outer, inner)
    return found


def write_expectations(path, filled):
    """Put each section's regular expressions in the process they belong to."""
    with open(path, encoding='utf-8') as handle:
        document = json.load(handle)
    for (outer, inner), body in filled.items():
        document['scenarios'][outer]['procs'][inner]['expect'] = body
    with open(path, 'w', encoding='utf-8', newline='\n') as handle:
        json.dump(document, handle, indent=2)
        handle.write('\n')


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
    parser.add_argument('--scenarios', default='scenarios.json',
                        help='the scenario file whose named "expect" holes this also '
                             'fills (default: scenarios.json)')
    parser.add_argument('--dry-run', action='store_true',
                        help='report what would change and write nothing')
    args = parser.parse_args()

    sections = read_bodies(args.bodies)
    # A section with no code under it is a marker this pass is not filling. Writing
    # it would take the marker and its placeholder away and leave a hole in the file.
    blank = [name for name, body in sections if not body]
    sections = [(name, body) for name, body in sections if body]
    if not sections:
        fail('{} carries no code: every section under a "== <marker>" line is empty, '
             'so there is nothing to fill'.format(args.bodies))
    markers = markers_of(args.src)
    expectations = expectations_of(args.scenarios)
    clash = sorted(set(markers) & set(expectations))
    if clash:
        fail('"{}" names both a marker of {} and an expectation of {}. Rename the '
             'expectation'.format(clash[0], args.src, args.scenarios))
    if not (markers or expectations):
        fail('no TODO(you) marker is left in {} and no named expectation in {}. There '
             'is nothing to fill'.format(args.src, args.scenarios))

    # Resolve every section before writing any file, so a refusal leaves the tree
    # exactly as it was rather than half filled.
    planned = []
    claimed = {}
    expected = {}
    dropped = 0
    for name, body in sections:
        if name in expectations:
            expected[expectations[name]] = [text.strip() for text in body
                                            if text.strip()]
            continue
        path, index, line, extra = resolve(name, markers, args.bodies)
        if (path, index) in claimed:
            fail('{} fills the same marker twice, as "{}" and as "{}"'
                 .format(args.bodies, claimed[(path, index)], name))
        claimed[(path, index)] = name
        dropped += extra
        planned.append((path, index, extra, name, place(body, line)))

    if expected and not args.dry_run:
        write_expectations(args.scenarios, expected)
    if expected:
        print('{} {}: {} process expectation(s) written'
              .format('would fill' if args.dry_run else 'filled',
                      args.scenarios.replace(os.sep, '/'), len(expected)))

    edits = {}
    for path, index, extra, name, body in planned:
        edits.setdefault(path, {})[index] = (extra, body, name)

    for path in sorted(edits):
        with open(path, encoding='utf-8') as handle:
            lines = handle.read().splitlines()
        out = []
        skip = 0
        # Where each body ended up, so the file does not have to be opened to see it.
        landed = []
        for index, line in enumerate(lines):
            if skip:
                skip -= 1
            elif index in edits[path]:
                extra, body, name = edits[path][index]
                landed.append((len(out) + 1, name, len(body)))
                out.extend(body)
                skip = extra
            else:
                out.append(line)
        if not args.dry_run:
            with open(path, 'w', encoding='utf-8') as handle:
                handle.write('\n'.join(out) + '\n')
        print('{} {}: {} marker(s) filled'
              .format('would fill' if args.dry_run else 'filled',
                      path.replace(os.sep, '/'), len(edits[path])))
        for number, name, count in landed:
            print('  {:<28} line {:<5} {} line(s)'.format(name, number, count))

    filled = set(claimed)
    left = [(name, path) for name, places in sorted(markers.items())
            for path, index, _, _ in places if (path, index) not in filled]
    left += [(name, args.scenarios) for name in sorted(expectations)
             if expectations[name] not in expected]
    print('{} of {} marker(s) filled{}'
          .format(len(planned) + len(expected), len(planned) + len(expected) + len(left),
                  ', {} placeholder line(s) dropped with them'.format(dropped)
                  if dropped else ''))
    if args.dry_run:
        for name, path in left[:SHOWN]:
            print('  still open: {} in {}'.format(name, path.replace(os.sep, '/')))
        if len(left) > SHOWN:
            print('  and {} more'.format(len(left) - SHOWN))
        return 0
    consume(args.bodies, set(name for name, _ in sections))
    if not left:
        print('  {} is empty: every marker of this project is filled'
              .format(args.bodies.replace(os.sep, '/')))
        return 0
    named = set(name for name, _ in sections) | set(blank)
    orphans = [entry for entry in left if entry[0] not in named]
    print('  {} marker(s) still open; {} now carries a section for {} of them'
          .format(len(left), args.bodies.replace(os.sep, '/'), len(left) - len(orphans)))
    for name, path in orphans[:SHOWN]:
        print('  no section for: {} in {}'.format(name, path.replace(os.sep, '/')))
    if len(orphans) > SHOWN:
        print('  and {} more the worksheet does not name; gen_skeleton.py --todos '
              'lists them'.format(len(orphans) - SHOWN))
    return 0


def consume(path, applied):
    """Take the sections that were applied out of the worksheet.

    What is left in the file is what is left to do, so the same file is given to
    this tool again without a filled marker being named a second time. The notes
    before the first section are kept: they are the worksheet's own guidance.

    A "#| ---- <file>" heading is held back until a section under it survives, and
    dropped when none does. Emitting it on the previous group's verdict labels the
    surviving section with the wrong file.
    """
    with open(path, encoding='utf-8') as handle:
        lines = handle.read().splitlines()
    out, pending, keep = [], [], True
    for line in lines:
        if FILE_HEADING.match(line):
            pending, keep = [line], None
            continue
        found = HEADER.match(line)
        if found:
            keep = found.group(1) not in applied
            if keep and pending:
                if out and out[-1].strip():
                    out.append('')
                out.extend(pending)
                pending = []
        if keep is None:
            pending.append(line)
        elif keep:
            out.append(line)
    text = '\n'.join(out).rstrip() + '\n'
    with open(path, 'w', encoding='utf-8', newline='\n') as handle:
        handle.write(text)



if __name__ == '__main__':
    sys.exit(main())
