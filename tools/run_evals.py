#!/usr/bin/env python3
# ===========================================================================
# Grades an application an agent built for one of the tasks in
# tools/agent_evals/tasks.json.
#
#   python3 tools/run_evals.py --list
#   python3 tools/run_evals.py --task 04-timer --dir ~/work/tick
#   python3 tools/run_evals.py --self-check --sdk-root .
#
# The task bank is prompts and pass criteria; nothing here runs an agent. An
# agent is given a prompt, writes a project into a directory, and this decides
# whether the result works: it configures, builds, runs and checks the output.
#
# --self-check grades the reference recipes instead, which is how the harness
# itself is verified.
#
# Report a run with --tokens and --hops to record what it cost; the numbers are
# printed back with the verdict and are not otherwise used.
#
# Exit code 0 when every graded task passed, 1 otherwise.
# ===========================================================================
import argparse
import glob
import json
import os
import shutil
import subprocess
import sys
import tempfile

SDK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BANK = os.path.join(SDK, 'tools', 'agent_evals', 'tasks.json')
RECIPES = os.path.join(SDK, 'docs', 'agent', 'recipes')


def load_tasks():
    with open(BANK, encoding='ascii') as handle:
        return json.load(handle)['tasks']


def run(command, cwd=None, timeout=900):
    try:
        return subprocess.run(command, cwd=cwd, capture_output=True, text=True,
                              timeout=timeout)
    except subprocess.TimeoutExpired:
        return None


def build(project, sdk_root):
    """Configures and builds the project the agent produced.

    FETCHCONTENT_SOURCE_DIR_AREG points the project's own FetchContent block at a
    local SDK, so a graded build needs no network and no edit to the project.
    """
    configure = ['cmake', '-B', 'build', '-DCMAKE_BUILD_TYPE=Release']
    if sdk_root:
        configure.append('-DFETCHCONTENT_SOURCE_DIR_AREG=' + os.path.abspath(sdk_root))
    result = run(configure, cwd=project)
    if result is None:
        return False, 'configure timed out'
    if result.returncode != 0:
        return False, 'configure failed: ' + (result.stderr or result.stdout)[-400:]

    result = run(['cmake', '--build', 'build', '-j', str(os.cpu_count() or 2)],
                 cwd=project)
    if result is None:
        return False, 'build timed out'
    if result.returncode != 0:
        return False, 'build failed: ' + (result.stderr or result.stdout)[-400:]
    return True, 'built'


SERVICES = ('mtrouter', 'logcollector', 'logobserver')
NOT_PROGRAMS = ('.so', '.dll', '.dylib', '.a', '.lib', '.json', '.init', '.txt')


def binaries_of(project):
    """The application executables in build/bin.

    Shared libraries carry the executable bit too, so the extension decides, and
    the framework's own services are never the thing under test.
    """
    found = []
    for path in sorted(glob.glob(os.path.join(project, 'build', 'bin', '*'))):
        name = os.path.basename(path)
        if os.path.isdir(path) or not os.access(path, os.X_OK):
            continue
        if name.split('.')[0] in SERVICES:
            continue
        if any(ext in name for ext in NOT_PROGRAMS):
            continue
        found.append(path)
    return found


def grade(task, source, sdk_root):
    """Grades a copy of the project, so nothing is built inside the original."""
    if not os.path.isdir(source):
        return False, 'no such directory: ' + source
    with tempfile.TemporaryDirectory(prefix='areg-eval-') as work:
        project = os.path.join(work, os.path.basename(os.path.abspath(source)))
        shutil.copytree(source, project,
                        ignore=shutil.ignore_patterns('build', '.git'))
        return _grade(task, project, sdk_root)


def _grade(task, project, sdk_root):
    if not os.path.isfile(os.path.join(project, 'CMakeLists.txt')):
        return False, 'no CMakeLists.txt in ' + project

    documents = (glob.glob(os.path.join(project, '**', '*.siml'), recursive=True)
                 + glob.glob(os.path.join(project, '**', '*.fsml'), recursive=True))
    documents = [d for d in documents if os.sep + 'build' + os.sep not in d]
    if not documents:
        return False, 'the project declares no service document'

    ok, note = build(project, sdk_root)
    if not ok:
        return False, note

    found = binaries_of(project)
    wanted = task.get('binaries', 1)
    if len(found) < wanted:
        return False, 'expected {} executable(s) in build/bin, found {}'.format(
            wanted, len(found))

    expect = task.get('expect') or []
    if not expect:
        return True, 'built {} executable(s); no output asserted'.format(len(found))

    result = run([found[0]], cwd=project, timeout=90)
    if result is None:
        return False, 'the application did not finish'
    if result.returncode != 0:
        return False, 'exit code {}'.format(result.returncode)

    output = result.stdout + result.stderr
    missing = [text for text in expect if text not in output]
    if missing:
        return False, 'output did not contain: ' + '; '.join(missing)
    return True, 'built, ran, output matched'


def main():
    parser = argparse.ArgumentParser(
        description='Grade an application built for an agent evaluation task.')
    parser.add_argument('--list', action='store_true', help='show the task bank')
    parser.add_argument('--task', help='the task id to grade')
    parser.add_argument('--dir', help='the project the agent produced')
    parser.add_argument('--sdk-root', default=SDK,
                        help='local SDK to build against (default: this repository)')
    parser.add_argument('--self-check', action='store_true',
                        help='grade the reference recipes instead, to verify the harness')
    parser.add_argument('--tokens', type=int, help='tokens the agent spent, for the report')
    parser.add_argument('--hops', type=int, help='documents the agent opened, for the report')
    args = parser.parse_args()

    tasks = load_tasks()

    if args.list:
        for task in tasks:
            print('{:<18} {}'.format(task['id'], task['teaches']))
        return 0

    if args.self_check:
        failures = 0
        for task in tasks:
            reference = task.get('reference')
            if not reference:
                print('SKIP  {:<18} no reference recipe'.format(task['id']))
                continue
            ok, note = grade(task, os.path.join(RECIPES, reference), args.sdk_root)
            print('{}  {:<18} {}'.format('PASS' if ok else 'FAIL', task['id'], note))
            failures += 0 if ok else 1
        return 1 if failures else 0

    if not args.task or not args.dir:
        parser.error('give --task and --dir, or --list, or --self-check')

    task = next((t for t in tasks if t['id'] == args.task), None)
    if task is None:
        print('no such task: {}. Try --list.'.format(args.task), file=sys.stderr)
        return 1

    ok, note = grade(task, args.dir, args.sdk_root)
    print('{}  {}  {}'.format('PASS' if ok else 'FAIL', task['id'], note))
    if args.tokens or args.hops:
        print('      cost: {} tokens, {} documents opened'.format(
            args.tokens if args.tokens else '-', args.hops if args.hops else '-'))
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
