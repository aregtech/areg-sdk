#!/usr/bin/env python3
# ---------------------------------------------------------------------------
# This file is part of the AREG SDK.
# Reports the flash and RAM footprint of the built areg artefacts.
# ---------------------------------------------------------------------------
"""
Reports what an areg deployment costs a device: the static size of every artefact
(flash) and the resident memory of the running processes (RAM).

It builds nothing. It reads a build directory that already exists, so the same script
serves a native build, a cross build and a CI job that only wants the sizes.

    python3 tools/footprint.py                          # flash and RAM, build/bin
    python3 tools/footprint.py --flash-only             # sizes only, no process is started
    python3 tools/footprint.py --size-tool arm-linux-gnueabihf-size --flash-only
    python3 tools/footprint.py --json footprint.json    # the same numbers as data

Every figure carries the commit, the compiler, the build type and the machine, so a
number copied out of the output says what it was measured on.

RAM is measured on Linux only: it comes from /proc/<pid>/status. VmHWM is the peak, and
it is the figure a device has to hold. Under an emulator the numbers belong to the
emulator, so a cross build reports flash only.
"""

import argparse
import json
import os
import platform
import re
import shutil
import subprocess
import sys
import time

ROUTER_PORT_DEFAULT = 8181
SAMPLE_INTERVAL = 0.05
ROUTER_READY_SECONDS = 15.0

# The artefacts a deployment stores, and what each one is for. A name that is not built
# is reported as absent rather than skipped silently: a missing mtrouter changes what the
# table means.
FLASH_ARTEFACTS = [
    ('libareg.so',          'the framework'),
    ('libareg.a',           'the framework, static'),
    ('libareglogger.so',    'the log observer client library'),
    ('mtrouter.elf',        'the message router'),
    ('logcollector.elf',    'the log collector'),
    ('logobserver.elf',     'the log observer'),
    ('01_minimalrpc.elf',   'a one-process application'),
    ('02_provideripc.elf',  'a minimal service provider'),
    ('02_consumeripc.elf',  'a minimal service consumer'),
]

# The console key that stops an application that reads one. Anything else is terminated.
QUIT_KEY = '-q'


def _run(command, timeout=30):
    """Runs a command and returns its stdout, or None when it cannot be run."""
    try:
        done = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
                              timeout=timeout, universal_newlines=True)
    except (OSError, subprocess.SubprocessError):
        return None
    return done.stdout if done.returncode == 0 else None


# ---------------------------------------------------------------------------
# Provenance
# ---------------------------------------------------------------------------

def _cmake_cache_value(build_dir, key):
    """Returns the value of one CMakeCache entry, or None."""
    cache = os.path.join(build_dir, 'CMakeCache.txt')
    if not os.path.isfile(cache):
        return None
    prefix = key + ':'
    with open(cache, 'r', errors='replace') as handle:
        for line in handle:
            if line.startswith(prefix) and '=' in line:
                return line.split('=', 1)[1].strip()
    return None


def _cmake_compiler(build_dir):
    """Returns the compiler CMake recorded for this build directory, as id, version and path."""
    root = os.path.join(build_dir, 'CMakeFiles')
    if not os.path.isdir(root):
        return None
    wanted = ('CMAKE_CXX_COMPILER', 'CMAKE_CXX_COMPILER_ID', 'CMAKE_CXX_COMPILER_VERSION')
    for entry in sorted(os.listdir(root)):
        recorded = os.path.join(root, entry, 'CMakeCXXCompiler.cmake')
        if not os.path.isfile(recorded):
            continue
        found = {}
        with open(recorded, 'r', errors='replace') as handle:
            for line in handle:
                match = re.match(r'\s*set\((\S+)\s+"([^"]*)"\)', line)
                if match and match.group(1) in wanted:
                    found[match.group(1)] = match.group(2)
        if found.get('CMAKE_CXX_COMPILER_ID'):
            return '%s %s (%s)' % (found['CMAKE_CXX_COMPILER_ID'],
                                   found.get('CMAKE_CXX_COMPILER_VERSION', '?'),
                                   found.get('CMAKE_CXX_COMPILER', '?'))
    return None


