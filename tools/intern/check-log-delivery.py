#!/usr/bin/env python3
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""Checks that every log message an observer missed was reported as dropped.

    check-log-delivery.py <bin dir> [--runs 20] [--profiles full,burst] [--port 8282]
                          [--summary file]

Each run starts logcollector, 35_logsink and one 35_logsource, in that order, and stops
only the processes it started. Profiles of the source:

    full   -c=100000 -w=1000            one thread writes as fast as it can
    burst  -c=200000 -w=0 -p=10 -b=400  400 messages every 10 ms, from the first moment

A run passes when the sink's 'lost' plus its 'scope rows lost' equals its 'dropped
messages': a message or a scope row may be dropped, but never without a notice.
'--summary' appends a Markdown table, for GITHUB_STEP_SUMMARY. Exits 1 when a run fails.
"""

import argparse
import os
import re
import socket
import subprocess
import sys
import threading
import time

PROFILES = {
    'full':  {'args': ['-c=100000', '-w=1000'], 'expected': 101000},
    'burst': {'args': ['-c=200000', '-w=0', '-p=10', '-b=400'], 'expected': 200000},
}
SUFFIXES = ('.exe', '.elf', '.mac', '')
RUN_TIMEOUT = 300
CONNECT_TIMEOUT = 20.0
RE_RECEIVED = re.compile(r'received (\d+) \| lost (\d+)')
RE_DROPPED = re.compile(r'dropped messages (\d+)')
RE_SCOPES = re.compile(r'scope rows lost (\d+)')


def executable(bin_dir, name):
    for suffix in SUFFIXES:
        path = os.path.join(bin_dir, name + suffix)
        if os.path.isfile(path):
            return path
    sys.exit('%s not found in %s' % (name, bin_dir))


def listening(port):
    try:
        with socket.create_connection(('127.0.0.1', port), timeout=0.2):
            return True
    except OSError:
        return False


def stop(proc, quit_line=None):
    """Asks the process to quit, then kills it if it is still running."""
    if proc.poll() is not None:
        return
    if quit_line and proc.stdin:
        try:
            proc.stdin.write(quit_line)
            proc.stdin.flush()
            proc.wait(timeout=5)
            return
        except (OSError, subprocess.TimeoutExpired):
            pass
    proc.kill()
    proc.wait()


def one_run(bins, profile, port):
    """Returns (verdict, received, lost, dropped, sink lines)."""
    if listening(port):
        return 'FAIL: port %d busy before the start' % port, 0, 0, 0, []
    collector = subprocess.Popen([bins['logcollector'], '--console'], cwd=bins['dir'], stdin=subprocess.PIPE,
                                 stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    sink = None
    lines = []
    try:
        deadline = time.monotonic() + CONNECT_TIMEOUT
        while (not listening(port)) and (time.monotonic() < deadline):
            time.sleep(0.05)
        if not listening(port):
            return 'FAIL: logcollector does not listen', 0, 0, 0, []

        connected = threading.Event()
        sink = subprocess.Popen([bins['35_logsink'], '-c=%d' % profile['expected']], cwd=bins['dir'],
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, errors='replace')

        def read_sink():
            for line in sink.stdout:
                lines.append(line.rstrip())
                if line.startswith('35_logsink: connected'):
                    connected.set()
            connected.set()

        reader = threading.Thread(target=read_sink, daemon=True)
        reader.start()
        if (not connected.wait(CONNECT_TIMEOUT)) or (sink.poll() is not None):
            return 'FAIL: 35_logsink did not connect', 0, 0, 0, lines

        source = subprocess.run([bins['35_logsource']] + profile['args'], cwd=bins['dir'], stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT, text=True, errors='replace', timeout=RUN_TIMEOUT)
        sink.wait(timeout=RUN_TIMEOUT)
        reader.join(timeout=5)
        if source.returncode != 0:
            return 'FAIL: 35_logsource exit %d' % source.returncode, 0, 0, 0, lines
    except subprocess.TimeoutExpired:
        return 'FAIL: timeout', 0, 0, 0, lines
    finally:
        if sink is not None:
            stop(sink)
        stop(collector, b'-q\n')

    text = '\n'.join(lines)
    got = RE_RECEIVED.search(text)
    if got is None:
        return 'FAIL: 35_logsink exit %d, no result' % sink.returncode, 0, 0, 0, lines
    received, lost = int(got.group(1)), int(got.group(2))
    match = RE_DROPPED.search(text)
    dropped = int(match.group(1)) if match else 0
    match = RE_SCOPES.search(text)
    lost += int(match.group(1)) if match else 0
    if lost == dropped:
        verdict = 'ok' if lost == 0 else 'ok, dropped and reported'
    elif lost > dropped:
        verdict = 'FAIL: %d lost without a notice' % (lost - dropped)
    else:
        verdict = 'FAIL: notices report %d more than lost' % (dropped - lost)
    return verdict, received, lost, dropped, lines


def main():
    parser = argparse.ArgumentParser(description='Check that every missed log message was reported as dropped.')
    parser.add_argument('bin_dir', help='directory of logcollector, 35_logsink and 35_logsource')
    parser.add_argument('--runs', type=int, default=20, help='runs per profile (default: 20)')
    parser.add_argument('--profiles', default='full,burst', help='comma separated: ' + ', '.join(PROFILES))
    parser.add_argument('--port', type=int, default=8282, help='port of logcollector (default: 8282)')
    parser.add_argument('--summary', default=None, help='Markdown file the table is appended to')
    args = parser.parse_args()

    bins = {'dir': os.path.abspath(args.bin_dir)}
    for name in ('logcollector', '35_logsink', '35_logsource'):
        bins[name] = executable(bins['dir'], name)

    table = []
    for name in args.profiles.split(','):
        profile = PROFILES[name]
        for run in range(1, args.runs + 1):
            verdict, received, lost, dropped, lines = one_run(bins, profile, args.port)
            table.append((name, run, received, lost, dropped, verdict))
            print('%-5s run %2d: received %d | lost %d | reported dropped %d | %s'
                  % (name, run, received, lost, dropped, verdict), flush=True)
            if verdict.startswith('FAIL'):
                print('\n'.join('        ' + l for l in lines if l))
            time.sleep(0.5)

    failed = [row for row in table if row[5].startswith('FAIL')]
    lossy = [row for row in table if row[3] > 0]
    print('\n%d runs, %d with drops, %d failed' % (len(table), len(lossy), len(failed)))
    if args.summary:
        out = ['### Log delivery: %d runs, %d with drops, %d failed' % (len(table), len(lossy), len(failed)), '',
               '| Profile | Run | Received | Lost | Reported dropped | Verdict |', '|---|---|---|---|---|---|']
        for row in table:
            if row[3] > 0 or row[5].startswith('FAIL'):
                out.append('| %s | %d | %d | %d | %d | %s |' % row)
        if len(out) == 4:
            out.append('| all | | | 0 | 0 | ok |')
        with open(args.summary, 'a', encoding='utf-8') as summary:
            summary.write('\n'.join(out) + '\n\n')
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
