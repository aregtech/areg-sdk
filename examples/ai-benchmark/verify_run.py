#!/usr/bin/env python3
# ===========================================================================
# Hidden acceptance probes for one benchmark run.
#
# Runs the project an agent built, after the agent has finished, against the
# requirements every task prompt states. Each probe is generated from the
# project's own scenarios.json and assumes no framework. The snapshot the agent
# reads does not contain this file.
#
#   python3 examples/ai-benchmark/verify_run.py <run directory>
#   python3 examples/ai-benchmark/verify_run.py <run directory> --sanitize
#
# Exit code: 0 every scored probe passed, 1 a probe failed, 2 nothing to probe.
# ===========================================================================

import argparse
import copy
import json
import os
import statistics
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.normpath(os.path.join(HERE, '..', '..', 'tools', 'agent')))
import run_scenarios  # noqa: E402

try:
    import resource
except ImportError:
    resource = None

START_DELAY = 3.0
WAIT_LIMIT = 20.0
WAIT_MARGIN = 10.0
LOSS_POINTS = (0.25, 0.5, 0.75)
CPU_LIMIT = 0.5
SANITIZE_FLAGS = '-fsanitize=address,undefined -fno-omit-frame-pointer'
SANITIZER_MARKS = ('ERROR: AddressSanitizer', 'runtime error:')

REQUIREMENTS = {
    'repeat':      'the scenario exits 0, every time it is run',
    'start-order': 'the client must survive the server being started after it',
    'no-peer':     'neither program waits more than 20 seconds for something that '
                   'never arrives',
    'peer-loss':   'if one side goes away mid-scenario, the other exits non-zero',
    'cpu':         'no busy-waiting',
    'sanitize':    'no memory or undefined-behaviour defect (not a checklist item)',
}


def child_cpu():
    """CPU seconds of every finished child process, or None where not measurable."""
    if resource is None:
        return None
    usage = resource.getrusage(resource.RUSAGE_CHILDREN)
    return usage.ru_utime + usage.ru_stime


def run(scenario, build_dirs):
    """Runs one scenario. Returns (passed, detail, observed, wall, cpu)."""
    observed = {}
    before = child_cpu()
    started = time.time()
    passed, _, detail = run_scenarios.run_scenario(scenario, build_dirs, False, True,
                                                   observed)
    wall = time.time() - started
    cpu = None if before is None else child_cpu() - before
    return passed, detail, observed, wall, cpu


def normal_scenario(document):
    """The first scenario with two or more processes, no stop, and a lead that exits 0."""
    for scenario in document.get('scenarios') or []:
        procs = scenario.get('procs') or []
        if len(procs) < 2 or scenario.get('stop'):
            continue
        lead = next((p for p in procs if p.get('lead')), procs[-1])
        if lead.get('exit') == 0 and not lead.get('stdin'):
            return scenario
    return None


def processes(scenario, keep_checks=True):
    """A copy of the scenario's processes with unique names. Returns (procs, lead)."""
    procs = copy.deepcopy(scenario['procs'])
    lead_index = next((i for i, p in enumerate(procs) if p.get('lead')), len(procs) - 1)
    for index, spec in enumerate(procs):
        spec['name'] = 'p{}-{}'.format(index, spec['binary'])
        spec.pop('lead', None)
        if not keep_checks:
            for key in ('expect', 'reject', 'exit'):
                spec.pop(key, None)
    procs[lead_index]['lead'] = True
    return procs, procs[lead_index]


def variant(scenario, name, procs, timeout, stop=None):
    """A scenario of the given processes, with the original's router setting."""
    made = {'name': name, 'timeout': timeout, 'procs': procs}
    if scenario.get('router'):
        made['router'] = True
    if stop:
        made['stop'] = stop
    return made


def timed_out(observed):
    return 'timed out' in (observed.get('verdict') or '')


def result(key, passed, evidence):
    return {'probe': key, 'requirement': REQUIREMENTS[key], 'passed': passed,
            'evidence': evidence}