ELF_MACHINES = {
    0x03: 'i386',
    0x08: 'mips',
    0x28: 'arm',
    0x3E: 'x86_64',
    0xB7: 'aarch64',
    0xF3: 'riscv',
}

HOST_ALIASES = {
    'x86_64': 'x86_64', 'amd64': 'x86_64',
    'aarch64': 'aarch64', 'arm64': 'aarch64',
    'armv7l': 'arm', 'armv6l': 'arm', 'armhf': 'arm',
    'i686': 'i386', 'i386': 'i386', 'x86': 'i386',
}


def _elf_target(path):
    """Reads the architecture out of an ELF header. Returns None for anything else."""
    try:
        with open(path, 'rb') as handle:
            header = handle.read(20)
    except OSError:
        return None
    if len(header) < 20 or header[:4] != b'\x7fELF':
        return None
    order = 'little' if header[5] == 1 else 'big'
    machine = int.from_bytes(header[18:20], order)
    bits = 64 if header[4] == 2 else 32
    return '%s (%d-bit)' % (ELF_MACHINES.get(machine, 'machine 0x%X' % machine), bits)


def _measured_target(bin_dir, lib_dir):
    """The architecture of the binaries being measured, not of the machine reading them."""
    for directory in (bin_dir, lib_dir):
        if not os.path.isdir(directory):
            continue
        for name, _purpose in FLASH_ARTEFACTS:
            target = _elf_target(os.path.join(directory, name))
            if target:
                return target
    return None


def collect_provenance(build_dir, bin_dir, size_tool, lib_dir=None):
    """Collects what the numbers were measured on."""
    head = _run(['git', 'rev-parse', '--short', 'HEAD'])
    status = _run(['git', 'status', '--porcelain'])
    version = _cmake_compiler(build_dir)
    compiler = _cmake_cache_value(build_dir, 'CMAKE_CXX_COMPILER')

    return {
        'commit': head.strip() if head else 'unknown',
        'tree': 'modified' if (status and status.strip()) else 'clean',
        'build_dir': build_dir,
        'bin_dir': bin_dir,
        'build_type': _cmake_cache_value(build_dir, 'CMAKE_BUILD_TYPE') or 'unknown',
        'cxx_flags': _cmake_cache_value(build_dir, 'CMAKE_CXX_FLAGS') or '',
        'compiler': version or (compiler or 'unknown'),
        'logging': _cmake_cache_value(build_dir, 'AREG_LOGGING') or 'unknown',
        'extended': _cmake_cache_value(build_dir, 'AREG_EXTENDED') or 'unknown',
        'no_exceptions': _cmake_cache_value(build_dir, 'AREG_NO_EXCEPTIONS') or 'unknown',
        'target': _measured_target(bin_dir, lib_dir or bin_dir) or 'unknown',
        'host': '%s %s' % (platform.system(), platform.machine()),
        'size_tool': size_tool,
    }


# ---------------------------------------------------------------------------
# Flash
# ---------------------------------------------------------------------------

