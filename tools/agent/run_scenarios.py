#!/usr/bin/env python3
# ===========================================================================
# Runs an application as a scenario and reports one verdict.
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
import tempfile
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
           is exercised, on the lead of a scenario of its own: on any other process
           the quit is read at once and it ends before its peers are served.
           WITHOUT this key the process still gets its own standard
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
    # Output already printed is flushed first: stdout is block-buffered into a
    # pipe, so without this the error reaches the reader before the lines it is
    # about.
    sys.stdout.flush()
    sys.stderr.write('error: {}\n'.format(message))
    sys.exit(code)


ROUTER_PORT = 8181
ROUTER_READY_SECONDS = 10.0

# A build writes its binaries a little after it reads the sources, so a difference
# under this is the build itself and not an edit.
STALE_TOLERANCE_SECONDS = 30.0
OUTPUT_TAIL_LINES = 40


def is_listening(port, host='127.0.0.1'):
    """Tells whether anything accepts a connection on the port right now."""
    probe = socket.socket()
    probe.settimeout(0.5)
    try:
        probe.connect((host, port))
        return True
    except OSError:
        return False
    finally:
        probe.close()


# The suffixes a framework service is built with, whichever system built it.
SERVICE_SUFFIXES = ('.elf', '.exe', '.mac', '')


def find_service(name, directories):
    """A framework service binary of this name, with or without a suffix."""
    for directory in directories:
        for suffix in SERVICE_SUFFIXES:
            path = os.path.join(directory, name + suffix)
            if os.path.isfile(path) and os.access(path, os.X_OK):
                return path
    return None


