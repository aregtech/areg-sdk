#!/usr/bin/env python3
# -*- coding: ascii -*-
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""
Mutation testing over the framework's own invariants.

tools/agent/check_mutations.py asks whether the documentation an agent reads still
diagnoses the mistakes it warns about. This asks the other question: when someone
changes framework/, does anything notice?

Each entry below breaks one invariant that a defect has already been paid for --
an alignment, a lock order, a gate, a cap, a release. The defect is written into
the tree, the framework is rebuilt, and the suite is run. The invariant counts as
enforced when something goes red: the compiler for a layout, ctest for a behaviour.

The number this prints is not a pass mark. An invariant nobody detects is a real
gap in the test suite, and the honest result is to see it. Seeding a defect whose
detection nothing yet asserts is how the gap becomes visible instead of assumed.

    python3 tools/check_invariants.py --list           what is seeded, and why
    python3 tools/check_invariants.py --dry-run        do the anchors still resolve
    python3 tools/check_invariants.py --build build    seed, rebuild, run the suite
    python3 tools/check_invariants.py --restore        undo a seed a killed run left

--dry-run needs no compiler and takes under a second: it only asserts that every
defect still applies to exactly one place. That is what rots first, and it is what
CI can afford on every push.

A full run edits framework/ in place and restores it afterwards. The original of
every file it touches is written beside the report before the edit, and the restore
is verified byte for byte. A run stopped with Ctrl-C or SIGTERM restores the file it
is holding before it exits. A run killed with SIGKILL cannot, and leaves one seeded
defect in the tree; --restore removes it without needing the copy, because every
seed is an exact text substitution that can be read back out.

