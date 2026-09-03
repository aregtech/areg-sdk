#!/usr/bin/env python3
# ===========================================================================
# Checks the agent corpus: the pages, recipes, tools and evals an agent builds
# an application from.
#
#   python3 tools/agent/check_corpus.py             the findings
#   python3 tools/agent/check_corpus.py --verbose   every check, passing ones too
#   python3 tools/agent/check_corpus.py --json      the same, machine readable
#   python3 tools/agent/check_corpus.py --strict    a warning fails the run too
#
# Every check is a rule with a definite answer, and the run reports which rules
# hold. Nothing is weighted, nothing is interpolated, and no total is printed: a
# number summed out of these checks would need weights, and a weight is a
# judgement wearing an instrument's clothes. A moved total says something broke
# and not what; a named finding says what.
#
# The instrument is static. It never builds, never runs the generator and never
# calls a model, so it gives the same answer on any machine with no network.
# Where a property can only be shown by running something, what is checked is
# whether CI runs it, not whether it passes here: that is the property that
# keeps the corpus true after this week.
#
# Three severities:
#   FAIL  a rule that must hold does not. Exit code 1.
#   WARN  a measurement is past a declared target. Exit code 0, or 1 under
#         --strict.
#   NOTE  a recorded exception or an observation. Never fails, always printed,
#         so an argued exception stays visible instead of disappearing.
#
# What this cannot answer is whether an agent succeeds. That is tools/agent/
# run_evals.py against a real model, which is slow, costs money and is not
# deterministic, so it is never a gate. It is the only measure not graded by
# the hand that wrote the corpus.
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
FRAMEWORK = os.path.join(ROOT, 'framework')
KB = 1024.0


# ---------------------------------------------------------------------------
# The feature catalogue.
#
# One row per thing an agent is expected to be able to do, taken from the scope
# statement of AGENTS.md. Each row is asked three questions: does a page answer
# it, does an example show it, does an eval grade it.
#
#   page    the document that must answer it, or None to search the whole corpus
#   proof   a substring that must appear there; proves coverage, not a mention
#   recipe  the recipe under docs/agent/recipes that demonstrates it, or None
#   also    further repository paths that demonstrate it, searched for the proof
#
# A recipe and an example are not the same demonstration. A recipe is copied and
# is proven to build and run by CI; an example is read. Either counts here, since
# either gives the agent working code, but only the recipe is protected from rot.
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
# pages. Only a page can carry them, so no example is asked for.
PROSE_ONLY = {'design', 'examples'}

