#!/usr/bin/env python3
# ===========================================================================
# Builds every recipe under docs/agent/recipes/ and runs the ones that end by
# themselves. A recipe that no longer compiles is worse than no recipe: an
# agent copies it and then debugs the copy.
#
#   python3 tools/agent/check_recipes.py --lib build/bin
#
# The framework has to be built first, for example:
#   cmake -B build -DAREG_TESTS=OFF -DAREG_EXAMPLES=OFF && cmake --build build -j$(nproc)
#
# Exit code 0 when every recipe passed, 1 otherwise.
# ===========================================================================
import argparse
import glob
import json
import os
import shutil
import subprocess
import sys
import tempfile
import time

SDK = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(SDK, 'tools', 'agent'))
import service_ports

RECIPES = os.path.join(SDK, 'docs', 'agent', 'recipes')
# Recipes whose processes end on their own, so their output can be checked.
RUNNABLE = {'01-local-single-process': ['provider: hello,', 'consumer: greeted'],
            '03-attributes-and-broadcast': ['consumer: alarm at 45',
                                            'consumer: measurement taken'],
            '04-timer': ['consumer: tick 1', 'consumer: tick 3'],
            '05-two-services': ['collector: reading 42', 'display: report 42'],
            '06-state-machine': ['provider: gate opening',
                                 'consumer: opening ran 1, so resume re-entered open',
                                 'consumer: opening ran 2, so a fresh order started at the beginning',
                                 'provider: gate refused 250',
                                 'provider: machine finished in GATE_DONE'],
            '07-worker-events': ['scanner: asked for depth 3',
                                 'worker: scanned depth 3',
                                 'scanner: 3 steps arrived in 12 bytes',
                                 'client: found 14'],
            '09-shared-types': ['collector: reading 42 from t-1',
                                'collector: quality Good, firmware 1.2.3',
                                'collector: history holds 1',
                                'collector: kept 42 from t-1',
                                'display: report 42 from t-1',
                                'display: readings crossed the boundary: 1'],
            '10-runtime-model': ['provider: hello, Consumer1', 'consumer: greeted 3'],
            '11-monitored-workers': ['monitor: alarm from Worker3',
                                     'operator: 2 of 3 alive'],
            '12-testing': ['a scripted success reaches the consumer ... PASS',
                           'a scripted refusal reaches the consumer ... PASS',
                           'tests: 2 passed, 0 failed'],
            '13-submachine': ['blink: warmup lit', 'blink: signal dark',
                              'lamp: finished in LAMP_DONE']}

# Recipes whose processes talk through mtrouter. The provider is started first and
# left running; the consumer is the one whose output is checked.
MULTIPROCESS = {'02-ipc-two-processes': (['provider'], 'consumer',
                                         ['consumer: greeted'])}

ROUTER_PORT = service_ports.ROUTER_PORT
ROUTER_READY_SECONDS = 15.0


def run(command, cwd=None):
    return subprocess.run(command, cwd=cwd, capture_output=True, text=True)


def wait_router_ready(timeout=ROUTER_READY_SECONDS):
    """Waits until the router accepts a connection, rather than for a fixed time."""
    return service_ports.wait_listening(ROUTER_PORT, timeout)


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

    if not service_ports.require_free([ROUTER_PORT]):
        return False, 'port {} was already held, so the recipe would have been ' \
                      'measured against another router'.format(ROUTER_PORT)

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
        service_ports.stop(handles + [router], [ROUTER_PORT])


