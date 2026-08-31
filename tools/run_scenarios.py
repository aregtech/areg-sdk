#!/usr/bin/env python3
# ===========================================================================
# Runs an AREG application as a scenario and reports one verdict.
#
# A scenario starts a list of processes in order, waits for the last one, and
# passes only when every process ended the way the scenario says and every
# expected line appeared in the output.
#
#   python3 tools/run_scenarios.py                      # reads ./scenarios.json
#   python3 tools/run_scenarios.py --file s.json --only smoke
#   python3 tools/run_scenarios.py --json               # machine readable
#
# Scenario file: {"scenarios": [ { ... }, ... ]}
#
#   name      Scenario name.
#   procs     Processes, started in the listed order. The last one leads: the
#             scenario ends when it ends.
#   timeout   Seconds before the scenario is failed and killed. Default 60.
#   router    true to start mtrouter first. Default false.
#
# Process: {"binary": "myapp", "args": [], "expect": ["regex"], "exit": 0}
#   binary   Name without a platform suffix; looked up in the build directory.
#   expect   Regular expressions that all have to match the captured output.
#   exit     Required exit code. Omit or null to accept any.
#   lead     true to make this the process the scenario waits for. Default:
#            the last one in the list.
#
# Exit code 0 when every scenario passed, 1 otherwise, 2 on a bad file.
# ===========================================================================
import argparse
import json
import os
import platform
import re
import socket
import subprocess
import sys
import time

SUFFIX = {'Windows': '.exe', 'Darwin': '.mac'}.get(platform.system(), '.elf')


def fail(message, code=2):
    sys.stderr.write('error: {}\n'.format(message))
    sys.exit(code)


ROUTER_PORT = 8181
ROUTER_READY_SECONDS = 10.0


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


def find_binary(name, build_dirs):
    """Finds an executable by name, with or without the platform suffix."""
    for directory in build_dirs:
        for candidate in (name + SUFFIX, name):
            path = os.path.join(directory, candidate)
            if os.path.isfile(path) and os.access(path, os.X_OK):
                return path
    return None


def run_scenario(scenario, build_dirs, verbose):
    name = scenario.get('name', 'unnamed')
    timeout = float(scenario.get('timeout', 60))
    procs = scenario.get('procs') or []
    if not procs:
        return False, name, 'no processes listed'

    router_handle = None
    if scenario.get('router'):
        router = find_binary('mtrouter', build_dirs)
        if router is None:
            return False, name, 'mtrouter not found in ' + ', '.join(build_dirs)
        # --service is the unattended mode. The readiness poll below also catches a
        # router that could not bind because another one already holds the port.
        router_handle = subprocess.Popen([router, '--service'],
                                         stdout=subprocess.DEVNULL,
                                         stderr=subprocess.DEVNULL)
        if not wait_router_ready():
            router_handle.terminate()
            return False, name, 'mtrouter did not start listening on port {}'.format(
                ROUTER_PORT)

    lead_index = next((i for i, p in enumerate(procs) if p.get('lead')), len(procs) - 1)
    handles = []
    outputs = {}
    verdict = None
    try:
        for index, spec in enumerate(procs):
            binary = find_binary(spec['binary'], build_dirs)
            if binary is None:
                verdict = 'binary not found: {}'.format(spec['binary'])
                break
            handles.append((spec, subprocess.Popen([binary] + list(spec.get('args', [])),
                                                   stdout=subprocess.PIPE,
                                                   stderr=subprocess.STDOUT,
                                                   text=True)))
            if index != lead_index:
                time.sleep(float(spec.get('delay', 0.5)))

        if verdict is None:
            _, lead = handles[lead_index]
            try:
                outputs[lead_index], _ = lead.communicate(timeout=timeout)
            except subprocess.TimeoutExpired:
                lead.kill()
                outputs[lead_index], _ = lead.communicate()
                verdict = 'timed out after {:.0f}s'.format(timeout)
    finally:
        # Every process that is still running is stopped, then drained, so its
        # output can be matched and no pipe is left open.
        for index, (_, handle) in enumerate(handles):
            if index in outputs:
                continue
            if handle.poll() is None:
                handle.terminate()
            try:
                outputs[index], _ = handle.communicate(timeout=5)
            except subprocess.TimeoutExpired:
                handle.kill()
                outputs[index], _ = handle.communicate()
        if router_handle is not None and router_handle.poll() is None:
            router_handle.terminate()
            try:
                router_handle.wait(timeout=5)
            except subprocess.TimeoutExpired:
                router_handle.kill()

    if verdict is not None:
        return False, name, verdict

    for index, (spec, handle) in enumerate(handles):
        output = outputs.get(index) or ''
        if verbose and output:
            sys.stdout.write(output)
        for pattern in spec.get('expect', []):
            if re.search(pattern, output, re.MULTILINE) is None:
                return False, name, 'no match for {!r} in the output of {}'.format(
                    pattern, spec['binary'])
        wanted = spec.get('exit')
        if wanted is not None and handle.returncode != wanted:
            return False, name, '{} exited {}, expected {}'.format(
                spec['binary'], handle.returncode, wanted)

    return True, name, 'ok'


def main():
    parser = argparse.ArgumentParser(description='Run AREG application scenarios.')
    parser.add_argument('--file', default='scenarios.json', help='the scenario file')
    parser.add_argument('--build', action='append', default=None,
                        help='directory holding the executables; repeatable')
    parser.add_argument('--only', default=None, help='run only the named scenario')
    parser.add_argument('--list', action='store_true', help='print the scenarios and exit')
    parser.add_argument('--json', action='store_true', help='print the verdict as JSON')
    parser.add_argument('--verbose', action='store_true', help='print captured output')
    args = parser.parse_args()

    if not os.path.isfile(args.file):
        fail('no scenario file at {}'.format(args.file))
    try:
        with open(args.file, encoding='utf-8') as handle:
            document = json.load(handle)
    except (OSError, ValueError) as error:
        fail('cannot read {}: {}'.format(args.file, error))

    scenarios = document.get('scenarios') or []
    if args.only:
        scenarios = [s for s in scenarios if s.get('name') == args.only]
        if not scenarios:
            fail('no scenario named {}'.format(args.only))

    if args.list:
        for scenario in scenarios:
            print(scenario.get('name', 'unnamed'))
        return 0

    build_dirs = args.build or [os.path.join('build', 'bin'), 'build']

    results = []
    for scenario in scenarios:
        passed, name, detail = run_scenario(scenario, build_dirs, args.verbose)
        results.append({'name': name, 'passed': passed, 'detail': detail})
        if not args.json:
            print('{:5} {:24} {}'.format('PASS' if passed else 'FAIL', name, detail))

    failed = [r for r in results if not r['passed']]
    if args.json:
        print(json.dumps({'passed': len(results) - len(failed),
                          'failed': len(failed),
                          'results': results}, indent=2))
    else:
        print('{} passed, {} failed'.format(len(results) - len(failed), len(failed)))
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
