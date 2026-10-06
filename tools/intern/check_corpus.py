#!/usr/bin/env python3
# ===========================================================================
# Checks the agent corpus: the pages, recipes, tools and evals an agent builds
# an application from.
#
#   python3 tools/intern/check_corpus.py             the findings
#   python3 tools/intern/check_corpus.py --verbose   every check, passing ones too
#   python3 tools/intern/check_corpus.py --json      the same, machine readable
#   python3 tools/intern/check_corpus.py --strict    a warning fails the run too
#
# Every check is a rule with a definite answer, and the run reports which rules
# hold. Nothing is weighted, nothing is interpolated, and no total is printed: a
# number summed out of these checks would need weights, and a weight is a
# judgement wearing an instrument's clothes. A moved total says something broke
# and not what; a named finding says what.
#
# The instrument never builds, never compiles and never calls a model, so it gives
# the same answer on any machine with no network. Some cases do run the Python
# tools: they lay out a temporary project and invoke the generator and the contract
# checker on it. Where a property can only be shown by compiling or running an
# application, what is checked is whether CI runs it, not whether it passes here:
# that is the property that keeps the corpus true after this week.
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
import contextlib
import io
import copy
import glob
import json
import os
import datetime
import re
import subprocess
import shutil
import sys
import tempfile
import zipfile

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
     '37-threads.md',           'REGISTER_WORKER_THREAD', '07-worker-events',
     ('examples/18_pubworker',)),
    ('timers',        'periodic and delayed work',
     '33-timers.md',            'start_timer',         '04-timer',
     ('examples/08_timer',)),
    ('custom_events', 'declaring and dispatching a custom event',
     '23-events.md',            'AREG_DECLARE_EVENT',  '07-worker-events',
     ('examples/18_pubworker',)),
    ('attributes',    'attributes and broadcasts',
     '20-service-interface.md', 'roadcast',            '03-attributes-and-broadcast',
     ('examples/25_pubsub',)),
    ('multi_service', 'more than one service in an application',
     None,                      'two services',        '05-two-services',
     ('examples/12_svcmulti',)),
    ('ipc',           'splitting an application across processes',
     None,                      'Category="Public"',   '02-ipc-two-processes',
     ('examples/15_pubsvc',)),
    ('state_machine', 'adding a state machine',
     '22-state-machine.md',     '.fsml',               '06-state-machine',
     ('examples/19_pubfsm',)),
    # The page teaches the spec field, not the XML element: gen_docs.py writes the
    # <Guard> tree from it, so "guard" is what an author now has to find.
    ('fsm_guard',     'guarding a transition on a condition',
     '22-state-machine.md',     '"guard"|<Guard',      '06-state-machine',
     ('examples/19_pubfsm',)),
    ('fsm_attribute', 'giving a state machine its own data',
     '22-state-machine.md',     '"attributes"|AttributeSet', '06-state-machine',
     ('examples/19_pubfsm',)),
    ('logging',       'logging from application code',
     '34-logging.md',           'LOG_DBG',             '07-worker-events',
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
     '37-threads.md',           'BEGIN_REGISTER_THREAD_EX', '07-worker-events',
     ('examples/21_locwatchdog',)),
    ('runtime_model', 'building a model at run time',
     '37-threads.md',           'add_model_unique',    '10-runtime-model',
     ('examples/17_pubtraffic',)),
    ('base_api',      'strings and containers',
     '40-base-api.md',          'areg::String',        '01-local-single-process',
     ('examples/05_buffer', 'examples/06_file')),
    ('runtime_api',   'the application, components, threads, time and files',
     '42-runtime-api.md',       'areg::SharedBuffer',  '07-worker-events',
     ('examples/05_buffer', 'examples/06_file')),
    ('testing',       'testing an application and its components',
     '52-testing.md',           'scripted provider',   '12-testing',
     ()),
    ('debugging',     'working out why it does not work',
     '51-debug.md',             'check_contract',      None,
     ('tools/intern/evals',)),
]

# ---------------------------------------------------------------------------
# Schema features: what an author may write into a .siml, .fsml or .dtml
#
# A construct the schemas accept and no page explains is one an agent can only
# learn from the grammar, and the grammar says what an element may contain and
# never what it means. One such gap -- History on a state -- cost a measured run
# a 50 KB schema read carried across sixty turns, and the answer was not in it.
#
#   name    what the author is trying to do
#   token   what it looks like in a document, matched literally
#   page    the page that must explain it
#   proof   what that explanation must contain
#
# A feature no page explains is a failure unless docs/agent/.schema-gaps records
# it, with the reason. A feature nothing in the tree writes is reported too: an
# unexercised construct is one whose first real use finds the defect.
# ---------------------------------------------------------------------------
SCHEMA_FEATURES = [
    ('history pseudo-state', 'HistoryDepth="',       '22-state-machine.md',
     '"depth"|HistoryDepth'),
    ('a hosted machine',     'Submachine="',         '22-state-machine.md', 'Submachine'),
    ('a level reporting done', 'OnFinal="',          '22-state-machine.md', 'OnFinal'),
    ('a guarded transition', '<Guard',               '22-state-machine.md', '"guard"|<Guard'),
    ('machine data',         '<AttributeSet',        '22-state-machine.md',
     '"attributes"|AttributeSet'),
    ('machine constants',    '<ConstantList',        '22-state-machine.md',
     '"constants"|ConstantList'),
    ('sending an event',     '<EventSend',           '22-state-machine.md',
     '"send"|EventSend'),
    ('starting a timer',     '<TimerStart',          '22-state-machine.md',
     '"start X"|TimerStart'),
    ('stopping a timer',     '<TimerStop',           '22-state-machine.md',
     '"stop X"|TimerStop'),
    ('an internal transition', 'Kind="Internal"',    '22-state-machine.md',
     'Kind="Internal"|a transition without `"to"`'),
    ('a final state',        'Kind="Final"',         '22-state-machine.md', 'Kind="Final"'),
    ('a trigger',            'MethodType="Trigger"', '22-state-machine.md', 'Trigger'),
    ('an action',            'MethodType="Action"',  '22-state-machine.md', 'Action'),
    ('service reach',        'Category="',           '20-service-interface.md', 'Category'),
    ('a request',            'MethodType="Request"', '20-service-interface.md', 'Request'),
    ('a response',           'MethodType="Response"', '20-service-interface.md', 'Response'),
    ('a broadcast',          'MethodType="Broadcast"', '20-service-interface.md', 'Broadcast'),
    ('attribute notification', 'Notify="',           '20-service-interface.md', 'Notify'),
    ('a shared constant',    '<Constant ',           '20-service-interface.md', 'ConstantList'),
    ('a parameter default',  '<Value',               '20-service-interface.md', 'default'),
    ('an enumeration',       'Type="Enumeration"',   '21-data-types.md',    'Enumeration'),
    ('a structure',          'Type="Structure"',     '21-data-types.md',    'Structure'),
    ('an existing C++ type', 'Type="Imported"',      '21-data-types.md',    'Imported'),
    ('a container type',     'Type="Container"',     '21-data-types.md',    'Container'),
    ('including a document', '<IncludeList',         '21-data-types.md',    'IncludeList'),
    ('deprecating a name',   'IsDeprecated="',       '20-service-interface.md', 'IsDeprecated'),
]

DOCUMENT_SUFFIXES = ('.siml', '.fsml', '.dtml')


def schema_gaps():
    """Recorded features no page explains yet: {token: reason}.

    docs/agent/.schema-gaps names them, so a gap is argued and reported rather
    than forgotten. An entry for a feature that is documented is a stale entry
    and is a failure, the same discipline as .budgets.
    """
    gaps = {}
    for line in read('docs', 'agent', '.schema-gaps').splitlines():
        line = line.split('#')[0].strip()
        if ' = ' in line:
            token, _, reason = line.partition(' = ')
            gaps[token.strip()] = reason.strip()
    return gaps


def documents_writing(token):
    """Which documents in the tree write this construct, recipes first."""
    found = []
    for base in ('docs/agent/recipes', 'examples'):
        root = os.path.join(ROOT, base)
        if not os.path.isdir(root):
            continue
        for here, dirs, files in os.walk(root):
            dirs[:] = [d for d in dirs if d not in ('.git', 'build', 'out')]
            for name in sorted(files):
                if not name.endswith(DOCUMENT_SUFFIXES):
                    continue
                with open(os.path.join(here, name), encoding='utf-8',
                          errors='replace') as handle:
                    if token in handle.read():
                        found.append(os.path.relpath(
                            os.path.join(here, name), ROOT).replace(os.sep, '/'))
    return found


# ---------------------------------------------------------------------------
# The generator carries its own copy of the catalogue
#
# codegen.jar embeds data/rules.xml and the three .xsd files, and validates with
# those. tools/explain_rule.py reads tools/schema/rules.xml instead. When a
# finding names a rule the other copy does not have, the number the generator
# printed explains as nothing, which is the one failure the rule numbers exist
# to prevent.
#
# The two directions are not the same fault. A rule the jar has and the tree
# does not is fatal: an agent is handed a number it cannot look up. A rule the
# tree has and the jar does not is a delivery in flight -- the catalogue is
# written before the generator that emits it.
# ---------------------------------------------------------------------------
JAR_DATA = 'data'


def read_bytes(*parts):
    """Raw bytes of a repository file, or None when it is not there.

    The text reader translates line endings, and these files are compared with
    the copies inside codegen.jar.
    """
    path = os.path.join(ROOT, *parts)
    if not os.path.isfile(path):
        return None
    with open(path, 'rb') as handle:
        return handle.read()


def same_document(one, other):
    """True when two XML files hold the same document.

    .gitattributes stores the schemas with line feeds and every checkout gets
    them back that way; the jar build packs the copies its own checkout holds.
    A line break is not part of an XML document, so it is folded away before
    the comparison and every other byte still has to match.
    """
    if one is None or other is None:
        return False
    return one.replace(b'\r\n', b'\n') == other.replace(b'\r\n', b'\n')


def jar_member(name):
    """The bytes codegen.jar carries for a schema file, or None."""
    jar = os.path.join(ROOT, 'tools', 'codegen.jar')
    if not os.path.isfile(jar):
        return None
    try:
        with zipfile.ZipFile(jar) as archive:
            return archive.read('{}/{}'.format(JAR_DATA, name))
    except (KeyError, zipfile.BadZipFile, OSError):
        return None


def jar_data_members():
    """Base names of the files codegen.jar carries under data/."""
    jar = os.path.join(ROOT, 'tools', 'codegen.jar')
    if not os.path.isfile(jar):
        return []
    try:
        with zipfile.ZipFile(jar) as archive:
            names = archive.namelist()
    except (zipfile.BadZipFile, OSError):
        return []
    prefix = JAR_DATA + '/'
    return [n[len(prefix):] for n in names
            if n.startswith(prefix) and not n.endswith('/')]


def catalogue_rows(text):
    """{(number, name): bands} for every rule a rules.xml declares."""
    rows = {}
    for number, name, bands in re.findall(
            r'<Rule Number="(\d+)" Name="([A-Z_]+)"[^>]*Bands="([^"]*)"', text):
        rows[(int(number), name)] = bands
    return rows


def check_rule_texts(report):
    """Every rule states the fault and the corrective action.

    The generator, the editor's validation window and explain_rule.py all read
    both texts out of this one file. A rule with an empty Fix leaves each of them
    naming a fault with nothing to do about it.
    """
    text = read('tools', 'schema', 'rules.xml')
    blocks = re.findall(r'<Rule Number="\d+" Name="[A-Z_]+".*?</Rule>', text, re.S)
    if not blocks:
        report.fail('catalogue', 'tools/schema/rules.xml declares no rules')
        return
    for block in blocks:
        number, name = re.search(r'Number="(\d+)" Name="([A-Z_]+)"', block).groups()
        for tag in ('Summary', 'Fix'):
            body = re.search(r'<{0}>(.*?)</{0}>'.format(tag), block, re.S)
            if body is None or not body.group(1).strip():
                report.fail('catalogue', '{} ({}) carries no {}, so the generator, '
                            'the editor and explain_rule.py {} all answer that '
                            'number with half an answer'
                            .format(number, name, tag, number))
    report.ok('catalogue', '{} rules each state a fault and a fix'.format(len(blocks)))


def check_generator_catalogue(report):
    embedded = jar_member('rules.xml')
    if embedded is None:
        report.fail('catalogue', 'codegen.jar carries no data/rules.xml, so what '
                    'the generator validates with cannot be compared with what '
                    'explain_rule.py explains from')
        return

    jar = catalogue_rows(embedded.decode('utf-8', 'replace'))
    tree = catalogue_rows(read('tools', 'schema', 'rules.xml'))

    unexplainable = sorted(jar.keys() - tree.keys())
    for number, name in unexplainable:
        report.fail('catalogue', 'codegen.jar can report {} ({}), which '
                    'tools/schema/rules.xml does not carry: explain_rule.py {} '
                    'answers nothing'.format(number, name, number))

    pending = sorted(tree.keys() - jar.keys())
    for number, name in pending:
        report.note('catalogue', 'tools/schema/rules.xml carries {} ({}), which '
                    'the installed codegen.jar cannot yet report: a generator '
                    'drop is outstanding'.format(number, name))

    # A band the jar can emit and the tree does not know is a code that explains
    # as nothing. The other way round is the catalogue being written first.
    for number, name in sorted(jar.keys() & tree.keys()):
        in_jar = set(jar[(number, name)].split())
        in_tree = set(tree[(number, name)].split())
        unexplained = sorted(in_jar - in_tree)
        if unexplained:
            report.fail('catalogue', '{} ({}) is banded {} by codegen.jar, which '
                        'tools/schema/rules.xml does not carry: the code it '
                        'reports there explains as nothing'
                        .format(number, name, ', '.join(unexplained)))
        elif in_tree - in_jar:
            report.note('catalogue', '{} ({}) gained the band(s) {} in '
                        'tools/schema/rules.xml, which the installed codegen.jar '
                        'does not yet report'
                        .format(number, name, ', '.join(sorted(in_tree - in_jar))))

    # A schema must exist beside the jar, because explain_rule.py and every editor
    # read that copy. The jar carries its own copy of the same file, written by the
    # jar build; the two are one file delivered twice, so they must hold the same
    # document. They hold different ones only when the jar was built from another
    # tree, and then a document is accepted by one and refused by the other.
    for name in ('siml.xsd', 'dtml.xsd', 'fsml.xsd'):
        beside = read_bytes('tools', 'schema', name)
        if not beside:
            report.fail('catalogue', 'tools/schema/{} is missing, so a document is '
                        'checked against no schema at all'.format(name))

    schema_files = ('siml.xsd', 'dtml.xsd', 'fsml.xsd', 'datatype.xml', 'rules.xml')

    # Driven by what is beside the jar, not by what the jar happens to carry: a jar
    # built by another route carries fewer members, and iterating over its members
    # made the missing comparisons disappear rather than fail.
    carried = jar_data_members()
    for name in sorted(schema_files):
        beside = read_bytes('tools', 'schema', name)
        if beside is None:
            continue
        if name not in carried:
            report.note('catalogue', 'codegen.jar carries no data/{}, so nothing '
                        'compares tools/schema/{} against the copy the generator '
                        'was built with'.format(name, name))
            continue
        if not same_document(beside, jar_member(name)):
            report.fail('catalogue', 'data/{} inside codegen.jar differs from '
                        'tools/schema/{}. The generator validates with its copy and '
                        'explain_rule.py and the editors read the other, so a '
                        'document is accepted by one and refused by the other. '
                        'Rebuild the jar from this tree'.format(name, name))

    report.ok('catalogue', '{} of {} rules are known to both codegen.jar and '
              'tools/schema/rules.xml'
              .format(len(jar.keys() & tree.keys()), len(tree)))


def check_schema_features(report):
    gaps = schema_gaps()
    explained = set()
    for name, token, page, proof in SCHEMA_FEATURES:
        text = read('docs', 'agent', page)
        documented = proven(text, proof)
        if documented:
            explained.add(token)
        elif token in gaps:
            report.note('schema', '{} ({}) is explained nowhere: {}'
                        .format(name, token, gaps[token]))
            continue
        else:
            report.fail('schema', '{} ({}) is accepted by the schema and '
                        'explained on no page -- an agent can only learn it from '
                        'the grammar'.format(name, token))
            continue

        writers = documents_writing(token)
        durable = [w for w in writers if w.startswith('docs/agent/recipes')]
        if durable:
            continue
        if writers:
            report.note('schema', '{} ({}) is written only under examples/, which '
                        'is optional -- a recipe would exercise it'
                        .format(name, token))
        else:
            report.warn('schema', '{} ({}) is documented and no document in this '
                        'tree writes it: its first real use is its first test'
                        .format(name, token))

    for token in sorted(gaps):
        if token in explained:
            report.fail('schema', '.schema-gaps records {}, which a page now '
                        'explains'.format(token))
        elif token not in [f[1] for f in SCHEMA_FEATURES]:
            report.fail('schema', '.schema-gaps records {}, which is not a '
                        'catalogued feature'.format(token))
    report.ok('schema', '{} of {} schema features are explained by a page'
              .format(len(explained), len(SCHEMA_FEATURES)))


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
    ('documented commands, deep', 'check_commands.py --deep'),
    ('the five-step chain',   'build_project.py'),
    ('a scenario actually run', 'run_scenarios.py --build'),
    # The macOS job is here by this string and not by "macos-": cmake.yml already
    # builds on macOS, so a runner name proves nothing about the agent tools.
    ('the scenario runner self-test', 'run_scenarios.py --self-test'),
    ('non-Linux runner',      'windows-'),
]

# Tools AGENTS.md tells an agent to run. A named tool that is absent is a dead
# instruction, and an agent follows it before it discovers that.
TOOLS = ['setup_project.py', 'gen_skeleton.py', 'fsml_layout.py', 'run_scenarios.py',
         'check_contract.py', 'explain_rule.py', 'schema_help.py', 'check-env.sh', 'check-env.bat', 'codegenerate.sh',
         'codegenerate.bat', 'setup-project.sh', 'setup-project.bat', 'setup-project.ps1']

# Every byte of the reading corpus is paid by the agent that opens it, and an addition
# is only ever local while a run pays for the whole set. The ceiling is what stops the
# set growing one locally-justified paragraph at a time: raising this number is a
# deliberate edit in a reviewed file, and the commit that raises it says what it bought.
# The headroom is deliberately small: a ceiling with room in it ratchets nothing.
CORPUS_CEILING = 195701

PAGE_CEILING = 8 * KB
# The stop the exception mechanism did not have. An entry in .budgets raises the
# ceiling for one page, and every raise so far has been argued and granted, so the
# six largest pages are the six exempt ones. No .budgets entry may name a size above
# this, and no page may pass it whatever .budgets says: a page this large is split or
# moved into the schema, and the exception mechanism cannot decide otherwise.
PAGE_HARD_CEILING = 20 * KB
ENTRY_TARGET = 10 * KB
# What AGENTS.md is allowed while it is over the target, and why. Everything that is
# stated at its point of use has come out: the worksheet format, which the worksheet
# prints; the two lookup tools, described once at the refusal they answer; the
# requirements check-env runs; the runbook routed twice. What is left is the routing
# table, the tool table and section 6, whose ten one-line rules --audit-prohibitions
# matches against api.json one by one. Over this size is a failure, not a warning:
# the page may shrink without a commit here and may not grow.
ENTRY_ALLOWED = 11776
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
    """Bytes of a repository file with CRLF counted as LF, so a checkout's line
    endings never move a budget verdict."""
    path = os.path.join(ROOT, *parts)
    if not os.path.isfile(path):
        return 0
    with open(path, 'rb') as handle:
        data = handle.read()
    return len(data) - data.count(b'\r\n')


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
    raw = read('tools', 'intern', 'evals', 'tasks.json')
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


def proven(text, proof):
    """True when the text carries the proof, or any one of its alternatives.

    A proof spelled "a|b" is answered by either, which is how one feature is proved
    by the spec field a page teaches and by the XML element a recipe document holds.
    """
    lowered = text.lower()
    return any(part.lower() in lowered for part in proof.split('|'))


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
            return proven(handle.read(), proof)

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
                if proven(handle.read(), proof):
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
            documented = bool(text) and proven(text, proof)
            where = 'docs/agent/' + page
        else:
            documented = proven(whole, proof)
            where = 'the corpus'

        # A demonstration under examples/ is optional: that directory is not
        # installed with the SDK and its contents change, so a feature it alone
        # shows is reported as an observation, never as a shortfall.
        durable = ['docs/agent/recipes/' + recipe] if recipe else []
        durable += [p for p in also if not p.split('/')[0] == 'examples']
        optional = [p for p in also if p.split('/')[0] == 'examples']
        shown = any(tree_has(p, proof) for p in durable)
        by_example = (not shown) and any(tree_has(p, proof) for p in optional)

        # A task grades a feature when it is the recipe's task, or when the task
        # names the feature or the thing that proves it.
        graded = bool(recipe) and recipe in graded_refs
        if not graded:
            graded = (key.replace('_', '-') in task_blob
                      or key.replace('_', ' ') in task_blob
                      or proven(task_blob, proof))

        resolved.append({'key': key, 'label': label, 'page': where,
                         'documented': documented, 'example': recipe,
                         'shown': shown or by_example, 'durable': shown,
                         'by_example': by_example, 'optional': bool(optional),
                         'graded': graded,
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
        if f['prose_only']:
            pass
        elif f['by_example']:
            report.note('coverage', '{}: only an example shows it, and examples/ '
                        'is optional -- a recipe would protect it'.format(f['key']))
        elif not f['durable']:
            report.warn('coverage', '{}: no recipe shows it{}'
                        .format(f['key'], ', and the example that did is not in '
                                'this tree' if f['optional'] else ''))
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

    # examples/ is optional: it is not installed with the SDK, it is not needed
    # to build an application, and its contents change. No page may state how
    # many there are, because that number is a claim the tree falsifies without
    # anything being wrong.
    counting = re.compile(
        r'\b(\d+|one|two|three|four|five|six|seven|eight|nine|ten|eleven|twelve|'
        r'thirteen|fourteen|fifteen|sixteen|seventeen|eighteen|nineteen|twenty|'
        r'thirty|forty|fifty)(-\w+)?\s+(complete applications?|examples)\b',
        re.IGNORECASE)
    counted = []
    for where in ('AGENTS.md', 'CODEBASE.md', 'docs/agent/41-examples.md',
                  'docs/agent/51-debug.md'):
        found = counting.search(read(*where.split('/')))
        if found:
            counted.append(where + ': "' + found.group(0) + '"')
    out.append(('no page states how many examples there are', not counted,
                '; '.join(counted)))

    api = read('docs', 'agent', 'api.json')
    out.append(('api.json states the C++ floor AGENTS.md states',
                '"C++17"' in api and 'C++17' in agents, ''))

    out.append(('the FSM event enumerator is documented with its EVENT_ prefix',
                'EVENT_' in fsm, 'the generator emits EVENT_<Name>'))
    out.append(('no page promises an unprefixed FSM event enumerator',
                not re.search(r'FsmEventValue::(?!EVENT_)[A-Z]', fsm), ''))
    out.append(('the FSM timer enumerator carries no prefix, and the page says so',
                'FsmTimer' in fsm and 'prefix' in fsm.lower(), ''))
    # The cheat sheet no longer carries the .siml XML: gen_docs.py writes the
    # document, so the element table of the page that teaches it is the one copy.
    out.append(('the interface page documents ConstantList',
                'ConstantList' in read('docs', 'agent', '20-service-interface.md'), ''))
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

    result = subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'intern',
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
    checker = (read('tools', 'intern', 'check_recipes.py')
               + read('tools', 'intern', 'check_observability.py'))
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
    data = json.loads(raw)
    rules = data.get('prohibitions', []) + data.get('base_api_notes', [])

    # What the checker reports, not what its source text spells. One rule is reported
    # under the number tools/schema/rules.xml gives it, read when the checker starts,
    # so it appears nowhere in the file to be found by a search.
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import check_contract
        implemented = set(rule for rule, _severity, _summary in check_contract.CHECKS)
    except Exception as failure:
        report.fail('prohibition', 'check_contract.py cannot say what it detects, so '
                    'no prohibition can be shown to be implemented: {}'.format(failure))
        return

    missing = [r['id'] for r in rules if r['id'] not in implemented]
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
    entry = read('AGENTS.md')
    if 'all {}'.format(spelled) not in entry:
        report.fail('prohibition', 'AGENTS.md does not say check_contract.py reports '
                    'all {} rules api.json states'.format(spelled))
    else:
        report.ok('prohibition', 'AGENTS.md states the {} rules api.json carries'
                  .format(spelled))

    # A reader who counts finds the two groups section 6 splits itself into, not the
    # total. The 2026-09-14 audit counted 18 where the sentence said nineteen: the
    # groups have to add up to it, and the first group has to be the bullets there
    # actually are.
    section = entry[entry.find('## 6. Never'):]
    section = section[:section.find('\n## ')] if '\n## ' in section else section
    bullets = len(re.findall(r'^- ', section, re.M))
    first = re.search(r'The (\w+) below', section)
    rest = re.search(r'The other (\w+) are', section)
    if not first or not rest:
        report.fail('prohibition', 'section 6 of AGENTS.md no longer says how many '
                                   'rules each of its two groups carries, so a reader '
                                   'counting them cannot reach the stated total')
    elif first.group(1) not in words or rest.group(1) not in words:
        report.fail('prohibition', 'section 6 of AGENTS.md counts its groups as "{}" '
                    'and "{}", which are not numbers a reader can add'
                    .format(first.group(1), rest.group(1)))
    else:
        named = words.index(first.group(1))
        others = words.index(rest.group(1))
        if named != bullets:
            report.fail('prohibition', 'section 6 of AGENTS.md says "{}" rules stand '
                        'below it and writes {} bullet(s)'.format(first.group(1), bullets))
        elif named + others != count:
            report.fail('prohibition', 'section 6 of AGENTS.md splits its rules into {} '
                        'and {}, which is {}; api.json states {}'
                        .format(named, others, named + others, count))
        else:
            report.ok('prohibition', 'the {} bullets and the {} one-line fixes of '
                      'AGENTS.md section 6 add up to the {} rules api.json states'
                      .format(named, others, count))

    report.ok('prohibition', '{} of {} rules are stated, detected and proven'
              .format(len(proven & {r['id'] for r in rules}), len(rules)))


def recipe_executables(reference):
    """The executable names a recipe's CMakeLists files declare."""
    names = []
    for part in (('CMakeLists.txt',), ('src', 'CMakeLists.txt')):
        text = read('docs', 'agent', 'recipes', reference, *part)
        names += re.findall(r'macro_declare_executable\s*\(\s*([A-Za-z0-9_]+)', text)
    return names


def defected_source(task):
    """The text of the file a repair task breaks, with the break applied."""
    defect = task.get('defect') or {}
    if 'file' not in defect or 'create' in defect or 'environment' in defect:
        return None
    text = read('docs', 'agent', 'recipes', task['reference'],
                *defect['file'].split('/'))
    if not text:
        return None
    for edit in defect.get('edits') or [defect]:
        if edit.get('find') not in text:
            return None
        text = text.replace(edit['find'], edit['replace'], 1)
    return text


def check_prescribed_calls(report):
    """A page that names a prohibited call names its replacement on the same page.

    A "Fix" column is read under pressure and taken literally. A page naming the call
    api.json forbids, without naming the one it prescribes, sends the reader to a
    finding of the very rule it is trying to help with. The pairing is api.json's, in
    the "pages" key of a prohibition: it holds the name to look for and the name that
    has to stand beside it.
    """
    raw = read('docs', 'agent', 'api.json')
    if not raw:
        report.fail('prescribed-call', 'docs/agent/api.json is missing')
        return
    paired = [(rule['id'], rule['pages']) for rule in json.loads(raw).get('prohibitions', [])
              if isinstance(rule.get('pages'), dict)]
    if not paired:
        report.fail('prescribed-call', 'no prohibition in api.json pairs a forbidden '
                    'call with its replacement, so no page can be checked against one')
        return
    checked = 0
    for page in agent_pages():
        text = read('docs', 'agent', page)
        if not text:
            continue
        for rule, pages in paired:
            names, requires = pages.get('names'), pages.get('requires')
            if not names or not requires or names not in text:
                continue
            checked += 1
            if requires not in text:
                report.fail('prescribed-call',
                            'docs/agent/{} names {}() and never {}(), which {} '
                            'prescribes: a reader following this page is reported by '
                            'that rule'.format(page, names, requires, rule))
    report.ok('prescribed-call', '{} page(s) name a call api.json restricts, and each '
              'names the replacement beside it'.format(checked))


def check_grpc_isolation(report):
    """The gRPC arm stages the scenario runner and nothing else.

    The arm measures a cold start against gRPC alone. A snapshot of this repository
    beside its working directory makes that claim unprovable, whatever the prompt
    says, because the shell is not restricted to the directory the Read tool is.
    """
    if not os.path.isdir(os.path.join(ROOT, 'examples', 'ai-benchmark')):
        report.note('grpc-arm', 'examples/ai-benchmark/ is not installed')
        return
    text = read('examples', 'ai-benchmark', 'run-benchmark.sh')
    if not text:
        report.fail('grpc-arm', 'examples/ai-benchmark/run-benchmark.sh is missing')
        return
    branch = re.search(r'if \[ "\$\{FRAMEWORK\}" = "grpc" \]; then(.*?)\n    else',
                       text, re.S)
    if branch is None:
        report.fail('grpc-arm', 'run-benchmark.sh no longer stages the gRPC arm '
                                'separately, so it takes the whole snapshot')
        return
    body = branch.group(1)
    if 'snapshot "${SDK}"' in body:
        report.fail('grpc-arm', 'the gRPC arm copies the whole checkout; the corpus '
                                'must not exist inside the run')
    if 'run_scenarios.py' not in body:
        report.fail('grpc-arm', 'the gRPC arm stages no scenario runner')
    stray = re.search(r'stray="\$\(find .*?\)"', body, re.S)
    staged = ('run_scenarios.py', 'scenario_dialect.py', 'task.md')
    if stray is None or any(name not in stray.group(0) for name in staged):
        report.fail('grpc-arm', 'the gRPC arm does not verify that it staged nothing '
                                'but the scenario runner, its dialect and the task')
    else:
        report.ok('grpc-arm', 'the gRPC arm stages only the runner, its gRPC dialect '
                              'and the task, and refuses to run if anything else is '
                              'beside it')
    # The task is the one page the arm has, and the agent is given the snapshot and
    # its own working directory. A task staged beside them is read by neither.
    if '"${SNAP}/task.md"' not in body:
        report.fail('grpc-arm', 'the gRPC arm does not stage the task inside the '
                                'snapshot, so the agent cannot open it')
    if re.search(r'TASK_RUN="\$\{RUN\}/task\.md"', text):
        report.fail('grpc-arm', 'the gRPC prompt sends the agent to ${RUN}/task.md, '
                                'which is outside every directory it is given')

    # One word in one file is not a cold start. The arm is only cold while nothing
    # the agent can reach carries a name of this corpus, its paths included, so the
    # ban list and the sweep that applies it are what the check holds in place.
    ban = re.search(r"COLD_START_BAN='([^']*)'", text)
    if ban is None:
        report.fail('grpc-arm', 'run-benchmark.sh has no COLD_START_BAN, so nothing '
                                'states which names the gRPC arm may not carry')
        ban_re = None
    else:
        ban_re = ban.group(1)
        missing = [word for word in ('areg', 'AGENTS', 'build_project', 'worksheet',
                                     'design', 'siml', 'runbook')
                   if word not in ban_re]
        if missing:
            report.fail('grpc-arm', 'COLD_START_BAN does not name {}: the gRPC arm '
                                    'would be told of them'.format(', '.join(missing)))
        sweep = re.search(r'grep -rniE "\$\{COLD_START_BAN\}"([^\n]*\n[^\n]*)', text)
        if sweep is None or 'prompt.txt' not in sweep.group(0) \
                or '${SNAP}' not in sweep.group(0):
            report.fail('grpc-arm', 'nothing sweeps the prompt and every staged file '
                                    'for the banned names before the agent starts')
        else:
            report.ok('grpc-arm', 'the prompt, every staged file and the run path are '
                                  'swept for this corpus before the gRPC arm starts')

    # The two arms are only comparable while neither is told the other exists. The
    # runner is shared, so it names no framework and takes its wording from the
    # dialect staged beside it.
    runner = read('tools', 'agent', 'run_scenarios.py') or ''
    named = [word for word in ('areg', 'AGENTS.md', 'build_project', 'gen_skeleton',
                               'worksheet', 'bodies.txt', 'design.json', '.siml',
                               'docs/agent', 'runbook')
             if word.lower() in runner.lower()]
    if named:
        report.fail('grpc-arm', 'run_scenarios.py is shared by both arms and names '
                                '{}: the gRPC arm reads the file it is told to '
                                'use'.format(', '.join(named)))
    if "spec_from_file_location('scenario_dialect'" not in runner \
            or 'dirname(os.path.abspath(__file__))' not in runner:
        report.fail('grpc-arm', 'run_scenarios.py does not load its wording from the '
                                'dialect beside it: either it names one framework to '
                                'both arms, or a file in the measured project decides '
                                'what it says')
    ours = read('tools', 'agent', 'scenario_dialect.py') or ''
    theirs = read('examples', 'ai-benchmark', 'grpc-scenario-dialect.py') or ''
    if not ours:
        report.fail('grpc-arm', 'tools/agent/scenario_dialect.py is missing, so the '
                                'areg arm gets the neutral wording and the two arms '
                                'are no longer told the same amount')
    elif re.search(r'grpc|protoc|protobuf|\.proto', ours, re.I):
        report.fail('grpc-arm', 'the areg dialect names gRPC; the rule is symmetric '
                                'and neither arm may name the other')
    if not theirs:
        report.fail('grpc-arm', 'examples/ai-benchmark/grpc-scenario-dialect.py is '
                                'missing, so the gRPC arm has no wording of its own')
    elif ban_re is not None and re.search(ban_re.replace('\\', '\\'), theirs, re.I):
        report.fail('grpc-arm', 'the gRPC dialect carries a name of this corpus, and '
                                'it is copied into the arm')
    elif theirs:
        report.ok('grpc-arm', 'each arm has a dialect naming its own framework and '
                              'neither names the other')
    helped = subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'agent',
                                                          'run_scenarios.py'), '--help'],
                            cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            stdin=subprocess.DEVNULL)
    if re.search(r'areg', helped.stdout.decode('utf-8', 'replace'), re.I):
        report.fail('grpc-arm', 'run_scenarios.py --help names areg, and the gRPC arm '
                                'reads it')
    for name in ('grpc-coffeemachine-prompt.txt', 'grpc-ai-prompt-template.txt'):
        wrapper = read('examples', 'ai-benchmark', name)
        if not wrapper:
            report.fail('grpc-arm', 'examples/ai-benchmark/{} is missing'.format(name))
            return
        if re.search(r'areg|another framework|other arm', wrapper, re.I):
            report.fail('grpc-arm', '{} names areg or another arm'.format(name))
            return
    report.ok('grpc-arm', 'the gRPC prompts and the runner help name no other '
                          'framework')


# Checklist phrases the hidden probes in verify_run.py score, one per probe.
PROBED_REQUIREMENTS = ('must survive', 'waits more than 20 seconds', 'goes away',
                       'busy-waiting', 'exits 0')


def check_hidden_probes(report):
    """The hidden probes exist, the agent cannot read them, and every task states
    the requirements they score."""
    bench = os.path.join(ROOT, 'examples', 'ai-benchmark')
    if not os.path.isdir(bench):
        report.note('hidden-probes', 'examples/ai-benchmark/ is not installed')
        return
    if not read('examples', 'ai-benchmark', 'verify_run.py'):
        report.fail('hidden-probes', 'examples/ai-benchmark/verify_run.py is missing')
        return
    runner = read('examples', 'ai-benchmark', 'run-benchmark.sh') or ''
    snapshot = runner.partition('snapshot()')[2].partition('\n}\n')[0]
    if 'verify_run.py' not in snapshot:
        report.fail('hidden-probes', 'the snapshot copies verify_run.py, so the agent can '
                                     'read the probes it is scored by')
        return
    for name in TASK_PROMPTS:
        text = read('examples', 'ai-benchmark', name) or ''
        missing = [phrase for phrase in PROBED_REQUIREMENTS if phrase not in text]
        if missing:
            report.fail('hidden-probes', '{} does not state "{}", which a hidden probe '
                                         'scores'.format(name, '", "'.join(missing)))
            return
    result = subprocess.run([sys.executable,
                             os.path.join(bench, 'verify_run.py'), '--self-test'],
                            capture_output=True, text=True, cwd=ROOT,
                            stdin=subprocess.DEVNULL)
    text = (result.stdout + result.stderr).strip()
    if result.returncode != 0:
        report.fail('hidden-probes', 'verify_run.py --self-test: ' +
                    (text.splitlines()[0] if text else 'failed'))
        return
    report.ok('hidden-probes', 'verify_run.py is hidden from the agent, every task '
                               'states the {} requirements it probes, and {}'
                               .format(len(PROBED_REQUIREMENTS),
                                       text.splitlines()[-1] if text else 'it self-tests'))


# Messages of the isolation guards both benchmark runners carry, word for word.
RUNNER_GUARDS = ('the gRPC arm staged more than the scenario runner',
                 'COLD_START_BAN',
                 'is not a cold start -- it names what it exists not to know',
                 'examples/ai-benchmark/verify_run.py',
                 'is inside the checkout; a run must not write where it reads')


def check_runner_parity(report):
    """run-benchmark.ps1 takes the options of run-benchmark.sh and carries its guards."""
    shell = read('examples', 'ai-benchmark', 'run-benchmark.sh')
    if not shell:
        return
    power = read('examples', 'ai-benchmark', 'run-benchmark.ps1')
    if not power:
        report.fail('runner-parity', 'examples/ai-benchmark/run-benchmark.ps1 is missing')
        return

    def options(text):
        return set(re.findall(r'^  (--[a-z-]+)', text, re.M))

    missing = sorted(options(shell) - options(power))
    extra = sorted(options(power) - options(shell))
    if missing or extra:
        report.fail('runner-parity', 'the two runners take different options: only in '
                                     'the .sh {}, only in the .ps1 {}'.format(missing, extra))
        return
    for guard in RUNNER_GUARDS:
        for name, text in (('run-benchmark.sh', shell), ('run-benchmark.ps1', power)):
            if guard not in text:
                report.fail('runner-parity', '{} lacks the guard "{}"'.format(name, guard))
                return
    report.ok('runner-parity', 'run-benchmark.sh and run-benchmark.ps1 take the same {} '
                               'options and carry the same {} guards'
                               .format(len(options(shell)), len(RUNNER_GUARDS)))


def check_fix_bound(report):
    """One number for the fix bound, in the four places that state it.

    The runbook tells the agent a bound; the runners override it for a measured run
    and must quote the runbook's own number when they do; their default is the bound
    every published run used, and the baseline records what that was. Nothing else
    connects the four, so they drift silently and a run comes out incomparable with
    the series it is meant to extend.
    """
    if not os.path.isdir(os.path.join(ROOT, 'examples', 'ai-benchmark')):
        report.note('fix-bound', 'examples/ai-benchmark/ is not installed')
        return
    runbook = read('docs', 'agent', '01-runbook.md')
    documented = set(re.findall(r'at most \*\*(\d+) (?:build|run)-and-fix cycles\*\*', runbook))
    if len(documented) != 1:
        report.fail('fix-bound', 'docs/agent/01-runbook.md states {} bound(s) {}; '
                    'section 8 gives one number for both cycle kinds'
                    .format(len(documented), sorted(documented) or '(none found)'))
        return
    stated = documented.pop()

    runners = {'run-benchmark.sh': read('examples', 'ai-benchmark', 'run-benchmark.sh'),
               'run-benchmark.ps1': read('examples', 'ai-benchmark', 'run-benchmark.ps1')}
    defaults = {}
    for name, text in runners.items():
        if not text:
            report.fail('fix-bound', 'examples/ai-benchmark/{} is missing'.format(name))
            return
        # The number the runner treats as "no override needed", and the one its
        # override rule tells the agent to read instead of.
        quoted = set(re.findall(r'ATTEMPTS\}" != "(\d+)"', text))
        quoted |= set(re.findall(r"\$Attempts -ne '(\d+)'", text))
        quoted |= set(re.findall(r'at most (\d+) (?:build|run)-and-fix', text))
        wrong = sorted(n for n in quoted if n != stated)
        if wrong:
            report.fail('fix-bound', 'docs/agent/01-runbook.md gives the agent a bound '
                        'of {}, and examples/ai-benchmark/{} states {} where it means '
                        'the runbook\'s number. A prompt that overrides the wrong '
                        'number leaves the agent with two bounds'
                        .format(stated, name, ', '.join(wrong)))
            return
        found = (re.findall(r'ATTEMPTS="(\d+)"', text)
                 + re.findall(r"\$Attempts = '(\d+)'", text))
        if len(found) != 1:
            report.fail('fix-bound', 'examples/ai-benchmark/{} sets no single default '
                        'for --attempts: found {}'.format(name, found or '(none)'))
            return
        defaults[name] = found[0]

    if len(set(defaults.values())) != 1:
        report.fail('fix-bound', 'the two runners default --attempts differently: {}'
                    .format(', '.join('%s %s' % kv for kv in sorted(defaults.items()))))
        return
    default = sorted(set(defaults.values()))[0]

    published = set()
    for name in sorted(os.listdir(os.path.join(ROOT, 'examples', 'ai-benchmark'))):
        if name.startswith('baseline-') and name.endswith('.md'):
            published |= set(re.findall(r'\| fix bound \| (\d+) build-and-fix',
                                        read('examples', 'ai-benchmark', name)))
    if not published:
        report.fail('fix-bound', 'no examples/ai-benchmark/baseline-*.md records the fix '
                    'bound its run used, so the default of {} is checked against nothing'
                    .format(default))
        return
    if published != {default}:
        report.fail('fix-bound', 'the runners default --attempts to {} and the published '
                    'baseline ran at {}. A run started with no flags does not reproduce '
                    'the series it is compared with'
                    .format(default, ', '.join(sorted(published))))
        return

    for name, text in runners.items():
        if 'default: {}'.format(default) not in text:
            report.fail('fix-bound', 'examples/ai-benchmark/{} defaults --attempts to {} '
                        'and its --help does not say so'.format(name, default))
            return
    report.ok('fix-bound', 'the runbook bounds the agent at {}, both runners override '
              'from {} and default to {}, and the published baseline ran at {}'
              .format(stated, stated, default, default))


def check_analyzer_keys(report):
    """Every request field analyze_run.py reads is a field it writes.

    The reader and the writer sit hundreds of lines apart, so a field added to one
    and not the other raises KeyError on the next run, after the agent has been paid
    for and the measurement is already spent.
    """
    if not os.path.isdir(os.path.join(ROOT, 'examples', 'ai-benchmark')):
        report.note('analyzer', 'examples/ai-benchmark/ is not installed')
        return
    text = read('examples', 'ai-benchmark', 'analyze_run.py')
    if not text:
        report.fail('analyzer', 'examples/ai-benchmark/analyze_run.py is missing')
        return
    start = text.find('requests.append({')
    if start < 0:
        report.fail('analyzer', 'analyze_run.py builds no request record')
        return
    depth, end = 0, start
    for index in range(start + len('requests.append('), len(text)):
        if text[index] == '{':
            depth += 1
        elif text[index] == '}':
            depth -= 1
            if depth == 0:
                end = index
                break
    written = set(re.findall(r'"(\w+)"\s*:', text[start:end + 1]))
    got = set(re.findall(r'\br\["(\w+)"\]', text))
    missing = sorted(got - written)
    if missing:
        report.fail('analyzer', 'analyze_run.py reads request field(s) it never '
                    'writes: {}'.format(', '.join(missing)))
    else:
        report.ok('analyzer', 'analyze_run.py reads {} request field(s), all written'
                  .format(len(got)))


def check_task_shape(report):
    """A task's declaration against the recipe it names, and its defect against
    the tree that defect builds.

    A task whose reference builds two executables and which declares neither
    binaries nor a run block is launched one process alone, with no peer and no
    router, and waits for a connection that never comes. Only run_evals.py
    --self-check sees it, and only after the 90 second timeout.
    """
    checked = 0
    for task in eval_tasks():
        reference = task.get('reference')
        if not reference:
            continue
        names = recipe_executables(reference)
        spec = task.get('run') or {}
        if len(names) > 1:
            checked += 1
            if task.get('binaries', 1) != len(names):
                report.fail('task-shape', '{} names recipe {}, which declares {} '
                            'executables, but the task declares binaries {}'
                            .format(task['id'], reference, len(names),
                                    task.get('binaries', 1)))
            if not spec:
                report.fail('task-shape', '{} names recipe {}, which declares {} '
                            'executables, and gives no run block, so the harness '
                            'runs one alone and waits out the timeout'
                            .format(task['id'], reference, len(names)))
        for role in [spec.get('lead')] + list(spec.get('background') or []):
            if role and not [n for n in names
                             if n == role or n.endswith('_' + role)]:
                report.fail('task-shape', '{} runs a process called {}, which '
                            'recipe {} does not declare'
                            .format(task['id'], role, reference))
        # Only the lead process's output is captured; a background process is
        # started with its stdout discarded.
        for role in spec.get('background') or []:
            for line in task.get('expect') or []:
                if line.startswith(role + ':'):
                    report.fail('task-shape', '{} expects "{}", which {} prints '
                                'as a background process, whose output the '
                                'harness discards'
                                .format(task['id'], line, role))
        # A defect that plants a definition nothing calls builds a tree the
        # task's own prompt describes wrongly.
        text = defected_source(task)
        if text is not None:
            for edit in (task['defect'].get('edits') or [task['defect']]):
                for name in re.findall(r'\n[A-Za-z_][A-Za-z0-9_:<>* ]*?\b'
                                       r'([a-z_][a-z0-9_]*)\s*\([^;)]*\)\s*\n\{',
                                       edit.get('replace') or ''):
                    if name in ('main', 'if', 'for', 'while', 'switch'):
                        continue
                    calls = re.findall(r'\b' + name + r'\s*\(', text)
                    bodies = re.findall(r'\b' + name + r'\s*\([^;)]*\)\s*\n\{',
                                        text)
                    if len(calls) - len(bodies) < 1:
                        report.fail('task-shape', "{}'s defect defines {}() and "
                                    'nothing in the broken tree calls it'
                                    .format(task['id'], name))
    report.ok('task-shape', '{} multi-process task(s) agree with their recipe, and '
              'no defect plants a definition nothing calls'.format(checked))


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
# Three scaffolders, one project
#
# setup-project.sh and setup-project.ps1 create a project without Python, and
# setup_project.py creates it for an agent. The shell scripts copy the same
# recipes with the same name tokens, so each has to name every mode, recipe and
# token the Python tool names, and nothing else.
# ---------------------------------------------------------------------------
SCAFFOLD_SCRIPTS = [('tools', 'setup-project.sh'), ('tools', 'setup-project.ps1')]
SCRIPT_FALLBACK_RE = re.compile(r'''(?:FALLBACK_TAG=|\$FallbackTag\s*=\s*)["']([^"']+)["']''')
RECIPE_NAME_RE = re.compile(r'\b\d\d-[a-z0-9]+(?:-[a-z0-9]+)+\b')


def scaffold_modes():
    """The MODES table of setup_project.py, read without importing the tool."""
    import ast
    tree = ast.parse(read('tools', 'agent', 'setup_project.py'))
    for node in tree.body:
        if (isinstance(node, ast.Assign) and len(node.targets) == 1
                and getattr(node.targets[0], 'id', None) == 'MODES'):
            return ast.literal_eval(node.value)
    return None


def check_scaffold_parity(report):
    modes = scaffold_modes()
    if not modes:
        report.fail('scaffold-parity', 'setup_project.py has no literal MODES table')
        return
    wanted = (json.loads(read('docs', 'agent', 'api.json') or '{}').get('sdk') or {}).get('fetch_ref')
    recipes = {mode['recipe'] for mode in modes.values()}
    held = 0
    for parts in SCAFFOLD_SCRIPTS:
        name = '/'.join(parts)
        text = read(*parts)
        if not text:
            report.fail('scaffold-parity', '{} is absent'.format(name))
            continue
        for mode, spec in sorted(modes.items()):
            if "'{}'".format(mode) not in text and '{})'.format(mode) not in text:
                report.fail('scaffold-parity', '{} has no mode "{}"'.format(name, mode))
            for old, new in spec['tokens']:
                if not re.search(r'(?<![\w{{}}]){}(?![\w{{}}])'.format(re.escape('{}={}'.format(old, new))), text):
                    report.fail('scaffold-parity', '{} does not rename {} to {} in mode "{}"'
                                .format(name, old, new, mode))
        named = set(RECIPE_NAME_RE.findall(text))
        if named != recipes:
            report.fail('scaffold-parity', '{} copies recipes {}; setup_project.py copies {}'
                        .format(name, sorted(named), sorted(recipes)))
        fallback = SCRIPT_FALLBACK_RE.search(text)
        if not fallback or fallback.group(1) != wanted:
            report.fail('scaffold-parity', '{} falls back to "{}"; api.json states '
                        'sdk.fetch_ref is "{}"'.format(name, fallback.group(1) if fallback else None, wanted))
        held += 1
    if 'setup-project.ps1' not in read('tools', 'setup-project.bat'):
        report.fail('scaffold-parity', 'tools/setup-project.bat does not run setup-project.ps1')
    report.ok('scaffold-parity', '{} shell scaffolders name the {} modes of setup_project.py'
              .format(held, len(modes)))


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


def stated_numbers():
    """Each documented measure, as (file, the sentence it must contain, source)."""
    claims = [
        ('docs/ai-readiness.md',
         '`AGENTS.md` at or below {:.0f} KB'.format(ENTRY_TARGET / KB),
         'ENTRY_TARGET in this file'),
        ('docs/ai-readiness.md',
         'allowed {:.1f} KB'.format(ENTRY_ALLOWED / KB),
         'ENTRY_ALLOWED in this file'),
        ('docs/ai-readiness.md',
         'Every page in `docs/agent/` at or below {:.0f} KB'.format(PAGE_CEILING / KB),
         'PAGE_CEILING in this file'),
        ('docs/ai-readiness.md',
         'No page, and no `.budgets` entry, above {:.1f} KB'
         .format(PAGE_HARD_CEILING / KB),
         'PAGE_HARD_CEILING in this file'),
        ('docs/agent/.budgets',
         'a second, hard ceiling of {:.0f} KB'.format(PAGE_HARD_CEILING / KB),
         'PAGE_HARD_CEILING in this file'),
        ('docs/ai-readiness.md',
         'at or below {:.1f} KB'.format(CORPUS_CEILING / KB),
         'CORPUS_CEILING in this file'),
        ('docs/ai-readiness.md',
         'Under {:.0f}% of 12-word runs'.format(DUPLICATION_TARGET * 100),
         'DUPLICATION_TARGET in this file'),
        ('docs/ai-readiness.md',
         '{} gates run on every change'.format(len(CI_GATES)),
         'CI_GATES in this file'),
    ]
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

    # A file that runs nothing of its own is a module another tool imports, not a
    # script an agent has to rule out, so the pages have no reason to name it.
    modules = set()
    for folder, folders, files in os.walk(os.path.join(ROOT, 'tools')):
        folders[:] = [d for d in folders if d not in ('schema', '__pycache__')]
        for name in files:
            if not name.endswith('.py'):
                continue
            body = read(os.path.relpath(os.path.join(folder, name), ROOT))
            if body and "__name__ == '__main__'" not in body:
                modules.add(name)

    wrong = 0
    for tool in sorted(set(tools) - modules):
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
        report.ok('shipped', '{} tool(s) and {} module(s) checked: the installed set '
                  'is the set the pages name'
                  .format(len(set(tools) - modules), len(modules)))


def check_entry_toll(report):
    entry = size('AGENTS.md')
    if entry > ENTRY_ALLOWED:
        report.fail('entry-toll', 'AGENTS.md is {:.1f} KB, over the {:.1f} KB it is '
                    'allowed. Every agent pays this on every task: take a fact out to '
                    'the page or the tool that states it, or raise ENTRY_ALLOWED and '
                    'say in the commit what is load-bearing'
                    .format(entry / KB, ENTRY_ALLOWED / KB))
    elif entry > ENTRY_TARGET:
        report.note('entry-toll', 'AGENTS.md is {:.1f} KB, {:.0f} bytes over the '
                    '{:.0f} KB target and within the {:.1f} KB recorded allowance'
                    .format(entry / KB, entry - ENTRY_TARGET, ENTRY_TARGET / KB,
                            ENTRY_ALLOWED / KB))
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


# An exception granted once and never looked at again is the failure mode of every
# waiver mechanism: the NOTE it prints on every run becomes furniture. Each entry of
# .budgets carries the date it was last argued, and one this old is reported.
REVIEW_DAYS = 120

REVIEWED = re.compile(r'^\s*([\w.\-]+)\s*=.*#.*\breviewed\s+(\d{4}-\d{2}-\d{2})\b')


def budget_reviews():
    """When each recorded exception was last argued: {page: 'YYYY-MM-DD'}."""
    dates = {}
    for line in read('docs', 'agent', '.budgets').splitlines():
        found = REVIEWED.match(line)
        if found:
            dates[found.group(1)] = found.group(2)
    return dates


def check_budget_review(report):
    """Every recorded exception says when it was last argued, and how long ago.

    The ratchet only turns one way while nothing asks whether an exception is still
    earning its bytes. This does not judge the reason -- it reports the age, so a
    waiver that has stopped being argued is visible next to the page it excuses.
    """
    allowed = page_budgets()
    if not allowed:
        report.ok('budget-review', 'no recorded exception to review')
        return
    dates = budget_reviews()
    today = datetime.date.today()
    stale = []
    for page in sorted(allowed):
        when = dates.get(page)
        if when is None:
            report.fail('budget-review', '.budgets excuses {} and does not say when '
                        'that was last argued. Add "reviewed YYYY-MM-DD" to its reason'
                        .format(page))
            continue
        try:
            age = (today - datetime.date(*[int(part) for part in when.split('-')])).days
        except ValueError:
            report.fail('budget-review', '.budgets gives {} the review date "{}", '
                        'which is not a date'.format(page, when))
            continue
        if age > REVIEW_DAYS:
            stale.append((page, when, age))
    for page, when, age in stale:
        report.note('budget-review', '{} has held its exception unreviewed for {} '
                    'day(s), since {}. Argue it again or shrink the page'
                    .format(page, age, when))
    report.ok('budget-review', '{} exception(s) recorded, {} reviewed within {} days'
              .format(len(allowed), len(allowed) - len(stale), REVIEW_DAYS))


# The pages every run opens before it has done anything: the entry document, the runbook
# it routes to, and the project's own AGENTS.md that setup_project.py writes. .budgets
# caps each page on its own and nothing caps the sum, which is what a run actually pays.
ENTRY_PAGES = ('AGENTS.md', 'docs/agent/01-runbook.md')
# The generated file interpolates the SDK path, so its size moves with the checkout.
# A fixed stand-in makes the measurement the same on every machine.
ENTRY_SDK_ROOT = '/opt/areg-sdk'
ENTRY_KEY = 'entry-path'
# The runbook row that sends a run to the project's own AGENTS.md, which then joins
# the entry path.
ENTRY_PROJECT_READ = re.compile(r'^\| `AGENTS\.md` \|[^\n]*\bread\b', re.I | re.M)


def entry_path_bytes():
    """The entry path in bytes, and what each part of it costs."""
    import tempfile, shutil
    parts = [(page, size(*page.split('/'))) for page in ENTRY_PAGES]
    with open(os.path.join(ROOT, 'docs', 'agent', '01-runbook.md'), encoding='utf-8') as page:
        if not ENTRY_PROJECT_READ.search(page.read()):
            return parts
    folder = tempfile.mkdtemp()
    try:
        sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
        import setup_project
        setup_project.write_agents(folder, 'entrycheck', 'ipc', ENTRY_SDK_ROOT,
                                   ['entrycheck'])
        made = os.path.join(folder, 'AGENTS.md')
        parts.append(("the project's own AGENTS.md", os.path.getsize(made)))
    except Exception as why:                       # noqa: BLE001 -- reported, not raised
        parts.append(('the project AGENTS.md could not be rendered: %s' % why, 0))
    finally:
        shutil.rmtree(folder, ignore_errors=True)
    return parts


def check_entry_path(report):
    """The sum of the pages a run reads before it can start, against its recorded cap.

    Every page here is inside its own .budgets exception and the total is still the
    largest single thing a run buys. A per-page cap cannot see it: three pages each a
    little under their own limit is a bigger entry path than one page over it.
    """
    allowed = page_budgets().get(ENTRY_KEY)
    parts = entry_path_bytes()
    total = sum(n for _, n in parts)
    detail = ', '.join('%s %d' % (name.split('/')[-1], n) for name, n in parts)
    if allowed is None:
        report.fail(ENTRY_KEY, 'the entry path is {} B ({}) and docs/agent/.budgets '
                    'records no "{} = <bytes>" cap for it'
                    .format(total, detail, ENTRY_KEY))
    elif total > allowed:
        report.fail(ENTRY_KEY, 'the entry path is {} B, over its recorded {} B by {}. '
                    'It is {}. Every run pays all of it before it does anything'
                    .format(total, allowed, total - allowed, detail))
    else:
        report.ok(ENTRY_KEY, 'the entry path is {} B against {} B recorded ({})'
                  .format(total, allowed, detail))


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
        if bytes_ > PAGE_HARD_CEILING:
            report.fail('budget', 'docs/agent/{} is {:.1f} KB, over the {:.0f} KB hard '
                        'ceiling that no .budgets entry may raise. Split it, or move '
                        'what a tool can answer into the schema'
                        .format(page, bytes_ / KB, PAGE_HARD_CEILING / KB))
        elif bytes_ <= allowed.get(page, 0):
            report.note('budget', 'docs/agent/{} is {:.1f} KB, over the ceiling by '
                        'recorded exception'.format(page, bytes_ / KB))
        else:
            report.fail('budget', 'docs/agent/{} is {:.1f} KB, over the {:.0f} KB '
                        'ceiling and not recorded in .budgets'
                        .format(page, bytes_ / KB, PAGE_CEILING / KB))

    # An exception that names nothing, or excuses a page that needs no excuse, is
    # a stale entry: it would quietly cover a page that grew into it later. The
    # entry-path cap is not a page and is checked by check_entry_path instead.
    for page in sorted(allowed):
        if page == ENTRY_KEY:
            continue
        if page not in pages:
            report.fail('budget', '.budgets excuses {}, which does not exist'
                        .format(page))
        elif size('docs', 'agent', page) <= PAGE_CEILING:
            report.fail('budget', '.budgets excuses {}, which needs no exception'
                        .format(page))
        elif allowed[page] > PAGE_HARD_CEILING:
            report.fail('budget', '.budgets allows {} {:.1f} KB, above the {:.0f} KB '
                        'hard ceiling. An exception raises the ceiling for one page; '
                        'it cannot pass this one'
                        .format(page, allowed[page] / KB, PAGE_HARD_CEILING / KB))

    # A page a handful of bytes under the ceiling is tuned to the metric, not
    # written to it: the next one-word edit trips CI.
    for page in pages:
        bytes_ = size('docs', 'agent', page)
        if PAGE_CEILING * 0.99 < bytes_ <= PAGE_CEILING:
            report.note('budget', 'docs/agent/{} is {} bytes under the ceiling, too '
                        'close to hold'.format(page, int(PAGE_CEILING - bytes_)))

    # The median page was a target until 2026-09-14 and is not a budget: moving it
    # means shrinking pages that are already inside the ceiling, with no criterion
    # for what to remove. The ceiling and the corpus total bound the same bytes, and
    # a page has grown to put a fact at its point of use more than once.
    report.ok('budget', '{} of {} pages are within the {:.0f} KB ceiling, and every '
              'page and every exception is within the {:.0f} KB hard ceiling'
              .format(len(pages) - len(over), len(pages), PAGE_CEILING / KB,
                      PAGE_HARD_CEILING / KB))

    # One line the trend is read from. Five NOTEs about five pages say nothing about
    # whether the exempted share of the corpus is growing; this does.
    total = sum(size('docs', 'agent', p) for p in pages)
    exempt = sum(bytes_ for _page, bytes_ in over)
    report.note('budget', 'pages total {:.1f} KB, of which {:.1f} KB ({:.0f}%) sits in '
                '{} page(s) over the ceiling'
                .format(total / KB, exempt / KB,
                        100.0 * exempt / total if total else 0, len(over)))
    if over:
        largest, bytes_ = max(over, key=lambda entry: entry[1])
        room = PAGE_HARD_CEILING - bytes_
        report.note('budget', 'the largest page is docs/agent/{} at {:.1f} KB, {:.1f} KB '
                    '{} the {:.0f} KB hard ceiling'
                    .format(largest, bytes_ / KB, abs(room) / KB,
                            'under' if room >= 0 else 'over', PAGE_HARD_CEILING / KB))


def corpus_files():
    """Every document an agent building on areg reads from, largest first."""
    found = [('AGENTS.md', size('AGENTS.md'))]
    for page in agent_pages():
        found.append(('docs/agent/' + page, size('docs', 'agent', page)))
    return sorted(found, key=lambda entry: -entry[1])


def check_corpus_toll(report):
    """The whole reading corpus against its ceiling.

    A page budget bounds one page and says nothing about how many pages there are.
    This bounds the set, so an addition has to be paid for by a removal.
    """
    files = corpus_files()
    total = sum(bytes_ for _, bytes_ in files)
    if total > CORPUS_CEILING:
        report.fail('corpus-toll',
                    'the reading corpus is {:.1f} KB over its {:.0f} KB ceiling '
                    '({} files, {} bytes). Pay for the addition with a removal, or '
                    'raise CORPUS_CEILING in this file and say in the commit what the '
                    'bytes bought. Largest: {}'
                    .format((total - CORPUS_CEILING) / KB, CORPUS_CEILING / KB,
                            len(files), total,
                            ', '.join('{} {:.1f} KB'.format(name, n / KB)
                                      for name, n in files[:3])))
    else:
        report.ok('corpus-toll',
                  'the reading corpus is {:.1f} KB, {:.1f} KB under its {:.0f} KB '
                  'ceiling'.format(total / KB, (CORPUS_CEILING - total) / KB,
                                   CORPUS_CEILING / KB))


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
# Nothing tracked points at the local session tree
#
# .claude/ is machine-local and git-ignored. A tracked document that names a path
# inside it reads as an instruction, and it resolves on the machine that wrote it,
# so nobody there sees it break. It does not resolve from a clean clone, which is
# the only tree an application author or CI ever has.
#
# Documents only. The tools that create and read the tree name it because that is
# what they do, and each one works when it is absent. A home path (~/.claude/,
# $HOME/.claude/) is Claude Code's own directory, not this tree.
# ---------------------------------------------------------------------------
LOCAL_TREE_RE = re.compile(r'(?<![\w.-])(?<!~/)(?<!HOME/)\.claude/')


def tracked_files():
    """Every file git tracks, as repository-relative paths, or [] without git."""
    try:
        result = subprocess.run(['git', 'ls-files', '-z'], cwd=ROOT,
                                stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
                                stdin=subprocess.DEVNULL)
    except OSError:
        return []
    if result.returncode != 0:
        return []
    return [name for name in result.stdout.decode('utf-8', 'replace').split('\0') if name]


def check_local_tree_references(report):
    names = [name for name in tracked_files()
             if name.endswith('.md') or name.endswith('.txt') or
             (name.startswith('docs/agent/.') and '/' not in name[len('docs/agent/'):])]
    if not names:
        report.note('local-tree', 'git does not list the tracked files here, so no '
                    'document was read for a path into the local session tree')
        return

    hits = []
    for name in names:
        path = os.path.join(ROOT, name)
        if not os.path.isfile(path) or os.path.getsize(path) > 1024 * 1024:
            continue
        try:
            with open(path, encoding='utf-8') as handle:
                lines = handle.read().splitlines()
        except (UnicodeDecodeError, OSError):
            continue
        for number, line in enumerate(lines, 1):
            if LOCAL_TREE_RE.search(line):
                hits.append((name, number))

    for name, number in hits:
        report.fail('local-tree', '{}:{} names a path inside .claude/, which is '
                    'git-ignored and exists only on the machine that wrote it. A '
                    'clean clone and CI do not have it, so the instruction is dead '
                    'for every other reader'.format(name, number))
    if not hits:
        report.ok('local-tree', '{} tracked document(s) name no path inside .claude/'
                  .format(len(names)))


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
    scripts = (read('tools', 'intern', 'check_recipes.py')
               + read('tools', 'agent', 'run_scenarios.py')
               + read('tools', 'intern', 'check_observability.py')
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


# A command the corpus tells an agent to run that does not exist on Windows, and the
# token whose presence on the same page shows the Windows form is given beside it. A
# page-level "does it mention Windows" test does not catch these: the pages that carry
# them are the ones most full of Windows notes.
POSIX_ONLY = (('ss', 'netstat'),
              ('lsof', 'netstat'),
              ('pkill', 'taskkill'),
              ('killall', 'taskkill'),
              ('uname', 'ver'),
              ('which', 'where'))

# A command word starts a line or follows a pipe, a semicolon, an && or a $( .
COMMAND_POSITION = r'(?:^|[|;&(]\s*|\$\(\s*)'


# The two long-lived services, and the shell each form belongs to. A POSIX shell
# redirects stdin, and console mode reads stdin and treats end of input as --quit,
# so a console service there binds its port and drops it again within a second. On
# Windows --service is the Service Control Manager and returns at once from a
# command line, so console mode in a window of its own is the only form that runs.
SERVICES = ('mtrouter', 'logcollector')


# A line whose command word is one of the services, with or without a path, a
# platform suffix and a Windows "start" prefix in front of it.
LAUNCH_RE = re.compile(r'^(?:start\s+"[^"]*"\s+)?[\w./\\$-]*?\b(?:'
                       + '|'.join(SERVICES) + r')(?:\.elf|\.exe|\.mac)?(?:\s|$)')


def launch_lines(text):
    """Every fenced line that starts a service, as (shell, line)."""
    found = []
    for shell, body in re.findall(r'```(bash|sh|shell|bat|cmd)\s*\n(.*?)```', text, re.S):
        for line in body.splitlines():
            line = line.split('#', 1)[0].strip()
            if line and LAUNCH_RE.match(line):
                found.append((shell, line))
    return found


def check_service_launch(report):
    """Every documented service launch is the form that works in its own shell."""
    pages = ['AGENTS.md'] + ['docs/agent/' + p for p in agent_pages()] \
        + ['docs/agent/recipes/README.md', 'tools/agent/setup_project.py']
    checked = 0
    bad = 0
    for page in pages:
        text = read(*page.split('/'))
        for shell, line in launch_lines(text):
            checked += 1
            posix = shell in ('bash', 'sh', 'shell')
            if posix and '--service' not in line:
                bad += 1
                report.fail('launch', '{}: "{}" starts a service in console mode from '
                            'a POSIX shell, where end of input is --quit: it binds its '
                            'port and drops it again, exit 0'.format(page, line))
            elif not posix and '--service' in line:
                bad += 1
                report.fail('launch', '{}: "{}" passes --service on Windows, where it '
                            'means the Service Control Manager and returns at once '
                            'having started nothing'.format(page, line))
            elif not posix and not line.startswith('start '):
                bad += 1
                report.fail('launch', '{}: "{}" starts a service on Windows without '
                            '\'start ""\', so it has no console of its own and the '
                            'sequence never reaches the next line'.format(page, line))
    for page in pages:
        if re.search(r'never binds 8181', read(*page.split('/'))):
            bad += 1
            report.fail('launch', '{} says a console router never binds 8181. It does '
                        'bind it; what ends it is end of input on stdin'.format(page))
    if not bad:
        report.ok('launch', '{} documented service launch(es), each the form its own '
                  'shell needs'.format(checked))


def check_posix_only(report):
    """Every POSIX-only command in a fenced block names its Windows form on the page."""
    pages = ['AGENTS.md'] + ['docs/agent/' + p for p in agent_pages()]
    found = 0
    bad = 0
    for page in pages:
        text = read(*page.split('/'))
        blocks = '\n'.join(re.findall(r'```.*?\n(.*?)```', text, re.S))
        for command, windows in POSIX_ONLY:
            if not re.search(COMMAND_POSITION + re.escape(command) + r'\b',
                             blocks, re.M):
                continue
            found += 1
            if windows not in text:
                bad += 1
                report.fail('portability',
                            '{} runs "{}", which does not exist on Windows, and the '
                            'page never names "{}". Give the Windows form beside the '
                            'command, not in a substitution list elsewhere'
                            .format(page, command, windows))
    report.ok('portability',
              '{} POSIX-only command(s) in the corpus, {} without a Windows form'
              .format(found, bad))


# Sourcing a process substitution reads nothing under bash 3.2: _evalfile sizes the
# read from fstat().st_size, which is 0 for a pipe, so the shell sources an empty
# string and reports success. The macOS runner image ships bash 3.2.57 and no other
# bash, so a step written this way sets no variable, prints no error, and fails on
# the assertion that follows it. Bash 4.0 and later read such a file to the end.
SOURCED_PIPE = re.compile(r'(?:\bsource|(?<![\w.])\.)\s+<\(')


def check_workflow_shell(report):
    """No workflow sources a process substitution, which bash 3.2 reads as empty."""
    folder = os.path.join(ROOT, '.github', 'workflows')
    if not os.path.isdir(folder):
        report.fail('portability', 'there are no workflows to check')
        return

    files = 0
    offenders = 0
    for name in sorted(os.listdir(folder)):
        if not name.endswith(('.yml', '.yaml')):
            continue
        files += 1
        for number, line in enumerate(read('.github', 'workflows', name).splitlines(), 1):
            if line.strip().startswith('#') or not SOURCED_PIPE.search(line):
                continue
            offenders += 1
            report.fail('portability',
                        '.github/workflows/{}:{} sources a process substitution. The '
                        'macOS runner has only bash 3.2, which reads 0 bytes from a '
                        'pipe and sources nothing, so the step is silently a no-op. '
                        'Write the text to a file and source that'
                        .format(name, number))
    if not offenders:
        report.ok('portability',
                  '{} workflow(s) source no process substitution'.format(files))


# A framework name is answered by api_help.py, which reads the public headers and
# prints the declaration and the header carrying it. A page that routes the lookup to
# grep or to a header instead sends the agent into a 155 KB tree, and a curated page
# read as exhaustive is how a name that does exist is reported as absent. The two
# routes may be named only to forbid them.
NAME_ROUTES = [
    (re.compile(r'grep\s+(?:for|the|a)\b', re.I), 'grep'),
    (re.compile(r'\b(?:read|open|consult)\s+the\s+headers?\b', re.I), 'the header'),
    (re.compile(r'\|\s*The headers?\.'), 'a routing row whose target is the header'),
]
DENIAL = re.compile(r'\bnever\b|\bnot\b|\brather than\b|\binstead of\b', re.I)


def clause_around(line, start, end):
    """The clause a match sits in, so a denial elsewhere on the line does not count.

    "Never write an areg name from memory; grep for it" carries a denial and a route,
    and the denial governs only the clause it is in.
    """
    left = max([line.rfind(mark, 0, start) + len(mark)
                for mark in ('. ', '; ', ' -- ', '|', ':')] + [0])
    right = min([pos for pos in (line.find(mark, end)
                                 for mark in ('. ', '; ', ' -- ', '|'))
                 if pos != -1] + [len(line)])
    return line[left:right]


def check_name_lookup(report):
    """No page routes a framework-name lookup to grep or to a header."""
    pages = ['AGENTS.md', 'docs/agent/api.json'] + ['docs/agent/' + p
                                                    for p in agent_pages()]
    offenders = 0
    for page in pages:
        for number, line in enumerate(read(*page.split('/')).splitlines(), 1):
            for pattern, what in NAME_ROUTES:
                for found in pattern.finditer(line):
                    clause = clause_around(line, found.start(), found.end())
                    if DENIAL.search(clause):
                        continue
                    offenders += 1
                    report.fail('name lookup',
                                '{}:{} routes a name lookup to {}. api_help.py is the '
                                'route; name grep or a header only to forbid it'
                                .format(page, number, what))
    if not offenders:
        report.ok('name lookup', '{} pages route every name lookup to api_help.py'
                  .format(len(pages)))


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

    check_posix_only(report)
    check_workflow_shell(report)

    if 'windows-' not in agent_workflow():
        report.fail('portability', 'the agent workflow has no Windows runner, so '
                    'half the documented commands are never executed')


# ---------------------------------------------------------------------------
def run():
    report = Report()
    features = resolve_features()
    check_coverage(report, features)
    check_schema_features(report)
    check_generator_catalogue(report)
    check_rule_texts(report)
    check_truth(report)
    check_verification(report)
    check_prohibitions(report)
    check_prescribed_calls(report)
    check_budget_review(report)
    check_entry_path(report)
    check_reachable(report)
    check_includes(report)
    check_member_shapes(report)
    check_sdk_paths(report)
    check_fetch_ref(report)
    check_scaffold_parity(report)
    check_data_types(report)
    check_stated_numbers(report)
    check_shipped_tools(report)
    check_example_type_placement(report)
    check_spec_value_prefixes(report)
    check_step_enum_values(report)
    check_worksheet_names_update_order(report)
    check_spec_refuses_a_nested_response(report)
    check_self_helper_is_described(report)
    check_step_hold(report)
    check_api_constructors(report)
    check_api_class_complete(report)
    check_review_verdicts(report)
    check_entry_toll(report)
    check_page_budget(report)
    check_corpus_toll(report)
    check_duplication(report)
    check_generated_code(report)
    check_local_tree_references(report)
    check_ci(report)
    check_tools(report)
    check_observability(report)
    check_portability(report)
    check_name_lookup(report)
    check_service_launch(report)
    check_project_routing(report)
    check_member_inventory(report)
    check_scenario_runner(report)
    check_mutation_verdicts(report)
    check_trigger_coverage(report)
    check_state_mirrors(report)
    check_placeholder_contract(report)
    check_design_template(report)
    check_peer_loss_branch(report)
    check_generated_defects(report)
    check_step_driver(report)
    check_late_arrival(report)
    check_self_transition(report)
    check_step_fall_through(report)
    check_step_values_split(report)
    check_example_until(report)
    check_stall_default(report)
    check_stall_names_latest_drops(report)
    check_step_enum_qualifier(report)
    check_spec_value_shapes(report)
    check_unused_parameters(report)
    check_hosted_machine(report)
    check_hosted_path(report)
    check_hosted_counter(report)
    check_empty_answer(report)
    check_installed_scaffold(report)
    check_method_names(report)
    check_accessor_collision(report)
    check_spec_semantics(report)
    check_await_spelling(report)
    check_start_kind(report)
    check_design_request(report)
    check_step_rules_at_use(report)
    check_machine_attribute_note(report)
    check_benchmark_vocabulary(report)
    check_phase_by_one_action(report)
    check_base_api_on_demand(report)
    check_codegen_name_rules(report)
    check_app_shape(report)
    check_update_note(report)
    check_scaffold_table(report)
    check_final_entry_rule(report)
    check_example_size(report)
    check_worksheet_order_note(report)
    check_empty_section_note(report)
    check_command_coverage(report)
    check_worksheet_rewrite(report)
    check_section_named_again(report)
    check_fix_file(report)
    check_unread_attribute_note(report)
    check_failure_names_the_error(report)
    check_passing_step_keeps_a_warning(report)
    check_names_carry_signatures(report)
    check_step_output_whole(report)
    check_design_reviewable(report)
    check_codegen_plain_error(report)
    check_generated_prefix(report)
    check_example_machine(report)
    check_example_programs(report)
    check_driver_client_early(report)
    check_removed_marker_body(report)
    check_answer_file(report)
    check_peer_lost_scenario(report)
    check_stepped_evidence(report)
    check_shared_request_action(report)
    check_provider_timers(report)
    check_scaffold_routing(report)
    check_build_routes_next(report)
    check_change_note_route(report)
    check_advice_flags_accepted(report)
    check_passing_output_kept(report)
    check_base_api_parity(report)
    check_errors_follow_output(report)
    check_regeneration_report(report)
    check_regeneration_idempotent(report)
    check_marker_spelling(report)
    check_worksheet_contract(report)
    check_contract_symmetry(report)
    check_task_prompt_neutrality(report)
    check_task_shape(report)
    check_analyzer_keys(report)
    check_grpc_isolation(report)
    check_hidden_probes(report)
    check_runner_parity(report)
    check_fix_bound(report)
    return report


# A project made by setup_project.py carries its own routing table, and that table
# is the only one its agent ever sees: AGENTS.md section 2 lives in the SDK, which
# the project is told not to search. A page routed here but missing there is
# unreachable from inside a real project -- the agent either guesses or lists
# docs/agent/ by hand, which is what the runbook forbids. Pages that genuinely do
# not apply to an already-created project outside the SDK are named below.
PROJECT_ROUTING_EXEMPT = {
    'docs/agent/10-new-project.md',   # the project exists; setup_project.py made it
    'docs/agent/41-examples.md',      # in-tree SDK examples, not an outside project
    'docs/agent/35-sqlog.md',         # reached from 34-logging.md
    'docs/agent/36-config.md',        # reached from 50-running.md
}



# docs/agent/members.json is what check_contract.py rule B-08 tests a call against.
# It is generated from the public headers, so a framework rename that is not
# regenerated turns the rule into a source of false findings.

# run_scenarios.py is what turns "it built" into "it is proven to work", so a defect
# in it is invisible: every project it runs fails the same way at once.
def check_scenario_runner(report):
    runner = os.path.join(ROOT, 'tools', 'agent', 'run_scenarios.py')
    if not os.path.isfile(runner):
        report.fail('runner', 'tools/agent/run_scenarios.py is missing')
        return
    result = subprocess.run([sys.executable, runner, '--self-test'],
                            capture_output=True, text=True, cwd=ROOT,
                            stdin=subprocess.DEVNULL)
    text = (result.stdout + result.stderr).strip()
    if result.returncode != 0:
        report.fail('runner', text.splitlines()[0] if text else
                    'run_scenarios.py --self-test failed')
        return
    report.ok('runner', text.splitlines()[0] if text else 'run_scenarios.py self-test passed')


def check_mutation_verdicts(report):
    """The verdicts of the defects that need a build are checked without one.

    by_runtime decides whether a documented diagnostic fired, and only --lib runs it,
    so a verdict that accepts the wrong observation is invisible in an ordinary run.
    """
    bank = os.path.join(ROOT, 'tools', 'intern', 'check_mutations.py')
    if not os.path.isfile(bank):
        report.fail('verdicts', 'tools/intern/check_mutations.py is missing')
        return
    result = subprocess.run([sys.executable, bank, '--self-test'],
                            capture_output=True, text=True, cwd=ROOT,
                            stdin=subprocess.DEVNULL)
    text = (result.stdout + result.stderr).strip()
    if result.returncode != 0:
        report.fail('verdicts', 'check_mutations.py --self-test: ' +
                    (text.splitlines()[0] if text else 'failed'))
        return
    report.ok('verdicts', text.splitlines()[-1] if text else 'the verdicts self-test')


def check_member_inventory(report):
    generator = os.path.join(ROOT, 'tools', 'intern', 'build_members.py')
    if not os.path.isfile(generator):
        report.fail('inventory', 'tools/intern/build_members.py is missing, so nothing '
                               'keeps docs/agent/members.json honest')
        return
    result = subprocess.run([sys.executable, generator, '--check'],
                            capture_output=True, text=True, cwd=ROOT)
    if result.returncode != 0:
        report.fail('inventory', (result.stderr or result.stdout).strip().splitlines()[-1]
                    if (result.stderr or result.stdout).strip()
                    else 'docs/agent/members.json does not match framework/areg')
        return
    report.ok('inventory', result.stdout.strip() or
              'docs/agent/members.json matches the public headers')


def check_spec_value_prefixes(report):
    """A spec value prefix with nothing after the colon is a literal or a refusal.

    "lit:" is the empty value. "param:", "attr:", "const:", "raw:" and "expr:" name
    something, so an empty one is refused. "raw:" and "expr:" are one prefix, in a
    "set" value as in a guard. Before this was checked, they all fell
    through to the verbatim branch and the generated code carried the text "lit:"
    while every step exited 0.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:
        report.fail('spec-prefixes', 'gen_docs.py does not import: {}'.format(failure))
        return

    class Machine(object):
        params_of = {}
        attribute_names = ()
        constant_names = ()

    kind, text = gen_docs.source_of(Machine(), 'lit:', None, 'a test')
    if (kind, text) != ('Value', ''):
        report.fail('spec-prefixes',
                    '"lit:" is written as {} "{}", not the empty value'.format(kind, text))
        return
    refused = []
    quiet, sys.stderr = sys.stderr, io.StringIO()
    try:
        for prefix in ('param', 'attr', 'const', 'expr', 'raw'):
            try:
                gen_docs.source_of(Machine(), prefix + ':', None, 'a test')
            except SystemExit:
                continue
            refused.append(prefix)
    finally:
        sys.stderr = quiet
    if refused:
        report.fail('spec-prefixes',
                    '"{}:" with nothing after the colon is accepted and written '
                    'verbatim'.format('", "'.join(refused)))
        return
    for prefix in ('raw', 'expr'):
        kind, text = gen_docs.source_of(Machine(), prefix + ':mAttrTries + 1', None, 'a test')
        if (kind, text) != ('Expression', 'mAttrTries + 1'):
            report.fail('spec-prefixes',
                        '"{}:<c++>" in a set value is written as {} "{}", not verbatim C++'
                        .format(prefix, kind, text))
            return
    report.ok('spec-prefixes',
              '"lit:" is the empty value; an empty param/attr/const/raw/expr is refused; '
              'raw: and expr: are verbatim C++ in a set value')


def check_step_enum_values(report):
    """A step argument naming a field of an enumeration is qualified with its type.

    A bare field name does not compile, and the generator used to write one and exit
    0, so the design was answered by a C++ error naming the generated call site. One
    measured run spent four requests qualifying them by hand.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_skeleton
    except Exception as failure:
        report.fail('step-enums', 'gen_skeleton.py does not import: {}'.format(failure))
        return

    class Iface(object):
        enum_fields = {'Speed': ['Slow', 'Fast']}

        def cpp_type(self, name):
            return 'Shared::' + name, False

    try:
        spelt = gen_skeleton.cpp_value('Fast', 'Speed', Iface(), 'a test')
    except TypeError as failure:
        report.fail('step-enums',
                    'cpp_value does not take the type and the document it is '
                    'resolved against: {}'.format(failure))
        return
    if spelt != 'Shared::Speed::Fast':
        report.fail('step-enums',
                    'a field of an enumeration is written "{}", which no C++ scope '
                    'declares'.format(spelt))
        return
    quiet, sys.stderr = sys.stderr, io.StringIO()
    try:
        gen_skeleton.cpp_value('Warp', 'Speed', Iface(), 'a test')
    except SystemExit:
        pass
    else:
        sys.stderr = quiet
        report.fail('step-enums',
                    'a name the enumeration has no field of is accepted and written '
                    'verbatim')
        return
    finally:
        sys.stderr = quiet
    quiet, sys.stderr = sys.stderr, io.StringIO()
    try:
        partial = gen_skeleton.cpp_value('Speed::Fast', 'Speed', Iface(), 'a test')
    except SystemExit:
        partial = None
    finally:
        sys.stderr = quiet
    if partial != 'Shared::Speed::Fast':
        report.fail('step-enums',
                    'a field qualified with its own enumeration, "Speed::Fast", is '
                    'refused although it names the type the parameter takes. The '
                    'bare name and the fully qualified one are both accepted, so '
                    'the one spelling a C++ programmer writes is the only one that '
                    'fails')
        return
    quiet, sys.stderr = sys.stderr, io.StringIO()
    try:
        gen_skeleton.cpp_value('Other::Fast', 'Speed', Iface(), 'a test')
    except SystemExit:
        pass
    else:
        sys.stderr = quiet
        report.fail('step-enums',
                    'a field qualified with a different enumeration is accepted, so '
                    'naming the wrong type is written through in silence')
        return
    finally:
        sys.stderr = quiet
    report.ok('step-enums',
              'a step argument names a field of an enumeration and the generator '
              'qualifies it, bare or already qualified with its own type; an '
              'unknown field and a different type are refused')


def check_self_helper_is_described(report):
    """Every member the generated header declares carries a description, self() too.

    The worksheet prints each member of the component under its "//!<" line, so a
    member without one is listed bare among members that say what they are for. One
    measured run read self() as a handle to call methods through and wrote "self()->"
    twenty-five times, which cost a build and a fix.
    """
    bare = []
    for name in ('gen_skeleton.py',):
        path = os.path.join(ROOT, 'tools', 'agent', name)
        lines = io.open(path, encoding='utf-8').read().split('\n')
        for number, line in enumerate(lines):
            if '& self()' not in line:
                continue
            above = lines[number - 1] if number else ''
            if '//!' not in line and '//!' not in above:
                bare.append('{}:{}'.format(name, number + 1))
    if bare:
        report.fail('self-described',
                    'the self() helper is emitted with no "//!<" line above it at {}, '
                    'so the worksheet lists it bare among members that say what they '
                    'are for'.format(', '.join(bare)))
        return
    report.ok('self-described',
              'every self() helper the generator emits carries a description line')


def check_spec_refuses_a_nested_response(report):
    """A request whose "response" is written out in place is refused, not crashed on.

    "response" names a response declared beside the request; the parameters of one
    written in place go under "answer". A spec that nests the object instead used to
    reach a set() and end the run with a Python traceback, which names no key and
    routes to no fix.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:
        report.fail('spec-nested-response',
                    'gen_docs.py does not import: {}'.format(failure))
        return
    spec = {'name': 'Probe', 'category': 'Public', 'description': 'A probe.',
            'requests': [{'name': 'order', 'description': 'Ask.',
                          'response': {'name': 'ordered', 'params': []}}]}
    quiet, sys.stderr = sys.stderr, io.StringIO()
    try:
        gen_docs.build_siml(spec, None, set(), '')
    except SystemExit:
        said = sys.stderr.getvalue()
    except Exception as failure:
        sys.stderr = quiet
        report.fail('spec-nested-response',
                    'a request whose "response" is an object ends the generator with '
                    '{}: {}. An agent is handed a traceback naming no key'
                    .format(type(failure).__name__, failure))
        return
    else:
        sys.stderr = quiet
        report.fail('spec-nested-response',
                    'a request whose "response" is an object is written through')
        return
    finally:
        sys.stderr = quiet
    if 'answer' not in said:
        report.fail('spec-nested-response',
                    'the refusal does not name "answer", the key that takes a '
                    'response written out in place: {}'.format(said.strip()[:160]))
        return
    report.ok('spec-nested-response',
              'a request whose "response" is an object is refused by name, and the '
              'refusal names "answer"')


def check_worksheet_names_update_order(report):
    """The worksheet says what a consumer handler sees of the other attributes.

    Two set_ calls in one function send two updates in that order, so a handler for
    the first reads a stale value for the second. The rule is on
    docs/agent/20-service-interface.md, a page no measured run has opened; the
    worksheet is read by every run, and this is where the bodies are written.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_skeleton
    except Exception as failure:
        report.fail('worksheet-attr-order',
                    'gen_skeleton.py does not import: {}'.format(failure))
        return

    class Iface(object):
        name = 'Probe'
        requests = responses = broadcasts = triggers = actions = conditions = ()
        imported_types = types = constants = ()
        declared = {}

        def __init__(self, attributes):
            self.attributes = attributes

        def spell(self, kind, name, form=None):
            return '{}_{}'.format(form or kind, name)

        def cpp_type(self, kind):
            return kind, False

        def passed_as(self, kind):
            return kind

        def attribute_setter(self, kind, flag):
            return 'const {} &'.format(kind)

        def generated_params(self, kind, name):
            return ''

    class Sends(Iface):
        broadcasts = [('alarm', [])]

    try:
        many = '\n'.join(gen_skeleton.contract_lines(
            Iface([('Level', 'uint32'), ('Phase', 'uint32')]), 'probe.siml'))
        one = '\n'.join(gen_skeleton.contract_lines(
            Iface([('Level', 'uint32')]), 'probe.siml'))
        mixed = '\n'.join(gen_skeleton.contract_lines(
            Sends([('Level', 'uint32')]), 'probe.siml'))
    except Exception as failure:
        report.fail('worksheet-attr-order',
                    'contract_lines does not run on a plain interface: {}'.format(failure))
        return
    if 'set last' not in many:
        report.fail('worksheet-attr-order',
                    'an interface with two attributes lists both setters and says '
                    'nothing about the order their updates reach a consumer in. A '
                    'handler that reads another attribute by its getter then reads a '
                    'value that has not been sent yet, and one measured run spent '
                    'four requests in generated sources finding that out')
        return
    if 'set last' in one:
        report.fail('worksheet-attr-order',
                    'the ordering line is printed for an interface with one '
                    'attribute, which has no order to get wrong')
        return
    if 'set last' not in mixed or 'broadcast_' not in mixed.split('set last')[0]:
        report.fail('worksheet-attr-order',
                    'an interface with an attribute and a broadcast says nothing about '
                    'the order the update and the broadcast reach a consumer in, so a '
                    'check on the update reads a broadcast that has not arrived')
        return
    report.ok('worksheet-attr-order',
              'the worksheet names the order set_, broadcast_ and response_ calls reach a '
              'consumer in, wherever there are two to order')


def check_api_class_complete(report, tools=None):
    """api_help.py answers a class with its operators and what it inherits, and
    Class::member through a public base: a lookup that leaves either out reads as
    an absence."""
    tool = os.path.join(tools or os.path.join(ROOT, 'tools', 'agent'), 'api_help.py')

    def ask(*words):
        done = subprocess.run([sys.executable, tool] + list(words),
                              capture_output=True, text=True)
        return done.returncode, done.stdout

    wanted = (
        (('String', '--class'), r'bool operator == \(const String& other\) const;',
         'String --class lists no operator == of String'),
        (('String', '--class'), r'inherited from areg::StringBase .*\n(?:  .*\n)*?.*\bcompare\b',
         'String --class does not name what it inherits from StringBase'),
        (('Timer', '--class'), r'inherited from areg::TimerBase',
         'Timer --class does not name what it inherits from TimerBase'),
        (('String::compare',), r'areg::StringBase .*inherited by areg::String',
         'String::compare is not answered from the base that declares it'),
    )
    for words, pattern, failure in wanted:
        code, said = ask(*words)
        if code or not re.search(pattern, said):
            report.fail('api-class-complete', failure)
            return
    report.ok('api-class-complete', 'api_help.py lists a class\'s operators and the names it '
                                    'inherits, and answers Class::member through a base')


def check_api_constructors(report):
    """api_help.py lists every explicit constructor a public header declares.

    A constructor it drops is answered as absent, and an agent told a class cannot
    be built from its arguments stops trusting the page that shows it.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import api_help
    except Exception as failure:
        report.fail('api-constructors', 'api_help.py does not import: {}'.format(failure))
        return
    missing, total = [], 0
    for path in sorted(glob.glob(os.path.join(ROOT, 'framework', 'areg', '**', '*.hpp'),
                                 recursive=True)):
        if os.sep + 'private' + os.sep in path:
            continue
        with open(path, encoding='utf-8', errors='ignore') as handle:
            names = set(re.findall(r'^\s+explicit\s+(\w+)\s*\(', handle.read(), re.M))
        if not names:
            continue
        # By signature: a destructor is listed under the class name too.
        listed = ' '.join(member[2] for member in api_help.Header(path, path).members)
        total += len(names)
        missing += ['{}({})'.format(name, os.path.relpath(path, ROOT))
                    for name in sorted(names)
                    if not re.search(r'\bexplicit\s+%s\s*\(' % name, listed)]
    if missing:
        report.fail('api-constructors',
                    'api_help.py does not list {} explicit constructor(s): {}'
                    .format(len(missing), ', '.join(missing[:5])))
        return
    report.ok('api-constructors',
              'api_help.py lists the explicit constructors of all {} classes that '
              'declare one'.format(total))


def check_step_hold(report):
    """A step check that returns early leaves no hold or jump behind it.

    stay() and go_to() take effect in complete(), which a return skips. A handler that
    does not clear them first lets the next arrival that passes the check be swallowed,
    and the run stalls on a step whose check already passed.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_skeleton
    except Exception as failure:
        report.fail('step-hold', 'gen_skeleton.py does not import: {}'.format(failure))
        return
    for kind in ('update', 'response', 'broadcast'):
        step = {'awaits': (kind, 'X'), 'enum': 'Watch', 'name': 'Watch'}
        lines = [line.strip() for line in gen_skeleton.step_dispatch([step], kind, 'X', 4)]
        head = lines[:lines.index('switch (mStep)')] if 'switch (mStep)' in lines else []
        if 'mHeld = false;' not in head or 'mJumped = false;' not in head:
            report.fail('step-hold',
                        'the {} handler does not clear mHeld and mJumped before its step '
                        'switch, so "stay(); return;" swallows the next arrival'
                        .format(kind))
            return
    report.ok('step-hold',
              'every step handler clears a hold or jump an earlier check left by '
              'returning')


# Every line a review prints says one of two things: the document is written and
# nothing has to be done about this, or exactly what is wrong and what to change.
# Nothing may be ambiguous between the two. A run that cannot tell them apart acts
# on the advice: one did, rewrote a design that was already correct, regenerated,
# and paid more than the whole difference between the two heads it was compared on.
REVIEW_SETTLED = ('asks for no change', 'nothing is wrong here')

# The openings a finding names its remedy with. A new note either settles itself
# with one of the phrases above or adds its own imperative here.
REVIEW_ACTION = ('Declare ', 'Give it ', 'take the attribute out', 'name the one ',
                 'await it on the step')


def review_blocks(said):
    """A review's output as one block per finding: the headline and its explanation."""
    found = []
    for line in said.splitlines():
        if line.startswith('  note  ') or line.startswith('  table '):
            found.append([line])
        elif line.startswith('        ') and found:
            found[-1].append(line)
    return [' '.join(block) for block in found]


# One design per note class, each the smallest that earns it. A class that stops
# firing is a hole in this check, so every case names what it must print.
REVIEW_CASES = (
    ('a skipped template entry', {'interfaces': [], 'machines': []}, 2, 'sample entr'),
    ('an attribute no rule reads', {'interfaces': [], 'machines': [{
        'name': 'M', 'attributes': [{'name': 'Count', 'type': 'uint32'}],
        'triggers': [{'name': 'go'}], 'initial': 'Idle',
        'states': [{'name': 'Idle', 'transitions': [{'on': 'go', 'to': 'Busy'}]},
                   {'name': 'Busy', 'transitions': []}]}]},
     0, 'passed to no guard'),
    ('one action on two forwarding triggers', {
        'interfaces': [{'name': 'S',
                        'requests': [{'name': 'open', 'answer': [{'name': 'ok'}]},
                                     {'name': 'close', 'answer': [{'name': 'ok'}]}]}],
        'machines': [{'name': 'M', 'triggers': [{'name': 'open'}, {'name': 'close'}],
                      'actions': [{'name': 'forward'}], 'initial': 'Idle',
                      'states': [{'name': 'Idle', 'transitions': [
                          {'on': 'open', 'to': 'Busy', 'do': ['forward']},
                          {'on': 'close', 'to': 'Busy', 'do': ['forward']}]},
                                 {'name': 'Busy', 'transitions': []}]}]},
     0, 'runs on open, close'),
    ('an attribute that cannot say every state', {
        'datatypes': {'declare': [{'name': 'Phase', 'kind': 'enum',
                                   'values': [{'name': 'Idle'}, {'name': 'Busy'}]}]},
        'interfaces': [{'name': 'S', 'attributes': [
            {'name': 'phase', 'type': 'Phase', 'notify': 'OnChange'}]}],
        'machines': [{'name': 'M', 'triggers': [{'name': 'go'}], 'initial': 'Idle',
                      'states': [{'name': 'Idle',
                                  'transitions': [{'on': 'go', 'to': 'Busy'}]},
                                 {'name': 'Busy',
                                  'transitions': [{'on': 'go', 'to': 'Halted'}]},
                                 {'name': 'Halted', 'transitions': []}]}]},
     0, 'has no value for'),
    ('which states answer each trigger', {'interfaces': [], 'machines': [{
        'name': 'M', 'triggers': [{'name': 'go'}], 'initial': 'Idle',
        'states': [{'name': 'Idle', 'transitions': [{'on': 'go', 'to': 'Busy'}]},
                   {'name': 'Busy', 'transitions': []}]}]},
     0, 'which states answer each trigger'),
    ('two services in one design',
     {'interfaces': [{'name': 'A'}, {'name': 'B'}], 'machines': []},
     0, 'describes 2 services'),
)


def check_review_verdicts(report):
    """No note of a review is ambiguous about whether the design has to change.

    A design that earns a note has usually shipped working software: across fifteen
    measured runs fourteen earned at least one. A note that reads as a defect is
    therefore paid for far more often than it is right.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:
        report.fail('review-verdict', 'gen_docs.py does not import: {}'.format(failure))
        return
    for label, project, skipped, wanted in REVIEW_CASES:
        held = io.StringIO()
        stdout, sys.stdout = sys.stdout, held
        try:
            gen_docs.review(project, skipped)
        except Exception as failure:
            sys.stdout = stdout
            report.fail('review-verdict', 'a review of {} raises {}'
                        .format(label, failure))
            return
        finally:
            sys.stdout = stdout
        said = held.getvalue()
        if wanted not in said:
            report.fail('review-verdict', 'the note on {} no longer fires, so nothing '
                        'here checks its wording'.format(label))
            return
        for block in review_blocks(said):
            settled = any(phrase in block for phrase in REVIEW_SETTLED)
            if not settled and not any(phrase in block for phrase in REVIEW_ACTION):
                report.fail('review-verdict',
                            'the note on {} says neither that the document is written '
                            'nor what to change: "{}"'.format(label, block[:160]))
                return
            # A note that asks the reader a question leaves the design's fate to the
            # answer, so it has to say what the other answer means. Without that, the
            # question alone reads as a defect and the run rewrites the design.
            if '?' in block and not settled:
                report.fail('review-verdict',
                            'the note on {} asks a question and never says that one '
                            'answer leaves the document as written: "{}"'
                            .format(label, block[:160]))
                return
    report.ok('review-verdict',
              'every note of a review over {} design(s) says either that the document '
              'is written or what to change'.format(len(REVIEW_CASES)))


# The task prompts are the comparison itself: the same requirements scored
# against gRPC, ZeroMQ, DDS or areg. A framework name, a tool name or a build command
# in one of them makes the comparison meaningless, and it has cost measured money --
# a superseded three-command verify chain in two of these files was obeyed by every
# run, over the runbook that supersedes it, because the task file is read later and
# is therefore nearer in context.
TASK_PROMPTS = ('prompt-tempalarm.md', 'prompt-coffeemachine.md', 'prompt-atm.md',
                'prompt-atm-fsm.md', 'prompt-printscan.md', 'prompt-elevator.md',
                'prompt-sensorgateway.md', 'prompt-greenhouse.md', 'prompt-washer.md',
                'prompt-orderdesk.md')

# Spellings that can only come from one framework or one operating system.
TASK_PROMPT_LEAKS = ('.siml', '.fsml', '.dtml', 'setup_project.py', 'gen_skeleton.py',
                     'gen_docs.py', 'build_project.py', 'check_contract.py',
                     'run_scenarios.py', 'schema_help.py', 'codegen.jar', 'cmake',
                     'CMakeLists', '$(nproc)', 'nproc', '.elf', 'apt-get', 'brew ',
                     'powershell', '.exe', 'AGENTS.md', '-prompt.txt',
                     'prompt-template')

# Framework and operating system names, matched as whole words.
TASK_PROMPT_WORDS = re.compile(r'\b(areg|grpc|protobuf|zeromq|dds|linux|windows|macos|'
                               r'wsl|posix|bash|powershell)\b', re.I)


def check_task_prompt_neutrality(report):
    """No task prompt names a framework, a tool, a build command or an OS.

    The prompts are the measuring instrument, not the corpus an application agent
    reads. examples/ is optional and is not installed with the SDK, so a tree without
    it is a supported install and not a defect: the check says it did not run rather
    than failing. A prompt missing from a directory that is there is still a defect.
    """
    bench = os.path.join(ROOT, 'examples', 'ai-benchmark')
    if not os.path.isdir(bench):
        report.note('task-neutral',
                    'examples/ai-benchmark/ is not installed, so the task prompts were '
                    'not checked. They measure the corpus rather than belong to it, and '
                    'examples/ is optional')
        return
    missing = [name for name in TASK_PROMPTS
               if not os.path.isfile(os.path.join(bench, name))]
    for name in missing:
        report.fail('task-neutral',
                    'examples/ai-benchmark/{} is missing; README.md offers it'.format(name))
    if missing:
        return
    readme = read('examples', 'ai-benchmark', 'README.md')
    for name in TASK_PROMPTS:
        text = read('examples', 'ai-benchmark', name)
        if name not in readme:
            report.fail('task-neutral',
                        'examples/ai-benchmark/README.md does not name {}'.format(name))
            return
        body = text.lower()
        for leak in TASK_PROMPT_LEAKS:
            if leak.lower() in body:
                report.fail('task-neutral',
                            '{} names "{}". A task prompt is scored against every '
                            'framework, so it carries requirements only: anything '
                            'about how to build belongs beside it, in the wrapper '
                            'for one framework'.format(name, leak))
                return
        word = TASK_PROMPT_WORDS.search(text)
        if word:
            report.fail('task-neutral',
                        '{} names "{}". A task prompt names no framework and no '
                        'operating system'.format(name, word.group(0)))
            return
        if '## The report' not in text:
            report.fail('task-neutral',
                        '{} has no "## The report" section, so a run of it cannot be '
                        'compared with a run of the others'.format(name))
            return
        # Peer loss and the timeout cannot be shown by a run where everything works.
        # Two runs of one measured pair wrote a scenario that killed the other side
        # and one did not, and the one that did not still claimed every item: an
        # acceptance list without an instruction to prove it grades on belief.
        if '### Proving it' not in text:
            report.fail('task-neutral',
                        '{} has no "### Proving it" section. Its checklist asks for '
                        'peer loss and a timeout, which no run where everything works '
                        'can show, so without it a run scores them from belief'
                        .format(name))
            return
    report.ok('task-neutral',
              '{} task prompts name no framework, tool or OS, each ends in a report, '
              'and README.md offers them all'.format(len(TASK_PROMPTS)))


def check_contract_symmetry(report):
    """Every line --contract prints names both sides of the interface.

    A line that names only one side is worse than no line: the tool is consulted
    immediately before the bodies are written, while the page carrying the other
    half was read many requests earlier. One measured run wrote bare method names
    on the consumer because the request line said only what the provider overrides,
    and paid a build-and-fix cycle for it.
    """
    import subprocess, tempfile, shutil, glob
    tools = os.path.join(ROOT, 'tools', 'agent')
    work = tempfile.mkdtemp(prefix='contract-')
    try:
        spec = os.path.join(work, 'spec.json')
        example = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                  '--example'], capture_output=True, text=True, cwd=ROOT)
        if example.returncode != 0:
            report.fail('contract-sides', 'gen_docs.py --example failed')
            return
        with open(spec, 'w', encoding='utf-8') as handle:
            handle.write(example.stdout)
        made = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                               '--spec', spec, '--outdir', work, '--force', '--chained'],
                              capture_output=True, text=True, cwd=ROOT)
        if made.returncode != 0:
            report.fail('contract-sides', 'gen_docs.py refused its own example')
            return
        document = sorted(glob.glob(os.path.join(work, '*.siml')))
        if not document:
            report.fail('contract-sides', 'the example spec wrote no .siml')
            return
        shown = subprocess.run([sys.executable, os.path.join(tools, 'gen_skeleton.py'),
                                '--doc', document[0], '--contract'],
                               capture_output=True, text=True, cwd=ROOT)
        if shown.returncode != 0:
            report.fail('contract-sides', 'gen_skeleton.py --contract failed')
            return
        lonely = []
        for line in shown.stdout.splitlines():
            body = line.strip()
            if not body.startswith(('provider', 'override', 'consumer')):
                continue
            if not ('provider' in body and 'consumer' in body):
                lonely.append(body[:70])
        if lonely:
            report.fail('contract-sides',
                        '--contract names one side only: "{}". An agent reads this '
                        'instead of the page and cannot tell what the other side '
                        'calls'.format(lonely[0]))
            return
        report.ok('contract-sides',
                  'every --contract line names the provider and the consumer')
    finally:
        shutil.rmtree(work, ignore_errors=True)


def check_trigger_coverage(report):
    """gen_docs.py says which states answer each trigger.

    A trigger with no transition in a state does nothing when it is called there:
    codegen accepts the machine, the generated call compiles, and no step reports
    it. One measured run lost a design thought and a whole regenerate-and-rebuild
    cycle to a trigger missing from the state its machine starts in. The note is
    the only thing in the chain that shows the gap, so a change that silences it
    for a composite machine has to fail here.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('trigger-coverage', 'gen_docs.py does not import: {}'.format(failure))
        return

    machine = {
        'name': 'T', 'initial': 'A',
        'triggers': [{'name': 'go'}, {'name': 'stop'}, {'name': 'dead'}],
        'states': [
            {'name': 'A', 'transitions': [{'on': 'go', 'to': 'B'}]},
            {'name': 'B', 'initial': 'B1',
             'transitions': [{'on': 'stop', 'to': 'A'}],
             'states': [{'name': 'B1', 'transitions': [{'on': 'go', 'to': 'B1'}]}]},
        ],
    }
    coverage = dict(gen_docs.trigger_coverage(machine))
    wanted = {'go': ['A*', 'B1'], 'stop': ['B+'], 'dead': []}
    for name, states in sorted(wanted.items()):
        if coverage.get(name) != states:
            report.fail('trigger-coverage',
                        'gen_docs.py reports trigger "{}" answered by {}, not {}: the '
                        'note no longer shows a state that cannot answer a trigger'
                        .format(name, coverage.get(name), states))
            return
    if gen_docs.trigger_coverage({'name': 'T', 'states': []}):
        report.fail('trigger-coverage',
                    'a machine with no trigger still prints a coverage note')
        return
    report.ok('trigger-coverage',
              'gen_docs.py marks the initial state, a composite and a trigger no '
              'state answers')


def check_state_mirrors(report):
    """gen_docs.py still names a machine state the service publishes no value for.

    An attribute whose enum takes a machine's state names is how a peer watches that
    machine. A state the enum cannot say is a phase the peer never sees, and under the
    default OnChange a phase left and re-entered re-sets the value already held and
    notifies nobody. Nothing else reports either: the documents generate, the code
    compiles, and a consumer waiting for that update waits for ever. One measured run
    spent $0.77 and three wrong diagnoses on it.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('state-mirror', 'gen_docs.py does not import: {}'.format(failure))
        return

    def project(values, extra=None):
        return {
            'datatypes': None,
            'interfaces': [{
                'name': 'S',
                'types': [{'name': 'EPhase', 'kind': 'enum',
                           'values': [{'name': v} for v in values]}],
                'attributes': [{'name': 'Phase', 'type': 'EPhase'}],
                'broadcasts': [{'name': n} for n in (extra or [])],
            }],
            'machines': [],
        }

    machine = {
        'name': 'M', 'initial': 'IDLE',
        'states': [
            {'name': 'IDLE', 'transitions': [{'on': 'go', 'to': 'WORK'}]},
            {'name': 'WORK', 'initial': 'WORK_ONE', 'states': [
                {'name': 'WORK_ONE', 'transitions': [{'on': 'next', 'to': 'WORK_TWO'}]},
                {'name': 'WORK_TWO', 'transitions': []},
                {'name': 'WORK_DONE', 'kind': 'final'},
                {'name': 'WorkHistory', 'kind': 'history', 'depth': 'Shallow'},
            ]},
            {'name': 'HELD', 'transitions': [{'on': 'go', 'to': 'WorkHistory'}]},
        ],
    }
    full = ['Idle', 'One', 'Two', 'Held']

    missing = gen_docs.state_mirrors(project([v for v in full if v != 'Held']), machine)
    if len(missing) != 1 or missing[0][4] != ['HELD']:
        report.fail('state-mirror',
                    'gen_docs.py reports {}, not one note naming HELD: a machine state '
                    'the service publishes no value for is no longer reported'
                    .format(missing))
        return
    if missing[0][2] != 'OnChange':
        report.fail('state-mirror',
                    'the note no longer carries the attribute\'s Notify, so it cannot '
                    'say that re-entering a state sends nothing')
        return
    for case, spec in (('a value per state', project(full)),
                       ('the phase named elsewhere on the interface',
                        project([v for v in full if v != 'Held'], extra=['HeldNow']))):
        if gen_docs.state_mirrors(spec, machine):
            report.fail('state-mirror',
                        'gen_docs.py still reports a missing state when {}: the note '
                        'fires on a design that has no defect'.format(case))
            return
    if gen_docs.state_mirrors(project(full), {'name': 'M', 'states': []}):
        report.fail('state-mirror', 'a machine with no state still prints a note')
        return
    # An enum that names one state and misses the rest is the worst case the two
    # pages describe, and it used to be the one case that drew nothing: the note
    # fired only once two names already matched.
    one = gen_docs.state_mirrors(project(['Idle', 'Elsewhere']), machine)
    if len(one) != 1 or sorted(one[0][4]) != ['HELD', 'WORK_ONE', 'WORK_TWO']:
        report.fail('state-mirror',
                    'gen_docs.py reports {} for an enum naming one state of four: an '
                    'attribute that mirrors a machine and can say only one of its '
                    'states draws no note'.format(one))
        return
    if one[0][3] != 1:
        report.fail('state-mirror',
                    'the note no longer carries how many state names matched, so it '
                    'cannot say that one name in common may be coincidence')
        return
    report.ok('state-mirror',
              'gen_docs.py names a machine state no attribute publishes, down to a '
              'single name in common, and is silent on a final state, a history '
              'marker and a phase published elsewhere')


def check_design_template(report):
    """The scaffold's design.json is a template the generator reads, and never a trap.

    Every key it shows is one the generator reads, an untouched copy is refused as the
    template, a filled copy generates, a key the generator does not read is refused by
    name instead of being ignored, and the template never replaces a file with work in it.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('template', 'gen_docs.py does not import: {}'.format(failure))
        return
    tools = os.path.join(ROOT, 'tools', 'agent')

    def refused(template):
        shown = io.StringIO()
        try:
            with contextlib.redirect_stderr(shown):
                gen_docs.check_shape({'datatypes': template['datatypes'],
                                      'interfaces': template['interfaces'],
                                      'machines': template['machines']})
        except SystemExit:
            return shown.getvalue().strip()
        return ''

    shape = refused(gen_docs.without_notes(gen_docs.TEMPLATE))
    if shape:
        report.fail('template', 'the template teaches a key the generator refuses: {}'
                    .format(shape[:200]))
        return

    def generate(path):
        return subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                               '--spec', path, '--outdir', 'out', '--force'],
                              capture_output=True, text=True)

    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        laid = subprocess.run([sys.executable, os.path.join(tools, 'setup_project.py'),
                               '--name', 'plan', '--root', '.', '--mode', 'ipc',
                               '--sdk-root', ROOT, '--quiet'], capture_output=True, text=True)
        if laid.returncode != 0 or not os.path.isfile('design.json'):
            report.fail('template', 'the scaffold writes no design.json, so the design is '
                                    'composed from an example of another application: {}'
                        .format(laid.stderr.strip()[:200]))
            return
        with open('design.json', encoding='utf-8') as handle:
            template = json.load(handle)
        if gen_docs.without_notes(template) != gen_docs.without_notes(gen_docs.TEMPLATE):
            report.fail('template', 'the scaffold writes a design.json that is not the '
                                    'template of gen_docs.py')
            return

        pristine = generate('design.json')
        if pristine.returncode == 0 or 'still the template' not in pristine.stderr:
            report.fail('template', 'an untouched template is not refused as the template: '
                                    '{}'.format((pristine.stderr or pristine.stdout)[:200]))
            return

        filled = json.loads(json.dumps(template))
        filled['datatypes']['name'] = 'PlanTypes'
        filled['datatypes']['declare'].insert(0, {
            'name': 'Mode', 'kind': 'enum', 'description': '',
            'values': [{'name': 'Off', 'value': 0, 'description': ''},
                       {'name': 'On', 'value': 1, 'description': ''}]})
        service = filled['interfaces'][0]
        service['name'] = 'PlanService'
        service['attributes'].insert(0, {'name': 'Level', 'type': 'uint32',
                                         'notify': 'OnChange', 'description': ''})
        service['requests'].insert(0, {'name': 'switch_to', 'description': '',
                                       'params': [{'name': 'mode', 'type': 'PlanTypes::Mode',
                                                   'description': ''}], 'answer': []})
        machine = filled['machines'][0]
        machine['name'] = 'Plan'
        machine['initial'] = 'IDLE'
        machine['triggers'].insert(0, {'name': 'go', 'description': '', 'params': []})
        machine['actions'].insert(0, {'name': 'start_work', 'description': '', 'params': []})
        machine['states'].insert(0, {'name': 'IDLE', 'kind': '', 'entry': [], 'exit': [],
                                     'transitions': [{'on': 'go', 'to': '', 'guard': [],
                                                      'set': {}, 'do': ['start_work']}],
                                     'initial': '', 'final_event': '', 'states': []})

        def written(spec, name):
            with open(name, 'w', encoding='utf-8') as handle:
                json.dump(spec, handle, indent=2)
            return name

        made = generate(written(filled, 'filled.json'))
        if made.returncode != 0 or 'skipped' not in made.stdout:
            report.fail('template', 'a filled template does not generate with its samples '
                                    'skipped: {}'.format((made.stderr or made.stdout)[:200]))
            return

        for label, change, expected in (
                ('a misspelt guard', lambda s: s['machines'][0]['states'][0]['transitions'][0]
                 .update(gaurd=['Level', 'gt', 'lit:0']), 'Did you mean "guard"?'),
                ('a misspelt step key', lambda s: s['machines'][0]['states'][0]
                 ['transitions'][0].update(do=[{'call': 'start_work', 'arg': {'x': 'y'}}]),
                 '"arg"'),
                ('a hand-written note key', lambda s: s['interfaces'][0]['attributes'][0]
                 .update({'//': 'in percent'}), 'A note in a spec is a "#|" key'),
                # An empty value means the same as an absent key, and a misspelling
                # means the same whatever it holds. Dropping it leaves the mistake in
                # the file, to be refused later once a value has been written in.
                ('a misspelt key with an empty value',
                 lambda s: s['interfaces'][0].update(broadcast=[]),
                 'Did you mean "broadcasts"?')):
            wrong = json.loads(json.dumps(filled))
            change(wrong)
            got = generate(written(wrong, 'wrong.json'))
            if got.returncode == 0 or expected not in got.stderr:
                report.fail('template', '{} is not refused by name, so a document silently '
                                        'lacks what the author wrote: {}'
                            .format(label, (got.stderr or got.stdout).strip()[:200]))
                return

        with open('filled.json', encoding='utf-8') as handle:
            before = handle.read()
        again = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                '--template', 'filled.json'], capture_output=True, text=True)
        with open('filled.json', encoding='utf-8') as handle:
            after = handle.read()
        if again.returncode == 0 or before != after:
            report.fail('template', '--template replaced a file that carries a design')
            return

        # Every other malformed input to this tool answers in one line. --outdir
        # naming a file raised FileExistsError through to the terminal.
        with open('afile', 'w', encoding='utf-8') as handle:
            handle.write('not a directory\n')
        blocked = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                  '--spec', 'filled.json', '--outdir', 'afile'],
                                 capture_output=True, text=True)
        if blocked.returncode == 0 or 'Traceback' in blocked.stderr:
            report.fail('template', '--outdir naming a file does not refuse in one line: '
                        '{}'.format((blocked.stderr or blocked.stdout).strip()[-200:]))
            return
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('template', 'the scaffold writes design.json as the template: its keys are '
                          'the generator\'s, untouched it is refused, filled it generates, '
                          'a stray key is refused by name with a value and without one, '
                          'work is never replaced, and a bad --outdir answers in one line')


# Two defects planted in the shape the generator writes, not in a hand-made sample.
# Each is an anchor the generated consumer carries and the text that replaces it.
GENERATED_DEFECTS = (
    ('P-06', 'a blocking call in service_connected',
     '// Subscriptions are made here, and again after every reconnection.',
     'areg::Thread::sleep(1000);'),
    ('P-05', 'quitting where a lost provider arrives',
     '// TODO(you) peer_lost',
     'quit_with(1);   // TODO(you) peer_lost'),
    ('B-08', 'a free function of namespace areg that no header declares',
     '// Subscriptions are made here, and again after every reconnection.',
     'bool valid = areg::is_data_valid(status);'),
    ('B-08', 'a static member of an areg class that no header declares',
     '// Subscriptions are made here, and again after every reconnection.',
     'areg::String text = areg::String::fromInt(3);'),
    ('B-02', 'a range-for with its body on the same line',
     '// Subscriptions are made here, and again after every reconnection.',
     'areg::ArrayList<int> seen; for (int each : seen) { (void)each; }'),
    ('B-02', 'an iterator taken from an areg container',
     '// Subscriptions are made here, and again after every reconnection.',
     'areg::ArrayList<int> seen; auto first = seen.begin();'),
)


# What a spec means, and what the documents it generates must say. Each case is a
# spec, the run it expects, and what the output must and must not carry. Every one of
# them was a silent change of meaning: accepted input, exit 0, a document saying
# something else.
SPEC_SEMANTICS = (
    ('a response that carries no value',
     {"interfaces": [{"name": "NoArg",
                      "requests": [{"name": "ping", "answer": []}]}]},
     0, ['MethodType="Response"'], []),
    ('the template sample nobody filled is no response',
     {"interfaces": [{"name": "T1", "requests": [
         {"name": "ping", "description": "",
          "params": [{"name": "", "type": "", "description": ""}],
          "answer": [{"name": "", "type": "", "description": ""}]}]}]},
     0, [], ['MethodType="Response"']),
    ('two documents of one name',
     {"interfaces": [{"name": "Same", "requests": [{"name": "a"}]},
                     {"name": "Same", "requests": [{"name": "b"}]}]},
     2, [], []),
    ('two unrelated services may share an attribute name',
     {"interfaces": [{"name": "Door", "attributes": [{"name": "Status", "type": "bool"}]},
                     {"name": "Sensor",
                      "attributes": [{"name": "Status", "type": "int32"}]}]},
     0, [], []),
    ('a guard written false is a guard',
     {"machines": [{"name": "Guard", "triggers": ["go"], "initial": "Idle",
                    "actions": [{"name": "act"}],
                    "constants": [{"name": "Verbose", "type": "bool", "value": False}],
                    "states": [{"name": "Idle", "transitions": [
                        {"on": "go", "to": "Done", "guard": False,
                         "do": [{"call": "act"}]}]},
                        {"name": "Done"}]}]},
     0, ['<Guard', '<Lit>false</Lit>', 'Value="false"'], ['False', 'True']),
    ('two requests of one name',
     {"interfaces": [{"name": "Twice", "requests": [{"name": "a", "answer": []},
                                                    {"name": "a", "answer": []}]}]},
     2, [], []),
    ('two attributes one accessor',
     {"interfaces": [{"name": "Case", "requests": [{"name": "a"}],
                      "attributes": [{"name": "Count", "type": "uint32"},
                                     {"name": "count", "type": "uint32"}]}]},
     2, [], []),
    ('two attributes one accessor, spelled with an underscore',
     {"interfaces": [{"name": "Under", "requests": [{"name": "a"}],
                      "attributes": [{"name": "my_Value", "type": "uint32"},
                                     {"name": "my_value", "type": "uint32"}]}]},
     2, [], []),
    ('a keyword spelled as written',
     {"interfaces": [{"name": "Word", "requests": [
         {"name": "a", "params": [{"name": "class", "type": "uint32"}]}]}]},
     2, [], []),
    ('a keyword behind a prefix compiles',
     {"interfaces": [{"name": "Prefixed", "requests": [{"name": "delete"}]}]},
     0, ['Name="delete"'], []),
)


def check_app_shape(report):
    """The application path names what it cannot write, rather than writing a part of it.

    gen_skeleton.py --app writes one service and at most one machine. Given several,
    build_project.py used to take the first of each, so a two-service design built,
    passed every check and was a third of the project.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    try:
        two = {"interfaces": [{"name": "Door", "requests": [{"name": "a"}]},
                              {"name": "Sensor", "requests": [{"name": "b"}]}]}
        one = {"interfaces": [{"name": "Door", "requests": [{"name": "a"}]}]}
        for what, spec, refuses in (('two services', two, True),
                                    ('one service', one, False)):
            root = os.path.join(holder, what.replace(' ', '-'))
            os.makedirs(root, exist_ok=True)
            with open(os.path.join(root, 'design.json'), 'w', encoding='utf-8',
                      newline='\n') as handle:
                json.dump(spec, handle)
            done = subprocess.run([sys.executable,
                                   os.path.join(tools, 'build_project.py'),
                                   '--root', root, '--spec', 'design.json',
                                   '--no-check'],
                                  capture_output=True, text=True, cwd=root)
            said = done.stdout + done.stderr
            named = '"programs"' in said
            if refuses and not named:
                report.fail('app-shape',
                            'build_project.py took one of two services without saying '
                            'so, and built a third of the project as if it were the '
                            'project')
                return
            if not refuses and named:
                report.fail('app-shape',
                            'build_project.py refuses a project of one service')
                return
        # build_project.py refuses at the application step, which is after the
        # documents are written. gen_docs.py says it at the documents step, one step
        # earlier and before anything is on disk -- and it must say it with --chained,
        # which is the only way build_project.py ever calls it. The refusal above
        # carries the same words, so a check that reads the whole chain cannot tell
        # the two apart and passes while the earlier one is dead.
        docs = os.path.join(holder, 'note')
        os.makedirs(docs, exist_ok=True)
        spec = os.path.join(holder, 'two.json')
        with open(spec, 'w', encoding='utf-8', newline='\n') as handle:
            json.dump(two, handle)
        done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                               '--spec', spec, '--outdir', docs, '--force', '--chained'],
                              capture_output=True, text=True)
        if '"programs"' not in done.stdout + done.stderr:
            report.fail('app-shape',
                        'gen_docs.py --chained does not name the "programs" block, '
                        'so the golden path first hears it from build_project.py, a '
                        'step later and after the documents are written')
            return
        report.ok('app-shape', 'the application path builds one service and names what '
                               'it cannot write, at the documents step and again at the '
                               'application step')
    finally:
        shutil.rmtree(holder, ignore_errors=True)


UPDATE_GUARD = 'if (state == areg::DataState::DataIsOK)'


def check_update_note(report):
    """The worksheet quotes the check every update_ body already runs inside, and the
    generated handler still makes that check."""
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    import gen_skeleton
    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        made = generate_application(tools, 'update')
        if not os.path.isfile(made):
            report.fail('update-note', made)
            return
        with open(made, encoding='utf-8') as handle:
            source = handle.read()
        with open(gen_skeleton.WORKSHEET, encoding='utf-8') as handle:
            worksheet = handle.read()
        if 'on_' not in source or UPDATE_GUARD not in source:
            report.fail('update-note', 'the generated update handler no longer makes the '
                                       'check "{}", so the worksheet note that quotes it '
                                       'is false'.format(UPDATE_GUARD))
            return
        quoted = [line for line in worksheet.splitlines()
                  if line.startswith('#|') and UPDATE_GUARD in line]
        if '== update_' in worksheet and not quoted:
            report.fail('update-note', 'the worksheet has update_ sections and does not '
                                       'quote the check their bodies run inside, so an '
                                       'agent writes its own and guesses a name for it')
            return
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('update-note', 'the worksheet quotes the DataIsOK check every update_ body '
                             'runs inside, and the handler makes it')


def check_scaffold_table(report):
    """The scaffold table of 01-runbook.md section 2 names every file setup_project.py
    writes, and nothing it does not. A harness pointer file is named by its row."""
    import fnmatch
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    import setup_project
    section = read('docs', 'agent', '01-runbook.md').split('\n## 2.', 1)[-1] \
        .split('\n## ', 1)[0]
    named = []
    for row in section.splitlines():
        if row.startswith('| `'):
            named += re.findall(r'`([^`]+)`', row.split('|')[1])
    if not named:
        report.fail('scaffold-table', '01-runbook.md section 2 carries no scaffold table')
        return
    holder = tempfile.mkdtemp()
    try:
        done = subprocess.run([sys.executable,
                               os.path.join(ROOT, 'tools', 'agent', 'setup_project.py'),
                               '--name', 'table', '--root', holder, '--mode', 'ipc',
                               '--sdk-root', ROOT], capture_output=True, text=True)
        if done.returncode != 0:
            report.fail('scaffold-table', 'setup_project.py no longer lays out a project: '
                        + (done.stderr or done.stdout).strip()[-160:])
            return
        written = []
        for path, _dirs, files in os.walk(holder):
            for name in files:
                written.append(os.path.relpath(os.path.join(path, name), holder)
                               .replace(os.sep, '/'))
        pointers = set(name for name in setup_project.HARNESS_FILES.values() if name)
        pointers.add('.gitignore')
        absent = [pattern for pattern in named
                  if not any(fnmatch.fnmatch(name, pattern) for name in written)]
        unnamed = sorted(name for name in written if name not in pointers
                         and not any(fnmatch.fnmatch(name, p) for p in named))
        if absent or unnamed:
            report.fail('scaffold-table', '01-runbook.md section 2 {}{}{}'.format(
                'names what the scaffold does not write: ' + ', '.join(absent)
                if absent else '', '; ' if absent and unnamed else '',
                'does not name what it writes: ' + ', '.join(unnamed)
                if unnamed else ''))
            return
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('scaffold-table', '01-runbook.md section 2 names the {} file(s) the scaffold '
                                'writes, harness pointers aside'.format(len(named)))


UNCOMPILABLE_NAMES_SIML = """<?xml version="1.0" encoding="utf-8"?>
<ServiceInterface FormatVersion="1.1.0">
    <Overview ID="1" Name="Word" Version="1.0.0" isRemote="true"/>
    <AttributeList>
        <Attribute ID="2" Name="Count" DataType="uint32" Notify="OnChange"/>
        <Attribute ID="3" Name="count" DataType="uint32" Notify="OnChange"/>
        <Attribute ID="4" Name="Class" DataType="uint32" Notify="OnChange"/>
    </AttributeList>
    <MethodList>
        <Method ID="5" Name="send" MethodType="Request">
            <ParamList>
                <Parameter ID="6" Name="class" DataType="uint32"/>
            </ParamList>
        </Method>
    </MethodList>
</ServiceInterface>
"""

PREFIXED_KEYWORD_SIML = """<?xml version="1.0" encoding="utf-8"?>
<ServiceInterface FormatVersion="1.1.0">
    <Overview ID="1" Name="Prefixed" Version="1.0.0" isRemote="true"/>
    <MethodList>
        <Method ID="2" Name="delete" MethodType="Request"/>
    </MethodList>
</ServiceInterface>
"""


CONTAINER_KEY_DTML = """<?xml version="1.0" encoding="utf-8"?>
<DataTypeDocument FormatVersion="1.0.0">
    <Overview ID="1" Name="KeyRules" Version="1.0.0"/>
    <DataTypeList>
        <DataType ID="2" Name="Blob" Type="Structure"><FieldList><Field ID="3" Name="data" DataType="BinaryBuffer"><Value IsDefault="true"/></Field></FieldList></DataType>
        <DataType ID="4" Name="Stamp" Type="Structure"><FieldList><Field ID="5" Name="id" DataType="uint32"><Value IsDefault="true">0</Value></Field><Field ID="6" Name="when" DataType="DateTime"><Value IsDefault="true"/></Field></FieldList></DataType>
        <DataType ID="9" Name="Point" Type="Structure"><FieldList><Field ID="10" Name="x" DataType="uint32"><Value IsDefault="true">0</Value></Field></FieldList></DataType>
        <DataType ID="20" Name="ByBlob" Type="Container"><Container>HashMap</Container><BaseTypeValue>uint32</BaseTypeValue><BaseTypeKey>Blob</BaseTypeKey></DataType>
        <DataType ID="22" Name="SortedStamp" Type="Container"><Container>Map</Container><BaseTypeValue>uint32</BaseTypeValue><BaseTypeKey>Stamp</BaseTypeKey></DataType>
        <DataType ID="25" Name="ByPoint" Type="Container"><Container>HashMap</Container><BaseTypeValue>uint32</BaseTypeValue><BaseTypeKey>Point</BaseTypeKey></DataType>
    </DataTypeList>
</DataTypeDocument>
"""


def check_codegen_name_rules(report):
    """codegen.jar refuses a hand-written document whose names cannot compile.

    Two attributes with one accessor are rule 4; an accessor that is a keyword and a
    keyword spelled as written are rule 5. A keyword behind a request prefix generates.
    Rule 59 is the same question asked of a container key: a HashMap key has to hash
    and a Map key has to order. A jar that carries no rule 59 generates a container
    that does not compile, and exits 0.
    """
    jar = os.path.join(ROOT, 'tools', 'codegen.jar')
    holder = tempfile.mkdtemp()
    try:
        os.makedirs(os.path.join(holder, 'src', 'services'))
        outcome = {}
        for name, text in (('Word.siml', UNCOMPILABLE_NAMES_SIML),
                           ('Prefixed.siml', PREFIXED_KEYWORD_SIML),
                           ('KeyRules.dtml', CONTAINER_KEY_DTML)):
            with open(os.path.join(holder, 'src', 'services', name), 'w',
                      encoding='utf-8', newline='\n') as handle:
                handle.write(text)
            try:
                done = subprocess.run(['java', '-jar', jar, '--root=' + holder,
                                       '--doc=src/services/' + name, '--target=generated'],
                                      cwd=holder, capture_output=True, text=True)
            except OSError as problem:
                report.fail('codegen-name-rules', 'java cannot be run: {}'.format(problem))
                return
            outcome[name] = (done.returncode, done.stdout + done.stderr)
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    code, text = outcome['Word.siml']
    found = (text.count('error[4/RULE_DUPLICATE_NAME]'),
             text.count('error[5/RULE_INVALID_IDENTIFIER]'))
    if code == 0 or found[0] < 1 or found[1] < 2:
        report.fail('codegen-name-rules', 'codegen.jar exited {} on Count/count, an '
                                          'attribute Class and a parameter class, with {} '
                                          'rule 4 and {} rule 5 finding(s); expected a '
                                          'refusal with 1 and 2'.format(code, *found))
        return
    code, text = outcome['Prefixed.siml']
    if code != 0:
        report.fail('codegen-name-rules', 'codegen.jar refused a request named delete, '
                                          'which is spelled behind its prefix: '
                                          + text.strip()[-200:])
        return
    code, text = outcome['KeyRules.dtml']
    keys = text.count('error[59/RULE_CONTAINER_KEY]')
    if code == 0 or keys != 1:
        report.fail('codegen-name-rules',
                    'codegen.jar exited {} on a HashMap keyed by a structure holding a '
                    'BinaryBuffer, with {} rule 59 finding(s); expected a refusal with '
                    '1. A jar with no rule 59 writes a container that does not compile'
                    .format(code, keys))
        return
    for legal in ('SortedStamp', 'ByPoint'):
        if legal in text:
            report.fail('codegen-name-rules',
                        'codegen.jar reports the container {}, whose key can be a key: '
                        'rule 59 fires on a document that has no defect'.format(legal))
            return
    report.ok('codegen-name-rules', 'codegen.jar refuses names that cannot compile (rules 4 '
                                    'and 5) and a container key that cannot be one '
                                    '(rule 59), and accepts a keyword behind a prefix '
                                    'and a key that hashes or orders')


def check_spec_semantics(report):
    """A spec that is accepted generates the documents it describes.

    Everything here was silent: the generator took the input, exited 0, and wrote a
    document that said something else. A refusal is loud and costs one edit; a
    document that means the wrong thing is built, filled and verified.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    try:
        for what, spec, code, wanted, refused in SPEC_SEMANTICS:
            path = os.path.join(holder, 'design.json')
            with open(path, 'w', encoding='utf-8', newline='\n') as handle:
                json.dump(spec, handle)
            out = os.path.join(holder, 'out')
            done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                   '--spec', path, '--outdir', out, '--force'],
                                  capture_output=True, text=True, cwd=holder)
            if done.returncode != code:
                report.fail('spec-semantics',
                            '{}: gen_docs.py exited {}, expected {}. {}'
                            .format(what, done.returncode, code,
                                    (done.stderr or done.stdout).strip()[:160]))
                return
            written = ''
            for name in sorted(glob.glob(os.path.join(out, '*'))):
                with open(name, encoding='utf-8') as handle:
                    written += handle.read()
                os.remove(name)
            for text in wanted:
                if text not in written:
                    report.fail('spec-semantics',
                                '{}: the document does not carry {!r}'.format(what, text))
                    return
            for text in refused:
                if text in written:
                    report.fail('spec-semantics',
                                '{}: the document carries {!r}'.format(what, text))
                    return
        report.ok('spec-semantics',
                  '{} spec(s) generate the document they describe'
                  .format(len(SPEC_SEMANTICS)))
    finally:
        shutil.rmtree(holder, ignore_errors=True)


def check_await_spelling(report):
    """A step may await a thing by the name of the consumer method that receives it."""
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('await-spelling', 'gen_docs.py does not import: {}'.format(failure))
        return
    tools = os.path.join(ROOT, 'tools', 'agent')

    def spec(target):
        return {"interfaces": [{
            "name": "Tank",
            "requests": [{"name": "set_limit", "params": [{"name": "high", "type": "int32"}],
                          "answer": [{"name": "accepted", "type": "bool"}]}],
            "broadcasts": [{"name": "limit_reached"}],
            "attributes": [{"name": "Overflow", "type": "bool"}],
            "steps": [{"name": "Go", "send": "set_limit", "args": {"high": 3},
                       "await": target}]}]}

    holder = tempfile.mkdtemp()
    try:
        def loaded(target):
            path = os.path.join(holder, 'design.json')
            with open(path, 'w', encoding='utf-8', newline='\n') as handle:
                json.dump(spec(target), handle)
            done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                   '--spec', path, '--outdir', os.path.join(holder, 'out'),
                                   '--force'], capture_output=True, text=True, cwd=holder)
            steps = gen_docs.load_spec(path)[0]['interfaces'][0]['steps']
            return done.returncode, steps[0].get('await'), done.stderr.strip()

        for said, meant in (('response_set_limit', 'set_limit'),
                            ('broadcast_limit_reached', 'limit_reached'),
                            ('on_overflow_update', 'Overflow'),
                            ('Overflow_update', 'Overflow')):
            code, target, err = loaded(said)
            if code != 0 or target != meant:
                report.fail('await-spelling',
                            'a step awaiting "{}" is read as "{}" and gen_docs.py exits {}, '
                            'where it names "{}" and nothing else: {}'
                            .format(said, target, code, meant, err[:160]))
                return
        for said in ('response_nothing', 'broadcast_set_limit', 'on_set_limit_update'):
            code, _target, _err = loaded(said)
            if code == 0:
                report.fail('await-spelling',
                            'a step awaiting "{}", which names nothing the service '
                            'declares, is accepted'.format(said))
                return
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('await-spelling',
              'a step awaiting response_<r>, broadcast_<b> or on_<attr>_update is read as '
              'the thing it names, and a prefix naming nothing is still refused')


def check_start_kind(report):
    """A state marked "kind": "start" is the initial of its level, or it is refused."""
    tools = os.path.join(ROOT, 'tools', 'agent')

    def machine(top_kind, inner_kind, top_initial='Idle'):
        idle = {"name": "Idle",
                "transitions": [{"on": "go", "to": "Busy", "do": [{"call": "act"}]}]}
        work = {"name": "Work", "transitions": [{"on": "go", "to": "Rest"}]}
        if top_kind:
            idle['kind'] = top_kind
        if inner_kind:
            work['kind'] = inner_kind
        busy = {"name": "Busy", "initial": "Work", "states": [work, {"name": "Rest"}],
                "transitions": [{"on": "stop", "to": "Idle"}]}
        spec = {"name": "Door", "triggers": ["go", "stop"], "actions": [{"name": "act"}],
                "states": [idle, busy]}
        if top_initial:
            spec['initial'] = top_initial
        return {"machines": [spec]}

    holder = tempfile.mkdtemp()
    try:
        def generated(spec):
            path = os.path.join(holder, 'design.json')
            out = os.path.join(holder, 'out')
            with open(path, 'w', encoding='utf-8', newline='\n') as handle:
                json.dump(spec, handle)
            done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                   '--spec', path, '--outdir', out, '--force'],
                                  capture_output=True, text=True, cwd=holder)
            if done.returncode != 0:
                return None, done.stderr.strip()
            with open(os.path.join(out, 'Door.fsml'), 'rb') as handle:
                return handle.read(), ''

        plain, err = generated(machine(None, None))
        if plain is None:
            report.fail('start-kind', 'the plain machine does not generate: ' + err[:160])
            return
        for what, spec in (('on the initial', machine('start', None)),
                           ('spelled Start, with no "initial"', machine('Start', None, None)),
                           ('spelled initial, on a nested level', machine(None, 'initial'))):
            text, err = generated(spec)
            if text != plain:
                report.fail('start-kind',
                            '"kind": "start" {} does not generate the machine without it: {}'
                            .format(what, err[:160] or 'the .fsml differs'))
                return
        wrong = machine(None, None)
        wrong['machines'][0]['states'][1]['kind'] = 'start'
        text, err = generated(wrong)
        if text is not None or '"initial"' not in err:
            report.fail('start-kind',
                        'a "start" state the level\'s "initial" does not name is {}'
                        .format('accepted' if text is not None else 'refused as: ' + err[:160]))
            return
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('start-kind',
              'a state of kind "start" generates the same .fsml as the level\'s "initial" '
              'naming it, and one the "initial" does not name is refused by that key')


def runbook_section(number):
    """The text of one numbered section of 01-runbook.md, or '' when it is missing."""
    for part in read('docs', 'agent', '01-runbook.md').split('\n## '):
        if part.startswith('{}. '.format(number)):
            return part
    return ''


# Text that sends a run past the design request: (file, phrase, what it does).
DESIGN_DETOURS = (
    (('AGENTS.md',), '05-design.md`, before writing any file',
     'routes to 05-design.md before the scaffold, which the runbook runs first'),
    (('tools', 'agent', 'setup_project.py'), '05-design.md`, before writing any file',
     'routes a project to 05-design.md before the scaffold'),
    (('docs', 'agent', '05-design.md'), '`20-service-interface.md` to write the documents',
     'sends a design to the XML page, which design.json replaces'),
    (('docs', 'agent', '01-runbook.md'), 'throws away its `#|` notes',
     'keeps design.json edited in pieces for notes gen_docs.py ignores'),
    (('docs', 'agent', '01-runbook.md'), '`22-state-machine.md` when a machine is needed',
     'opens 22-state-machine.md on a decision 05-design.md makes in the same request'),
)


def check_design_request(report):
    """The design is one request after the scaffold, and nothing routes a run past it."""
    first = runbook_section(1)
    if not first or re.search(r'^pwd$', first, re.M):
        report.fail('design-request', '01-runbook.md section 1 asks for pwd in a request '
                    'of its own, although the scaffold command checks the root')
        return
    design = runbook_section(3)
    missing = [name for name in ('in one request', 'gen_docs.py --example', '05-design.md',
                                 '22-state-machine.md', 'depends on what came before',
                                 'Open nothing else')
               if name not in design]
    if missing:
        report.fail('design-request', '01-runbook.md section 3 does not name the design '
                    'request; it lacks: {}'.format(', '.join(missing)))
        return
    if not all(phrase in runbook_section(4) for phrase in ('whole', '--write design.json')):
        report.fail('design-request', '01-runbook.md section 4 does not say design.json '
                    'is written whole and built in one call, with --write design.json')
        return
    for parts, phrase, does in DESIGN_DETOURS:
        if phrase in read(*parts):
            report.fail('design-request', '{} {}'.format('/'.join(parts), does))
            return
    for name in ('areg-ai-prompt-template.txt', 'areg-coffeemachine-prompt.txt'):
        wrapper = read('examples', 'ai-benchmark', name)
        if 'all four in one request' not in ' '.join(wrapper.split()) or \
                '\n2. Read <task>' in wrapper:
            report.fail('design-request', 'examples/ai-benchmark/{} reads the task file in '
                        'a request of its own'.format(name))
            return

    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    try:
        sdk = os.path.join(holder, 'sdk')
        os.makedirs(sdk)
        open(os.path.join(sdk, 'areg.cmake'), 'w').close()
        inner = os.path.join(sdk, 'app')
        done = subprocess.run([sys.executable, os.path.join(tools, 'setup_project.py'),
                               '--name', 'app', '--root', inner, '--mode', 'ipc',
                               '--sdk-root', sdk, '--quiet'],
                              capture_output=True, text=True)
        if done.returncode == 0 or os.path.exists(inner):
            report.fail('design-request', 'setup_project.py scaffolds a root inside '
                        '--sdk-root instead of refusing it')
            return
        outer = os.path.join(holder, 'app')
        done = subprocess.run([sys.executable, os.path.join(tools, 'setup_project.py'),
                               '--name', 'app', '--root', outer, '--mode', 'ipc',
                               '--sdk-root', ROOT, '--quiet'],
                              capture_output=True, text=True)
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    if done.returncode != 0 or 'design.json is the one file to read' not in done.stdout:
        report.fail('design-request', 'the scaffold does not name design.json as the one '
                    'file of the project to read')
        return
    report.ok('design-request', 'the runbook names one design request after the scaffold, '
              'the scaffold refuses a root inside the SDK, and no page routes past it')


# The scenario rules, each where it is used: (where, phrase).
STEP_RULES_AT_USE = (
    ('the template steps note', 'not one per message'),
    ('the template steps note', 'awaits the broadcast saying it finished'),
    ('the template steps note', 'Every step sends, awaits or waits'),
    ('the worksheet scenarios.json header', 'One line per acceptance item'),
)

# The runbook sentences those replace, which it no longer states.
STEP_RULES_MOVED = ('not one per message', '`await`s the update that says it finished',
                    'Every acceptance item goes in')


def check_step_rules_at_use(report):
    """The step and scenario rules are in the template note and the worksheet, once."""
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('step-rules', 'gen_docs.py does not import: {}'.format(failure))
        return
    note = ' '.join(gen_docs.TEMPLATE['interfaces'][0]['steps'][0][gen_docs.NOTE])
    where = {'the template steps note': ' '.join(note.split()),
             'the worksheet scenarios.json header': read('tools', 'agent', 'gen_skeleton.py')}
    for place, phrase in STEP_RULES_AT_USE:
        if phrase not in where[place]:
            report.fail('step-rules', '{} does not say "{}"'.format(place, phrase))
            return
    runbook = ' '.join(read('docs', 'agent', '01-runbook.md').split())
    for phrase in STEP_RULES_MOVED:
        if phrase in runbook:
            report.fail('step-rules', '01-runbook.md still states "{}", which the point of '
                        'use now carries'.format(phrase))
            return
    report.ok('step-rules', 'the step rules are in the template '
              'steps note, the acceptance rule in the worksheet, and neither in the runbook')


def check_machine_attribute_note(report):
    """The template machine note says a same-named contract attribute is a separate value."""
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('machine-attribute', 'gen_docs.py does not import: {}'.format(failure))
        return
    note = ' '.join(' '.join(gen_docs.TEMPLATE['machines'][0][gen_docs.NOTE]).split())
    if 'contract attribute of the same name is another value' not in note:
        report.fail('machine-attribute', 'the template machine note does not say that a '
                                         'contract attribute named like a machine attribute '
                                         'is a separate value, so a design expects them synced')
        return
    report.ok('machine-attribute', 'the template machine note says a same-named contract '
                                   'attribute is a separate value')


# Nouns only a benchmark task uses. An example spelled with them hands one task its answer.
BENCHMARK_WORDS = ('coffee', 'espresso', 'latte', 'cappuccino', 'drink', 'coin',
                   'insert_coin', 'MakingHistory', 'MAKING', 'elevator', 'greenhouse',
                   'thermostat', 'coffeemachine', 'tempalarm', 'atmfsm', 'printscan',
                   'refund', 'credit')

# A benchmark run's directory name, or a sentence citing one as evidence.
RUN_ID_RE = re.compile(r'\b20\d{6}[a-z]?-[a-z]|\bruns? 20\d{6}', re.IGNORECASE)


def shipped_agent_sources():
    """(place, text) of every docs/agent dotfile and every tools/agent script installed."""
    install = read('conf', 'cmake', 'install.cmake') or ''
    shipped = []
    pages = os.path.join(ROOT, 'docs', 'agent')
    for name in sorted(os.listdir(pages)):
        if name.startswith('.') and os.path.isfile(os.path.join(pages, name)):
            shipped.append(('docs/agent/' + name, read('docs', 'agent', name)))
    listed = install.replace('(', '|').replace(')', '|')
    for name in sorted(os.listdir(os.path.join(ROOT, 'tools', 'agent'))):
        if name.endswith('.py') and '|{}|'.format(name[:-3]) not in listed:
            shipped.append(('tools/agent/' + name, read('tools', 'agent', name)))
    return shipped


def template_notes(node):
    """Every #| note line of the design template, in order."""
    import gen_docs
    if isinstance(node, dict):
        for key, value in node.items():
            if key == gen_docs.NOTE:
                for line in value:
                    yield line
            else:
                for line in template_notes(value):
                    yield line
    elif isinstance(node, list):
        for item in node:
            for line in template_notes(item):
                yield line


def check_benchmark_vocabulary(report):
    """No shipped page, note, dotfile or tool teaches with a benchmark's nouns or runs."""
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('benchmark-words', 'gen_docs.py does not import: {}'.format(failure))
        return
    places = [('AGENTS.md', read('AGENTS.md')),
              ('the design template notes', '\n'.join(template_notes(gen_docs.TEMPLATE)))]
    pages = os.path.join(ROOT, 'docs', 'agent')
    places += [('docs/agent/' + name, read('docs', 'agent', name))
               for name in sorted(os.listdir(pages)) if name.endswith('.md')]
    places += shipped_agent_sources()
    for place, text in places:
        found = RUN_ID_RE.search(text or '')
        if found:
            line = text[:found.start()].count('\n') + 1
            report.fail('benchmark-words', '{}:{} cites a benchmark run: what a shipped '
                        'file says is true of every project, not of one run'
                        .format(place, line))
            return
        for word in BENCHMARK_WORDS:
            flags = 0 if word.isupper() or word[0].isupper() else re.IGNORECASE
            found = re.search(r'\b{}\b'.format(re.escape(word)), text, flags)
            if found:
                line = text[:found.start()].count('\n') + 1
                report.fail('benchmark-words', '{}:{} says "{}", a noun of a benchmark '
                            'task: an example in it hands that task its answer'
                            .format(place, line, word))
                return
    report.ok('benchmark-words', 'no page, AGENTS.md, template note, docs/agent dotfile '
              "or shipped tool uses a benchmark task's own nouns or cites a run")


def check_phase_by_one_action(report):
    """22-state-machine.md publishes a phase by one action taking it, not one per state."""
    page = ' '.join(read('docs', 'agent', '22-state-machine.md').split())
    for phrase in ('{"call": "publish_phase", "args": {"phase": "lit:',
                   'under `OnChange` the consumer hears it only if the value changed'):
        if phrase not in page:
            report.fail('phase-action', '22-state-machine.md does not say "{}", so a '
                        'machine publishing its phase declares one action per state'
                        .format(phrase))
            return
    if 'declare such an attribute `Always`' not in ' '.join(runbook_section(3).split()):
        report.fail('phase-action', '01-runbook.md section 3 says what OnChange does to a '
                    're-entered phase, not what to declare instead')
        return
    if "target must be a sibling of the state that declares it" not in page:
        report.fail('phase-action', '22-state-machine.md does not say whose sibling a '
                    'transition target is')
        return
    report.ok('phase-action', '22-state-machine.md publishes a phase from every entry by '
              'one action taking it, and says when OnChange sends a resumed one; the '
              'runbook names the remedy; a target is a sibling of its declaring state')


def check_base_api_on_demand(report):
    """40-base-api.md is opened for a call the worksheet list lacks, not before every body."""
    implement = ' '.join(runbook_section(6).split())
    if 'Read it before writing bodies' in implement or \
            'is the one page a body still needs' in implement:
        report.fail('base-api-demand', '01-runbook.md section 6 makes 40-base-api.md a read '
                    'before every body, although the worksheet lists the String calls')
        return
    missing = [name for name in ('floor, not a limit', '40-base-api.md', 'api_help.py')
               if name not in implement]
    if missing:
        report.fail('base-api-demand', '01-runbook.md section 6 does not name {} beside '
                    'the worksheet list'.format(', '.join(missing)))
        return
    for parts in (('AGENTS.md',), ('tools', 'agent', 'setup_project.py')):
        rows = [line for line in read(*parts).splitlines()
                if '40-base-api.md' in line and 'string' in line.lower() and
                line.startswith('|')]
        if not rows or any('worksheet' not in row for row in rows):
            report.fail('base-api-demand', '{} routes every string to 40-base-api.md '
                        'without the worksheet list first'.format('/'.join(parts)))
            return
    report.ok('base-api-demand', '40-base-api.md is routed for a call the worksheet list '
              'lacks, beside api_help.py, and the list is a floor')


def check_generated_defects(report):
    """Every prohibition is checked against the source the generator itself writes.

    A handler defined out of line carries no override keyword, and the exit helper
    the scaffold teaches is not signal_quit. A rule that knows neither reports
    nothing on the one layout every project starts from.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        made = generate_application(tools, 'defect')
        if not os.path.isfile(made):
            report.fail('generated-defects', made)
            return
        with open(made, encoding='utf-8') as handle:
            pristine = handle.read()

        def contract():
            done = subprocess.run([sys.executable,
                                   os.path.join(tools, 'check_contract.py'), '.',
                                   '--strict', '--allow-todo'],
                                  capture_output=True, text=True)
            return done.stdout + done.stderr

        clean = contract()
        if 'ERROR' in clean:
            report.fail('generated-defects',
                        'the generated application reports a prohibition of its own: '
                        + next(line for line in clean.splitlines() if 'ERROR' in line))
            return
        for rule, what, anchor, planted in GENERATED_DEFECTS:
            if anchor not in pristine:
                report.fail('generated-defects',
                            'the generated consumer no longer carries "{}", so {} '
                            'cannot be planted where it belongs'.format(anchor, rule))
                return
            with open(made, 'w', encoding='utf-8', newline='\n') as handle:
                handle.write(pristine.replace(anchor, planted, 1))
            found = contract()
            if rule not in found:
                report.fail('generated-defects',
                            '{} does not report {} in the application the generator '
                            'writes, though it reports the same defect in a handler '
                            'declared inline'.format(rule, what))
                return
        with open(made, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write(pristine)
        report.ok('generated-defects',
                  'the generated application is clean, and {} planted defect(s) in it '
                  'are reported'.format(len(GENERATED_DEFECTS)))
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)


def generate_application(tools, name='gen', steps=None):
    """Lay out a project in the working directory and write the application into it.

    Steps, when given, are declared on the first interface of the example design.

    Returns the path of the generated consumer source, or a sentence naming the step
    that stopped.
    """
    if subprocess.run([sys.executable, os.path.join(tools, 'setup_project.py'),
                       '--name', name, '--root', '.', '--mode', 'ipc',
                       '--sdk-root', ROOT],
                      capture_output=True, text=True).returncode != 0:
        return 'the scaffold no longer lays out a project'
    spec = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                           '--example'], capture_output=True, text=True)
    design = spec.stdout
    if steps is not None:
        parsed = json.loads(design)
        parsed['interfaces'][0]['steps'] = steps
        design = json.dumps(parsed, indent=2)
    with open('design.json', 'w', encoding='utf-8', newline='\n') as handle:
        handle.write(design)
    if subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                       '--outdir', 'src/services', '--force', '--chained',
                       '--spec', 'design.json'],
                      capture_output=True, text=True).returncode != 0:
        return 'the example spec no longer generates'
    docs = sorted(glob.glob(os.path.join('src', 'services', '*.siml')))
    machines = sorted(glob.glob(os.path.join('src', 'services', '*.fsml')))
    if not docs:
        return 'no .siml was generated to build a consumer from'
    made = [sys.executable, os.path.join(tools, 'gen_skeleton.py'),
            '--doc', docs[0], '--app', '--mode', 'ipc', '--force']
    if machines:
        made += ['--machine', machines[0]]
    if steps is not None:
        made += ['--spec', 'design.json']
    if subprocess.run(made, capture_output=True, text=True).returncode != 0:
        return 'gen_skeleton.py --app no longer writes an application'
    found = sorted(glob.glob(os.path.join('src', 'consumer', '*Consumer.cpp')))
    if not found:
        return 'no consumer source was written'
    return found[0]


def check_removed_marker_body(report):
    """A body of a marker the regenerated design no longer has leaves bodies.txt, named;
    a section that never was a marker stays for fill_markers.py to refuse."""
    tools = os.path.join(ROOT, 'tools', 'agent')
    here = os.getcwd()
    holder = tempfile.mkdtemp(prefix='areg-removed-marker-')
    try:
        os.chdir(holder)
        made = generate_application(tools, 'gen', STEP_SAMPLE)
        if not os.path.isfile(made):
            report.fail('removed-marker', made)
            return
        with open('bodies.txt', 'w', encoding='utf-8', newline='\n') as handle:
            handle.write('== step_open_gate\n// accepted\n== step_watch_width\n// seen\n'
                         '== step_watch_widht\n// misspelt\n')
        with open('design.json', encoding='utf-8') as handle:
            design = json.load(handle)
        design['interfaces'][0]['steps'] = [step for step in STEP_SAMPLE
                                            if step['name'] != 'watch_width']
        with open('design.json', 'w', encoding='utf-8', newline='\n') as handle:
            json.dump(design, handle)
        done = subprocess.run([sys.executable, os.path.join(tools, 'gen_skeleton.py'),
                               '--doc', sorted(glob.glob('src/services/*.siml'))[0],
                               '--app', '--mode', 'ipc', '--force', '--spec', 'design.json'],
                              capture_output=True, text=True)
        with open('bodies.txt', encoding='utf-8') as handle:
            sections = [line[3:].strip() for line in handle if line.startswith('== ')]
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
    if sections != ['step_open_gate', 'step_watch_widht'] or \
            'removed step_watch_width' not in done.stdout:
        report.fail('removed-marker', 'regenerating without a step leaves its body in '
                    'bodies.txt, which then refuses the whole write, or removes a section '
                    'that never was a marker; sections left: {}'.format(sections))
        return
    report.ok('removed-marker', 'the body of a marker the design removed leaves bodies.txt, '
              'named, and a misspelt section stays for the refusal')


NAMING_SIML = """<?xml version="1.0" encoding="utf-8"?>
<ServiceInterface FormatVersion="1.1.0">
    <Overview ID="1" Name="Naming" Version="1.0.0" Category="Public"/>
    <AttributeList>
        <Attribute ID="2" Name="WaterLevel" DataType="uint32" Notify="OnChange"/>
    </AttributeList>
    <MethodList>
        <Method ID="3" Name="OpenValve" MethodType="Request" Response="OpenValve"/>
        <Method ID="4" Name="OpenValve" MethodType="Response"/>
        <Method ID="5" Name="LowWarning" MethodType="Broadcast"/>
    </MethodList>
</ServiceInterface>
"""


def check_method_names(report):
    """The tools spell a generated method the way codegen.jar does.

    codegen.jar keeps a request, response or broadcast name as written after its
    prefix and turns only an attribute name into snake_case. A skeleton or a contract
    check that snake-cases a method name writes overrides the base does not declare
    and rejects the ones it does: one measured run paid 19 requests between the two.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')
    try:
        naming = json.loads(read('docs', 'agent', 'api.json'))['naming']['rules']
    except (ValueError, KeyError, TypeError):
        report.fail('method-names', 'docs/agent/api.json carries no naming rules')
        return
    transforms = dict((rule.get('applies_to'), rule.get('transform')) for rule in naming)
    if transforms.get('method') != 'verbatim' or transforms.get('broadcast') != 'verbatim' \
            or transforms.get('attribute') != 'snake_case':
        report.fail('method-names', 'api.json says a method is {}, a broadcast {} and an '
                                    'attribute {}; codegen.jar keeps the first two as '
                                    'written and converts the third'
                    .format(transforms.get('method'), transforms.get('broadcast'),
                            transforms.get('attribute')))
        return
    holder = tempfile.mkdtemp()
    try:
        os.makedirs(os.path.join(holder, 'src', 'services'))
        with open(os.path.join(holder, 'src', 'services', 'Naming.siml'), 'w',
                  encoding='utf-8', newline='\n') as handle:
            handle.write(NAMING_SIML)
        made = subprocess.run([sys.executable, os.path.join(tools, 'gen_skeleton.py'),
                               '--doc', 'src/services/Naming.siml', '--app', '--mode',
                               'ipc', '--force', '--scenarios', 'none.json'],
                              cwd=holder, capture_output=True, text=True)
        sources = ''
        for path in glob.glob(os.path.join(holder, 'src', '*', '*.[ch]pp')):
            with open(path, encoding='utf-8') as handle:
                sources += handle.read()
        wanted = ('request_OpenValve(', 'response_OpenValve(', 'broadcast_LowWarning(',
                  'notify_on_broadcast_LowWarning(', 'on_water_level_update(')
        missing = [name for name in wanted if name not in sources]
        if made.returncode != 0 or missing:
            report.fail('method-names', 'gen_skeleton.py does not spell {} as codegen.jar '
                                        'does{}'.format(', '.join(missing) or 'the names',
                                                        (': ' + made.stderr[-160:])
                                                        if made.returncode else ''))
            return

        def contract():
            done = subprocess.run([sys.executable, os.path.join(tools, 'check_contract.py'),
                                   '.', '--strict', '--allow-todo'],
                                  cwd=holder, capture_output=True, text=True)
            return done.stdout + done.stderr

        clean = contract()
        if 'ERROR' in clean:
            report.fail('method-names', 'check_contract.py rejects the names codegen.jar '
                                        'generates: ' + next(
                                            line for line in clean.splitlines()
                                            if 'ERROR' in line))
            return
        consumer = glob.glob(os.path.join(holder, 'src', 'consumer', '*Consumer.cpp'))[0]
        with open(consumer, encoding='utf-8') as handle:
            text = handle.read()
        with open(consumer, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write(text.replace('mDeadline.stop_timer();',
                                      'mDeadline.stop_timer();\n        '
                                      'request_open_valve();', 1))
        if 'P-02' not in contract():
            report.fail('method-names', 'check_contract.py accepts request_open_valve '
                                        'for a document request OpenValve, which the base '
                                        'does not declare')
            return
        with open(consumer, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write(text.replace('mDeadline.stop_timer();',
                                      'mDeadline.stop_timer();\n        '
                                      'mFsm.request_status();', 1))
        if 'P-02' in contract():
            report.fail('method-names', 'check_contract.py reports mFsm.request_status(), '
                                        'a call on another object, as a member no .siml '
                                        'declares')
            return
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('method-names', 'a method keeps its document spelling after its prefix in '
                              'api.json, the skeleton and the contract check, and a '
                              'snake-cased spelling of it is reported')


COLLIDING_SIML = """<?xml version="1.0" encoding="utf-8"?>
<ServiceInterface FormatVersion="1.1.0">
    <Overview ID="1" Name="Collide" Version="1.0.0" Category="Public"/>
    <AttributeList>
        <Attribute ID="2" Name="Level" DataType="int32" Notify="OnChange"/>
    </AttributeList>
    <MethodList>
        <Method ID="3" Name="set_level" MethodType="Request" Response="set_level">
            <ParamList>
                <Parameter ID="4" Name="value" DataType="int32"/>
            </ParamList>
        </Method>
        <Method ID="5" Name="set_level" MethodType="Response"/>
        <Method ID="6" Name="open_valve" MethodType="Request" Response="open_valve"/>
        <Method ID="7" Name="open_valve" MethodType="Response"/>
    </MethodList>
</ServiceInterface>
"""


def check_accessor_collision(report):
    """An attribute accessor is a legal bare call even when a request shares its name.

    An attribute Level generates set_level(), is_level_valid() and invalidate_level(),
    none of which carries a request_, response_ or broadcast_ prefix. A request named
    set_level beside it made check_contract.py rule P-02 report the accessor the
    skeleton itself writes, and the runbook forbids editing the generated base the
    message points at.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    try:
        os.makedirs(os.path.join(holder, 'src', 'services'))
        with open(os.path.join(holder, 'src', 'services', 'Collide.siml'), 'w',
                  encoding='utf-8', newline='\n') as handle:
            handle.write(COLLIDING_SIML)
        made = subprocess.run([sys.executable, os.path.join(tools, 'gen_skeleton.py'),
                               '--doc', 'src/services/Collide.siml', '--app', '--mode',
                               'ipc', '--force', '--scenarios', 'none.json'],
                              cwd=holder, capture_output=True, text=True)
        if made.returncode != 0:
            report.fail('accessor-collision', 'gen_skeleton.py --app failed on an '
                                              'attribute and a request of the same '
                                              'name: ' + made.stderr[-160:])
            return

        def contract():
            done = subprocess.run([sys.executable, os.path.join(tools, 'check_contract.py'),
                                   '.', '--strict', '--allow-todo'],
                                  cwd=holder, capture_output=True, text=True)
            return done.stdout + done.stderr

        clean = contract()
        if 'ERROR' in clean:
            report.fail('accessor-collision', 'check_contract.py rejects the code '
                                              'gen_skeleton.py wrote: ' + next(
                                                  line for line in clean.splitlines()
                                                  if 'ERROR' in line))
            return
        provider = glob.glob(os.path.join(holder, 'src', 'provider', '*Provider.cpp'))[0]
        with open(provider, encoding='utf-8') as handle:
            text = handle.read()
        with open(provider, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write(text.replace('set_level(0);',
                                      'set_level(0);\n    open_valve();', 1))
        if 'P-02' not in contract():
            report.fail('accessor-collision', 'check_contract.py no longer reports a '
                                              'bare request call: the accessor exemption '
                                              'disabled the rule')
            return
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('accessor-collision', 'an attribute accessor whose name collides with a '
                                    'request is accepted, and a bare request call is '
                                    'still reported')


STEP_SAMPLE = [{'name': 'open_gate', 'send': 'open', 'args': {'width': 600}},
               {'name': 'settle', 'wait': 100},
               {'name': 'close_gate', 'send': 'close', 'args': {'by': 'night shift'}},
               {'name': 'watch_width', 'await': 'Width'}]

STEP_REFUSALS = [({'name': 'fly', 'send': 'fly'}, 'is not a request'),
                 ({'name': 'open_gate', 'send': 'open'}, 'gives no value'),
                 ({'name': 'both', 'await': 'Width', 'wait': 10}, 'does one of the two'),
                 ({'name': 'done', 'wait': 10}, 'the driver declares itself (Start, Done'),
                 ({'name': 'nothing', 'await': 'Nobody'}, 'no response, broadcast'),
                 ({'name': 'odd_width', 'send': 'open', 'args': {'width': 3}},
                  'takes only 600, 1200, 2000')]


LATE_SAMPLE = [{'name': 'open_gate', 'send': 'open', 'args': {'width': 600}},
               {'name': 'watch_width', 'await': 'Width'},
               {'name': 'watch_moved', 'await': 'gate_moved'},
               {'name': 'reopen', 'send': 'open', 'args': {'width': 1200}, 'await': 'Recent'}]


def check_self_transition(report):
    """A transition to its own state runs neither exit nor entry, and a design that
    relies on them is told so."""
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('self-transition', 'gen_docs.py does not import: {}'.format(failure))
        return

    def notes(target, moves=None, repeat=1):
        held = io.StringIO()
        with contextlib.redirect_stdout(held):
            gen_docs.review({'interfaces': [], 'machines': [{
                'name': 'M', 'timers': [{'name': 'T', 'timeout': 100, 'repeat': repeat}],
                'initial': 'A', 'states': [
                    {'name': 'A', 'entry': ['start T'], 'exit': ['stop T'],
                     'transitions': moves or [{'on': 'T', 'to': target}]},
                    {'name': 'B', 'transitions': [{'on': 'T', 'to': 'A'}]}]}]}, 0)
        return held.getvalue()

    if 'goes to itself' not in notes('A'):
        report.fail('self-transition', 'a state with "entry" that goes to itself earns no '
                                       'note, and its entry does not run again')
        return
    if 'goes to itself' in notes('B'):
        report.fail('self-transition', 'a transition to another state earns the note of '
                                       'a transition to its own state')
        return
    leave = {'on': 'T', 'to': 'B', 'guard': ['lit:1', 'eq', 'lit:2']}
    if 'stays on timer' not in notes('', [leave, {'on': 'T', 'do': ['a']}]):
        report.fail('self-transition', 'a state that stays on a timer its entry starts, '
                                       'and starts it nowhere again, earns no note')
        return
    for moves, repeat in (([leave, {'on': 'T', 'do': ['a', 'start T']}], 1),
                          ([{'on': 'T', 'do': ['a']}, {'on': 'T', 'do': ['start T']}], 1),
                          ([leave, {'on': 'T', 'do': ['a']}], 0)):
        if 'stays on timer' in notes('', moves, repeat):
            report.fail('self-transition', 'a timer started again, or repeating, earns '
                                           'the note of a timer that fires once')
            return
    with open(os.path.join(ROOT, 'docs', 'agent', '22-state-machine.md'),
              encoding='utf-8') as handle:
        page = handle.read()
    if 'or to its own state' not in page:
        report.fail('self-transition', '22-state-machine.md does not say a transition to '
                                       'its own state runs in place')
        return
    report.ok('self-transition', 'a transition to its own state is documented and noted '
                                 'as running neither exit nor entry; a timer that cannot '
                                 'fire again in the state that stays on it is noted')


def check_late_arrival(report):
    """An update or broadcast that arrives before the step awaiting it is kept for it.

    It is kept from the last request on, and the step that awaits it without sending
    runs its check on it once. A replay does not run the arrival body a second time.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('late-arrival', 'gen_docs.py does not import: {}'.format(failure))
        return
    held = io.StringIO()
    with contextlib.redirect_stdout(held):
        gen_docs.review({'machines': [], 'interfaces': [{
            'name': 'S', 'attributes': [{'name': 'X'}], 'broadcasts': [{'name': 'B'}],
            'requests': [{'name': 'ask', 'answer': [{'name': 'ok'}]}],
            'steps': [{'name': 'a', 'send': 'ask'}, {'name': 'b', 'await': 'X'},
                      {'name': 'c', 'await': 'B'}]}]}, 0)
    if held.getvalue().strip():
        report.fail('late-arrival', 'a design awaiting an update or broadcast after a '
                                    'request still earns a note the driver made moot: '
                                    + held.getvalue().strip()[:160])
        return
    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        made = generate_application(tools, 'late', LATE_SAMPLE)
        if not os.path.isfile(made):
            report.fail('late-arrival', made)
            return
        with open(made, encoding='utf-8') as handle:
            source = handle.read()
        with open(made[:-4] + '.hpp', encoding='utf-8') as handle:
            source += handle.read()

        def body_of(signature):
            found = re.search(re.escape(signature) + r'[^{]*\{(.*?)\n\}', source, re.S)
            return found.group(1) if found else ''

        width = body_of('::on_width_update(')
        moved = body_of('::broadcast_gate_moved(')
        begin = body_of('::begin(Step step)')
        replay = body_of('::replay_late()')
        wanted = (
            ('an update dropped on another step is latched',
             re.search(r'dropped\("update Width"\);\s*mLateWidth = true;', width)),
            ('a broadcast dropped on another step keeps its arguments',
             re.search(r'dropped\("broadcast gate_moved"\);\s*mLateGateMoved = true;\s*'
                       r'mLateGateMovedWidth = width;\s*mLateGateMovedReading = reading;',
                       moved)),
            ('the arrival body is not run again on a replay',
             re.search(r'if \(mReplaying == false\)\s*\{\s*// TODO\(you\) update_width',
                       width)),
            ('a step that sends forgets what arrived before it',
             re.search(r'case Step::Reopen:[^;]*;\s*forget_late\(\);\s*request_open\(1200\);',
                       begin)),
            ('a step that awaits without sending replays what is latched',
             re.search(r'case Step::WatchWidth:[^;]*;\s*if \(mLateWidth\)\s*\{\s*'
                       r'mLate\.start_timer\(', begin)),
            ('the replay hands the held value to the check',
             re.search(r'case Step::WatchWidth:.*?width\(lateState\).*?on_width_update\(',
                       replay, re.S)),
            ('no local of the replay can be spelled as a getter',
             not re.search(r'\b(?:auto|areg::DataState)\s+[a-z_0-9]+\b', replay)),
            ('the replay hands the kept arguments to the check',
             re.search(r'case Step::WatchMoved:.*?broadcast_gate_moved\('
                       r'mLateGateMovedWidth, mLateGateMovedReading\)', replay, re.S)),
        )
        for what, found in wanted:
            if not found:
                report.fail('late-arrival', 'the step driver does not hold a late arrival: '
                            'nothing generated shows that ' + what)
                return
        if 'mLateRecent' in source:
            report.fail('late-arrival', 'an attribute awaited only by a step that sends '
                                        'is latched, and nothing replays it')
            return
        with open('worksheet.txt', encoding='utf-8') as handle:
            sheet = handle.read()
        ordered = re.search(r'// TODO\(you\) broadcast_gate_moved:[^\n]*runs before the '
                            r'step_ check of the same broadcast.*?switch \(mStep\)',
                            moved, re.S)
        if not ordered or not re.search(r'== broadcast_gate_moved\n#\| [^\n]*runs before '
                                        r'the step_ check of the same broadcast', sheet):
            report.fail('broadcast-order', 'a stepped broadcast_ section does not say it '
                                           'runs before the step_ check of the same '
                                           'broadcast, or the handler no longer does so')
        else:
            report.ok('broadcast-order', 'a stepped broadcast_ section says it runs before '
                                         'the step_ check of the same broadcast')
        sent = re.search(r'== step_reopen\n#\| [^\n]*before this step\'s request', sheet)
        awaited = re.search(r'== step_watch_width\n#\| [^\n]*before this step\'s request',
                            sheet)
        if not sent or awaited:
            report.fail('in-flight-update', 'the step_ section of a step that sends and '
                                            'awaits an update does not say an update sent '
                                            'before its request can arrive first, or a step '
                                            'that sends nothing says so too')
        else:
            report.ok('in-flight-update', 'a step that sends and awaits an update is told an '
                                          'update sent before its request can arrive first')
        if 'test the value already held' in sheet or 'dropped there' in sheet:
            report.fail('late-arrival', 'the worksheet of a stepped design still asks the '
                                        'author to handle an update that arrived early')
            return
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
        holder = tempfile.mkdtemp()
        os.chdir(holder)
        made = generate_application(tools, 'prompt', LATE_SAMPLE[:1])
        with open(made, encoding='utf-8') as handle:
            if 'mLate' in handle.read():
                report.fail('late-arrival', 'a design with no step awaiting without a '
                                            'request still gets the latch')
                return
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('late-arrival',
              'an update or broadcast arriving before the step that awaits it is kept '
              'from the last request on and checked once, without its arrival body')


def check_step_fall_through(report):
    """Every step_ section names the step its check falls through to, and a new
    generation names each section whose target moved."""
    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    first = [{'name': 'open_gate', 'send': 'open', 'args': {'width': 600}},
             {'name': 'watch_width', 'await': 'Width'}]
    try:
        os.chdir(holder)
        made = generate_application(tools, 'fall', first)
        if not os.path.isfile(made):
            report.fail('fall-through', made)
            return

        def section(name):
            with open('worksheet.txt', encoding='utf-8') as handle:
                found = re.search(r'^== {}\n((?:#\|.*\n)*)'.format(name), handle.read(),
                                  re.M)
            return found.group(1) if found else ''

        if 'falling through begins Step::WatchWidth' not in section('step_open_gate') \
                or 'falling through ends the run' not in section('step_watch_width'):
            report.fail('fall-through', 'a step_ section does not say which step its '
                                        'check falls through to, so a step inserted '
                                        'after it re-targets the check unseen')
            return
        with open('worksheet.txt', encoding='utf-8') as handle:
            sheet = re.sub(r'\n#\| ', ' ', handle.read())
        claims = [phrase for phrase in ('prints only', 'prints nothing', '"step ')
                  if phrase in sheet]
        if claims or 'Each line comes from a body above' not in sheet:
            report.fail('step-print', 'a stepped worksheet says what generated code '
                                      'prints ({}), and a run opens the sources to check '
                                      'it'.format(', '.join(claims) or 'no rule instead'))
        else:
            report.ok('step-print', 'a stepped worksheet says each expected line comes '
                                    'from a body, and claims nothing about generated code')
        design = json.load(open('design.json', encoding='utf-8'))
        design['interfaces'][0]['steps'] = [first[0], {'name': 'settle', 'wait': 100},
                                            {'name': 'reopen', 'send': 'open',
                                             'args': {'width': 300}}, first[1]]
        with open('design.json', 'w', encoding='utf-8', newline='\n') as handle:
            json.dump(design, handle)
        docs = sorted(glob.glob(os.path.join('src', 'services', '*.siml')))
        machines = sorted(glob.glob(os.path.join('src', 'services', '*.fsml')))
        again = [sys.executable, os.path.join(tools, 'gen_skeleton.py'), '--doc', docs[0],
                 '--app', '--mode', 'ipc', '--force', '--spec', 'design.json']
        if machines:
            again += ['--machine', machines[0]]
        said = subprocess.run(again, capture_output=True, text=True).stdout
        if not re.search(r'step_open_gate\b.*Step::Settle.*Step::WatchWidth', said):
            report.fail('fall-through', 'a generation that moved where a check falls '
                                        'through does not name it: {}'
                        .format(said.strip()[-200:]))
            return
        report.ok('fall-through', 'every step_ section names where its check falls '
                                  'through, and a generation names each one that moved')
        if not re.search(r'\bstep_reopen\b', said) or 'applies to it as before' in said:
            report.fail('new-section', 'a generation that added a worksheet section does '
                                       'not name it, so bodies.txt is written without it: {}'
                        .format(said.strip()[-200:]))
            return
        same = subprocess.run(again, capture_output=True, text=True).stdout
        if 'applies to it as before' not in same or 'step_reopen' in same:
            report.fail('new-section', 'a generation that added no section no longer says '
                                       'bodies.txt applies as before: {}'
                        .format(same.strip()[-200:]))
            return
        design['interfaces'][0]['steps'].insert(-1, {'name': 'recheck', 'send': 'open',
                                                     'args': {'width': 200}})
        with open('design.json', 'w', encoding='utf-8', newline='\n') as handle:
            json.dump(design, handle)
        with open('bodies.txt', 'w', encoding='utf-8', newline='\n') as handle:
            handle.write('== step_recheck\n    // any answer\n')
        written = subprocess.run(again, capture_output=True, text=True).stdout
        if 'step_recheck' in written or 'applies to it as before' not in written:
            report.fail('new-section', 'a generation says bodies.txt has no body for a '
                                       'section it already holds: {}'
                        .format(written.strip()[-200:]))
            return
        scaffold = json.load(open('scenarios.json', encoding='utf-8'))
        scaffold['scenarios'] = scaffold['scenarios'][:1]
        scaffold['scenarios'][0]['scaffold'] = True
        with open('scenarios.json', 'w', encoding='utf-8', newline='\n') as handle:
            json.dump(scaffold, handle)
        fresh = ' '.join(subprocess.run(again, capture_output=True,
                                        text=True).stdout.split())
        if 'wrote scenarios.json' not in fresh or 'prints nothing' in fresh:
            report.fail('build-print', 'a stepped generation that writes scenarios.json '
                                       'says the generated code prints nothing, against '
                                       'the worksheet and the step lines it prints')
            return
        report.ok('build-print', 'a stepped generation makes no claim on what the '
                                 'generated code prints; the worksheet states it once')
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('new-section', 'a generation names each worksheet section it added and '
                             'bodies.txt lacks, and otherwise says bodies.txt applies as before')


def check_step_values_split(report):
    """The example sends a total its values lack as one step per listed value."""
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('values-split', 'gen_docs.py does not import: {}'.format(failure))
        return
    iface = gen_docs.EXAMPLE['interfaces'][0]
    steps = iface.get('steps') or []
    listed = {}
    for request in iface.get('requests') or []:
        for param in request.get('params') or []:
            if param.get('values'):
                listed[(request['name'], param['name'])] = set(param['values'])
    for first, second in zip(steps, steps[1:]):
        if first.get('send') and first.get('send') == second.get('send'):
            for (request, param), values in listed.items():
                if request != first['send']:
                    continue
                parts = (first.get('args', {}).get(param), second.get('args', {}).get(param))
                if all(part in values for part in parts) and sum(parts) not in values:
                    report.ok('values-split', 'the example sends a total its values lack '
                                              'as one step per listed value')
                    return
    report.fail('values-split', 'the example never sends a total its values lack as one '
                                'step per listed value')


def check_example_until(report):
    """The example proves a request's work by a broadcast with until, never an attribute."""
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('example-until', 'gen_docs.py does not import: {}'.format(failure))
        return
    iface = gen_docs.EXAMPLE['interfaces'][0]
    attributes = set(entry['name'] for entry in iface.get('attributes') or [])
    broadcasts = set(entry['name'] for entry in iface.get('broadcasts') or [])
    held = [step['name'] for step in iface.get('steps') or []
            if step.get('until') and step.get('await') in attributes]
    if held:
        report.fail('example-until', 'the example holds {} until an attribute reaches a '
                    'value: its first update after a request can be the value from before '
                    'it'.format(', '.join(held)))
        return
    if not any(step.get('until') and step.get('await') in broadcasts
               for step in iface.get('steps') or []):
        report.fail('example-until', 'no step of the example awaits a broadcast with until')
        return
    report.ok('example-until', 'the example awaits a broadcast with until, and no attribute')


def check_stall_default(report):
    """The default stall outlasts the default reconnect by the margin, and refusing a
    stall no longer than the reconnect names a value that repairs each."""
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
        import docmodel
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('stall-default', 'gen_docs.py does not import: {}'.format(failure))
        return
    stall = gen_docs.driver_of({})['stall_ticks']
    if stall != gen_docs.DRIVER_DEFAULTS['reconnect_seconds'] + gen_docs.STALL_MARGIN_TICKS:
        report.fail('stall-default', 'the default stall is {} ticks, not the default '
                    'reconnect plus the margin'.format(stall))
        return
    problems = []
    with docmodel.gathering(problems), contextlib.suppress(docmodel.Refused):
        gen_docs.check_drivers({'interfaces': [{'name': 'X', 'driver': {
            'reconnect_seconds': 20, 'stall_ticks': 20}}]})
    if not problems or 'stall_ticks 21 or more, or reconnect_seconds 19 or less' \
            not in problems[0]:
        report.fail('stall-default', 'a stall equal to the reconnect is accepted, or its '
                    'refusal names no value that repairs it: {}'
                    .format(problems[0] if problems else 'accepted'))
        return
    report.ok('stall-default', 'the default stall is {} ticks, and the refusal of a stall '
              'equal to the reconnect names both repairs'.format(stall))


def check_stall_names_latest_drops(report):
    """The stall report names the latest messages dropped, not the first ones.

    The drop on the step before the stall is the one that explains it, and a long
    scenario drops many harmless messages before it.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        made = generate_application(tools, 'stall', STEP_SAMPLE)
        if not os.path.isfile(made):
            report.fail('stall-drops', made)
            return
        with open(made, encoding='utf-8') as handle:
            source = handle.read()
        with open(made[:-4] + '.hpp', encoding='utf-8') as handle:
            source += handle.read()
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)

    def body_of(signature):
        found = re.search(re.escape(signature) + r'[^{;]*\{(.*?)\n\}', source, re.S)
        return found.group(1) if found else ''

    kept = body_of('::dropped(const char * what)')
    shown = body_of('::stalled()')
    if not kept or not shown:
        report.fail('stall-drops', 'a stepped driver no longer defines dropped() and '
                                   'stalled(), so this check reads nothing')
        return
    if not re.search(r'mDroppedCount\s*%\s*cDroppedMost', kept) \
            or re.search(r'if\s*\(\s*\w+\s*<\s*cDroppedMost', kept) \
            or not re.search(r'%\s*cDroppedMost', shown):
        report.fail('stall-drops', 'the stall report keeps the first messages dropped and '
                                   'counts the rest, so the drop just before the stall is '
                                   'hidden behind "and N more"')
        return
    report.ok('stall-drops', 'the stall report names the latest messages dropped')


def check_step_enum_qualifier(report):
    """A step giving an enumeration field under another type's name is refused by
    gen_docs, every such step in one answer, and every spelling of the right type
    is taken."""
    tools = os.path.join(ROOT, 'tools', 'agent')
    design = json.loads(subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                        '--example'], capture_output=True, text=True).stdout)
    service = design['interfaces'][0]
    service.setdefault('types', []).append(
        {'name': 'Pace', 'kind': 'enum', 'values': [{'name': 'Slow'}, {'name': 'Fast'}]})
    service['requests'].append({'name': 'grade', 'params': [
        {'name': 'quality', 'type': 'GateTypes::Quality'}, {'name': 'pace', 'type': 'Pace'}]})

    def grade(name, quality, pace):
        return {'name': name, 'send': 'grade', 'args': {'quality': quality, 'pace': pace}}

    holder = tempfile.mkdtemp()
    try:
        path = os.path.join(holder, 'design.json')

        def generate(steps):
            service['steps'] = steps
            with open(path, 'w', encoding='utf-8', newline='\n') as handle:
                json.dump(design, handle)
            out = os.path.join(holder, 'out')
            shutil.rmtree(out, ignore_errors=True)
            done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                   '--spec', path, '--outdir', out],
                                  capture_output=True, text=True)
            return done.returncode, done.stdout + done.stderr, os.path.isdir(out)

        code, said, wrote = generate([grade('g_one', 'Other::Good', 'Slow'),
                                      grade('g_two', 'Suspect', 'Wrong::Fast')])
        if code == 0 or wrote or 'is not that type' not in said \
                or 'g_one' not in said or 'g_two' not in said:
            report.fail('step-enum-qualifier', 'a step giving an enumeration field under '
                                               'another type is not refused by gen_docs, '
                                               'every one in one answer, before a document '
                                               'is written: {}'.format(said.strip()[-240:]))
            return
        code, said, _ = generate([grade('g_bare', 'Good', 'Fast'),
                                  grade('g_type', 'Quality::Good', 'Pace::Fast'),
                                  grade('g_full', 'GateTypes::Quality::Good',
                                        'GateService::Pace::Fast')])
        if code != 0:
            report.fail('step-enum-qualifier', 'gen_docs refuses a spelling of the right '
                                               'enumeration type: {}'
                        .format(said.strip()[-240:]))
            return
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('step-enum-qualifier', 'gen_docs refuses every step giving an enumeration '
                                     'field under another type, and takes every spelling '
                                     'of the right one')


def check_spec_value_shapes(report):
    """A known spec key given a value of the wrong type is refused by name, not
    ended in a traceback."""
    tools = os.path.join(ROOT, 'tools', 'agent')
    design = json.loads(subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                        '--example'], capture_output=True, text=True).stdout)
    shapes = (('types', 5), ('broadcasts', 5), ('constants', 5),
              ('requests.0.answer', 5), ('requests.0.name', {'a': 1}),
              ('requests.0.name', ['x']), ('requests.0.name', 7))
    holder = tempfile.mkdtemp()
    try:
        for where, value in shapes:
            spec = json.loads(json.dumps(design))
            node = spec['interfaces'][0]
            *path, key = where.split('.')
            for part in path:
                node = node[int(part)] if part.isdigit() else node[part]
            node[key] = value
            path = os.path.join(holder, 'design.json')
            with open(path, 'w', encoding='utf-8', newline='\n') as handle:
                json.dump(spec, handle)
            done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                   '--spec', path, '--outdir', os.path.join(holder, 'out')],
                                  capture_output=True, text=True)
            said = done.stdout + done.stderr
            if 'Traceback' in said or '"{}"'.format(key) not in said:
                report.fail('spec-value-shape', '"{}" given as {} ends in {} instead of a '
                                                'refusal naming the key'
                            .format(where, json.dumps(value),
                                    said.strip().splitlines()[-1][:120] if said.strip()
                                    else 'nothing'))
                return
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('spec-value-shape', '{} wrong-typed spec values are each refused by name'
              .format(len(shapes)))


HOSTED_SAMPLE = {
    'interfaces': [{
        'name': 'LoopService', 'category': 'Public', 'description': 'Runs passes.',
        'requests': [{'name': 'begin', 'description': 'Start.', 'params': []},
                     {'name': 'advance', 'description': 'Advance the pass running now.',
                      'params': []}]}],
    'machines': [
        {'name': 'Pass', 'description': 'One pass.',
         'triggers': [{'name': 'advance'}], 'actions': [{'name': 'on_pass'}],
         'conditions': [{'name': 'may_end'}], 'initial': 'RUNNING',
         'states': [{'name': 'RUNNING', 'entry': ['on_pass'],
                     'transitions': [{'on': 'advance', 'to': 'ENDED',
                                      'guard': {'call': 'may_end'}}]},
                    {'name': 'ENDED', 'kind': 'final'}]},
        {'name': 'Loop', 'description': 'Runs Pass twice.',
         'submachines': [{'name': 'Pass', 'path': 'src/services/Pass.fsml',
                          'version': '1.0.0'}],
         'triggers': [{'name': 'begin'}], 'actions': [{'name': 'on_rest'}],
         'events': [{'name': 'FirstEnded'}, {'name': 'SecondEnded'}], 'initial': 'REST',
         'states': [{'name': 'REST', 'entry': ['on_rest'],
                     'transitions': [{'on': 'begin', 'to': 'FIRST'}]},
                    {'name': 'FIRST', 'submachine': 'Pass', 'final_event': 'FirstEnded',
                     'transitions': [{'on': 'FirstEnded', 'to': 'SECOND'}]},
                    {'name': 'SECOND', 'submachine': 'Pass', 'final_event': 'SecondEnded',
                     'transitions': [{'on': 'SecondEnded', 'to': 'REST'}]}]}]}


def hosted_failure(tools):
    """What stops a machine that hosts another from becoming an application, or ''."""
    import importlib.util
    if subprocess.run([sys.executable, os.path.join(tools, 'setup_project.py'),
                       '--name', 'loop', '--root', '.', '--mode', 'ipc', '--sdk-root', ROOT],
                      capture_output=True, text=True).returncode != 0:
        return 'the scaffold no longer lays out a project'

    def generate(design):
        with open('design.json', 'w', encoding='utf-8', newline='\n') as handle:
            json.dump(design, handle, indent=2)
        done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                               '--outdir', 'src/services', '--force', '--chained',
                               '--spec', 'design.json'], capture_output=True, text=True)
        return (done.stdout + done.stderr).strip().splitlines()[-1:] \
            if done.returncode else None

    refused = generate(HOSTED_SAMPLE)
    if refused:
        return 'gen_docs.py refuses a design whose machine hosts another: {}'.format(refused)
    alone = copy.deepcopy(HOSTED_SAMPLE)
    alone['machines'] = alone['machines'][1:]
    refused = generate(alone)
    if refused:
        return ('gen_docs.py refuses a machine hosting a .fsml the project already holds: '
                '{}'.format(refused))
    spec = importlib.util.spec_from_file_location(
        'hosted_build', os.path.join(tools, 'build_project.py'))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    if not hasattr(module, 'built_machines'):
        return 'build_project.py counts a hosted machine as one more to build'
    found = module.documents_of(['design.json'], os.path.join('src', 'services'))
    chosen = module.built_machines(found[1], found[3])
    if [os.path.basename(path) for path in chosen] != ['Loop.fsml']:
        return 'build_project.py would build {} instead of the host alone'.format(chosen)
    done = subprocess.run([sys.executable, os.path.join(tools, 'gen_skeleton.py'),
                           '--doc', 'src/services/LoopService.siml', '--app', '--mode',
                           'ipc', '--force', '--machine', 'src/services/Loop.fsml'],
                          capture_output=True, text=True)
    if done.returncode:
        return 'gen_skeleton.py --app refuses the host: {}'.format(
            (done.stdout + done.stderr).strip().splitlines()[-1:])
    text = ''
    for source in glob.glob(os.path.join('src', 'provider', 'LoopServiceProvider.*')):
        with open(source, encoding='utf-8') as handle:
            text += handle.read()
    with open('worksheet.txt', encoding='utf-8') as handle:
        sheet = handle.read()
    if len(re.findall(r'static_cast<PassActionHandler &>\(self\(\)\)', text)) != 2:
        return 'the provider does not pass one handler per hosting state to the machine'
    for name in ('action_on_pass', 'may_end'):
        if len(re.findall(r'\b{}\('.format(name), text)) != 2:
            return 'the provider does not implement {} of the hosted machine once'.format(name)
    for wanted in ('== action_on_pass', '== condition_may_end',
                   'hosted by Loop in FIRST, SECOND', 'bool advance_first()'):
        if wanted not in sheet:
            return 'the worksheet does not carry "{}"'.format(wanted)
    return ''


def check_hosted_machine(report, tools=None):
    """A machine that hosts another, from two states, becomes an application: with
    both in the design, and with the hosted one already on disk."""
    tools = tools or os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        failure = hosted_failure(tools)
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
    if failure:
        report.fail('hosted-machine', failure)
        return
    report.ok('hosted-machine', 'a machine hosting another from two states generates an '
                                'application, whether the hosted one is designed with it '
                                'or already on disk')


def hosted_path_failure(tools):
    """What a hosted machine of the same design is refused for over its "path", or ''."""
    import importlib.util

    def generate(design, outdir):
        with open('design.json', 'w', encoding='utf-8', newline='\n') as handle:
            json.dump(design, handle, indent=2)
        done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                               '--outdir', outdir, '--force', '--spec', 'design.json'],
                              capture_output=True, text=True)
        texts = {}
        for path in glob.glob(os.path.join(outdir, '*')):
            with open(path, encoding='utf-8') as handle:
                texts[os.path.basename(path)] = handle.read()
        return done.returncode, (done.stdout + done.stderr).strip(), texts

    code, said, named = generate(HOSTED_SAMPLE, os.path.join('src', 'services'))
    if code:
        return 'the design with its path refused: {}'.format(said.splitlines()[-1:])
    bare = copy.deepcopy(HOSTED_SAMPLE)
    del bare['machines'][1]['submachines'][0]['path']
    code, said, left = generate(bare, os.path.join('src', 'services'))
    if code:
        return 'a hosted machine of the design with no "path" is refused: {}'.format(
            said.splitlines()[-1:])
    if left != named:
        return 'a hosted machine with no "path" writes other documents than with its path'
    spec = importlib.util.spec_from_file_location(
        'hosted_path_build', os.path.join(tools, 'build_project.py'))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    found = module.documents_of(['design.json'], os.path.join('src', 'services'))
    chosen = [os.path.basename(path) for path in module.built_machines(found[1], found[3])]
    if chosen != ['Loop.fsml']:
        return 'build_project.py builds {} for a host whose hosted path is left out'.format(
            chosen)
    unlisted = copy.deepcopy(HOSTED_SAMPLE)
    del unlisted['machines'][1]['submachines']
    code, said, left = generate(unlisted, os.path.join('src', 'services'))
    if code:
        return 'a hosted machine of the design with no "submachines" entry is refused: ' \
               '{}'.format(said.splitlines()[-1:])
    if left != named:
        return 'a hosted machine with no "submachines" entry writes other documents'
    found = module.documents_of(['design.json'], os.path.join('src', 'services'))
    chosen = [os.path.basename(path) for path in module.built_machines(found[1], found[3])]
    if chosen != ['Loop.fsml']:
        return 'build_project.py builds {} for a host with no "submachines" entry'.format(
            chosen)
    unknown = copy.deepcopy(unlisted)
    unknown['machines'][1]['states'][1]['submachine'] = 'Elsewhere'
    code, said, _ = generate(unknown, os.path.join('src', 'services'))
    if not code or '"path"' not in said:
        return 'a hosted name that is no machine of the design is not refused with the ' \
               'entry form'
    wrong = copy.deepcopy(HOSTED_SAMPLE)
    wrong['machines'][1]['submachines'][0]['path'] = 'src/other/Pass.fsml'
    code, said, _ = generate(wrong, os.path.join('src', 'services'))
    if not code or 'src/services/Pass.fsml' not in said:
        return ('a path naming no file for a machine of the design is not refused '
                'with the right one')
    return ''


def hosted_counter_failure(tools):
    """What is wrong with the advice for a hosted counter that never restarts, or ''.

    The note names the hosted machine and its initial state, the reset at the start of a
    visit, and that the host cannot set it; a design that follows it draws no note."""
    def review(design):
        holder = tempfile.mkdtemp()
        try:
            spec = os.path.join(holder, 'design.json')
            with open(spec, 'w', encoding='utf-8') as handle:
                json.dump(design, handle)
            done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                   '--review', '--spec', spec], capture_output=True,
                                  text=True, cwd=holder)
            return done.returncode, done.stdout + done.stderr
        finally:
            shutil.rmtree(holder, ignore_errors=True)

    counting = copy.deepcopy(HOSTED_SAMPLE)
    inner = counting['machines'][0]
    inner['attributes'] = [{'name': 'Count', 'type': 'uint32', 'value': '0'}]
    inner['states'][0]['transitions'].append(
        {'on': 'advance', 'set': {'Count': 'raw:mAttrCount + 1'}})
    code, said = review(counting)
    note = ' '.join(line.strip() for line in said.splitlines()
                    if 'only ever set from its own value' in line or line.startswith(' ' * 6))
    if code or 'only ever set from its own value' not in said:
        return 'a hosted counter only ever set from itself draws no note: {}'.format(
            said.strip().splitlines()[-1:])
    for wanted in ('inside Pass', '"entry" of "RUNNING"', '"lit:0"', 'host cannot set it'):
        if wanted not in note:
            return 'the hosted-counter note does not say {}: "{}"'.format(wanted, note[:300])
    fixed = copy.deepcopy(counting)
    inner = fixed['machines'][0]
    inner['events'] = [{'name': 'PassStarted'}]
    inner['states'][0]['entry'] = ['on_pass', {'send': 'PassStarted'}]
    inner['states'][0]['transitions'].append({'on': 'PassStarted', 'set': {'Count': 'lit:0'}})
    code, said = review(fixed)
    if code:
        return 'the design that follows the note is refused: {}'.format(
            said.strip().splitlines()[-1:])
    if 'only ever set from its own value' in said:
        return 'the design that follows the note still draws it'
    return ''


def check_hosted_counter(report, tools=None):
    """A hosted counter that never restarts is told to reset inside the hosted machine as a
    visit starts, and the design that does so is accepted and no longer noted."""
    failure = hosted_counter_failure(tools or os.path.join(ROOT, 'tools', 'agent'))
    if failure:
        report.fail('hosted-counter', failure)
        return
    report.ok('hosted-counter', 'a hosted counter is told to reset inside its machine as a '
              'visit starts; the design that does so is accepted and not noted')


def empty_answer_failure(tools):
    """What is wrong with "answer": [], or '': it declares a response, and the template and
    the service page both say so."""
    example = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'), '--example'],
                             capture_output=True, text=True)
    design = json.loads(example.stdout)
    request = design['interfaces'][0]['requests'][1]
    request['answer'] = []
    holder = tempfile.mkdtemp()
    try:
        spec = os.path.join(holder, 'design.json')
        with open(spec, 'w', encoding='utf-8') as handle:
            json.dump(design, handle)
        done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'), '--outdir',
                               os.path.join(holder, 'out'), '--force', '--spec', spec],
                              capture_output=True, text=True)
        siml = ''.join(open(path, encoding='utf-8').read()
                       for path in glob.glob(os.path.join(holder, 'out', '*.siml')))
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    if done.returncode or 'Name="{}" MethodType="Response"'.format(request['name']) not in siml:
        return '"answer": [] no longer declares a response'
    holder = tempfile.mkdtemp()
    try:
        path = os.path.join(holder, 'design.json')
        subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'), '--template', path],
                       capture_output=True, text=True)
        template = open(path, encoding='utf-8').read() if os.path.isfile(path) else ''
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    if 'even []' not in template:
        return 'the design template does not say that "answer": [] still has a response'
    if '`"answer": []` has an' not in read('docs', 'agent', '20-service-interface.md'):
        return '20-service-interface.md does not say that "answer": [] still has a response'
    return ''


def check_empty_answer(report, tools=None):
    """An empty answer declares a response, and the pages that define "answer" say so."""
    failure = empty_answer_failure(tools or os.path.join(ROOT, 'tools', 'agent'))
    if failure:
        report.fail('empty-answer', failure)
        return
    report.ok('empty-answer', '"answer": [] declares a response with no value, and the '
              'template and the service page say so')


def check_hosted_path(report, tools=None):
    """A hosted machine of the same design needs no "path" and no "submachines" entry, and
    a wrong path is refused with the path the design writes it to."""
    tools = tools or os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        failure = hosted_path_failure(tools)
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
    if failure:
        report.fail('hosted-path', failure)
        return
    report.ok('hosted-path', 'a hosted machine of the same design takes its path and its '
                             '"submachines" entry from the design; a path naming no file is '
                             'refused with the right one')


def installed_scaffold_failure(prefix):
    """What a project made from an installed SDK names that does not exist, or ''."""
    tools = os.path.join(prefix, 'tools', 'areg')
    sdk = os.path.join(prefix, 'share', 'areg', 'sdk')
    shutil.copytree(os.path.join(ROOT, 'tools'), tools,
                    ignore=shutil.ignore_patterns('intern', '__pycache__', '*.jar'))
    shutil.copytree(os.path.join(ROOT, 'docs', 'agent'), os.path.join(sdk, 'docs', 'agent'),
                    ignore=shutil.ignore_patterns('build', 'generated'))
    shutil.copy(os.path.join(ROOT, 'AGENTS.md'), sdk)
    setup = os.path.join(tools, 'agent', 'setup_project.py')
    for kind in ('scaffold', 'attach'):
        project = os.path.join(prefix, kind)
        os.makedirs(project)
        if kind == 'attach':
            with open(os.path.join(project, 'main.cpp'), 'w', encoding='utf-8') as handle:
                handle.write('int main() { return 0; }\n')
        done = subprocess.run([sys.executable, setup, '--name', 'lamp', '--root', project,
                               '--mode', 'ipc', '--quiet'], capture_output=True, text=True)
        if done.returncode:
            return 'the {} from an installation fails: {}'.format(
                kind, (done.stdout + done.stderr).strip().splitlines()[-1:])
        with open(os.path.join(project, 'AGENTS.md'), encoding='utf-8') as handle:
            text = handle.read()
        if 'build/packages/' in text:
            return 'the {} from an installation names a fetch path'.format(kind)
        named = set(re.findall(r'(?:python3? |`)((?:[A-Za-z]:)?/[^\s`]+\.py)',
                               text.replace('\\', '/')))
        missing = sorted(path for path in named if not os.path.isfile(path))
        if not named or missing:
            return 'the {} from an installation names {}'.format(
                kind, ', '.join(missing) if missing else 'no tool by its path')
        moved = '`<areg-sdk>/tools/`, the tools are in `{}/`'.format(tools.replace('\\', '/'))
        if moved not in text:
            return ('the {} from an installation does not say where the tools a page '
                    'writes as <areg-sdk>/tools/ are'.format(kind))
    return ''


def check_installed_scaffold(report):
    """A project scaffolded or attached from an installed SDK, with no --sdk-root, names
    tools that exist in that installation."""
    holder = tempfile.mkdtemp()
    try:
        failure = installed_scaffold_failure(holder)
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    if failure:
        report.fail('installed-scaffold', failure)
        return
    report.ok('installed-scaffold', 'a project made from an installed SDK names the '
                                    'installed tools')


def check_unused_parameters(report):
    """A generated definition whose body is the author's marks each parameter its
    generated lines do not read [[maybe_unused]], and the worksheet shows none."""
    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        made = generate_application(tools, 'unused', STEP_SAMPLE)
        if not os.path.isfile(made):
            report.fail('unused-parameter', made)
            return
        sources = sorted(glob.glob(os.path.join('src', '*', '*.cpp')))
        with open('worksheet.txt', encoding='utf-8') as handle:
            sheet = handle.read()
        missed, marked = [], 0
        for source in sources:
            with open(source, encoding='utf-8') as handle:
                text = handle.read()
            for head, body in re.findall(r'^(\S[^\n]*\([^\n]*\)[^\n]*)\n\{\n(.*?)\n\}',
                                         text, re.M | re.S):
                if 'TODO(you)' not in body:
                    continue
                inner = head[head.find('(') + 1:head.rfind(')')]
                code = re.sub(r'"(?:\\.|[^"\\])*"', '""', '\n'.join(
                    line for line in body.splitlines()
                    if 'TODO(you)' not in line and 'placeholder(you)' not in line))
                for part in (inner.split(',') if inner.strip() else []):
                    name = re.findall(r'\w+', part)[-1]
                    if re.search(r'\b{}\b'.format(name), code):
                        continue
                    if '[[maybe_unused]]' in part:
                        marked += 1
                    else:
                        missed.append('{} in {}'.format(name, head.strip()))
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
    if missed or not marked:
        report.fail('unused-parameter', 'a parameter the author\'s body may ignore is not '
                                        '[[maybe_unused]], so the body warns under -Wextra and '
                                        'MSVC /W4: {}'.format('; '.join(missed[:3]) or 'none marked'))
        return
    if '[[maybe_unused]]' in sheet:
        report.fail('unused-parameter', 'the worksheet shows [[maybe_unused]] in a signature '
                                        'it lists')
        return
    report.ok('unused-parameter', '{} parameter(s) an author\'s body may ignore are '
                                  '[[maybe_unused]], and the worksheet lists the signatures '
                                  'as before'.format(marked))


def check_step_driver(report):
    """A declared step sequence becomes a driver, and no rule of the problem is generated.

    Opt-in: a design without steps gets no driver. With steps, every request, wait and
    exit code of the sequence is generated, a step that awaits something carries one
    marker for its check and no code, the result passes the contract, and a step naming
    what the service does not declare is refused before any document is written.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        made = generate_application(tools, 'plain')
        if not os.path.isfile(made):
            report.fail('step-driver', made)
            return
        with open(made[:-4] + '.hpp', encoding='utf-8') as handle:
            if 'enum class Step' in handle.read():
                report.fail('step-driver', 'a design with no steps got a step driver: the '
                                           'driver is no longer opt-in')
                return
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
        holder = tempfile.mkdtemp()
        os.chdir(holder)
        made = generate_application(tools, 'stepped', STEP_SAMPLE)
        if not os.path.isfile(made):
            report.fail('step-driver', made)
            return
        with open(made, encoding='utf-8') as handle:
            source = handle.read()
        for wanted, what in (('request_open(600);', 'the request with its argument'),
                             ('request_close("night shift");',
                              'a request with no answer, and a String argument the '
                              'design writes as plain text and C++ needs quoted'),
                             ('hold(100);', 'a timed wait'),
                             ('quit_with(0);', 'the exit after the last step'),
                             ('begin(Step::OpenGate);', 'the first step')):
            if wanted not in source:
                report.fail('step-driver', 'the driver does not write {}: "{}" is missing'
                            .format(what, wanted))
                return
        # A wait left running while the provider is away ends its step, and a last
        # step that waits then exits 0 with no provider at all.
        lost = re.search(r'ServiceConnectionState::ConnectionLost\)\)(.*?)'
                         r'ServiceConnectionState::Rejected', source, re.S)
        if lost is None or 'mHold.stop_timer();' not in lost.group(1) or \
                'hold(mHoldMs);' not in source.split('ServiceConnectionState::Disconnected')[0]:
            report.fail('step-driver', 'losing the provider does not stop the wait of the '
                                       'current step, or reconnecting does not start it '
                                       'again: a run whose steps left are waits exits 0 '
                                       'without its provider')
            return
        if 'case Step::PeerLostHold:  return "step_peer_lost_hold";' not in source:
            report.fail('step-driver', 'a failure during the generated hold step names '
                                       '"no step" instead of the step')
            return
        checks = sorted(re.findall(r'TODO\(you\) (step_\w+):', source))
        if checks != ['step_open_gate', 'step_watch_width']:
            report.fail('step-driver', 'the steps that await something should carry one '
                                       'marker each, step_open_gate and step_watch_width; '
                                       'found {}'.format(checks or 'none'))
            return
        if 'TODO(you) response_open:' in source or \
                'TODO(you) broadcast_gate_moved:' not in source:
            report.fail('step-driver', 'an awaited answer still carries its generic marker, '
                                       'or a broadcast no step awaits lost its own')
            return
        # The check stands alone inside a braced region, behind StepEnd. Unbraced, a
        # check that declares a local makes the compiler refuse every case label after
        # it, and the error names those labels rather than the declaration. Without
        # StepEnd first, a check that returns skips complete(), and a go_to() it made
        # is discarded: the run stalls with the answer already in hand.
        lines = source.splitlines()
        for index, line in enumerate(lines):
            if 'TODO(you) step_' not in line:
                continue
            around = [lines[index - 2].strip(), lines[index - 1].strip(),
                      lines[index + 1].strip(), lines[index + 2].strip()]
            if around != ['{', 'StepEnd ending(*this);', '}', 'break;']:
                report.fail('step-driver',
                            'the check of a step is not a braced region carrying '
                            'StepEnd and the marker alone: the generator wrote {} '
                            'around it'
                            .format(' / '.join('"{}"'.format(text) for text in around)))
                return
        # StepEnd is what makes every exit from a check end the step. A destructor
        # that stops calling complete() brings the discarded-go_to() stall back.
        header = glob.glob(os.path.join('src', 'consumer', '*Consumer.hpp'))
        guard = ''
        if header:
            with open(header[0], encoding='utf-8') as handle:
                guard = handle.read()
        if 'struct StepEnd' not in guard or not re.search(
                r'~StepEnd\(void\)\s*\{[^}]*complete\(\);', guard, re.S):
            report.fail('step-driver', 'the consumer has no StepEnd whose destructor '
                                       'calls complete(): a check that returns after '
                                       'go_to() would stall the run')
            return
        # Ending the step twice is as wrong as never ending it, and it is silent:
        # the step after the one a body chose is entered and its check never runs.
        # The guard arms mEnding, complete() disarms it, the destructor obeys it.
        source = glob.glob(os.path.join('src', 'consumer', '*Consumer.cpp'))
        body = ''
        if source:
            with open(source[0], encoding='utf-8') as handle:
                body = handle.read()
        if not re.search(r'StepEnd\([^)]*\)\s*:\s*mOwner\(owner\)\s*\{\s*'
                         r'mOwner\.mEnding = true;', guard, re.S) \
                or not re.search(r'~StepEnd\(void\)\s*\{[^}]*if \(mOwner\.mEnding\)',
                                 guard, re.S) \
                or not re.search(r'::complete\(\)\s*\{\s*mEnding = false;',
                                 guard + body, re.S):
            report.fail('step-driver', 'StepEnd does not end the step at most once: a '
                                       'body that calls complete() itself would advance '
                                       'twice and silently skip the next check')
            return
        found = subprocess.run([sys.executable, os.path.join(tools, 'check_contract.py'),
                                '.', '--strict', '--allow-todo'],
                               capture_output=True, text=True)
        if 'ERROR' in found.stdout + found.stderr:
            report.fail('step-driver', 'the driver breaks the contract: ' + next(
                line for line in (found.stdout + found.stderr).splitlines()
                if 'ERROR' in line))
            return
        with open('design.json', encoding='utf-8') as handle:
            design = json.load(handle)
        for step, words in STEP_REFUSALS:
            design['interfaces'][0]['steps'] = [step]
            with open('bad.json', 'w', encoding='utf-8', newline='\n') as handle:
                json.dump(design, handle)
            done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                   '--spec', 'bad.json', '--outdir', 'refused'],
                                  capture_output=True, text=True)
            if done.returncode == 0 or words not in done.stderr or \
                    os.path.isdir('refused'):
                report.fail('step-driver', 'step {} is not refused before a document is '
                                           'written with "{}": {}'
                            .format(json.dumps(step), words,
                                    (done.stderr or done.stdout).strip()[:160]))
                return
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('step-driver', 'a declared step sequence generates its driver with one marker '
                             'per check and no rule of its own, passes the contract, is '
                             'opt-in, and {} malformed steps are refused'
              .format(len(STEP_REFUSALS)))


def check_peer_loss_branch(report):
    """The consumer can answer a peer that went away, where the peer going away arrives.

    Disconnected and ConnectionLost are the states a killed provider produces, and the
    framework reconnects from them, so nothing there may quit. They used to share a
    branch with Rejected and Shutdown, where a body could not run at all: two measured
    runs proved peer loss with a 20 second stall watchdog instead of a detection that
    arrives in about one second.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        made = generate_application(tools, 'peer')
        if not os.path.isfile(made):
            report.fail('peer-loss', made)
            return
        with open(made, encoding='utf-8') as handle:
            lines = handle.read().splitlines()

        start = None
        for index, line in enumerate(lines):
            if 'ServiceConnectionState::Disconnected' in line or \
                    'ServiceConnectionState::ConnectionLost' in line:
                start = index
                break
        if start is None:
            report.fail('peer-loss',
                        'the generated consumer names neither Disconnected nor '
                        'ConnectionLost, so the states a killed provider produces reach '
                        'no branch and peer loss can only be proved by a stall')
            return

        depth, body, seen = 0, [], False
        for line in lines[start:]:
            depth += line.count('{') - line.count('}')
            body.append(line)
            if seen and depth <= 0:
                break
            if '{' in line:
                seen = True
        text = '\n'.join(body)
        if 'TODO(you)' not in text:
            report.fail('peer-loss',
                        'the branch that sees a provider go away carries no marker, so '
                        'there is nowhere to write what losing it means')
            return
        for quit_call in ('quit_with', 'signal_quit'):
            if quit_call in text:
                report.fail('peer-loss',
                            'the branch for Disconnected and ConnectionLost calls {}. '
                            'The framework reconnects from both, so quitting there turns '
                            'a provider restart into a dead application'.format(quit_call))
                return
        if 'Rejected' in text or 'Shutdown' in text:
            report.fail('peer-loss',
                        'Disconnected shares its branch with a terminal state again: a '
                        'body written there runs on the states a killed provider never '
                        'produces')
            return
        guard = text.find('is_quitting()')
        if guard < 0 or guard > text.find('TODO(you)'):
            report.fail('peer-loss',
                        'the marker for a provider that went away is not under an '
                        'is_quitting() guard, so it also runs while this process quits '
                        'and its peer is torn down on purpose. A scenario that passed '
                        'every step then reports a lost peer and exits non-zero')
            return
        mains = sorted(glob.glob(os.path.join('src', 'consumer', 'main.cpp')))
        source = ''
        if mains:
            with open(mains[0], encoding='utf-8') as handle:
                source = handle.read()
        if 'bool is_quitting()' not in source:
            report.fail('peer-loss',
                        'the consumer main() defines no is_quitting(), so the guard the '
                        'generated branch reads does not link')
            return
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)

    report.ok('peer-loss', 'the generated consumer answers a provider that went away '
                           'where that arrives, with a marker and without quitting')


def check_final_entry_rule(report):
    """Rule 108 is reported before the document is written as well as after.

    A nested final state carrying entry operations was reported only by
    check_contract.py, which runs after two generators have, so a design was written,
    refused, edited and regenerated. The rule is the
    same rule, so the number comes from the shared catalogue in both tools rather
    than being written down twice.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
        import check_contract
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('final-entry', 'the rule tools do not import: {}'.format(failure))
        return

    number = gen_docs.rule_number(gen_docs.FINAL_ENTRY_RULE, gen_docs.WARNING_BAND)
    if number is None:
        report.fail('final-entry', 'gen_docs.py cannot read the rule catalogue, so it '
                                   'has no number to report a nested final entry under')
        return
    if number != check_contract.FINAL_ENTRY_CODE:
        report.fail('final-entry',
                    'gen_docs.py reports rule {} and check_contract.py reports {}. One '
                    'rule read out of one catalogue is what stops the two drifting'
                    .format(number, check_contract.FINAL_ENTRY_CODE))
        return

    # Through the command line, not the function: a check nobody calls reports
    # nothing, and that is the failure this case exists to catch.
    tools = os.path.join(ROOT, 'tools', 'agent')
    shown = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                            '--example'], capture_output=True, text=True)
    try:
        example = json.loads(shown.stdout)
    except ValueError:
        report.fail('final-entry', 'gen_docs.py --example no longer prints a spec')
        return

    def nested_final(machine):
        """The machine's final state inside a composite that reports it finished."""
        def walk(states, parent):
            for state in states or []:
                if (parent is not None and parent.get('final_event')
                        and str(state.get('kind', '')).lower() == 'final'):
                    return state
                found = walk(state.get('states'), state)
                if found is not None:
                    return found
            return None
        return walk(machine.get('states'), None)

    target = None
    for machine in example.get('machines') or []:
        target = nested_final(machine)
        if target is not None:
            break
    if target is None:
        report.fail('final-entry', 'the example spec carries no final state inside a '
                                   'composite, so this rule is exercised by nothing')
        return
    action = (example['machines'][0].get('actions') or [{}])[0].get('name')
    if not action:
        report.fail('final-entry', 'the example machine declares no action to place')
        return

    holder = tempfile.mkdtemp()
    here = os.getcwd()

    def generate(spec_doc, outdir):
        path = os.path.join(holder, outdir + '.json')
        with open(path, 'w', encoding='utf-8', newline='\n') as handle:
            json.dump(spec_doc, handle, indent=2)
        out = os.path.join(holder, outdir)
        done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                               '--outdir', out, '--spec', path],
                              capture_output=True, text=True, cwd=holder)
        written = sorted(os.listdir(out)) if os.path.isdir(out) else []
        return done, written

    try:
        os.chdir(holder)
        clean, _ = generate(example, 'clean')
        if clean.returncode != 0:
            report.fail('final-entry', 'the example spec no longer generates: {}'
                        .format((clean.stderr or clean.stdout).strip()[:160]))
            return

        broken = copy.deepcopy(example)
        nested_final(broken['machines'][0])['entry'] = [action]
        refused, written = generate(broken, 'broken')
        if refused.returncode == 0:
            report.fail('final-entry',
                        'gen_docs.py writes a machine whose nested final state carries '
                        'entry operations. The rule is then reported only after two '
                        'generators have run, and the spec is edited and regenerated')
            return
        if number not in (refused.stderr or '') + (refused.stdout or ''):
            report.fail('final-entry',
                        'the design-time refusal does not name rule {}, so it cannot be '
                        'looked up or silenced'.format(number))
            return
        if written:
            report.fail('final-entry',
                        'the spec was refused and {} document(s) were written anyway; '
                        'a refusal after writing is what costs the regeneration'
                        .format(len(written)))
            return

        escaped = copy.deepcopy(broken)
        nested_final(escaped['machines'][0])['description'] = \
            'areg-check: ignore ' + number
        allowed, _ = generate(escaped, 'escaped')
        if allowed.returncode != 0:
            report.fail('final-entry',
                        'the escape check_contract.py documents does not work at design '
                        'time, so a deliberate case cannot be written at all')
            return
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)

    report.ok('final-entry', 'rule {} is reported by gen_docs.py before a document is '
                             'written and by check_contract.py after, from one '
                             'catalogue'.format(number))


# What "short enough to read in one call" is allowed to mean, in 01-runbook.md.
EXAMPLE_LINES = 400
EXAMPLE_BYTES = 12 * 1024


def check_example_size(report):
    """The runbook says --example is short enough to read in one call.

    A run that is not told its length pages it, and pays two requests for one
    answer. The page now says, and the
    claim is only true while the template stays short. A number on the page would go
    stale instead, so the page carries the promise and this carries the measurement.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')
    spec = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                           '--example'], capture_output=True, text=True)
    if spec.returncode != 0:
        report.fail('example-size', 'gen_docs.py --example no longer prints a spec')
        return
    lines = len(spec.stdout.splitlines())
    size = len(spec.stdout.encode('utf-8'))
    if lines > EXAMPLE_LINES or size > EXAMPLE_BYTES:
        report.fail('example-size',
                    'gen_docs.py --example is {} line(s) and {} bytes, over the {} and '
                    '{} a run can take in one call. 01-runbook.md promises it is short '
                    'enough to read whole; a run that does not believe that pages it, '
                    'and pays a request for the second half'
                    .format(lines, size, EXAMPLE_LINES, EXAMPLE_BYTES))
        return
    runbook = os.path.join(ROOT, 'docs', 'agent', '01-runbook.md')
    with open(runbook, encoding='utf-8') as handle:
        page = ' '.join(handle.read().split())
    # The promise has to be attached to the command, not merely somewhere on the page,
    # and it has to carry the length: a run that is told only "short" caps the output
    # at a guess of its own and pays a request for the remainder.
    promised = False
    stated = None
    at = page.find('--example')
    while at >= 0:
        near = page[at:at + 200]
        if 'one call' in near:
            promised = True
            found = re.search(r'(\d+) lines', near)
            stated = int(found.group(1)) if found else None
            break
        at = page.find('--example', at + 1)
    if not promised:
        report.fail('example-size',
                    '01-runbook.md no longer says, where it names --example, that it '
                    'reads in one call, so nothing stops a run paging it')
        return
    if stated is None:
        report.fail('example-size',
                    '01-runbook.md says --example reads in one call but not how long it '
                    'is. A run then caps it at a guess of its own and pays a second '
                    'request for the rest; a length on the page is what stops that')
        return
    if stated != lines:
        report.fail('example-size',
                    '01-runbook.md says --example is {} lines and it is {}. A run that '
                    'trusts the page truncates the spec; one that does not pages it'
                    .format(stated, lines))
        return
    report.ok('example-size',
              'gen_docs.py --example is {} line(s) and {} bytes, and 01-runbook.md says '
              'it reads in one call'.format(lines, size))


def check_worksheet_order_note(report):
    """The worksheet says a response and an update are two independent deliveries.

    The fact is on 20-service-interface.md and 31-consumer.md, and AGENTS.md tells a
    run filling a marker not to open either. A run that opens neither guesses an
    order, stalls for the whole watchdog and pays a run-and-fix cycle to find the
    worked example that page already carries. The worksheet is the one file every run
    reads, so the fact is written in its header: it is true of every body that waits,
    and writing it under the first response body put it under whichever body came
    first rather than the one the reader is filling.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_skeleton
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('order-note', 'gen_skeleton.py does not import: {}'.format(failure))
        return

    def sections(names):
        return [(name, 'hint', 'src/consumer/C.cpp', 'C.cpp', '') for name in names]

    def ordered(notes):
        return [name for name, lines in notes.items() if lines == gen_skeleton.ORDER_NOTE]

    full = sections(['first_request', 'response_go', 'response_stop', 'update_level'])
    for kind, lines in (('stepped', gen_skeleton.STEPPED_ORDER_NOTE),
                        ('plain', gen_skeleton.ORDER_NOTE)):
        said = ' '.join(lines)
        if 'in the order sent' not in said or 'one event at a time' not in said \
                or 'Neither waits' in said:
            report.fail('order-facts',
                        'the {} worksheet note does not say that what a provider sends '
                        'arrives in send order and that the provider handles one event at '
                        'a time, so a run designs around a race that cannot happen'
                        .format(kind))
            return
    report.ok('order-facts', 'the worksheet says what a provider sends arrives in send '
                             'order, and that a request is handled whole')
    if gen_skeleton.header_notes(full) != gen_skeleton.ORDER_NOTE:
        report.fail('order-note',
                    'the worksheet header does not carry the note, so nothing a run '
                    'reads before it starts filling says a response and an update are '
                    'two deliveries that do not wait for each other')
        return
    if ordered(gen_skeleton.section_notes(full)):
        report.fail('order-note',
                    'the note is on a section as well as the header: it is one fact '
                    'and it is paid for once per place it is written')
        return
    for only in (['response_go'], ['update_level']):
        if gen_skeleton.header_notes(sections(only)):
            report.fail('order-note',
                        'a worksheet with only {} bodies carries the note anyway, and '
                        'there is nothing for the other delivery to race with'
                        .format(only[0].split('_')[0]))
            return

    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        if subprocess.run([sys.executable, os.path.join(tools, 'setup_project.py'),
                           '--name', 'note', '--root', '.', '--mode', 'ipc',
                           '--sdk-root', ROOT],
                          capture_output=True, text=True).returncode != 0:
            report.fail('order-note', 'the scaffold no longer lays out a project')
            return
        spec = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                               '--example'], capture_output=True, text=True)
        with open('design.json', 'w', encoding='utf-8', newline='\n') as handle:
            handle.write(spec.stdout)
        if subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                           '--outdir', 'src/services', '--force', '--chained',
                           '--spec', 'design.json'],
                          capture_output=True, text=True).returncode != 0:
            report.fail('order-note', 'the example spec no longer generates')
            return
        docs = sorted(glob.glob(os.path.join('src', 'services', '*.siml')))
        machines = sorted(glob.glob(os.path.join('src', 'services', '*.fsml')))
        # With --spec the steps are generated, and a step that waits gives the
        # consumer its mHold branch. Without it the skeleton has no steps, its
        # process_timer is six lines shorter, and the quoting below is measured
        # against a consumer no real project has.
        made = [sys.executable, os.path.join(tools, 'gen_skeleton.py'),
                '--doc', docs[0], '--app', '--mode', 'ipc', '--force',
                '--spec', 'design.json', '--scenarios', 'scenarios.json']
        if machines:
            made += ['--machine', machines[0]]
        if subprocess.run(made, capture_output=True, text=True).returncode != 0:
            report.fail('order-note', 'gen_skeleton.py --app no longer writes a worksheet')
            return
        if not os.path.exists(gen_skeleton.WORKSHEET):
            report.fail('order-note', 'no worksheet was written to carry the note')
            return
        with open(gen_skeleton.WORKSHEET, encoding='utf-8') as handle:
            sheet = handle.read()
        if gen_skeleton.ORDER_NOTE[0] not in sheet:
            report.fail('order-note',
                        'the worksheet of a generated application carries no note about '
                        'the order a response and an update arrive in, so the fact is '
                        'only on the pages a run filling markers is told not to open')
            return
        first_section = next((n for n, line in enumerate(sheet.splitlines())
                              if line.startswith('== ')), None)
        note_at = next(n for n, line in enumerate(sheet.splitlines())
                       if gen_skeleton.ORDER_NOTE[0] in line)
        if first_section is None or note_at > first_section:
            report.fail('order-note',
                        'the note sits under a section rather than in the header, so a '
                        'run filling a different section never reads it')
            return
        # The worksheet names mPace, cStallTicks, mStep and complete() as taken and
        # says nothing about when they fire or when mStep moves. One measured run
        # opened seven generated files for that; each function that decides it is a
        # dozen lines. A limit that quietly drops one is the same as not quoting it.
        head = gen_skeleton.DRIVEN_HEAD.splitlines()[0]
        if head not in sheet:
            report.fail('order-note',
                        'the worksheet lists the timers and the step helpers a '
                        'generated consumer owns and quotes none of them, so a body '
                        'that has to know when they run has to open a generated file')
            return
        quoted = sheet.split(head)[-1]
        for wanted, _ in gen_skeleton.DRIVEN_BY:
            if '::{}('.format(wanted) not in quoted:
                report.fail('order-note',
                            'the worksheet does not quote the generated {}(), so a '
                            'body that has to know what it does opens the generated '
                            'file it is in. A body over the quoting limit is dropped '
                            'without a word: raise DRIVEN_LIMIT, do not shorten the '
                            'answer'.format(wanted))
                return
        # A signature with no behaviour behind it is a name, and a name is a guess.
        for helper, said in (('void complete()', 'Ends the current step'),
                             ('void stay()', 'Keeps the current step')):
            spelt = [line for line in sheet.splitlines() if line.endswith(helper)]
            if not spelt:
                report.fail('order-note',
                            'the worksheet no longer lists "{}" among the names a body '
                            'may call'.format(helper))
                return
            if said not in sheet:
                report.fail('order-note',
                            'the worksheet lists "{}" and not what it does, so the '
                            'header it was read from is opened for the one line that '
                            'was already written there'.format(helper))
                return
        # A step that stalls names the step. What the step sent and waits for is what
        # turns that into a cause, and both are known when the switch is written.
        with open(os.path.join('src', 'consumer',
                               'GateServiceConsumer.cpp'), encoding='utf-8') as handle:
            consumer = handle.read()
        for wanted, why in (
                ('step_detail', 'a stalled run names the step and not what it awaits'),
                ('dropped(', 'a message arriving on a step with no check for it is '
                             'discarded with no trace, and the step that wanted it '
                             'then waits for ever with nothing to read')):
            if wanted not in consumer:
                report.fail('order-note',
                            'the generated consumer carries no {}: {}'
                            .format(wanted, why))
                return
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)

    report.ok('order-note', 'the worksheet says a response and an update are two '
                            'deliveries, once, before any section; quotes every '
                            'generated body a name does not give away; says what each '
                            'helper does; and a stall names what it waited for')


def check_command_coverage(report):
    """--deep classifies materially more of the corpus as runnable than the fast run.

    The fast run executes the commands that need nothing -- a handful of them -- and
    checks the flags of the rest against --help, which a command that parses and no
    longer works passes. An audit priced that at a guarantee over 8% of the commands
    a corpus gives. This asks the classifier, not the commands: running them is the
    CI step, and what rots here is the allowlist that decides which ones are tried.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import check_commands
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('command-depth', 'check_commands.py does not import: {}'
                    .format(failure))
        return
    if not hasattr(check_commands, 'DEEP') or not check_commands.DEEP:
        report.fail('command-depth', 'check_commands.py offers no --deep allowlist, so '
                                     'the commands that need a project or a build are '
                                     'never executed anywhere')
        return

    total = 0
    counts = {False: 0, True: 0}
    for document in check_commands.AGENT_DOCS:
        if not os.path.isfile(os.path.join(ROOT, document)):
            continue
        for _number, command in check_commands.blocks(document):
            command = check_commands.substitute(command)
            total += 1
            for deep in (False, True):
                if check_commands.classify(command, deep)[0] == 'RUN':
                    counts[deep] += 1
    if not total:
        report.fail('command-depth', 'check_commands.py finds no command in the agent '
                                     'corpus at all')
        return
    if counts[True] <= counts[False]:
        report.fail('command-depth', '--deep would run {} command(s) and the fast run '
                    '{}: the deep allowlist covers nothing the fast one does not'
                    .format(counts[True], counts[False]))
        return
    report.ok('command-depth', '--deep runs {} of the {} documented command(s), against '
              '{} without it'.format(counts[True], total, counts[False]))


def check_empty_section_note(report):
    """The worksheet says how a section that needs nothing is closed, in its preamble.

    The convention was printed only inside a step_ section, and the "*_state" sections
    are the ones a design most often needs nothing in. A run that left them empty
    built, passed both scenarios and then failed the final contract check on P-17,
    paying one worksheet pass, one full rebuild and two turns after it had already
    reported success.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_skeleton
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('empty-section', 'gen_skeleton.py does not import: {}'.format(failure))
        return
    if 'a "//" comment saying so' not in gen_skeleton.WORKSHEET_HEAD:
        report.fail('empty-section', 'the worksheet preamble does not say that a section '
                                     'needing nothing is closed by one // line, so the '
                                     'convention is stated only inside a step_ section')
        return

    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        os.makedirs(os.path.join('src', 'services'))
        with open(os.path.join('src', 'services', 'Collide.siml'), 'w',
                  encoding='utf-8', newline='\n') as handle:
            handle.write(COLLIDING_SIML)
        if subprocess.run([sys.executable, os.path.join(tools, 'gen_skeleton.py'),
                           '--doc', 'src/services/Collide.siml', '--app', '--mode', 'ipc',
                           '--force', '--scenarios', 'none.json'],
                          capture_output=True, text=True).returncode != 0:
            report.fail('empty-section', 'gen_skeleton.py --app no longer writes a worksheet')
            return
        with open('worksheet.txt', encoding='utf-8') as handle:
            sheet = handle.read()
        filled = []
        for line in sheet.splitlines():
            filled.append(line)
            if line.strip() in ('== provider_state', '== consumer_state'):
                filled.append('// no state of its own')
        with open('bodies.txt', 'w', encoding='utf-8', newline='\n') as handle:
            handle.write('\n'.join(filled) + '\n')
        if subprocess.run([sys.executable, os.path.join(tools, 'fill_markers.py'),
                           '--bodies', 'bodies.txt'],
                          capture_output=True, text=True).returncode != 0:
            report.fail('empty-section', 'fill_markers.py refuses a section whose only '
                                         'line is a comment')
            return
        left = ''
        for path in glob.glob(os.path.join('src', '*', '*.hpp')):
            with open(path, encoding='utf-8') as handle:
                left += handle.read()
        # The anchors a filled body keeps also name the slot, so what says the marker
        # is still open is the marker itself.
        if 'TODO(you) provider_state' in left or 'TODO(you) consumer_state' in left:
            report.fail('empty-section', 'a "*_state" section closed by one // line '
                                         'leaves its marker open, so the convention the '
                                         'worksheet states does not work')
            return
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)

    report.ok('empty-section', 'the worksheet preamble states how a section needing '
                               'nothing is closed, and one // line closes a "*_state" '
                               'marker')


def check_regeneration_idempotent(report):
    """Regenerating an unchanged design touches no document, so no build goes stale.

    build_project.py --run regenerates every document of the project on every call.
    While gen_docs.py wrote them unconditionally, a byte-identical document still got
    a new modification time, the build had nothing to relink, and run_scenarios.py --
    which reads that time -- then refused the build of the very command that had just
    made it. Running it again widened the gap, and no command the message suggested
    cleared it: the documented definition-of-done command could not exit 0 twice.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import run_scenarios
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('regeneration-idempotent',
                    'run_scenarios.py does not import: {}'.format(failure))
        return

    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        if subprocess.run([sys.executable, os.path.join(tools, 'setup_project.py'),
                           '--name', 'again', '--root', '.', '--mode', 'ipc',
                           '--sdk-root', ROOT, '--quiet'],
                          capture_output=True, text=True).returncode != 0:
            report.fail('regeneration-idempotent', 'the scaffold no longer lays out a '
                                                   'project')
            return
        spec = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                               '--example'], capture_output=True, text=True)
        with open('design.json', 'w', encoding='utf-8', newline='\n') as handle:
            handle.write(spec.stdout)

        def generate():
            return subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                   '--outdir', 'src/services', '--force', '--chained',
                                   '--spec', 'design.json'],
                                  capture_output=True, text=True)

        if generate().returncode != 0:
            report.fail('regeneration-idempotent', 'the example spec no longer generates')
            return
        documents = sorted(glob.glob(os.path.join('src', 'services', '*.*ml')))
        if not documents:
            report.fail('regeneration-idempotent', 'no document was written to compare')
            return
        before = dict((path, os.stat(path)) for path in documents)

        # A build that ran after the documents were written, and relinked nothing
        # because nothing about them changed.
        os.makedirs(os.path.join('build', 'bin'))
        with open('scenarios.json', encoding='utf-8') as handle:
            scenarios = json.load(handle)
        names = set()
        for scenario in scenarios.get('scenarios') or []:
            for entry in scenario.get('procs') or []:
                if entry.get('binary'):
                    names.add(entry['binary'])
        for name in sorted(names):
            binary = os.path.join('build', 'bin', name + run_scenarios.SUFFIX)
            with open(binary, 'w', encoding='utf-8') as handle:
                handle.write('built\n')
            os.chmod(binary, 0o755)

        again = generate()
        if again.returncode != 0:
            report.fail('regeneration-idempotent',
                        'a second generation of the same design fails: {}'
                        .format((again.stderr or again.stdout).strip()[-160:]))
            return
        touched = [path for path in documents
                   if os.stat(path).st_mtime != before[path].st_mtime]
        if touched:
            report.fail('regeneration-idempotent',
                        '{} is byte-identical after a second generation and its '
                        'modification time moved, so every build made before it reads '
                        'as stale'.format(touched[0]))
            return
        stale = run_scenarios.stale_binaries(scenarios.get('scenarios') or [],
                                             ['build/bin'], '.')
        if stale:
            report.fail('regeneration-idempotent',
                        'run_scenarios.py refuses a build that nothing edited after: '
                        '{} against {}'.format(stale[0][0], stale[0][1]))
            return

        # The other half. A source time can move without its content moving -- a
        # checkout, a copy -- and the compiler then relinks nothing, so the guard
        # would refuse the build build_project.py has just made in this same command,
        # with no command able to clear it.
        driver = read('tools', 'agent', 'build_project.py')
        block = driver[driver.find("if args.run:"):]
        block = block[:block.find("if not args.no_check:")] if block else ''
        if "'--stale-ok'" not in block:
            report.fail('regeneration-idempotent',
                        'build_project.py --run hands its own fresh build to '
                        'run_scenarios.py without --stale-ok, so a source whose time '
                        'moved without its content leaves the run refusing a build it '
                        'just made')
            return
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)

    report.ok('regeneration-idempotent', 'a second generation of an unchanged design '
                                         'writes nothing, the build before it is still '
                                         'current, and build_project.py --run does not '
                                         'refuse the build it just made')


def check_regeneration_report(report):
    """A regeneration reports the markers of the files it kept, not of the ones it
    proposed.

    An existing file is kept, and the worksheet is what an agent fills next. Derived
    from the proposed text it names sections no file contains, and a run that fills
    it fills nothing: one worksheet pass plus one repair cycle for a design the
    project does not carry.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_skeleton
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('regeneration', 'gen_skeleton.py does not import: {}'.format(failure))
        return

    holder = tempfile.mkdtemp()
    try:
        path = os.path.join(holder, 'Kept.cpp')
        with open(path, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write('void filled() { work(); }\n')
        proposed = 'void filled() {\n    // TODO(you) filled: the rule\n}\n'
        back = gen_skeleton.write(path, proposed, False)
        if back != 'void filled() { work(); }\n':
            report.fail('regeneration', 'gen_skeleton.write() returns the proposed '
                        'text for a file it kept, so every marker report and the '
                        'worksheet describe a file that was never written')
            return
        written = gen_skeleton.write(os.path.join(holder, 'New.cpp'), proposed, False)
        if written != proposed:
            report.fail('regeneration', 'gen_skeleton.write() does not return what it '
                        'wrote for a new file')
            return
    finally:
        shutil.rmtree(holder, ignore_errors=True)

    body = read('tools', 'agent', 'gen_skeleton.py')
    for call in ('write_worksheet(retained', 'print_todos(retained'):
        if call not in body:
            report.fail('regeneration', 'the application branch does not pass the '
                        'retained sources to {}(): the report is derived from the '
                        'text that was proposed'.format(call.split('(')[0]))
            return
    report.ok('regeneration', 'a kept file reports its own content, and the worksheet '
              'is derived from the retained sources')


def check_step_output_whole(report):
    """A step shorter than its allowance prints all of it, not its last line.

    The documents step carries the design notes -- a step that awaits an update its
    own earlier request caused, a state an attribute cannot express. A tail allowance
    one line longer than the output once turned into a negative start index, showed
    the last line alone and warned about nothing, and a dropped note is rediscovered
    by hand over several requests.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import build_project
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('step-output', 'build_project.py does not import: {}'
                    .format(failure))
        return

    lines = ['line {}'.format(number) for number in range(1, 16)]
    for allowance in (1, 8, 15, 16, 40, None):
        printed = io.StringIO()
        held = sys.stdout
        sys.stdout = printed
        try:
            build_project.show(lines, allowance)
        finally:
            sys.stdout = held
        shown = [row.strip() for row in printed.getvalue().splitlines()
                 if not row.strip().startswith('...')]
        wanted = len(lines) if allowance is None else min(allowance, len(lines))
        if len(shown) != wanted:
            report.fail('step-output',
                        'a step of {} line(s) allowed {} printed {} of them. A log '
                        'shorter than its allowance has to arrive whole: the notes '
                        'that name a design defect are in it, and a request spent '
                        'finding one again is billed for the whole conversation'
                        .format(len(lines), allowance, len(shown)))
            return
        if shown[-1] != lines[-1]:
            report.fail('step-output',
                        'a step does not print its last line, which is its verdict')
            return
    report.ok('step-output',
              'a step prints its whole log when it is shorter than its allowance, '
              'and its last lines when it is longer')


def lay_example_app(tools, edit=None):
    """In the working directory: a scaffold, the example design and its application.

    "edit" changes the example design, as a dict, before anything is generated.

    Returns '' when every step ran, or what failed.
    """
    steps = [[os.path.join(tools, 'setup_project.py'), '--name', 'sheet', '--root', '.',
              '--mode', 'ipc', '--sdk-root', ROOT]]
    for command in steps:
        done = subprocess.run([sys.executable] + command, capture_output=True, text=True)
        if done.returncode != 0:
            return 'the scaffold no longer lays out a project: ' + done.stderr.strip()[:200]
    spec = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'), '--example'],
                          capture_output=True, text=True)
    design = json.loads(spec.stdout)
    if edit:
        edit(design)
    with open('design.json', 'w', encoding='utf-8') as handle:
        json.dump(design, handle, indent=2)
    done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'), '--outdir',
                           'src/services', '--force', '--chained', '--spec', 'design.json'],
                          capture_output=True, text=True)
    if done.returncode != 0:
        return 'the example no longer generates its documents: ' + done.stderr.strip()[:200]
    docs = sorted(os.listdir(os.path.join('src', 'services')))
    siml = next((d for d in docs if d.endswith('.siml')), None)
    fsml = next((d for d in docs if d.endswith('.fsml')), None)
    done = subprocess.run(
        [sys.executable, os.path.join(tools, 'gen_skeleton.py'), '--doc',
         'src/services/' + siml, '--app', '--mode', 'ipc', '--force', '--spec',
         'design.json'] + (['--machine', 'src/services/' + fsml] if fsml else []),
        capture_output=True, text=True)
    if done.returncode != 0:
        return 'the example no longer generates an application: ' + done.stderr.strip()[:200]
    return ''


def check_answer_file(report):
    """The generator writes the worksheet and never the file the bodies go in.

    A pre-written bodies file of empty sections is filled one Edit per section by
    some runs, one request per section.
    A file that does not exist yet can only be created whole.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')
    sys.path.insert(0, tools)
    import gen_skeleton
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        failed = lay_example_app(tools)
        if failed:
            report.fail('answer-file', failed)
            return
        if not os.path.exists(gen_skeleton.WORKSHEET):
            report.fail('answer-file', 'gen_skeleton.py --app writes no {}'
                        .format(gen_skeleton.WORKSHEET))
            return
        if os.path.exists(gen_skeleton.BODIES):
            report.fail('answer-file', 'gen_skeleton.py --app writes {} itself, so a run '
                        'finds a file of empty sections and may fill it one Edit at a '
                        'time'.format(gen_skeleton.BODIES))
            return
        with open(gen_skeleton.WORKSHEET, encoding='utf-8') as handle:
            sheet = handle.read()
        # An empty bodies file an older generator left is removed; one with work stays.
        for content, kept in ((sheet, False), ('== provider_state\n// mine\n', True)):
            with open(gen_skeleton.BODIES, 'w', encoding='utf-8') as handle:
                handle.write(content)
            machines = sorted(glob.glob('src/services/*.fsml'))
            again = subprocess.run(
                [sys.executable, os.path.join(tools, 'gen_skeleton.py'), '--doc',
                 sorted(glob.glob('src/services/*.siml'))[0], '--app', '--mode', 'ipc',
                 '--force', '--spec', 'design.json'] +
                (['--machine', machines[0]] if machines else []),
                capture_output=True, text=True)
            if again.returncode != 0:
                report.fail('answer-file', 'a regeneration failed: '
                            + again.stderr.strip()[:200])
                return
            if os.path.exists(gen_skeleton.BODIES) != kept:
                report.fail('answer-file', 'a regeneration {} a {} that {}'.format(
                    'removed' if kept else 'kept', gen_skeleton.BODIES,
                    'carries a body' if kept else 'holds only empty sections'))
                return
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('answer-file', 'the generator writes {} and never {}: the bodies file '
              'starts absent, so it is written whole'.format(gen_skeleton.WORKSHEET,
                                                             gen_skeleton.BODIES))


def stepped_evidence_failure(tools):
    """What is wrong with the expectation holes of the example, with and without steps,
    or ''. The working directory is a fresh one per call."""
    sys.path.insert(0, tools)
    import fill_markers
    import gen_skeleton
    import run_scenarios
    sys.path.pop(0)

    def no_steps(design):
        for entry in design['interfaces']:
            entry.pop('steps', None)

    here = os.getcwd()
    for edit in (None, no_steps):
        holder = tempfile.mkdtemp()
        try:
            os.chdir(holder)
            failed = lay_example_app(tools, edit)
            if failed:
                return failed
            with open('scenarios.json', encoding='utf-8') as handle:
                smoke = json.load(handle)['scenarios'][0]
            procs = smoke['procs']
            holes = [bool(p.get('expect')) for p in procs]
            if edit is no_steps:
                if not all(holes):
                    return 'with no steps a process of smoke holds no expectation hole'
                continue
            if holes != [False] * (len(procs) - 1) + [True]:
                return ('with steps smoke holds a hole on {}, not on the lead alone'
                        .format([p.get('name') or p['binary'] for p, h in zip(procs, holes)
                                 if h]))
            if run_scenarios.asserts_nothing(smoke):
                return 'with steps smoke asserts nothing'
            silent = gen_skeleton.expect_slot(smoke.get('name'), procs[0])
            with open('worksheet.txt', encoding='utf-8') as handle:
                sheet = handle.read()
            if re.search(r'^== {}$'.format(silent), sheet, re.MULTILINE) or \
                    'section == ' + silent not in re.sub(r'\s*\n#\|\s*', ' ', sheet):
                return ('the worksheet does not name {} beside the lead\'s expectations, '
                        'or offers it as a hole'.format(silent))
            with open('bodies.txt', 'w', encoding='utf-8') as handle:
                handle.write('== {}\nprovider: only it can show this\n'.format(silent))
            done = subprocess.run([sys.executable, os.path.join(tools, 'fill_markers.py'),
                                   '--bodies', 'bodies.txt'], capture_output=True, text=True)
            with open('scenarios.json', encoding='utf-8') as handle:
                first = json.load(handle)['scenarios'][0]['procs'][0]
            if first.get('expect') != ['provider: only it can show this']:
                return 'a section for the provider did not reach its "expect": {}'.format(
                    (done.stdout + done.stderr).strip().splitlines()[-1:])
        finally:
            os.chdir(here)
            shutil.rmtree(holder, ignore_errors=True)
    return ''


def check_stepped_evidence(report, tools=None):
    """With steps only the lead of smoke holds an expectation hole, and a section still
    gives the provider one; with no steps every process holds one."""
    failure = stepped_evidence_failure(tools or os.path.join(ROOT, 'tools', 'agent'))
    if failure:
        report.fail('stepped-evidence', failure)
        return
    report.ok('stepped-evidence', 'with steps the lead alone holds an expectation hole and '
              'the worksheet names the provider\'s optional section; with no steps every '
              'process holds one')


def check_peer_lost_scenario(report):
    """Every two-process project gets a peer-lost scenario the run does not write.

    Written by hand, it costs a page read and a scenario edit, and a trigger matched
    against the wrong process costs a build and a run. A stepped design gets the
    trigger written against a hold the generated consumer makes only when the scenario
    starts it with the hold flag, so no design needs a wait step for it, nor the design
    edit and the regeneration that adding one costs. A design that declares no steps
    gets the trigger as a single worksheet section, so it is not left looking in the
    generated sources for what to write.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')
    sys.path.insert(0, tools)
    import fill_markers
    import gen_skeleton

    def no_wait(design):
        for entry in design['interfaces']:
            entry['steps'] = [step for step in entry.get('steps', [])
                              if not step.get('wait')]

    def no_steps(design):
        for entry in design['interfaces']:
            entry.pop('steps', None)

    here = os.getcwd()
    for edit in (None, no_wait, no_steps):
        holder = tempfile.mkdtemp()
        try:
            os.chdir(holder)
            failed = lay_example_app(tools, edit)
            if failed:
                report.fail('peer-lost-scenario', failed)
                return
            with open('scenarios.json', encoding='utf-8') as handle:
                scenarios = json.load(handle)['scenarios']
            lost = [s for s in scenarios if isinstance(s.get('stop'), dict)]
            if not lost:
                report.fail('peer-lost-scenario', 'the example, two processes and '
                            '{} steps, gets no scenario that takes the provider away'
                            .format('no' if edit is no_steps else 'unheld'
                                    if edit is no_wait else 'held'))
                return
            lead = [p for p in lost[0]['procs'] if p.get('lead')]
            if not lead or lead[0].get('exit') != 1 or \
                    lost[0]['stop']['proc'] == lead[0].get('name'):
                report.fail('peer-lost-scenario', 'the scenario does not stop the '
                            'provider and expect exit 1 from the consumer leading it')
                return
            holes = fill_markers.expectations_of('scenarios.json')[1]
            slot = next((h for h in holes if h.startswith('stop_')), None)
            sheet = ''
            if os.path.exists('worksheet.txt'):
                with open('worksheet.txt', encoding='utf-8') as handle:
                    sheet = handle.read()
            if edit is not no_steps:
                kind = 'unheld' if edit is no_wait else 'held'
                trigger = lost[0]['stop']['after']
                if slot or '== stop_' in sheet:
                    report.fail('peer-lost-scenario', 'the example with {} steps leaves '
                                'the stop trigger to the run, so a design edit and a '
                                'regeneration follow: {}'.format(kind, trigger))
                    return
                hold = re.match(r'\^step (\w+)\$$', trigger)
                if hold is None or lead[0].get('args') != [gen_skeleton.HOLD_FLAG]:
                    report.fail('peer-lost-scenario', 'the example with {} steps is not '
                                'stopped at the consumer\'s own hold: trigger {}, lead '
                                'args {}'.format(kind, trigger, lead[0].get('args')))
                    return
                sources = {}
                for folder, _dirs, files in os.walk('src'):
                    for name in files:
                        with open(os.path.join(folder, name), encoding='utf-8') as handle:
                            sources[os.path.join(folder, name)] = handle.read()
                consumer = ''.join(text for name, text in sources.items()
                                   if name.endswith('Consumer.cpp') or
                                   name.endswith('Consumer.hpp'))
                mains = [text for name, text in sources.items()
                         if os.path.basename(name) == 'main.cpp' and 'hold_requested' in text]
                step = 'case Step::{}:'.format(gen_skeleton.pascal(hold.group(1)))
                if step not in consumer or 'if (mHoldOnce)' not in consumer or \
                        len(mains) != 1 or '"{}"'.format(gen_skeleton.HOLD_FLAG) not in mains[0]:
                    report.fail('peer-lost-scenario', 'the example with {} steps names '
                                'a hold the generated consumer does not make, or that '
                                'main() never switches on'.format(kind))
                    return
                continue
            if slot is None or '== ' + slot not in sheet:
                report.fail('peer-lost-scenario', 'with no steps the stop trigger '
                            'is no section of the worksheet, so it is written by '
                            'editing scenarios.json')
                return
            for body, code in (('a\nb\n', 2), ('consumer: midway\n', 0)):
                with open('bodies.txt', 'w', encoding='utf-8') as handle:
                    handle.write('== {}\n{}'.format(slot, body))
                done = subprocess.run([sys.executable,
                                       os.path.join(tools, 'fill_markers.py'),
                                       '--bodies', 'bodies.txt'],
                                      capture_output=True, text=True)
                if done.returncode != code:
                    report.fail('peer-lost-scenario', 'fill_markers.py exits {} on a '
                                'stop section of {} line(s), expected {}'
                                .format(done.returncode, body.count('\n'), code))
                    return
            with open('scenarios.json', encoding='utf-8') as handle:
                after = [s['stop']['after'] for s in json.load(handle)['scenarios']
                         if isinstance(s.get('stop'), dict)]
            if after != ['consumer: midway']:
                report.fail('peer-lost-scenario', 'the stop section did not become '
                            'the trigger: {}'.format(after))
                return
        finally:
            os.chdir(here)
            shutil.rmtree(holder, ignore_errors=True)
    report.ok('peer-lost-scenario', 'every two-process project gets a peer-lost '
              'scenario: a stepped one is stopped at the hold its consumer makes when '
              'started with --hold, one with no steps gets the trigger as one worksheet '
              'section')


def check_scaffold_routing(report):
    """Before a design exists, nothing sends a run to a page the generator makes moot.

    The ipc scaffold used to end on "docs/agent/50-running.md has its keys" for a peer
    that goes away, and runs read that page before designing, for scenarios
    build_project.py writes itself. A lookup of step syntax in schema_help.py answers
    "no such element" and costs a request to find gen_docs.py --example.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    try:
        done = subprocess.run([sys.executable, os.path.join(tools, 'setup_project.py'),
                               '--name', 'route', '--root', holder, '--mode', 'ipc',
                               '--sdk-root', ROOT], capture_output=True, text=True)
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    said = done.stdout + done.stderr
    if '50-running.md' in said or '"peer-lost"' not in said:
        report.fail('scaffold-route', 'the ipc scaffold routes to 50-running.md for a '
                    'scenario build_project.py writes, or does not say it writes it')
        return
    done = subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'schema_help.py'),
                           'Step', '--document', 'siml'], capture_output=True, text=True)
    if 'gen_docs.py --example' not in done.stdout:
        report.fail('scaffold-route', 'schema_help.py answers a design.json key such as '
                    '"Step" with "no such element" and no pointer to gen_docs.py --example')
        return
    report.ok('scaffold-route', 'the ipc scaffold says which scenarios are generated, and '
              'schema_help.py sends a design.json key to gen_docs.py --example')


def check_base_api_parity(report):
    """The worksheet's base-API block names only real calls, and says it is a floor.

    It is a second copy of what 40-base-api.md carries, and a second copy that drifts is
    worse than none. Two things are held: every name in it is a name the framework
    actually declares (members.json is generated from the headers), and every name is
    also on the page, so the two cannot disagree. The floor sentence is checked as well:
    an enumerated list of what a type carries is read as the list of what it carries,
    which once cost seven working lines rewritten against a header that contradicted it.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_skeleton
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('base-api-parity',
                    'gen_skeleton.py does not import: {}'.format(failure))
        return

    block = ' '.join(gen_skeleton.BASE_API)
    if 'floor' not in block or 'not' not in block or 'api_help.py' not in block:
        report.fail('base-api-parity',
                    'the worksheet base-API block does not say it is a floor and not a '
                    'limit, or does not route the rest to api_help.py. An enumerated '
                    'list read as exhaustive manufactures absences')
        return

    try:
        with open(os.path.join(ROOT, 'docs', 'agent', 'members.json'), encoding='utf-8') as handle:
            declared = set(json.load(handle).get('members') or [])
    except (OSError, ValueError) as failure:
        report.fail('base-api-parity', 'members.json does not read: {}'.format(failure))
        return
    try:
        with open(os.path.join(ROOT, 'docs', 'agent', '40-base-api.md'), encoding='utf-8') as handle:
            page = handle.read()
    except OSError as failure:
        report.fail('base-api-parity', '40-base-api.md does not read: {}'.format(failure))
        return

    # Every call of the block is written on its receiver, as the page spells it.
    for line in gen_skeleton.BASE_API:
        called = line.strip().split('(')[0]
        if called in gen_skeleton.BASE_API_NAMES:
            report.fail('base-api-parity',
                        'the worksheet lists "{}" with no receiver, so it reads as a free '
                        'function in areg::. Write it as "s.{}(...)", the member call it '
                        'is'.format(called, called))
            return

    for name in gen_skeleton.BASE_API_NAMES:
        if name not in declared:
            report.fail('base-api-parity',
                        'the worksheet offers "{}", which the framework does not declare '
                        '(members.json is generated from the headers). A body written '
                        'from it does not compile'.format(name))
            return
        if name not in page:
            report.fail('base-api-parity',
                        'the worksheet offers "{}" and 40-base-api.md does not carry it. '
                        'The two copies have drifted'.format(name))
            return
        if name not in block:
            report.fail('base-api-parity',
                        '"{}" is listed in BASE_API_NAMES and is not in the block the '
                        'worksheet prints'.format(name))
            return
    report.ok('base-api-parity',
              'the worksheet base-API block names {} call(s), all of them declared by the '
              'framework and all of them on 40-base-api.md, and says it is a floor'
              .format(len(gen_skeleton.BASE_API_NAMES)))


def check_build_routes_next(report):
    """After a generation that leaves markers open, the build names the worksheet.

    build_project.py used to close every generate-only call with "the same command
    with --run", whichever it was, even with every marker open and no bodies.txt, so
    a run spent a request deciding for itself that the worksheet came first. The line a tool ends on is
    what routes the next call.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import build_project
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('build-route', 'build_project.py does not import: {}'.format(failure))
        return

    holder = tempfile.mkdtemp()
    try:
        source = os.path.join(holder, 'src', 'provider')
        os.makedirs(source)
        with open(os.path.join(source, 'P.cpp'), 'w', encoding='utf-8') as handle:
            handle.write('void f()\n{\n    // TODO(you) body: write it.\n}\n')
        with open(os.path.join(holder, 'worksheet.txt'), 'w', encoding='utf-8') as handle:
            handle.write('#| one\n== body\n')
        open_said = build_project.closing_lines(holder, 'bodies.txt', ['design.json'])
        with open(os.path.join(holder, 'bodies.txt'), 'w', encoding='utf-8') as handle:
            handle.write('== other\n    int kept{ 0 };\n')
        partial_said = build_project.closing_lines(holder, 'bodies.txt', ['design.json'])
        os.remove(os.path.join(holder, 'bodies.txt'))
        os.remove(os.path.join(source, 'P.cpp'))
        filled_said = build_project.closing_lines(holder, 'bodies.txt', ['design.json'])
    finally:
        shutil.rmtree(holder, ignore_errors=True)

    whole = ' '.join(open_said)
    if 'worksheet.txt' not in whole or 'bodies.txt' not in whole:
        report.fail('build-route',
                    'a build that leaves markers open does not name the worksheet and '
                    'the bodies file as the next step: "{}"'.format(whole))
        return
    if 'marker' not in whole:
        report.fail('build-route',
                    'a build that leaves markers open does not say that it did, so '
                    '--run reads as the next call: "{}"'.format(whole))
        return
    partial = ' '.join(partial_said)
    if 'every section' in partial or 'body' not in partial or '--run' not in partial:
        report.fail('build-route',
                    'a build whose bodies.txt already carries code asks for every section '
                    'again instead of naming the open ones: "{}"'.format(partial))
        return
    if 'worksheet.txt' in ' '.join(filled_said):
        report.fail('build-route',
                    'a build with every marker filled still sends the reader to the '
                    'worksheet, which has nothing left in it')
        return
    report.ok('build-route',
              'the build names the worksheet while markers are open and --run once '
              'they are filled')


def change_note_route_failure(tools):
    """What is wrong with the closing lines after a note asking for a design change, or ''.

    The note is the one gen_docs.py prints for an action shared by two answered requests.
    """
    sys.path.insert(0, tools)
    import build_project
    sys.path.pop(0)
    if not hasattr(build_project, 'CHANGES_ASKED'):
        return 'build_project.py keeps no record of a note asking for a design change'
    example = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'), '--example'],
                             capture_output=True, text=True)
    shared = json.loads(example.stdout)
    for request in shared['interfaces'][0]['requests']:
        request.setdefault('answer', [{'name': 'done', 'type': 'bool'}])
    shared['machines'][0]['states'][0]['transitions'] += [
        {'on': 'open', 'do': ['on_open']}, {'on': 'close', 'do': ['on_open']}]
    holder = tempfile.mkdtemp()
    try:
        spec = os.path.join(holder, 'design.json')
        with open(spec, 'w', encoding='utf-8') as handle:
            json.dump(shared, handle)
        done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'), '--outdir',
                               os.path.join(holder, 'out'), '--force', '--spec', spec],
                              capture_output=True, text=True)
        lines = (done.stdout + done.stderr).splitlines()
        source = os.path.join(holder, 'src', 'provider')
        os.makedirs(source)
        with open(os.path.join(source, 'P.cpp'), 'w', encoding='utf-8') as handle:
            handle.write('void f()\n{\n    // TODO(you) body: write it.\n}\n')
        record = os.path.join(holder, 'build', 'notes')
        said = []
        for _ in range(2):
            del build_project.CHANGES_ASKED[:]
            build_project.collapse_notes(lines, record)
            said.append(' '.join(build_project.closing_lines(holder, 'bodies.txt',
                                                              ['design.json'])))
        del build_project.CHANGES_ASKED[:]
        build_project.collapse_notes([line for line in lines if 'forwards' not in line
                                      and 'runs on' not in line], os.path.join(holder, 'x'))
        said.append(' '.join(build_project.closing_lines(holder, 'bodies.txt',
                                                          ['design.json'])))
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    if 'change to design.json' not in said[0] or 'before reading worksheet.txt' not in said[0]:
        return 'a fresh shared-action note does not put the design change before the ' \
               'worksheet: "{}"'.format(said[0])
    if 'change to design.json' in said[1]:
        return 'a note already shown asks for the design change again: "{}"'.format(said[1])
    if 'change to design.json' in said[2]:
        return 'a build with no design-change note asks for one: "{}"'.format(said[2])
    return ''


def check_change_note_route(report, tools=None):
    """A note asking for a design change, printed fresh, puts the change before the
    worksheet read; a repeated note or none leaves the closing lines as they are."""
    failure = change_note_route_failure(tools or os.path.join(ROOT, 'tools', 'agent'))
    if failure:
        report.fail('change-note-route', failure)
        return
    report.ok('change-note-route', 'a fresh design-change note puts the change before the '
              'worksheet read; a repeated note or none does not')


def check_advice_flags_accepted(report):
    """Every flag a build_project.py failure advice names, and every run control the
    run pages name, is one it accepts.

    The advice is read right after a build_project.py call, so a flag it names is
    tried on build_project.py. A flag only another tool takes is refused with exit 2,
    and under a pipe the refusal prints nothing.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import build_project
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('advice-flags', 'build_project.py does not import: {}'.format(failure))
        return
    helped = subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'agent',
                                                          'build_project.py'), '--help'],
                            capture_output=True, text=True).stdout
    accepted = set(re.findall(r'--[a-z][a-z-]*', helped))
    named = set()
    for advice in build_project.ADVICE.values():
        named.update(re.findall(r'--[a-z][a-z-]*', advice))
    # The run controls the pages name are tried on build_project.py, the command that runs.
    pages = read('docs', 'agent', '01-runbook.md') + read('docs', 'agent', '51-debug.md')
    named.update(flag for flag in ('--only', '--verbose') if flag in pages)
    refused = sorted(named - accepted)
    if refused:
        report.fail('advice-flags',
                    'build_project.py advice names {} but build_project.py refuses it'
                    .format(', '.join(refused)))
        return
    report.ok('advice-flags', 'every flag the build advice and the run pages name is '
                              'accepted: {}'
              .format(', '.join(sorted(named)) or 'none'))


def check_passing_output_kept(report):
    """A passing run keeps each process's whole output where the last line names it.

    The last line of a passing build_project.py --run is the one a pipe through tail
    keeps. Output deleted on a pass leaves an agent that cut it only a second run.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import build_project
        import run_scenarios
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('output-kept', 'a tool does not import: {}'.format(failure))
        return
    if not hasattr(build_project, 'passed_lines'):
        report.fail('output-kept', 'build_project.py has no passed_lines(): the lines '
                    'a passing --run ends on name no place its output is kept')
        return
    last = build_project.passed_lines('out')[-1]
    kept_at = build_project.KEPT_OUTPUT.format(build='out')
    if kept_at not in last:
        report.fail('output-kept', 'the last line of a passing --run does not name {}: '
                    '"{}"'.format(kept_at, last))
        return
    root = tempfile.mkdtemp(prefix='output-kept-')
    try:
        names = [name + run_scenarios.SELF_TEST_SUFFIX for name in ('keptprov', 'keptcons')]
        for name, body in zip(names, run_scenarios.SELF_TEST_BODIES):
            path = os.path.join(root, name)
            with open(path, 'w', encoding='utf-8',
                      newline=run_scenarios.SELF_TEST_NEWLINE) as handle:
                handle.write(body)
            os.chmod(path, 0o755)
        scenario = {'name': 'kept', 'timeout': 15,
                    'procs': [{'binary': names[0], 'name': 'provider',
                               'expect': ['provider: serving']},
                              {'binary': names[1], 'name': 'consumer',
                               'expect': ['consumer: the provider was still there'],
                               'exit': 0}]}
        keep = os.path.join(root, 'kept-output')
        try:
            passed, _, detail = run_scenarios.run_scenario(scenario, [root], False, True,
                                                           keep=keep)
        except TypeError:
            report.fail('output-kept', 'run_scenario() takes no keep directory')
            return
        if not passed:
            report.fail('output-kept', 'the planted scenario did not pass: {}'.format(detail))
            return
        logs = []
        for folder, _, files in os.walk(keep):
            logs += [os.path.join(folder, name) for name in files]
        text = ''
        for path in logs:
            with open(path, encoding='utf-8', errors='replace') as handle:
                text += handle.read()
        if 'consumer: the provider was still there' not in text:
            report.fail('output-kept', 'a passing scenario left no process output in {}'
                        .format(keep))
            return
    finally:
        shutil.rmtree(root, ignore_errors=True)
    report.ok('output-kept', 'a passing run keeps each process log and its last line '
              'names {}'.format(kept_at))


def check_provider_timers(report):
    """A provider's own timer is declared in the design and filled in the worksheet.

    With no key for it, a run searches the framework headers for includes and edits
    generated files by hand, and a regeneration deletes the edits.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')

    def with_timer(design):
        design['interfaces'][0]['timers'] = [{'name': 'Heartbeat', 'timeout': 200,
                                              'repeat': 0, 'start': True}]

    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        failed = lay_example_app(tools, with_timer)
        if failed:
            report.fail('provider-timers', failed)
            return
        sources = ''
        for folder, _dirs, files in os.walk('src'):
            for name in files:
                if 'Provider.' in name:
                    with open(os.path.join(folder, name), encoding='utf-8') as handle:
                        sources += handle.read()
        sheet = ''
        if os.path.exists('worksheet.txt'):
            with open('worksheet.txt', encoding='utf-8') as handle:
                sheet = handle.read()
        wanted = [('areg/component/TimerConsumer.hpp', sources),
                  ('private   areg::TimerConsumer', sources),
                  ('mHeartbeat(static_cast<areg::TimerConsumer &>(self()), "Heartbeat")',
                   sources),
                  ('if (&timer == &mHeartbeat)', sources),
                  ('start_heartbeat();', sources),
                  ('mHeartbeat.stop_timer();', sources),
                  ('== timer_heartbeat', sheet),
                  ('inline void start_heartbeat()', sheet)]
        missing = [text for text, where in wanted if text not in where]
        if missing:
            report.fail('provider-timers', 'a timer the design gives the provider is not '
                        'generated whole; missing: {}'.format('; '.join(missing)))
            return
        with open('design.json', encoding='utf-8') as handle:
            design = json.load(handle)
        design['interfaces'][0]['timers'][0]['timeout'] = 0
        with open('bad.json', 'w', encoding='utf-8') as handle:
            json.dump(design, handle)
        done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                               '--outdir', 'bad', '--force', '--spec', 'bad.json'],
                              capture_output=True, text=True)
        if done.returncode == 0:
            report.fail('provider-timers', 'a provider timer with timeout 0 is accepted')
            return
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('provider-timers', 'a provider timer in design.json becomes a member, '
              'start and stop helpers, a process_timer branch and a worksheet section')


def check_shared_request_action(report):
    """An action shared by triggers that forward answered requests is named twice.

    Its body cannot tell which request it answers. Found while writing the bodies, it
    costs a redesign, a regeneration and a second worksheet read. The design note says it before generation; the worksheet section says where
    the action runs.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')
    sys.path.insert(0, tools)
    import gen_docs
    import gen_skeleton
    example = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                              '--example'], capture_output=True, text=True)
    try:
        clean = json.loads(example.stdout)
    except ValueError:
        report.fail('shared-action', 'gen_docs.py --example prints no spec')
        return
    shared = json.loads(json.dumps(clean))
    for request in shared['interfaces'][0]['requests']:
        request.setdefault('answer', [{'name': 'done', 'type': 'bool'}])
    machine = shared['machines'][0]
    machine['states'][0]['transitions'] += [{'on': 'open', 'do': ['on_open']},
                                            {'on': 'close', 'do': ['on_open']}]
    if gen_docs.shared_request_actions(clean, clean['machines'][0]):
        report.fail('shared-action', 'the example, whose actions each run on one '
                    'trigger, is reported as sharing one')
        return
    found = gen_docs.shared_request_actions(shared, machine)
    if [name for name, _ in found] != ['on_open']:
        report.fail('shared-action', 'an action run on two answered request triggers '
                    'is not named before generation: {}'.format(found))
        return
    holder = tempfile.mkdtemp()
    try:
        spec = os.path.join(holder, 'design.json')
        with open(spec, 'w', encoding='utf-8') as handle:
            json.dump(shared, handle)
        done = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                               '--outdir', holder, '--force', '--spec', spec],
                              capture_output=True, text=True)
        document = os.path.join(holder, machine['name'] + '.fsml')
        if not os.path.exists(document):
            report.fail('shared-action', 'gen_docs.py wrote no machine: {}'.format(
                (done.stderr or done.stdout).strip()[-300:]))
            return
        tail = gen_skeleton.runs_on(gen_skeleton.Interface(document), 'on_open')
        if 'open in' not in tail or 'close in' not in tail:
            report.fail('shared-action', 'the worksheet section of an action run on two '
                        'triggers does not name them: {!r}'.format(tail))
            return
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('shared-action', 'an action run on two answered request triggers is a '
              'design note, and its worksheet section names where it runs')


def check_codegen_plain_error(report):
    """A parameter name given two types across the responses and broadcasts of one
    interface is refused by codegen.jar under its own rule, and the review says so."""
    tool = os.path.join(ROOT, 'tools', 'agent', 'gen_docs.py')
    example = subprocess.run([sys.executable, tool, '--example'],
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             universal_newlines=True)
    if example.returncode != 0:
        report.fail('codegen-plain-error', 'gen_docs.py --example does not run')
        return
    design = json.loads(example.stdout)
    service = design['interfaces'][0]
    service['requests'][0].setdefault('answer', []).append(
        {'name': 'level', 'type': 'uint32'})
    service['broadcasts'][0]['params'].append({'name': 'level', 'type': 'String'})
    design['machines'] = []
    holder = tempfile.mkdtemp(prefix='areg-plain-error-')
    try:
        with open(os.path.join(holder, 'design.json'), 'w', encoding='utf-8') as handle:
            json.dump(design, handle)
        reviewed = subprocess.run([sys.executable, tool, '--spec', 'design.json',
                                   '--review'], cwd=holder, stdout=subprocess.PIPE,
                                  stderr=subprocess.STDOUT, universal_newlines=True)
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    if reviewed.returncode == 0:
        report.fail('codegen-plain-error',
                    'a design giving "level" two types in one interface reviews clean')
        return
    if 'error[60/' not in reviewed.stdout:
        report.fail('codegen-plain-error',
                    'a design giving "level" two types in one interface is refused, but not '
                    'as codegen rule 60: ' + reviewed.stdout.strip().splitlines()[-1:][0]
                    if reviewed.stdout.strip() else 'the review printed nothing')
        return
    report.ok('codegen-plain-error',
              'a parameter name of two types is refused as codegen rule 60')


def check_generated_prefix(report):
    """A design name that starts with a prefix the generator adds is refused with the name
    to write instead, before codegen.jar doubles it."""
    tool = os.path.join(ROOT, 'tools', 'agent', 'gen_docs.py')
    example = subprocess.run([sys.executable, tool, '--example'],
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             universal_newlines=True)
    if example.returncode != 0:
        report.fail('generated-prefix', 'gen_docs.py --example does not run')
        return
    design = json.loads(example.stdout.replace('"open"', '"request_open"'))
    holder = tempfile.mkdtemp(prefix='areg-generated-prefix-')
    try:
        with open(os.path.join(holder, 'design.json'), 'w', encoding='utf-8') as handle:
            json.dump(design, handle)
        reviewed = subprocess.run([sys.executable, tool, '--spec', 'design.json',
                                   '--review'], cwd=holder, stdout=subprocess.PIPE,
                                  stderr=subprocess.STDOUT, universal_newlines=True)
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    if reviewed.returncode == 0 or 'Name it "open"' not in reviewed.stdout:
        report.fail('generated-prefix',
                    'a request named "request_open" is not refused with the name to write; '
                    'codegen.jar generates it as request_request_open()')
        return
    report.ok('generated-prefix',
              'a request named "request_open" is refused with "open" as the name to write')


def check_example_machine(report):
    """--example machine is a design gen_docs.py accepts with no note, whose hosted machine
    runs from two states and keeps its own timer, within the size of one call."""
    tool = os.path.join(ROOT, 'tools', 'agent', 'gen_docs.py')
    example = subprocess.run([sys.executable, tool, '--example', 'machine'],
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             universal_newlines=True)
    if example.returncode != 0:
        report.fail('example-machine', 'gen_docs.py --example machine does not run')
        return
    if (len(example.stdout.splitlines()) > EXAMPLE_LINES
            or len(example.stdout.encode('utf-8')) > EXAMPLE_BYTES):
        report.fail('example-machine', 'gen_docs.py --example machine is over the size '
                    'a run reads in one call')
        return
    design = json.loads(example.stdout)
    machines = {spec.get('name'): spec for spec in design.get('machines') or []}
    hosts = {}
    for spec in machines.values():
        for state in spec.get('states') or []:
            if state.get('submachine'):
                hosts.setdefault(state['submachine'], []).append(state['name'])
    reused = [name for name, states in hosts.items()
              if len(states) > 1 and machines.get(name, {}).get('timers')]
    if not reused:
        report.fail('example-machine', 'gen_docs.py --example machine no longer shows a '
                    'machine with its own timer hosted from two states')
        return
    holder = tempfile.mkdtemp(prefix='areg-example-machine-')
    try:
        with open(os.path.join(holder, 'design.json'), 'w', encoding='utf-8') as handle:
            handle.write(example.stdout)
        reviewed = subprocess.run([sys.executable, tool, '--spec', 'design.json',
                                   '--review'], cwd=holder, stdout=subprocess.PIPE,
                                  stderr=subprocess.STDOUT, universal_newlines=True)
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    notes = [line.strip() for line in reviewed.stdout.splitlines()
             if line.lstrip().startswith('note ')]
    if reviewed.returncode != 0 or notes:
        report.fail('example-machine', 'gen_docs.py --example machine earns a refusal or '
                    'a design note, and a run copies both: ' +
                    (notes[0] if notes else reviewed.stdout.strip()[-300:]))
        return
    report.ok('example-machine', 'gen_docs.py --example machine reviews clean, with "{}" '
              'hosted from {}'.format(reused[0], ', '.join(hosts[reused[0]])))


def check_example_programs(report):
    """--example programs is a design gen_docs.py accepts with no note, showing each kind
    of component: one that provides, one that drives and one that watches."""
    tool = os.path.join(ROOT, 'tools', 'agent', 'gen_docs.py')
    example = subprocess.run([sys.executable, tool, '--example', 'programs'],
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             universal_newlines=True)
    if example.returncode != 0:
        report.fail('example-programs', 'gen_docs.py --example programs does not run')
        return
    design = json.loads(example.stdout)
    kinds = set(kind for program in design.get('programs') or []
                for component in program.get('components') or []
                for kind in ('provides', 'drives', 'watches') if component.get(kind))
    if kinds != {'provides', 'drives', 'watches'}:
        report.fail('example-programs', 'gen_docs.py --example programs shows no component '
                    'that {}, so a design needing one writes it with "uses" only and is '
                    'refused'.format(' or '.join(sorted({'provides', 'drives', 'watches'}
                                                        - kinds))))
        return
    holder = tempfile.mkdtemp(prefix='areg-example-programs-')
    try:
        with open(os.path.join(holder, 'design.json'), 'w', encoding='utf-8') as handle:
            handle.write(example.stdout)
        reviewed = subprocess.run([sys.executable, tool, '--spec', 'design.json',
                                   '--review'], cwd=holder, stdout=subprocess.PIPE,
                                  stderr=subprocess.STDOUT, universal_newlines=True)
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    notes = [line.strip() for line in reviewed.stdout.splitlines()
             if line.lstrip().startswith('note ')]
    if reviewed.returncode != 0 or notes:
        report.fail('example-programs', 'gen_docs.py --example programs earns a refusal or '
                    'a design note, and a run copies both: ' +
                    (notes[0] if notes else reviewed.stdout.strip()[-300:]))
        return
    report.ok('example-programs', 'gen_docs.py --example programs reviews clean and shows '
              'a component that provides, one that drives and one that watches')


def check_driver_client_early(report):
    """A driver's used client keeps an update that arrives before the steps begin and
    runs its body once they do; a client of a component with no steps does not."""
    tools = os.path.join(ROOT, 'tools', 'agent')
    example = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                              '--example', 'programs'], stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE, universal_newlines=True)
    if example.returncode != 0:
        report.fail('client-early', 'gen_docs.py --example programs does not run')
        return
    design = json.loads(example.stdout)
    for program in design['programs']:
        for component in program['components']:
            if component.get('drives'):
                component['uses'] = [{'service': 'StockService', 'role': 'stock'}]
    holder = tempfile.mkdtemp(prefix='areg-client-early-')
    try:
        if subprocess.run([sys.executable, os.path.join(tools, 'setup_project.py'),
                           '--name', 'early', '--root', holder, '--mode', 'ipc',
                           '--sdk-root', ROOT, '--quiet'],
                          capture_output=True, text=True).returncode != 0:
            report.fail('client-early', 'the scaffold no longer lays out a project')
            return
        with open(os.path.join(holder, 'design.json'), 'w', encoding='utf-8') as handle:
            json.dump(design, handle)
        for command in (['gen_docs.py', '--outdir', 'src/services', '--force', '--chained',
                         '--spec', 'design.json'],
                        ['gen_skeleton.py', '--programs', '--services', 'src/services',
                         '--force', '--spec', 'design.json']):
            done = subprocess.run([sys.executable, os.path.join(tools, command[0])] +
                                  command[1:], cwd=holder, stdout=subprocess.PIPE,
                                  stderr=subprocess.STDOUT, universal_newlines=True)
            if done.returncode != 0:
                report.fail('client-early', '{} refuses the example with a driver that uses '
                            'a service: {}'.format(command[0], done.stdout.strip()[-300:]))
                return
        sources = {}
        for path in glob.glob(os.path.join(holder, 'src', '*', '*.?pp')):
            with open(path, encoding='utf-8') as handle:
                sources[os.path.relpath(path, holder).replace(os.sep, '/')] = handle.read()
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    driving = ''.join(text for name, text in sources.items()
                      if name.startswith('src/customer/') and 'Client' in name)
    owner = ''.join(text for name, text in sources.items()
                    if name.startswith('src/customer/') and 'Client' not in name)
    other = ''.join(text for name, text in sources.items()
                    if name.startswith('src/desk/') and 'Client' in name)
    if not re.search(r'if \(mOwner\.mStep == \w+::Step::Start\)\s*\{\s*mEarly = true;'
                     r'\s*return;', driving) or 'void deliver_early()' not in \
            ''.join(text for name, text in sources.items() if name.endswith('.hpp')):
        report.fail('client-early', 'a used client of a driver runs an update that arrives '
                    'before the steps begin and keeps nothing: the first step never sees it')
        return
    if len(re.findall(r'begin\(Step::Small\);\s*mStock\.deliver_early\(\);', owner)) != 2:
        report.fail('client-early', 'the driver does not hand its used clients the updates '
                    'they kept, each time its steps begin')
        return
    if 'mEarly' in other:
        report.fail('client-early', 'a used client of a component with no steps keeps '
                    'its updates for steps that never begin')
        return
    report.ok('client-early', 'a driver\'s used client keeps an update that arrives before '
              'the steps begin and runs it as they begin')


def check_design_reviewable(report):
    """A design can be reviewed before it is built, and the review writes nothing.

    Every note gen_docs.py prints is a design finding: an attribute no rule reads, a
    state no consumer can see, which states answer each trigger. Printed only by a
    generation, each of them costs a --regenerate to act on.
    """
    tool = os.path.join(ROOT, 'tools', 'agent', 'gen_docs.py')
    example = subprocess.run([sys.executable, tool, '--example'],
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                             universal_newlines=True)
    if example.returncode != 0:
        report.fail('design-review', 'gen_docs.py --example does not run')
        return

    def noted(text):
        return [line for line in text.splitlines()
                if line.startswith('  note  ') or line.startswith('  table ')
                or line.startswith('        ')]

    holder = tempfile.mkdtemp(prefix='areg-review-')
    try:
        spec = os.path.join(holder, 'design.json')
        with open(spec, 'w', encoding='utf-8') as handle:
            handle.write(example.stdout)
        reviewed = subprocess.run([sys.executable, tool, '--spec', 'design.json',
                                   '--review'], cwd=holder, stdout=subprocess.PIPE,
                                  stderr=subprocess.STDOUT, universal_newlines=True)
        if reviewed.returncode != 0:
            report.fail('design-review',
                        'gen_docs.py --spec design.json --review exits {} on the '
                        'design --example prints:\n{}'
                        .format(reviewed.returncode, reviewed.stdout[:600]))
            return
        left = sorted(os.listdir(holder))
        if left != ['design.json']:
            report.fail('design-review',
                        'a review wrote {}. It is run on a design the agent is still '
                        'editing, so it reads and reports and touches nothing'
                        .format(', '.join(name for name in left
                                          if name != 'design.json')))
            return
        written = subprocess.run([sys.executable, tool, '--spec', 'design.json',
                                  '--outdir', os.path.join('src', 'services')],
                                 cwd=holder, stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT, universal_newlines=True)
        if written.returncode != 0:
            report.fail('design-review', 'the same design does not generate: {}'
                        .format(written.stdout[:600]))
            return
        if noted(reviewed.stdout) != noted(written.stdout):
            report.fail('design-review',
                        'a review and a generation of one design print different '
                        'notes. A finding the build keeps to itself is one the agent '
                        'meets after the documents exist, and acts on with a '
                        '--regenerate')
            return
    finally:
        shutil.rmtree(holder, ignore_errors=True)

    report.ok('design-review',
              'a review writes nothing and prints the notes the generation prints')


def check_errors_follow_output(report):
    """A tool flushes what it printed before it writes the error about it.

    stdout is block-buffered into a pipe and stderr is not, and every documented call
    is piped into head or tail. Without the flush the refusal arrives above the lines
    it names, and the agent reads the two in the wrong order.
    """
    import ast
    checked = 0
    for name in sorted(os.listdir(os.path.join(ROOT, 'tools', 'agent'))):
        if not name.endswith('.py'):
            continue
        path = os.path.join(ROOT, 'tools', 'agent', name)
        with open(path, encoding='utf-8') as handle:
            source = handle.read()
        try:
            tree = ast.parse(source)
        except SyntaxError as failure:
            report.fail('error-ordering', '{} does not parse: {}'.format(name, failure))
            return
        for node in tree.body:
            if not isinstance(node, ast.FunctionDef) or node.name != 'fail':
                continue
            checked += 1
            body = ast.dump(ast.Module(body=node.body, type_ignores=[]))
            flush = body.find('flush')
            writes = body.find("'stderr'")
            if flush < 0 or (writes >= 0 and flush > writes):
                report.fail('error-ordering',
                            '{}: fail() writes to stderr without flushing stdout '
                            'first. Piped into head, its error prints above the '
                            'output it is about'.format(name))
                return
    if not checked:
        report.fail('error-ordering', 'no fail() found in tools/agent: the rule '
                                      'checks nothing')
        return
    report.ok('error-ordering',
              '{} fail() implementation(s) flush stdout before the error, so a piped '
              'run reads in order'.format(checked))


def check_failure_names_the_error(report):
    """A failed step prints the line that names the defect, not the log tail.

    A compiler and a generator print the diagnostic in the middle and the summary of
    the tool that gave up at the end, so a tail carries no error, and a run asks a
    second command for the errors the first one already had.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import build_project
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('failure-errors',
                    'build_project.py does not import: {}'.format(failure))
        return

    # A build log of the shape the compiler writes: one diagnostic with its candidates,
    # then enough make noise to push it out of any tail.
    log = ['[ 50%] Building CXX object x.cpp.o',
           'src/consumer/C.cpp:120:9: error: no matching function for call to '
           "'C::arm_deadline()'",
           '  120 |         arm_deadline();',
           '      |         ^~~~~~~~~~~~',
           'src/consumer/C.hpp:88:10: note: candidate expects 1 argument, 0 provided']
    log += ['gmake[2]: *** [build.make:76: x.cpp.o] Error 1'] * 12

    picked = build_project.diagnostics(log, 40)
    if picked is None:
        report.fail('failure-errors',
                    'build_project.diagnostics() found no error in a log that carries '
                    'one. A failed step then prints its tail, which is the summary of '
                    'the tool that gave up, and the agent spends a request asking for '
                    'the errors again')
        return
    shown = '\n'.join(line for line in picked if line is not None)
    if 'error: no matching function' not in shown:
        report.fail('failure-errors',
                    'a failed step does not print the line naming the error: {}'
                    .format(shown[:120]))
        return
    if 'candidate expects' not in shown:
        report.fail('failure-errors',
                    'a failed step prints the error without the lines under it that '
                    'say what to write instead')
        return
    if 'Error 1' in shown:
        report.fail('failure-errors',
                    'a failed step prints the summary of the tool that gave up, which '
                    'names no defect and crowds out the ones that do')
        return
    # A log with no diagnostic at all still has to print something.
    quiet = ['configuring', 'gmake: *** [Makefile:146: all] Error 2']
    if build_project.diagnostics(quiet, 40) is not None:
        report.fail('failure-errors',
                    'a log whose only "error" lines are a tool giving up is reported '
                    'as carrying a diagnostic; the fallback to the tail never runs')
        return
    report.ok('failure-errors',
              'a failed step prints the lines naming the error and what belongs to '
              'them, not the summary of the tool that gave up')


def check_passing_step_keeps_a_warning(report):
    """A step that passed hands over the warnings it did not stop for.

    The tail of a step that passed is a line count and nothing anchors it to a
    diagnostic, so a generator warning above it arrives in part or not at all: a
    warning whose line naming its rule is cut off is rediscovered from a hung scenario,
    at the price of a build-and-run cycle.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import build_project
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('passing-warnings',
                    'build_project.py does not import: {}'.format(failure))
        return

    def printed(lines, tail):
        held = io.StringIO()
        with contextlib.redirect_stdout(held):
            build_project.show_passed(lines, tail)
        return held.getvalue()

    # One codegen warning, pushed above the tail by the lines cmake prints after it.
    warning = ['src/services/M.fsml:37:70: warning[126/RULE_UNREFERENCED]: '
               'declared but never referenced',
               '  the Action method [ on_request_taken ]']
    log = ['-- line {}'.format(i) for i in range(40)] + warning + [
        '-- Generating done (0.1s)',
        '-- Build files have been written to: /w/build',
        'src/services/M.fsml: 0 errors, 1 warning -- 8 files written']
    shown = printed(log, 3)
    if 'warning[126/RULE_UNREFERENCED]' not in shown:
        report.fail('passing-warnings',
                    'a step that passed drops the line naming the rule it warned '
                    'about, so the number explain_rule.py takes never reaches the '
                    'agent and only a verbless fragment does')
        return
    if 'the Action method' not in shown:
        report.fail('passing-warnings',
                    'a warning reaches the agent without the line under it that says '
                    'which declaration it is about')
        return

    # A compiler warning the build did not stop for, for the same reason.
    build = ['[ 50%] Building CXX object x.cpp.o',
             "/w/src/C.cpp:222:62: warning: unused parameter 'Active' [-Wunused-parameter]",
             '  222 | void C::on_update(bool Active)'] + \
            ['[100%] Built target {}'.format(i) for i in range(8)]
    if 'unused parameter' not in printed(build, 2):
        report.fail('passing-warnings',
                    'a compiler warning above the tail of a build that passed is '
                    'dropped, so code an agent writes warns only on a toolchain it '
                    'never runs')
        return

    # Nothing to say costs nothing: a clean step prints exactly what it always did.
    clean = ['-- line {}'.format(i) for i in range(30)] + \
            ['src/services/M.siml: 0 errors, 0 warnings -- 10 files written']
    held = io.StringIO()
    with contextlib.redirect_stdout(held):
        build_project.show(clean, 3)
    if held.getvalue() != printed(clean, 3):
        report.fail('passing-warnings',
                    'a step with no warning prints something other than its tail; '
                    'every run would pay for the check')
        return

    # A step shown whole hides nothing, so an application line reading "warning:" is output.
    output = ['pump_provider: ready', 'warning: tank level is low',
              '      pump_consumer step 1: refused as expected']
    held = io.StringIO()
    with contextlib.redirect_stdout(held):
        build_project.show(output, None)
    if held.getvalue() != printed(output, None):
        report.fail('passing-warnings',
                    'a step shown whole also prints the application\'s own "warning:" '
                    'lines as a warning it did not stop for, so they are printed twice')
        return
    report.ok('passing-warnings',
              'a step that passed hands over the warnings above its tail, with the '
              'rule number, and prints only the tail when there are none')


def check_names_carry_signatures(report):
    """A name the worksheet hands a body carries the parameters it takes.

    A call written from a bare name costs a build-and-fix cycle: a worksheet that
    lists "arm_deadline()" for a helper taking a uint32_t invites one. A name
    without its parameters is a guess, and a guess is a compile, a read and an edit.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_skeleton
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('name-signatures',
                    'gen_skeleton.py does not import: {}'.format(failure))
        return

    header = ('class Cons\n'
              '{\n'
              '    uint32_t mIdle{ 0 };   //!< How many times it idled.\n'
              '    //! Starts the deadline timer for this many seconds.\n'
              '    void arm_deadline(uint32_t seconds);\n'
              '    inline Cons & self();\n'
              '    void progressed();\n'
              '};\n')
    members, helpers = gen_skeleton.defined_names(header)
    # A signature with no behaviour behind it is a name, and a body that has to know
    # what it does opens the generated file for a line the header already carries.
    docs = gen_skeleton.helper_docs(header)
    said = docs.get('void arm_deadline(uint32_t seconds)')
    if not said or 'deadline timer' not in said:
        report.fail('name-signatures',
                    'the worksheet lists a helper without the line its header writes '
                    'above it, so what that helper does is learnt by opening the file')
        return
    spelt = [text for text, _ in members if 'mIdle' in text]
    if not spelt:
        report.fail('name-signatures',
                    'the worksheet no longer lists the members the skeleton declares')
        return
    # A member named without its type is read as a value, and a body that has to
    # know which one it is opens the generated header.
    if 'uint32_t' not in spelt[0]:
        report.fail('name-signatures',
                    'the worksheet names member "{}" without the type it is declared '
                    'with. A body then guesses whether it holds a value or an object '
                    'and opens the generated header to find out'.format(spelt[0]))
        return
    note = [said for text, said in members if 'mIdle' in text][0]
    if not note or 'how many times' not in note.lower():
        report.fail('name-signatures',
                    'the worksheet drops the line the header writes beside a member, '
                    'so what that member is for is learnt by opening the file')
        return
    armed = [h for h in helpers if 'arm_deadline' in h]
    if not armed:
        report.fail('name-signatures',
                    'the worksheet no longer lists the helpers the skeleton declares')
        return
    if 'uint32_t' not in armed[0]:
        report.fail('name-signatures',
                    'the worksheet names helper "{}" without the parameters it takes. '
                    'A body then calls it with the wrong ones and the run pays a '
                    'build, a read and an edit for a name it was already given'
                    .format(armed[0]))
        return
    report.ok('name-signatures',
              'the worksheet hands a body every name it may call with the parameters '
              'that name takes')


def check_worksheet_rewrite(report):
    """A body the filler has written stays addressable by the same section.

    The worksheet is the only place a body is edited after a build, so a filled body
    has to keep its name: the filler leaves an anchor pair around it, finds that pair
    again, and rewrites the body between them and nothing else. Without this every
    later fix is a hunt through a generated file.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import fill_markers
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('worksheet-rewrite',
                    'fill_markers.py does not import: {}'.format(failure))
        return

    holder = tempfile.mkdtemp()
    src = os.path.join(holder, 'src')
    os.makedirs(src)
    source = os.path.join(src, 'Cons.cpp')
    filler = os.path.join(ROOT, 'tools', 'agent', 'fill_markers.py')
    sheet = os.path.join(holder, 'bodies.txt')

    def write(first):
        with open(sheet, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write('#| guidance the worksheet keeps\n'
                         '== step_one\n' + first + '\n'
                         '== step_two\n'
                         'second();\n')

    def fill():
        return subprocess.run([sys.executable, filler, '--bodies', sheet,
                               '--src', src],
                              cwd=holder, capture_output=True, text=True)

    def text():
        with open(source, encoding='utf-8') as handle:
            return handle.read()

    try:
        with open(source, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write('void f()\n{\n'
                         '    // TODO(you) step_one: check this.\n'
                         '    // TODO(you) step_two: check that.\n'
                         '}\n')
        write('first();')
        done = fill()
        if done.returncode != 0:
            report.fail('worksheet-rewrite',
                        'the first pass refused the worksheet: {}'
                        .format((done.stderr or done.stdout).strip()[:200]))
            return
        if 'first();' not in text():
            report.fail('worksheet-rewrite', 'the first pass wrote no body')
            return

        # The worksheet is the durable copy: a section applied is still there.
        with open(sheet, encoding='utf-8') as handle:
            kept = handle.read()
        if '== step_one' not in kept or 'first();' not in kept:
            report.fail('worksheet-rewrite',
                        'the filler took an applied section out of the worksheet, so '
                        'the body it wrote can no longer be changed from it')
            return

        # The anchors that make a written body addressable again.
        if 'body(you) step_one' not in text() or 'end(you) step_one' not in text():
            report.fail('worksheet-rewrite',
                        'a filled body carries no body(you)/end(you) anchor, so the '
                        'section that wrote it cannot find it again')
            return

        # A changed section rewrites that body where it stands, and nothing else.
        write('rewritten();')
        done = fill()
        if done.returncode != 0:
            report.fail('worksheet-rewrite',
                        'the second pass refused a section whose body it had already '
                        'written: {}'.format((done.stderr or done.stdout).strip()[:200]))
            return
        after = text()
        if 'rewritten();' not in after:
            report.fail('worksheet-rewrite',
                        'a changed section did not rewrite the body it had written')
            return
        if 'first();' in after:
            report.fail('worksheet-rewrite',
                        'the rewritten body was added beside the old one rather than '
                        'replacing it')
            return
        if after.count('body(you) step_one') != 1:
            report.fail('worksheet-rewrite',
                        'a rewrite left {} anchors for one body, so the next pass has '
                        'no single place to write'
                        .format(after.count('body(you) step_one')))
            return
        # A pass names the bodies it changed, and says so when it changed none.
        if '1 changed: step_one' not in done.stdout or 'step_two' in \
                done.stdout.split('changed:')[-1]:
            report.fail('worksheet-rewrite',
                        'a pass that changed one body of two does not name it, so a '
                        'scenario failing the same way twice sends the reader into '
                        'the source to see whether the edit landed')
            return

        # Writing the same worksheet again writes the same file.
        again = fill()
        if text() != after:
            report.fail('worksheet-rewrite',
                        'filling the same worksheet twice changed the source, so the '
                        'body grows or moves on every build')
            return
        if 'none changed' not in again.stdout:
            report.fail('worksheet-rewrite',
                        'a pass that changed no body does not say so')
            return
    finally:
        shutil.rmtree(holder, ignore_errors=True)

    report.ok('worksheet-rewrite', 'the worksheet keeps every body it writes, and a '
              'changed section rewrites that body in place')


def check_section_named_again(report):
    """A section named again in the bodies file replaces the earlier one.

    A repair that touches several bodies is then one append at the end of the file,
    not one edit per section, and each of those edits re-reads the whole context.
    """
    holder = tempfile.mkdtemp()
    src = os.path.join(holder, 'src')
    os.makedirs(src)
    source = os.path.join(src, 'Cons.cpp')
    sheet = os.path.join(holder, 'bodies.txt')
    try:
        with open(source, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write('void f()\n{\n'
                         '    // TODO(you) step_one: check this.\n'
                         '    // TODO(you) step_two: check that.\n'
                         '}\n')
        with open(sheet, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write('== step_one\nfirst();\n== step_two\nsecond();\n'
                         '== step_one\nfixed();\n== step_two\n\n')
        done = subprocess.run([sys.executable,
                               os.path.join(ROOT, 'tools', 'agent', 'fill_markers.py'),
                               '--bodies', sheet, '--src', src],
                              cwd=holder, capture_output=True, text=True)
        with open(source, encoding='utf-8') as handle:
            text = handle.read()
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    if done.returncode != 0:
        report.fail('section-again', 'a section named again is refused, so a repair of '
                                     'several bodies is one edit per section: {}'
                    .format((done.stderr or done.stdout).strip()[:160]))
        return
    if 'fixed();' not in text or 'first();' in text or 'second();' not in text:
        report.fail('section-again', 'a section named again did not replace the earlier '
                                     'one, or an empty repeat erased a body')
        return
    if 'named again' not in done.stdout or 'step_two' in done.stdout.split(
            'named again')[-1]:
        report.fail('section-again', 'the filler does not name the section whose later '
                                     'copy it used')
        return
    report.ok('section-again', 'a section named again with code replaces the earlier '
                               'one, and the filler names it')


def check_fix_file(report):
    """A repair goes in fix.txt, a file that starts absent, and is folded into bodies.txt.

    A file that does not exist is written in one call; an existing one is edited one
    section per request. Every repair route names fix.txt, and a refused fix changes
    nothing.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import build_project
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('fix-file', 'build_project.py does not import: {}'.format(failure))
        return
    routes = {'scenarios advice': build_project.ADVICE.get('scenarios', ''),
              'build advice': build_project.ADVICE.get('build', '')}
    with open(os.path.join(ROOT, 'docs', 'agent', '01-runbook.md'), encoding='utf-8') as handle:
        routes['runbook'] = handle.read()
    holder = tempfile.mkdtemp()
    src = os.path.join(holder, 'src')
    os.makedirs(src)
    source = os.path.join(src, 'Cons.cpp')
    sheet = os.path.join(holder, 'bodies.txt')
    fix = os.path.join(holder, 'fix.txt')
    filler = [sys.executable, os.path.join(ROOT, 'tools', 'agent', 'fill_markers.py'),
              '--bodies', sheet, '--src', src, '--fix', fix]

    def read(path):
        if not os.path.exists(path):
            return None
        with open(path, encoding='utf-8') as handle:
            return handle.read()

    def write(path, text):
        with open(path, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write(text)
    try:
        write(source, 'void f()\n{\n'
                      '    // TODO(you) step_one: check this.\n'
                      '    // TODO(you) step_two: check that.\n'
                      '    // TODO(you) step_three: check more.\n'
                      '}\n')
        write(sheet, '== step_one\nfirst();\n== step_two\nsecond();\n')
        write(fix, '== step_one\nfixed();\n== step_three\nthird();\n== step_two\n\n')
        done = subprocess.run(filler, cwd=holder, capture_output=True, text=True)
        text, bodies, left = read(source), read(sheet), read(fix)
        write(fix, '== step_one\nagain();\n== step_nine\nnine();\n')
        refused = subprocess.run(filler, cwd=holder, capture_output=True, text=True)
        after = (read(source), read(sheet), read(fix))
        write(source, 'void g()\n{\n    // TODO(you) step_four: check the rest.\n}\n')
        routes['closing lines'] = ' '.join(build_project.closing_lines(
            holder, 'bodies.txt', ['design.json']))
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    silent = [name for name, said in routes.items() if 'fix.txt' not in said]
    if silent:
        report.fail('fix-file', 'a repair route does not name fix.txt: {}'
                    .format(', '.join(silent)))
        return
    if done.returncode != 0:
        report.fail('fix-file', 'fix.txt is not folded into bodies.txt: {}'
                    .format((done.stderr or done.stdout).strip()[:160]))
        return
    if (left is not None or not text or 'fixed();' not in text or 'first();' in text
            or 'second();' not in text or 'third();' not in text):
        report.fail('fix-file', 'fix.txt did not replace, add and keep the bodies it '
                                'should, or was left behind')
        return
    if (bodies is None or bodies.count('== step_one') != 1 or 'first();' in bodies
            or 'second();' not in bodies or 'third();' not in bodies):
        report.fail('fix-file', 'bodies.txt does not hold the folded sections once each: '
                                '{!r}'.format(bodies))
        return
    if (refused.returncode == 0 or after[0] != text or after[1] != bodies
            or after[2] is None):
        report.fail('fix-file', 'a refused fix.txt changed the sources or bodies.txt, '
                                'or was removed')
        return
    report.ok('fix-file', 'a repair written to fix.txt is folded into bodies.txt section '
                          'by section, and a refused one changes nothing')


def check_unread_attribute_note(report):
    """The unread-attribute note says what the tool checked, and no more.

    It sees the arguments a document passes, never a condition or action body, and a
    body is where a parameterless condition reads machine data.
    """
    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    try:
        spec = json.loads(subprocess.run([sys.executable,
                                          os.path.join(tools, 'gen_docs.py'), '--example'],
                                         capture_output=True, text=True).stdout)
        machine = spec['machines'][0]
        machine.setdefault('attributes', []).append(
            {'name': 'Spare', 'type': 'uint32', 'value': '0',
             'description': 'Read only by a condition body.'})
        path = os.path.join(holder, 'design.json')
        with open(path, 'w', encoding='utf-8', newline='\n') as handle:
            json.dump(spec, handle)
        said = ' '.join(subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                                        '--outdir', os.path.join(holder, 'out'), '--force',
                                        '--chained', '--spec', path],
                                       capture_output=True, text=True).stdout.split())
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    if '"Spare"' not in said:
        report.fail('unread-note', 'a machine attribute no argument reads draws no note')
        return
    if 'never read' in said or 'passed to no guard, condition or action' not in said:
        report.fail('unread-note', 'the note says a machine attribute is never read, '
                                   'although a condition or action body may read it')
        return
    report.ok('unread-note', 'the unread-attribute note says what the tool checked: the '
                             'arguments, not the bodies')


def check_marker_spelling(report):
    """Three tools carry the TODO(you) pattern, and they have to agree.

    gen_skeleton.py writes the marker, fill_markers.py fills it, check_contract.py
    refuses a project that still carries one and run_scenarios.py names it beside a
    passing suite. Nothing imports anything, so each holds its own copy: a marker
    spelt one way and looked for another is filled by nobody and reported by nobody.
    """
    wanted = {
        'gen_skeleton.py': 'MARKER',
        'fill_markers.py': 'MARKER',
        'check_contract.py': 'TODO_MARKER_RE',
        'run_scenarios.py': 'TODO_MARKER_RE',
    }
    seen = {}
    for name, const in sorted(wanted.items()):
        path = os.path.join(ROOT, 'tools', 'agent', name)
        try:
            with open(path, encoding='utf-8') as handle:
                text = handle.read()
        except OSError as failure:
            report.fail('marker-spelling', '{} cannot be read: {}'.format(name, failure))
            return
        found = re.search(re.escape(const) + r"\s*=\s*re\.compile\(r'([^']*)'\)", text)
        if found is None:
            report.fail('marker-spelling',
                        '{} declares no {}, so the marker it writes or looks for cannot '
                        'be compared with the others'.format(name, const))
            return
        seen[name] = found.group(1)

    # The tail differs on purpose: two of them capture the description and two only
    # need the slot. What must match is the part that decides whether a line is a
    # marker at all, and the slot it names.
    head = r'//\s*TODO\(you\)\s+([A-Za-z_][\w]*)\s*:'
    wrong = [name for name, pattern in seen.items() if not pattern.startswith(head)]
    if wrong:
        report.fail('marker-spelling',
                    '{} look{} for a marker spelt differently from the one '
                    'gen_skeleton.py writes: a marker no tool agrees on is filled by '
                    'nobody and reported by nobody'
                    .format(', '.join(sorted(wrong)), '' if len(wrong) > 1 else 's'))
        return

    # The rule has to be stated where an agent reads the prohibitions, not only in code.
    api = os.path.join(ROOT, 'docs', 'agent', 'api.json')
    try:
        with open(api, encoding='utf-8') as handle:
            stated = json.load(handle).get('prohibitions', [])
    except (OSError, ValueError) as failure:
        report.fail('marker-spelling', 'api.json cannot be read: {}'.format(failure))
        return
    if not any(item.get('id') == 'P-17' for item in stated):
        report.fail('marker-spelling',
                    'api.json states no P-17, so an unfilled marker is a rule the '
                    'checker enforces and no page an agent reads carries')
        return

    report.ok('marker-spelling',
              '{} tools spell the TODO(you) marker the same way, and api.json states '
              'P-17'.format(len(seen)))


def check_worksheet_contract(report):
    """The worksheet names every hole, and an unfilled section never empties one.

    gen_skeleton.py writes one file carrying a section per open marker, the names the
    generated classes already carry and the contract the bodies are written against,
    so nothing has to be recalled from a command that ran many requests earlier, nor
    asked again of a tool whose output already printed it.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_skeleton
        import fill_markers
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('worksheet', 'the marker tools do not import: {}'.format(failure))
        return

    tools = os.path.join(ROOT, 'tools', 'agent')
    holder = tempfile.mkdtemp()
    here = os.getcwd()
    try:
        os.chdir(holder)
        # A real scaffold, because the scenario file's holes are part of what is checked
        # and only setup_project.py writes the shape update_scenarios() recognises.
        laid = subprocess.run([sys.executable, os.path.join(tools, 'setup_project.py'),
                               '--name', 'sheet', '--root', '.', '--mode', 'ipc',
                               '--sdk-root', ROOT], capture_output=True, text=True)
        if laid.returncode != 0:
            report.fail('worksheet', 'the scaffold no longer lays out a project: {}'
                        .format(laid.stderr.strip()[:200]))
            return
        spec = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                               '--example'], capture_output=True, text=True)
        with open('design.json', 'w', encoding='utf-8') as handle:
            handle.write(spec.stdout)
        made = subprocess.run([sys.executable, os.path.join(tools, 'gen_docs.py'),
                               '--outdir', 'src/services', '--force', '--chained',
                               '--spec', 'design.json'], capture_output=True, text=True)
        if made.returncode != 0:
            report.fail('worksheet', 'the example spec no longer generates its '
                                     'documents: {}'.format(made.stderr.strip()[:200]))
            return
        docs = sorted(os.listdir(os.path.join('src', 'services')))
        siml = next((d for d in docs if d.endswith('.siml')), None)
        fsml = next((d for d in docs if d.endswith('.fsml')), None)
        built = subprocess.run(
            [sys.executable, os.path.join(tools, 'gen_skeleton.py'),
             '--doc', 'src/services/' + siml, '--app', '--mode', 'ipc', '--force'] +
            (['--machine', 'src/services/' + fsml] if fsml else []),
            capture_output=True, text=True)
        if built.returncode != 0:
            report.fail('worksheet', 'the example no longer generates an application: '
                                     '{}'.format(built.stderr.strip()[:200]))
            return
        sheet = gen_skeleton.WORKSHEET
        if not os.path.exists(sheet):
            report.fail('worksheet', 'gen_skeleton.py --app writes no {}, so every body '
                                     'is composed from a note printed earlier'
                        .format(sheet))
            return

        named = [line[3:].strip() for line in
                 open(sheet, encoding='utf-8').read().splitlines()
                 if line.startswith('== ')]
        open_now = sorted(list(fill_markers.markers_of('src')) +
                          list(fill_markers.expectations_of('scenarios.json')[1]))
        if sorted(named) != open_now:
            report.fail('worksheet', '{} names {} hole(s) and the project leaves {}: a '
                                     'hole with no section is one the run has to find '
                                     'for itself'
                        .format(sheet, len(named), len(open_now)))
            return
        # A machine's "call" lines sit together under the line naming their receiver,
        # never after an "override" line, whose receiver is the component itself.
        block, seen_override = [], False
        for line in open(sheet, encoding='utf-8').read().splitlines():
            if 'the machine object is mFsm' in line:
                block, seen_override = ['head'], False
            elif block and line.startswith('#|   override '):
                seen_override = True
            elif block and line.startswith('#|   call ') and seen_override:
                report.fail('worksheet', 'a machine "call" line follows an "override" '
                                         'line, away from the line naming its receiver: {}'
                            .format(line[4:].strip()))
                return
        if not block:
            report.fail('worksheet', 'the example worksheet carries no machine block')
            return
        if not fill_markers.expectations_of('scenarios.json')[1]:
            report.fail('worksheet', 'scenarios.json leaves no named expectation, so '
                                     'what a run has to print is decided in a file the '
                                     'worksheet does not reach')
            return

        code = [name for name in named if name in fill_markers.markers_of('src')]
        named = code + [name for name in named if name not in code]

        # An expectation is filled as a list of regular expressions, so no comma and
        # no quote of the scenario file's own syntax is ever the author's to write.
        expect = next(name for name in named
                      if name in fill_markers.expectations_of('scenarios.json')[1])
        with open('scenarios.json', encoding='utf-8') as handle:
            before_json = handle.read()
        with open(sheet, encoding='utf-8') as handle:
            pending = handle.read().splitlines()
        typed = []
        for line in pending:
            typed.append(line)
            if line == '== ' + expect:
                typed.append('proved "one" thing, \\d+ times')
        with open(sheet, 'w', encoding='utf-8') as handle:
            handle.write('\n'.join(typed) + '\n')
        got = subprocess.run([sys.executable, os.path.join(tools, 'fill_markers.py'),
                              '--bodies', sheet], capture_output=True, text=True)
        if got.returncode != 0:
            report.fail('worksheet', 'an expectation section is refused: {}'
                        .format(got.stderr.strip()[:200]))
            return
        try:
            written = json.load(open('scenarios.json', encoding='utf-8'))
        except ValueError as broken:
            report.fail('worksheet', 'filling an expectation left scenarios.json '
                                     'unreadable: {}'.format(broken))
            return
        every = [entry for scenario in written['scenarios']
                 for spec in scenario.get('procs', [])
                 for entry in spec.get('expect', [])]
        if 'proved "one" thing, \\d+ times' not in every:
            report.fail('worksheet', 'an expectation written as a section did not reach '
                                     'scenarios.json unchanged')
            return
        with open(sheet, 'w', encoding='utf-8') as handle:
            handle.write('\n'.join(pending) + '\n')
        with open('scenarios.json', 'w', encoding='utf-8') as handle:
            handle.write(before_json)

        # The property the whole shape rests on: a section nobody filled is not an
        # instruction to empty its marker.
        before = open('src/provider/' + os.listdir('src/provider')[0],
                      encoding='utf-8').read()
        pristine = subprocess.run([sys.executable, os.path.join(tools, 'fill_markers.py'),
                                   '--bodies', sheet], capture_output=True, text=True)
        if pristine.returncode == 0:
            report.fail('worksheet', 'the worksheet as written fills markers with '
                                     'nothing: every empty section would take its '
                                     'marker and its placeholder away')
            return
        if len(fill_markers.markers_of('src')) + \
                len(fill_markers.expectations_of('scenarios.json')[1]) != len(open_now):
            report.fail('worksheet', 'a refused fill changed the project')
            return

        # A note is this file's own guidance and never reaches a source. A
        # preprocessor directive is not a note, and travels as written.
        lines = open(sheet, encoding='utf-8').read().splitlines()
        out = []
        for line in lines:
            out.append(line)
            if line == '== ' + named[0]:
                out.append('#include <cstdio>')
                out.append('    int mark_me{ 0 };')
        with open(sheet, 'w', encoding='utf-8') as handle:
            handle.write('\n'.join(out) + '\n')
        done = subprocess.run([sys.executable, os.path.join(tools, 'fill_markers.py'),
                               '--bodies', sheet], capture_output=True, text=True)
        if done.returncode != 0:
            report.fail('worksheet', 'a filled section is refused: {}'
                        .format(done.stderr.strip()[:200]))
            return
        touched = ''.join(open(os.path.join('src', 'provider', name), encoding='utf-8')
                          .read() for name in sorted(os.listdir('src/provider')))
        if 'mark_me' not in touched or '#include <cstdio>' not in touched:
            report.fail('worksheet', 'a filled section did not reach the source, or a '
                                     'preprocessor line in it was taken for a note')
            return
        if '#|' in touched:
            report.fail('worksheet', "the worksheet's own furniture reached a source file")
            return
        # Every section says which function it sits in, and says it right, so no
        # generated file is read back to find its parameter names.
        made = open(sheet, encoding='utf-8').read().splitlines()
        for number, line in enumerate(made):
            if not line.startswith('#| in: '):
                continue
            where = line[len('#| in: '):]
            name = next((made[back][3:].strip() for back in range(number, -1, -1)
                         if made[back].startswith('== ')), '')
            owner = next((f for f in sorted(os.listdir('src/provider') +
                                            os.listdir('src/consumer'))), None)
            found = False
            for folder in ('src/provider', 'src/consumer'):
                for entry in sorted(os.listdir(folder)):
                    text = open(os.path.join(folder, entry), encoding='utf-8').read()
                    found = found or where in text.replace('[[maybe_unused]] ', '')
            if not found:
                report.fail('worksheet', 'section "{}" says it sits in "{}", which no '
                                         'generated file carries'.format(name, where))
                return
            for prefix, shape in (('request_', 'request_{}('), ('response_', 'response_{}('),
                                  ('broadcast_', 'broadcast_{}('), ('update_', 'on_{}_update('),
                                  ('action_', 'action_{}(')):
                if name.startswith(prefix):
                    if shape.format(name[len(prefix):]) not in where:
                        report.fail('worksheet', 'section "{}" says it sits in "{}", '
                                                 'which is another method'
                                    .format(name, where))
                        return
                    break

        # A hash line in a body was meant as a comment, and C++ has none. Writing it
        # silently as nothing leaves the marker open.
        kept = open(sheet, encoding='utf-8').read().splitlines()
        marked = []
        for line in kept:
            marked.append(line)
            if line == '== ' + named[1]:
                marked.append('# nothing to do here')
        with open(sheet, 'w', encoding='utf-8') as handle:
            handle.write('\n'.join(marked) + '\n')
        hashed = subprocess.run([sys.executable, os.path.join(tools, 'fill_markers.py'),
                                 '--bodies', sheet], capture_output=True, text=True)
        if hashed.returncode == 0 or 'nothing to do here' not in hashed.stderr:
            report.fail('worksheet', 'a "#" line in a body is taken for a note and '
                                     'dropped, so a section written that way fills '
                                     'nothing and says nothing')
            return
        with open(sheet, 'w', encoding='utf-8') as handle:
            handle.write('\n'.join(kept) + '\n')

        stays = [line[3:].strip() for line in
                 open(sheet, encoding='utf-8').read().splitlines()
                 if line.startswith('== ')]
        # The worksheet is the durable copy of every body, so an applied section
        # stays: it is where that body is changed after a build or a scenario run.
        if named[0] not in stays or len(stays) != len(named):
            report.fail('worksheet', 'the filler took the section it applied out of {}, '
                                     'so the body it wrote can only be changed by '
                                     'hunting for it in a generated file'.format(sheet))
            return
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('worksheet', 'the worksheet names every hole -- code and scenario '
                           'expectation -- and the function each body sits in, an '
                           'empty section leaves its hole alone, a "#" body is '
                           'refused, and an applied section stays so its body can be '
                           'rewritten from the same place')


def check_placeholder_contract(report):
    """A marker and the placeholder under it go together, and both tools agree how.

    gen_skeleton.py writes lines that exist only so the skeleton runs before a rule is
    filled in. They belong to the marker above them. When fill_markers.py leaves one
    behind, the body runs and then the placeholder runs after it: a request answered
    twice, a guard that returns the answer it was given.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_skeleton
        import fill_markers
    except Exception as failure:                    # noqa: BLE001 - reported, not raised
        report.fail('placeholder', 'the marker tools do not import: {}'.format(failure))
        return

    tag = getattr(gen_skeleton, 'PLACEHOLDER_TAG', None)
    if not tag:
        report.fail('placeholder', 'gen_skeleton.py declares no PLACEHOLDER_TAG, so a '
                                   'line that stands only until its marker is filled '
                                   'is no longer marked as one')
        return
    if not fill_markers.PLACEHOLDER.search(tag):
        report.fail('placeholder', 'fill_markers.py does not recognise the tag '
                                   'gen_skeleton.py writes ({!r}): every placeholder '
                                   'would be left behind'.format(tag))
        return
    if gen_skeleton.MARKER.search(tag):
        report.fail('placeholder', 'the placeholder tag reads as a TODO(you) marker, so '
                                   'it would be reported as a hole to fill')
        return

    holder = tempfile.mkdtemp()
    try:
        source = os.path.join(holder, 'Sample.cpp')
        with open(source, 'w', encoding='utf-8') as handle:
            handle.write('void f()\n{\n')
            handle.write(gen_skeleton.marker('request_f', 'the rule', 4) + '\n')
            handle.write(gen_skeleton.placeholder('    response_f(false);') + '\n')
            handle.write(gen_skeleton.placeholder('    return;') + '\n')
            handle.write('    keep_me();\n}\n')
        found = fill_markers.markers_of(holder)
        places = found.get('request_f') or []
        if len(places) != 1:
            report.fail('placeholder', 'fill_markers.py finds {} marker(s) where the '
                                       'skeleton wrote one'.format(len(places)))
            return
        reach = places[0][3]
        if reach != 2:
            report.fail('placeholder', 'fill_markers.py reaches {} placeholder line(s) '
                                       'under a marker, not the 2 written: a body would '
                                       'run and the placeholder would run after it'
                        .format(reach))
            return
    finally:
        shutil.rmtree(holder, ignore_errors=True)
    report.ok('placeholder', 'a marker takes its placeholder lines with it, and an '
                             'untagged line under a marker is left alone')


def check_example_type_placement(report):
    """The example gen_docs.py prints obeys the rule 21-data-types.md states.

    An agent copies that example and follows its shape, not the page's table. When
    the example declares a shared type document that only one document spells, every
    project built from it carries a .dtml with one consumer: the types are then named
    by signatures that no single --contract call can explain, and the run pays to
    find out what they are.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import gen_docs
        example = gen_docs.EXAMPLE
    except Exception as failure:
        report.fail('example-types', 'gen_docs.py has no example to check: {}'
                    .format(failure))
        return

    documents = [('interface', e) for e in example.get('interfaces') or []]
    documents += [('machine', e) for e in example.get('machines') or []]

    def spells(blob, name):
        return name in json.dumps(blob)

    wrong = 0
    shared = example.get('datatypes') or {}
    space = shared.get('name') or ''
    declared = shared.get('declare') or []

    def reached_by(entry):
        """Every shared type this document names, directly or through another.

        A document names a shared type by its qualified spelling. A declaration
        names a sibling by either spelling, so a document that holds a struct holds
        the types of its fields without ever spelling them itself.
        """
        found = set()
        pending = True
        while pending:
            pending = False
            for shape in declared:
                if shape['name'] in found:
                    continue
                qualified = '{}::{}'.format(space, shape['name'])
                carried = spells(entry, qualified)
                for sibling in declared:
                    if sibling is shape or sibling['name'] not in found:
                        continue
                    carried = carried or spells(sibling, qualified) \
                        or spells(sibling, shape['name'])
                if carried:
                    found.add(shape['name'])
                    pending = True
        return found

    reach = [(entry.get('name'), reached_by(entry)) for _kind, entry in documents]
    for shape in declared:
        users = [name for name, found in reach if shape['name'] in found]
        if len(users) < 2:
            wrong += 1
            report.fail('example-types',
                        '"{}::{}" is declared in the example\'s "datatypes", and {} '
                        'reaches it. A type fewer than two documents need belongs in '
                        'that document\'s own "types", which is what '
                        '21-data-types.md tells the agent to do'
                        .format(space, shape['name'],
                                'only ' + users[0] if users else 'no document'))

    for kind, entry in documents:
        for local in entry.get('types') or []:
            others = [e.get('name') for _k, e in documents
                      if e is not entry and spells(e, local['name'])]
            if others:
                wrong += 1
                report.fail('example-types',
                            '"{}" is declared inside the example\'s {} "{}", and {} '
                            'also spells it. A type more than one document needs '
                            'belongs in "datatypes"'
                            .format(local['name'], kind, entry.get('name'),
                                    ', '.join(others)))

    if not wrong:
        report.ok('example-types',
                  '{} shared and {} local type(s) in the example: each is declared '
                  'where 21-data-types.md says it belongs'
                  .format(len(shared.get('declare') or []),
                          sum(len(e.get('types') or []) for _k, e in documents)))


def check_project_routing(report):
    """Every page AGENTS.md routes is reachable from a generated project."""
    entry = read('AGENTS.md')
    start = entry.find('## 2. Task routing')
    end = entry.find('## 3.', start + 1)
    if start < 0 or end < 0:
        report.fail('routing', 'AGENTS.md has no section 2 to compare against')
        return
    routed = set(re.findall(r'docs/agent/[0-9A-Za-z-]+\.md', entry[start:end]))
    template = read('tools', 'agent', 'setup_project.py')
    if not template:
        report.fail('routing', 'tools/agent/setup_project.py is missing')
        return
    offered = set(re.findall(r'docs/agent/[0-9A-Za-z-]+\.md', template))
    missing = sorted(routed - offered - PROJECT_ROUTING_EXEMPT)
    for page in missing:
        report.fail('routing', '{} is routed by AGENTS.md but a project made by '
                    'setup_project.py cannot reach it, so its agent must guess or '
                    'search the SDK by hand'.format(page))
    stale = sorted(p for p in PROJECT_ROUTING_EXEMPT if p not in routed)
    for page in stale:
        report.note('routing', '{} is exempted from the project routing table but '
                    'AGENTS.md no longer routes it'.format(page))
    if not missing:
        report.ok('routing', '{} of {} routed pages reachable from a generated '
                  'project, {} exempt'
                  .format(len(routed & offered), len(routed),
                          len(PROJECT_ROUTING_EXEMPT)))


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
