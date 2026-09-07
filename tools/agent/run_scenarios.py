#!/usr/bin/env python3
# ===========================================================================
# Runs an AREG application as a scenario and reports one verdict.
#
# A scenario starts a list of processes in order, waits for the last one, and
# passes only when every process ended the way the scenario says and every
# expected line appeared in the output.
#
#   python3 tools/agent/run_scenarios.py                      # reads ./scenarios.json
#   python3 tools/agent/run_scenarios.py --file s.json --only smoke
#   python3 tools/agent/run_scenarios.py --json               # machine readable
#
# The scenario file format is SCHEMA below, and "--help" prints it.
# Exit code 0 when every scenario passed, 1 otherwise, 2 on a bad file.
# ===========================================================================
import argparse
import json
import os
import platform
import re
import socket
import shutil
import subprocess
import sys
import threading
import time

SCHEMA = """Scenario file: {"scenarios": [ { ... }, ... ]}

  name      Scenario name.
  procs     Processes, started in the listed order. The last one leads: the
            scenario ends when it ends.
  timeout   Seconds before the scenario is failed and killed. Default 60.
  router    true to start mtrouter first. Default false.
  stop      Stop one process while the others keep running, so the peers can be
            seen reacting to it. See below.

Process: {"binary": "myapp", "args": [], "expect": ["regex"], "exit": 0}
  binary   Name without a platform suffix; looked up in the build directory.
  name     Label for this process, for reports and for "stop". Default: binary.
  args     Command line arguments.
  expect   Regular expressions that all have to match the captured output.
  reject   Regular expressions that must not match the captured output.
  stdin    Lines written to the process's standard input once it has started;
           the stream is closed after the last one. This is how a "-q" quit path
           is exercised. WITHOUT this key the process still gets its own standard
           input and nothing ever writes to it or closes it, so a console loop
           waits there instead of reading end of input at once.
  exit     Required exit code. Omit or null to accept any.
  lead     true to make this the process the scenario waits for. Default: the
           last one in the list.
  delay    Seconds to wait before starting the next process. Default 0.5.

Stop: {"proc": "provider", "after": "consumer: ready", "signal": "term"}
  proc     The process to stop, by its name.
  after    A regular expression: the process is stopped once this matches the
           lead's output. A number instead waits that many seconds. Omitted, the
           process is stopped as soon as it is running.
  signal   "term" (default) or "kill". "kill" is an abrupt loss of the peer.
  A list of these runs them in order.

The output of a failing process is printed with the verdict, and a passing
scenario prints the line every expectation matched, so one run is the whole
evidence. --quiet prints only the verdict.

Two acceptance items recur in nearly every task and both are keys here, so
neither needs a shell: the console quit path is "stdin", and the peer going away
is "stop". Never start the processes by hand with &, sleep, pkill or ps.
"""

SUFFIX = {'Windows': '.exe', 'Darwin': '.mac'}.get(platform.system(), '.elf')


def fail(message, code=2):
    sys.stderr.write('error: {}\n'.format(message))
    sys.exit(code)


ROUTER_PORT = 8181
ROUTER_READY_SECONDS = 10.0
OUTPUT_TAIL_LINES = 40


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


def proc_name(spec):
    return spec.get('name') or spec['binary']


def send_stdin(handle, lines):
    """Write the lines to the process and close the stream."""
    try:
        for line in lines:
            handle.stdin.write(line if line.endswith('\n') else line + '\n')
        handle.stdin.flush()
        handle.stdin.close()
    except (OSError, ValueError):
        pass


def stop_handle(handle, signal_name):
    """Stop one running process, abruptly for "kill"."""
    if handle.poll() is not None:
        return
    if signal_name == 'kill':
        handle.kill()
    else:
        handle.terminate()


