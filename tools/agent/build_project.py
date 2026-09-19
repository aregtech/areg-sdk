#!/usr/bin/env python3
# ===========================================================================
# Runs the mechanical chain of a project on areg, in one command:
#
#   documents  ->  application  ->  worksheet  ->  contract check  ->  configure
#   ->  build
#
# None of those six steps carries a decision, and each costs a request of its
# own. This runs them in order and stops at the first failure, naming the step,
# the command it ran and what to do about it.
#
#   python3 tools/agent/build_project.py --spec design.json
#   python3 tools/agent/build_project.py --spec design.json --regenerate
#
# The application is written once. A later run keeps the sources as they are and
# only builds them, so filled-in markers are never overwritten; --regenerate asks
# for them back, and discards what is in them that bodies.txt does not carry. The
# worksheet step writes every body it does carry, so a body is changed there and
# reaches the build in the same command.
#
# Exit code 0 when every step passed, 1 otherwise.
# ===========================================================================
import argparse
import glob
import hashlib
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
PYTHON = sys.executable or 'python3'

sys.path.insert(0, HERE)
import gen_docs  # noqa: E402
import gen_skeleton  # noqa: E402

# A fixed job count is right on every machine the corpus has to describe, and it
# costs no page: $(nproc) is absent on macOS and spelled differently on Windows.
DEFAULT_JOBS = 8

# What to do when a step fails. Naming the step is the whole point of a chain:
# a failure that does not say where it happened costs more than the requests saved.
ADVICE = {
    'documents': 'the spec was refused. The message names the file, the line and the '
                 'rule: fix the spec, not the XML. "python3 {tools}/explain_rule.py '
                 '<number>" explains a rule by its number.',
    'application': 'the documents generated, but the application could not be written '
                   'from them. The message names what the document lacks.',
    'contract': 'the sources break a rule of docs/agent/api.json. Every finding names '
                'the file, the line and the rule. Fix them before building.',
    'configure': 'CMake could not configure. The generator runs here, so a refused '
                 'document appears in this output; anything else is CMakeLists.txt.',
    'scenarios': 'the application built, but a scenario did not pass. Each failure '
                 'names the process, what it was expected to print and what it '
                 'wrote. "--only <name>" iterates on one.',
    'final': 'the final pass does not allow an open marker. A passing scenario says '
             'nothing about the requirement behind one: no body was written for it. '
             'Fill it, then run this again.',
    'build': 'the compiler refused a source. The errors are above, each with the '
             'line it is on: no second command is needed to see them. Fix the body '
             'in bodies.txt, not the generated file, and run this again. A '
             'provider that is abstract means the document gained a request the '
             'application has no handler for: add the handler, or --regenerate and '
             'fill the markers again.',
}


def fail(message):
    # Output already printed is flushed first: stdout is block-buffered into a
    # pipe, so without this the error reaches the reader before the lines it is
    # about.
    sys.stdout.flush()
    sys.stderr.write('error: {}\n'.format(message))
    sys.exit(2)


def show(lines, tail):
    """Prints the last `tail` lines, or all of them when tail is None.

    The start index is clamped: a negative one counts from the end, which showed a
    log of 15 lines asked for 16 as its last line alone, and said nothing about the
    14 it dropped.
    """
    tail = len(lines) if tail is None else tail
    for line in lines[max(0, len(lines) - tail):]:
        print('   ' + line)
    if len(lines) > tail:
        print('   ... {} earlier line(s) not shown'.format(len(lines) - tail))


NOTES_SHOWN = '.notes-shown'
NOTE_LINE = re.compile(r'^(\s*)note  (\S+)')


