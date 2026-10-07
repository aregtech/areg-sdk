#!/usr/bin/env python3
# ===========================================================================
# Hidden acceptance probes for one benchmark run.
#
# Runs the project an agent built, after the agent has finished, against the
# requirements every task prompt states. Each probe is generated from the
# project's own scenarios.json and assumes no framework, except "checked", which
# reads the step bodies of bodies.txt. The snapshot the agent reads does not contain
# this file.
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
import re
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
# The deadline the task asks the application to respect.
WAIT_LIMIT = 20.0
# What a probe measures is the whole process: start, areg initialisation and
# teardown all land inside it and none of them is waiting. Both the verdict and
# the kill threshold allow this much on top, so an application that gives up at
# the limit is not refused for the seconds it did not spend waiting.
WAIT_MARGIN = 10.0
LOSS_POINTS = (0.25, 0.5, 0.75)
# Seconds from the kill to the lead seeing the loss: the runner polls every 50 ms,
# and the router then reports the closed connection. A lead that exits 0 inside
# this window may have finished before the loss reached it.
LOSS_LATENCY = 0.25
# Seconds a process that gave up after a loss may take to end: areg's teardown and the
# runner's poll. Unlike WAIT_MARGIN, no start-up falls inside a loss.
LOSS_TEARDOWN = 3.0
# Lines of the lead's own output a failed loss point keeps as its evidence.
LOSS_TAIL_LINES = 8
CPU_LIMIT = 0.5
# The least time, in seconds, a normal run of a task with stated timing can take: half
# of what its own waits add up to, so only a run that skips the timed work falls below.
# Keyed by the prompt's file name; kept here and not in the prompt, so it cannot be met
# by a sleep.
MIN_SECONDS = {
    'prompt-washer.md':        6.3,    # 12.6 s: every fill, stage, spin and failed attempt
    'prompt-elevator.md':      4.2,    # 8.4 s: the floors travelled and four door sequences
    'prompt-tempalarm.md':     2.2,    # 4.4 s: 22 readings of 200 ms up to the clear
    'prompt-sensorgateway.md': 1.25,   # 2.5 s: 20 sets of 100 ms and 500 ms of silence
    'prompt-greenhouse.md':    0.7,    # 1.4 s: five and then two degrees of 200 ms
    'prompt-orderdesk.md':     0.45,   # 0.9 s: three charges of 300 ms in sequence
}
SANITIZE_FLAGS = '-fsanitize=address,undefined -fno-omit-frame-pointer'
SANITIZER_MARKS = ('ERROR: AddressSanitizer', 'runtime error:')

REQUIREMENTS = {
    'repeat':      'the scenario exits 0, every time it is run',
    'start-order': 'the client must survive the server being started after it',
    'no-peer':     'neither program waits more than 20 seconds for something that '
                   'never arrives',
    'peer-loss':   'if one side goes away mid-scenario, the other exits non-zero',
    'cpu':         'no busy-waiting',
    'programs':    'as many separate programs as the task asks for, all in the normal run',
    'lines':       'every line the task says its programs print, in the normal run',
    'checked':     'a printed task line states only values its steps compared',
    'timing':      'the timed work the task states takes its time in the normal run',
    'sanitize':    'no memory or undefined-behaviour defect (not a checklist item)',
}


# How a task names its programs, and how an agent reports its checklist.
PROGRAMS_RE = re.compile(r'\b(two|three|four|five)\s+separate\s+programs\b', re.IGNORECASE)
PROGRAM_COUNT = {'two': 2, 'three': 3, 'four': 4, 'five': 5}
CLAIM_RE = re.compile(r'acceptance items passing\s*\|\s*(\d+)\s+of\s+(\d+)', re.IGNORECASE)
# The lines a task says its programs print: a fenced block with the info string "lines".
LINES_RE = re.compile(r'^```lines[ \t]*\n(.*?)^```', re.MULTILINE | re.DOTALL)
PLACEHOLDER_RE = re.compile(r'<[^<>]+>')


def task_file(run_dir):
    """The path of the task prompt the run was given, from meta.txt, or ''."""
    try:
        with open(os.path.join(run_dir, 'meta.txt'), encoding='utf-8') as handle:
            for line in handle:
                key, _, value = line.partition(' ')
                if key == 'task':
                    return value.strip()
    except OSError:
        pass
    return ''


def task_text(run_dir):
    """The task prompt the run was given, or '' when it cannot be read."""
    path = task_file(run_dir)
    try:
        with open(path, encoding='utf-8') as task:
            return task.read()
    except OSError:
        return ''