class LeadReader(object):
    """Reads the lead's output line by line so triggers can fire while it runs."""

    def __init__(self, handle):
        self._handle = handle
        self._lines = []
        self._lock = threading.Lock()
        self._thread = threading.Thread(target=self._pump)
        self._thread.daemon = True
        self._thread.start()

    def _pump(self):
        for line in iter(self._handle.stdout.readline, ''):
            with self._lock:
                self._lines.append(line)
        try:
            self._handle.stdout.close()
        except (OSError, ValueError):
            pass

    def text(self):
        with self._lock:
            return ''.join(self._lines)

    def join(self, timeout):
        self._thread.join(timeout)


def resolve_stops(scenario, handles, index_of):
    """Turn the scenario's stop entries into (spec, handle, trigger) triples."""
    stops = scenario.get('stop')
    if stops is None:
        return []
    if isinstance(stops, dict):
        stops = [stops]
    pending = []
    for entry in stops:
        target = entry.get('proc')
        if target not in index_of:
            return 'stop names {!r}, which is not a process in this scenario'.format(target)
        pending.append({'handle': handles[index_of[target]][1],
                        'proc': target,
                        'after': entry.get('after'),
                        'signal': entry.get('signal', 'term')})
    return pending


def fire_stops(pending, started, text):
    """Stop every process whose trigger has come true. Returns what is left."""
    left = []
    for entry in pending:
        after = entry['after']
        ready = (after is None
                 or (isinstance(after, (int, float)) and time.time() - started >= after)
                 or (isinstance(after, str) and re.search(after, text, re.MULTILINE)))
        if ready:
            stop_handle(entry['handle'], entry['signal'])
        else:
            left.append(entry)
    return left


def run_scenario(scenario, build_dirs, verbose, quiet):
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
    index_of = dict((proc_name(spec), i) for i, spec in enumerate(procs))
    handles = []
    outputs = {}
    verdict = None
    reader = None
    try:
        for index, spec in enumerate(procs):
            binary = find_binary(spec['binary'], build_dirs)
            if binary is None:
                verdict = 'binary not found: {}'.format(spec['binary'])
                break
            feed = spec.get('stdin')
            # Every process gets its own pipe, written and closed only when the
            # scenario says so. Inheriting this runner's standard input hands the
            # process whatever the shell had -- usually end of input at once, which
            # stops a console loop before the service has served anything.
            handle = subprocess.Popen([binary] + list(spec.get('args', [])),
                                      stdin=subprocess.PIPE,
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.STDOUT,
                                      text=True)
            handles.append((spec, handle))
            if feed:
                send_stdin(handle, feed)
            if index != lead_index:
                time.sleep(float(spec.get('delay', 0.5)))

        if verdict is None:
            _, lead = handles[lead_index]
            pending = resolve_stops(scenario, handles, index_of)
            if isinstance(pending, str):
                verdict = pending
            else:
                reader = LeadReader(lead)
                started = time.time()
                deadline = started + timeout
                while True:
                    pending = fire_stops(pending, started, reader.text())
                    if lead.poll() is not None:
                        break
                    if time.time() >= deadline:
                        lead.kill()
                        verdict = 'timed out after {:.0f}s'.format(timeout)
                        break
                    time.sleep(0.05)
                reader.join(5)
                outputs[lead_index] = reader.text()
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
        report_output(handles, outputs, quiet)
        return False, name, verdict

    evidence = []
    for index, (spec, handle) in enumerate(handles):
        label = proc_name(spec)
        output = outputs.get(index) or ''
        if verbose and output:
            sys.stdout.write(output)
        for pattern in spec.get('expect', []):
            found = re.search(pattern, output, re.MULTILINE)
            if found is None:
                report_output(handles, outputs, quiet, only=index)
                return False, name, 'no match for {!r} in the output of {}'.format(
                    pattern, label)
            evidence.append((label, found.group(0)))
        for pattern in spec.get('reject', []):
            found = re.search(pattern, output, re.MULTILINE)
            if found is not None:
                report_output(handles, outputs, quiet, only=index)
                return False, name, '{!r} was rejected but matched {!r} in {}'.format(
                    pattern, found.group(0), label)
        wanted = spec.get('exit')
        if wanted is not None and handle.returncode != wanted:
            report_output(handles, outputs, quiet, only=index)
            return False, name, '{} exited {}, expected {}'.format(
                label, handle.returncode, wanted)

    if not quiet and not verbose:
        for label, line in evidence:
            sys.stdout.write('      {:<20} {}\n'.format(label, line.strip()[:100]))
    return True, name, 'ok'


