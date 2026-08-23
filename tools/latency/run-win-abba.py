#!/usr/bin/env python3
# -*- coding: ascii -*-
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
# ---------------------------------------------------------------------------
#  Interleaved A/B runner for native Windows.
#
#  Keeps two builds apart -- one made from the unchanged sources and one made
#  from the changed sources -- and runs them alternately in ABBA order inside a
#  single session, so that the monotonic drift of the machine is charged to
#  both of them equally.
#
#  Example 31 needs nothing but its own executable. Example 30 needs the
#  message router and the provider, which this script starts and stops itself.
#
#  Usage:
#      python3 tools/latency/run-win-abba.py --base DIR --chg DIR [options]
# ===========================================================================

import argparse
import os
import re
import subprocess
import sys
import time

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# One result row of example 31.
_ROW31 = re.compile(
    r'^\s*(\w+)\s*\|\s*(\w+)\s*\|\s*(\d+)\s*\|\s*(\d+)\s*\|\s*(\d+)\s*\|'
    r'\s*([0-9.]+)\s*\|\s*([0-9.]+)\s*\|\s*([0-9.]+)\s*\|\s*([0-9.]+)\s*\|'
    r'\s*([0-9.]+)\s*\|\s*([0-9.]+)\s*\|\s*([0-9.]+)\s*\|\s*([0-9.]+)\s*\|'
    r'\s*([0-9.]+)\s*\|\s*([0-9.]+)\s*$')

# One result line of the headless consumer of example 30.
_ROW30 = re.compile(
    r'run\s+(\d+)\s+mode\s+(\w+)\s+samples\s+(\d+)\s+min\s+([0-9.]+)\s+p50\s+([0-9.]+)'
    r'\s+p95\s+([0-9.]+)\s+p99\s+([0-9.]+)\s+max\s+([0-9.]+)\s+mean\s+([0-9.]+)')


def _exe(bin_dir, name):
    return os.path.join(bin_dir, name + '.exe')


def _kill(name):
    subprocess.run(['taskkill.exe', '/F', '/IM', name + '.exe'],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


def run31(bin_dir, modes, count, warmup):
    """Runs example 31 in both topologies. Returns {(topology, mode): {stat: value}}."""
    out = {}
    for topo in ('same', 'cross'):
        cmd = [_exe(bin_dir, '31_loclatency'), '-t=' + topo, '-m=' + modes,
               '-c=%d' % count, '-w=%d' % warmup, '-r=1']
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=900)
        for line in proc.stdout.splitlines():
            m = _ROW31.match(line.replace('\r', ''))
            if m is None:
                continue
            out[(m.group(1), m.group(2))] = {
                'min': float(m.group(6)), 'p50': float(m.group(7)),
                'p90': float(m.group(8)), 'p99': float(m.group(9)),
                'mean': float(m.group(13)), 'rate': float(m.group(15)),
            }
    return out