def build_recipe(root, lib, compiler):
    """Generates and compiles a recipe already copied into `root`.

    Returns (ok, binaries, detail). Reused by tools/agent/check_mutations.py, which needs
    the same build and needs it to be allowed to fail.
    """
    documents = sorted(glob.glob(os.path.join(root, 'src', 'services', '*.siml'))
                       + glob.glob(os.path.join(root, 'src', 'services', '*.fsml')))
    if not documents:
        return False, [], 'no .siml or .fsml document'
    for document in documents:
        result = run(['java', '-jar', os.path.join(SDK, 'tools', 'codegen.jar'),
                      '--root=' + root, '--doc=' + os.path.relpath(document, root),
                      '--target=generated'], cwd=root)
        if result.returncode != 0:
            return False, [], 'generator failed on {}: {}'.format(
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
            return False, binaries, 'build of {} failed: {}'.format(
                os.path.basename(source), result.stderr[-400:])
        binaries.append(output)
    return True, binaries, 'built {} binaries'.format(len(binaries))


def check(recipe, work, lib, compiler, repeat=1):
    """Build a recipe once, then run it `repeat` times.

    An intermittent defect -- a broadcast that overtakes its subscription, two
    threads inside one chained <<, a quit that beats a message across the router --
    is found only by running the same binary again. Once is not evidence.
    """
    name = os.path.basename(recipe)
    root = os.path.join(work, name)
    shutil.copytree(recipe, root)

    ok, binaries, detail = build_recipe(root, lib, compiler)
    if not ok:
        return False, detail

    for attempt in range(1, repeat + 1):
        passed, detail = run_once(name, root, binaries, lib)
        if not passed:
            return False, ('{} (run {} of {})'.format(detail, attempt, repeat)
                           if repeat > 1 else detail)
    return True, (detail + ', {} runs'.format(repeat) if repeat > 1 else detail)


# Designs gen_skeleton.py --app writes a whole application for, as (label, mode, spec,
# runs). A spec of None is the example gen_docs.py prints. Only a local application
# ends by itself, so only that one is run.
APP_SHAPES = [
    ('app: one request, no steps', 'local',
     {'interfaces': [{'name': 'Sensor',
                      'attributes': [{'name': 'Level', 'type': 'uint32'}],
                      'requests': [{'name': 'read',
                                    'answer': [{'name': 'value', 'type': 'uint32'}]}],
                      'broadcasts': [{'name': 'alarm'}]}]}, True),
    ('app: one request, no steps, ipc', 'ipc',
     {'interfaces': [{'name': 'Sensor',
                      'requests': [{'name': 'read'}]}]}, False),
    ('app: attributes spelled with underscores', 'local',
     {'interfaces': [{'name': 'Sensor',
                      'requests': [{'name': 'read',
                                    'answer': [{'name': 'value', 'type': 'uint32'}]}],
                      'attributes': [{'name': 'my_Value', 'type': 'uint32'},
                                     {'name': '_Private', 'type': 'uint32'}]}]}, True),
    ('app: one request, no response', 'local',
     {'interfaces': [{'name': 'Sensor', 'requests': [{'name': 'read'}]}]}, True),
    ('app: attributes only', 'local',
     {'interfaces': [{'name': 'Sensor',
                      'attributes': [{'name': 'Level', 'type': 'uint32'}]}]}, True),
    ('app: broadcasts only', 'local',
     {'interfaces': [{'name': 'Sensor', 'broadcasts': [{'name': 'alarm'}]}]}, True),
    ('app: example, steps and a machine', 'ipc', None, False),
    ('app: example, an embedded condition', 'ipc', lambda example: embedded(example), False),
]


def embedded(example):
    """The example design with its first condition written as an Embedded body."""
    condition = example['machines'][0]['conditions'][0]
    condition['implement'] = 'Embedded'
    condition['body'] = 'return true;'
    return example


def check_app_shape(label, mode, spec, runs, work, lib, compiler, tools=None):
    """Writes the application of one design the way build_project.py does, then builds
    each of its programs and runs a local one. Returns (ok, detail)."""
    tools = tools or os.path.join(SDK, 'tools', 'agent')
    root = os.path.join(work, label.replace(':', '').replace(',', '').replace(' ', '-'))
    os.makedirs(os.path.join(root, 'src', 'services'))
    if spec is None or callable(spec):
        printed = run([sys.executable, os.path.join(tools, 'gen_docs.py'), '--example'])
        spec = json.loads(printed.stdout) if spec is None else spec(json.loads(printed.stdout))
    with open(os.path.join(root, 'design.json'), 'w', encoding='utf-8', newline='\n') as handle:
        json.dump(spec, handle)
    steps = [[sys.executable, os.path.join(tools, 'gen_docs.py'), '--spec', 'design.json',
              '--outdir', 'src/services', '--force', '--chained']]
    for step in steps:
        result = run(step, cwd=root)
        if result.returncode != 0:
            return False, 'gen_docs.py refused the design: ' + result.stderr[-300:]
    services = sorted(glob.glob(os.path.join(root, 'src', 'services', '*.siml')))
    machines = sorted(glob.glob(os.path.join(root, 'src', 'services', '*.fsml')))
    command = [sys.executable, os.path.join(tools, 'gen_skeleton.py'),
               '--doc', os.path.relpath(services[0], root), '--app', '--mode', mode,
               '--force', '--spec', 'design.json']
    if machines:
        command += ['--machine', os.path.relpath(machines[0], root)]
    result = run(command, cwd=root)
    if result.returncode != 0:
        return False, 'gen_skeleton.py --app failed: ' + result.stderr[-300:]
    for document in services + machines:
        result = run(['java', '-jar', os.path.join(SDK, 'tools', 'codegen.jar'),
                      '--root=' + root, '--doc=' + os.path.relpath(document, root),
                      '--target=generated'], cwd=root)
        if result.returncode != 0:
            return False, 'codegen.jar refused {}: {}'.format(
                os.path.basename(document), (result.stderr or result.stdout)[-300:])
    generated = glob.glob(os.path.join(root, 'generated', 'src', 'services', 'private',
                                       '*.cpp'))
    programs = [os.path.join(root, 'src')] if mode == 'local' else \
        [os.path.join(root, 'src', 'provider'), os.path.join(root, 'src', 'consumer')]
    binaries = []
    for program in programs:
        output = program + '.elf'
        result = run([compiler, '-std=c++17', '-O0',
                      '-I', os.path.join(SDK, 'framework'),
                      '-I', os.path.join(root, 'generated')]
                     + sorted(glob.glob(os.path.join(program, '*.cpp'))) + generated
                     + ['-o', output, '-L', lib, '-lareg', '-Wl,-rpath,' + lib, '-pthread'])
        if result.returncode != 0:
            return False, 'the generated {} does not compile: {}'.format(
                os.path.relpath(program, root), result.stderr[-400:])
        binaries.append(output)
    if not runs:
        return True, 'built {} program(s) as written'.format(len(binaries))
    try:
        result = subprocess.run([binaries[0]], cwd=root, capture_output=True, text=True,
                                timeout=60)
    except subprocess.TimeoutExpired:
        return False, 'built, and did not end within 60s'
    if result.returncode != 0:
        return False, 'built, and exited {}'.format(result.returncode)
    return True, 'built and ran as written'


def run_once(name, root, binaries, lib):
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
    parser.add_argument('--repeat', type=int, default=1,
                        help='run each recipe this many times after building it '
                             'once; an intermittent defect is invisible at 1')
    args = parser.parse_args()
    if args.repeat < 1:
        parser.error('--repeat takes 1 or more')

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
            passed, detail = check(recipe, work, lib, args.compiler, args.repeat)
            print('{:5} {:32} {}'.format('PASS' if passed else 'FAIL',
                                         os.path.basename(recipe), detail))
            failures += 0 if passed else 1
        for label, mode, spec, runs in APP_SHAPES:
            passed, detail = check_app_shape(label, mode, spec, runs, work, lib,
                                             args.compiler)
            print('{:5} {:32} {}'.format('PASS' if passed else 'FAIL', label, detail))
            failures += 0 if passed else 1
    finally:
        if not args.keep:
            shutil.rmtree(work, ignore_errors=True)
        else:
            print('work directory: ' + work)

    return 1 if failures else 0


if __name__ == '__main__':
    sys.exit(main())