def measure_flash(bin_dir, lib_dir, size_tool):
    """
    Returns one record per artefact: the Berkeley text, data and bss of the sections the
    loader brings in, and the bytes the file occupies on disk.
    """
    if shutil.which(size_tool) is None:
        return None, '%s is not on PATH' % size_tool

    records = []
    for name, purpose in FLASH_ARTEFACTS:
        path = os.path.join(bin_dir, name)
        if not os.path.isfile(path):
            path = os.path.join(lib_dir, name)
        if not os.path.isfile(path):
            records.append({'artefact': name, 'purpose': purpose, 'present': False})
            continue

        record = {'artefact': name, 'purpose': purpose, 'present': True,
                  'file_bytes': os.path.getsize(path)}

        # An archive is not a deployment figure. Its members are compiled with function
        # sections, which the Berkeley format does not count, and a device never stores
        # the archive: it stores the binary the linker produced from the part of it that
        # was used. Only the file size is reported, and the linked binaries carry the rest.
        if name.endswith('.a'):
            record['archive'] = True
            records.append(record)
            continue

        out = _run([size_tool, path])
        if out:
            # Berkeley format: a header line, then text data bss dec hex filename. An
            # archive prints one such row per member, so every row is added up: the figure
            # is what the whole artefact holds, not what its last object file holds.
            totals = [0, 0, 0]
            members = 0
            for line in out.splitlines():
                fields = line.split()
                if len(fields) >= 3 and all(field.isdigit() for field in fields[:3]):
                    members += 1
                    for index in range(3):
                        totals[index] += int(fields[index])
            if members:
                record['text'], record['data'], record['bss'] = totals
                record['flash'] = totals[0] + totals[1]
                record['members'] = members
        records.append(record)

    return records, None


# ---------------------------------------------------------------------------
# RAM
# ---------------------------------------------------------------------------

def _status_of(pid):
    """Reads VmRSS, VmHWM, RssAnon and the thread count of one process, in kB."""
    wanted = ('VmRSS', 'VmHWM', 'RssAnon', 'Threads')
    values = {}
    try:
        with open('/proc/%d/status' % pid, 'r', errors='replace') as handle:
            for line in handle:
                key, _, rest = line.partition(':')
                if key in wanted:
                    number = re.search(r'\d+', rest)
                    if number:
                        values[key] = int(number.group(0))
    except OSError:
        return None
    return values


class Process(object):
    """One started binary, its log, and the peak of every figure seen while it ran."""

    def __init__(self, name, path, args, log_path, cwd):
        self.name = name
        self.log = open(log_path, 'wb')
        self.peak = {}
        self.last = {}
        self.samples = 0
        # The binary runs with the bin directory as its working directory, so that it finds
        # config/areg.init. Its own path is therefore made absolute first.
        self.proc = subprocess.Popen([os.path.abspath(path)] + list(args), cwd=cwd,
                                     stdin=subprocess.PIPE, stdout=self.log,
                                     stderr=subprocess.STDOUT)

    def is_running(self):
        return self.proc.poll() is None

    def sample(self):
        """Takes one reading and keeps the peak of each figure."""
        values = _status_of(self.proc.pid)
        if values is None:
            return
        self.samples += 1
        self.last = values
        for key, value in values.items():
            if value > self.peak.get(key, 0):
                self.peak[key] = value

    def send(self, line):
        """Writes one console command to the application."""
        try:
            self.proc.stdin.write((line + '\n').encode())
            self.proc.stdin.flush()
        except (OSError, ValueError):
            pass

    def stop(self, console=False):
        """Stops the application: its own quit key when it reads one, a signal otherwise."""
        if self.is_running() and console:
            self.send(QUIT_KEY)
            deadline = time.time() + 8.0
            while (time.time() < deadline) and self.is_running():
                time.sleep(0.1)
        if self.is_running():
            self.proc.terminate()
            try:
                self.proc.wait(timeout=8)
            except subprocess.TimeoutExpired:
                self.proc.kill()
                self.proc.wait(timeout=8)
        try:
            self.proc.stdin.close()
        except (OSError, ValueError):
            pass
        self.log.close()

    def record(self, state):
        return {'process': self.name, 'state': state, 'samples': self.samples,
                'vmrss_kb': self.peak.get('VmRSS', 0), 'vmhwm_kb': self.peak.get('VmHWM', 0),
                'rssanon_kb': self.peak.get('RssAnon', 0), 'threads': self.peak.get('Threads', 0)}


def _hold(processes, seconds):
    """Samples every process for the given time."""
    deadline = time.time() + seconds
    while time.time() < deadline:
        for process in processes:
            process.sample()
        time.sleep(SAMPLE_INTERVAL)


