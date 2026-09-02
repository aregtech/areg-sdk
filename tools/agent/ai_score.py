#!/usr/bin/env python3
# ===========================================================================
# Scores this repository as a place an AI agent builds applications from.
#
#   python3 tools/agent/ai_score.py               the three scores
#   python3 tools/agent/ai_score.py --verbose     every component, and what failed
#   python3 tools/agent/ai_score.py --json        the same, for CI
#   python3 tools/agent/ai_score.py --floor 95    exit 1 when any score is below it
#   python3 tools/agent/ai_score.py --floor 94,96,59   one floor per score, in order
#
# The rubric, and the reasoning behind every weight, is docs/ai-readiness.md.
# Read that first; this file is its implementation and nothing more.
#
# The instrument is static. It never builds, never runs the generator and never
# calls a model, so it gives the same three numbers on any machine with no
# network. Where a property can only be shown by running something, what is
# scored is whether CI runs it, not whether it passes here: that is the property
# that keeps the repository true after this week.
#
# Exit code 0, or 1 when --floor is given and some score is under it.
# ===========================================================================
import argparse
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
AGENT_DIR = os.path.join(ROOT, 'docs', 'agent')
RECIPE_DIR = os.path.join(AGENT_DIR, 'recipes')
KB = 1024.0


# ---------------------------------------------------------------------------
# The feature catalogue.
#
# One row per thing an agent is expected to be able to do, taken from the scope
# statement of AGENTS.md. F1 scores each row three ways: the page answers it,
# an example shows it, an eval grades it. T3 counts the rows no page answers.
# R1 counts the rows no eval grades.
#
#   page    the document that must answer it, or None to search the whole corpus
#   proof   a substring that must appear there; proves coverage, not a mention
#   recipe  the recipe under docs/agent/recipes that demonstrates it, or None
#   also    further repository paths that demonstrate it, searched for the proof
#
# A recipe and an example are not the same demonstration. A recipe is copied and
# is proven to build and run by CI; an example is read. Either counts here, since
# either gives the agent working code, but only the recipe is protected from rot.
#
# One row is demonstrated by neither. Debugging is shown by the repair bank in
# tools/agent/evals: each task is a symptom, the defect that causes it, and the
# diagnostic that must report it, and check_mutations.py runs every one of them in
# CI. That is a worked session per defect, and it rots no more quietly than a recipe.
# ---------------------------------------------------------------------------
FEATURES = [
    ('design',        'deciding what the services are',
     '05-design.md',            'boundary',            None,
     ()),
    ('new_project',   'starting a project',
     '10-new-project.md',       'addServiceInterface', '01-local-single-process',
     ()),
    ('interface',     'defining a service interface',
     '20-service-interface.md', '.siml',               '01-local-single-process',
     ('examples/13_locsvc',)),
    ('data_types',    'custom structures and enumerations',
     '21-data-types.md',        '.dtml',               None,
     ('examples', 'docs')),
    ('provider',      'implementing a service provider',
     '30-provider.md',          'request_',            '01-local-single-process',
     ('examples/03_helloservice',)),
    ('consumer',      'implementing a service consumer',
     '31-consumer.md',          'service_connected',   '01-local-single-process',
     ('examples/03_helloservice',)),
    ('model',         'registering components into threads',
     '32-model.md',             'BEGIN_MODEL',         '01-local-single-process',
     ('examples/03_helloservice',)),
    ('worker_thread', 'a component worker thread',
     '37-threads.md',           'REGISTER_WORKER_THREAD', None,
     ('examples/18_pubworker',)),
    ('timers',        'periodic and delayed work',
     '33-timers.md',            'start_timer',         '04-timer',
     ('examples/08_timer',)),
    ('custom_events', 'declaring and dispatching a custom event',
     '23-events.md',            'AREG_DECLARE_EVENT',  None,
     ('examples/18_pubworker',)),
    ('attributes',    'attributes and broadcasts',
     '20-service-interface.md', 'roadcast',            '03-attributes-and-broadcast',
     ('examples/25_pubsub',)),
    ('multi_service', 'more than one service in an application',
     None,                      'two services',        '05-two-services',
     ('examples/12_svcmulti',)),
    ('ipc',           'splitting an application across processes',
     None,                      'mtrouter',            '02-ipc-two-processes',
     ('examples/15_pubsvc',)),
    ('state_machine', 'adding a state machine',
     '22-state-machine.md',     '.fsml',               '06-state-machine',
     ('examples/19_pubfsm',)),
    ('fsm_guard',     'guarding a transition on a condition',
     '22-state-machine.md',     '<Guard',              '06-state-machine',
     ('examples/19_pubfsm',)),
    ('fsm_attribute', 'giving a state machine its own data',
     '22-state-machine.md',     'AttributeSet',        '06-state-machine',
     ('examples/19_pubfsm',)),
    ('logging',       'logging from application code',
     '34-logging.md',           'LOG_DBG',             None,
     ('examples/07_logging',)),
    ('log_collect',   'collecting logs from several processes',
     '34-logging.md',           'logcollector',        None,
     ('framework/logcollector',)),
    ('sqlog',         'reading and querying a log database',
     '35-sqlog.md',             'sqlog',               '08-observability',
     ('framework/logobserver',)),
    ('configuration', 'the runtime configuration file',
     '36-config.md',            'router::*::address',  None,
     ('framework/areg/resources/areg.init',)),
    ('watchdog',      'the thread watchdog',
     '37-threads.md',           'BEGIN_REGISTER_THREAD_EX', None,
     ('examples/21_locwatchdog',)),
    ('runtime_model', 'building a model at run time',
     '37-threads.md',           'add_model_unique',    None,
     ('examples/17_pubtraffic',)),
    ('base_api',      'strings and containers',
     '40-base-api.md',          'areg::String',        None,
     ('examples/05_buffer', 'examples/06_file')),
    ('runtime_api',   'the application, components, threads, time and files',
     '42-runtime-api.md',       'areg::SharedBuffer',  '04-timer',
     ('examples/05_buffer', 'examples/06_file')),
    ('testing',       'testing an application and its components',
     '52-testing.md',           'scripted provider',   '12-testing',
     ()),
    ('debugging',     'working out why it does not work',
     '51-debug.md',             'check_contract',      None,
     ('tools/agent/evals',)),
    ('examples',      'finding the example that shows a facility',
     '41-examples.md',          'examples/',           None,
     ()),
]