def probe_programs(scenario, run_dir):
    """The normal scenario runs as many distinct programs as the task asks for."""
    found = PROGRAMS_RE.search(task_text(run_dir))
    if found is None:
        return result('programs', None, 'the task names no number of separate programs')
    need = PROGRAM_COUNT[found.group(1).lower()]
    have = sorted(set(proc.get('binary') for proc in scenario.get('procs') or []))
    return result('programs', len(have) >= need,
                  '{} asked, {} in the normal run: {}'.format(need, len(have), ', '.join(have)))


def task_lines(text):
    """Every line of the task's "lines" blocks, in order."""
    return [line.strip() for block in LINES_RE.findall(text)
            for line in block.splitlines() if line.strip()]


def line_pattern(line):
    """A printed line ending with this text, any case and spacing; <name> is any value."""
    parts = []
    for index, piece in enumerate(PLACEHOLDER_RE.split(line)):
        if index:
            parts.append('.+?')
        parts += [r'\s+' if token.isspace() else re.escape(token)
                  for token in re.split(r'(\s+)', piece) if token]
    return re.compile(r'(?<![0-9a-z])' + ''.join(parts) + r'\s*$', re.IGNORECASE)


def judge_lines(wanted, outputs):
    """Every wanted line is printed by some process of the normal run."""
    if not wanted:
        return result('lines', None, 'the task names no printed lines')
    printed = [line for text in outputs.values() for line in text.splitlines()]
    missing = [want for want in wanted
               if not any(line_pattern(want).search(line) for line in printed)]
    evidence = '{} of {} printed'.format(len(wanted) - len(missing), len(wanted))
    if missing:
        evidence += '; missing: ' + '; '.join('"{}"'.format(want) for want in missing)
    return result('lines', not missing, evidence)


# A section of bodies.txt, its C++ strings and comments, and a number in text or code.
SECTION_RE = re.compile(r'^== (\S+)[ \t]*$', re.MULTILINE)
STRING_RE = re.compile(r'"((?:[^"\\\n]|\\.)*)"')
COMMENT_RE = re.compile(r'//[^\n]*|/\*.*?\*/', re.DOTALL)
CODE_NUMBER_RE = re.compile(r'(?<![\w.])(\d+(?:\.\d+)?)[uUlLfF]*(?![\w.])')
TEXT_NUMBER_RE = re.compile(r'(?:([A-Za-z_]+)\s+)?(?<![\w.])(\d+(?:\.\d+)?)(?![\w.])')
STEP_PREFIX_RE = re.compile(r'\bstep\s+\d+\s*:', re.IGNORECASE)
# A statement that prints, and a string a statement compares against.
OUTPUT_RE = re.compile(r'\bcout\b|\bprintf\s*\(|\bputs\s*\(')
COMPARED_STRING_RE = re.compile(
    r'(?:==|!=)\s*"((?:[^"\\\n]|\\.)*)"|"((?:[^"\\\n]|\\.)*)"\s*(?:==|!=)'
    r'|\b(?:find\w*|compare|starts_with|ends_with|contains)\s*\(\s*"((?:[^"\\\n]|\\.)*)"')


def body_sections(text):
    """{section name: body} of a bodies.txt, file prefix kept; a later one wins."""
    sections = {}
    marks = list(SECTION_RE.finditer(text))
    for index, mark in enumerate(marks):
        end = marks[index + 1].start() if index + 1 < len(marks) else len(text)
        sections[mark.group(1)] = text[mark.end():end]
    return sections


def step_order(design):
    """[(step name, its until and args values)] of every interface's steps, in order."""
    return [(step.get('name', ''),
             [str(value) for key in ('until', 'args') for value in (step.get(key) or {}).values()])
            for spec in design.get('interfaces') or [] for step in spec.get('steps') or []]


def number(text):
    """A number as one spelling: 12, 12.0 and 12.00 are the same."""
    return text.rstrip('0').rstrip('.') if '.' in text else text


def stated_values(body, wanted):
    """[(word, number)] a body prints as fixed text inside a task line, without step N:."""
    pieces = [piece.lower() for line in wanted for piece in PLACEHOLDER_RE.split(line)]
    body = COMMENT_RE.sub('', body)
    masked = STRING_RE.sub(lambda found: '"' + 'x' * len(found.group(1)) + '"', body)
    stated = []
    for found in STRING_RE.finditer(body):
        start = masked.rfind(';', 0, found.start()) + 1
        end = masked.find(';', found.end())
        if not OUTPUT_RE.search(masked[start:end if end >= 0 else len(masked)]):
            continue
        text = found.group(1).replace('\\"', '"').strip()
        if len(text) < 6 or not any(text.lower() in piece for piece in pieces):
            continue
        for word, value in TEXT_NUMBER_RE.findall(STEP_PREFIX_RE.sub('', text)):
            stated.append((word, number(value)))
    return stated


