#!/usr/bin/env python3
# ===========================================================================
# Runs the mechanical chain of a project on areg, in one command:
#
#   documents  ->  application  ->  contract check  ->  configure  ->  build
#
# None of those five steps carries a decision, and each costs a request of its
# own. This runs them in order and stops at the first failure, naming the step,
# the command it ran and what to do about it.
#
#   python3 tools/agent/build_project.py --spec design.json
#   python3 tools/agent/build_project.py --spec design.json --regenerate
#
# The application is written once. A later run keeps the sources as they are and
# only builds them, so filled-in TODO(you) markers are never overwritten;
# --regenerate asks for them back, and discards what is in them.
#
# Exit code 0 when every step passed, 1 otherwise.
# ===========================================================================
import argparse
import glob
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
PYTHON = sys.executable or 'python3'

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
    'build': 'the compiler refused a source. Ask for the errors alone, never the whole '
             'log: "cmake --build build 2>&1 | grep -E \'error\' | head -20". A '
             'provider that is abstract means the document gained a request the '
             'application has no handler for: add the handler, or --regenerate and '
             'fill the markers again.',
}


def fail(message):
    sys.stderr.write('error: {}\n'.format(message))
    sys.exit(2)


def show(lines, tail):
    for line in lines[-tail:]:
        print('   ' + line)
    if len(lines) > tail:
        print('   ... {} earlier line(s) not shown'.format(len(lines) - tail))


def run(step, command, cwd, kept=2):
    """One step of the chain, and whether it passed.

    A step that passed prints its last `kept` lines and nothing more: a build log
    that reaches the conversation is re-sent with every later request. A step that
    failed prints what it said, and what to do about it.
    """
    print('== {}: {}'.format(step, ' '.join(command)))
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True)
    lines = ((result.stdout or '') + (result.stderr or '')).splitlines()
    if result.returncode != 0:
        show(lines, 40)
        print('')
        print('FAILED at step "{}", exit {}.'.format(step, result.returncode))
        print(ADVICE[step].format(tools=os.path.dirname(HERE)))
        return False
    show(lines, kept)
    return True


def documents_of(specs, outdir):
    """The .siml and the .fsml the specs name, as paths under outdir."""
    interfaces, machines = [], []
    for spec in specs:
        try:
            with open(spec, encoding='utf-8') as handle:
                document = json.load(handle)
        except (ValueError, OSError) as error:
            fail('cannot read {}: {}'.format(spec, error))
        for entry in document.get('interfaces') or []:
            interfaces.append(os.path.join(outdir, entry['name'] + '.siml'))
        for entry in document.get('machines') or []:
            machines.append(os.path.join(outdir, entry['name'] + '.fsml'))
    return interfaces, machines


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


def app_matches(root, mode, document):
    """True when src/ already holds the application of this document.

    A scaffolded project starts with the recipe's own sources, which name a
    different service. Those are written over; an application already generated
    from this document is kept, because its TODO(you) markers have been filled in.
    """
    names = ['main.cpp'] if mode == 'local' else ['provider.cpp', 'consumer.cpp']
    stem = os.path.basename(document).rsplit('.', 1)[0]
    wanted = ('{}ProviderBase.hpp'.format(stem), '{}ConsumerBase.hpp'.format(stem))
    for name in names:
        path = os.path.join(root, 'src', name)
        if not os.path.exists(path):
            return False
        with open(path, encoding='utf-8', errors='ignore') as handle:
            text = handle.read()
        if not any(header in text for header in wanted):
            return False
    return True


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
    parser.add_argument('--jobs', type=int, default=os.cpu_count() or 4)
    parser.add_argument('--regenerate', action='store_true',
                        help='write the application again, discarding what is in it')
    parser.add_argument('--no-check', action='store_true',
                        help='skip the contract check')
    args = parser.parse_args()

    root = os.path.abspath(args.root)
    if not os.path.isdir(root):
        fail('no such directory: {}'.format(args.root))
    mode = mode_of(root, args.mode)

    if args.spec:
        command = [PYTHON, os.path.join(HERE, 'gen_docs.py'), '--outdir', args.outdir,
                   '--force']
        for spec in args.spec:
            command += ['--spec', spec]
        if not run('documents', command, root, kept=len(args.spec) * 8 + 8):
            return 1

    document = args.doc
    machine = args.machine
    if args.spec and (document is None or machine is None):
        interfaces, machines = documents_of(args.spec, args.outdir)
        document = document or (interfaces[0] if interfaces else None)
        machine = machine or (machines[0] if machines else None)
    if document is None:
        found = sorted(glob.glob(os.path.join(root, args.outdir, '*.siml')))
        document = os.path.relpath(found[0], root) if found else None
    if document is None:
        fail('no .siml document: pass --spec, or --doc')

    if args.regenerate or not app_matches(root, mode, document):
        command = [PYTHON, os.path.join(HERE, 'gen_skeleton.py'), '--doc', document,
                   '--app', '--mode', mode, '--force']
        if machine:
            command += ['--machine', machine]
        if not run('application', command, root, kept=200):
            return 1
    else:
        print('== application: kept src/ as it is. --regenerate writes it again.')

    if not args.no_check:
        if not run('contract',
                   [PYTHON, os.path.join(HERE, 'check_contract.py'), '.', '--strict'],
                   root, kept=1):
            return 1

    # The first build compiles the framework itself, which is minutes rather than
    # seconds. Saying so before it starts is what stops the wait being read as a hang.
    if not os.path.isdir(os.path.join(root, args.build)):
        print('== the first build compiles the framework too, so it takes minutes.')
        print('   Give this command a timeout of at least 15 minutes. A shorter one')
        print('   reports a timeout or moves the command to the background, and')
        print('   neither is a failure of the build.')
        print('   Every step is incremental: if this is interrupted, run it again and')
        print('   it continues from where it stopped. Nothing is lost and nothing is')
        print('   done twice.')
    if not run('configure', ['cmake', '-B', args.build], root, kept=3):
        return 1
    if not run('build',
               ['cmake', '--build', args.build, '-j', str(args.jobs)], root, kept=3):
        return 1

    print('')
    print('Every step passed. Run the scenarios next:')
    print('  python3 {} --build {}/bin'
          .format(os.path.join(HERE, 'run_scenarios.py'), args.build))
    return 0


if __name__ == '__main__':
    sys.exit(main())