# Features no example can demonstrate: they are judgement, or an index of other
# pages. Their example weight goes to the page, which is the only thing that can
# carry them.
PROSE_ONLY = {'design', 'examples'}

# The seven gates that must run on every change. Each is a substring of the
# workflow that only that gate produces.
CI_GATES = [
    ('documented paths',   'check_agent_docs.py'),
    ('contract on recipes', 'check_contract.py'),
    ('recipes build and run', 'check_recipes.py'),
    ('project setup',      'setup_project.py'),
    ('eval self-check',    'run_evals.py'),
    ('token budget',       'ai_score.py'),
    ('mutations',          'check_mutations.py'),
    ('observability',      'check_observability.py'),
    ('non-Linux runner',   'windows-'),
]

# Tools AGENTS.md tells an agent to run. A named tool that is absent is a dead
# instruction, and an agent follows it before it discovers that.
TOOLS = ['setup_project.py', 'gen_skeleton.py', 'fsml_layout.py', 'run_scenarios.py',
         'check_agent_docs.py', 'check_contract.py', 'explain_rule.py', 'run_evals.py',
         'check_recipes.py', 'check-env.sh', 'check-env.bat', 'codegenerate.sh',
         'codegenerate.bat', 'setup-project.sh', 'setup-project.bat']

PAGE_CEILING = 8 * KB
PAGE_MEDIAN_TARGET = 6 * KB
ENTRY_TARGET = 10 * KB
ENTRY_ZERO = 20 * KB
RULE_SUMMARY_TARGET = 300


def read(*parts):
    """Text of a repository file, or the empty string when it is not there."""
    path = os.path.join(ROOT, *parts)
    if not os.path.isfile(path):
        return ''
    with open(path, encoding='utf-8', errors='replace') as handle:
        return handle.read()


def size(*parts):
    path = os.path.join(ROOT, *parts)
    return os.path.getsize(path) if os.path.isfile(path) else 0


def agent_pages():
    """The agent corpus, in file order: what an agent may be sent to read."""
    if not os.path.isdir(AGENT_DIR):
        return []
    return sorted(f for f in os.listdir(AGENT_DIR) if f.endswith('.md'))


def recipe_names():
    if not os.path.isdir(RECIPE_DIR):
        return []
    return sorted(d for d in os.listdir(RECIPE_DIR)
                  if os.path.isdir(os.path.join(RECIPE_DIR, d)))


def recipe_text(name):
    """Every text file of a recipe, concatenated. Used to prove a feature is shown."""
    root = os.path.join(RECIPE_DIR, name)
    if not os.path.isdir(root):
        return ''
    chunks = []
    for here, _dirs, files in os.walk(root):
        for f in sorted(files):
            if f.endswith(('.md', '.cpp', '.hpp', '.siml', '.dtml', '.fsml',
                           '.txt', '.json', '.init')):
                with open(os.path.join(here, f), encoding='utf-8',
                          errors='replace') as handle:
                    chunks.append(handle.read())
    return '\n'.join(chunks)


def corpus():
    """AGENTS.md, CODEBASE.md and every agent page, as one lowercased string."""
    text = read('AGENTS.md') + read('CODEBASE.md')
    for page in agent_pages():
        text += read('docs', 'agent', page)
    return text.lower()


def eval_tasks():
    raw = read('tools', 'agent', 'evals', 'tasks.json')
    if not raw:
        return []
    try:
        data = json.loads(raw)
    except ValueError:
        return []
    return data.get('tasks', data) if isinstance(data, dict) else data