def probe_repeat(scenario, build_dirs, count):
    """The normal scenario, count times. Returns (result, lead times, CPU loads)."""
    passes, leads, loads, first = 0, [], [], None
    for _ in range(count):
        passed, detail, observed, wall, cpu = run(scenario, build_dirs)
        if not passed:
            first = first or detail
            continue
        passes += 1
        if observed.get('elapsed') is not None:
            leads.append(observed['elapsed'])
        if cpu is not None and wall > 0:
            loads.append(cpu / wall)
    evidence = '{} of {} passed'.format(passes, count)
    if first:
        evidence += '; first failure: ' + first
    return result('repeat', passes == count, evidence), leads, loads


def probe_start_order(scenario, build_dirs):
    """The normal scenario with the lead started first and the rest after a delay."""
    procs, lead = processes(scenario)
    lead['delay'] = START_DELAY
    order = [lead] + [p for p in procs if p is not lead]
    timeout = float(scenario.get('timeout', 60)) + START_DELAY
    passed, detail, _, _, _ = run(variant(scenario, 'start-order', order, timeout),
                                  build_dirs)
    return result('start-order', passed, 'the rest started {:.0f}s after the lead: {}'
                  .format(START_DELAY, detail))


def probe_no_peer(scenario, build_dirs):
    """The lead alone: it has to give up, non-zero, within the stated limit."""
    _, lead = processes(scenario, keep_checks=False)
    limit = WAIT_LIMIT + WAIT_MARGIN
    _, _, observed, _, _ = run(variant(scenario, 'no-peer', [lead], limit), build_dirs)
    if timed_out(observed):
        return result('no-peer', False, 'still waiting after {:.0f}s'.format(limit))
    code = observed.get('exits', {}).get(lead['name'])
    if code is None:
        return result('no-peer', False, observed.get('verdict') or 'did not run')
    return result('no-peer', code != 0, 'exited {} after {:.1f}s alone'
                  .format(code, observed.get('elapsed') or 0.0))


def probe_peer_loss(scenario, build_dirs, lead_time):
    """The normal scenario with the first other process killed at several points."""
    if not lead_time:
        return result('peer-loss', False, 'no passing normal run to time the loss against')
    points, failures, reached = [], 0, 0
    for fraction in LOSS_POINTS:
        procs, lead = processes(scenario, keep_checks=False)
        peer = next(p for p in procs if p is not lead)
        at = round(fraction * lead_time, 1)
        stop = {'proc': peer['name'], 'after': at, 'signal': 'kill'}
        timeout = at + WAIT_LIMIT + WAIT_MARGIN
        _, _, observed, _, _ = run(variant(scenario, 'peer-loss', procs, timeout, stop),
                                   build_dirs)
        code = observed.get('exits', {}).get(lead['name'])
        elapsed = observed.get('elapsed') or 0.0
        label = '{:.0%}'.format(fraction)
        if timed_out(observed):
            reached += 1
            failures += 1
            points.append('{} hung'.format(label))
        elif code == 0 and elapsed < at:
            points.append('{} finished before the loss'.format(label))
        else:
            reached += 1
            failures += 1 if code in (None, 0) else 0
            points.append('{} exit {} {:.1f}s after the loss'.format(label, code,
                                                                      elapsed - at))
    return result('peer-loss', reached > 0 and failures == 0, '; '.join(points))


def probe_cpu(loads):
    """The highest average CPU load over the passing normal runs."""
    if not loads:
        return result('cpu', None, 'not measured: no passing run, or no CPU accounting '
                                   'on this platform')
    return result('cpu', max(loads) < CPU_LIMIT,
                  'at most {:.2f} cores on average over a normal run, limit {:.2f}'
                  .format(max(loads), CPU_LIMIT))