def collapse_notes(lines, record):
    """The lines with every design note an earlier call printed replaced by one count.

    A note is its "note" line and the more indented lines under it. The digests of
    the notes printed are kept in `record` for the next call.
    """
    try:
        with open(record, encoding='utf-8') as handle:
            shown = set(handle.read().split())
    except OSError:
        shown = set()
    kept, printed, index, collapsed, where = [], [], 0, 0, None
    while index < len(lines):
        head = NOTE_LINE.match(lines[index])
        if not head:
            kept.append(lines[index])
            index += 1
            continue
        end = index + 1
        while end < len(lines) and lines[end].strip() and \
                len(lines[end]) - len(lines[end].lstrip()) > len(head.group(1)):
            end += 1
        block = '\n'.join(lines[index:end])
        digest = hashlib.sha256(block.encode('utf-8')).hexdigest()[:16]
        printed.append(digest)
        if digest in shown:
            if where is None:
                where = (len(kept), head.group(1))
                kept.append('')
            collapsed += 1
        else:
            kept.extend(lines[index:end])
        index = end
    if where is not None:
        kept[where[0]] = '{}note  {} design note(s) unchanged since the last call, not repeated' \
            .format(where[1], collapsed)
    try:
        os.makedirs(os.path.dirname(record), exist_ok=True)
        with open(record, 'w', encoding='utf-8') as handle:
            handle.write('\n'.join(printed) + '\n')
    except OSError:
        pass
    return kept


# What a failure is worth printing: the line that names the defect. A build log ends
# with the summary of the tool that gave up, so its last lines carry no diagnostic.
DIAGNOSTIC = re.compile(
    r'(^|[\s:])(error|fatal error|undefined reference|undefined symbol'
    r'|multiple definition)\b'
    r'|^Traceback \(most recent call last\)'
    r'|^\s*error\[\d+/|^\w*(Error|Exception):|error C\d{4}|LNK\d{4}',
    re.IGNORECASE)

# A line that follows a diagnostic and belongs to it: the offending source, the caret,
# a candidate, a note, a traceback frame.
FOLLOWS = re.compile(r'^\s|^\s*\d+\s*\||note:|candidate|required from|in expansion of')

# A line a tool prints when it gives up, which names no defect.
GIVING_UP = re.compile(r'\*\*\*|^(g?make|ninja|cmake)(\[\d+\])?:|^Error\s*$'
                       r'|recipe for target|Stop\.$', re.IGNORECASE)


def diagnostics(lines, budget, context=6):
    """The lines of a failed log that name the defect, or None if it names none.

    Each match brings the lines under it that belong to it -- the source line, the
    caret, the candidates -- so one finding arrives whole. Matches are taken from
    the first, because a later error is usually a consequence of the first.
    """
    hits = [i for i, line in enumerate(lines)
            if DIAGNOSTIC.search(line) and not GIVING_UP.search(line)]
    if not hits:
        return None
    kept, last = [], -1
    for i in hits:
        if len(kept) >= budget:
            break
        end = i + 1
        while (end < len(lines) and end - i <= context and FOLLOWS.search(lines[end])
               and not DIAGNOSTIC.search(lines[end])):
            end += 1
        start = max(i, last + 1)
        if start > last + 1 and last >= 0:
            kept.append(None)
        kept.extend(lines[start:end])
        last = end - 1
    return kept[:budget]


def show_failure(lines, tail):
    """Prints what a failed step said about the defect, and how much was left out."""
    tail = len(lines) if tail is None else tail
    picked = diagnostics(lines, tail)
    if picked is None:
        show(lines, tail)
        return
    for line in picked:
        print('   ...' if line is None else '   ' + line)
    shown = sum(1 for line in picked if line is not None)
    if shown < len(lines):
        print('   ... {} of {} log line(s) shown: the ones naming an error. '
              'The whole log is the same command without this one.'
              .format(shown, len(lines)))


def run(step, command, cwd, kept=2, failed_kept=40, notes=None):
    """One step of the chain, and whether it passed.

    A step that passed prints its last `kept` lines and nothing more: a build log
    that reaches the conversation is re-sent with every later request. A step that
    failed prints the lines of its log that name an error, up to `failed_kept` of
    them, and what to do about it. With `notes`, the design notes printed by an
    earlier call are replaced by one line that counts them.
    """
    print('== {}: {}'.format(step, ' '.join(command)))
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True)
    lines = ((result.stdout or '') + (result.stderr or '')).splitlines()
    if result.returncode != 0:
        show_failure(lines, failed_kept)
        print('')
        print('FAILED at step "{}", exit {}.'.format(step, result.returncode))
        advice = ADVICE.get(step, '')
        if advice:
            print(advice.format(tools=os.path.dirname(HERE)))
        return False
    if notes:
        lines = collapse_notes(lines, notes)
    show(lines, kept)
    return True