def repair_tasks():
    """Tasks that hand the agent a broken application instead of a blank one.

    An agent that can only build from nothing is not ready: most of the work is
    repair. A task counts when it names a defect to find rather than a thing to
    build.
    """
    marks = ('repair', 'broken', 'fix ', 'diagnose', 'does not connect', 'defect')
    found = []
    for task in eval_tasks():
        blob = json.dumps(task).lower()
        if any(m in blob for m in marks) or str(task.get('kind', '')) == 'repair':
            found.append(task.get('id', '?'))
    return found


def scale(value, best, worst):
    """1.0 at best or beyond, 0.0 at worst or beyond, linear between."""
    if best == worst:
        return 1.0 if value == best else 0.0
    span = (worst - value) / float(worst - best)
    return max(0.0, min(1.0, span))


TEXTUAL = ('.md', '.cpp', '.hpp', '.siml', '.dtml', '.fsml', '.txt', '.json',
           '.init', '.cmake', '.py', '.xml')


def tree_has(relative, proof, budget=600):
    """True when a repository path demonstrates the thing `proof` names.

    A proof that starts with a dot names a file extension and is answered by the
    existence of such a file: a format with no file in the tree is a format the
    agent has never seen written. Anything else is looked for in the text.
    """
    root = os.path.join(ROOT, relative)
    if not os.path.exists(root):
        return False
    if os.path.isfile(root):
        if proof.startswith('.'):
            return root.lower().endswith(proof.lower())
        with open(root, encoding='utf-8', errors='replace') as handle:
            return proof.lower() in handle.read().lower()

    seen = 0
    for here, dirs, files in os.walk(root):
        dirs[:] = [d for d in dirs if d not in ('.git', 'build', 'out')]
        for name in sorted(files):
            if proof.startswith('.'):
                if name.lower().endswith(proof.lower()):
                    return True
                continue
            if not name.lower().endswith(TEXTUAL):
                continue
            seen += 1
            if seen > budget:
                return False
            with open(os.path.join(here, name), encoding='utf-8',
                      errors='replace') as handle:
                if proof.lower() in handle.read().lower():
                    return True
    return False


# ---------------------------------------------------------------------------
# The feature catalogue, resolved once: every component that needs it reads
# the same answer.
# ---------------------------------------------------------------------------
def resolve_features():
    whole = corpus()
    graded_refs = set()
    task_blob = ''
    for task in eval_tasks():
        ref = task.get('reference')
        if ref:
            graded_refs.add(ref)
        task_blob += json.dumps(task).lower()

    resolved = []
    for key, label, page, proof, recipe, also in FEATURES:
        if page:
            text = read('docs', 'agent', page).lower()
            documented = bool(text) and proof.lower() in text
            where = 'docs/agent/' + page
        else:
            documented = proof.lower() in whole
            where = 'the corpus'

        places = []
        if recipe:
            places.append('docs/agent/recipes/' + recipe)
        places += list(also)
        shown = any(tree_has(p, proof) for p in places)

        # A task grades a feature when it is the recipe's task, or when the task
        # names the feature or the thing that proves it. A task needs no reference
        # implementation to be a task.
        graded = bool(recipe) and recipe in graded_refs
        if not graded:
            graded = (key.replace('_', '-') in task_blob
                      or key.replace('_', ' ') in task_blob
                      or proof.lower() in task_blob)

        resolved.append({'key': key, 'label': label, 'page': where,
                         'documented': documented, 'example': recipe,
                         'shown': shown, 'graded': graded,
                         'prose_only': key in PROSE_ONLY})
    return resolved


# ---------------------------------------------------------------------------
# AI Friendliness
# ---------------------------------------------------------------------------
def f1_feature_coverage(features):
    notes = []
    earned = 0.0
    for f in features:
        if f['prose_only']:
            part = 0.8 * f['documented'] + 0.2 * f['graded']
        else:
            part = 0.6 * f['documented'] + 0.2 * f['shown'] + 0.2 * f['graded']
        earned += part
        if part < 1.0:
            gaps = []
            if not f['documented']:
                gaps.append('no page answers it')
            if not f['shown'] and not f['prose_only']:
                gaps.append('no example shows it')
            if not f['graded']:
                gaps.append('no eval grades it')
            notes.append('{:<14} {} -- {}'.format(f['key'], f['label'],
                                                  ', '.join(gaps)))
    return 30.0 * earned / len(features), 30.0, notes