def _tcp_states(port):
    """Returns the socket states seen on one local port, read from /proc."""
    hexport = ':%04X' % port
    states = []
    for table in ('/proc/net/tcp', '/proc/net/tcp6'):
        try:
            with open(table, 'r', errors='replace') as handle:
                next(handle, None)
                for line in handle:
                    fields = line.split()
                    if len(fields) > 3 and fields[1].endswith(hexport):
                        states.append(fields[3])
        except OSError:
            continue
    return states


def _router_is_listening(port):
    """True when something listens on the router port. Reads /proc, so it needs no tools."""
    return '0A' in _tcp_states(port)


def _router_connections(port):
    """Counts the connections the router currently holds. State 01 is ESTABLISHED."""
    return sum(1 for state in _tcp_states(port) if state == '01')


class Harness(object):
    """Starts the processes of one scenario, samples them, and always stops them."""

    def __init__(self, bin_dir, out_dir, port):
        self.bin_dir = bin_dir
        self.out_dir = out_dir
        self.port = port
        self.started = []

    def binary(self, name):
        path = os.path.join(self.bin_dir, name)
        return path if os.path.isfile(path) else None

    def start(self, name, label=None, args=()):
        """Starts one binary and returns it, or None when it is not built."""
        path = self.binary(name)
        if path is None:
            return None
        label = label or name
        log = os.path.join(self.out_dir, re.sub(r'[^\w.-]', '_', label) + '.log')
        process = Process(label, path, args, log, self.bin_dir)
        self.started.append(process)
        return process

    def start_router(self):
        """Starts the router and waits until it listens."""
        if _router_is_listening(self.port):
            return None, 'a router is already listening on port %d' % self.port
        router = self.start('mtrouter.elf', 'mtrouter')
        if router is None:
            return None, 'mtrouter.elf is not built'
        deadline = time.time() + ROUTER_READY_SECONDS
        while (time.time() < deadline) and not _router_is_listening(self.port):
            if not router.is_running():
                return None, 'mtrouter exited before it listened'
            time.sleep(0.2)
        if not _router_is_listening(self.port):
            return None, 'mtrouter did not listen on port %d' % self.port
        return router, None

    def stop_all(self, console=()):
        """Stops everything this harness started, newest first."""
        for process in reversed(self.started):
            process.stop(console=process in console)
        self.started = []


# The single-process examples measured for RAM. Only one that stays up long enough to
# reach a steady state belongs here: an example that ends in a third of a second reports
# how far it got before the first sample, which is not a footprint. 01_minimalrpc is such
# an example, so it is measured for flash and not for RAM.
SINGLE_PROCESS = [
    ('21_locservice.elf', '21_locservice', 'no router, at rest'),
]


def scenario_single(harness, hold):
    """One-process applications: no router, no socket. The floor of an areg process."""
    rows = []
    for binary, label, state in SINGLE_PROCESS:
        process = harness.start(binary, label)
        if process is None:
            rows.append({'process': label, 'state': 'not built', 'samples': 0,
                         'vmrss_kb': 0, 'vmhwm_kb': 0, 'rssanon_kb': 0, 'threads': 0})
            continue
        # These examples end by themselves, so each is sampled until it does.
        deadline = time.time() + hold
        while (time.time() < deadline) and process.is_running():
            process.sample()
            time.sleep(SAMPLE_INTERVAL)
        rows.append(process.record(state))
        harness.stop_all()
    return rows, None