def compared_values(bodies, untils):
    """Every number these bodies' code or compared strings hold, and every step value."""
    found = []
    for body in bodies:
        body = COMMENT_RE.sub('', body)
        code = STRING_RE.sub('""', body)
        found += [number(value) for value in CODE_NUMBER_RE.findall(code)]
        for groups in COMPARED_STRING_RE.findall(body):
            found += [number(value) for text in groups
                      for _, value in TEXT_NUMBER_RE.findall(text)]
    return found + [number(value) for value in untils]


def read_text(path):
    """The text of a file, or None when it cannot be read."""
    try:
        with open(path, encoding='utf-8') as handle:
            return handle.read()
    except OSError:
        return None


def read_json(path):
    """A JSON file's object, or {} when it cannot be read."""
    try:
        with open(path, encoding='utf-8') as handle:
            return json.load(handle)
    except (OSError, ValueError):
        return {}


def judge_checked(wanted, bodies_text, design):
    """Each fixed value a printed task line states is sent or compared by the steps it ends."""
    if not wanted:
        return result('checked', None, 'the task names no printed lines')
    if bodies_text is None:
        return result('checked', None, 'no bodies.txt: the step bodies cannot be read')
    sections = body_sections(bodies_text)
    order = step_order(design)
    position = {'step_' + name: index for index, (name, _) in enumerate(order)}
    short = {name: name.rpartition(':')[2] for name in sections}
    printing = sorted((name for name in sections if stated_values(sections[name], wanted)),
                      key=lambda name: position.get(short[name], -1))
    shared = [body for name, body in sections.items() if short[name] == 'consumer_state']
    unchecked = []
    begin = 0
    for name in printing:
        if short[name] in position:
            span = order[begin:position[short[name]] + 1]
            begin = position[short[name]] + 1
            steps = set('step_' + step for step, _ in span)
            bodies = shared + [body for other, body in sections.items() if short[other] in steps]
            untils = [value for _, values in span for value in values]
        else:
            bodies, untils = list(sections.values()), []
        have = compared_values(bodies, untils)
        missing = []
        for word, value in stated_values(sections[name], wanted):
            if value in have:
                have.remove(value)
            else:
                missing.append((word + ' ' + value).strip())
        if missing:
            unchecked.append('{} prints {} unchecked'.format(name, ', '.join(missing)))
    if not printing:
        return result('checked', True, 'no task line is printed as fixed text with a value')
    evidence = '{} of {} printing bodies compare every value they print'.format(
        len(printing) - len(unchecked), len(printing))
    if unchecked:
        evidence += '; ' + '; '.join(unchecked)
    return result('checked', not unchecked, evidence)


def judge_timing(least, lead_time):
    """The normal run took at least the least time its task's stated waits allow."""
    if least is None:
        return result('timing', None, 'the task states no timing')
    if not lead_time:
        return result('timing', None, 'no passing normal run to time')
    return result('timing', lead_time >= least,
                  'a normal run takes {:.2f}s; the timing the task states needs at least '
                  '{:.2f}s'.format(lead_time, least))


def claimed(run_dir):
    """The last acceptance count the agent reported, as "n of m", or None.

    Reported beside the probes and not scored: an agent counts an item it did not
    observe as not passing, so a correct run can claim less than all.
    """
    claims = []
    for name in ('run.out', 'transcript.jsonl'):
        try:
            with open(os.path.join(run_dir, name), encoding='utf-8', errors='replace') as handle:
                claims += CLAIM_RE.findall(handle.read())
        except OSError:
            continue
    return '{} of {}'.format(*claims[-1]) if claims else None


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


def ran(observed):
    """Whether any process of the run reached an exit code."""
    exits = observed.get('exits') or {}
    return any(code is not None for code in exits.values())


def result(key, passed, evidence):
    return {'probe': key, 'requirement': REQUIREMENTS[key], 'passed': passed,
            'evidence': evidence}


def progress(text):
    """Overwrites one status line on a terminal; prints nothing anywhere else."""
    if sys.stderr.isatty():
        sys.stderr.write('\r   {:<76}'.format(text)[:80] + ('\r' if not text else ''))
        sys.stderr.flush()


