#!/usr/bin/env python3
# ###########################################################################
# Connection scenario driver
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
"""Run mtrouter, one provider and two consumers (A and B) of areg-conn-scenario, disturb
one of them, and report what each consumer saw.

    python3 conn_scenario.py --bin build/bin pause-b --seconds 5
    python3 conn_scenario.py --bin build/bin pause-router --seconds 5 --bytes 65536 --gap-us 0
    python3 conn_scenario.py --bin build/bin loss-b --seconds 20      (Linux, no root needed)

Actions:
    none          no disturbance, the reference run
    pause-b       consumer B is suspended for --seconds, then resumed
    pause-router  mtrouter is suspended for --seconds, then resumed
    loss          every packet is dropped for --seconds, then the path is restored
    loss-b        only the packets of consumer B's connection are dropped
                  (both Linux only: the run is moved into its own network namespace)
    standby       a second provider of the same service runs from the start; the first
                  is killed and the second must take over
    standby-leaves  the second provider is killed first, then the first: the service
                  must stay down
    duplicate     a second provider of the same role names runs alongside the first for
                  --seconds; the router must reject it and keep serving the first
    provider-exit-b-paused  B is suspended for --seconds and the provider is killed halfway;
                  B must get the provider's queued data before its disconnect notice

--local runs the provider with a consumer L of its own service in the same process
(with duplicate: the second provider).

Each run gets its own configuration file and a free port, so it does not disturb a
router already running on this machine. The last line is one RESULT line.
"""

import argparse
import os
import re
import shutil
import signal
import socket
import subprocess
import sys
import tempfile
import threading
import time

WINDOWS = sys.platform.startswith('win')
EXE = '.exe' if WINDOWS else ('.mac' if sys.platform == 'darwin' else '.elf')


def binary(bin_dir, name):
    path = os.path.join(bin_dir, name + EXE)
    if not os.path.isfile(path):
        sys.exit('error: {} not found'.format(path))
    return path


def free_port():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(('127.0.0.1', 0))
        return s.getsockname()[1]


def wait_port(port, seconds=10.0):
    end = time.time() + seconds
    while time.time() < end:
        try:
            with socket.create_connection(('127.0.0.1', port), timeout=0.5):
                return True
        except OSError:
            time.sleep(0.1)
    return False


def set_key(text, key, value):
    pattern = re.compile(r'^(\s*' + re.escape(key) + r'\s*=)[^#\n]*', re.M)
    if pattern.search(text):
        return pattern.sub(lambda m: m.group(1) + ' ' + str(value) + ' ', text, count=1)
    return text + '\n{} = {}\n'.format(key, value)


def write_config(bin_dir, work, port, args):
    with open(os.path.join(bin_dir, 'config', 'areg.init'), encoding='utf-8') as f:
        text = f.read()
    keys = {
        'router::*::port::tcpip': port,
        'router::*::address::tcpip': '127.0.0.1',
        'service::*::list': 'router',
        'log::*::enable': 'false',
        'net::*::tcpip::pairs': args.pairs,
    }
    if args.timeout is not None:
        keys['net::*::tcpip::timeout'] = args.timeout
    if args.keepalive is not None:
        keys['net::*::tcpip::keepalive'] = args.keepalive
    if args.router_log:
        keys.update({
            'log::*::enable': 'true',
            'log::*::target': 'file',
            'log::*::enable::file': 'false',
            'log::mtrouter::enable::file': 'true',
            'log::mtrouter::file::location': os.path.join(work, 'router-log.txt').replace('\\', '/'),
            'log::mtrouter::scope::*': 'WARN ;',
            'log::*::scope::areg_*': 'NOTSET ;',
        })
    if args.app_log:
        keys.update({
            'log::*::enable': 'true',
            'log::*::target': 'file',
            'log::*::enable::file': 'true',
            'log::*::enable::remote': 'false',
            'log::*::file::location': os.path.join(work, '%appname%_%time%.txt').replace('\\', '/'),
            'log::*::scope::*': 'WARN ;',
            'log::*::scope::areg_*': 'WARN ;',
        })
    if args.app_log and os.environ.get('CONN_SCENARIO_DEBUG_SCOPE'):
        keys['log::*::scope::' + os.environ['CONN_SCENARIO_DEBUG_SCOPE']] = 'DEBUG | SCOPE ;'
    for key, value in keys.items():
        text = set_key(text, key, value)
    if args.router_log:
        text += '\nlog::mtrouter::scope::areg_* = WARN ;\n'
        if os.environ.get('CONN_SCENARIO_ROUTER_DEBUG_SCOPE'):
            text += 'log::mtrouter::scope::{} = DEBUG | SCOPE ;\n'.format(os.environ['CONN_SCENARIO_ROUTER_DEBUG_SCOPE'])
    path = os.path.join(work, 'areg.init')
    with open(path, 'w', encoding='utf-8') as f:
        f.write(text)
    return path