def report_output(handles, outputs, quiet, only=None):
    """Print what the processes said, so a failure needs no second run."""
    if quiet:
        return
    for index, (spec, _) in enumerate(handles):
        if only is not None and index != only:
            continue
        output = (outputs.get(index) or '').rstrip()
        if not output:
            continue
        lines = output.splitlines()
        head = '--- {} ---'.format(proc_name(spec))
        if len(lines) > OUTPUT_TAIL_LINES:
            head += ' (last {} of {} lines)'.format(OUTPUT_TAIL_LINES, len(lines))
            lines = lines[-OUTPUT_TAIL_LINES:]
        sys.stdout.write(head + '\n' + '\n'.join(lines) + '\n')


SELF_TEST_PROVIDER = """#!/usr/bin/env bash
echo "provider: serving"
while IFS= read -r line; do
    [ "$line" = "-q" ] && break
done
echo "provider: stopped"
"""

SELF_TEST_CONSUMER = """#!/usr/bin/env bash
sleep 2
echo "consumer: the provider was still there"
"""


def self_test():
    """Check that a process is not handed end of input it did not ask for.

    A console loop reading standard input is the shape of every long-running
    service here. Inheriting this runner's own standard input gives that loop end
    of input at once, the service stops before it has served anything, and the
    peers time out against a process that exited normally and printed nothing.
    """
    import tempfile
    root = tempfile.mkdtemp(prefix='areg-scenario-selftest-')
    try:
        for name, body in (('selftestprov', SELF_TEST_PROVIDER),
                           ('selftestcons', SELF_TEST_CONSUMER)):
            path = os.path.join(root, name + SUFFIX)
            with open(path, 'w', encoding='utf-8', newline='\n') as handle:
                handle.write(body)
            os.chmod(path, 0o755)
        scenario = {
            'name': 'no-console-attached', 'timeout': 15,
            'procs': [{'binary': 'selftestprov', 'name': 'provider',
                       'expect': ['provider: serving'],
                       'reject': ['provider: stopped']},
                      {'binary': 'selftestcons', 'name': 'consumer',
                       'expect': ['consumer: the provider was still there'],
                       'exit': 0}]}
        passed, name, detail = run_scenario(scenario, [root], False, True)
        if passed:
            print('self-test ok: a process with no "stdin" key is not given end of '
                  'input')
            return 0
        print('self-test FAILED: {}'.format(detail))
        print('a process with no "stdin" key was handed end of input, so a console '
              'loop stops the service before it serves anything')
        return 1
    finally:
        shutil.rmtree(root, ignore_errors=True)


def main():
    parser = argparse.ArgumentParser(
        description='Run AREG application scenarios.',
        epilog=SCHEMA,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--file', default='scenarios.json', help='the scenario file')
    parser.add_argument('--build', action='append', default=None,
                        help='directory holding the executables; repeatable')
    parser.add_argument('--only', default=None, help='run only the named scenario')
    parser.add_argument('--list', action='store_true', help='print the scenarios and exit')
    parser.add_argument('--json', action='store_true', help='print the verdict as JSON')
    parser.add_argument('--verbose', action='store_true',
                        help='print every line each process wrote')
    parser.add_argument('--quiet', action='store_true',
                        help='print only the verdict, no output and no evidence')
    parser.add_argument('--self-test', action='store_true',
                        help='check this runner itself; needs no project')
    args = parser.parse_args()

    if args.self_test:
        return self_test()

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
        passed, name, detail = run_scenario(scenario, build_dirs,
                                            args.verbose, args.quiet or args.json)
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
