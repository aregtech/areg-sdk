#!/usr/bin/env python3
# -*- coding: ascii -*-
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""
Checks that a service listens after each start, and records a start that does not.

  repeat  starts the binary COUNT times; every start must listen within the limit.
  watch   runs beside another tool until DONE exists and records every process
          with the given name that lives longer than the limit without listening.

A stuck process is recorded before anything kills it: a memory dump goes to the output
directory first (a full minidump on Windows, gcore on Linux), then the thread stacks
are printed, from the dump and twice from the live process a few seconds apart. The
process itself is left alone in watch mode; repeat mode stops it by its PID afterwards.

Exit code: 0 when every start listened, 1 when one did not, 2 on a usage error.
"""

import argparse
import ctypes
import os
import platform
import shutil
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from service_ports import is_listening  # noqa: E402

IS_WINDOWS = os.name == 'nt'
STACK_SNAPSHOTS = 2         #!< stack snapshots taken of a stuck process
STACK_GAP = 3.0             #!< seconds between two snapshots
TOOL_WAIT = 120             #!< seconds a debugger may take
KITS_CDB = r'C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe'


def run_tool(command, seconds=TOOL_WAIT):
    """Runs a diagnostic tool and returns its output. Never raises."""
    try:
        done = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                              timeout=seconds)
        return done.stdout.decode('utf-8', 'replace')
    except subprocess.TimeoutExpired:
        return '<{} did not answer within {} s>\n'.format(command[0], seconds)
    except Exception as error:
        return '<{} failed: {}>\n'.format(command[0], error)


def find_cdb():
    """The path of cdb.exe, or None."""
    found = shutil.which('cdb')
    if found:
        return found
    return KITS_CDB if os.path.isfile(KITS_CDB) else None


def stack_commands(pid, symbols):
    """The debugger command lines that print every thread stack of the process."""
    if IS_WINDOWS:
        cdb = find_cdb()
        if cdb is None:
            return []
        command = [cdb, '-p', str(pid), '-pv', '-lines', '-c', '~*kn 80; q']
        return [command[:1] + ['-y', symbols + ';srv*'] + command[1:]] if symbols else [command]
    if platform.system() == 'Darwin':
        tools = [['lldb', '--batch', '-p', str(pid), '-o', 'thread backtrace all', '-o', 'detach'],
                 ['sample', str(pid), '2', '-mayDie']]
    else:
        tools = [['gdb', '-p', str(pid), '--batch', '-nx', '-ex', 'info threads',
                  '-ex', 'thread apply all bt']]
    return [tool for tool in tools if shutil.which(tool[0]) is not None]


def write_minidump(pid, path):
    """Writes a full memory dump of a Windows process. Returns '' or the reason it failed."""
    from ctypes import wintypes
    kernel = ctypes.WinDLL('kernel32', use_last_error=True)
    dbghelp = ctypes.WinDLL('dbghelp', use_last_error=True)
    kernel.OpenProcess.restype = wintypes.HANDLE
    kernel.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
    kernel.CloseHandle.argtypes = [wintypes.HANDLE]
    dbghelp.MiniDumpWriteDump.restype = wintypes.BOOL
    dbghelp.MiniDumpWriteDump.argtypes = [wintypes.HANDLE, wintypes.DWORD, wintypes.HANDLE,
                                          wintypes.DWORD, ctypes.c_void_p, ctypes.c_void_p,
                                          ctypes.c_void_p]
    # PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_DUP_HANDLE
    process = kernel.OpenProcess(0x0400 | 0x0010 | 0x0040, False, pid)
    if not process:
        return 'OpenProcess failed with error {}'.format(ctypes.get_last_error())
    import msvcrt
    try:
        with open(path, 'wb') as target:
            # full memory, handle data, unloaded modules, thread info, full memory info
            kind = 0x0002 | 0x0004 | 0x0020 | 0x1000 | 0x0800
            written = dbghelp.MiniDumpWriteDump(process, pid, msvcrt.get_osfhandle(target.fileno()),
                                                kind, None, None, None)
            error = ctypes.get_last_error()
    finally:
        kernel.CloseHandle(process)
    return '' if written else 'MiniDumpWriteDump failed with error {}'.format(error)


def write_dump(pid, path):
    """Writes a memory dump of the process. Returns a line that says where, or why not."""
    if IS_WINDOWS:
        why = write_minidump(pid, path + '.dmp')
        return why or 'memory dump: {}.dmp'.format(path)
    if shutil.which('gcore') is None:
        return 'memory dump: gcore is not installed'
    run_tool(['gcore', '-o', path, str(pid)])
    dumped = '{}.{}'.format(path, pid)
    return 'memory dump: {}'.format(dumped) if os.path.isfile(dumped) else 'memory dump: gcore wrote nothing'


def record_stuck(pid, name, seconds, out_dir, symbols):
    """Writes the dump of a stuck process, prints its stacks and writes the report to out_dir."""
    os.makedirs(out_dir, exist_ok=True)
    base = os.path.join(out_dir, '{}-{}'.format(name, pid))
    parts = ['==== {} (pid {}) did not listen within {:.1f} s ====\n'.format(name, pid, seconds)]
    parts.append(write_dump(pid, base) + '\n')
    cdb = find_cdb() if IS_WINDOWS else None
    if cdb and os.path.isfile(base + '.dmp'):
        command = [cdb, '-z', base + '.dmp', '-lines', '-c', '~*kn 80; q']
        if symbols:
            command[1:1] = ['-y', symbols + ';srv*']
        parts.append('---- stacks in the dump: {} ----\n'.format(' '.join(command)))
        parts.append(run_tool(command))
    commands = stack_commands(pid, symbols)
    if not commands:
        parts.append('no debugger found to print the stacks of the live process\n')
    for snapshot in range(STACK_SNAPSHOTS if commands else 0):
        if snapshot or cdb:
            time.sleep(STACK_GAP)
        for command in commands:
            parts.append('---- live snapshot {} of {}: {} ----\n'.format(snapshot + 1, STACK_SNAPSHOTS,
                                                                         ' '.join(command)))
            parts.append(run_tool(command))
    report = ''.join(parts)
    with open(base + '.txt', 'w') as target:
        target.write(report)
    sys.stdout.write(report)
    sys.stdout.flush()


def process_ids(name):
    """The PIDs of the running processes with this executable name."""
    if IS_WINDOWS:
        image = name if name.lower().endswith('.exe') else name + '.exe'
        listed = run_tool(['tasklist', '/FO', 'CSV', '/NH', '/FI', 'IMAGENAME eq {}'.format(image)], 30)
        pids = []
        for line in listed.splitlines():
            fields = [field.strip('"') for field in line.split('","')]
            if len(fields) > 1 and fields[0].lower() == image.lower() and fields[1].isdigit():
                pids.append(int(fields[1]))
        return pids
    listed = run_tool(['pgrep', '-x', name], 30)
    return [int(word) for word in listed.split() if word.isdigit()]


def stop(handle):
    """Stops a started process by its handle and waits for it to end."""
    if handle.poll() is None:
        handle.terminate()
    try:
        handle.wait(timeout=10.0)
    except subprocess.TimeoutExpired:
        handle.kill()
        handle.wait(timeout=5.0)


def repeat(args):
    """Starts the binary args.count times; every start must listen within args.within."""
    binary = os.path.abspath(args.binary)
    name = os.path.splitext(os.path.basename(binary))[0]
    symbols = os.path.dirname(binary)
    command = [binary] + ([] if IS_WINDOWS else ['--service']) + args.extra
    os.makedirs(args.out, exist_ok=True)
    output = os.path.join(args.out, '{}-output.txt'.format(name))
    worst, failed = 0.0, 0
    for index in range(1, args.count + 1):
        if is_listening(args.port):
            print('FAIL  port {} is served before start {}'.format(args.port, index))
            return 1
        with open(output, 'wb') as sink:
            begun = time.time()
            handle = subprocess.Popen(command, stdin=subprocess.PIPE if IS_WINDOWS else subprocess.DEVNULL,
                                      stdout=sink, stderr=subprocess.STDOUT)
            listening = False
            while time.time() - begun < args.within and handle.poll() is None:
                if is_listening(args.port):
                    listening = True
                    break
                time.sleep(0.1)
            took = time.time() - begun
            worst = max(worst, took)
            if not listening:
                failed += 1
                if handle.poll() is None:
                    record_stuck(handle.pid, name, took, args.out, symbols)
                else:
                    print('FAIL  start {}: {} exited {} before it listened'.format(index, name, handle.returncode))
            stop(handle)
        if not listening:
            with open(output, 'rb') as text:
                print('---- output of start {} ----\n{}'.format(index, text.read().decode('utf-8', 'replace')))
            if failed >= args.max_failures:
                break
    print('{}  {} of {} start(s) of {} listened on port {}, slowest {:.2f} s'
          .format('FAIL' if failed else 'PASS', index - failed, index, name, args.port, worst))
    return 1 if failed else 0


def watch(args):
    """Records every process named args.name that does not listen within args.within."""
    seen, handled, stuck = {}, set(), 0
    deadline = time.time() + args.max_seconds
    while not os.path.exists(args.done) and time.time() < deadline:
        now = time.time()
        for pid in process_ids(args.name):
            seen.setdefault(pid, now)
            if pid in handled or now - seen[pid] < args.within:
                continue
            handled.add(pid)
            if not is_listening(args.port):
                stuck += 1
                record_stuck(pid, args.name, now - seen[pid], args.out, args.symbols)
        time.sleep(0.25)
    return 1 if stuck else 0


def main():
    parser = argparse.ArgumentParser(description=__doc__.strip().splitlines()[0])
    sub = parser.add_subparsers(dest='mode', required=True)
    rep = sub.add_parser('repeat', help='start the binary COUNT times')
    rep.add_argument('--binary', required=True)
    rep.add_argument('--count', type=int, default=100)
    rep.add_argument('--max-failures', type=int, default=1)
    rep.add_argument('extra', nargs='*', help='arguments passed to the binary')
    wat = sub.add_parser('watch', help='record stuck processes until DONE exists')
    wat.add_argument('--name', required=True)
    wat.add_argument('--done', required=True)
    wat.add_argument('--symbols', default='')
    wat.add_argument('--max-seconds', type=float, default=1800.0)
    for each in (rep, wat):
        each.add_argument('--port', type=int, required=True)
        each.add_argument('--within', type=float, default=7.0)
        each.add_argument('--out', default='service-start-check')
    args = parser.parse_args()
    return repeat(args) if args.mode == 'repeat' else watch(args)


if __name__ == '__main__':
    sys.exit(main())