# The gates that must run on every change. Each is a substring of the workflow
# that only that gate produces.
CI_GATES = [
    ('documented paths',      'check_agent_docs.py'),
    ('contract on recipes',   'check_contract.py'),
    ('recipes build and run', 'check_recipes.py'),
    ('project setup',         'setup_project.py'),
    ('eval self-check',       'run_evals.py'),
    ('corpus check',          'check_corpus.py'),
    ('mutations',             'check_mutations.py'),
    ('observability',         'check_observability.py'),
    ('non-Linux runner',      'windows-'),
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
RULE_SUMMARY_TARGET = 300
DUPLICATION_TARGET = 0.02

FAIL, WARN, NOTE = 'FAIL', 'WARN', 'NOTE'


class Report(object):
    """The findings of one run, and the checks that produced none."""

    def __init__(self):
        self.findings = []
        self.passed = []

    def fail(self, check, message):
        self.findings.append((FAIL, check, message))

    def warn(self, check, message):
        self.findings.append((WARN, check, message))

    def note(self, check, message):
        self.findings.append((NOTE, check, message))

    def ok(self, check, message):
        self.passed.append((check, message))

    def count(self, severity):
        return len([f for f in self.findings if f[0] == severity])


# ---------------------------------------------------------------------------
# Reading the tree
# ---------------------------------------------------------------------------
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


def resolve_features():
    """The catalogue, answered once: every check reads the same answer."""
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
        # names the feature or the thing that proves it.
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
# Coverage: does the corpus answer, show and grade what it claims to teach
# ---------------------------------------------------------------------------
def check_coverage(report, features):
    for f in features:
        if not f['documented']:
            report.fail('coverage', '{}: no page answers "{}" -- an agent must '
                        'read a schema, an example or the framework source to '
                        'learn it'.format(f['key'], f['label']))
        if not f['shown'] and not f['prose_only']:
            report.warn('coverage', '{}: no recipe or example shows it'
                        .format(f['key']))
        if not f['graded']:
            report.warn('coverage', '{}: no eval task grades it'.format(f['key']))
    answered = len([f for f in features if f['documented']])
    report.ok('coverage', '{} of {} features have a page that answers them'
              .format(answered, len(features)))


# ---------------------------------------------------------------------------
# Truth: claims the documentation makes about this repository
# ---------------------------------------------------------------------------
def claims():
    """Self-claims the documentation makes, each checked literally against the tree.

    A claim that has quietly stopped being true is worse than an absent one: it
    is believed.
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


def agent_workflow():
    """The text of the workflow that guards the agent documentation.

    The SDK's own build matrix compiles on Windows and proves nothing about
    these pages, so only the agent workflow counts.
    """
    flow = ''
    folder = os.path.join(ROOT, '.github', 'workflows')
    if os.path.isdir(folder):
        for name in sorted(os.listdir(folder)):
            if 'agent' in name:
                flow += read('.github', 'workflows', name)
    return flow


def check_truth(report):
    checked = claims()
    for name, holds, detail in checked:
        if holds:
            continue
        report.fail('claim', '{} -- {}'.format(name, detail or 'does not hold'))
    report.ok('claim', '{} of {} self-claims hold'
              .format(len([c for c in checked if c[1]]), len(checked)))

    result = subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'agent',
                                                          'check_agent_docs.py')],
                            cwd=ROOT, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT)
    if result.returncode != 0:
        report.fail('paths', 'check_agent_docs.py reports an unresolved path')
    else:
        report.ok('paths', 'every path named in the corpus resolves')

    # A claim that is present is not a claim that works. These are the documented
    # instructions that can be executed rather than read, and whether a checker in
    # CI executes each one.
    flow = agent_workflow()
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
    for name, done in executed:
        if not done:
            report.warn('executed', 'no check in CI executes it: {}'.format(name))
    report.ok('executed', '{} of {} documented instructions are executed by CI'
              .format(len([e for e in executed if e[1]]), len(executed)))


# ---------------------------------------------------------------------------
# Verification: is the working code protected from rot
# ---------------------------------------------------------------------------
def check_verification(report):
    recipes = recipe_names()
    checker = (read('tools', 'agent', 'check_recipes.py')
               + read('tools', 'agent', 'check_observability.py'))
    # A recipe is run-verified when something asserts its output: a table inside
    # check_recipes.py, a checker of its own, or a scenario file beside it. Built
    # but never run is not verification.
    verified = [r for r in recipes
                if ("'" + r + "'") in checker
                or os.path.isfile(os.path.join(RECIPE_DIR, r, 'scenarios.json'))]
    for r in recipes:
        if r not in verified:
            report.fail('recipe', '{} is built but its output is never asserted'
                        .format(r))
    report.ok('recipe', '{} of {} recipes have their output asserted'
              .format(len(verified), len(recipes)))

    tasks = eval_tasks()
    for t in tasks:
        if not t.get('reference'):
            report.warn('eval', 'task {} has no reference implementation, so the '
                        'harness cannot prove the task is solvable'
                        .format(t.get('id', '?')))

    repairs = repair_tasks()
    if not repairs:
        report.fail('eval', 'the eval bank has no repair task: every task builds '
                    'from nothing, which is not the work')
    else:
        report.ok('eval', '{} of {} eval tasks are repairs'
                  .format(len(repairs), len(tasks)))

    if 'check_recipes.py' not in read('.github', 'workflows', 'agent-docs.yml'):
        report.fail('recipe', 'CI does not build and run the recipes')


# ---------------------------------------------------------------------------
# Prohibitions: stated, detected, and proven to still fire
# ---------------------------------------------------------------------------
def check_prohibitions(report):
    raw = read('docs', 'agent', 'api.json')
    if not raw:
        report.fail('prohibition', 'docs/agent/api.json is missing')
        return
    checker = read('tools', 'agent', 'check_contract.py')
    data = json.loads(raw)
    rules = data.get('prohibitions', []) + data.get('base_api_notes', [])

    missing = [r['id'] for r in rules if r['id'] not in checker]
    for rule in missing:
        report.fail('prohibition', '{} is stated but check_contract.py does not '
                    'detect it'.format(rule))

    # Implemented is not the same as working. A rule is proven only when a repair
    # task breaks something on purpose and check_mutations.py watches that rule
    # report it. Without this half the check asks only whether every rule somebody
    # wrote down exists, which it always does: the same hand wrote both lists.
    proven = set()
    for task in eval_tasks():
        detect = task.get('detect') or {}
        if detect.get('by') == 'check_contract' and detect.get('rule'):
            proven.add(detect['rule'])
    for rule in rules:
        if rule['id'] in missing or rule['id'] in proven:
            continue
        report.fail('prohibition', '{} is detected, but no repair task proves it '
                    'still fires'.format(rule['id']))
    # AGENTS.md states how many rules the checker reports. A rule added to
    # api.json and not to that sentence leaves the entry file understating itself.
    stated = data.get('prohibitions', [])
    words = ('zero one two three four five six seven eight nine ten eleven twelve '
             'thirteen fourteen fifteen sixteen seventeen eighteen nineteen twenty'
             ).split()
    count = len(stated)
    spelled = words[count] if count < len(words) else str(count)
    if 'all {}'.format(spelled) not in read('AGENTS.md'):
        report.fail('prohibition', 'AGENTS.md does not say check_contract.py reports '
                    'all {} rules api.json states'.format(spelled))
    else:
        report.ok('prohibition', 'AGENTS.md states the {} rules api.json carries'
                  .format(spelled))

    report.ok('prohibition', '{} of {} rules are stated, detected and proven'
              .format(len(proven & {r['id'] for r in rules}), len(rules)))


def check_reachable(report):
    agents = read('AGENTS.md')
    pages = agent_pages()
    for page in pages:
        if page not in agents:
            report.fail('reachable', 'docs/agent/{} is not named in the AGENTS.md '
                        'task table, so nothing routes an agent to it'.format(page))
    report.ok('reachable', '{} of {} pages are reachable from the task table'
              .format(len([p for p in pages if p in agents]), len(pages)))


# ---------------------------------------------------------------------------
# The includes a page shows against the names its code uses
#
# A page whose code names areg::Component and shows no include that reaches
# Component.hpp does not compile as written. The agent that follows it discovers
# that from the compiler, having already been told the page was the whole
# reading list.
# ---------------------------------------------------------------------------
FENCE_RE = re.compile(r'```(\w*)\n(.*?)```', re.DOTALL)
INCLUDE_RE = re.compile(r'#\s*include\s+[<"]([^>"]+)[>"]')
QUALIFIED_RE = re.compile(r'\bareg::([A-Za-z_][A-Za-z0-9_]*)')
MACRO_USE_RE = re.compile(r'\b((?:AREG|LOG|DEF|BEGIN|END|REGISTER)_[A-Z0-9_]{2,})\b')
DEFINE_RE = re.compile(r'^\s*#\s*define\s+([A-Za-z_][A-Za-z0-9_]*)')
DECL_RES = (
    re.compile(r'\b(?:class|struct|union|enum(?:\s+class)?)\s+'
               r'(?:[A-Z_]+_API\s+)?([A-Za-z_][A-Za-z0-9_]*)'),
    re.compile(r'\busing\s+([A-Za-z_][A-Za-z0-9_]*)\s*='),
    re.compile(r'\btypedef\b[^;]*?\b([A-Za-z_][A-Za-z0-9_]*)\s*;'),
    re.compile(r'\bconstexpr\b[^;=()]*?\b([A-Za-z_][A-Za-z0-9_]*)\s*(?:=|\{)'),
)

# Names a page may use without naming a header: they are spelled out by the page
# that owns them, or they belong to the generated code the recipe supplies.
INCLUDE_EXEMPT = {'String', 'CEString'}

# What a generated base header includes, so a page that includes one reaches
# these too. Taken from the generator's own output: a provider base derives from
# StubBase, a consumer base from the proxy and its listener. A change here breaks
# the recipes at compile time in check_recipes.py, so the list cannot rot quietly.
GENERATED_BASE_INCLUDES = {
    'ProviderBase.hpp': ('areg/base/areg_global.h',
                         'areg/component/StubBase.hpp'),
    'ConsumerBase.hpp': ('areg/base/areg_global.h',
                         'areg/component/ProxyListener.hpp',
                         'areg/component/NotificationEvent.hpp'),
}

# A fenced block that is a signature or a fragment rather than code to copy opts
# out with the same marker check_contract.py honours, on its first line.
OPT_OUT = '// areg-check: ignore'


def framework_index():
    """Maps every name framework/ declares to the headers that declare it, and
    every header to the framework headers it includes."""
    declares = {}
    includes = {}
    for folder, folders, files in os.walk(FRAMEWORK):
        folders[:] = [d for d in folders if d not in ('.git', 'build')]
        for entry in files:
            if not entry.endswith(('.hpp', '.hxx', '.hh', '.h')):
                continue
            path = os.path.join(folder, entry)
            key = os.path.relpath(path, FRAMEWORK).replace(os.sep, '/')
            try:
                with open(path, encoding='utf-8', errors='replace') as handle:
                    text = handle.read()
            except OSError:
                continue
            includes[key] = set(INCLUDE_RE.findall(text))
            for line in text.splitlines():
                found = DEFINE_RE.match(line)
                if found:
                    declares.setdefault(found.group(1), set()).add(key)
                    continue
                stripped = line.lstrip()
                if stripped.startswith(('*', '//', '/*')):
                    continue
                for pattern in DECL_RES:
                    hit = pattern.search(line)
                    if hit:
                        declares.setdefault(hit.group(1), set()).add(key)
    return declares, includes


def reachable_headers(shown, includes):
    """Every framework header reached from the includes a page shows."""
    seen = set()
    queue = [s for s in shown if s in includes]
    while queue:
        head = queue.pop()
        if head in seen:
            continue
        seen.add(head)
        queue.extend(n for n in includes.get(head, ()) if n in includes and n not in seen)
    return seen


def check_includes(report):
    declares, includes = framework_index()
    if not declares:
        report.fail('includes', 'no framework headers found under framework/')
        return

    checked = 0
    for page in agent_pages():
        text = read('docs', 'agent', page)
        blocks = [body for lang, body in FENCE_RE.findall(text)
                  if lang.lower() in ('cpp', 'c++', 'cc')
                  and OPT_OUT not in body.split('\n', 1)[0]]
        if not blocks:
            continue
        checked += 1
        code = '\n'.join(blocks)

        named = set(INCLUDE_RE.findall(text))
        for shown_include in sorted(named):
            for suffix, roots in GENERATED_BASE_INCLUDES.items():
                if shown_include.endswith(suffix):
                    named.update(roots)
        shown = reachable_headers(named, includes)

        used = set(QUALIFIED_RE.findall(code)) | set(MACRO_USE_RE.findall(code))
        wanted = {}
        for name in sorted(used):
            if name in INCLUDE_EXEMPT or name not in declares:
                continue
            homes = declares[name]
            if homes & shown:
                continue
            wanted.setdefault(sorted(homes)[0], []).append(name)

        if not wanted:
            report.ok('includes', '{}: every areg name its code uses is reachable '
                      'from an include it shows'.format(page))
            continue
        for header, names in sorted(wanted.items()):
            report.fail('includes', '{}: uses {} and shows no include that reaches '
                        '{}'.format(page, ', '.join(sorted(names)[:4]), header))
    report.ok('includes', '{} pages carry C++ and were checked'.format(checked))


# ---------------------------------------------------------------------------
# The generated members a page writes against the ones api.json declares
#
# check_symbols.py anchors every areg:: name to a framework header, and
# check_includes anchors the headers. Neither sees the members the generator
# emits: they carry no namespace and live in no header until a build runs. Those
# are the names an agent copies most and the ones a generator change renames, so
# they are held to the shapes api.json declares -- the same file check_contract.py
# holds application code to.
# ---------------------------------------------------------------------------
# The names api.json spells with a stand-in service, attribute or broadcast.
MEMBER_PLACEHOLDERS = ('foo', 'bar', 'baz')

# What a page writes that belongs to a generated service. Every other snake_case
# name on a page is framework API and is check_symbols.py's to answer for.
MEMBER_SHAPE_RE = re.compile(r'\b((?:request_|response_|broadcast_|notify_on_)'
                             r'[a-z0-9_]+|[a-z0-9_]+_update)\b')
TICK_RE = re.compile(r'`([^`\n]+)`')
DOC_PLACEHOLDER_RE = re.compile(r'<[^<>\n]+>')

# What '<attribute>' becomes while a name is matched, so a documented shape is read
# as a name. Spelled back out when the name is reported.
DOC_STANDIN = 'zplaceholderz'


def member_templates():
    """The member name shapes api.json declares, as regular expressions."""
    try:
        contract = json.loads(read('docs', 'agent', 'api.json'))
    except ValueError:
        return []
    names = []

    def collect(node):
        if isinstance(node, dict):
            for key, value in node.items():
                if key in ('member', 'read_accessor', 'requires_subscription'):
                    names.extend([value] if isinstance(value, str) else value)
                collect(value)
        elif isinstance(node, list):
            for value in node:
                collect(value)

    collect(contract.get('members', []))
    shapes = []
    for name in sorted(set(re.sub(r'\(.*', '', n).strip() for n in names)):
        if '<' in name:
            continue
        parts = name.split('_')
        # A name that is only a stand-in is a read accessor: it matches anything.
        if not any(p in MEMBER_PLACEHOLDERS for p in parts):
            continue
        if all(p in MEMBER_PLACEHOLDERS for p in parts):
            continue
        pattern = '_'.join('[a-z0-9]+(?:_[a-z0-9]+)*' if p in MEMBER_PLACEHOLDERS
                           else re.escape(p) for p in parts)
        shapes.append((name, re.compile('^' + pattern + '$')))
    return shapes


def check_member_shapes(report):
    shapes = member_templates()
    if not shapes:
        report.fail('members', 'api.json declares no member names to check against')
        return

    checked = 0
    for page in agent_pages():
        text = read('docs', 'agent', page)
        blocks = [body for lang, body in FENCE_RE.findall(text)
                  if lang.lower() in ('cpp', 'c++', 'cc')
                  and OPT_OUT not in body.split('\n', 1)[0]]
        # A member is named in prose as often as in code, and a wrong one there
        # is copied just as readily.
        source = ' '.join(blocks + TICK_RE.findall(text))
        source = DOC_PLACEHOLDER_RE.sub(DOC_STANDIN, source)
        wrong = sorted(name for name in set(MEMBER_SHAPE_RE.findall(source))
                       if not any(shape.match(name) for _, shape in shapes))
        if not source:
            continue
        checked += 1
        for name in wrong:
            report.fail('members', '{}: writes {}, which is no member shape '
                        'api.json declares'
                        .format(page, name.replace(DOC_STANDIN, '<...>')))
        if not wrong:
            report.ok('members', '{}: every generated member it names has a shape '
                      'api.json declares'.format(page))
    report.ok('members', '{} pages checked against {} member shapes'
              .format(checked, len(shapes)))


# ---------------------------------------------------------------------------
# Token cost
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# The file that tells an agent where the SDK is
#
# conf/cmake/setup.cmake writes build/areg-sdk.paths, and setup_project.py tells a
# created project to read it. A key renamed on one side and not the other leaves the
# agent reading for something that is not there, and nothing fails until then.
# ---------------------------------------------------------------------------
PATHS_FILE = 'areg-sdk.paths'
PATHS_KEYS = ('sdk_root', 'headers', 'agent_docs', 'agents_md', 'codegen',
              'schema')


def check_sdk_paths(report):
    producer = read('conf', 'cmake', 'setup.cmake')
    if PATHS_FILE not in producer:
        report.fail('sdk-paths', 'conf/cmake/setup.cmake writes no {}, so a project '
                    'that fetched the SDK cannot find it'.format(PATHS_FILE))
        return

    written = [key for key in PATHS_KEYS
               if re.search(r'^{}\s*='.format(key), producer, re.M)]
    for key in PATHS_KEYS:
        if key not in written:
            report.fail('sdk-paths', 'conf/cmake/setup.cmake does not write the {} '
                        'key'.format(key))

    told = read('tools', 'agent', 'setup_project.py')
    if PATHS_FILE not in told:
        report.fail('sdk-paths', 'setup_project.py does not tell a created project '
                    'to read {}'.format(PATHS_FILE))
        return
    for key in PATHS_KEYS:
        if key not in told:
            report.fail('sdk-paths', 'setup_project.py does not name the {} key a '
                        'project is told to read'.format(key))
    report.ok('sdk-paths', '{} carries {} keys, and setup_project.py names them all'
              .format(PATHS_FILE, len(written)))


# ---------------------------------------------------------------------------
# The revision a new project fetches
#
# A recipe copied by hand has to build with no Python and no lookup, so every
# recipe carries the git ref as a literal. Fourteen literals that nothing compares
# are fourteen chances to disagree, and they all have to change together on the day
# a release is tagged. api.json owns the value; this holds the copies to it.
# ---------------------------------------------------------------------------
GIT_TAG_RE = re.compile(r'GIT_TAG\s+"([^"]+)"')
FALLBACK_TAG_RE = re.compile(r"^FALLBACK_TAG\s*=\s*'([^']+)'", re.M)


def check_fetch_ref(report):
    try:
        contract = json.loads(read('docs', 'agent', 'api.json'))
    except ValueError:
        report.fail('fetch-ref', 'docs/agent/api.json cannot be parsed')
        return
    sdk = contract.get('sdk') or {}
    wanted = sdk.get('fetch_ref')
    if not wanted:
        report.fail('fetch-ref', 'api.json states no sdk.fetch_ref, so nothing owns '
                    'the revision a new project fetches')
        return

    carriers = ['docs/agent/recipes/{}/CMakeLists.txt'.format(n) for n in recipe_names()]
    carriers.append('docs/agent/10-new-project.md')
    found = 0
    for relative in carriers:
        text = read(*relative.split('/'))
        for tag in GIT_TAG_RE.findall(text):
            found += 1
            if tag != wanted:
                report.fail('fetch-ref', '{} fetches "{}"; api.json states sdk.'
                            'fetch_ref is "{}"'.format(relative, tag, wanted))

    setup = read('tools', 'agent', 'setup_project.py')
    default = FALLBACK_TAG_RE.search(setup)
    if not default:
        report.fail('fetch-ref', 'setup_project.py states no FALLBACK_TAG')
    elif default.group(1) != wanted:
        found += 1
        report.fail('fetch-ref', 'setup_project.py falls back to "{}"; api.json '
                    'states sdk.fetch_ref is "{}"'.format(default.group(1), wanted))
    else:
        found += 1

    if not found:
        report.fail('fetch-ref', 'no recipe or page names a revision to fetch')
        return
    # A pin to a release the pages do not describe compiles against nothing, so the
    # supported range is stated where the pin is written.
    if sdk.get('supported') and sdk['supported'] not in read('docs', 'agent',
                                                             '10-new-project.md'):
        report.fail('fetch-ref', '10-new-project.md does not state which versions '
                    'api.json supports ("{}")'.format(sdk['supported']))
    report.ok('fetch-ref', '{} literals fetch "{}", the ref api.json states'
              .format(found, wanted))


# ---------------------------------------------------------------------------
# The C++ spelling of a document data type
#
# An override signature needs the C++ type a .siml DataType becomes, and four of
# them are not named the way the document names them. tools/schema/datatype.xml is
# the generator's own answer; the page that states it has to keep saying what that
# file says, or an agent writes a signature that does not compile.
# ---------------------------------------------------------------------------
DATATYPE_PAGE = '21-data-types.md'


def check_data_types(report):
    import xml.etree.ElementTree as ElementTree
    path = os.path.join(ROOT, 'tools', 'schema', 'datatype.xml')
    try:
        root = ElementTree.parse(path).getroot()
    except (OSError, ElementTree.ParseError) as error:
        report.fail('data-types', 'tools/schema/datatype.xml cannot be read: {}'
                    .format(error))
        return

    page = read('docs', 'agent', DATATYPE_PAGE)
    if not page:
        report.fail('data-types', 'docs/agent/{} is missing'.format(DATATYPE_PAGE))
        return

    checked = 0
    missing = 0
    for entry in root.iter('DataType'):
        name = entry.get('Name')
        if not name or entry.get('Type') == 'Dummy' or ' ' in name:
            continue
        spelling = next((v.get('cpp') for v in entry.iter('Variant') if v.get('cpp')), None)
        if not spelling:
            continue
        checked += 1
        if name not in page:
            missing += 1
            report.fail('data-types', '{} does not name the document type "{}"'
                        .format(DATATYPE_PAGE, name))
        # A spelling equal to the document name needs no table row to be right.
        elif spelling != name and spelling not in page:
            missing += 1
            report.fail('data-types', '{} does not give "{}" its C++ spelling "{}"'
                        .format(DATATYPE_PAGE, name, spelling))
    if not checked:
        report.fail('data-types', 'datatype.xml declares no type with a C++ spelling')
        return
    if not missing:
        report.ok('data-types', '{} states the C++ spelling of all {} predefined '
                  'types'.format(DATATYPE_PAGE, checked))


# ---------------------------------------------------------------------------
# The numbers a document states against the thing that owns them
#
# A measure written into a page is a copy of a value some tool or file decides. The
# copy cannot be edited by whoever changes the value, so it is the first thing to go
# stale, and a page that misstates a threshold is read as the rule.
# ---------------------------------------------------------------------------
def rule_count():
    """The rules tools/schema/rules.xml declares."""
    import xml.etree.ElementTree as ElementTree
    try:
        root = ElementTree.parse(os.path.join(ROOT, 'tools', 'schema', 'rules.xml'))
    except (OSError, Exception):                # noqa: BLE001 - reported by the caller
        return None
    return len([n for n in root.getroot().iter('Rule')])


def stated_numbers():
    """Each documented measure, as (file, the sentence it must contain, source)."""
    claims = [
        ('docs/ai-readiness.md',
         '`AGENTS.md` at or below {:.0f} KB'.format(ENTRY_TARGET / KB),
         'ENTRY_TARGET in this file'),
        ('docs/ai-readiness.md',
         'Every page in `docs/agent/` at or below {:.0f} KB'.format(PAGE_CEILING / KB),
         'PAGE_CEILING in this file'),
        ('docs/ai-readiness.md',
         'Median page at or below {:.0f} KB'.format(PAGE_MEDIAN_TARGET / KB),
         'PAGE_MEDIAN_TARGET in this file'),
        ('docs/ai-readiness.md',
         'Under {:.0f}% of 12-word runs'.format(DUPLICATION_TARGET * 100),
         'DUPLICATION_TARGET in this file'),
    ]
    rules = rule_count()
    if rules:
        claims.append(('docs/CODEGEN_MESSAGE_FORMAT.md',
                       'registry of {} rules'.format(rules),
                       'tools/schema/rules.xml'))
    return claims


def check_stated_numbers(report):
    for relative, sentence, source in stated_numbers():
        text = read(*relative.split('/'))
        if not text:
            report.fail('stated', '{} is missing'.format(relative))
        elif sentence not in text:
            report.fail('stated', '{} does not say "{}"; {} is what decides it'
                        .format(relative, sentence, source))
    report.ok('stated', '{} documented measure(s) match the file that owns them'
              .format(len(stated_numbers())))


# ---------------------------------------------------------------------------
# What the installed package puts in front of a reader
#
# install.cmake ships tools/ minus the corpus and framework maintenance scripts. A
# tool the pages tell an agent to run and the install leaves out is a dead
# instruction; a maintenance tool that ships is one more script an agent has to rule
# out before it finds the seven that matter.
# ---------------------------------------------------------------------------
def check_shipped_tools(report):
    install = read('conf', 'cmake', 'install.cmake')
    if not install:
        report.fail('shipped', 'conf/cmake/install.cmake is missing')
        return
    named = read('AGENTS.md') + ''.join(read('docs', 'agent', p) for p in agent_pages())

    # Directories install.cmake drops whole; a file inside one needs no rule of its own.
    dropped = set(re.findall(r'PATTERN\s+"([^"]+)"\s+EXCLUDE', install))
    tools = []
    for folder, folders, files in os.walk(os.path.join(ROOT, 'tools')):
        folders[:] = [d for d in folders
                      if d not in dropped and d not in ('schema', '__pycache__')]
        tools.extend(f for f in files if f.endswith('.py'))

    wrong = 0
    for tool in sorted(set(tools)):
        stem = tool[:-3]
        excluded = '|{}|'.format(stem) in install.replace('(', '|').replace(')', '|')
        if tool in named and excluded:
            wrong += 1
            report.fail('shipped', 'the pages tell an agent to run {}, and '
                        'install.cmake excludes it'.format(tool))
        elif tool not in named and not excluded:
            wrong += 1
            report.fail('shipped', '{} is a maintenance tool the pages never name, '
                        'and install.cmake ships it'.format(tool))
    if not wrong:
        report.ok('shipped', '{} tools checked: the installed set is the set the '
                  'pages name'.format(len(set(tools))))


def check_entry_toll(report):
    entry = size('AGENTS.md')
    if entry > ENTRY_TARGET:
        report.warn('entry-toll', 'AGENTS.md is {:.1f} KB, {:.1f} KB over the '
                    '{:.0f} KB target every task pays'
                    .format(entry / KB, (entry - ENTRY_TARGET) / KB, ENTRY_TARGET / KB))
    else:
        report.ok('entry-toll', 'AGENTS.md is {:.1f} KB, within its {:.0f} KB target'
                  .format(entry / KB, ENTRY_TARGET / KB))


def page_budgets():
    """Recorded exceptions to the page ceiling: {page: allowed bytes}.

    A page is normally split when it passes the ceiling. docs/agent/.budgets
    names the few that are not, and why, so the exception is argued rather than
    forgotten.
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


def check_page_budget(report):
    pages = agent_pages()
    if not pages:
        report.fail('budget', 'docs/agent holds no pages')
        return
    allowed = page_budgets()
    sizes = sorted(size('docs', 'agent', p) for p in pages)

    over = [(p, size('docs', 'agent', p)) for p in pages
            if size('docs', 'agent', p) > PAGE_CEILING]
    for page, bytes_ in sorted(over, key=lambda x: -x[1]):
        if bytes_ <= allowed.get(page, 0):
            report.note('budget', 'docs/agent/{} is {:.1f} KB, over the ceiling by '
                        'recorded exception'.format(page, bytes_ / KB))
        else:
            report.fail('budget', 'docs/agent/{} is {:.1f} KB, over the {:.0f} KB '
                        'ceiling and not recorded in .budgets'
                        .format(page, bytes_ / KB, PAGE_CEILING / KB))

    # An exception that names nothing, or excuses a page that needs no excuse, is
    # a stale entry: it would quietly cover a page that grew into it later.
    for page in sorted(allowed):
        if page not in pages:
            report.fail('budget', '.budgets excuses {}, which does not exist'
                        .format(page))
        elif size('docs', 'agent', page) <= PAGE_CEILING:
            report.fail('budget', '.budgets excuses {}, which needs no exception'
                        .format(page))

    # A page a handful of bytes under the ceiling is tuned to the metric, not
    # written to it: the next one-word edit trips CI.
    for page in pages:
        bytes_ = size('docs', 'agent', page)
        if PAGE_CEILING * 0.99 < bytes_ <= PAGE_CEILING:
            report.note('budget', 'docs/agent/{} is {} bytes under the ceiling, too '
                        'close to hold'.format(page, int(PAGE_CEILING - bytes_)))

    median = sizes[len(sizes) // 2]
    if median > PAGE_MEDIAN_TARGET:
        report.warn('budget', 'median page is {:.1f} KB, over the {:.0f} KB target'
                    .format(median / KB, PAGE_MEDIAN_TARGET / KB))
    report.ok('budget', '{} of {} pages are within the {:.0f} KB ceiling'
              .format(len(pages) - len(over), len(pages), PAGE_CEILING / KB))


def check_duplication(report):
    pages = agent_pages()
    seen = {}
    for page in pages:
        words = re.findall(r'[a-z0-9_]+', read('docs', 'agent', page).lower())
        for i in range(max(0, len(words) - 11)):
            key = ' '.join(words[i:i + 12])
            seen.setdefault(key, set()).add(page)
    if not seen:
        return
    repeated = [k for k, v in seen.items() if len(v) >= 3]
    ratio = len(repeated) / float(len(seen))
    if ratio > DUPLICATION_TARGET:
        report.warn('duplication', '{:.1f}% of twelve word runs appear on three or '
                    'more pages, over the {:.0f}% target'
                    .format(100.0 * ratio, 100.0 * DUPLICATION_TARGET))
    else:
        report.ok('duplication', '{:.1f}% of twelve word runs are repeated across '
                  'three or more pages'.format(100.0 * ratio))


def check_generated_code(report):
    if 'Never edit a generated file' not in read('AGENTS.md'):
        report.fail('generated', 'AGENTS.md does not state the rule against editing '
                    'generated files')
    shipped = []
    for name in recipe_names():
        for here, dirs, _files in os.walk(os.path.join(RECIPE_DIR, name)):
            for d in dirs:
                if d in ('generate', 'generated'):
                    shipped.append(os.path.relpath(os.path.join(here, d), ROOT))
    for path in shipped:
        report.fail('generated', '{} ships generated code an agent will read'
                    .format(path))
    if not shipped:
        report.ok('generated', 'no recipe ships generated code')


# ---------------------------------------------------------------------------
# CI enforcement
# ---------------------------------------------------------------------------
def gating_workflow_text():
    """The workflow text that can actually fail a run, and the text that cannot.

    A step under 'continue-on-error: true', or any step of a job carrying it,
    reports and never gates. Concatenating the files and searching for a tool
    name treats the two the same, so a checker can be wired in and still stop
    nothing. The parse is by indentation rather than by a YAML library, because
    the check must run on a runner with nothing installed.

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
                # A comment names the tool it explains and runs nothing. Counting
                # one as a gate reads a reporting step as a failing one.
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


def check_ci(report):
    gating, reporting = gating_workflow_text()
    held = 0
    for name, mark in CI_GATES:
        if mark in gating:
            held += 1
        elif mark in reporting:
            report.fail('ci-gate', '{} runs under continue-on-error, so it reports '
                        'and never gates'.format(name))
        else:
            report.fail('ci-gate', 'no CI gate for {}'.format(name))
    report.ok('ci-gate', '{} of {} gates fail the run when they fail'
              .format(held, len(CI_GATES)))


# ---------------------------------------------------------------------------
# The tools an agent is told to run
# ---------------------------------------------------------------------------
def check_tools(report):
    missing = [t for t in TOOLS
               if not os.path.isfile(os.path.join(ROOT, 'tools', t))
               and not os.path.isfile(os.path.join(ROOT, 'tools', 'agent', t))]
    for tool in missing:
        report.fail('tool', 'tools/{} is named by AGENTS.md but is absent, so the '
                    'instruction that names it is dead'.format(tool))
    report.ok('tool', '{} of {} named tools are present'
              .format(len(TOOLS) - len(missing), len(TOOLS)))

    # What explain_rule.py actually prints, not what one of its inputs holds: the
    # answer is the registry's summary plus the corrective action the tool carries.
    sys.path.insert(0, os.path.join(ROOT, 'tools'))
    try:
        import explain_rule
        rules = explain_rule.load()
    except Exception:
        report.fail('tool', 'explain_rule.py cannot read the rule registry, so a '
                    'refused document explains nothing')
        return

    unanswered = [r['name'] for r in rules if not r['fix']]
    if unanswered:
        report.warn('tool', '{} of {} rules say what is wrong and not what to '
                    'change: {}'.format(len(unanswered), len(rules),
                                        ', '.join(unanswered[:3])))
    lengths = [len(r['summary']) + len(r['fix']) for r in rules]
    mean = sum(lengths) / float(len(lengths)) if lengths else 0
    if mean < RULE_SUMMARY_TARGET:
        report.warn('tool', 'rule answers average {:.0f} characters against a '
                    'target of {}'.format(mean, RULE_SUMMARY_TARGET))
    else:
        report.ok('tool', '{} rules answer with a mean of {:.0f} characters'
                  .format(len(rules), mean))


# ---------------------------------------------------------------------------
# The loop the SDK exists to support: run it, collect the logs, read them
# ---------------------------------------------------------------------------
def check_observability(report):
    # What a script does, not what a document says. A recipe that describes the
    # collector proves nothing; a checker that starts one and reads the database
    # back is the only evidence this path still works.
    scripts = (read('tools', 'agent', 'check_recipes.py')
               + read('tools', 'agent', 'run_scenarios.py')
               + read('tools', 'agent', 'check_observability.py')
               + read('docs', 'agent', 'recipes', '08-observability',
                      'query_sqlog.py'))
    flow = ''
    wf = os.path.join(ROOT, '.github', 'workflows')
    if os.path.isdir(wf):
        for f in sorted(os.listdir(wf)):
            flow += read('.github', 'workflows', f)

    for ok, message in (
            ('mtrouter' in scripts, 'no checker starts the router'),
            ('logcollector' in scripts,
             'no checker collects logs with logcollector, so the multi-process '
             'debugging path is unproven'),
            ('sqlite3' in scripts and '.sqlog' in scripts,
             'no checker queries a .sqlog database, so the page that documents '
             'reading logs is never exercised'),
            ('check_observability.py' in flow,
             'the observability check does not run in CI, so it proves the path '
             'worked once and not that it still does')):
        if not ok:
            report.fail('observability', message)
    report.ok('observability', 'router, collector and .sqlog query are exercised '
              'by a checker CI runs')


def check_portability(report):
    attrs = read('.gitattributes')
    normalised = all(re.search(re.escape(pat) + r'\s+text\s+eol=lf', attrs)
                     for pat in ('*.sh', '*.py'))
    if not normalised:
        report.fail('portability', '.gitattributes does not force LF on *.sh and '
                    '*.py, so a Windows checkout produces scripts that will not run')

    posix = re.compile(r'python3 |\.elf|\bcodegenerate\.sh\b')
    windows = re.compile(r'\.exe|\.bat|Windows|python tools')
    pages = ['AGENTS.md'] + ['docs/agent/' + p for p in agent_pages()]
    offenders = []
    for page in pages:
        text = read(*page.split('/'))
        if posix.search(text) and not windows.search(text):
            offenders.append(page)
            report.fail('portability', '{} gives a POSIX command with no Windows '
                        'form'.format(page))
    report.ok('portability', '{} of {} pages give both command forms'
              .format(len(pages) - len(offenders), len(pages)))

    if 'windows-' not in agent_workflow():
        report.fail('portability', 'the agent workflow has no Windows runner, so '
                    'half the documented commands are never executed')


# ---------------------------------------------------------------------------
def run():
    report = Report()
    features = resolve_features()
    check_coverage(report, features)
    check_truth(report)
    check_verification(report)
    check_prohibitions(report)
    check_reachable(report)
    check_includes(report)
    check_member_shapes(report)
    check_sdk_paths(report)
    check_fetch_ref(report)
    check_data_types(report)
    check_stated_numbers(report)
    check_shipped_tools(report)
    check_entry_toll(report)
    check_page_budget(report)
    check_duplication(report)
    check_generated_code(report)
    check_ci(report)
    check_tools(report)
    check_observability(report)
    check_portability(report)
    return report


def main():
    parser = argparse.ArgumentParser(
        description='Check the agent corpus: pages, recipes, tools and evals.')
    parser.add_argument('--verbose', action='store_true',
                        help='print the checks that pass as well as the findings')
    parser.add_argument('--json', action='store_true', help='machine readable')
    parser.add_argument('--strict', action='store_true',
                        help='exit 1 on a warning as well as on a failure')
    args = parser.parse_args()

    report = run()
    failures = report.count(FAIL)
    warnings = report.count(WARN)

    if args.json:
        print(json.dumps({
            'findings': [{'severity': s, 'check': c, 'message': m}
                         for s, c, m in report.findings],
            'passed': [{'check': c, 'message': m} for c, m in report.passed],
            'fail': failures, 'warn': warnings, 'note': report.count(NOTE),
        }, indent=2))
    else:
        order = {FAIL: 0, WARN: 1, NOTE: 2}
        for severity, check, message in sorted(report.findings,
                                               key=lambda f: (order[f[0]], f[1])):
            print('{:<5} {:<14} {}'.format(severity, check, message))
        if args.verbose:
            for check, message in report.passed:
                print('{:<5} {:<14} {}'.format('ok', check, message))
        print('{} finding(s): {} failure(s), {} warning(s), {} note(s)'.format(
            len(report.findings), failures, warnings, report.count(NOTE)))

    if failures:
        return 1
    return 1 if (args.strict and warnings) else 0


if __name__ == '__main__':
    sys.exit(main())
