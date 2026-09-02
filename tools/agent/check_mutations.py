#!/usr/bin/env python3
# ===========================================================================
# Breaks a known-good recipe the way the eval bank says, and asserts that the
# diagnostic the documentation promises actually reports it.
#
#   python3 tools/agent/check_mutations.py --static
#   python3 tools/agent/check_mutations.py --lib build/bin
#   python3 tools/agent/check_mutations.py --lib build/bin --id r01-role-typo
#
# No agent takes part. Every defect and every expected detection is data in
# tools/agent/evals/tasks.json, on the tasks whose kind is "repair".
#
# A rule that quietly stops firing is invisible: the suite stays green because
# nothing was checking the checker. This is what checks it.
#
#   --static  runs only the defects a static checker decides, so it needs no
#             build, no compiler and no library. That is most of them.
#   --lib     adds the defects that need a compiled recipe: a name the compiler
#             must reject, and a process that must hang without the router.
#
# Exit code 0 when every defect was detected, 1 otherwise.
# ===========================================================================
import argparse
import glob
import json
import os
import shutil
import subprocess
import sys
import tempfile

SDK = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
BANK = os.path.join(SDK, 'tools', 'agent', 'evals', 'tasks.json')
RECIPES = os.path.join(SDK, 'docs', 'agent', 'recipes')
NO_ROUTER_SECONDS = 15


def repairs():
    with open(BANK, encoding='ascii') as handle:
        tasks = json.load(handle)['tasks']
    return [t for t in tasks if t.get('kind') == 'repair']


def contract(project):
    """The contract checker's report on a project, as one string."""
    result = subprocess.run(
        [sys.executable, os.path.join(SDK, 'tools', 'agent', 'check_contract.py'), project],
        capture_output=True, text=True)
    return result.stdout + result.stderr


