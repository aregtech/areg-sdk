#!/usr/bin/env python3
# ===========================================================================
# Verifies that every repository path named in the agent documentation exists.
# A path that does not resolve makes an agent fail its first tool call, so a
# broken reference is treated as a build error.
#
# Two kinds of reference are checked:
#   repository paths  - start with a top level entry, resolved from the root
#   relative paths    - resolved from the directory of the document naming them
#
# Absolute paths are rejected: the reader's directory layout is unknown, and
# only the layout inside the SDK is fixed.
#
#   python3 tools/agent/check_agent_docs.py
#
# Exit code 0 when every reference resolves, 1 otherwise.
# ===========================================================================
import os
import re
import shutil
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DOCS = ['AGENTS.md', 'CODEBASE.md']
DOCS += [os.path.join('docs/agent', f)
         for f in sorted(os.listdir(os.path.join(ROOT, 'docs/agent')))
         if f.endswith('.md')]
DOCS += [os.path.join('docs/agent/recipes', f)
         for f in sorted(os.listdir(os.path.join(ROOT, 'docs/agent/recipes')))
         if f.endswith('.md')]

EXTENSIONS = 'md|hpp|cpp|siml|dtml|fsml|xsd|xml|jar|sh|bat|py|cmake|txt|init|json'

# A path inside backticks: has a known extension or ends with a slash, no spaces.
PATTERN = re.compile(r'`([A-Za-z0-9_./-]+\.(?:' + EXTENSIONS + r')|[A-Za-z0-9_./-]+/)`')

# Top level entries of this repository. A reference starting with one of them is
# a path into the SDK and is resolved from the repository root.
TOP_LEVEL = {'framework', 'examples', 'docs', 'tools', 'conf', 'tests', 'thirdparty',
             'AGENTS.md', 'CODEBASE.md', 'README.md', 'CMakeLists.txt', 'areg.cmake'}


def is_ours(ref, here):
    """True when a relative reference names something this repository owns.

    A sibling document, or a path whose first segment exists next to the document
    naming it. Anything else names a file of the reader's own project, which can
    never be resolved from here.
    """
    if '/' not in ref:
        return ref.endswith('.md')
    head = ref.split('/')[0]
    return head == '..' or head == '.' or os.path.exists(os.path.join(here, head))


# Facts that are deliberately repeated so a page can end a task on its own. The
# duplication is the point; drifting copies are not. Each entry is a substring that
# must appear in every file listed, byte for byte.
CONSISTENT = [
    ('transient connection states name Failed',
     ['ConnectionLost', 'Failed'],
     ['AGENTS.md', 'docs/agent/00-cheatsheet.md', 'docs/agent/31-consumer.md',
      'docs/agent/51-debug.md']),
    ('terminal connection states name both',
     ['Rejected', 'Shutdown'],
     ['AGENTS.md', 'docs/agent/00-cheatsheet.md', 'docs/agent/31-consumer.md']),
    ('the role name rule is stated exactly',
     ['character for character'],
     ['AGENTS.md', 'docs/agent/00-cheatsheet.md']),
]


def check_planted_template():
    """The AGENTS.md that setup_project.py writes into every created project.

    It carries its own path table and is the document with the widest reach, yet it
    lives in a Python string and no other check reads it.
    """
    sys.path.insert(0, os.path.join(ROOT, 'tools', 'agent'))
    try:
        import importlib.util
        spec = importlib.util.spec_from_file_location(
            'setup_project', os.path.join(ROOT, 'tools', 'agent', 'setup_project.py'))
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
    except Exception as error:                      # noqa: BLE001 - reported, not raised
        return ['tools/agent/setup_project.py: cannot be imported: {}'.format(error)]

    work = tempfile.mkdtemp(prefix='agent-docs-')
    try:
        module.write_agents(work, 'sample', 'local', ROOT, ['sample'])
        with open(os.path.join(work, 'AGENTS.md'), encoding='ascii') as handle:
            text = handle.read()
    except Exception as error:                      # noqa: BLE001
        return ['tools/agent/setup_project.py: write_agents failed: {}'.format(error)]
    finally:
        shutil.rmtree(work, ignore_errors=True)

    problems = []
    for match in PATTERN.finditer(text):
        ref = match.group(1)
        if ref.startswith('docs/agent/') and not os.path.exists(os.path.join(ROOT, ref)):
            problems.append('planted AGENTS.md: {}'.format(ref))
    if '<areg-sdk>' in text and ROOT in text:
        problems.append('planted AGENTS.md: mixes a resolved SDK path with the '
                        'unsubstituted placeholder <areg-sdk>')
    return problems


failures = []
checked = 0

for doc in DOCS:
    path = os.path.join(ROOT, doc)
    with open(path, encoding='ascii') as handle:
        text = handle.read()
    here = os.path.dirname(path)

    for match in PATTERN.finditer(text):
        ref = match.group(1)
        line = text[:match.start()].count('\n') + 1

        if ref.startswith('/') or re.match(r'^[A-Za-z]:', ref):
            failures.append('{}:{}: absolute path: {}'.format(doc, line, ref))
            continue

        head = ref.split('/')[0]
        if ref.startswith('.') or head not in TOP_LEVEL:
            if not is_ours(ref, here):
                continue
            checked += 1
            if not os.path.exists(os.path.normpath(os.path.join(here, ref))):
                failures.append('{}:{}: {}'.format(doc, line, ref))
            continue

        # A path into this repository, resolved from the root. The same spelling
        # may also name a file of the reader's project; accept either.
        checked += 1
        if not (os.path.exists(os.path.join(ROOT, ref))
                or os.path.exists(os.path.normpath(os.path.join(here, ref)))):
            failures.append('{}:{}: {}'.format(doc, line, ref))

failures += check_planted_template()

for name, tokens, files in CONSISTENT:
    for doc in files:
        with open(os.path.join(ROOT, doc), encoding='ascii') as handle:
            text = handle.read()
        missing = [t for t in tokens if t not in text]
        checked += 1
        if missing:
            failures.append('{}: {} -- absent: {}'.format(
                doc, name, ', '.join(missing)))

for failure in failures:
    print('missing: ' + failure)

print('{} references checked, {} missing'.format(checked, len(failures)))
sys.exit(1 if failures else 0)
