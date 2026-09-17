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
     ('tools/agent/evals',)),
    ('examples',      'finding the example that shows a facility',
     '41-examples.md',          'examples/',           None,
     ()),
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
    ('history pseudo-state', 'HistoryDepth="',       '22-state-machine.md', 'HistoryDepth'),
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

    for name in sorted(jar_data_members()):
        beside = read_bytes('tools', 'schema', name)
        if beside is None:
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
         'check_agent_docs.py', 'check_contract.py', 'explain_rule.py', 'run_evals.py',
         'schema_help.py',
         'check_recipes.py', 'check-env.sh', 'check-env.bat', 'codegenerate.sh',
         'codegenerate.bat', 'setup-project.sh', 'setup-project.bat', 'setup-project.ps1']

# Every byte of the reading corpus is paid by the agent that opens it, and an addition
# is only ever local while a run pays for the whole set. The ceiling is what stops the
# set growing one locally-justified paragraph at a time: raising this number is a
# deliberate edit in a reviewed file, and the commit that raises it says what it bought.
# Measured 2026-09-09 at 183,365 bytes, down from 191,363 the day before. The headroom
# is deliberately small: a ceiling with room in it ratchets nothing. Raised twice since,
# both paid for: the Windows form of starting mtrouter and logcollector, which the pages
# had wrong and which cost another agent two failed starts; and the exit-code block in
# 32-model.md section 6, because two benchmark runs each invented a global to carry a
# result from a component into main() and no page said that Application::store_element
# outlives unload_model and release(). A third raise, for the subagent hand-off in
# runbook section 4, was reclaimed: two runs carried the instruction and made zero
# subagent calls, the second with the tool explicitly allowed. Raised again for the
# String search entry in 40-base-api.md: a failed find_first returns areg::END_POS while
# the header's own Doxygen promises areg::INVALID_POS, so the obvious test passes on a
# search that found nothing. That one is silent; the run that found it lost a build cycle
# and four framework greps to the visible half of the same gap. Raised a last time, by
# 206 bytes, to route the agent at "gen_skeleton.py --app": the tool now writes the whole
# application -- components, model and main() -- compiling and running as generated, so
# the build phase stops reading 30-provider, 31-consumer, 32-model, 40-base-api,
# 42-runtime-api, 33-timers and 50-running. In the run that paid for this those seven
# pages entered context at request 21 of 55 and carried 27,309 reasoning tokens with them.
# Raised by 391 bytes to route every document at tools/agent/gen_docs.py, which writes
# the .dtml, the .siml and the .fsml of a project from one JSON description. Hand-
# authoring the XML was the largest single reasoning sink measured: 20,700 of one run's
# 63,000 reasoning tokens went into the grammar, the unique IDs and the numeric
# Transition/@To targets, none of which is about the machine. A 143-line spec reproduces
# the benchmark's own 464 lines of .siml and .fsml, generating byte-identical code.
# The pages that taught the XML now teach the spec instead, which is why the number did
# not have to rise again: the routing is substitutive, not additive.
# Raised by 1,300 bytes for three tools that each remove a request rather than a page.
# build_project.py runs the five mechanical steps -- documents, application, contract,
# configure, build -- as one command and names the step that failed; the run that paid
# for this spent six requests and about $0.15 of residency on that chain, with no
# decision in any of them. api_help.py answers one framework name with its declarations
# and the header carrying them: the run that paid for it spent 14,199 output tokens
# around a grep for start_timer, the second largest reasoning moment measured. The third
# is gen_skeleton.py naming every TODO(you) marker, which is what lets a hole be filled
# by one Edit of one unique line; the run before it rewrote two generated files whole
# for 40,018 output tokens, about $0.40, the largest single removable block left. 1,400
# bytes of the addition were paid back inside docs/agent/01-runbook.md, whose
# build-log and raw-output blocks the new command makes redundant.
# Raised again, by 1,100 bytes, for the rule in 05-design.md section 2 that decides
# between an attribute and a broadcast. The page had two lines of it and they were not
# enough: two of the thirteen recipes shipped a one-shot broadcast carrying state a
# consumer could not miss, and both hung about one run in eight because the broadcast
# overtook the subscriber's registration. The page's own worked example named the
# design its rule forbids. What the bytes buy is the whole of the distinction rather
# than half: that subscribing and unsubscribing are the same for both and decide
# nothing; that an attribute outlives the moment it was sent and is delivered to a
# consumer that subscribes later, carrying a DataState, while a broadcast is gone; that
# a broadcast carries as many parameters as the event needs where an attribute is one
# value, so several values reported together are a broadcast with the one a late
# subscriber needs published as an attribute beside it; and that only an attribute has
# a validity state. 99 bytes were reclaimed inside the page and 83 spent on the reverse
# smell, an attribute holding values that only mean something together. The last of
# the raise is one row in 51-debug.md, under the symptom this cost a session to find:
# a broadcast that fired once before the subscription request reached the provider.
# The page listed every other cause of "an attribute or broadcast never arrives" and
# not that one, and it is the one that leaves an application hanging with subscription
# code that reads as correct.
# Lowered 188800 -> 188400 when 20-service-interface.md gave back the ID counter and
# the <Value>-child spelling: gen_docs.py assigns every ID and decides where a default
# is written, and the page's own opening already says the shape of the XML is the
# tool's business. The order-of-arrival and OnChange-waits-for-ever facts moved onto
# that page from 31-consumer.md, which routing had taken off the build path -- a run
# guessed the first of them and re-invented the exit-code global for want of the
# second half of the same routing decision.
# Raised from 188400 by 624 bytes on 2026-09-14: the two connection deadlines the
# consumer scaffold now writes, documented in 31-consumer.md with the state table
# they belong to, and P-19 in AGENTS.md section 6. Both are rules an agent cannot
# follow from the code alone.
# Raised from 189024 by 416 bytes on 2026-09-14: 00-cheatsheet.md names the "steps"
# a sequencing consumer declares, on the page a run searched for them and found
# nothing, and the naming row now says a method name is kept as written after its
# prefix, which codegen.jar does and three pages denied.
# Raised from 189440 by 1024 bytes on 2026-09-14, after run 20260914b: the two pages
# a run reads before it designs described "steps" as a fixed sequence and named
# neither stay() nor go_to(), so a run whose scenario branched ruled the feature out
# and hand-wrote the generated step machine -- 422 lines, and the largest single
# request of that run. 01-runbook.md now says branching is included and names both,
# and gives --example its length, because the same run capped it at head -300 of 370
# and paid a request for the rest. 30-provider.md's unblock rule said skipping the
# call is always a defect; it is not, and the run spent thought deciding that a
# synchronous state machine trigger may answer directly.
# Raised 190464 -> 194560 on 2026-09-14 for what the independent audit of that date
# found missing or wrong. What the 4 KB bought: the corpus's only functional coverage
# hole, synchronization, which no page named at all (42-runtime-api.md section 8, eight
# verified citations); the condition on every page that prescribed signal_quit(), which
# check_contract.py reports as P-18 in the only project shape the runbook produces; the
# first line of 30/31/32 saying what those 21 KB are for, which nothing stated; and the
# narrowing of the "do not open them" rule that four pointers in 51-debug.md and
# 05-design.md contradicted. check_corpus.py's own prescribed-call rule now holds the
# first of those, so the bytes cannot silently rot back.
# Raised 194560 -> 194816 on 2026-09-17 for the worksheet becoming the one place a
# body is edited. 01-runbook.md section 6 now says that a body changed after a build
# or a scenario run is changed in bodies.txt, that a failing check names the section
# it is in, and that --regenerate restores every body; the paragraph sending the
# reader to hand-edit a generated file came out, so the section grew 246 bytes net.
# What the bytes replace: hunting a body in a generated file was 15 requests and 18%
# of run 20260917c, against 246 bytes of residency worth about $0.002 a run.
CORPUS_CEILING = 194816

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
    if '! -name run_scenarios.py' not in body:
        report.fail('grpc-arm', 'the gRPC arm does not verify that it staged nothing '
                                'but the scenario runner')
    else:
        report.ok('grpc-arm', 'the gRPC arm stages only run_scenarios.py, and refuses '
                              'to run if anything else is beside it')

    if 'grep -qi areg' not in body:
        report.fail('grpc-arm', 'the gRPC arm does not refuse a prompt or a task that '
                                'names areg')
    helped = subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'agent',
                                                          'run_scenarios.py'), '--help'],
                            cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            stdin=subprocess.DEVNULL)
    if re.search(r'areg', helped.stdout.decode('utf-8', 'replace'), re.I):
        report.fail('grpc-arm', 'run_scenarios.py --help names areg, and the gRPC arm '
                                'reads it')
    wrapper = read('examples', 'ai-benchmark', 'grpc-coffeemachine-prompt.txt')
    if not wrapper:
        report.fail('grpc-arm', 'examples/ai-benchmark/grpc-coffeemachine-prompt.txt '
                                'is missing')
    elif re.search(r'areg|another framework|other arm', wrapper, re.I):
        report.fail('grpc-arm', 'the gRPC wrapper names areg or another arm')
    else:
        report.ok('grpc-arm', 'the gRPC prompt and the runner help name no other '
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
                 'names areg; the gRPC arm must not be told of it',
                 'the gRPC prompt names areg',
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
# caps each page on its own and nothing caps the sum, which is what a run actually pays:
# 36 059 B of documentation in run 20260915b, on top of the prompt.
ENTRY_PAGES = ('AGENTS.md', 'docs/agent/01-runbook.md')
# The generated file interpolates the SDK path, so its size moves with the checkout.
# A fixed stand-in makes the measurement the same on every machine.
ENTRY_SDK_ROOT = '/opt/areg-sdk'
ENTRY_KEY = 'entry-path'


def entry_path_bytes():
    """The entry path in bytes, and what each part of it costs."""
    import tempfile, shutil
    parts = [(page, size(*page.split('/'))) for page in ENTRY_PAGES]
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
# what they do, and each one works when it is absent.
# ---------------------------------------------------------------------------
LOCAL_TREE_RE = re.compile(r'(?<![\w.-])\.claude/')


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
             if name.endswith('.md') or name.endswith('.txt')]
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
    check_method_names(report)
    check_accessor_collision(report)
    check_spec_semantics(report)
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
    check_failure_names_the_error(report)
    check_names_carry_signatures(report)
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
    bank = os.path.join(ROOT, 'tools', 'agent', 'check_mutations.py')
    if not os.path.isfile(bank):
        report.fail('verdicts', 'tools/agent/check_mutations.py is missing')
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
    generator = os.path.join(ROOT, 'tools', 'agent', 'build_members.py')
    if not os.path.isfile(generator):
        report.fail('inventory', 'tools/agent/build_members.py is missing, so nothing '
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

    "lit:" is the empty value. "param:", "attr:", "const:" and "expr:" name
    something, so an empty one is refused. Before this was checked, they all fell
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
        for prefix in ('param', 'attr', 'const', 'expr'):
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
    report.ok('spec-prefixes',
              '"lit:" is the empty value; an empty param/attr/const/expr is refused')


# The four task prompts are the comparison itself: the same requirements scored
# against gRPC, ZeroMQ, DDS or areg. A framework name, a tool name or a build command
# in one of them makes the comparison meaningless, and it has cost measured money --
# a superseded three-command verify chain in two of these files was obeyed by every
# run, over the runbook that supersedes it, because the task file is read later and
# is therefore nearer in context.
TASK_PROMPTS = ('prompt-tempalarm.md', 'prompt-coffeemachine.md', 'prompt-atm.md',
                'prompt-printscan.md')

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
            named = 'writes one application' in said
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
        if 'writes one application' not in done.stdout + done.stderr:
            report.fail('app-shape',
                        'gen_docs.py --chained does not name the one-service ceiling, '
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


NAMING_SIML = """<?xml version="1.0" encoding="utf-8"?>
<ServiceInterface FormatVersion="1.1.0">
    <Overview ID="1" Name="Naming" Version="1.0.0" Category="Public"/>
    <AttributeList>
        <Attribute ID="2" Name="WaterLevel" DataType="uint32" Notify="OnChange"/>
    </AttributeList>
    <MethodList>
        <Method ID="3" Name="InsertCoin" MethodType="Request" Response="InsertCoin"/>
        <Method ID="4" Name="InsertCoin" MethodType="Response"/>
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
        wanted = ('request_InsertCoin(', 'response_InsertCoin(', 'broadcast_LowWarning(',
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
                                      'request_insert_coin();', 1))
        if 'P-02' not in contract():
            report.fail('method-names', 'check_contract.py accepts request_insert_coin '
                                        'for a document request InsertCoin, which the base '
                                        'does not declare')
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
        <Method ID="6" Name="insert_coin" MethodType="Request" Response="insert_coin"/>
        <Method ID="7" Name="insert_coin" MethodType="Response"/>
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
                                      'set_level(0);\n    insert_coin();', 1))
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


STEP_SAMPLE = [{'name': 'open_gate', 'send': 'open', 'args': {'width': 3}},
               {'name': 'settle', 'wait': 100},
               {'name': 'close_gate', 'send': 'close'},
               {'name': 'watch_width', 'await': 'Width'}]

STEP_REFUSALS = [({'name': 'fly', 'send': 'fly'}, 'is not a request'),
                 ({'name': 'open_gate', 'send': 'open'}, 'gives no value'),
                 ({'name': 'both', 'await': 'Width', 'wait': 10}, 'does one of the two'),
                 ({'name': 'done', 'wait': 10}, 'the driver declares itself (Start, Done'),
                 ({'name': 'nothing', 'await': 'Nobody'}, 'no response, broadcast')]


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
        for wanted, what in (('request_open(3);', 'the request with its argument'),
                             ('request_close();', 'a request with no answer'),
                             ('mHold.start_timer(100,', 'a timed wait'),
                             ('quit_with(0);', 'the exit after the last step'),
                             ('begin(Step::OpenGate);', 'the first step')):
            if wanted not in source:
                report.fail('step-driver', 'the driver does not write {}: "{}" is missing'
                            .format(what, wanted))
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
        # The check stands alone inside a braced region. Unbraced, a check that
        # declares a local makes the compiler refuse every case label after it, and
        # the error names those labels rather than the declaration.
        lines = source.splitlines()
        for index, line in enumerate(lines):
            if 'TODO(you) step_' not in line:
                continue
            around = [lines[index - 1].strip(), lines[index + 1].strip(),
                      lines[index + 2].strip()]
            if around != ['{', '}', 'break;']:
                report.fail('step-driver',
                            'the check of a step is not a braced region carrying the '
                            'marker alone: the generator wrote {} around it'
                            .format(' / '.join('"{}"'.format(text) for text in around)))
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
    check_contract.py, which runs after two generators have. Run 20260912f wrote the
    documents, was refused, edited the spec twice and regenerated. The rule is the
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

    Run 20260913a read it as "head -260" and then "sed -n 260,420p": two requests
    for one answer, because nothing said how long it is. The page now says, and the
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
                    'is. Run 20260914b capped it at head -300 of 370 and paid a second '
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
    run filling a marker not to open either. Run 20260913a opened neither, guessed an
    order, stalled for the whole watchdog and paid a run-and-fix cycle to find the
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
        made = [sys.executable, os.path.join(tools, 'gen_skeleton.py'),
                '--doc', docs[0], '--app', '--mode', 'ipc', '--force']
        if machines:
            made += ['--machine', machines[0]]
        if subprocess.run(made, capture_output=True, text=True).returncode != 0:
            report.fail('order-note', 'gen_skeleton.py --app no longer writes a worksheet')
            return
        if not os.path.exists('bodies.txt'):
            report.fail('order-note', 'no worksheet was written to carry the note')
            return
        with open('bodies.txt', encoding='utf-8') as handle:
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
        # The worksheet names mPace, mDeadline and cStallTicks as taken and says
        # nothing about when they fire. One measured run opened four generated files
        # for that, at 10,141 tokens; the generated function that decides it is a
        # dozen lines.
        head = gen_skeleton.DRIVEN_HEAD.splitlines()[0]
        quoted = sheet.split(head)[-1] if head in sheet else ''
        body = [line for line in quoted.splitlines()[:gen_skeleton.DRIVEN_LIMIT + 2]
                if line.startswith('#|   ')]
        if head not in sheet or len(body) < 5:
            report.fail('order-note',
                        'the worksheet lists the timers a generated consumer owns and '
                        'does not quote the {}() that fires them, so a body that has '
                        'to know when they tick has to open a generated file'
                        .format(gen_skeleton.DRIVEN_BY))
            return
    finally:
        os.chdir(here)
        shutil.rmtree(holder, ignore_errors=True)

    report.ok('order-note', 'the worksheet says a response and an update are two '
                            'deliveries, once, in its header and before any section, '
                            'and quotes the {}() that fires the timers it lists'
              .format(gen_skeleton.DRIVEN_BY))


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
        with open('bodies.txt', encoding='utf-8') as handle:
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


def check_failure_names_the_error(report):
    """A failed step prints the line that names the defect, not the log tail.

    A compiler and a generator print the diagnostic in the middle and the summary of
    the tool that gave up at the end, so a tail carries no error. Run 20260917d spent
    three requests asking a second command for the errors the first one already had.
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


def check_names_carry_signatures(report):
    """A name the worksheet hands a body carries the parameters it takes.

    Both build-and-fix cycles of run 20260917d were a call written from a bare name:
    the worksheet listed "arm_deadline()" for a helper that takes a uint32_t. A name
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
              '    uint32_t mIdle{ 0 };\n'
              '    void arm_deadline(uint32_t seconds);\n'
              '    inline Cons & self();\n'
              '    void progressed();\n'
              '};\n')
    members, helpers = gen_skeleton.defined_names(header)
    if 'mIdle' not in members:
        report.fail('name-signatures',
                    'the worksheet no longer lists the members the skeleton declares')
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
    later fix is a hunt through a generated file, which cost 15 requests and 18% of
    run 20260917c.
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

        # Writing the same worksheet again writes the same file.
        fill()
        if text() != after:
            report.fail('worksheet-rewrite',
                        'filling the same worksheet twice changed the source, so the '
                        'body grows or moves on every build')
            return
    finally:
        shutil.rmtree(holder, ignore_errors=True)

    report.ok('worksheet-rewrite', 'the worksheet keeps every body it writes, and a '
              'changed section rewrites that body in place')


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
    so nothing has to be recalled from a command that ran fifteen requests earlier.
    Run 20260912d spent 22,538 reasoning tokens on a plan it then dropped to ask one
    tool for its own usage, which its build output had already printed.
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
        # Every section says which function it sits in, and says it right: run
        # 20260912e read four generated files back to find its parameter names.
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
                    found = found or where in text
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
        # silently as nothing left two markers open in run 20260912e.
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
    twice, a guard that returns the answer it was given. Run 20260912c paid 10
    requests to find and strip them by hand.
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