def run30(bin_dir, modes, count, warmup):
    """Runs example 30 through the router, one mode per scenario. Returns {mode: {stat: value}}."""
    out = {}
    for mode in modes:
        for name in ('30_pubconsumer', '30_pubprovider', 'mtrouter'):
            _kill(name)
        # The router reads its console. A closed stdin is end of file for it, and it quits at
        # once, so the pipe is opened and deliberately left open for the whole scenario.
        router = subprocess.Popen([_exe(bin_dir, 'mtrouter')],
                                  stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                                  stdin=subprocess.PIPE)
        time.sleep(2.0)
        provider = subprocess.Popen([_exe(bin_dir, '30_pubprovider'), '-n'],
                                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                                    stdin=subprocess.PIPE)
        time.sleep(2.0)
        try:
            cmd = [_exe(bin_dir, '30_pubconsumer'), '-m=' + mode, '-c=%d' % count,
                   '-w=%d' % warmup, '-s=1']
            proc = subprocess.run(cmd, capture_output=True, text=True, timeout=600)
            for line in proc.stdout.splitlines():
                m = _ROW30.search(line.replace('\r', ''))
                if m is not None:
                    out[mode] = {'min': float(m.group(4)), 'p50': float(m.group(5)),
                                 'p95': float(m.group(6)), 'p99': float(m.group(7)),
                                 'mean': float(m.group(9))}
        except subprocess.TimeoutExpired:
            sys.stderr.write('TIMEOUT on mode %s\n' % mode)
        finally:
            time.sleep(1.0)
            for proc_handle, name in ((provider, '30_pubprovider'), (router, 'mtrouter')):
                if proc_handle.poll() is None:
                    _kill(name)
                    proc_handle.wait(timeout=30)
    return out


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--base', required=True, help='bin folder of the unchanged build')
    parser.add_argument('--chg', required=True, help='bin folder of the changed build')
    parser.add_argument('--example', default='31', choices=['30', '31'])
    parser.add_argument('--modes', default='pp0,bc0,pp64,bc64,pp1024,bc1024')
    parser.add_argument('--count', type=int, default=50000)
    parser.add_argument('--warmup', type=int, default=5000)
    parser.add_argument('--pairs', type=int, default=4, help='number of A/B pairs (ABBA blocks)')
    parser.add_argument('--out', default=os.path.join(REPO_ROOT, 'product', 'tasks',
                                                      'measurements', 't2-abba'))
    args = parser.parse_args()

    os.makedirs(args.out, exist_ok=True)
    stamp = time.strftime('%Y%m%d-%H%M%S')
    csv_path = os.path.join(args.out, '%s-win-abba-ex%s.csv' % (stamp, args.example))

    modes = [m.strip() for m in args.modes.split(',') if m.strip()]
    runner = (lambda d: run31(d, args.modes, args.count, args.warmup)) if args.example == '31' \
        else (lambda d: run30(d, modes, args.count, args.warmup))

    # ABBA: base, chg, chg, base, base, chg, ...
    order = []
    for block in range(args.pairs // 2 + args.pairs % 2):
        order += ['base', 'chg', 'chg', 'base'] if (block % 2 == 0) else ['chg', 'base', 'base', 'chg']
    order = order[:args.pairs * 2]

    rows = []
    for index, which in enumerate(order):
        bin_dir = args.base if which == 'base' else args.chg
        print('[%2d/%2d] %-4s  %s' % (index + 1, len(order), which, bin_dir), flush=True)
        result = runner(bin_dir)
        for key, stats in result.items():
            label = key if isinstance(key, str) else '%s/%s' % key
            rows.append((index, which, label, stats))

    with open(csv_path, 'w', newline='') as handle:
        handle.write('seq,build,case,min,p50,p90_p95,p99,mean\n')
        for index, which, label, stats in rows:
            handle.write('%d,%s,%s,%s,%s,%s,%s,%s\n' % (
                index, which, label, stats.get('min', ''), stats.get('p50', ''),
                stats.get('p90', stats.get('p95', '')), stats.get('p99', ''), stats.get('mean', '')))
    print('csv: %s' % csv_path)

    # Pairwise differences: each A is paired with the B that stands next to it.
    print('\n%-16s %-6s %10s %10s %10s' % ('case', 'stat', 'base p50', 'chg p50', 'delta %'))
    cases = sorted({row[2] for row in rows})
    for case in cases:
        for stat in ('min', 'p50'):
            pairs = []
            for index in range(0, len(order) - 1, 2):
                a = [r for r in rows if r[0] == index and r[2] == case]
                b = [r for r in rows if r[0] == index + 1 and r[2] == case]
                if not a or not b:
                    continue
                first, second = a[0], b[0]
                base_v = first[3][stat] if first[1] == 'base' else second[3][stat]
                chg_v = second[3][stat] if first[1] == 'base' else first[3][stat]
                if base_v:
                    pairs.append((base_v, chg_v, 100.0 * (chg_v - base_v) / base_v))
            if not pairs:
                continue
            pairs.sort(key=lambda t: t[2])
            mid = pairs[len(pairs) // 2]
            print('%-16s %-6s %10.3f %10.3f %+9.1f' % (case, stat, mid[0], mid[1], mid[2]))
    return 0


if __name__ == '__main__':
    sys.exit(main())
