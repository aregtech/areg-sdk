#!/usr/bin/env python3
# -*- coding: ascii -*-
"""
Runs a program under a debugger over and over until it dies of a fatal signal, then saves the
stacks of every thread.

Written for the defects that appear once in a few dozen runs. A core file is not always
available -- ptrace is restricted in containers, macOS writes crash reports instead of cores,
and WSL often writes neither -- so the program is started under the debugger from the start
and the debugger is asked for the stacks at the moment it stops.

Uses gdb on Linux and cygwin, lldb on macOS. Both are driven in batch mode, so nothing is
interactive and the tool can be left running.

Usage
-----
    python3 tools/hunt-crash.py --runs 200 14_locmesh.elf
    python3 tools/hunt-crash.py --bin-dir <dir> --runs 50 --timeout 120 22_pubservice.elf
    python3 tools/hunt-crash.py --runs 100 --out crash.txt 30_pubprovider.elf -- --some-arg

The binary is looked up in --bin-dir, or in the most recently written 'bin' directory under
'product/build' when --bin-dir is not given, so it works on any checkout without configuration.

Exit status is 0 when a crash was captured, 1 when the runs finished without one, and 2 when
the tool could not run at all.
"""

import argparse
import os
import shutil
import subprocess
import sys
import time

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

FATAL = ('SIGSEGV', 'SIGABRT', 'SIGBUS', 'SIGILL', 'SIGFPE', 'EXC_BAD_ACCESS')
MARKER = '==== CRASH CAPTURED ===='


def newest_bin_dir():
    """The most recently written 'bin' directory of a build, so no path has to be typed."""
    best, best_time = None, -1.0
    build_root = os.path.join(REPO_ROOT, 'product', 'build')
    for directory, subdirs, _files in os.walk(build_root):
        if os.path.basename(directory) == 'bin':
            subdirs[:] = []
            stamp = os.path.getmtime(directory)
            if stamp > best_time:
                best, best_time = directory, stamp
    return best


def debugger_command(binary, args):
    """Returns the batch command line of whichever debugger this platform has."""
    if sys.platform == 'darwin':
        if shutil.which('lldb') is None:
            return None
        script = ['lldb', '--batch',
                  '-o', 'run',
                  '-o', 'script print("%s")' % MARKER,
                  '-o', 'thread backtrace all',
                  '-o', 'quit',
                  '--', binary] + list(args)
        return script

    if shutil.which('gdb') is None:
        return None
    return ['gdb', '-q', '-batch',
            '-ex', 'set confirm off',
            '-ex', 'handle SIGPIPE nostop noprint pass',
            '-ex', 'run',
            '-ex', 'echo \\n%s\\n' % MARKER,
            '-ex', 'info program',
            '-ex', 'thread apply all bt full',
            '--args', binary] + list(args)


def main():
    parser = argparse.ArgumentParser(
        description='Repeats a program under a debugger until it crashes, then saves the stacks.')
    parser.add_argument('binary', help='name of the executable, as it appears in the bin directory')
    parser.add_argument('args', nargs='*', help='arguments passed to the program')
    parser.add_argument('--bin-dir', help='directory holding the executable')
    parser.add_argument('--runs', type=int, default=100, help='how many times to try (default 100)')
    parser.add_argument('--timeout', type=int, default=120,
                        help='seconds allowed per run before it is killed (default 120)')
    parser.add_argument('--out', default='crash-stacks.txt',
                        help='where to write the stacks of the run that crashed')
    args = parser.parse_args()

    bin_dir = args.bin_dir or newest_bin_dir()
    if not bin_dir or not os.path.isdir(bin_dir):
        print('no build directory found, pass --bin-dir', file=sys.stderr)
        return 2

    binary = os.path.join(bin_dir, args.binary)
    if not os.path.isfile(binary):
        print('no such executable: %s' % binary, file=sys.stderr)
        return 2

    command = debugger_command(binary, args.args)
    if command is None:
        print('neither gdb nor lldb is installed', file=sys.stderr)
        return 2

    print('binary : %s' % binary)
    print('runs   : %d, %d s each' % (args.runs, args.timeout))
    print('output : %s' % args.out)

    started = time.time()
    for attempt in range(1, args.runs + 1):
        try:
            done = subprocess.run(command, cwd=bin_dir, timeout=args.timeout,
                                  stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            text = done.stdout.decode('utf-8', 'replace')
        except subprocess.TimeoutExpired as expired:
            captured = expired.stdout or b''
            text = captured.decode('utf-8', 'replace') + '\n<the run was killed on timeout>\n'
        except OSError as err:
            print('cannot start the debugger: %s' % err, file=sys.stderr)
            return 2

        if (MARKER in text) and any(signal in text for signal in FATAL):
            with open(args.out, 'w') as report:
                report.write(text)
            print('')
            print('crashed on run %d of %d after %.0f s, stacks written to %s'
                  % (attempt, args.runs, time.time() - started, args.out))
            for line in text.splitlines():
                if any(signal in line for signal in FATAL):
                    print('  %s' % line.strip())
            return 0

        sys.stdout.write('.' if attempt % 10 else '%d' % attempt)
        sys.stdout.flush()

    print('')
    print('no crash in %d run(s), %.0f s' % (args.runs, time.time() - started))
    return 1


if __name__ == '__main__':
    sys.exit(main())