def probe_repeat(scenario, build_dirs, count):
    """The normal scenario, count times.

    Returns (result, lead times, CPU loads, the outputs of the first passing run, or
    of the first run when none passed).
    """
    passes, leads, loads, first, outputs = 0, [], [], None, None
    for index in range(count):
        progress('repeat: run {} of {}{}'.format(
            index + 1, count, ', {:.0f}s each'.format(leads[-1]) if leads else ''))
        passed, detail, observed, wall, cpu = run(scenario, build_dirs)
        if outputs is None or (passed and not passes):
            outputs = observed.get('outputs') or {}
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
    return result('repeat', passes == count, evidence), leads, loads, outputs or {}


def probe_start_order(scenario, build_dirs):
    """The normal scenario with the lead started first and the rest after a delay."""
    progress('start-order: the rest start {:.0f}s after the lead'.format(START_DELAY))
    procs, lead = processes(scenario)
    lead['delay'] = START_DELAY
    order = [lead] + [p for p in procs if p is not lead]
    timeout = float(scenario.get('timeout', 60)) + START_DELAY
    passed, detail, _, _, _ = run(variant(scenario, 'start-order', order, timeout),
                                  build_dirs)
    return result('start-order', passed, 'the rest started {:.0f}s after the lead: {}'
                  .format(START_DELAY, detail))


def judge_no_peer(name, observed):
    """The verdict of the no-peer probe, from what was observed."""
    limit = WAIT_LIMIT + WAIT_MARGIN
    if timed_out(observed):
        return result('no-peer', False, 'still waiting after {:.0f}s'.format(limit))
    code = observed.get('exits', {}).get(name)
    elapsed = observed.get('elapsed')
    if code is None or elapsed is None:
        return result('no-peer', False, observed.get('verdict') or 'did not run')
    if code == 0:
        return result('no-peer', False,
                      'exited 0 after {:.1f}s alone, with no peer to serve'.format(elapsed))
    return result('no-peer', elapsed <= limit,
                  'exited {} after {:.1f}s alone, limit {:.0f}s and {:.0f}s allowed '
                  'for start and teardown'
                  .format(code, elapsed, WAIT_LIMIT, WAIT_MARGIN))


def probe_no_peer(scenario, build_dirs):
    """The lead alone: it has to give up, non-zero, within the stated limit."""
    progress('no-peer: the lead runs alone')
    _, lead = processes(scenario, keep_checks=False)
    limit = WAIT_LIMIT + WAIT_MARGIN
    _, _, observed, _, _ = run(variant(scenario, 'no-peer', [lead], limit), build_dirs)
    return judge_no_peer(lead['name'], observed)


def judge_loss_point(label, at, name, observed):
    """One kill point. Returns (the loss was reached, it failed, the evidence).

    Every process the kill left running exits non-zero, each within WAIT_LIMIT and
    LOSS_TEARDOWN of the loss or of the survivor that exited before it: a process
    that waits on another survivor learns of the loss only when that one gives up.
    """
    if timed_out(observed):
        return True, True, '{} hung'.format(label)
    code = observed.get('exits', {}).get(name)
    elapsed = observed.get('elapsed')
    if code is None or elapsed is None:
        return True, True, '{} did not run: {}'.format(
            label, observed.get('verdict') or 'no exit code')
    fired = [action.get('fired_at') for action in observed.get('actions') or []]
    killed = fired[0] if fired and fired[0] is not None else at
    if code == 0 and elapsed < killed + LOSS_LATENCY:
        # The loss never reached the process before it finished its work, so this
        # point proves nothing.
        return False, False, '{} finished at {:.2f}s, before the loss at {:.2f}s ' \
            'reached it'.format(label, elapsed, killed)
    stopped = set(observed.get('stopped') or [])
    ended_at = observed.get('ended_at') or {name: elapsed}
    survivors = sorted((who for who in ended_at if who not in stopped),
                       key=lambda who: (ended_at[who] is None, ended_at[who] or 0))
    allowed = WAIT_LIMIT + LOSS_TEARDOWN
    said, bad, since = [], False, killed
    for who in survivors:
        when, code = ended_at[who], observed.get('exits', {}).get(who)
        who = who if len(survivors) > 1 else ''
        if when is None:
            bad = True
            said.append('{} never exited'.format(who).strip())
            continue
        if when < killed:
            bad = True
            said.append('{} exited {} at {:.2f}s, before the loss at {:.2f}s'
                        .format(who, code, when, killed).strip())
        elif code == 0:
            bad = True
            said.append('{} exit 0 {:.2f}s after the loss'.format(who, when - killed).strip())
        elif when - since > allowed:
            bad = True
            said.append('{} exit {} {:.2f}s after the loss, over the {:.0f}s allowed'
                        .format(who, code, when - killed, allowed).strip())
        else:
            said.append('{} exit {} {:.2f}s after the loss'
                        .format(who, code, when - killed).strip())
        since = max(since, when)
    return True, bad, '{} {}'.format(label, ', '.join(said))