# What update_cmake prints when it takes a document's line out.
PRUNED = re.compile(r'^removed\s+\w+\(\s*\S+\s+(\S+?)\s*\)')


def documents_of(specs, outdir):
    """The .siml, the .fsml and the .dtml the specs name, as paths under outdir."""
    interfaces, machines, shared_types = [], [], []
    for spec in specs:
        document, _skipped = gen_docs.load_spec(spec)
        for entry in document.get('interfaces') or []:
            interfaces.append(os.path.join(outdir, entry['name'] + '.siml'))
        for entry in document.get('machines') or []:
            machines.append(os.path.join(outdir, entry['name'] + '.fsml'))
        shared = document.get('datatypes') or {}
        if shared.get('name'):
            shared_types.append(os.path.join(outdir, shared['name'] + '.dtml'))
    return interfaces, machines, shared_types


def drop_placeholders(root, changed, wanted):
    """Remove a document whose CMake line was just pruned.

    Nothing builds it any more, so it is the scaffold's placeholder and only invites
    a read. The file is removed only when this call removed its line and no spec
    names it, so a document of the caller's own is never touched.
    """
    keep = set(os.path.normcase(os.path.normpath(path)) for path in wanted)
    dropped = []
    for entry in changed or []:
        found = PRUNED.match(entry)
        if not found:
            continue
        relative = found.group(1)
        if os.path.normcase(os.path.normpath(relative)) in keep:
            continue
        path = os.path.join(root, relative)
        if not os.path.isfile(path):
            continue
        os.remove(path)
        dropped.append(relative.replace('\\', '/'))
    return dropped


def mode_of(root, given):
    """ipc or local: what the project was scaffolded as.

    Taken from scenarios.json, which names one process for a local project and two
    for an ipc one, so the mode is never asked for twice.
    """
    if given:
        return given
    try:
        with open(os.path.join(root, 'scenarios.json'), encoding='utf-8') as handle:
            procs = json.load(handle)['scenarios'][0]['procs']
        return 'local' if len(procs) < 2 else 'ipc'
    except (ValueError, OSError, KeyError, IndexError, TypeError):
        return 'ipc'


def app_present(root, document):
    """How much of the application of this document src/ already holds: 0, 1 or 2.

    Counted by the generated base headers the sources include, wherever they are
    and whatever they are called, so a project that renamed or split its files is
    still recognised. A scaffold's sources name another service and count 0.
    """
    stem = os.path.basename(document).rsplit('.', 1)[0]
    wanted = {'{}ProviderBase.hpp'.format(stem), '{}ConsumerBase.hpp'.format(stem)}
    found = set()
    source_root = os.path.join(root, 'src')
    for folder, dirs, files in os.walk(source_root):
        dirs[:] = [d for d in dirs if d not in ('services', 'build')]
        for name in files:
            if not name.endswith(('.cpp', '.hpp', '.h', '.cc', '.cxx')):
                continue
            with open(os.path.join(folder, name), encoding='utf-8', errors='ignore') as handle:
                text = handle.read()
            found |= set(header for header in wanted if header in text)
    return len(found)


APP_STAMP = '.app-design'


def design_digest(specs):
    """One digest of every spec's content, in order."""
    digest = hashlib.sha256()
    for spec in specs:
        with open(spec, 'rb') as handle:
            digest.update(handle.read())
    return digest.hexdigest()


def sources_digest(root):
    """One digest of every application source under src/, by path and content."""
    digest = hashlib.sha256()
    source_root = os.path.join(root, 'src')
    for folder, dirs, files in os.walk(source_root):
        dirs[:] = sorted(d for d in dirs if d not in ('services', 'build'))
        for name in sorted(files):
            if name.endswith(('.cpp', '.hpp', '.h', '.cc', '.cxx')):
                path = os.path.join(folder, name)
                digest.update(os.path.relpath(path, source_root).encode('utf-8'))
                with open(path, 'rb') as handle:
                    digest.update(handle.read())
    return digest.hexdigest()


