#!/usr/bin/env python3
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ---------------------------------------------------------------------------
#  Areg SDK -- example applications test driver (Windows, Linux, macOS).
#
#  Runs the built example applications as complete scenarios: the processes
#  that belong together are started together, the driving process is given a
#  deadline, and the scenario passes only if every process ends the way it is
#  supposed to end.
#
#  Usage:
#      python tools/run-all-examples.py [options]
#
#  Common options:
#      --tier smoke|ipc|all|perf   Which set of scenarios to run (default: all)
#      --bin-dir DIR               Directory with the built binaries
#      --timeout SEC               Per-scenario deadline (default: 120)
#      --repeat N                  Run the selection N times, report every failure
#      --only NAME[,NAME...]       Run only the named scenarios
#      --list                      Print the scenarios and exit
#      --out-dir DIR               Where to write the captured output
#      --timestamp                 Put the output in a time stamped sub-directory
#      --junit FILE                Write the results as a JUnit report
#      --summary FILE              Append a report in Markdown, for a build server
#
#  On a build server the binary directory should always be passed explicitly,
#  either with '--bin-dir' or through the AREG_BIN_DIR environment variable.
#
#  Exit code: 0 when every scenario passed, 1 otherwise.
# ===========================================================================

import argparse
import os
import platform
import re
import shutil
import socket
import subprocess
import sys
import threading
import time

IS_WINDOWS = platform.system() == 'Windows'
EXE_SUFFIXES = ('', '.exe', '.elf', '.mac', '.out')

if IS_WINDOWS:
    pty = None
else:
    import fcntl
    import pty
    import struct
    import termios

# Size reported to a process that runs on a pseudo terminal. The applications lay their
# output out in columns and rows, and a terminal of 0 x 0 -- what openpty gives by default
# -- makes them write everything into the first cell.
PTY_ROWS = 40
PTY_COLS = 120

# Router settings have to match 'framework/areg/resources/areg.init'.
ROUTER_HOST = '127.0.0.1'
ROUTER_PORT = 8181

# Time given to a helper process to shut down on its own after the leading
# process of a scenario has finished.
GRACE_SECONDS = 15.0

# Time given to the router to accept connections after it was started.
ROUTER_READY_SECONDS = 10.0

# Time given to a process to leave on its own after it was asked to quit.
QUIT_SECONDS = 12.0


# ---------------------------------------------------------------------------
# Scenario description.
#
#   name      Scenario identifier, also used by '--only'.
#   tier      'smoke' (single process, no router), 'ipc' or 'perf'.
#   procs     Processes of the scenario, started in the listed order. The last
#             one is the leading process: the scenario ends when it ends.
#   timeout   Per-scenario deadline in seconds, overrides '--timeout'.
#   soak      Seconds to keep a scenario without a natural end running. Some
#             examples are demonstrations that run until the user stops them.
#             For those the test is: stay alive for that long, do not crash and
#             do not let any process die early.
#   known     Description of an open defect this scenario runs into. A failure
#             is then reported as XFAIL and does not break the run, while an
#             unexpected success is reported as XPASS and does break it, so
#             that the marker cannot be forgotten once the defect is fixed.
#
# Process fields:
#   bin       Base name of the binary, without the platform suffix.
#   args      Command line arguments.
#   stdin     Sequence of (delay in seconds, line) written to the standard input.
#   quit      Lines that make the application leave through its own exit point.
#             They are written when the scenario is over, and the process is
#             only killed if it does not leave by itself afterwards.
#   must_exit true  -- the process ends on its own before the deadline, with
#                      code 0, without being asked.
#             false -- the process is a demonstration that keeps running until
#                      it is told to stop. It has to survive the scenario and
#                      then leave when it is asked to.
#   has_exit  true  -- the application leaves through its own exit point, so
#                      having to terminate it is reported as a failure. Every
#                      example is expected to behave this way: the applications
#                      of a scenario are connected, and the one that quits makes
#                      the others quit as well.
#             false -- the application has no ending of its own and is expected
#                      to be terminated. Reserved for applications written that
#                      way in the future; nothing uses it today.
#   expect    Regular expressions that all have to match the captured output.
# ---------------------------------------------------------------------------

def proc(binary, args=None, stdin=None, must_exit=True, expect=None, quit=None,
         has_exit=True):
    return {
        'bin': binary,
        'args': args or [],
        'stdin': stdin or [],
        'quit': quit or [],
        'must_exit': must_exit,
        'has_exit': has_exit,
        'expect': expect or [],
    }