def suspend(proc, on):
    if not WINDOWS:
        os.kill(proc.pid, signal.SIGSTOP if on else signal.SIGCONT)
        return
    import ctypes
    ntdll = ctypes.WinDLL('ntdll')
    kernel = ctypes.WinDLL('kernel32')
    handle = kernel.OpenProcess(0x0800, False, proc.pid)  # PROCESS_SUSPEND_RESUME
    try:
        (ntdll.NtSuspendProcess if on else ntdll.NtResumeProcess)(handle)
    finally:
        kernel.CloseHandle(handle)


def netem(loss):
    cmd = ['tc', 'qdisc', 'replace' if loss else 'del', 'dev', 'lo', 'root']
    if loss:
        cmd += ['netem', 'loss', '100%']
    subprocess.run(cmd, check=loss, stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL)


def local_port_of(pid, router_port):
    """The local port of the connection a process holds to the router (Linux)."""
    out = subprocess.run(['ss', '-tnpH', 'dport', '=', ':{}'.format(router_port)],
                         capture_output=True, text=True).stdout
    for line in out.splitlines():
        if 'pid={},'.format(pid) in line:
            return int(line.split()[3].rsplit(':', 1)[1])
    return None


def netem_port(port, loss):
    """Drop every packet to or from one local port on lo, or remove the rule."""
    if not loss:
        subprocess.run(['tc', 'qdisc', 'del', 'dev', 'lo', 'root'],
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        return
    run = lambda *a: subprocess.run(list(a), check=True)
    run('tc', 'qdisc', 'add', 'dev', 'lo', 'root', 'handle', '1:', 'prio')
    run('tc', 'qdisc', 'add', 'dev', 'lo', 'parent', '1:3', 'handle', '30:', 'netem', 'loss', '100%')
    for field in ('sport', 'dport'):
        run('tc', 'filter', 'add', 'dev', 'lo', 'parent', '1:0', 'protocol', 'ip', 'u32',
            'match', 'ip', field, str(port), '0xffff', 'flowid', '1:3')


def proc_stat(pid):
    """Threads and RSS in KB of a Linux process, or (None, None)."""
    try:
        with open('/proc/{}/status'.format(pid)) as f:
            text = f.read()
        threads = int(re.search(r'^Threads:\s+(\d+)', text, re.M).group(1))
        rss = int(re.search(r'^VmRSS:\s+(\d+)', text, re.M).group(1))
        return threads, rss
    except (OSError, AttributeError):
        return None, None


def parse_consumer(path, t_from, t_to):
    """What one consumer saw; the window [t_from, t_to] is the disturbance."""
    out = {'recv': 0, 'lost': 0, 'bad': 0, 'downs': 0, 'ups': 0, 'late': 0, 'maxgap_ms': 0,
           'maxgap_at': 0, 'gaps': 0, 'last': 0, 'stalls_in': 0, 'attr_updates': 0, 'attr_repeats': 0}
    with open(path, encoding='utf-8', errors='replace') as f:
        for line in f:
            parts = line.split()
            if len(parts) < 2 or not parts[1].isdigit():
                continue
            tag, t = parts[0], int(parts[1])
            fields = dict(p.split('=', 1) for p in parts[3:] if '=' in p)
            if tag == 'C':
                out['recv'] = int(fields['recv'])
                out['last'] = int(fields['last'])
                gap = int(fields['maxgap_ms'])
                if gap > out['maxgap_ms']:
                    out['maxgap_ms'], out['maxgap_at'] = gap, t
            elif tag == 'CONN':
                out['ups' if parts[3] == 'up' else 'downs'] += 1
            elif tag == 'GAP':
                out['gaps'] += 1
                out['lost'] += int(fields['lost'])
            elif tag == 'ATTR':
                out['attr_updates'] = int(fields['updates'])
                out['attr_repeats'] = int(fields['repeats'])
            elif tag == 'BAD':
                out['bad'] += 1
            elif tag == 'LATE':
                out['late'] += 1
            elif tag == 'STALL':
                gap = int(fields['gap_ms'])
                if gap > out['maxgap_ms']:
                    out['maxgap_ms'], out['maxgap_at'] = gap, t
                if t_from <= t <= t_to + 60000:
                    out['stalls_in'] += 1
    return out


def sample_router(pid, port, path, stop):
    """Every 100 ms: each thread's current syscall and wait channel, and the router's socket queues."""
    with open(path, 'w') as out:
        while not stop.is_set():
            out.write('T {}\n'.format(monotonic_ms()))
            try:
                for tid in sorted(os.listdir('/proc/{}/task'.format(pid))):
                    base = '/proc/{}/task/{}/'.format(pid, tid)
                    with open(base + 'comm') as f:
                        comm = f.read().strip()
                    with open(base + 'syscall') as f:
                        call = f.read().strip()
                    with open(base + 'wchan') as f:
                        wchan = f.read().strip()
                    out.write('  {} {} {} {}\n'.format(tid, comm, wchan, call))
            except OSError:
                pass
            ss = subprocess.run(['ss', '-tnH', 'sport', '=', ':{}'.format(port)],
                                capture_output=True, text=True).stdout
            for line in ss.splitlines():
                out.write('  ss ' + ' '.join(line.split()) + '\n')
            stop.wait(0.1)


def monotonic_ms():
    return int(time.monotonic() * 1000)


def run(args):
    bin_dir = os.path.abspath(args.bin)
    router_bin = binary(bin_dir, 'mtrouter')
    app_bin = binary(bin_dir, 'areg-conn-scenario')
    work = tempfile.mkdtemp(prefix='areg-conn-')
    port = free_port()
    config = write_config(bin_dir, work, port, args)
    env = dict(os.environ, CONN_SCENARIO_BYTES=str(args.bytes))
    if args.app_log:
        env['CONN_SCENARIO_LOG'] = '1'
    if not WINDOWS:
        env['LD_LIBRARY_PATH'] = bin_dir + os.pathsep + env.get('LD_LIBRARY_PATH', '')
    procs = {}
    logs = {}

    def start(name, cmd, keep_stdin=False):
        logs[name] = os.path.join(work, name + '.log')
        procs[name] = subprocess.Popen(cmd, cwd=bin_dir, env=env,
                                       stdout=open(logs[name], 'w'),
                                       stderr=subprocess.STDOUT,
                                       stdin=subprocess.PIPE if keep_stdin else subprocess.DEVNULL)

    result = {}
    try:
        wrap = args.router_wrap.split() if args.router_wrap else []
        wrap = [w.replace('{work}', work) for w in wrap]
        # The console quits when its stdin ends, so it gets a pipe that stays open.
        start('router', wrap + [router_bin, '--console', '--load=' + config], keep_stdin=True)
        if not wait_port(port):
            sys.exit('error: mtrouter did not listen on {}'.format(port))
        start('A', [app_bin, 'consumer', config, 'A'])
        start('B', [app_bin, 'consumer', config, 'B'])
        time.sleep(1.0)
        local_second = args.local and args.action == 'duplicate'
        provider = ['both', config, str(args.bytes), str(args.gap_us), 'L'] if (args.local and not local_second) \
            else ['provider', config, str(args.bytes), str(args.gap_us)]
        start('provider', [app_bin] + provider)
        if args.action.startswith('standby') or args.action == 'duplicate':
            time.sleep(1.0)
            second = ['both', config, str(args.bytes), str(args.gap_us), 'L'] if local_second \
                else ['provider', config, str(args.bytes), str(args.gap_us)]
            start('standby', [app_bin] + second)
        time.sleep(args.warmup)
        before = proc_stat(procs['router'].pid)
        stop = threading.Event()
        sampler = None
        if args.sample and not WINDOWS:
            sampled = args.sample_process or 'router'
            sampler = threading.Thread(target=sample_router, args=(procs[sampled].pid, port,
                                       os.path.join(work, sampled + '-sample.txt'), stop))
            sampler.start()

        target = {'pause-b': 'B', 'pause-router': 'router', 'provider-exit-b-paused': 'B'}.get(args.action)
        t_from = monotonic_ms()
        print('DISTURB {} {}'.format(t_from, time.strftime('%H:%M:%S', time.localtime())) + '.{:03d}'.format(int(time.time() * 1000) % 1000), flush=True)
        if target:
            suspend(procs[target], True)
        elif args.action == 'loss':
            netem(True)
        elif args.action == 'standby':
            procs['provider'].kill()
        elif args.action == 'standby-leaves':
            procs['standby'].kill()
            time.sleep(1.0)
            procs['provider'].kill()
        elif args.action == 'loss-b':
            b_port = local_port_of(procs['B'].pid, port)
            if b_port is None:
                sys.exit('error: no connection of B to the router found')
            netem_port(b_port, True)
        mid = proc_stat(procs['router'].pid)
        if args.action == 'provider-exit-b-paused':
            time.sleep(args.seconds / 2.0)
            procs['provider'].kill()
            time.sleep(args.seconds / 2.0)
        else:
            time.sleep(args.seconds)
        if target:
            suspend(procs[target], False)
        elif args.action == 'loss':
            netem(False)
        elif args.action == 'loss-b':
            netem_port(0, False)
        t_to = monotonic_ms()
        time.sleep(args.tail)
        after = proc_stat(procs['router'].pid)
        stop.set()
        if sampler:
            sampler.join()
        result['router_threads'] = '{}/{}/{}'.format(before[0], mid[0], after[0])
        result['router_rss_kb'] = '{}/{}/{}'.format(before[1], mid[1], after[1])
    finally:
        for name in ('provider', 'standby', 'A', 'B', 'router'):
            p = procs.get(name)
            if p is None:
                continue
            try:
                suspend(p, False)
            except OSError:
                pass
            p.terminate()
        for p in procs.values():
            try:
                p.wait(timeout=5)
            except subprocess.TimeoutExpired:
                p.kill()

    sent = 0
    with open(logs['provider'], encoding='utf-8', errors='replace') as f:
        for line in f:
            m = re.match(r'P \d+ sent=(\d+)', line)
            if m:
                sent = int(m.group(1))
    print('work directory: ' + work)
    local_log = 'standby' if (args.local and args.action == 'duplicate') else 'provider'
    consumers = [('A', 'A'), ('B', 'B')] + ([('L', local_log)] if args.local else [])
    for name, log in consumers:
        c = parse_consumer(logs[log], t_from, t_to)
        for key in ('recv', 'lost', 'bad', 'downs', 'ups', 'late', 'maxgap_ms', 'attr_updates', 'attr_repeats'):
            result['{}_{}'.format(name, key)] = c[key]
        result[name + '_maxgap_s_after_start'] = round((c['maxgap_at'] - t_from) / 1000.0, 1) \
            if c['maxgap_at'] else '-'
    line = 'RESULT action={} seconds={} bytes={} gap_us={} pairs={} timeout={} sent={} '.format(
        args.action, args.seconds, args.bytes, args.gap_us, args.pairs,
        args.timeout if args.timeout is not None else 'ini', sent)
    line += ' '.join('{}={}'.format(k, v) for k, v in result.items())
    print(line)
    if not args.keep:
        shutil.rmtree(work, ignore_errors=True)
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('action', choices=['none', 'pause-b', 'pause-router', 'loss', 'loss-b',
                                           'standby', 'standby-leaves', 'duplicate',
                                           'provider-exit-b-paused'])
    parser.add_argument('--bin', required=True, help='directory holding mtrouter and areg-conn-scenario')
    parser.add_argument('--seconds', type=float, default=5.0, help='length of the disturbance')
    parser.add_argument('--bytes', type=int, default=256, help='size of one block')
    parser.add_argument('--gap-us', type=int, default=1000, help='gap between two blocks, 0 = full rate')
    parser.add_argument('--pairs', type=int, default=0, help='net::*::tcpip::pairs of the router')
    parser.add_argument('--timeout', type=int, help='net::*::tcpip::timeout, default: the shipped file')
    parser.add_argument('--keepalive', type=int, help='net::*::tcpip::keepalive')
    parser.add_argument('--warmup', type=float, default=3.0)
    parser.add_argument('--tail', type=float, default=10.0, help='seconds observed after the disturbance')
    parser.add_argument('--keep', action='store_true', help='keep the work directory and its logs')
    parser.add_argument('--sample', action='store_true',
                        help='Linux: sample the router threads and socket queues every 100 ms')
    parser.add_argument('--sample-process', choices=['router', 'provider', 'A', 'B'],
                        help='the process --sample samples, default the router')
    parser.add_argument('--local', action='store_true',
                        help='the provider also runs a consumer L of its own service')
    parser.add_argument('--router-wrap', help='command line put before mtrouter, {work} = the work directory')
    parser.add_argument('--app-log', action='store_true',
                        help='write the warnings of every process to files in the work directory')
    parser.add_argument('--router-log', action='store_true',
                        help='write the warnings of mtrouter to router-log.txt in the work directory')
    args = parser.parse_args()

    if args.action in ('loss', 'loss-b') and not WINDOWS and os.environ.get('CONN_SCENARIO_NETNS') != '1':
        cmd = ['unshare', '-rn', 'sh', '-c', 'ip link set lo up && exec "$@"', 'sh',
               sys.executable] + sys.argv
        return subprocess.call(cmd, env=dict(os.environ, CONN_SCENARIO_NETNS='1'))
    if args.action in ('loss', 'loss-b') and WINDOWS:
        sys.exit('error: loss needs Linux (tc netem in a network namespace)')
    return run(args)


if __name__ == '__main__':
    sys.exit(main())