def scenario_router(harness, clients, hold):
    """The router alone, then with a growing number of idle consumers connected to it."""
    router, error = harness.start_router()
    if router is None:
        return [], error

    rows = []
    _hold([router], hold)
    rows.append(router.record('0 clients, %d connected' % _router_connections(harness.port)))

    attached = []
    for count in clients:
        while len(attached) < count:
            client = harness.start('23_pubclient.elf', '23_pubclient_%d' % (len(attached) + 1))
            if client is None:
                harness.stop_all(console=[router])
                return rows, '23_pubclient.elf is not built'
            attached.append(client)
            time.sleep(0.2)
        _hold([router] + attached, hold)
        rows.append(router.record('%d started, %d connected'
                                  % (count, _router_connections(harness.port))))
        rows.append({'process': '23_pubclient', 'state': 'idle consumer, %d running' % count,
                     'samples': attached[0].samples,
                     'vmrss_kb': max(c.peak.get('VmRSS', 0) for c in attached),
                     'vmhwm_kb': max(c.peak.get('VmHWM', 0) for c in attached),
                     'rssanon_kb': max(c.peak.get('RssAnon', 0) for c in attached),
                     'threads': max(c.peak.get('Threads', 0) for c in attached)})

    harness.stop_all(console=[router])
    return rows, None


def scenario_pair(harness, hold, shape, channels):
    """A provider and a consumer through the router, first idle and then under load."""
    router, error = harness.start_router()
    if router is None:
        return [], error

    provider = harness.start('23_pubservice.elf', '23_pubservice')
    if provider is None:
        harness.stop_all(console=[router])
        return [], '23_pubservice.elf is not built'
    time.sleep(1.0)
    consumer = harness.start('23_pubclient.elf', '23_pubclient')
    if consumer is None:
        harness.stop_all(console=[router])
        return [], '23_pubclient.elf is not built'

    group = [router, provider, consumer]
    _hold(group, hold)
    rows = [p.record('connected, no traffic') for p in group]

    # The provider reads console commands: set the block shape and the channel count,
    # then start the stream and hold it.
    for process in group:
        process.peak = {}
        process.samples = 0
    provider.send('%s -c=%d' % (shape, channels))
    time.sleep(1.0)
    provider.send('-s')
    _hold(group, hold * 2)
    rows.extend(p.record('streaming, %d channels' % channels) for p in group)

    harness.stop_all(console=[router, provider])
    return rows, None


# ---------------------------------------------------------------------------
# Report
# ---------------------------------------------------------------------------

def _thousands(value):
    return '{:,}'.format(value).replace(',', ' ')


def print_flash(records):
    print('Flash -- static size of the artefacts')
    print('%-22s %12s %10s %10s %12s  %s' % ('artefact', 'text', 'data', 'bss', 'file', 'what it is'))
    print('-' * 100)
    for record in records:
        if not record.get('present'):
            print('%-22s %12s %10s %10s %12s  %s'
                  % (record['artefact'], '--', '--', '--', 'not built', record['purpose']))
            continue
        if record.get('archive'):
            print('%-22s %12s %10s %10s %12s  %s'
                  % (record['artefact'], 'n/a', 'n/a', 'n/a',
                     _thousands(record['file_bytes']), record['purpose'] + ', archive'))
            continue
        print('%-22s %12s %10s %10s %12s  %s'
              % (record['artefact'],
                 _thousands(record.get('text', 0)),
                 _thousands(record.get('data', 0)),
                 _thousands(record.get('bss', 0)),
                 _thousands(record['file_bytes']),
                 record['purpose']))
    print()


def print_ram(sections):
    print('RAM -- resident memory of the running processes, peak of the run')
    print('%-24s %-28s %10s %10s %10s %8s' % ('process', 'state', 'VmRSS', 'VmHWM', 'RssAnon', 'threads'))
    print('-' * 100)
    for title, rows, error in sections:
        if error:
            print('%-24s %s' % (title, 'not measured: ' + error))
            continue
        for row in rows:
            print('%-24s %-28s %10s %10s %10s %8d'
                  % (row['process'], row['state'],
                     _thousands(row['vmrss_kb']) + ' kB',
                     _thousands(row['vmhwm_kb']) + ' kB',
                     _thousands(row['rssanon_kb']) + ' kB',
                     row['threads']))
    print()


