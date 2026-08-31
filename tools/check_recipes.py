#!/usr/bin/env python3
# ===========================================================================
# Builds every recipe under docs/agent/recipes/ and runs the ones that end by
# themselves. A recipe that no longer compiles is worse than no recipe: an
# agent copies it and then debugs the copy.
#
#   python3 tools/check_recipes.py --lib build/bin
#
# The framework has to be built first, for example:
#   cmake -B build -DAREG_TESTS=OFF -DAREG_EXAMPLES=OFF && cmake --build build -j
#
# Exit code 0 when every recipe passed, 1 otherwise.
# ===========================================================================
import argparse
import glob
import os
import shutil
import socket
import subprocess
import sys
import tempfile
import time

SDK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RECIPES = os.path.join(SDK, 'docs', 'agent', 'recipes')
# Recipes whose processes end on their own, so their output can be checked.
RUNNABLE = {'01-local-single-process': ['provider: hello,', 'consumer: greeted'],
            '03-attributes-and-broadcast': ['consumer: alarm at 45',
                                            'consumer: measurement taken'],
            '04-timer': ['consumer: tick 1', 'consumer: tick 3'],
            '05-two-services': ['collector: reading 42', 'display: report 42'],
            '06-state-machine': ['provider: gate open', 'consumer: gate is closed']}

# Recipes whose processes talk through mtrouter. The provider is started first and
# left running; the consumer is the one whose output is checked.
MULTIPROCESS = {'02-ipc-two-processes': (['provider'], 'consumer',
                                         ['consumer: greeted'])}

ROUTER_PORT = 8181
ROUTER_READY_SECONDS = 15.0


def run(command, cwd=None):
    return subprocess.run(command, cwd=cwd, capture_output=True, text=True)


def wait_router_ready(timeout=ROUTER_READY_SECONDS):
    """Waits until the router accepts a connection, rather than for a fixed time."""
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


def run_multiprocess(root, binaries, spec, lib):
    """Runs a recipe whose processes reach each other through mtrouter.

    The router is started with --service, the unattended mode. The readiness poll
    also catches a router that could not bind because another one holds the port.
    """
    background, lead, expected = spec
    router_bin = None
    for candidate in (os.path.join(lib, 'mtrouter.elf'),
                      os.path.join(lib, 'mtrouter'),
                      os.path.join(lib, 'mtrouter.exe')):
        if os.path.isfile(candidate):
            router_bin = candidate
            break
    if router_bin is None:
        # Never a pass: this is the one recipe with a history of failing silently,
        # and a green suite that skipped it is worse than a red one.
        return False, 'mtrouter not found beside the library, so the IPC recipe ' \
                      'was not run; point --lib at a directory that has it'

    by_name = {os.path.basename(b): b for b in binaries}
    if lead not in by_name or any(b not in by_name for b in background):
        return False, 'expected binaries {} and {}'.format(background, lead)

    router = subprocess.Popen([router_bin, '--service'], cwd=root,
                              stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    handles = []
    try:
        if not wait_router_ready():
            return False, 'mtrouter did not listen on port {}'.format(ROUTER_PORT)
        for name in background:
            handles.append(subprocess.Popen([by_name[name]], cwd=root,
                                            stdout=subprocess.PIPE,
                                            stderr=subprocess.STDOUT, text=True))
        time.sleep(1.0)
        try:
            result = subprocess.run([by_name[lead]], cwd=root, capture_output=True,
                                    text=True, timeout=60)
        except subprocess.TimeoutExpired:
            return False, '{} did not finish within 60s'.format(lead)
        if result.returncode != 0:
            return False, '{} exited {}'.format(lead, result.returncode)
        output = result.stdout + result.stderr
        for line in expected:
            if line not in output:
                return False, 'output missing {!r}'.format(line)
        return True, 'built and ran through mtrouter'
    finally:
        for handle in handles:
            handle.terminate()
        router.terminate()


def check(recipe, work, lib, compiler):
    name = os.path.basename(recipe)
    root = os.path.join(work, name)
    shutil.copytree(recipe, root)

    documents = sorted(glob.glob(os.path.join(root, 'src', 'services', '*.siml'))
                       + glob.glob(os.path.join(root, 'src', 'services', '*.fsml')))
    if not documents:
        return False, 'no .siml or .fsml document'
    for document in documents:
        result = run(['java', '-jar', os.path.join(SDK, 'tools', 'codegen.jar'),
                      '--root=' + root, '--doc=' + os.path.relpath(document, root),
                      '--target=generated'], cwd=root)
        if result.returncode != 0:
            return False, 'generator failed on {}: {}'.format(
                os.path.basename(document), (result.stderr or result.stdout)[-300:])

    generated = glob.glob(os.path.join(root, 'generated', 'src', 'services',
                                       'private', '*.cpp'))
    binaries = []
    for source in sorted(glob.glob(os.path.join(root, 'src', '*.cpp'))):
        output = os.path.join(root, os.path.basename(source)[:-4])
        result = run([compiler, '-std=c++17', '-O0',
                      '-I', os.path.join(SDK, 'framework'),
                      '-I', os.path.join(root, 'generated'),
                      source] + generated +
                     ['-o', output, '-L', lib, '-lareg',
                      '-Wl,-rpath,' + lib, '-pthread'])
        if result.returncode != 0:
            return False, 'build of {} failed: {}'.format(
                os.path.basename(source), result.stderr[-400:])
        binaries.append(output)

    if name in MULTIPROCESS:
        return run_multiprocess(root, binaries, MULTIPROCESS[name], lib)

    expected = RUNNABLE.get(name)
    if expected is None:
        return True, 'built ({} binaries), not run'.format(len(binaries))

    try:
        result = subprocess.run([binaries[0]], cwd=root, capture_output=True,
                                text=True, timeout=60)
    except subprocess.TimeoutExpired:
        return False, 'did not finish within 60s'
    if result.returncode != 0:
        return False, 'exited {}'.format(result.returncode)
    output = result.stdout + result.stderr
    for line in expected:
        if line not in output:
            return False, 'output missing {!r}'.format(line)
    return True, 'built and ran'


def main():
    parser = argparse.ArgumentParser(description='Build and run the recipes.')
    parser.add_argument('--lib', default=os.path.join('build', 'bin'),
                        help='directory holding libareg (default: build/bin)')
    parser.add_argument('--compiler', default=os.environ.get('CXX', 'g++'))
    parser.add_argument('--keep', action='store_true', help='keep the work directory')
    args = parser.parse_args()

    lib = os.path.abspath(args.lib)
    if not glob.glob(os.path.join(lib, 'libareg*')):
        sys.stderr.write('error: no libareg under {}; build the framework first\n'.format(lib))
        return 1

    work = tempfile.mkdtemp(prefix='areg-recipes-')
    failures = 0
    try:
        for recipe in sorted(glob.glob(os.path.join(RECIPES, '*'))):
            if not os.path.isdir(recipe):
                continue
            passed, detail = check(recipe, work, lib, args.compiler)
            print('{:5} {:32} {}'.format('PASS' if passed else 'FAIL',
                                         os.path.basename(recipe), detail))
            failures += 0 if passed else 1
    finally:
        if not args.keep:
            shutil.rmtree(work, ignore_errors=True)
        else:
            print('work directory: ' + work)

    return 1 if failures else 0


if __name__ == '__main__':
    sys.exit(main())