def probe_peer_loss(scenario, build_dirs, lead_time):
    """The normal scenario with the first other process killed at several points.

    The first process is the one the others need, as every task lists it.
    """
    if not lead_time:
        return result('peer-loss', False, 'no passing normal run to time the loss against')
    points, failures, reached = [], 0, 0
    for fraction in LOSS_POINTS:
        progress('peer-loss: the peer is killed at {:.0%} of a normal run'.format(fraction))
        procs, lead = processes(scenario, keep_checks=False)
        peer = next(p for p in procs if p is not lead)
        at = round(fraction * lead_time, 2)
        stop = {'proc': peer['name'], 'after': at, 'signal': 'kill'}
        timeout = at + (WAIT_LIMIT + LOSS_TEARDOWN) * (len(procs) - 1) + LOSS_TEARDOWN
        observed = {}
        run_scenarios.run_scenario(variant(scenario, 'peer-loss', procs, timeout, stop),
                                   build_dirs, False, True, observed, wait_all=True)
        hit, bad, text = judge_loss_point('{:.0%}'.format(fraction), at,
                                          lead['name'], observed)
        reached += 1 if hit else 0
        failures += 1 if bad else 0
        if bad:
            said = (observed.get('outputs') or {}).get(lead['name'], '').strip()
            text += ' [last lines of {}: {}]'.format(
                lead['name'], ' | '.join(said.splitlines()[-LOSS_TAIL_LINES:]) or 'none')
        points.append(text)
    if lead_time < 1:
        points.insert(0, 'a normal run takes {:.2f}s, so a point within {:.2f}s of its '
                         'end is not reached'.format(lead_time, LOSS_LATENCY))
    if reached == 0:
        return result('peer-loss', None, 'not evaluated: no run reached the loss; '
                                         + '; '.join(points))
    return result('peer-loss', failures == 0, '; '.join(points))


def probe_cpu(loads):
    """The highest average CPU load over the passing normal runs."""
    if not loads:
        return result('cpu', None, 'not measured: no passing run, or no CPU accounting '
                                   'on this platform')
    return result('cpu', max(loads) < CPU_LIMIT,
                  'at most {:.2f} cores on average over a normal run, limit {:.2f}'
                  .format(max(loads), CPU_LIMIT))


def binary_dirs(build, document):
    """build/bin and build, then every directory under build holding a scenario binary."""
    names = set()
    for scenario in document.get('scenarios') or []:
        for proc in scenario.get('procs') or []:
            if proc.get('binary'):
                names.update((proc['binary'], proc['binary'] + run_scenarios.SUFFIX))
    found = [os.path.join(build, 'bin'), build]
    for root, dirs, files in os.walk(build):
        dirs[:] = [d for d in dirs if d != 'CMakeFiles']
        if root not in found and any(
                name in names and os.access(os.path.join(root, name), os.X_OK)
                for name in files):
            found.append(root)
    return found


def probe_sanitize(run_dir, work, scenario, document):
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
    build_dirs = binary_dirs(build, document)
    slow = dict(scenario, timeout=float(scenario.get('timeout', 60)) * 3)
    _, detail, observed, _, _ = run(slow, build_dirs)
    if not ran(observed):
        return result('sanitize', None, 'not evaluated: the instrumented application did '
                                        'not run: ' + (observed.get('verdict') or detail))
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


# Every case the verdicts have to get right: what was observed, and the verdict it
# earns. The three marked "audit" each passed before the probe judged them.
SELF_TEST_NO_PEER = (
    ('gave up inside the limit', {'exits': {'p': 1}, 'elapsed': 12.0}, True),
    ('audit: gave up past the allowance', {'exits': {'p': 1}, 'elapsed': 35.0}, False),
    ('exactly at the limit', {'exits': {'p': 1}, 'elapsed': 20.0}, True),
    ('the limit plus the start it did not spend waiting',
     {'exits': {'p': 1}, 'elapsed': 20.4}, True),
    ('exactly at the allowance', {'exits': {'p': 1}, 'elapsed': 30.0}, True),
    ('audit: past the allowance by a whisker',
     {'exits': {'p': 1}, 'elapsed': 30.1}, False),
    ('exited 0 with no peer', {'exits': {'p': 0}, 'elapsed': 5.0}, False),
    ('still waiting', {'verdict': 'timed out after 30s'}, False),
    ('never ran', {'exits': {}, 'verdict': 'binary not found: x'}, False),
)