def apply_defect(root, defect):
    """Writes the defect into the copy. Returns an error string, or None."""
    if 'environment' in defect:
        return None
    path = os.path.join(root, defect['file'])
    if 'create' in defect:
        # Some defects are a file that should not be there at all, not an edit to
        # one that should. A generated file copied into the application sources is
        # the standing example: nothing in the sources changes, and the copy stops
        # matching its original at the next build.
        if os.path.isfile(path):
            return 'the file to create is already in the recipe: ' + defect['file']
        directory = os.path.dirname(path)
        if directory and not os.path.isdir(directory):
            os.makedirs(directory)
        with open(path, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write(defect['create'])
        return None
    if not os.path.isfile(path):
        return 'no such file in the recipe: ' + defect['file']
    with open(path, encoding='utf-8') as handle:
        text = handle.read()
    count = text.count(defect['find'])
    if count != 1:
        return 'the text to break appears {} times, not once'.format(count)
    with open(path, 'w', encoding='utf-8', newline='\n') as handle:
        handle.write(text.replace(defect['find'], defect['replace'], 1))
    return None


def by_contract(root, rule, clean_report):
    if rule in clean_report:
        return False, '{} already fires on the unbroken recipe, so it proves ' \
                      'nothing'.format(rule)
    report = contract(root)
    if rule not in report:
        return False, '{} did not fire on the broken copy'.format(rule)
    return True, '{} reported it'.format(rule)


def by_compiler(root, lib, compiler):
    sys.path.insert(0, os.path.join(SDK, 'tools', 'agent'))
    import check_recipes
    ok, _binaries, detail = check_recipes.build_recipe(root, lib, compiler)
    if ok:
        return False, 'the broken copy still compiles, so the compiler is not ' \
                      'the diagnostic for this defect'
    return True, 'the compiler rejected it: ' + detail.split(':')[0]


def by_runtime(root, lib, compiler, task):
    """The defect is the environment, not the source: build it and run it broken.

    The only case so far is a Public service with no router. The consumer is
    expected to wait, not to fail, so the assertion is that it never prints what
    it prints when the router is there, and that it does not exit on its own.

    A router left behind by an earlier checker makes this task report a false
    failure, so the port is proved free before the broken copy is run.
    """
    sys.path.insert(0, os.path.join(SDK, 'tools', 'agent'))
    import check_recipes
    import service_ports

    if not service_ports.require_free([service_ports.ROUTER_PORT]):
        return False, 'port {} is held, so "no router" could not be arranged'.format(
            service_ports.ROUTER_PORT)
    ok, binaries, detail = check_recipes.build_recipe(root, lib, compiler)
    if not ok:
        return False, 'the recipe did not build: ' + detail

    lead = task.get('run', {}).get('lead')
    chosen = None
    for path in binaries:
        if lead and os.path.basename(path) == lead:
            chosen = path
    if chosen is None:
        return False, 'no binary named {} was built'.format(lead)

    try:
        result = subprocess.run([chosen], cwd=root, capture_output=True, text=True,
                                timeout=NO_ROUTER_SECONDS)
        output = result.stdout + result.stderr
    except subprocess.TimeoutExpired as expired:
        output = (expired.stdout or b'').decode('utf-8', 'replace') \
            + (expired.stderr or b'').decode('utf-8', 'replace')
        for line in task['detect'].get('absent', []):
            if line in output:
                return False, 'printed {!r} with no router running'.format(line)
        return True, 'waited for the router instead of failing, as documented'

    for line in task['detect'].get('absent', []):
        if line in output:
            return False, 'printed {!r} with no router running'.format(line)
    return True, 'exited without doing the work, and printed none of it'


def verify(task, work, lib, compiler, clean_reports):
    reference = task['reference']
    source = os.path.join(RECIPES, reference)
    if not os.path.isdir(source):
        return False, 'no such recipe: ' + reference

    if reference not in clean_reports:
        base = os.path.join(work, 'clean-' + reference)
        if not os.path.isdir(base):
            shutil.copytree(source, base)
        clean_reports[reference] = contract(base)

    root = os.path.join(work, task['id'])
    shutil.copytree(source, root)
    error = apply_defect(root, task['defect'])
    if error:
        return False, error

    how = task['detect']['by']
    if how == 'check_contract':
        return by_contract(root, task['detect']['rule'], clean_reports[reference])
    if how == 'compiler':
        return by_compiler(root, lib, compiler)
    if how == 'runtime':
        return by_runtime(root, lib, compiler, task)
    return False, 'unknown detection: ' + how


NEEDS_BUILD = ('compiler', 'runtime')


def main():
    parser = argparse.ArgumentParser(
        description='Assert that every documented diagnostic still fires.')
    parser.add_argument('--lib', help='directory holding libareg; enables the '
                                      'defects that need a compiled recipe')
    parser.add_argument('--static', action='store_true',
                        help='only the defects a static checker decides')
    parser.add_argument('--compiler', default=os.environ.get('CXX', 'g++'))
    parser.add_argument('--id', help='run one repair task by id')
    args = parser.parse_args()

    lib = os.path.abspath(args.lib) if args.lib else None
    if lib and not glob.glob(os.path.join(lib, 'libareg*')):
        sys.stderr.write('error: no libareg under {}\n'.format(lib))
        return 1

    tasks = repairs()
    if args.id:
        tasks = [t for t in tasks if t['id'] == args.id]
        if not tasks:
            sys.stderr.write('error: no repair task called {}\n'.format(args.id))
            return 1
    if not tasks:
        sys.stderr.write('error: the task bank holds no repair task\n')
        return 1

    work = tempfile.mkdtemp(prefix='areg-mutations-')
    clean_reports = {}
    failures = 0
    skipped = 0
    try:
        for task in tasks:
            how = task['detect']['by']
            if how in NEEDS_BUILD and (args.static or lib is None):
                print('{:5} {:22} {}'.format('SKIP', task['id'],
                                             'needs a built recipe; pass --lib'))
                skipped += 1
                continue
            passed, detail = verify(task, work, lib, args.compiler, clean_reports)
            print('{:5} {:22} {}'.format('PASS' if passed else 'FAIL',
                                         task['id'], detail))
            failures += 0 if passed else 1
    finally:
        shutil.rmtree(work, ignore_errors=True)

    print('{} checked, {} failed, {} skipped'.format(
        len(tasks) - skipped, failures, skipped))
    return 1 if failures else 0


if __name__ == '__main__':
    sys.exit(main())
