#!/usr/bin/env python3
# -*- coding: ascii -*-
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ---------------------------------------------------------------------------
#  Unattended latency measurement of example 30 (30_publatency), topology T-C:
#  provider and consumer in two processes on one machine, through the message
#  router.
#
#  Both applications of example 30 are started headless: the provider with
#  '-n', the consumer with the mode and the counts. The consumer runs the
#  measurement, prints one plain line per finished run, asks the provider to
#  quit and quits itself, so a scenario ends exactly when the work is done and
#  no fixed waiting time has to be guessed.
#
#  The message router still has to be started and stopped, and every process
#  still needs a pseudo terminal. That part is not re-implemented here: it
#  comes from tools/run-all-examples.py, which is imported as a module. Only
#  the scenario list is different, one scenario per mode, with the modes and
#  the sample counts that example 31 uses, so the two examples can be compared.
#
#  Usage:
#      python3 tools/latency/run-pub-latency.py [options]
#
#      --modes bc0,pp0,...   Modes to measure. Default: bc0,pp0,bc64,pp64,bc1024,pp1024
#      --count N             Measured samples per run.       Default: 20000
#      --warmup N            Warm-up samples per run.        Default: 2000
#      --runs N              Runs per mode.                  Default: 3
#      --label TEXT          Text written into every CSV row. Default: baseline
#      --bin-dir DIR         Directory with the built binaries. Auto-detected if omitted.
#      --out-dir DIR         Where to write. Default: product/tasks/measurements
#
#  Exit code: 0 when every mode produced results, 1 otherwise.
# ===========================================================================

import argparse
import importlib.util
import os
import platform
import re
import sys
import time

TOOLS_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REPO_ROOT = os.path.dirname(TOOLS_DIR)

# Payload size in bytes carried by each mode name, on top of id and timestamps.
PAYLOAD = {'0': 0, '8': 8, '16': 16, '32': 32, '64': 64, '128': 128, '256': 256,
           '512': 512, '1024': 1024, '4096': 4096, '65536': 65536}

# One finished run, as the headless consumer prints it:
# "run 1   mode pp64    samples 20000   min ... p50 ... p95 ... p99 ... max ... mean ... dur ... ms rate ... msg/s"
_ROW = re.compile(
    r'run\s+(\d+)\s+mode\s+(\w+)\s+samples\s+(\d+)\s+min\s+([0-9.]+)\s+p50\s+([0-9.]+)'
    r'\s+p95\s+([0-9.]+)\s+p99\s+([0-9.]+)\s+max\s+([0-9.]+)\s+mean\s+([0-9.]+)'
    r'\s+dur\s+([0-9.]+)\s+ms\s+rate\s+([0-9.]+)\s+msg/s')

_ANSI = re.compile(rb'\x1b\[[0-9;?]*[a-zA-Z]')