SELF_TEST_LOSS = (
    ('exited after the loss', 12.0, {'exits': {'p': 1}, 'elapsed': 13.0}, (True, False)),
    ('audit: crashed before the loss', 12.0, {'exits': {'p': 1}, 'elapsed': 0.1},
     (True, True)),
    ('finished its work before the loss', 12.0, {'exits': {'p': 0}, 'elapsed': 5.0},
     (False, False)),
    ('survived the loss and exited 0', 12.0, {'exits': {'p': 0}, 'elapsed': 13.0},
     (True, True)),
    ('hung', 12.0, {'verdict': 'timed out after 40s'}, (True, True)),
    ('never ran', 12.0, {'exits': {}, 'verdict': 'binary not found: x'}, (True, True)),
    ('audit: a 0.18s run killed at 75%, done as the kill landed', 0.14,
     {'exits': {'p': 0}, 'elapsed': 0.18, 'actions': [{'fired_at': 0.15}]}, (False, False)),
    ('a 0.18s run, the kill time unknown', 0.14, {'exits': {'p': 0}, 'elapsed': 0.18},
     (False, False)),
    ('exited 0 two seconds after the kill', 0.14,
     {'exits': {'p': 0}, 'elapsed': 2.15, 'actions': [{'fired_at': 0.15}]}, (True, True)),
    ('audit: 07b, the only survivor 29.7s after the loss', 1.0,
     {'exits': {'w': -9, 'p': 1}, 'elapsed': 30.7, 'stopped': ['w'],
      'ended_at': {'w': 1.0, 'p': 30.7}}, (True, True)),
    ('audit: 07b, a survivor that never exits', 1.0,
     {'exits': {'w': -9, 'd': -15, 'p': 1}, 'elapsed': 21.3, 'stopped': ['w'],
      'ended_at': {'w': 1.0, 'd': None, 'p': 21.3}}, (True, True)),
    ('07a, every survivor exits after the loss', 1.0,
     {'exits': {'w': -9, 'd': 1, 'p': 1}, 'elapsed': 21.6, 'stopped': ['w'],
      'ended_at': {'w': 1.0, 'd': 21.4, 'p': 21.6}}, (True, False)),
    ('a survivor waiting on another survivor exits after it', 1.0,
     {'exits': {'w': -9, 'd': 1, 'p': 1}, 'elapsed': 42.0, 'stopped': ['w'],
      'ended_at': {'w': 1.0, 'd': 21.4, 'p': 42.0}}, (True, False)),
    ('a survivor exits 0 after the loss', 1.0,
     {'exits': {'w': -9, 'd': 0, 'p': 1}, 'elapsed': 21.6, 'stopped': ['w'],
      'ended_at': {'w': 1.0, 'd': 21.4, 'p': 21.6}}, (True, True)),
)

# Printed lines against the wanted ones: (name, wanted, printed, verdict).
SELF_TEST_LINES = (
    ('every line, after a prefix and in another case',
     ['step 1: refused, insufficient credit', 'step 4: Cappuccino finished, credit 20'],
     'user: Step 1: Refused, insufficient credit\n[4]  step 4:  Cappuccino finished, '
     'credit 20  \n', True),
    ('a longer number is not the wanted one', ['step 4: credit 20'],
     'step 4: credit 200\n', False),
    ('the wanted text inside a longer word', ['step 1: done'], 'substep 1: done\n', False),
    ('the wanted text with more after it', ['step 1: done'], 'step 1: done twice\n', False),
    ('a placeholder takes the value', ['step 3: paused in <stage>, resumed in the same stage'],
     'step 3: paused in Frothing milk, resumed in the same stage\n', True),
    ('a placeholder takes something', ['step 5: halted at floor <floor>'],
     'step 5: halted at floor \n', False),
    ('audit: a partial run, every probe passing', ['step 2: Cappuccino accepted',
     'step 6: refilled, Latte finished'], 'step 2: Cappuccino accepted\n', False),
    ('no lines asked', [], 'anything\n', None),
)