Exit code 0 when every seeded defect was detected, 1 otherwise.
"""

import argparse
import os
import signal
import subprocess
import sys
import tempfile

SDK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CTEST_SECONDS = 900
BUILD_SECONDS = 3600

# Each defect: one invariant, one edit, and what must go red.
#
#   detector 'compiler'  the translation unit must stop compiling
#   detector 'ctest'     the build must succeed and the suite must fail
#
# 'invariant' is the sentence the defect violates. It is the same sentence the
# framework memory carries, so a reader can tell what was broken without reading
# the diff.
DEFECTS = [
    {
        'id': 'event-header-layout',
        'invariant': 'EventHeader is exactly 128 bytes and every field sits at a '
                     'fixed offset, because the header crosses the wire',
        'file': 'framework/areg/base/MemoryDefs.hpp',
        'find': "    uint64_t            custom      { 0 };  "
                "//!< [120..127] LOCAL-ONLY: DATA_CLASS cleanup hook -- "
                "zeroed on IPC wire; see struct docs\n};",
        'replace': "    uint64_t            custom      { 0 };  "
                   "//!< [120..127] LOCAL-ONLY: DATA_CLASS cleanup hook -- "
                   "zeroed on IPC wire; see struct docs\n"
                   "    uint32_t            seeded      { 0 };\n};",
        'detector': 'compiler',
    },
    {
        'id': 'mpsc-cell-alignment',
        'invariant': 'a ring slot is cache-line aligned, or neighbouring slots '
                     'share a line and producers fight over it',
        'file': 'framework/areg/component/private/EventQueue.hpp',
        'find': 'struct alignas(AREG_MPSC_CELL_ALIGN) Cell',
        'replace': 'struct Cell',
        'detector': 'ctest',
    },
    {
        'id': 'ring-closed-gate',
        'invariant': 'close_lanes() sets RING_CLOSED with one fetch_or, which both '
                     'refuses every new slot and returns the high-water mark',
        'file': 'framework/areg/component/private/EventQueue.cpp',
        'find': 'const size_t closing{ mEnqueuePos.fetch_or(EventQueue::RING_CLOSED, '
                'std::memory_order_acq_rel) };',
        'replace': 'const size_t closing{ mEnqueuePos.load(std::memory_order_acquire) };',
        'detector': 'ctest',
    },
    {
        'id': 'notification-not-posted',
        'invariant': 'a notification is posted to the proxy queue and never '
                     'dispatched inline, or a handler that triggers teardown '
                     'runs inside the dispatcher loop',
        'file': 'framework/areg/component/private/ProxyBase.cpp',
        'find': '    mDispatcherThread.event_dispatcher().post_event(eventElem);',
        'replace': '    static_cast<void>(eventElem);',
        'detector': 'ctest',
    },
    {
        'id': 'priority-lane-not-drained',
        'invariant': 'the priority lane is always drained before the ring',
        'file': 'framework/areg/component/private/EventQueue.cpp',
        'find': '    // Priority lane: always drained before the ring.\n'
                '    if (mPrioCount.load(std::memory_order_relaxed) != 0u)',
        'replace': '    // Priority lane: always drained before the ring.\n'
                   '    if (false)',
        'detector': 'ctest',
    },
    {
        'id': 'message-cap-lowered',
        'invariant': 'one message is capped by MAX_BUF_LENGTH, one GiB, and the '
                     'cap is the only thing between a large frame and a truncation',
        'file': 'framework/areg/base/MemoryDefs.hpp',
        'find': "constexpr uint32_t      MAX_BUF_LENGTH  { 0x4000'0000u };",
        'replace': "constexpr uint32_t      MAX_BUF_LENGTH  { 0x0000'1000u };",
        'detector': 'ctest',
    },
    {
        'id': 'reconnect-timer-not-stopped',
        'invariant': 'Timer::is_stopped() lies after a natural expiry, so '
                     'on_reconnect_timer() stops the timer before restarting',
        'file': 'framework/areg/ipc/private/ServiceClientConnectionBase.cpp',
        'find': '    LOG_DBG("Reconnect timer expired, attempting to restart service '
                'connection, thread [ %s ]", Thread::current_thread_name().as_string());\n'
                '\n    mTimerConnect.stop_timer( );',
        'replace': '    LOG_DBG("Reconnect timer expired, attempting to restart service '
                   'connection, thread [ %s ]", Thread::current_thread_name().as_string());\n',
        'detector': 'ctest',
    },
    {
        'id': 'rx-cache-not-released',
        'invariant': 'thread_rx_cache_release() runs after every failed receive, '
                     'or the cache leaks per socket and a reused fd resurrects a '
                     'stale cursor',
        'file': 'framework/areg/base/private/posix/SocketDefsPosix.cpp',
        'find': '    areg::thread_rx_cache_release(hSocket);',
        'replace': '    // seeded: the release is gone',
        'detector': 'ctest',
    },
]


def read(path):
    with open(os.path.join(SDK, path), encoding='utf-8', newline='') as handle:
        return handle.read()


def write(path, text):
    with open(os.path.join(SDK, path), 'w', encoding='utf-8', newline='') as handle:
        handle.write(text)


# The file a seeded defect is written into right now, mapped to its original text.
# A signal handler puts the original back before the process leaves.
IN_FLIGHT = {}


def restore_in_flight():
    """Puts back every file a seed is currently written into."""
    for path, original in list(IN_FLIGHT.items()):
        write(path, original)
        IN_FLIGHT.pop(path, None)


def on_signal(number, _frame):
    """Restores the tree and leaves. Bound to SIGINT and SIGTERM."""
    restore_in_flight()
    sys.stderr.write('\ninterrupted by signal {}, framework/ restored\n'.format(number))
    sys.exit(128 + number)


def restore(defects):
    """Removes any seeded defect a killed run left behind. Returns the exit code."""
    removed = 0
    unknown = 0
    for defect in defects:
        if not os.path.isfile(os.path.join(SDK, defect['file'])):
            continue
        text = read(defect['file'])
        if defect['find'] in text:
            continue
        if defect['replace'] not in text:
            print('UNKNOWN  {:<28} {}'.format(defect['id'], defect['file']))
            unknown += 1
            continue
        write(defect['file'], text.replace(defect['replace'], defect['find'], 1))
        print('RESTORED {:<28} {}'.format(defect['id'], defect['file']))
        removed += 1
    print('{} seeded defect(s) removed, {} file(s) matching neither text'
          .format(removed, unknown))
    return 1 if unknown else 0


def anchors(defects):
    """Which defects still apply to exactly one place. Returns a list of problems."""
    problems = []
    for defect in defects:
        path = os.path.join(SDK, defect['file'])
        if not os.path.isfile(path):
            problems.append((defect['id'], 'no such file: ' + defect['file']))
            continue
        count = read(defect['file']).count(defect['find'])
        if count != 1:
            problems.append((defect['id'],
                             'the text to break appears {} times, not once'.format(count)))
    return problems


def build(build_dir, jobs):
    """Rebuilds. Returns (ok, tail of the output)."""
    command = ['cmake', '--build', build_dir, '-j', str(jobs)]
    try:
        result = subprocess.run(command, cwd=SDK, capture_output=True, text=True,
                                timeout=BUILD_SECONDS)
    except subprocess.TimeoutExpired:
        return False, 'the build did not finish within {}s'.format(BUILD_SECONDS)
    tail = (result.stdout + result.stderr).strip().splitlines()[-1:]
    return result.returncode == 0, (tail[0] if tail else '')


def suite(build_dir):
    """Runs ctest. Returns (passed, detail). 'passed' false means something went red."""
    command = ['ctest', '--test-dir', build_dir, '--output-on-failure', '--timeout', '120']
    try:
        result = subprocess.run(command, cwd=SDK, capture_output=True, text=True,
                                timeout=CTEST_SECONDS)
    except subprocess.TimeoutExpired:
        return False, 'the suite did not finish within {}s'.format(CTEST_SECONDS)
    output = result.stdout + result.stderr
    failed = [line.strip() for line in output.splitlines()
              if 'tests failed out of' in line or 'Failed ' in line]
    return result.returncode == 0, (failed[0] if failed else '')


def seed(defect, build_dir, jobs, keep_dir):
    """Applies one defect, rebuilds, runs the suite, and restores. Returns (found, why)."""
    original = read(defect['file'])
    backup = os.path.join(keep_dir, defect['id'] + '.orig')
    with open(backup, 'w', encoding='utf-8', newline='') as handle:
        handle.write(original)

    try:
        IN_FLIGHT[defect['file']] = original
        write(defect['file'], original.replace(defect['find'], defect['replace'], 1))
        compiled, detail = build(build_dir, jobs)
        if defect['detector'] == 'compiler':
            if compiled:
                return False, 'the translation unit still compiles'
            return True, 'the compiler rejected it'
        if not compiled:
            # A behavioural defect that stops the build is still detected, but by
            # something other than the invariant this entry is about. Say so.
            return True, 'the build failed rather than the suite: ' + detail
        passed, detail = suite(build_dir)
        if passed:
            return False, 'the suite stayed green'
        return True, 'the suite went red: ' + (detail or 'see ctest output')
    finally:
        write(defect['file'], original)
        IN_FLIGHT.pop(defect['file'], None)
        if read(defect['file']) != original:
            sys.stderr.write('error: {} was not restored; the original is {}\n'
                             .format(defect['file'], backup))
        else:
            os.remove(backup)


def main():
    parser = argparse.ArgumentParser(
        description='Seed defects into framework/ and ask whether anything notices.')
    parser.add_argument('--build', default='build',
                        help='the configured build directory (default: build)')
    parser.add_argument('--jobs', type=int, default=os.cpu_count() or 4)
    parser.add_argument('--only', action='append', default=[],
                        help='run only this defect id; repeatable')
    parser.add_argument('--list', action='store_true',
                        help='print what is seeded and stop')
    parser.add_argument('--dry-run', action='store_true',
                        help='only check that every defect still applies; no build')
    parser.add_argument('--restore', action='store_true',
                        help='undo a seeded defect a killed run left in the tree')
    args = parser.parse_args()

    defects = [d for d in DEFECTS if not args.only or d['id'] in args.only]
    if not defects:
        sys.stderr.write('error: no defect matches {}\n'.format(', '.join(args.only)))
        return 1

    if args.list:
        for defect in defects:
            print('{:<28} {:<9} {}'.format(defect['id'], defect['detector'],
                                           defect['invariant']))
        return 0

    if args.restore:
        return restore(defects)

    problems = anchors(defects)
    for name, why in problems:
        print('STALE {:<28} {}'.format(name, why))
    if args.dry_run:
        print('{} seeded defect(s), {} stale'.format(len(defects), len(problems)))
        return 1 if problems else 0
    if problems:
        print('the seed set does not match the tree; fix it before measuring')
        return 1

    if not os.path.isfile(os.path.join(SDK, args.build, 'CMakeCache.txt')):
        sys.stderr.write('error: {} is not a configured build directory\n'.format(args.build))
        return 1

    for number in (signal.SIGINT, signal.SIGTERM):
        signal.signal(number, on_signal)

    keep = tempfile.mkdtemp(prefix='areg-invariants-')
    print('originals are kept in ' + keep)
    print('if this run is killed outright, run --restore before building again')
    found = 0
    try:
        for defect in defects:
            detected, why = seed(defect, args.build, args.jobs, keep)
            found += 1 if detected else 0
            print('{:5} {:<28} {}'.format('FOUND' if detected else 'MISSED',
                                          defect['id'], why))
            if not detected:
                print('      the invariant nothing enforces: ' + defect['invariant'])
    finally:
        if not os.listdir(keep):
            os.rmdir(keep)
        else:
            print('originals of files that were not restored: ' + keep)

    print('{} of {} seeded defect(s) detected'.format(found, len(defects)))
    return 0 if found == len(defects) else 1


if __name__ == '__main__':
    sys.exit(main())