def claims():
    """Self-claims the documentation makes about this repository.

    Each is checked literally against the tree. A claim that has quietly stopped
    being true is worse than an absent one: it is believed.
    """
    out = []
    agents = read('AGENTS.md')
    fsm = read('docs', 'agent', '22-state-machine.md')

    legacy = []
    for base in ('framework', 'examples'):
        for here, _dirs, files in os.walk(os.path.join(ROOT, base)):
            for f in files:
                if re.match(r'^(NE|TE)[A-Z]', f):
                    legacy.append(os.path.relpath(os.path.join(here, f), ROOT))
    out.append(('no file is named NE* or TE*', not legacy,
                'found ' + ', '.join(legacy[:3]) if legacy else ''))

    router = read('framework', 'mtrouter', 'app', 'MTRouterNames.hpp')
    out.append(('the one surviving NE* namespace is where check_contract.py says',
                'NEMultitargetRouterSettings' in router, ''))

    count = len([d for d in os.listdir(os.path.join(ROOT, 'examples'))
                 if re.match(r'^\d\d_', d)]) if os.path.isdir(
                     os.path.join(ROOT, 'examples')) else 0
    claimed = re.search(r'(\d+)\s+complete applications', agents)
    out.append(('the example count in AGENTS.md is the real one',
                bool(claimed) and int(claimed.group(1)) == count,
                'AGENTS.md says {}, the tree has {}'.format(
                    claimed.group(1) if claimed else '?', count)))

    api = read('docs', 'agent', 'api.json')
    out.append(('api.json states the C++ floor AGENTS.md states',
                '"C++17"' in api and 'C++17' in agents, ''))

    out.append(('the FSM event enumerator is documented with its EVENT_ prefix',
                'EVENT_' in fsm, 'the generator emits EVENT_<Name>'))
    out.append(('no page promises an unprefixed FSM event enumerator',
                not re.search(r'FsmEventValue::(?!EVENT_)[A-Z]', fsm), ''))
    out.append(('the FSM timer enumerator carries no prefix, and the page says so',
                'FsmTimer' in fsm and 'prefix' in fsm.lower(), ''))
    out.append(('the cheat sheet documents ConstantList',
                'ConstantList' in read('docs', 'agent', '00-cheatsheet.md'), ''))
    out.append(('the model page says BEGIN_REGISTER_COMPONENT constructs the component',
                'BEGIN_REGISTER_COMPONENT' in read('docs', 'agent', '32-model.md'), ''))
    out.append(('every recipe named by an eval task exists',
                all(os.path.isdir(os.path.join(RECIPE_DIR, t['reference']))
                    for t in eval_tasks() if t.get('reference')), ''))
    return out


def f2_documentation_truth():
    checked = claims()
    ok = [c for c in checked if c[1]]
    notes = ['{} -- {}'.format(c[0], c[2] or 'does not hold')
             for c in checked if not c[1]]

    docs_ok = subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'agent',
                                                           'check_agent_docs.py')],
                             cwd=ROOT, stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT).returncode == 0
    if not docs_ok:
        notes.append('check_agent_docs.py reports an unresolved path')

    # A claim that is present is not a claim that works. These are the documented
    # instructions that can be executed rather than read, and whether a checker in
    # CI executes each one. The list is a floor: an instruction nobody can run yet
    # belongs here too, scoring zero, so the gap stays visible.
    # Only the workflow that guards the agent documentation counts. The SDK's own
    # build matrix compiles on Windows and proves nothing about these pages.
    flow = ''
    wf = os.path.join(ROOT, '.github', 'workflows')
    if os.path.isdir(wf):
        for f in sorted(os.listdir(wf)):
            if 'agent' in f:
                flow += read('.github', 'workflows', f)
    executed = [
        ('the golden path builds a scaffolded project',
         'setup_project.py' in flow and 'citest/build' in flow),
        ('the cheat sheet configuration block logs what it promises',
         'check_doc_config.py' in flow),
        ('the recipes compile and print what they promise',
         'check_recipes.py' in flow),
        ('the router, collector and .sqlog path runs',
         'check_observability.py' in flow),
        ('the diagnostics the pages promise still fire',
         'check_mutations.py' in flow),
        ('a recipe is compiled by the Windows toolchain',
         bool(re.search(r'(cl\.exe|vcvars|msvc)', flow, re.I))),
    ]
    unrun = [name for name, done in executed if not done]
    notes += ['no check executes it: {}'.format(name) for name in unrun]

    earned = (10.0 * len(ok) / len(checked)
              + (5.0 if docs_ok else 0.0)
              + 10.0 * (len(executed) - len(unrun)) / len(executed))
    return earned, 25.0, notes


def f3_verification():
    notes = []
    recipes = recipe_names()
    checker = (read('tools', 'agent', 'check_recipes.py')
               + read('tools', 'agent', 'check_observability.py'))
    # A recipe is run-verified when something asserts its output: a table inside
    # check_recipes.py, a checker of its own, or a scenario file beside it. Built
    # but never run is not verification: the recipes that mattered most
    # historically failed at run time.
    verified = [r for r in recipes
                if ("'" + r + "'") in checker
                or os.path.isfile(os.path.join(RECIPE_DIR, r, 'scenarios.json'))]
    for r in recipes:
        if r not in verified:
            notes.append('{} is built but its output is never asserted'.format(r))

    tasks = eval_tasks()
    referenced = [t for t in tasks if t.get('reference')]
    for t in tasks:
        if not t.get('reference'):
            notes.append('eval task {} has no reference implementation, so the '
                         'harness cannot prove the task is solvable'.format(
                             t.get('id', '?')))

    flow = read('.github', 'workflows', 'agent-docs.yml')
    built = 'check_recipes.py' in flow
    if not built:
        notes.append('CI does not build and run the recipes')

    earned = (10.0 * len(verified) / max(1, len(recipes))
              + 8.0 * len(referenced) / max(1, len(tasks))
              + (7.0 if built else 0.0))
    return earned, 25.0, notes