# (case, least seconds or None, median lead time or None, expected verdict)
# Bodies against the task lines: (name, wanted, bodies.txt, steps, expected verdict).
CHECK_LINE = ['step 1: bolt 10, nut 5, gear 3']
SELF_TEST_CHECKED = (
    ('one of three values compared', CHECK_LINE,
     '== step_start\nif (bolt != 10) fail("bolt");\n'
     'else std::cout << "step 1: bolt 10, nut 5, gear 3" << std::endl;\n',
     [{'name': 'start'}], False),
    ('every value compared', CHECK_LINE,
     '== step_start\nif (bolt != 10u || nut != 5 || gear != 3) { fail("step 1"); return; }\n'
     'std::cout << "step 1: bolt 10, nut 5, gear 3" << std::endl;\n',
     [{'name': 'start'}], True),
    ('compared in an earlier step and an until', CHECK_LINE,
     '== step_first\nif (bolt != 10) fail("x");\n== step_last\n'
     'if (nut != 5) fail("x"); else std::cout << "step 1: bolt 10, nut 5, gear 3";\n',
     [{'name': 'first'}, {'name': 'last', 'until': {'gear': 3}}], True),
    ('a value the step sent', ['step 1: refused, hysteresis 0'],
     '== step_send\nif (accepted) fail("x"); else std::cout << "step 1: refused, hysteresis 0";\n',
     [{'name': 'send', 'args': {'hysteresis': 0}}], True),
    ('a semicolon inside the printed text', ['step 5: order 4 accepted; order 6 refused'],
     '== step_all\nif (count != 3) fail("x"); '
     'else std::cout << "step 5: order 4 accepted; order 6 refused" << std::endl;\n',
     [{'name': 'all'}], False),
    ('printed by a helper, compared in a handler', CHECK_LINE,
     '== consumer_state\nvoid show() { std::cout << "step 1: bolt 10, nut 5, gear 3"; }\n'
     '== Client.cpp:update_stock\nif (bolt == 10 && nut == 5 && gear == 3) mOwner.show();\n',
     [], True),
    ('a name compared as a string', ['step 2: normal done: rinse 1'],
     '== step_done\nif (stage != "rinse 1") fail("x"); '
     'else std::cout << "step 2: normal done: rinse 1";\n', [{'name': 'done'}], True),
    ('only the step number is a number', ['step 6: done'],
     '== step_end\nstd::cout << "step 6: done" << std::endl;\n', [{'name': 'end'}], True),
    ('the value printed from a variable', CHECK_LINE,
     '== step_start\nstd::cout << "step 1: bolt " << bolt << ", nut " << nut << ", gear "'
     ' << gear;\n', [{'name': 'start'}], True),
    ('a string in a fail message is no check', CHECK_LINE,
     '== step_start\nif (bolt != 10) fail("nut 5 gear 3");\n'
     'std::cout << "step 1: bolt 10, nut 5, gear 3";\n', [{'name': 'start'}], False),
    ('no bodies.txt', CHECK_LINE, None, [], None),
)

SELF_TEST_TIMING = (
    ('the task states no timing', None, 0.05, None),
    ('no passing run', 6.3, None, None),
    ('the timed work skipped: 06f-washer', 6.3, 0.05, False),
    ('the timed work done: 06e-washer', 6.3, 13.5, True),
    ('at the least time', 0.45, 0.45, True),
)

SELF_TEST_RAN = (
    ('audit: nothing ran', {'exits': {}, 'verdict': 'binary not found: x'}, False),
    ('no exit code', {'exits': {'p': None}}, False),
    ('a process exited', {'exits': {'p': 0}}, True),
)