SCENARIOS = [
    # -- single process, no router ------------------------------------------
    {'name': '01_minimalrpc', 'tier': 'smoke', 'procs': [proc('01_minimalrpc')]},
    {'name': '03_onethread', 'tier': 'smoke', 'procs': [proc('03_onethread')]},
    {'name': '03_twothreads', 'tier': 'smoke', 'procs': [proc('03_twothreads')]},
    {'name': '04_hellothread', 'tier': 'smoke', 'procs': [proc('04_hellothread')]},
    {'name': '05_buffer', 'tier': 'smoke', 'procs': [proc('05_buffer')]},
    {'name': '06_file', 'tier': 'smoke', 'procs': [proc('06_file')]},
    {'name': '07_logging', 'tier': 'smoke', 'procs': [proc('07_logging')]},
    {'name': '08_timer', 'tier': 'smoke', 'procs': [proc('08_timer')]},
    {'name': '09_threads', 'tier': 'smoke', 'procs': [proc('09_threads')]},
    {'name': '10_sync', 'tier': 'smoke', 'procs': [proc('10_sync')]},
    {'name': '11_service', 'tier': 'smoke', 'procs': [proc('11_service')]},
    {'name': '12_svcmulti', 'tier': 'smoke', 'procs': [proc('12_svcmulti')]},
    {'name': '13_locservice', 'tier': 'smoke', 'procs': [proc('13_locservice')]},
    {'name': '14_locmesh', 'tier': 'smoke', 'procs': [proc('14_locmesh')]},
    {'name': '21_locwatchdog', 'tier': 'smoke', 'timeout': 60,
     'procs': [proc('21_locservice',
                    expect=[r'Reached maximum number of service restarts'])]},
    {'name': '28_stlsync', 'tier': 'smoke', 'procs': [proc('28_stlsync')]},
    {'name': '29_syncevent', 'tier': 'smoke', 'procs': [proc('29_syncevent')]},

    # -- multiple processes, router required --------------------------------
    # The expected output is what proves that the messages really crossed the
    # process boundary. Without it a scenario where nothing was exchanged, but
    # every process ended tidily, would count as a pass.
    {'name': '02_minimalipc', 'tier': 'ipc',
     'procs': [proc('02_provideripc', expect=[r"'Hello Service!'"]),
               proc('02_consumeripc', expect=[r"'Good bye Service!'"])]},
    {'name': '03_helloservice', 'tier': 'ipc',
     'procs': [proc('03_pubservice', expect=[r"'Hello Service!' from"]),
               proc('03_pubclient', expect=[r'Succeeded to output message'])]},
    {'name': '15_pubsvc', 'tier': 'ipc',
     'procs': [proc('15_pubservice', expect=[r'remain to process \[ 0 \]']),
               proc('15_pubclient')]},
    {'name': '16_pubmesh', 'tier': 'ipc',
     'procs': [proc('16_pubservice'), proc('16_pubsvcmesh'),
               proc('16_pubclients', expect=[r'greetings succeeded'])]},
    # The client takes its direction once and then only displays, so the traffic
    # light drives the scenario: start, run for a while, quit. Quitting the
    # traffic light is what makes the client leave too.
    {'name': '17_pubtraffic', 'tier': 'ipc', 'timeout': 90,
     'procs': [proc('17_pubservice', stdin=[(3.0, 'start'), (12.0, 'quit')]),
               proc('17_pubclient', stdin=[(3.0, 'sn')],
                    expect=[r'Selected direction is South - North'])]},
    # The provider takes the patient data from its console and leaves when the
    # answer to "continue" is no. The expected client output is the same record
    # arriving on the other side, so it also proves the data was transferred.
    {'name': '18_pubworker', 'tier': 'ipc',
     'procs': [proc('18_pubservice',
                    stdin=[(4.0, 'John'), (0.3, 'Doe'), (0.3, '72.5'), (0.3, '35'),
                           (3.0, 'n')]),
               proc('18_pubclient',
                    expect=[r'Example Hardware is initialized',
                            r'First Name \.+: John',
                            r'Last Name \.+: Doe',
                            r'Patient age \.+: 35'])]},
    # The client picks its direction before it starts, and it has no command of
    # its own: it leaves when the traffic light is gone. So the provider drives
    # the whole scenario through its menu, power on, run, power off, quit.
    {'name': '19_pubfsm', 'tier': 'ipc', 'timeout': 90,
     'procs': [proc('19_pubservice',
                    stdin=[(4.0, 'on'), (3.0, 'start'), (10.0, 'off'), (2.0, 'quit')]),
               proc('19_pubclient', stdin=[(0.0, 'sn')],
                    expect=[r'Selected Choice: South-North traffic',
                            r'Vehicle Light:'])]},
    # Ends once the watchdog has restarted the blocked provider thread often
    # enough, so it also proves the restart works across processes.
    {'name': '22_pubwatchdog', 'tier': 'ipc', 'timeout': 90,
     'procs': [proc('22_pubservice'), proc('22_pubclient')]},
    {'name': '24_pubunblock', 'tier': 'ipc',
     'procs': [proc('24_pubservice', expect=[r'<<< Response at']),
               proc('24_pubclient', expect=[r'<<< Response at'])]},
    # The publisher of the pub/sub examples drives the session from its console
    # prompt: 's' starts publishing, 'i' invalidates the data, 'q' quits.
    {'name': '25_pubsub', 'tier': 'ipc',
     'procs': [proc('25_subscriber', expect=[r'=> \d+ \{ changed \}']),
               proc('25_publisher', stdin=[(3.0, 's'), (10.0, 'i'), (5.0, 'q')])]},
    {'name': '26_pubsubmix', 'tier': 'ipc',
     'procs': [proc('26_pubsubdyn', expect=[r'=> string_\d+']),
               proc('26_pubsubctrl', stdin=[(3.0, 's'), (10.0, 'i'), (5.0, 'q')])]},
    {'name': '27_pubsubmulti', 'tier': 'ipc',
     'procs': [proc('27_subscribermulti', expect=[r'=> \d+ \{ changed \}']),
               proc('27_publisher', stdin=[(3.0, 's'), (10.0, 'i'), (5.0, 'q')])]},

    # -- benchmarks, not part of the regular run ----------------------------
    # The client has no command of its own, it leaves when the provider leaves.
    {'name': '23_pubdatarate', 'tier': 'perf', 'timeout': 180,
     'procs': [proc('23_pubclient'),
               proc('23_pubservice',
                    stdin=[(4.0, '-w=1024 -h=1024 -l=1024 -c=25 -t=28'),
                           (4.0, '-s'), (40.0, '-q')],
                    expect=[r'Network sent rate'])]},
    {'name': '30_publatency', 'tier': 'perf', 'timeout': 240,
     'procs': [proc('30_pubprovider', must_exit=False, quit=['-q']),
               proc('30_pubconsumer',
                    stdin=[(4.0, '-m=pp64 -c=10000 -w=1000'),
                           (4.0, '-s=3'), (90.0, '-q')],
                    expect=[r'P50'])]},
]