def f4_prohibitions():
    raw = read('docs', 'agent', 'api.json')
    checker = read('tools', 'agent', 'check_contract.py')
    if not raw:
        return 0.0, 10.0, ['docs/agent/api.json is missing']
    data = json.loads(raw)
    rules = data.get('prohibitions', []) + data.get('base_api_notes', [])
    missing = [r['id'] for r in rules if r['id'] not in checker]
    notes = ['{} -- stated but not detected'.format(m) for m in missing]

    # Implemented is not the same as working. A rule is proven only when a repair
    # task breaks something on purpose and tools/agent/check_mutations.py watches that
    # rule report it. Without this half, the component asks only whether every rule
    # somebody wrote down exists -- which it always does, since the same hand wrote
    # both lists.
    proven = set()
    for task in eval_tasks():
        detect = task.get('detect') or {}
        if detect.get('by') == 'check_contract' and detect.get('rule'):
            proven.add(detect['rule'])
    unproven = [r['id'] for r in rules
                if r['id'] not in missing and r['id'] not in proven]
    notes += ['{} -- detected, but no repair task proves it still fires'.format(u)
              for u in unproven]

    detected = 5.0 * (len(rules) - len(missing)) / max(1, len(rules))
    verified = 5.0 * len(proven & {r['id'] for r in rules}) / max(1, len(rules))
    return detected + verified, 10.0, notes


def f5_self_claims():
    agents = read('AGENTS.md')
    pages = agent_pages()
    unreachable = [p for p in pages if p not in agents]
    notes = ['docs/agent/{} is not named in the AGENTS.md task table'.format(p)
             for p in unreachable]
    reach = 5.0 * (len(pages) - len(unreachable)) / max(1, len(pages))

    result = subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'agent',
                                                          'check_agent_docs.py')],
                            cwd=ROOT, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT)
    if result.returncode != 0:
        notes.append('check_agent_docs.py exits non-zero')
    return reach + (5.0 if result.returncode == 0 else 0.0), 10.0, notes


# ---------------------------------------------------------------------------
# Token Optimization
# ---------------------------------------------------------------------------
def t1_entry_toll():
    entry = size('AGENTS.md')
    notes = []
    if entry > ENTRY_TARGET:
        notes.append('AGENTS.md is {:.1f} KB, {:.1f} KB over the {:.0f} KB target '
                     'every task pays'.format(entry / KB, (entry - ENTRY_TARGET) / KB,
                                              ENTRY_TARGET / KB))
    return 25.0 * scale(entry, ENTRY_TARGET, ENTRY_ZERO), 25.0, notes


def page_budgets():
    """Recorded exceptions to the page ceiling: {page: allowed bytes}.

    A page is normally split when it passes the ceiling. docs/agent/.budgets names
    the few that are not, and why. The exception is recorded so it is argued rather
    than forgotten, and it is worth less than compliance so the pressure stays.
    """
    allowed = {}
    for line in read('docs', 'agent', '.budgets').splitlines():
        line = line.split('#')[0].strip()
        if not line or '=' not in line:
            continue
        page, _, limit = line.partition('=')
        try:
            allowed[page.strip()] = int(limit.strip())
        except ValueError:
            continue
    return allowed