def probe_sanitize(run_dir, work, scenario):
    """A rebuild under ASan and UBSan, the normal run and one peer loss under it."""
    build = os.path.join(run_dir, 'verify-sanitize-build')
    steps = [['cmake', '-S', work, '-B', build, '-DCMAKE_BUILD_TYPE=Debug',
              '-DCMAKE_C_FLAGS=' + SANITIZE_FLAGS, '-DCMAKE_CXX_FLAGS=' + SANITIZE_FLAGS,
              '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined',
              '-DCMAKE_SHARED_LINKER_FLAGS=-fsanitize=address,undefined'],
             ['cmake', '--build', build, '-j8']]
    for step in steps:
        done = subprocess.run(step, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                              text=True)
        if done.returncode != 0:
            last = (done.stdout.strip().splitlines() or ['no output'])[-1]
            return result('sanitize', None, 'sanitizer build failed: ' + last[:160])
    os.environ.setdefault('ASAN_OPTIONS', 'detect_leaks=0')
    build_dirs = [os.path.join(build, 'bin'), build]
    slow = dict(scenario, timeout=float(scenario.get('timeout', 60)) * 3)
    _, detail, observed, _, _ = run(slow, build_dirs)
    outputs = list((observed.get('outputs') or {}).items())
    if observed.get('elapsed'):
        procs, lead = processes(scenario, keep_checks=False)
        peer = next(p for p in procs if p is not lead)
        at = round(0.5 * observed['elapsed'], 1)
        stop = {'proc': peer['name'], 'after': at, 'signal': 'kill'}
        _, _, lost, _, _ = run(variant(scenario, 'peer-loss', procs,
                                       at + WAIT_LIMIT + WAIT_MARGIN, stop), build_dirs)
        outputs += list((lost.get('outputs') or {}).items())
    found = ['{}: {}'.format(name, line.strip()[:120])
             for name, text in outputs for line in text.splitlines()
             if any(mark in line for mark in SANITIZER_MARKS)]
    if found:
        return result('sanitize', False, '{} finding(s); first: {}'
                      .format(len(found), found[0]))
    return result('sanitize', True, 'normal run and a peer loss under ASan and UBSan: '
                                    'no finding (normal run: {})'.format(detail))


def main():
    parser = argparse.ArgumentParser(
        description='Hidden acceptance probes for one benchmark run.')
    parser.add_argument('run', help='the run directory run-benchmark.sh made')
    parser.add_argument('--repeat', type=int, default=10,
                        help='normal runs in the repeat probe (default: 10)')
    parser.add_argument('--sanitize', action='store_true',
                        help='also rebuild under ASan and UBSan and run under them')
    args = parser.parse_args()

    run_dir = os.path.abspath(args.run)
    work = os.path.join(run_dir, 'work')
    try:
        with open(os.path.join(work, 'scenarios.json'), encoding='utf-8') as handle:
            document = json.load(handle)
    except (OSError, ValueError) as error:
        print('verify_run: cannot read {}: {}'.format(
            os.path.join(work, 'scenarios.json'), error))
        return 2
    scenario = normal_scenario(document)
    if scenario is None:
        print('verify_run: no normal scenario in scenarios.json: two or more processes, '
              'no stop, and a lead that exits 0')
        return 2
    build_dirs = [os.path.join(work, 'build', 'bin'), os.path.join(work, 'build')]
    os.chdir(work)

    print('== hidden probes: {}'.format(run_dir))
    print('   normal scenario  {}'.format(scenario.get('name', 'unnamed')))
    results = []

    def report(item):
        state = {True: 'PASS', False: 'FAIL', None: 'SKIP'}[item['passed']]
        print('   {:5} {:12} {}'.format(state, item['probe'], item['requirement']))
        print('   {:5} {:12} {}'.format('', '', item['evidence']))
        sys.stdout.flush()
        results.append(item)

    repeat, leads, loads = probe_repeat(scenario, build_dirs, args.repeat)
    report(repeat)
    report(probe_start_order(scenario, build_dirs))
    report(probe_no_peer(scenario, build_dirs))
    report(probe_peer_loss(scenario, build_dirs,
                           statistics.median(leads) if leads else None))
    report(probe_cpu(loads))
    if args.sanitize:
        report(probe_sanitize(run_dir, work, scenario))

    scored = [item for item in results if item['passed'] is not None]
    passed = sum(1 for item in scored if item['passed'])
    print('   probes passed    {} of {}'.format(passed, len(scored)))
    with open(os.path.join(run_dir, 'verify.json'), 'w', encoding='utf-8') as handle:
        json.dump({'scenario': scenario.get('name'), 'passed': passed,
                   'scored': len(scored), 'results': results}, handle, indent=2)
    return 0 if passed == len(scored) else 1


if __name__ == '__main__':
    sys.exit(main())