def read_stamp(root, build):
    """The design digest and the sources digest of the last generation, or None."""
    try:
        with open(os.path.join(root, build, APP_STAMP), encoding='utf-8') as handle:
            lines = handle.read().split()
    except OSError:
        return None, None
    return (lines[0] if lines else None), (lines[1] if len(lines) > 1 else None)


def app_untouched(root, build):
    """True when src/ is exactly what the last generation wrote."""
    _design, sources = read_stamp(root, build)
    return sources is not None and sources == sources_digest(root)


def app_older_than(root, build, specs):
    """True when the design changed after src/ was last generated from it.

    The digest written by the last generation answers it. A build directory without
    one falls back to the times: a spec newer than every application source.
    """
    if not specs:
        return False
    design, _sources = read_stamp(root, build)
    if design is not None:
        return design != design_digest(specs)
    newest = 0.0
    for folder, dirs, files in os.walk(os.path.join(root, 'src')):
        dirs[:] = [d for d in dirs if d not in ('services', 'build')]
        for name in files:
            if name.endswith(('.cpp', '.hpp', '.h', '.cc', '.cxx')):
                newest = max(newest, os.path.getmtime(os.path.join(folder, name)))
    return newest > 0 and max(os.path.getmtime(spec) for spec in specs) > newest


# The project's own record of which files describe it. Without it a later call
# has no way to know what --spec the first one was given, so it rebuilds the
# application of the previous design and verifies the previous contract.
MANIFEST = 'areg-project.json'


def worksheet_has_code(path):
    """True when the worksheet names a section and carries a line of code under one."""
    try:
        with open(path, encoding='utf-8') as handle:
            lines = handle.read().splitlines()
    except (IOError, OSError):
        return False
    named = False
    for line in lines:
        if line.startswith('#|'):
            continue
        if line.startswith('== '):
            named = True
        elif named and line.strip():
            return True
    return False


def read_manifest(root):
    """The spec paths this project was last built from, as absolute paths."""
    path = os.path.join(root, MANIFEST)
    try:
        with open(path, encoding='utf-8') as handle:
            named = json.load(handle).get('spec') or []
    except (OSError, ValueError):
        return []
    return [spec if os.path.isabs(spec) else os.path.join(root, spec)
            for spec in named]


def write_manifest(root, specs):
    """Records the spec paths, relative to the project when they are inside it."""
    named = []
    for spec in specs:
        inside = os.path.relpath(spec, root)
        named.append(spec if inside.startswith(os.pardir) else inside.replace(os.sep, '/'))
    with open(os.path.join(root, MANIFEST), 'w', encoding='utf-8') as handle:
        json.dump({'spec': named}, handle, indent=2)
        handle.write('\n')