# Exit codes that mean the process was terminated abnormally.
WIN_CRASH_CODES = {
    0xC0000005: 'access violation',
    0xC0000017: 'no memory',
    0xC000001D: 'illegal instruction',
    0xC0000025: 'non continuable exception',
    0xC0000026: 'invalid disposition',
    0xC0000094: 'integer divide by zero',
    0xC00000FD: 'stack overflow',
    0xC0000135: 'dll not found',
    0xC0000139: 'entry point not found',
    0xC0000142: 'dll init failed',
    0xC0000409: 'stack buffer overrun',
    0xC0000374: 'heap corruption',
    0xC000041D: 'unhandled exception in callback',
    0x80000003: 'breakpoint',
    3: 'abort',
}


def describe_exit(code):
    """Returns a readable reason when the exit code means a crash, else None."""
    if code is None:
        return None
    if IS_WINDOWS:
        unsigned = code & 0xFFFFFFFF
        if unsigned in WIN_CRASH_CODES:
            return WIN_CRASH_CODES[unsigned]
        if unsigned >= 0x80000000:
            return 'exception 0x%08X' % unsigned
        return None
    if code < 0:
        try:
            import signal
            return 'signal %s' % signal.Signals(-code).name
        except (ImportError, ValueError):
            return 'signal %d' % -code
    if code == 134:
        return 'abort'
    if code == 139:
        return 'segmentation fault'
    return None


# ---------------------------------------------------------------------------
# Process handling.
# ---------------------------------------------------------------------------