def start_service(binary, port, args=(), cwd=None, timeout=ROUTER_READY_SECONDS):
    """Starts a long-lived service and waits for the process it started to listen.

    Returns (handle, '') or (None, why). On POSIX --service is the unattended mode:
    no console loop, so a standard input at /dev/null cannot end it. On Windows it
    hands the process to the Service Control Manager, which refuses one the manager
    did not start, so console mode is used there and its standard input is held
    open instead.

    The wait ends when the service this call started exits, so a port another
    process already holds is never taken for readiness.
    """
    if platform.system() == 'Windows':
        command, feed = [binary] + list(args), subprocess.PIPE
    else:
        command, feed = [binary, '--service'] + list(args), subprocess.DEVNULL
    handle = subprocess.Popen(command, cwd=cwd, stdin=feed,
                              stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    name = os.path.basename(binary)
    deadline = time.time() + timeout
    while time.time() < deadline:
        if handle.poll() is not None:
            return None, '{} exited {} before it listened on port {}'.format(
                name, handle.returncode, port)
        if is_listening(port):
            return handle, ''
        time.sleep(0.2)
    stop_services([handle])
    return None, '{} did not listen on port {} within {:.0f}s'.format(
        name, port, timeout)


def stop_services(handles, timeout=10.0):
    """Stops long-lived services and waits for each one to be gone.

    'terminate()' returns before the process has released its port, and the next
    thing to start then connects to a corpse's successor instead of to its own.
    """
    live = [handle for handle in handles if handle is not None]
    for handle in live:
        if handle.poll() is None:
            try:
                handle.terminate()
            except OSError:
                pass
    deadline = time.time() + timeout
    for handle in live:
        try:
            handle.wait(timeout=max(0.5, deadline - time.time()))
        except subprocess.TimeoutExpired:
            try:
                handle.kill()
                handle.wait(timeout=5.0)
            except (OSError, subprocess.TimeoutExpired):
                pass


def tools_dir():
    """The directory holding this tool, spelled for a command line."""
    return os.path.dirname(os.path.abspath(__file__)).replace(chr(92), '/')


def nothing_built(build_dirs):
    """True when not one executable exists in any of the build directories."""
    for directory in build_dirs:
        if not os.path.isdir(directory):
            continue
        for entry in os.listdir(directory):
            path = os.path.join(directory, entry)
            if os.path.isfile(path) and os.access(path, os.X_OK):
                return False
    return True


def find_binary(name, build_dirs):
    """Finds an executable by name, with or without the platform suffix."""
    for directory in build_dirs:
        for candidate in (name + SUFFIX, name):
            path = os.path.join(directory, candidate)
            if os.path.isfile(path) and os.access(path, os.X_OK):
                return path
    return None


# A binary older than the sources it was built from is the previous program. Its
# failures read as logic or environment faults and cost a diagnosis every time, so
# the run is refused before anything starts.
SOURCE_SUFFIXES = ('.cpp', '.hpp', '.h', '.cc', '.cxx', '.siml', '.fsml', '.dtml')
SOURCE_SKIP = {'build', '.git', '.vs', '.idea', '__pycache__'}


def newest_source(root):
    """The newest source of the project, as (path, mtime), or (None, 0)."""
    newest, when = None, 0.0
    for folder, dirs, files in os.walk(root):
        dirs[:] = [d for d in dirs if d not in SOURCE_SKIP and not d.startswith('.')]
        for name in files:
            if not name.endswith(SOURCE_SUFFIXES) and name != 'CMakeLists.txt':
                continue
            path = os.path.join(folder, name)
            try:
                stamp = os.path.getmtime(path)
            except OSError:
                continue
            if stamp > when:
                newest, when = path, stamp
    return newest, when


# The marker gen_skeleton.py writes and fill_markers.py fills. The spelling is the
# same in all three tools; check_corpus.py case "marker-spelling" holds them together.
TODO_MARKER_RE = re.compile(r'//\s*TODO\(you\)\s+([A-Za-z_][\w]*)\s*:')


def open_markers(root):
    """Every TODO(you) marker still in the project, as (path, line, slot)."""
    found = []
    for folder, dirs, files in os.walk(root):
        dirs[:] = [d for d in dirs if d not in SOURCE_SKIP and not d.startswith('.')]
        for name in sorted(files):
            if not name.endswith(SOURCE_SUFFIXES):
                continue
            path = os.path.join(folder, name)
            try:
                with open(path, encoding='utf-8') as handle:
                    lines = handle.read().splitlines()
            except (OSError, UnicodeDecodeError):
                continue
            for number, line in enumerate(lines, 1):
                hit = TODO_MARKER_RE.search(line)
                if hit:
                    found.append((path, number, hit.group(1)))
    return found


def stale_binaries(scenarios, build_dirs, root):
    """Whether the sources were edited and no build has run since.

    The reference is the **newest** of the executables, not each one of them: an
    incremental build relinks only the targets whose own sources changed, so a
    binary older than the newest source is normal once a build has run. What is
    never normal is every binary being older than the newest source, which means
    no build ran between the edit and this call.

    Returns a list of (binary path, source path, minutes between them), empty when
    the build is current.
    """
    source, when = newest_source(root)
    if source is None:
        return []
    seen, found = set(), []
    for scenario in scenarios:
        for spec in scenario.get('procs') or []:
            name = spec.get('binary')
            if not name or name in seen:
                continue
            seen.add(name)
            path = find_binary(name, build_dirs)
            if path is None:
                continue
            try:
                found.append((path, os.path.getmtime(path)))
            except OSError:
                continue
    if not found:
        return []
    built = max(stamp for _, stamp in found)
    if built >= when - STALE_TOLERANCE_SECONDS:
        return []
    return [(path, source, (when - built) / 60.0) for path, _ in found]


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


# What one process may keep in memory. A pipe holds about 64 KB, and a process that
# fills it blocks in write() until someone reads, so every process is drained from
# the moment it starts. The whole output goes to its spool file; this is the tail
# kept for matching and for the report.
TAIL_LIMIT = 4 * 1024 * 1024


class OutputReader(object):
    """Drains one process line by line, spools it to a file and keeps the tail.

    Every process gets one from the moment it is launched. A process that is only
    drained at cleanup blocks in write() once the pipe is full and never reaches
    its own work, and the scenario then reports a timeout with no cause in it.
    """

    def __init__(self, handle, path):
        self._handle = handle
        self._path = path
        self._lines = []
        self._size = 0
        self._dropped = 0
        self._lock = threading.Lock()
        self._spool = open(path, 'w', encoding='utf-8', errors='replace')
        self._thread = threading.Thread(target=self._pump)
        self._thread.daemon = True
        self._thread.start()

    def _pump(self):
        for line in iter(self._handle.stdout.readline, ''):
            self._take(line)
        for stream in (self._handle.stdout, self._spool):
            try:
                stream.close()
            except (OSError, ValueError):
                pass

    def _take(self, line):
        """Spools one line and keeps it in the tail."""
        with self._lock:
            self._spool.write(line)
            self._lines.append(line)
            self._size += len(line)
            while self._size > TAIL_LIMIT and len(self._lines) > 1:
                self._size -= len(self._lines.pop(0))
                self._dropped += 1

    def text(self):
        with self._lock:
            return ''.join(self._lines)

    def dropped(self):
        """Lines written past the tail limit; they are in the spool file only."""
        with self._lock:
            return self._dropped

    def path(self):
        return self._path

    def join(self, timeout):
        self._thread.join(timeout)
        try:
            self._spool.close()
        except (OSError, ValueError):
            pass


class LaggingReader(OutputReader):
    """An OutputReader that hands every line over late. The self-test uses it."""

    def _take(self, line):
        time.sleep(0.2)
        OutputReader._take(self, line)


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
            entry['handle'].stopped_by_scenario = True
            entry['fired_at'] = time.time() - started
            stop_handle(entry['handle'], entry['signal'])
        else:
            left.append(entry)
    return left


def stops_missed(pending):
    """Why the scenario proves nothing, when a stop it declared never fired.

    A stop is the experiment the scenario exists to run. Cleanup terminates every
    process whether it fired or not, so without this the scenario passes having
    never injected the fault it names.
    """
    entry = pending[0]
    after = entry['after']
    if isinstance(after, (int, float)):
        why = 'the run ended before {}s'.format(after)
    else:
        why = 'nothing matched {!r}'.format(after)
    return ('the stop on {} never fired: {}, so the scenario did not test what it '
            'declares'.format(entry['proc'], why))


def run_scenario(scenario, build_dirs, verbose, quiet, observed=None, reader_class=None):
    reader_class = reader_class or OutputReader
    name = scenario.get('name', 'unnamed')
    timeout = float(scenario.get('timeout', 60))
    procs = scenario.get('procs') or []
    if not procs:
        return False, name, 'no processes listed'

    router_handle = None
    if scenario.get('router'):
        router = find_service('mtrouter', build_dirs)
        if router is None:
            return False, name, 'mtrouter not found in ' + ', '.join(build_dirs)
        router_handle, why = start_service(router, ROUTER_PORT)
        if router_handle is None:
            return False, name, why

    lead_index = next((i for i, p in enumerate(procs) if p.get('lead')), len(procs) - 1)
    index_of = dict((proc_name(spec), i) for i, spec in enumerate(procs))
    handles = []
    outputs = {}
    launched = {}
    ended = {}
    verdict = None
    readers = {}
    spool = tempfile.mkdtemp(prefix='areg-scenario-')
    actions = []
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
            launched[index] = time.time()
            handles.append((spec, handle))
            readers[index] = reader_class(
                handle, os.path.join(spool, '{}-{}.log'.format(index, proc_name(spec))))
            if feed:
                send_stdin(handle, feed)
            if index != lead_index or (index < len(procs) - 1 and 'delay' in spec):
                time.sleep(float(spec.get('delay', 0.5)))

        if verdict is None:
            _, lead = handles[lead_index]
            actions = resolve_stops(scenario, handles, index_of)
            if isinstance(actions, str):
                verdict, actions = actions, []
            else:
                pending = list(actions)
                reader = readers[lead_index]
                started = time.time()
                deadline = started + timeout
                while True:
                    pending = fire_stops(pending, started, reader.text())
                    for index, (_, handle) in enumerate(handles):
                        if index not in ended and handle.poll() is not None:
                            ended[index] = time.time() - launched[index]
                    if lead.poll() is not None:
                        ended.setdefault(lead_index, time.time() - launched[lead_index])
                        # The lead's last lines can still be in its reader: drain it to
                        # the end, then judge the stops on everything it printed.
                        reader.join(5)
                        pending = fire_stops(pending, started, reader.text())
                        break
                    if time.time() >= deadline:
                        lead.kill()
                        verdict = 'timed out after {:.0f}s'.format(timeout)
                        break
                    time.sleep(0.05)
                if verdict is None and pending:
                    verdict = stops_missed(pending)
    finally:
        # Every process that is still running is stopped, then reaped. Its output
        # was drained by its own reader from the moment it started, so nothing here
        # waits on a pipe.
        for index, (_, handle) in enumerate(handles):
            if handle.poll() is None:
                handle.terminate()
            try:
                handle.wait(timeout=5)
            except subprocess.TimeoutExpired:
                handle.kill()
                handle.wait()
            reader = readers.get(index)
            if reader is not None:
                reader.join(5)
                outputs[index] = reader.text()
                if reader.dropped():
                    sys.stderr.write(
                        '      {} printed more than the {} MB kept in memory; '
                        '{} line(s) are in {} only\n'.format(
                            proc_name(handles[index][0]), TAIL_LIMIT // (1024 * 1024),
                            reader.dropped(), reader.path()))
        stop_services([router_handle])

    # Exit codes, outputs and the lead's run time, for a caller that asks.
    if observed is not None:
        observed['exits'] = dict((proc_name(spec), handle.returncode)
                                 for spec, handle in handles)
        observed['outputs'] = dict((proc_name(spec), outputs.get(index) or '')
                                   for index, (spec, _) in enumerate(handles))
        observed['elapsed'] = ended.get(lead_index)
        observed['verdict'] = verdict
        observed['actions'] = [{'proc': entry['proc'], 'after': entry['after'],
                                'signal': entry['signal'],
                                'fired_at': entry.get('fired_at')}
                               for entry in actions]

    def failed(detail):
        report_output(handles, outputs, quiet, full=verbose)
        report_status(handles, outputs, ended, lead_index, quiet)
        if not quiet:
            sys.stdout.write('      full output of every process: {}\n'.format(spool))
        return False, name, detail

    if verdict is not None:
        return failed(verdict)

    evidence = []
    for index, (spec, handle) in enumerate(handles):
        label = proc_name(spec)
        output = outputs.get(index) or ''
        for pattern in spec.get('expect', []):
            found = re.search(pattern, output, re.MULTILINE)
            if found is None:
                return failed('no match for {!r} in the output of {}'.format(
                    pattern, label))
            evidence.append((label, found.group(0)))
        for pattern in spec.get('reject', []):
            found = re.search(pattern, output, re.MULTILINE)
            if found is not None:
                return failed('{!r} was rejected but matched {!r} in {}'.format(
                    pattern, found.group(0), label))
        wanted = spec.get('exit')
        if wanted is not None and handle.returncode != wanted:
            return failed('{} exited {}, expected {}'.format(
                label, handle.returncode, wanted))

    if verbose:
        report_output(handles, outputs, quiet, full=True)
    elif not quiet:
        for label, line in evidence:
            sys.stdout.write('      {:<20} {}\n'.format(label, line.strip()[:100]))
    shutil.rmtree(spool, ignore_errors=True)
    return True, name, 'ok'


def report_output(handles, outputs, quiet, full=False):
    """Print what every process said, so a failure needs no second run.

    Every process is printed, not only the one an expectation failed on: the output
    that explains a failure is usually the peer's. full prints it untruncated.
    """
    if quiet:
        return
    for index, (spec, _) in enumerate(handles):
        output = (outputs.get(index) or '').rstrip()
        if not output:
            continue
        lines = output.splitlines()
        head = '--- {} ---'.format(proc_name(spec))
        if not full and len(lines) > OUTPUT_TAIL_LINES:
            head += ' (last {} of {} lines)'.format(OUTPUT_TAIL_LINES, len(lines))
            lines = lines[-OUTPUT_TAIL_LINES:]
        sys.stdout.write(head + '\n' + '\n'.join(lines) + '\n')


def report_status(handles, outputs, ended, lead_index, quiet):
    """One line per process on a failure, the silent ones included.

    A process that ended on its own while the lead was still running is named, since
    its peers then wait for it until the timeout and print nothing that says why.
    """
    if quiet:
        return
    for index, (spec, handle) in enumerate(handles):
        text = (outputs.get(index) or '').strip()
        said = '{} line(s) of output'.format(len(text.splitlines())) if text else 'no output'
        if index not in ended:
            state = 'still running when the scenario ended, stopped by the runner'
        elif getattr(handle, 'stopped_by_scenario', False):
            state = 'stopped by "stop" after {:.1f}s'.format(ended[index])
        else:
            state = 'had exited {} by {:.1f}s'.format(handle.returncode, ended[index])
            if index != lead_index:
                state += ', on its own while the lead was running'
                if spec.get('stdin'):
                    state += ' -- its "stdin" was written at start'
        sys.stdout.write('      status  {:<20} {}, {}\n'.format(proc_name(spec), state, said))


def asserts_nothing(scenario):
    """True when no process of the scenario declares anything to check.

    A scenario is the evidence that a requirement holds. One that declares no
    "expect", no "reject" and no "exit" passes whatever the program prints, so a
    suite of them exits 0 on a program that does nothing. An empty "expect" list
    is the same thing written a second way.
    """
    for spec in scenario.get('procs') or []:
        if spec.get('expect') or spec.get('reject'):
            return False
        if spec.get('exit') is not None:
            return False
    return True


def lint_scenario(scenario):
    """Mistakes in a scenario that show up only as a hang, said before it runs."""
    procs = scenario.get('procs') or []
    lead_index = next((i for i, p in enumerate(procs) if p.get('lead')), len(procs) - 1)
    notes = []
    for index, spec in enumerate(procs):
        if index != lead_index and spec.get('stdin'):
            notes.append('"stdin" on {} is written the moment it starts, and it is not the '
                         'lead: a quit there ends it before its peers are served. Give the '
                         'quit path a scenario of its own, where that process leads.'
                         .format(proc_name(spec)))
    return notes


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

# Windows cannot execute a shell script, whatever it is named, so the same two
# fixtures are written again as batch files. "set /p" blocks while the pipe is
# open and leaves the variable unset at end of input, which is the condition the
# self-test looks for. "ping" stands in for "sleep": "timeout" refuses to run at
# all once standard input is a pipe.
SELF_TEST_PROVIDER_BATCH = """@echo off
echo provider: serving
:read
set "line="
set /p line=
if not defined line goto stop
if "%line%"=="-q" goto stop
goto read
:stop
echo provider: stopped
"""

SELF_TEST_CONSUMER_BATCH = """@echo off
ping -n 3 127.0.0.1 >nul
echo consumer: the provider was still there
"""

# A process that is not the lead, printing far more than a pipe holds. The pipe is
# about 64 KB, so 2100 lines of 128 characters is four times over it.
SELF_TEST_NOISY = """#!/bin/sh
echo "provider: serving"
i=0
while [ $i -lt 2100 ]; do
    echo "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    i=$((i + 1))
done
echo "provider: drained"
"""

SELF_TEST_NOISY_BATCH = """@echo off
echo provider: serving
set LINE=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
for /l %%i in (1,1,2100) do echo %LINE%
echo provider: drained
"""

# What a self-test fixture is named and what it holds. The name carries the
# extension the platform can start, and find_binary falls back to the bare name,
# so the fixture is named in full and no suffix is appended to it a second time.
if platform.system() == 'Windows':
    SELF_TEST_SUFFIX  = '.bat'
    SELF_TEST_NEWLINE = '\r\n'
    SELF_TEST_BODIES  = (SELF_TEST_PROVIDER_BATCH, SELF_TEST_CONSUMER_BATCH)
    SELF_TEST_NOISY_BODY = SELF_TEST_NOISY_BATCH
else:
    SELF_TEST_SUFFIX  = SUFFIX
    SELF_TEST_NEWLINE = '\n'
    SELF_TEST_BODIES  = (SELF_TEST_PROVIDER, SELF_TEST_CONSUMER)
    SELF_TEST_NOISY_BODY = SELF_TEST_NOISY


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
        provider, consumer = (name + SELF_TEST_SUFFIX
                              for name in ('selftestprov', 'selftestcons'))
        for name, body in zip((provider, consumer), SELF_TEST_BODIES):
            path = os.path.join(root, name)
            with open(path, 'w', encoding='utf-8',
                      newline=SELF_TEST_NEWLINE) as handle:
                handle.write(body)
            os.chmod(path, 0o755)
        scenario = {
            'name': 'no-console-attached', 'timeout': 15,
            'procs': [{'binary': provider, 'name': 'provider',
                       'expect': ['provider: serving'],
                       'reject': ['provider: stopped']},
                      {'binary': consumer, 'name': 'consumer',
                       'expect': ['consumer: the provider was still there'],
                       'exit': 0}]}
        passed, name, detail = run_scenario(scenario, [root], False, True)
        if not passed:
            print('self-test FAILED: stdin: {}'.format(detail))
            print('a process with no "stdin" key was handed end of input, so a console '
                  'loop stops the service before it serves anything')
            return 1

        # A stop is the experiment a scenario runs. One that never fires must not
        # leave a passing verdict behind: cleanup stops every process anyway.
        unfired = dict(scenario, name='stop-that-never-fires',
                       stop={'proc': 'provider', 'after': 'NEVER_PRINTED',
                             'signal': 'kill'})
        passed, _, detail = run_scenario(unfired, [root], False, True)
        if passed:
            print('self-test FAILED: a stop whose trigger never matched still passed')
            print('the scenario declared a fault it never injected, and cleanup '
                  'terminating the process was taken for the experiment')
            return 1

        fired = dict(scenario, name='stop-that-fires',
                     stop={'proc': 'provider',
                           'after': 'consumer: the provider was still there',
                           'signal': 'kill'})
        passed, _, detail = run_scenario(fired, [root], False, True)
        if not passed:
            print('self-test FAILED: a stop whose trigger matched did not pass: {}'
                  .format(detail))
            return 1

        # The lead prints the trigger and exits before its reader hands the line over.
        passed, _, detail = run_scenario(fired, [root], False, True,
                                         reader_class=LaggingReader)
        if not passed:
            print('self-test FAILED: a stop triggered by the last line of a lead that '
                  'has exited did not pass: {}'.format(detail))
            return 1

        # A process that is not the lead has its own reader from the moment it
        # starts. Drained only at cleanup, it blocks in write() once the pipe is
        # full and never reaches the line below its own output.
        noisy = 'selftestnoisy' + SELF_TEST_SUFFIX
        path = os.path.join(root, noisy)
        with open(path, 'w', encoding='utf-8', newline=SELF_TEST_NEWLINE) as handle:
            handle.write(SELF_TEST_NOISY_BODY)
        os.chmod(path, 0o755)
        pressure = {
            'name': 'output-pressure', 'timeout': 30,
            'procs': [{'binary': noisy, 'name': 'noisy',
                       'expect': ['provider: drained']},
                      {'binary': consumer, 'name': 'consumer',
                       'expect': ['consumer: the provider was still there'],
                       'exit': 0}]}
        passed, _, detail = run_scenario(pressure, [root], False, True)
        if not passed:
            print('self-test FAILED: output pressure: {}'.format(detail))
            print('a process that is not the lead filled its pipe and blocked in '
                  'write(), so it never reached its own work')
            return 1

        # A scenario that declares nothing to check passes whatever the program
        # prints, so a suite of them reports success about an untested program.
        vacuous = [{'name': 'no-keys', 'procs': [{'binary': consumer}]},
                   {'name': 'empty-expect',
                    'procs': [{'binary': consumer, 'expect': []}]},
                   {'name': 'exit-null',
                    'procs': [{'binary': consumer, 'exit': None}]}]
        for scenario in vacuous:
            if not asserts_nothing(scenario):
                print('self-test FAILED: vacuity: {} was taken for an assertion'
                      .format(scenario['name']))
                return 1
        if asserts_nothing({'name': 'real',
                            'procs': [{'binary': consumer, 'exit': 0}]}):
            print('self-test FAILED: vacuity: a declared exit code is an assertion')
            return 1

        print('self-test ok: 6 case(s): end of input, an unfired stop, a fired stop, '
              'a stop on the last line of an exited lead, output pressure, '
              'a scenario that asserts nothing')
        return 0
    finally:
        shutil.rmtree(root, ignore_errors=True)


def main():
    parser = argparse.ArgumentParser(
        description='Run application scenarios.',
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
    parser.add_argument('--stale-ok', action='store_true',
                        help='run even when a binary is older than the sources')
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
    # An empty suite is not a passing suite: it is a project with nothing proven
    # about it, and it used to exit 0 under the same words a full suite exits 0 with.
    if not scenarios:
        fail('no scenarios in {}: an empty suite proves nothing. Write one scenario '
             'per requirement the task states.'.format(args.file))
    if args.only:
        scenarios = [s for s in scenarios if s.get('name') == args.only]
        if not scenarios:
            fail('no scenario named {}'.format(args.only))

    if args.list:
        for scenario in scenarios:
            print(scenario.get('name', 'unnamed'))
        return 0

    # The empty-suite refusal above, one level deeper: a suite of scenarios that
    # assert nothing proves no more than no suite at all.
    vacuous = [s.get('name', 'unnamed') for s in scenarios if asserts_nothing(s)]
    if vacuous:
        named = ', '.join('"{}"'.format(name) for name in vacuous)
        fail('{} {} nothing: no "expect", "reject" or "exit" on any process, so {} '
             'whatever the program prints. Give one process an "expect" naming a line '
             'the requirement demands, or the "exit" it must end with.'
             .format(named,
                     'asserts' if len(vacuous) == 1 else 'assert',
                     'it passes' if len(vacuous) == 1 else 'they pass'))

    build_dirs = args.build or [os.path.join('build', 'bin'), 'build']

    # Nothing was built at all. Left to the runner this is reported one missing
    # binary at a time, and no one of those lines names the build, so the reader
    # is told what is absent and never what to do about it.
    if nothing_built(build_dirs):
        fail('nothing is built: no executable in {}. No scenario can run until the '
             'project is built:\n  python3 {}/build_project.py'
             .format(', '.join(build_dirs), tools_dir()))

    if not args.stale_ok:
        stale = stale_binaries(scenarios, build_dirs,
                               os.path.dirname(os.path.abspath(args.file)) or '.')
        if stale:
            for path, source, minutes in stale:
                sys.stderr.write(
                    'error: {} is older than {} (edited {:.0f} min after the build)\n'
                    .format(os.path.basename(path), os.path.relpath(source), minutes))
            sys.stderr.write('build first, then run:\n')
            sys.stderr.write('  python3 {}/build_project.py\n'.format(tools_dir()))
            sys.stderr.write('running the old program is what makes a fixed defect '
                             'look unfixed. --stale-ok runs it anyway.\n')
            return 2

    results = []
    for scenario in scenarios:
        if not args.json:
            for note in lint_scenario(scenario):
                print('note  {:24} {}'.format(scenario.get('name', 'unnamed'), note))
        passed, name, detail = run_scenario(scenario, build_dirs,
                                            args.verbose, args.quiet or args.json)
        results.append({'name': name, 'passed': passed, 'detail': detail})
        if not args.json:
            print('{:5} {:24} {}'.format('PASS' if passed else 'FAIL', name, detail))

    failed = [r for r in results if not r['passed']]
    # A passing suite is what a report is written from, so the markers still open are
    # reported here rather than only where they were filled.
    still_open = open_markers(os.path.dirname(os.path.abspath(args.file)) or '.')
    if args.json:
        print(json.dumps({'passed': len(results) - len(failed),
                          'failed': len(failed),
                          'open_markers': [{'file': path.replace(os.sep, '/'),
                                            'line': line, 'marker': slot}
                                           for path, line, slot in still_open],
                          'results': results}, indent=2))
    else:
        print('{} passed, {} failed'.format(len(results) - len(failed), len(failed)))
        if still_open:
            print('{} marker(s) still open, so this project is not finished:'
                  .format(len(still_open)))
            for path, line, slot in still_open[:8]:
                print('  {}:{}  {}'.format(os.path.relpath(path).replace(os.sep, '/'),
                                           line, slot))
            if len(still_open) > 8:
                print('  and {} more'.format(len(still_open) - 8))
            print('A scenario passing says nothing about the requirement behind an open')
            print('marker: no body was written for it. Fill it with')
            print('fill_markers.py --bodies, or write it in place and delete the line.')
            print('check_contract.py reports the same markers as P-17.')
        # A run without --only reports every scenario by name, so isolating one that
        # already passes tells the caller nothing it is not about to be told anyway.
        if args.only and not failed:
            print('--only is for iterating on a failure. Run without it to check the '
                  'whole suite; it reports each scenario on its own line.')
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