def self_test():
    """Every verdict against a case it has to get right. No build, no processes."""
    failures = []
    for name, observed, expected in SELF_TEST_NO_PEER:
        got = judge_no_peer('p', observed)['passed']
        if got is not expected:
            failures.append('no-peer, {}: {} expected, got {}'
                            .format(name, expected, got))
    for name, at, observed, expected in SELF_TEST_LOSS:
        hit, bad, _ = judge_loss_point('50%', at, 'p', observed)
        if (hit, bad) != expected:
            failures.append('peer-loss, {}: {} expected, got {}'
                            .format(name, expected, (hit, bad)))
    for name, wanted, printed, expected in SELF_TEST_LINES:
        got = judge_lines(wanted, {'p': printed})['passed']
        if got is not expected:
            failures.append('lines, {}: {} expected, got {}'.format(name, expected, got))
    blocks = task_lines('text\n```lines\nstep 1: a\n\nstep 2: b\n```\n```\nx\n```\n')
    if blocks != ['step 1: a', 'step 2: b']:
        failures.append('lines, the task block: {} read'.format(blocks))
    for name, wanted, bodies, steps, expected in SELF_TEST_CHECKED:
        got = judge_checked(wanted, bodies, {'interfaces': [{'steps': steps}]})['passed']
        if got is not expected:
            failures.append('checked, {}: {} expected, got {}'.format(name, expected, got))
    for name, least, lead_time, expected in SELF_TEST_TIMING:
        got = judge_timing(least, lead_time)['passed']
        if got is not expected:
            failures.append('timing, {}: {} expected, got {}'.format(name, expected, got))
    for name, observed, expected in SELF_TEST_RAN:
        if ran(observed) is not expected:
            failures.append('sanitize, {}: {} expected, got {}'
                            .format(name, expected, ran(observed)))
    cases = (len(SELF_TEST_NO_PEER) + len(SELF_TEST_LOSS) + len(SELF_TEST_LINES) + 1
             + len(SELF_TEST_CHECKED) + len(SELF_TEST_TIMING) + len(SELF_TEST_RAN))
    for line in failures:
        print('   FAIL  ' + line)
    print('verify_run --self-test: {} case(s), {} failure(s)'.format(cases, len(failures)))
    return 1 if failures else 0


def main():
    parser = argparse.ArgumentParser(
        description='Hidden acceptance probes for one benchmark run.')
    parser.add_argument('run', nargs='?', help='the run directory run-benchmark.sh made')
    parser.add_argument('--repeat', type=int, default=10,
                        help='normal runs in the repeat probe (default: 10)')
    parser.add_argument('--sanitize', action='store_true',
                        help='also rebuild under ASan and UBSan and run under them')
    parser.add_argument('--self-test', action='store_true',
                        help='check every verdict against its cases and exit')
    args = parser.parse_args()

    if args.self_test:
        return self_test()
    if not args.run:
        parser.error('a run directory is required')

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
    build_dirs = binary_dirs(os.path.join(work, 'build'), document)
    os.chdir(work)

    print('== hidden probes: {}'.format(run_dir))
    print('   normal scenario  {}'.format(scenario.get('name', 'unnamed')))
    results = []

    def report(item):
        progress('')
        state = {True: 'PASS', False: 'FAIL', None: 'SKIP'}[item['passed']]
        print('   {:5} {:12} {}'.format(state, item['probe'], item['requirement']))
        print('   {:5} {:12} {}'.format('', '', item['evidence']))
        sys.stdout.flush()
        results.append(item)

    repeat, leads, loads, outputs = probe_repeat(scenario, build_dirs, args.repeat)
    report(repeat)
    report(probe_start_order(scenario, build_dirs))
    report(probe_no_peer(scenario, build_dirs))
    report(probe_peer_loss(scenario, build_dirs,
                           statistics.median(leads) if leads else None))
    report(probe_cpu(loads))
    report(probe_programs(scenario, run_dir))
    report(judge_lines(task_lines(task_text(run_dir)), outputs))
    report(judge_checked(task_lines(task_text(run_dir)), read_text('bodies.txt'),
                         read_json('design.json')))
    report(judge_timing(MIN_SECONDS.get(os.path.basename(task_file(run_dir))),
                        statistics.median(leads) if leads else None))
    if args.sanitize:
        report(probe_sanitize(run_dir, work, scenario, document))

    scored = [item for item in results if item['passed'] is not None]
    skipped = [item['probe'] for item in results if item['passed'] is None]
    passed = sum(1 for item in scored if item['passed'])
    claim = claimed(run_dir)
    print('   agent claims     {} acceptance item(s) passing'.format(claim or 'no count of'))
    print('   probes passed    {} of {}{}'.format(
        passed, len(scored),
        ', {} not evaluated: {}'.format(len(skipped), ', '.join(skipped))
        if skipped else ''))
    # The sanitizer pass writes its own file. A run verified twice keeps both
    # artefacts, and each one records the settings that produced it.
    name = 'verify-sanitize.json' if args.sanitize else 'verify.json'
    with open(os.path.join(run_dir, name), 'w', encoding='utf-8') as handle:
        json.dump({'scenario': scenario.get('name'), 'passed': passed,
                   'scored': len(scored), 'skipped': skipped, 'claimed': claim,
                   'repeat': args.repeat, 'sanitize': args.sanitize,
                   'verified': time.strftime('%Y-%m-%dT%H:%M:%SZ', time.gmtime()),
                   'results': results},
                  handle, indent=2)
    return 0 if passed == len(scored) else 1


if __name__ == '__main__':
    sys.exit(main())