def t2_page_budget():
    pages = agent_pages()
    if not pages:
        return 0.0, 35.0, ['docs/agent holds no pages']
    sizes = sorted(size('docs', 'agent', p) for p in pages)
    allowed = page_budgets()

    over = [(p, size('docs', 'agent', p)) for p in pages
            if size('docs', 'agent', p) > PAGE_CEILING]
    excepted = [(p, n) for p, n in over if n <= allowed.get(p, 0)]
    unexcused = [(p, n) for p, n in over if n > allowed.get(p, 0)]

    notes = ['docs/agent/{} is {:.1f} KB, over the {:.0f} KB ceiling'.format(
        p, n / KB, PAGE_CEILING / KB) for p, n in sorted(unexcused, key=lambda x: -x[1])]
    notes += ['docs/agent/{} is {:.1f} KB, over the ceiling by recorded exception'
              .format(p, n / KB) for p, n in sorted(excepted, key=lambda x: -x[1])]

    # An exception that names nothing, or excuses a page that needs no excuse, is a
    # stale entry: it would quietly cover a page that grew into it later.
    stale = [p for p in allowed
             if p not in pages or size('docs', 'agent', p) <= PAGE_CEILING]
    notes += ['docs/agent/.budgets excuses {}, which needs no exception'.format(p)
              for p in sorted(stale)]

    # A page sitting a handful of bytes under the ceiling is tuned to the metric, not
    # written to it: the next one-word edit trips CI. Name them so the shape is visible.
    brink = [(p, size('docs', 'agent', p)) for p in pages
             if PAGE_CEILING * 0.99 < size('docs', 'agent', p) <= PAGE_CEILING]
    notes += ['docs/agent/{} is {} bytes under the ceiling, too close to hold'
              .format(p, int(PAGE_CEILING - n)) for p, n in sorted(brink, key=lambda x: -x[1])]

    median = sizes[len(sizes) // 2]
    if median > PAGE_MEDIAN_TARGET:
        notes.append('median page is {:.1f} KB, over the {:.0f} KB target'.format(
            median / KB, PAGE_MEDIAN_TARGET / KB))

    compliant = len(pages) - len(over)
    within = 20.0 * (compliant + 0.7 * len(excepted) - len(stale)) / len(pages)
    central = 15.0 * scale(median, PAGE_MEDIAN_TARGET, PAGE_CEILING * 1.5)
    return max(0.0, within) + central, 35.0, notes


def t3_off_path(features):
    absent = [f for f in features if not f['documented']]
    notes = ['{} -- an agent must read a schema, an example or the framework '
             'source to learn it'.format(f['key']) for f in absent]
    return 20.0 * (len(features) - len(absent)) / len(features), 20.0, notes


def t4_duplication():
    pages = agent_pages()
    seen = {}
    for page in pages:
        words = re.findall(r'[a-z0-9_]+', read('docs', 'agent', page).lower())
        for i in range(max(0, len(words) - 11)):
            key = ' '.join(words[i:i + 12])
            seen.setdefault(key, set()).add(page)
    if not seen:
        return 10.0, 10.0, []
    repeated = [k for k, v in seen.items() if len(v) >= 3]
    ratio = len(repeated) / float(len(seen))
    notes = []
    if ratio > 0.02:
        notes.append('{:.1f}% of twelve word runs appear on three or more pages'
                     .format(100.0 * ratio))
    return 10.0 * scale(ratio, 0.02, 0.10), 10.0, notes


def t5_generated_code():
    stated = 'Never edit a generated file' in read('AGENTS.md')
    shipped = []
    for name in recipe_names():
        for here, dirs, _files in os.walk(os.path.join(RECIPE_DIR, name)):
            for d in dirs:
                if d in ('generate', 'generated'):
                    shipped.append(os.path.relpath(os.path.join(here, d), ROOT))
    notes = []
    if not stated:
        notes.append('AGENTS.md does not state the rule against editing generated files')
    notes += ['{} ships generated code an agent will read'.format(s) for s in shipped]
    return (5.0 if stated else 0.0) + (5.0 if not shipped else 0.0), 10.0, notes


# ---------------------------------------------------------------------------
# AI Readiness
# ---------------------------------------------------------------------------
def r1_eval_coverage(features):
    ungraded = [f['key'] for f in features if not f['graded']]
    notes = ['no eval task grades {}'.format(k) for k in ungraded]
    build = 12.0 * (len(features) - len(ungraded)) / len(features)

    repairs = repair_tasks()
    if not repairs:
        notes.append('the eval bank has no repair task: every task builds from '
                     'nothing, which is not the work')
    repair = 13.0 * min(1.0, len(repairs) / 6.0)
    return build + repair, 25.0, notes


def gating_workflow_text():
    """The workflow text that can actually fail a run, and the text that cannot.

    A step under 'continue-on-error: true', or any step of a job carrying it, reports
    and never gates. Concatenating the files and searching for a tool name scores the
    two the same, so a checker can be wired in and still never stop anything. The
    parse is by indentation rather than by a YAML library, because the score must be
    computable on a runner with nothing installed.

    Returns (gating, reporting): the two bodies of text.
    """
    gating, reporting = [], []
    wf = os.path.join(ROOT, '.github', 'workflows')
    if not os.path.isdir(wf):
        return '', ''

    for name in sorted(os.listdir(wf)):
        job = []                # every line of the job, header and steps alike
        marks = []              # (index into job, gates) for each step
        job_gates = True
        in_steps = False
        step_start = None
        step_gates = True

        def close_job():
            if not job:
                return
            bounds = marks + [(len(job), None)]
            head = job[:bounds[0][0]] if marks else job
            (gating if job_gates else reporting).append(''.join(head))
            for index, (begin, gates) in enumerate(marks):
                body = ''.join(job[begin:bounds[index + 1][0]])
                (gating if (job_gates and gates) else reporting).append(body)

        for line in read('.github', 'workflows', name).splitlines(True):
            stripped = line.strip()
            indent = len(line) - len(line.lstrip(' '))
            if stripped.startswith('#'):
                # A comment names the tool it explains and runs nothing. Counting one
                # as a gate is how a step that only reports scores as a step that fails.
                continue
            starts_job = (line.rstrip() and indent == 2 and stripped.endswith(':')
                          and not stripped.startswith('-'))
            if starts_job:
                if in_steps and step_start is not None:
                    marks.append((step_start, step_gates))
                close_job()
                job, marks = [line], []
                job_gates, in_steps, step_gates, step_start = True, False, True, None
                continue
            job.append(line)
            if not in_steps:
                if stripped.startswith('continue-on-error:') and 'true' in stripped:
                    job_gates = False
                if stripped == 'steps:':
                    in_steps = True
                continue
            if stripped.startswith('- '):
                if step_start is not None:
                    marks.append((step_start, step_gates))
                step_start, step_gates = len(job) - 1, True
                continue
            if stripped.startswith('continue-on-error:') and 'true' in stripped:
                step_gates = False
        if in_steps and step_start is not None:
            marks.append((step_start, step_gates))
        close_job()

    return ''.join(gating), ''.join(reporting)


def r2_ci_enforcement():
    gating, reporting = gating_workflow_text()
    missing, ungated = [], []
    for name, mark in CI_GATES:
        if mark in gating:
            continue
        (ungated if mark in reporting else missing).append(name)

    notes = ['no CI gate for {}'.format(m) for m in missing]
    notes += ['{} runs under continue-on-error, so it reports and never gates'
              .format(m) for m in ungated]
    # A gate that cannot fail is worth something -- the evidence is produced and
    # readable -- but not what a gate is worth.
    earned = (len(CI_GATES) - len(missing) - len(ungated)) + 0.4 * len(ungated)
    return 25.0 * earned / len(CI_GATES), 25.0, notes


def r3_tools():
    missing = [t for t in TOOLS
               if not os.path.isfile(os.path.join(ROOT, 'tools', t))
               and not os.path.isfile(os.path.join(ROOT, 'tools', 'agent', t))]
    notes = ['tools/{} is named by AGENTS.md but absent'.format(m) for m in missing]
    present = 10.0 * (len(TOOLS) - len(missing)) / len(TOOLS)

    # What explain_rule.py actually prints, not what one of its inputs holds: the
    # answer is the registry's summary plus the corrective action the tool carries.
    sys.path.insert(0, os.path.join(ROOT, 'tools'))
    try:
        import explain_rule
        rules = explain_rule.load()
    except Exception:
        notes.append('explain_rule.py cannot read the rule registry, so it answers '
                     'nothing')
        return present, 20.0, notes

    unanswered = [r['name'] for r in rules if not r['fix']]
    if unanswered:
        notes.append('{} of {} rules say what is wrong and not what to change: {}'
                     .format(len(unanswered), len(rules),
                             ', '.join(unanswered[:3])))
    lengths = [len(r['summary']) + len(r['fix']) for r in rules]
    mean = sum(lengths) / float(len(lengths)) if lengths else 0
    if mean < RULE_SUMMARY_TARGET:
        notes.append('rule answers average {:.0f} characters against a target of {}'
                     .format(mean, RULE_SUMMARY_TARGET))

    answered = 5.0 * (len(rules) - len(unanswered)) / max(1, len(rules))
    depth = 5.0 * scale(RULE_SUMMARY_TARGET - mean, 0, RULE_SUMMARY_TARGET)
    return present + answered + depth, 20.0, notes


def r4_observability():
    """The loop the SDK exists to support: run it, collect the logs, read them.

    An agent cannot invent this loop. It is three programs and a database, and
    nothing in the repository currently walks an agent through all three, nor
    proves the walk still works.
    """
    # What a script does, not what a document says. A recipe that describes the
    # collector proves nothing; a checker that starts one and reads the database
    # back is the only evidence this path still works.
    # The reader the observability check runs is the recipe's own query_sqlog.py, so
    # it counts as part of what CI executes. Scoring only the tools directory would
    # ask the checker to keep a second copy of the query.
    scripts = (read('tools', 'agent', 'check_recipes.py') + read('tools', 'agent', 'run_scenarios.py')
               + read('tools', 'agent', 'check_observability.py')
               + read('docs', 'agent', 'recipes', '08-observability',
                      'query_sqlog.py'))
    flow = ''
    wf = os.path.join(ROOT, '.github', 'workflows')
    if os.path.isdir(wf):
        for f in sorted(os.listdir(wf)):
            flow += read('.github', 'workflows', f)

    routed = 'mtrouter' in scripts
    collected = 'logcollector' in scripts
    queried = 'sqlite3' in scripts and '.sqlog' in scripts
    enforced = 'check_observability.py' in flow

    notes = []
    if not routed:
        notes.append('no checker starts the router')
    if not collected:
        notes.append('no checker collects logs with logcollector, so the '
                     'multi-process debugging path is unproven')
    if not queried:
        notes.append('no checker queries a .sqlog database, so the page that '
                     'documents reading logs is never exercised')
    if not enforced:
        notes.append('the observability check does not run in CI, so it proves '
                     'the path worked once and not that it still does')
    return ((4.0 if routed else 0.0) + (4.0 if collected else 0.0)
            + (4.0 if queried else 0.0) + (3.0 if enforced else 0.0)), 15.0, notes


def r5_portability():
    attrs = read('.gitattributes')
    normalised = all(re.search(re.escape(pat) + r'\s+text\s+eol=lf', attrs)
                     for pat in ('*.sh', '*.py'))
    notes = []
    if not normalised:
        notes.append('.gitattributes does not force LF on *.sh and *.py, so a '
                     'Windows checkout produces scripts that will not run')

    posix = re.compile(r'python3 |\.elf|\bcodegenerate\.sh\b')
    windows = re.compile(r'\.exe|\.bat|Windows|python tools')
    offenders = []
    for page in ['AGENTS.md'] + ['docs/agent/' + p for p in agent_pages()]:
        text = read(*page.split('/'))
        if posix.search(text) and not windows.search(text):
            offenders.append(page)
    notes += ['{} gives a POSIX command with no Windows form'.format(o)
              for o in offenders]
    pages = 1 + len(agent_pages())
    commands = 5.0 * (pages - len(offenders)) / pages

    flow = ''
    wf = os.path.join(ROOT, '.github', 'workflows')
    if os.path.isdir(wf):
        for f in sorted(os.listdir(wf)):
            if 'agent' in f:
                flow += read('.github', 'workflows', f)
    leg = 'windows-' in flow
    if not leg:
        notes.append('the agent workflow has no Windows runner, so half the '
                     'documented commands are never executed')
    return (5.0 if normalised else 0.0) + commands + (5.0 if leg else 0.0), 15.0, notes


# ---------------------------------------------------------------------------
def measure():
    features = resolve_features()
    groups = [
        ('AI Friendliness', [
            ('F1', 'feature coverage', f1_feature_coverage(features)),
            ('F2', 'documentation truth', f2_documentation_truth()),
            ('F3', 'verification coverage', f3_verification()),
            ('F4', 'prohibition detectability', f4_prohibitions()),
            ('F5', 'self-claim integrity', f5_self_claims()),
        ]),
        ('Token Optimization', [
            ('T1', 'entry toll', t1_entry_toll()),
            ('T2', 'page budget', t2_page_budget()),
            ('T3', 'forced off-path reads', t3_off_path(features)),
            ('T4', 'duplication', t4_duplication()),
            ('T5', 'generated-code avoidance', t5_generated_code()),
        ]),
        ('AI Readiness', [
            ('R1', 'eval coverage', r1_eval_coverage(features)),
            ('R2', 'CI enforcement', r2_ci_enforcement()),
            ('R3', 'tool completeness', r3_tools()),
            ('R4', 'observability workflow', r4_observability()),
            ('R5', 'portability', r5_portability()),
        ]),
    ]
    return groups


def main():
    parser = argparse.ArgumentParser(
        description='Score this repository as a place an AI agent builds from.')
    parser.add_argument('--verbose', action='store_true',
                        help='every component, and what cost it marks')
    parser.add_argument('--json', action='store_true', help='machine readable')
    parser.add_argument('--floor',
                        help='exit 1 below this: one number for all three scores, or '
                             'three separated by commas, in the printed order')
    args = parser.parse_args()

    groups = measure()
    report = {}
    for title, components in groups:
        total = sum(c[2][0] for c in components)
        report[title] = {
            'score': round(total, 1),
            'components': {c[0]: {'name': c[1], 'earned': round(c[2][0], 2),
                                  'weight': c[2][1], 'notes': c[2][2]}
                           for c in components},
        }

    if args.json:
        print(json.dumps(report, indent=2))
    else:
        for title, components in groups:
            total = sum(c[2][0] for c in components)
            print('{:<20} {:5.1f} / 100'.format(title, total))
            if args.verbose:
                for key, name, (earned, weight, notes) in components:
                    print('  {} {:<26} {:5.1f} / {:<4.0f}'.format(
                        key, name, earned, weight))
                    for note in notes:
                        print('       - ' + note)
                print('')

    if args.floor is not None:
        titles = [title for title, _ in groups]
        try:
            floors = [float(f) for f in args.floor.split(',')]
        except ValueError:
            print('--floor takes one number or three separated by commas')
            return 1
        if len(floors) == 1:
            floors *= len(titles)
        if len(floors) != len(titles):
            print('--floor takes one number or {} separated by commas'.format(
                len(titles)))
            return 1
        failed = 0
        for title, floor in zip(titles, floors):
            if report[title]['score'] < floor:
                print('{} is {}, below its floor of {}'.format(
                    title, report[title]['score'], floor))
                failed = 1
        return failed
    return 0


if __name__ == '__main__':
    sys.exit(main())
