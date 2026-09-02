#!/usr/bin/env python3
# ===========================================================================
# Grades an application an agent built for one of the tasks in
# tools/agent/evals/tasks.json.
#
#   python3 tools/agent/run_evals.py --list
#   python3 tools/agent/run_evals.py --task 04-timer --dir ~/work/tick
#   python3 tools/agent/run_evals.py --self-check --sdk-root .
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
import socket
import subprocess
import sys
import tempfile
import time

SDK = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
BANK = os.path.join(SDK, 'tools', 'agent', 'evals', 'tasks.json')
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


ROUTER_PORT = 8181


def router_of(project):
    """The router built beside the project, or None."""
    for name in ('mtrouter.elf', 'mtrouter', 'mtrouter.exe'):
        candidate = os.path.join(project, 'build', 'bin', name)
        if os.path.isfile(candidate):
            return candidate
    return None


def wait_router_ready(timeout=15.0):
    deadline = time.time() + timeout
    while time.time() < deadline:
        probe = socket.socket()
        probe.settimeout(0.5)
        try:
            probe.connect(('127.0.0.1', ROUTER_PORT))
            return True
        except OSError:
            time.sleep(0.2)
        finally:
            probe.close()
    return False


def run_processes(task, project, found):
    """Runs a task whose processes reach each other through the router.

    The shape is the task's own "run" block: which executable leads, which run in
    the background, and whether a router is needed. The lead is the one whose
    output is checked.
    """
    spec = task['run']

    def named(role):
        """The executable playing this role.

        A recipe compiled straight from src/ is called consumer; the same recipe
        built through CMake is called hello_consumer, because the project names its
        targets. Both answer to the role the task names.
        """
        for path in found:
            stem = os.path.basename(path).split('.')[0]
            if stem == role or stem.endswith('_' + role):
                return path
        return None

    lead = named(spec['lead'])
    if lead is None:
        return False, 'no executable called {} was built'.format(spec['lead'])

    router = None
    if spec.get('router'):
        router_bin = router_of(project)
        if router_bin is None:
            return False, 'mtrouter was not built beside the project'
        router = subprocess.Popen([router_bin, '--service'], cwd=project,
                                  stdout=subprocess.DEVNULL,
                                  stderr=subprocess.DEVNULL)
        if not wait_router_ready():
            router.terminate()
            return False, 'mtrouter did not listen on port {}'.format(ROUTER_PORT)

    handles = []
    try:
        for name in spec.get('background', []):
            path = named(name)
            if path is None:
                return False, 'no executable called {} was built'.format(name)
            handles.append(subprocess.Popen([path], cwd=project,
                                            stdout=subprocess.DEVNULL,
                                            stderr=subprocess.DEVNULL))
        time.sleep(1.0)
        result = run([lead], cwd=project, timeout=90)
        if result is None:
            return False, '{} did not finish'.format(spec['lead'])
        if result.returncode != 0:
            return False, '{} exited {}'.format(spec['lead'], result.returncode)
        output = result.stdout + result.stderr
        missing = [text for text in task.get('expect', []) if text not in output]
        if missing:
            return False, 'output did not contain: ' + '; '.join(missing)
        return True, 'built, ran through {} processes, output matched'.format(
            1 + len(handles))
    finally:
        for handle in handles:
            handle.terminate()
        if router is not None:
            router.terminate()


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

    if task.get('run'):
        return run_processes(task, project, found)

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
        # Several tasks answer to one recipe -- every repair task shares the recipe
        # it is the repair of. Grading is a full configure and build, so the same
        # (recipe, criteria) pair is graded once and the verdict reused.
        failures = 0
        seen = {}
        for task in tasks:
            reference = task.get('reference')
            if not reference:
                print('SKIP  {:<22} no reference recipe'.format(task['id']))
                continue
            key = (reference, tuple(task.get('expect') or []),
                   task.get('binaries', 1), json.dumps(task.get('run'), sort_keys=True))
            if key in seen:
                ok, note = seen[key]
                note += ' (same criteria as {})'.format(key[0])
            else:
                ok, note = grade(task, os.path.join(RECIPES, reference), args.sdk_root)
                seen[key] = (ok, note)
            print('{}  {:<22} {}'.format('PASS' if ok else 'FAIL', task['id'], note))
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