class Runner:
    """Starts one process, feeds its standard input and captures its output.

    On POSIX the output is captured through a pseudo terminal rather than by writing
    straight into the log file. The reason is that the C library buffers a redirected
    standard output in blocks of 4 KB, and a process that has to be terminated -- which is
    exactly the case that has to be investigated -- dies with that buffer still unwritten.
    Every log of a killed process was therefore empty. A terminal makes the library switch
    to line buffering, so every completed line is in the log the moment it is printed and
    at most the current partial line is lost.
    """

    def __init__(self, path, args, stdin_script, log_path, quit_script=None, use_pty=True):
        self.path = path
        self.name = os.path.basename(path)
        self.log_path = log_path
        self.output = ''
        self.killed = False
        self._quit_script = quit_script or []
        self._stdin_script = stdin_script
        self._log = open(log_path, 'wb')
        self._log_lock = threading.Lock()
        self._pty_master = None
        self._drainer = None
        kwargs = {
            'stdin': subprocess.PIPE,
            'cwd': os.path.dirname(path),
        }
        if IS_WINDOWS:
            kwargs['creationflags'] = subprocess.CREATE_NEW_PROCESS_GROUP
        else:
            kwargs['start_new_session'] = True

        slave = None
        if (pty is not None) and use_pty:
            self._pty_master, slave = pty.openpty()
            _set_pty_size(slave, PTY_ROWS, PTY_COLS)
            kwargs['stdout'] = slave
            kwargs['stderr'] = slave
            # A terminal without TERM makes the curses based applications give up. The
            # value is only a default: an environment that already sets it is left alone.
            environment = os.environ.copy()
            environment.setdefault('TERM', 'xterm-256color')
            kwargs['env'] = environment
        else:
            kwargs['stdout'] = self._log
            kwargs['stderr'] = subprocess.STDOUT

        try:
            self.proc = subprocess.Popen([path] + args, **kwargs)
        finally:
            if slave is not None:
                os.close(slave)

        if self._pty_master is not None:
            self._drainer = threading.Thread(target=self._drain, daemon=True)
            self._drainer.start()

        self._feeder = threading.Thread(target=self._feed, daemon=True)
        self._feeder.start()

    def _drain(self):
        """Copies the pseudo terminal into the log file until the process closes it.

        The line discipline turns every '\\n' into '\\r\\n' on the way out. The pair is
        folded back, so that the captured output stays comparable with the output of the
        runs that write into the log file directly.
        """
        pending = b''
        try:
            while True:
                chunk = os.read(self._pty_master, 65536)
                if not chunk:
                    break
                data = pending + chunk
                pending = b''
                if data.endswith(b'\r'):
                    # A '\r' at the very end may be the first half of a '\r\n' pair that
                    # has not arrived yet. Hold it back until the next read.
                    data, pending = data[:-1], b'\r'
                self._write_log(data.replace(b'\r\n', b'\n'))
        except OSError:
            # EIO on Linux once the last writing end of the terminal is closed.
            pass
        finally:
            if pending:
                self._write_log(pending)

    def _write_log(self, data):
        with self._log_lock:
            if not self._log.closed:
                self._log.write(data)
                self._log.flush()

    def _feed(self):
        """Writes the scripted input lines, then leaves the pipe open.

        The pipe stays open on purpose. Closing it makes the console read
        return end of file, which is a different code path than a user who
        simply types nothing.
        """
        try:
            for delay, line in self._stdin_script:
                time.sleep(delay)
                if self.proc.poll() is not None:
                    return
                self.proc.stdin.write((line + '\n').encode('ascii', 'ignore'))
                self.proc.stdin.flush()
        except (OSError, ValueError):
            pass

    def is_running(self):
        return self.proc.poll() is None

    def _write_line(self, line):
        try:
            self.proc.stdin.write((line + '\n').encode('ascii', 'ignore'))
            self.proc.stdin.flush()
            return True
        except (OSError, ValueError):
            return False

    def request_quit(self):
        """Asks the application to leave through its own exit point.

        Returns True when the process ended on its own. An application that is
        killed loses whatever it still had in its output buffers, so leaving on
        request is also what makes the captured output complete.
        """
        if not self._quit_script:
            return False
        for line in self._quit_script:
            if self.proc.poll() is not None:
                return True
            if not self._write_line(line):
                break
            time.sleep(0.3)

        deadline = time.time() + QUIT_SECONDS
        while (self.proc.poll() is None) and (time.time() < deadline):
            time.sleep(0.2)
        return self.proc.poll() is not None

    def stop(self):
        """Ends the process, preferring its own exit point over a kill."""
        if self.proc.poll() is None:
            self.request_quit()
        if self.proc.poll() is None:
            self.killed = True
            if IS_WINDOWS:
                subprocess.run(['taskkill', '/PID', str(self.proc.pid), '/T', '/F'],
                               stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            else:
                import signal as sig
                # SIGABRT first: a process that has to be killed is a process that hung,
                # and the core dump is the only record of where it hung. SIGTERM and
                # SIGKILL leave nothing behind, not even the buffered output.
                try:
                    os.killpg(os.getpgid(self.proc.pid), sig.SIGABRT)
                except OSError:
                    pass
                deadline = time.time() + 5.0
                while (self.proc.poll() is None) and (time.time() < deadline):
                    time.sleep(0.1)
                if self.proc.poll() is None:
                    try:
                        os.killpg(os.getpgid(self.proc.pid), sig.SIGTERM)
                    except OSError:
                        pass
                    deadline = time.time() + 5.0
                    while (self.proc.poll() is None) and (time.time() < deadline):
                        time.sleep(0.1)
                if self.proc.poll() is None:
                    try:
                        os.killpg(os.getpgid(self.proc.pid), sig.SIGKILL)
                    except OSError:
                        pass
        try:
            self.proc.wait(timeout=10)
        except subprocess.TimeoutExpired:
            pass
        self.close()

    def close(self):
        try:
            if self.proc.stdin is not None:
                self.proc.stdin.close()
        except OSError:
            pass

        if self._drainer is not None:
            # The terminal reaches the end of file when the process and everything it
            # started have closed it. A grandchild that outlives the parent would keep the
            # reader waiting, so the wait is bounded.
            self._drainer.join(timeout=5.0)
            self._drainer = None
        if self._pty_master is not None:
            try:
                os.close(self._pty_master)
            except OSError:
                pass
            self._pty_master = None

        with self._log_lock:
            if not self._log.closed:
                self._log.close()
                closed = True
            else:
                closed = False
        if closed:
            with open(self.log_path, 'rb') as src:
                self.output = src.read().decode('utf-8', 'replace')


def _set_pty_size(fd, rows, cols):
    """Gives the pseudo terminal a usable geometry. Failure is not fatal."""
    try:
        fcntl.ioctl(fd, termios.TIOCSWINSZ, struct.pack('HHHH', rows, cols, 0, 0))
    except OSError:
        pass


def is_router_listening():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(0.5)
    try:
        return sock.connect_ex((ROUTER_HOST, ROUTER_PORT)) == 0
    finally:
        sock.close()


def find_binary(bin_dir, name):
    for suffix in EXE_SUFFIXES:
        candidate = os.path.join(bin_dir, name + suffix)
        if os.path.isfile(candidate) and os.access(candidate, os.X_OK):
            return candidate
    return None


def find_bin_dirs(root):
    """Returns every built binary directory, the most recently built one first."""
    build = os.path.join(root, 'product', 'build')
    found = []
    if os.path.isdir(build):
        for compiler in os.listdir(build):
            target = os.path.join(build, compiler)
            if not os.path.isdir(target):
                continue
            for config in os.listdir(target):
                candidate = os.path.join(target, config, 'bin')
                if os.path.isdir(candidate):
                    found.append((os.path.getmtime(candidate), candidate))
    found.sort(reverse=True)
    return [entry[1] for entry in found]


# ---------------------------------------------------------------------------
# Scenario execution.
# ---------------------------------------------------------------------------

def one_line(text):
    """Folds a note into a single line, for the places that need one."""
    return ' | '.join(part.strip() for part in str(text).splitlines() if part.strip())


CAPTURE_EXCERPT_BYTES = 1024    #!< head and tail of the captured stream kept in a report
CAPTURE_HEXDUMP_BYTES = 256     #!< bytes of the hexdump added when the stream is not text


def describe_capture(runner):
    """Renders what the harness really received from one process.

    An expectation that does not match has three possible causes, and they need
    different work: the text was never printed, it was printed and the capture lost
    it, or it was printed and arrived damaged. Only the bytes tell them apart, so the
    report carries them instead of pointing at an artifact that has to be downloaded.
    """
    try:
        with open(runner.log_path, 'rb') as src:
            data = src.read()
    except OSError as err:
        return 'captured output of %s is unreadable: %s' % (runner.name, err)

    lines = ['captured %u bytes from %s (%s)' % (len(data), runner.name, runner.log_path)]
    if len(data) <= 2 * CAPTURE_EXCERPT_BYTES:
        excerpt = repr(data)
    else:
        excerpt = '%r ... %r' % (data[:CAPTURE_EXCERPT_BYTES], data[-CAPTURE_EXCERPT_BYTES:])
    lines.append('  text: ' + excerpt)

    # Bytes outside the printable ASCII range plus tab, newline and escape mean the
    # stream is not what a console application prints. The hexdump makes the shape of
    # the damage readable: a pointer, a run of zeros, a truncated line.
    printable = set(range(0x20, 0x7F)) | {0x09, 0x0A, 0x0D, 0x1B}
    odd = [i for i, b in enumerate(data) if b not in printable]
    if odd:
        first = odd[0]
        start = max(0, (first - 16) & ~0x0F)
        block = data[start:start + CAPTURE_HEXDUMP_BYTES]
        lines.append('  %u byte(s) outside printable ASCII, first at offset %u' % (len(odd), first))
        for offset in range(0, len(block), 16):
            row = block[offset:offset + 16]
            hexpart = ' '.join('%02x' % b for b in row)
            txtpart = ''.join(chr(b) if 0x20 <= b < 0x7F else '.' for b in row)
            lines.append('  %08x  %-47s  %s' % (start + offset, hexpart, txtpart))

    return '\n       '.join(lines)


class Result:
    def __init__(self, name):
        self.name = name
        self.status = 'PASS'
        self.notes = []
        self.seconds = 0.0

    def fail(self, note):
        self.status = 'FAIL'
        self.notes.append(note)

    def skip(self, note):
        if self.status == 'PASS':
            self.status = 'SKIP'
            self.notes.append(note)


def run_scenario(scenario, bin_dir, out_dir, timeout, router_bin, keep_logs, use_pty=True,
                 round_index=0, rounds=1):
    result = Result(scenario['name'])
    started = time.time()

    paths = []
    for entry in scenario['procs']:
        path = find_binary(bin_dir, entry['bin'])
        if path is None:
            result.skip('binary not built: %s' % entry['bin'])
            return result
        paths.append(path)

    # Every round owns its directory. A defect that is hunted with --repeat shows up in
    # one round out of many, and a shared directory means the round after it overwrites
    # the only evidence there was.
    scenario_dir = os.path.join(out_dir, scenario['name'])
    if rounds > 1:
        scenario_dir = os.path.join(scenario_dir, 'round-%03d' % (round_index + 1))
    os.makedirs(scenario_dir, exist_ok=True)

    router = None
    router_was_running = False
    if scenario['tier'] in ('ipc', 'perf'):
        router_was_running = is_router_listening()
        if router_was_running:
            # Somebody else owns this router. Use it and never touch it.
            pass
        elif router_bin is None:
            result.skip('mtrouter is not built and none is running')
            return result
        else:
            router = Runner(router_bin, [], [], os.path.join(scenario_dir, 'mtrouter.log'),
                            ['-q'], use_pty)
            deadline = time.time() + ROUTER_READY_SECONDS
            while (time.time() < deadline) and not is_router_listening():
                if not router.is_running():
                    break
                time.sleep(0.2)
            if not is_router_listening():
                router.stop()
                result.fail('mtrouter did not start listening on port %d' % ROUTER_PORT)
                return result

    runners = []
    try:
        for index, entry in enumerate(scenario['procs']):
            log = os.path.join(scenario_dir, '%d_%s.log' % (index, entry['bin']))
            runners.append(Runner(paths[index], entry['args'], entry['stdin'], log,
                                  entry['quit'], use_pty))
            if index + 1 < len(scenario['procs']):
                time.sleep(1.0)      # let the provider register before the consumer looks it up

        soak = scenario.get('soak')
        if soak is not None:
            # The scenario has no natural end. Everything has to survive the
            # soak period; a process that disappears early is a failure.
            deadline = time.time() + soak
            while time.time() < deadline:
                broken = False
                for runner in runners:
                    if runner.is_running():
                        continue
                    runner.close()
                    code = runner.proc.returncode
                    reason = describe_exit(code)
                    if reason is not None:
                        result.fail('%s crashed %d s into the %d s soak: %s'
                                    % (runner.name, int(soak - (deadline - time.time())),
                                       soak, reason))
                        broken = True
                    elif code != 0:
                        result.fail('%s exited with code %d %d s into the %d s soak'
                                    % (runner.name, code,
                                       int(soak - (deadline - time.time())), soak))
                        broken = True
                # A clean early exit is fine: the scenario simply reached its end.
                if broken or all(not r.is_running() for r in runners):
                    break
                time.sleep(0.5)
        else:
            lead = runners[-1]
            lead_entry = scenario['procs'][-1]
            deadline = time.time() + timeout
            while (time.time() < deadline) and lead.is_running():
                time.sleep(0.2)

            if lead.is_running():
                if lead_entry['must_exit']:
                    result.fail('%s did not finish within %d s' % (lead.name, timeout))
                lead.stop()
            else:
                code = lead.proc.returncode
                crash = describe_exit(code)
                if crash is not None:
                    result.fail('%s crashed: %s' % (lead.name, crash))
                elif code != 0:
                    result.fail('%s exited with code %d' % (lead.name, code))
                lead.close()

            # Give the helper processes a chance to end on their own.
            grace = time.time() + GRACE_SECONDS
            while time.time() < grace:
                if all(not r.is_running() for r in runners[:-1]):
                    break
                time.sleep(0.2)

            for index, helper in enumerate(runners[:-1]):
                entry = scenario['procs'][index]
                if helper.is_running():
                    if entry['must_exit']:
                        result.fail('%s was still running %d s after %s finished'
                                    % (helper.name, int(GRACE_SECONDS), lead.name))
                    helper.stop()
                else:
                    code = helper.proc.returncode
                    crash = describe_exit(code)
                    if crash is not None:
                        result.fail('%s crashed: %s' % (helper.name, crash))
                    elif (code != 0) and entry['must_exit']:
                        result.fail('%s exited with code %d' % (helper.name, code))
                    helper.close()
    finally:
        for runner in runners:
            if runner.is_running():
                runner.stop()
            else:
                runner.close()
        if router is not None:
            if router.is_running():
                router.stop()
            else:
                result.fail('mtrouter stopped before the scenario finished')
                router.close()
        if router_was_running:
            result.notes.append('used the already running mtrouter')

        # An application of a scenario that had to be terminated did not reach
        # its own exit point. The applications of a scenario are connected, so
        # the one that stayed behind is the one to look at.
        killed = [runners[i].name for i in range(len(runners))
                  if runners[i].killed and scenario['procs'][i]['has_exit']]
        if (router is not None) and router.killed:
            killed.append(router.name)
        if killed:
            result.fail('terminated, own exit point not reached: %s' % ', '.join(killed))

    # Every process is closed by now, so the captured output is complete.
    for index, entry in enumerate(scenario['procs']):
        if index >= len(runners):
            break
        missed = False
        for pattern in entry['expect']:
            if re.search(pattern, runners[index].output) is None:
                result.fail('%s output does not contain %r' % (runners[index].name, pattern))
                missed = True
        if missed:
            # "does not contain" alone does not say whether the text was never printed,
            # was printed and lost, or arrived damaged. The captured stream answers that,
            # so it goes into the report and not only into the artifact.
            result.notes.append(describe_capture(runners[index]))

    result.seconds = time.time() - started

    known = scenario.get('known')
    if known is not None:
        if result.status == 'FAIL':
            result.status = 'XFAIL'
            result.notes.append('known: %s' % known)
        elif result.status == 'PASS':
            result.status = 'XPASS'
            result.notes.append('passed although marked as a known defect (%s),'
                                ' remove the marker' % known)

    if (result.status == 'PASS') and not keep_logs:
        shutil.rmtree(scenario_dir, ignore_errors=True)
    return result


# ---------------------------------------------------------------------------
# Reports.
# ---------------------------------------------------------------------------

def _xml_text(value):
    return (str(value).replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')
                      .replace('"', '&quot;'))


def write_junit(path, results, seconds):
    """Writes the results in the JUnit format, understood by every build server."""
    failed = sum(1 for r in results if r.status in ('FAIL', 'XPASS'))
    skipped = sum(1 for r in results if r.status in ('SKIP', 'XFAIL'))
    lines = ['<?xml version="1.0" encoding="UTF-8"?>',
             '<testsuites>',
             '  <testsuite name="areg-examples" tests="%d" failures="%d" skipped="%d" time="%.3f">'
             % (len(results), failed, skipped, seconds)]
    for result in results:
        note = _xml_text('; '.join(one_line(n) for n in result.notes))
        lines.append('    <testcase classname="areg.examples" name="%s" time="%.3f">'
                     % (_xml_text(result.name), result.seconds))
        if result.status in ('FAIL', 'XPASS'):
            lines.append('      <failure message="%s">%s</failure>' % (note, note))
        elif result.status in ('SKIP', 'XFAIL'):
            lines.append('      <skipped message="%s"/>' % note)
        lines.append('    </testcase>')
    lines.append('  </testsuite>')
    lines.append('</testsuites>')
    with open(path, 'w') as report:
        report.write('\n'.join(lines) + '\n')


def markdown_report(results, seconds, tier, bin_dir):
    """Builds the Markdown report shown on the build server result page."""
    icons = {'PASS': 'ok', 'FAIL': 'FAILED', 'XFAIL': 'known defect',
             'XPASS': 'FAILED (fixed, remove the marker)', 'SKIP': 'not built'}
    passed = sum(1 for r in results if r.status == 'PASS')
    failed = sum(1 for r in results if r.status in ('FAIL', 'XPASS'))
    known = sum(1 for r in results if r.status == 'XFAIL')
    skipped = sum(1 for r in results if r.status == 'SKIP')

    lines = ['## Example applications (%s)' % tier, '']
    lines.append('**%s** -- %d passed, %d failed, %d known defects, %d not built, %d s total'
                 % ('FAILED' if failed else 'passed', passed, failed, known, skipped, int(seconds)))
    lines.append('')
    lines.append('Binaries: `%s`' % bin_dir)
    lines.append('')
    lines.append('| Scenario | Result | Time | Details |')
    lines.append('|---|---|---|---|')
    for result in results:
        lines.append('| %s | %s | %.1f s | %s |'
                     % (result.name, icons.get(result.status, result.status),
                        result.seconds,
                        '; '.join(one_line(n) for n in result.notes).replace('|', '/')))
    lines.append('')
    return '\n'.join(lines)


def annotate(results):
    """Prints build server annotations, so failures show up next to the job."""
    if os.environ.get('GITHUB_ACTIONS') != 'true':
        return
    for result in results:
        note = '; '.join(one_line(n) for n in result.notes) or result.status
        if result.status in ('FAIL', 'XPASS'):
            print('::error title=%s::%s' % (result.name, note))
        elif result.status == 'XFAIL':
            print('::warning title=%s::%s' % (result.name, note))


# ---------------------------------------------------------------------------
# Entry point.
# ---------------------------------------------------------------------------

def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

    parser = argparse.ArgumentParser(
        description='Run the Areg SDK example applications as a test suite.')
    parser.add_argument('--tier', default='all', choices=['smoke', 'ipc', 'all', 'perf'],
                        help='which set of scenarios to run (default: all)')
    parser.add_argument('--bin-dir', default=os.environ.get('AREG_BIN_DIR'),
                        help='directory with the built binaries')
    parser.add_argument('--out-dir', default=None,
                        help='where to write the captured output')
    parser.add_argument('--timeout', type=int, default=120,
                        help='per scenario deadline in seconds (default: 120)')
    parser.add_argument('--repeat', type=int, default=1,
                        help='run the selection several times')
    parser.add_argument('--only', default=None,
                        help='comma separated list of scenario names')
    parser.add_argument('--list', action='store_true', help='print the scenarios and exit')
    parser.add_argument('--keep-logs', action='store_true',
                        help='keep the captured output of the scenarios that passed')
    parser.add_argument('--no-pty', action='store_true',
                        help='capture the output by redirecting it into the log file instead'
                             ' of through a pseudo terminal (POSIX only). The output of a'
                             ' process that has to be terminated is then lost.')
    parser.add_argument('--timestamp', action='store_true',
                        help='write the output into a time stamped sub-directory')
    parser.add_argument('--junit', default=None,
                        help='write the results as a JUnit report (default: results.xml'
                             ' in the output directory)')
    parser.add_argument('--summary', default=os.environ.get('GITHUB_STEP_SUMMARY'),
                        help='append a Markdown report to this file')
    args = parser.parse_args()

    selection = SCENARIOS
    if args.tier != 'all':
        selection = [s for s in selection if s['tier'] == args.tier]
    else:
        selection = [s for s in selection if s['tier'] != 'perf']
    if args.only:
        wanted = {n.strip() for n in args.only.split(',') if n.strip()}
        selection = [s for s in SCENARIOS if s['name'] in wanted]
        missing = wanted - {s['name'] for s in selection}
        if missing:
            print('unknown scenario: %s' % ', '.join(sorted(missing)), file=sys.stderr)
            return 1

    if args.list:
        for scenario in selection:
            print('%-16s %-6s %s' % (scenario['name'], scenario['tier'],
                                     ' + '.join(p['bin'] for p in scenario['procs'])))
        return 0

    bin_dir = args.bin_dir
    others = []
    if bin_dir is None:
        candidates = find_bin_dirs(root)
        if candidates:
            bin_dir = candidates[0]
            others = candidates[1:]
    if (bin_dir is None) or not os.path.isdir(bin_dir):
        print('binary directory not found, build the examples or pass --bin-dir',
              file=sys.stderr)
        return 1
    bin_dir = os.path.abspath(bin_dir)

    out_dir = args.out_dir or os.path.join(root, 'product', 'test-results')
    if (args.out_dir is None) and args.timestamp:
        out_dir = os.path.join(out_dir, time.strftime('%Y%m%d-%H%M%S'))
    out_dir = os.path.abspath(out_dir)
    os.makedirs(out_dir, exist_ok=True)

    router_bin = find_binary(bin_dir, 'mtrouter')

    print('binaries : %s' % bin_dir)
    print('output   : %s' % out_dir)
    print('scenarios: %d, tier %s, timeout %d s' % (len(selection), args.tier, args.timeout))
    for other in others:
        print('note     : another build is present, not used: %s' % other)
    if others:
        print('note     : pass --bin-dir to choose, the most recently built one was taken')
    if is_router_listening():
        print('note     : a message router is already listening on port %d, it will be used'
              ' and left alone' % ROUTER_PORT)
    print('')

    started = time.time()
    results = []
    failures = []
    known = []
    skipped = 0
    passed = 0
    for round_index in range(args.repeat):
        if args.repeat > 1:
            print('--- round %d of %d ---' % (round_index + 1, args.repeat))
        for scenario in selection:
            sys.stdout.write('%-16s ' % scenario['name'])
            sys.stdout.flush()
            timeout = scenario.get('timeout', args.timeout)
            result = run_scenario(scenario, bin_dir, out_dir, timeout, router_bin,
                                  args.keep_logs, not args.no_pty,
                                  round_index, args.repeat)
            print('%-5s %6.1fs %s'
                  % (result.status, result.seconds,
                     '; '.join(one_line(n) for n in result.notes)))
            sys.stdout.flush()
            results.append(result)
            if result.status in ('FAIL', 'XPASS'):
                failures.append(result)
            elif result.status == 'XFAIL':
                known.append(result)
            elif result.status == 'SKIP':
                skipped += 1
            else:
                passed += 1

    elapsed = time.time() - started
    report = markdown_report(results, elapsed, args.tier, bin_dir)
    with open(os.path.join(out_dir, 'summary.md'), 'w') as summary:
        summary.write(report)
    write_junit(args.junit or os.path.join(out_dir, 'results.xml'), results, elapsed)
    if args.summary:
        with open(args.summary, 'a') as summary:
            summary.write(report)
    annotate(results)

    print('')
    print('passed %d, failed %d, known defects %d, skipped %d'
          % (passed, len(failures), len(known), skipped))
    if known:
        print('')
        for result in known:
            print('KNOWN DEFECT %s' % result.name)
            for note in result.notes:
                print('     %s' % note)
    if failures:
        print('')
        for result in failures:
            print('%s %s' % (result.status, result.name))
            for note in result.notes:
                print('     %s' % note)
        print('')
        print('captured output: %s' % out_dir)
        return 1

    return 0


if __name__ == '__main__':
    sys.exit(main())