def print_provenance(provenance):
    print('Measured on')
    for key in ('commit', 'tree', 'build_type', 'compiler', 'target', 'host',
                'logging', 'extended', 'no_exceptions', 'cxx_flags', 'size_tool'):
        value = provenance.get(key)
        if value:
            print('  %-14s %s' % (key, value))
    print()


def main():
    parser = argparse.ArgumentParser(description='Report the flash and RAM footprint of areg.')
    parser.add_argument('--build-dir', default='build', help='the build directory to read')
    parser.add_argument('--bin-dir', default=None, help='where the binaries are (default: <build>/bin)')
    parser.add_argument('--lib-dir', default=None, help='where the static libraries are (default: <build>/lib)')
    parser.add_argument('--out-dir', default=None, help='where to write the captured output')
    parser.add_argument('--size-tool', default='size', help='the size utility of the toolchain')
    parser.add_argument('--flash-only', action='store_true', help='do not start any process')
    parser.add_argument('--ram-only', action='store_true', help='do not read any size')
    parser.add_argument('--clients', default='1,10', help='router client counts to measure')
    parser.add_argument('--hold', type=float, default=5.0, help='seconds to sample each state')
    parser.add_argument('--shape', default='-w=128 -h=128 -l=1 -t=25',
                        help='the block shape the provider streams')
    parser.add_argument('--channels', type=int, default=8, help='channels the provider streams')
    parser.add_argument('--port', type=int, default=ROUTER_PORT_DEFAULT, help='the router port')
    parser.add_argument('--json', default=None, help='write the same numbers to this file')
    options = parser.parse_args()

    bin_dir = options.bin_dir or os.path.join(options.build_dir, 'bin')
    lib_dir = options.lib_dir or os.path.join(options.build_dir, 'lib')
    if not os.path.isdir(bin_dir):
        print('no such directory: %s' % bin_dir, file=sys.stderr)
        return 2

    out_dir = options.out_dir or os.path.join(options.build_dir, 'footprint')
    os.makedirs(out_dir, exist_ok=True)

    report = {'provenance': collect_provenance(options.build_dir, bin_dir, options.size_tool, lib_dir)}
    print_provenance(report['provenance'])

    if not options.ram_only:
        records, error = measure_flash(bin_dir, lib_dir, options.size_tool)
        if error:
            print('Flash -- not measured: %s\n' % error)
            report['flash_error'] = error
        else:
            print_flash(records)
            report['flash'] = records

    target_arch = report['provenance']['target'].split(' ')[0]
    host_arch = HOST_ALIASES.get(platform.machine().lower(), platform.machine().lower())
    foreign = target_arch not in ('unknown', host_arch)

    if not options.flash_only:
        if foreign:
            # A foreign binary either refuses to start or runs under an emulator, and an
            # emulator reports its own memory. Neither is the target's footprint.
            reason = 'the binaries are %s and this host is %s' % (target_arch, host_arch)
            print('RAM -- not measured: %s\n' % reason)
            report['ram_error'] = reason
        elif platform.system() != 'Linux':
            print('RAM -- not measured: /proc is needed, this is %s\n' % platform.system())
            report['ram_error'] = 'no /proc on ' + platform.system()
        else:
            clients = [int(part) for part in options.clients.split(',') if part.strip()]
            sections = []
            for title, runner in (('one process', lambda h: scenario_single(h, options.hold)),
                                  ('router', lambda h: scenario_router(h, clients, options.hold)),
                                  ('provider and consumer',
                                   lambda h: scenario_pair(h, options.hold, options.shape,
                                                           options.channels))):
                harness = Harness(bin_dir, out_dir, options.port)
                try:
                    rows, error = runner(harness)
                finally:
                    harness.stop_all()
                sections.append((title, rows, error))
            print_ram(sections)
            report['ram'] = [{'scenario': title, 'rows': rows, 'error': error}
                             for title, rows, error in sections]

    if options.json:
        with open(options.json, 'w') as handle:
            json.dump(report, handle, indent=2)
        print('wrote %s' % options.json)

    return 0


if __name__ == '__main__':
    sys.exit(main())