def main():
    parser = argparse.ArgumentParser(
        description='Generate the documents and the application, then configure and '
                    'build the project.')
    parser.add_argument('--spec', action='append', default=[],
                        help='a gen_docs.py JSON description; pass it once per file')
    parser.add_argument('--root', default='.', help='the project root (default: here)')
    parser.add_argument('--outdir', default=os.path.join('src', 'services'),
                        help='where the documents are written')
    parser.add_argument('--doc', help='the .siml the application is built from '
                                      '(default: the first the specs name)')
    parser.add_argument('--machine', help='the .fsml the provider owns '
                                          '(default: the first the specs name)')
    parser.add_argument('--mode', choices=['ipc', 'local'],
                        help='default: read from scenarios.json')
    parser.add_argument('--build', default='build', help='the build directory')
    parser.add_argument('--bodies', default='bodies.txt',
                        help='the bodies applied before the build (default: '
                             'bodies.txt)')
    parser.add_argument('--no-fill', action='store_true',
                        help='do not apply the bodies before the build')
    parser.add_argument('--jobs', type=int, default=DEFAULT_JOBS,
                        help='parallel compile jobs (default: {})'
                             .format(DEFAULT_JOBS))
    parser.add_argument('--run', action='store_true',
                        help='run the scenarios once the build passed, in this '
                             'same call')
    parser.add_argument('--regenerate', action='store_true',
                        help='write the application again, discarding what is in it')
    parser.add_argument('--no-check', action='store_true',
                        help='skip the contract check')
    args = parser.parse_args()

    root = os.path.abspath(args.root)
    if not os.path.isdir(root):
        fail('no such directory: {}'.format(args.root))
    mode = mode_of(root, args.mode)

    # Every input path is resolved once, against the project root, and that one
    # path is what each step uses. The steps do not share a working directory:
    # gen_docs.py runs as a subprocess in the root and documents_of() reads the
    # same files in whichever directory this tool was called from.
    specs = [spec if os.path.isabs(spec) else os.path.join(root, spec)
             for spec in args.spec]
    missing = [spec for spec in specs if not os.path.isfile(spec)]
    if missing:
        fail('no such spec: {}. A relative --spec is resolved against --root {}'
             .format(', '.join(missing), root))
    if specs:
        write_manifest(root, specs)
    else:
        specs = [spec for spec in read_manifest(root) if os.path.isfile(spec)]
        if specs:
            print('== design: {} names {}, so the documents are written from it '
                  'again.'.format(MANIFEST,
                                  ', '.join(os.path.basename(s) for s in specs)))

    if specs:
        command = [PYTHON, os.path.join(HERE, 'gen_docs.py'), '--outdir', args.outdir,
                   '--force', '--chained']
        for spec in specs:
            command += ['--spec', spec]
        if not run('documents', command, root, kept=len(specs) * 8 + 8,
                   notes=os.path.join(root, args.build, NOTES_SHOWN)):
            return 1

    document = args.doc
    machine = args.machine
    if specs and (document is None or machine is None):
        interfaces, machines, _shared = documents_of(specs, args.outdir)
        # The application this tool writes is one service and at most one machine.
        # Picking the first of several silently builds a part of the project and
        # calls it the project, so several are named and refused here instead.
        for what, found, option in (('service', interfaces, '--doc'),
                                    ('state machine', machines, '--machine')):
            if len(found) > 1 and (args.doc if option == '--doc' else args.machine) is None:
                fail('this project describes {} {}s: {}. build_project.py writes one '
                     'application, of one service and at most one machine: name the one '
                     'to build with {}, and write the others with gen_skeleton.py --app '
                     'into their own directories. docs/agent/10-new-project.md has the '
                     'shape.'.format(len(found), what,
                                     ', '.join(os.path.basename(f) for f in found),
                                     option))
        document = document or (interfaces[0] if interfaces else None)
        machine = machine or (machines[0] if machines else None)
    if document is None:
        found = sorted(glob.glob(os.path.join(root, args.outdir, '*.siml')))
        document = os.path.relpath(found[0], root) if found else None
    if document is None:
        fail('no .siml document: pass --spec, or --doc')

    # The documents the spec describes are the ones the project builds, so the CMake
    # lines follow the spec: a new document gains its line and a dropped one loses it.
    if specs:
        interfaces, machines, shared_types = documents_of(specs, args.outdir)
        wanted = [('addServiceInterface', path) for path in interfaces] + \
                 [('addStateMachine', path) for path in machines]
        changed = gen_skeleton.update_cmake(
            os.path.join(root, 'src', 'CMakeLists.txt'),
            documents=[(fn, os.path.normpath(p).replace('\\', '/')) for fn, p in wanted],
            prune=os.path.normpath(args.outdir).replace('\\', '/'))
        for change in changed or []:
            print('   src/CMakeLists.txt: {}'.format(change))
        for path in drop_placeholders(root, changed,
                                      interfaces + machines + shared_types):
            print('   removed {} -- the placeholder document the spec replaced'
                  .format(path))

    present = app_present(root, document)
    stale = present == 2 and app_older_than(root, args.build, specs)
    untouched = stale and app_untouched(root, args.build)
    if untouched and not args.regenerate:
        print('== application: {} changed and src/ holds nothing of yours yet, so it '
              'is written again.'.format(', '.join(os.path.basename(s) for s in specs)))
    if args.regenerate or present == 0 or untouched:
        command = [PYTHON, os.path.join(HERE, 'gen_skeleton.py'), '--doc', document,
                   '--app', '--mode', mode, '--force']
        if machine:
            command += ['--machine', machine]
        for spec in specs:
            command += ['--spec', spec]
        if not run('application', command, root, kept=200):
            return 1
        if specs:
            os.makedirs(os.path.join(root, args.build), exist_ok=True)
            with open(os.path.join(root, args.build, APP_STAMP), 'w',
                      encoding='utf-8') as handle:
                handle.write(design_digest(specs) + '\n' + sources_digest(root) + '\n')
    elif present == 1:
        print('== application: src/ holds only part of the application of {}, so it is '
              'kept as it is.'.format(os.path.basename(document)))
        print('   --regenerate writes the whole application again and discards what is '
              'in it.')
    elif stale:
        print('== application: kept src/ as it is, but {} changed after src/ was generated from it.'
              .format(', '.join(os.path.basename(spec) for spec in specs)))
        print('   A request, action or step added since has no marker in it, and one')
        print('   removed still has. --regenerate writes src/ again from the design and')
        print('   bodies.txt puts every body back.')
    else:
        print('== application: kept src/ as it is. --regenerate writes it again.')

    # bodies.txt is the source of the bodies, so it is applied on every call: a
    # section changed since the last one is written, and a --regenerate that reset the
    # sources gets every body back. Filling the same body twice writes the same file.
    worksheet = os.path.join(root, args.bodies)
    if not args.no_fill and worksheet_has_code(worksheet):
        if not run('worksheet',
                   [PYTHON, os.path.join(HERE, 'fill_markers.py'),
                    '--bodies', args.bodies],
                   root, kept=3):
            return 1

    # The scaffold pass allows an open marker: the generated files are promised to
    # compile and run as written, and a marker is where a body goes. The final pass
    # after the scenarios does not.
    if not args.no_check:
        if not run('contract',
                   [PYTHON, os.path.join(HERE, 'check_contract.py'), '.', '--strict',
                    '--allow-todo'],
                   root, kept=1):
            return 1

    # The first build compiles the framework itself, which is minutes rather than
    # seconds.
    if not os.path.isdir(os.path.join(root, args.build)):
        print('== the first build compiles the framework too, so it takes minutes.')
        print('   A timeout here is the command timeout, not a failure of the build:')
        print('   every step is incremental, so running this again continues from')
        print('   where it stopped. Nothing is lost and nothing is done twice.')
    if not run('configure', ['cmake', '-B', args.build], root, kept=3):
        return 1
    if not run('build',
               ['cmake', '--build', args.build, '-j', str(args.jobs)], root, kept=3):
        return 1

    print('')
    print('generated headers: {}/generate/{}/ -- already on the include path.'
          .format(args.build, args.outdir.replace(os.sep, '/')))
    print('   Include one by the path it mirrors, for example #include "{}/X.hpp".'
          .format(args.outdir.replace(os.sep, '/')))
    print('   Nothing else is written there and none of it is yours to edit.')

    if args.run:
        print('')
        # run_scenarios.py refuses a build older than the newest source, because a
        # run of the previous program reads as a logic fault. That guard is for a
        # hand-run after an edit. Here the build above has just succeeded in this
        # same command, so there is nothing older to refuse: a source whose time
        # moved without its content moving -- a checkout, a copy, a regeneration --
        # leaves the compiler with nothing to relink and the binary with its old
        # time, and the guard would then refuse the build it was given, every time,
        # with no command able to clear it.
        # run_scenarios.py bounds each process's output itself. A failure is only
        # readable whole, and a pass carries the lead's lines the report is written from.
        if not run('scenarios',
                   [PYTHON, os.path.join(HERE, 'run_scenarios.py'),
                    '--build', os.path.join(args.build, 'bin'), '--stale-ok'],
                   root, kept=None, failed_kept=None):
            return 1
        if not args.no_check:
            print('')
            if not run('final',
                       [PYTHON, os.path.join(HERE, 'check_contract.py'), '.',
                        '--strict'],
                       root, kept=1):
                return 1
        print('')
        print('Every step passed, the scenarios included. The lines each scenario printed')
        print('are above: report from them, since a second run prints the same lines.')
        return 0

    print('')
    print('Every step passed. Next, and after every fix, the same command with --run:')
    print('it applies the worksheet, builds, runs the scenarios and the final check:')
    print('  python3 {} {} --run'.format(
        os.path.join(HERE, 'build_project.py'),
        ' '.join('--spec ' + spec for spec in args.spec)).replace('  ', ' '))
    return 0


if __name__ == '__main__':
    sys.exit(main())