def _load_driver():
    """Imports tools/run-all-examples.py, whose name is not a valid module name."""
    if TOOLS_DIR not in sys.path:
        sys.path.insert(0, TOOLS_DIR)   # run-all-examples.py imports areg_benchmarks
    path = os.path.join(TOOLS_DIR, 'run-all-examples.py')
    spec = importlib.util.spec_from_file_location('areg_example_driver', path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def _payload_of(mode):
    return PAYLOAD.get(mode[2:], 0)


def _deadline_seconds(count, warmup, runs):
    """
    Upper bound for one mode. Nothing waits this long on purpose: the consumer ends as soon
    as it is finished. This is only the point at which a run that hangs is given up, and the
    consumer has its own watchdog that fires earlier.
    """
    assumed_rate = 1000.0                 # messages per second, deliberately pessimistic
    pause_between_runs = 1.0              # the consumer's default pause between batch cycles
    return 90.0 + runs * ((count + warmup) / assumed_rate + pause_between_runs)


def _scenario(driver, mode, count, warmup, runs):
    """
    One mode, both applications headless. Both end by themselves: the consumer quits when the
    last run is done, and asks the provider to quit with it.
    """
    return {
        'name': '30_%s' % mode,
        'tier': 'perf',
        'measure': 'latency',
        'timeout': _deadline_seconds(count, warmup, runs),
        'procs': [
            driver.proc('30_pubprovider', args=['-n'], must_exit=True),
            driver.proc('30_pubconsumer',
                        args=['-m=%s' % mode, '-c=%d' % count,
                              '-w=%d' % warmup, '-s=%d' % runs],
                        must_exit=True,
                        expect=[r'run\s+1\s+mode']),
        ],
    }


def _read_rows(log_path):
    """Returns the unique result rows of one consumer log, in the order they appeared."""
    with open(log_path, 'rb') as handle:
        text = _ANSI.sub(b'', handle.read()).decode('utf-8', 'replace')

    seen, rows = set(), []
    for row in _ROW.findall(text):
        if row not in seen:
            seen.add(row)
            rows.append(row)
    return rows


def _build_info():
    system = {'Windows': 'windows', 'Darwin': 'macos', 'Linux': 'linux'}
    return system.get(platform.system(), 'posix')


def main():
    parser = argparse.ArgumentParser(add_help=True)
    parser.add_argument('--modes', default='bc0,pp0,bc64,pp64,bc1024,pp1024')
    parser.add_argument('--count', type=int, default=20000)
    parser.add_argument('--warmup', type=int, default=2000)
    parser.add_argument('--runs', type=int, default=3)
    parser.add_argument('--label', default='baseline')
    parser.add_argument('--bin-dir', default=None)
    parser.add_argument('--out-dir',
                        default=os.path.join(REPO_ROOT, 'product', 'tasks', 'measurements'))
    args = parser.parse_args()

    driver = _load_driver()

    bin_dir = args.bin_dir
    if bin_dir is None:
        found = driver.find_bin_dirs(REPO_ROOT)
        bin_dir = found[0] if found else None
    if (bin_dir is None) or not os.path.isdir(bin_dir):
        print('ERROR: binary directory not found. Build the examples or pass --bin-dir.')
        return 1
    bin_dir = os.path.abspath(bin_dir)

    router_bin = driver.find_binary(bin_dir, 'mtrouter')
    if router_bin is None and not driver.is_router_listening():
        print('ERROR: mtrouter is not built and none is running. Example 30 needs it.')
        return 1

    stamp = time.strftime('%Y%m%d-%H%M%S')
    out_dir = os.path.abspath(os.path.join(args.out_dir, '%s-publatency-%s' % (stamp, args.label)))
    os.makedirs(out_dir, exist_ok=True)
    csv_path = os.path.join(args.out_dir, '%s-publatency-%s.csv' % (stamp, args.label))

    modes = [m.strip() for m in args.modes.split(',') if m.strip()]
    print('binaries : %s' % bin_dir)
    print('output   : %s' % out_dir)
    print('modes    : %s' % ', '.join(modes))
    print('per mode : %d measured, %d warm-up, %d run(s)' % (args.count, args.warmup, args.runs))
    if os.path.exists('/proc/version'):
        with open('/proc/version') as handle:
            if 'microsoft' in handle.read().lower():
                print('WARNING  : this is WSL. Use these numbers as a trend, never as an'
                      ' absolute latency.')
    print('')

    rows_out = []
    failures = []
    for mode in modes:
        scenario = _scenario(driver, mode, args.count, args.warmup, args.runs)
        sys.stdout.write('%-12s ' % scenario['name'])
        sys.stdout.flush()

        started = time.time()
        result = driver.run_scenario(scenario, bin_dir, out_dir, scenario['timeout'],
                                     router_bin, keep_logs=True, use_pty=True)
        log = os.path.join(out_dir, scenario['name'], '1_30_pubconsumer.log')
        rows = _read_rows(log) if os.path.isfile(log) else []

        if not rows:
            failures.append(mode)
            print('FAILED after %.0f s (no result row in the console output)' % (time.time() - started))
            continue

        print('%d run(s), P50 %s us  [%.0f s]' % (len(rows), rows[0][4], time.time() - started))
        for row in rows:
            rows_out.append((mode, row))

        if result.status != 'PASS':
            # The numbers are there; only the tidy shutdown was not. Worth a note, not a stop.
            print('             note: scenario status %s -- %s'
                  % (result.status, driver.one_line('; '.join(result.notes))))

    if rows_out:
        exists = os.path.exists(csv_path)
        with open(csv_path, 'a') as csv:
            if not exists:
                csv.write('timestamp,label,platform,topology,mode,payload_bytes,run,samples,'
                          'warmup,min_us,p50_us,p95_us,p99_us,max_us,mean_us,duration_ms,'
                          'msg_per_sec\n')
            now = time.strftime('%Y-%m-%d %H:%M:%S')
            for mode, row in rows_out:
                index, name, count = row[0], row[1], row[2]
                mn, p50, p95, p99, mx, mean, dur, mps = row[3:11]
                csv.write('%s,%s,%s,proc,%s,%d,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s\n'
                          % (now, args.label, _build_info(), name, _payload_of(mode), index,
                             count, args.warmup, mn, p50, p95, p99, mx, mean, dur, mps))
        print('\ncsv      : %s' % csv_path)

    print('logs     : %s' % out_dir)
    if failures:
        print('FAILED modes: %s' % ', '.join(failures))
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
