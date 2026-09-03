#!/usr/bin/env python3
# -*- coding: ascii -*-
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""
Every command a document gives, checked against the tree, and the safe ones run.

A documented command is an instruction, and an instruction that cannot be pasted is
worse than a missing one: it is followed first and discovered afterwards. Two failures
of exactly this kind have already been paid for -- a checker whose script had CRLF in
the working tree and died with 'syntax error near unexpected token', and a sequence of
tools that only works when run in an order the document does not give.

Each fenced 'bash', 'sh' or 'bat' block is split into commands and each command lands
in one of four states:

  RUN      the command was executed here and exited 0
  RED      the command was executed here and did not
  HOLE     it names a file, a script or a tool that is not in the tree
  SKIP     it is not safe or not sensible to run from a checker -- it builds, it
           starts a service, it edits the tree, or it carries an unfilled
           <placeholder> that a reader has to substitute

SKIP is not a pass. It is the honest size of what this cannot verify, and it is
reported as a number so the gap does not read as coverage.

    python3 tools/agent/check_commands.py                     the agent corpus
    python3 tools/agent/check_commands.py --contrib           CLAUDE.md and .claude/
    python3 tools/agent/check_commands.py --verbose           every command and its state

Exit code 0 when nothing is RED and nothing is a HOLE, 1 otherwise.
"""

import argparse
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
FENCE_RE = re.compile(r'^```(bash|sh|shell|bat|cmd)\s*$')
FENCE_END_RE = re.compile(r'^```\s*$')
# A tool invocation written inline rather than fenced. Most of the routing tables --
# CLAUDE.md section 4, AGENTS.md section 5 -- give their commands this way, and those
# are the commands a reader actually pastes.
INLINE_RE = re.compile(r'`((?:python3?|tools/)[^`]*?\.(?:py|sh|bat)[^`]*)`')
PLACEHOLDER_RE = re.compile(r'<[a-z][a-z0-9 _.-]*>', re.IGNORECASE)
# The one placeholder a checker can fill in for itself: the documents write the SDK
# root as a placeholder because a reader's copy is elsewhere, and here it is the
# working directory. Substituting it is what makes those commands checkable at all.
SDK_ROOT_PLACEHOLDERS = ('<areg-sdk>', '<areg-sdk-root>', '<sdk>', '<sdk-root>')

# The corpus an application builder reads, and the corpus a framework contributor
# reads. They are separate because they are separate agendas: nothing tracked may
# point at '.claude/', so the second list only exists on a machine that has it.
AGENT_DOCS = ['AGENTS.md', 'CODEBASE.md'] + [
    os.path.join('docs', 'agent', name)
    for name in sorted(os.listdir(os.path.join(ROOT, 'docs', 'agent')))
    if name.endswith('.md')] + [os.path.join('docs', 'agent', 'recipes', 'README.md')]
CONTRIB_DOCS = ['CLAUDE.md', 'CONTRIBUTING.md'] + [
    os.path.join('.claude', 'memory', name)
    for name in sorted(os.listdir(os.path.join(ROOT, '.claude', 'memory')))
    if name.endswith('.md')] if os.path.isdir(os.path.join(ROOT, '.claude', 'memory')) \
    else ['CLAUDE.md', 'CONTRIBUTING.md']

SAFE_HEAD = ('python3', 'python')

# The scripts this checker is allowed to execute, and the arguments each is allowed
# to carry. They read the tree and write nothing outside a temporary directory, and
# every one of them is under a second or two. Anything not listed is a SKIP: a tool
# that builds, installs, generates or starts a service is not something a checker of
# documents may run, however harmless the command line looks.
# 'deny' names the flags that would turn a read-only tool into a build; 'require'
# names a flag the command must carry before it is safe at all. A flag in neither
# list is run, because an unknown flag is exactly the defect worth catching: argparse
# rejects it, and a documented command with a misspelled flag is a dead instruction.
READ_ONLY = {
    'check_agent_docs.py': {},
    'check_symbols.py': {},
    'check-ascii.py': {},
    'check_corpus.py': {},
    'explain_rule.py': {},
    'check_commands.py': {'require': ('--help',)},
    'check_mutations.py': {'deny': ('--lib',)},
    'check_invariants.py': {'require': ('--dry-run', '--list'), 'deny': ('--build',)},
}

# Any command carrying one of these is never run, whatever else it says.
UNSAFE_WORDS = ('cmake', 'ctest', 'make ', 'ninja', 'msbuild', 'git ', 'rm ', 'del ',
                'sudo', 'apt', 'brew', 'choco', 'java', 'start ', 'kill', 'pkill',
                'mtrouter', 'logcollector', 'logobserver', 'sanitize.sh', '>', '&&')


def blocks(path):
    """Every fenced shell command of one document, as (line number, text).

    A command continued with a trailing backslash, or with a caret on Windows, is
    joined into one: half a command classified on its own reads as a tool nobody has.
    """
    found = []
    with open(os.path.join(ROOT, path), encoding='utf-8', errors='replace') as handle:
        lines = handle.read().splitlines()
    inside = False
    pending = ''
    began = 0
    for number, line in enumerate(lines, 1):
        if not inside:
            if FENCE_RE.match(line):
                inside = True
                pending = ''
            continue
        if FENCE_END_RE.match(line):
            if pending:
                found.append((began, pending))
                pending = ''
            inside = False
            continue
        text = line.strip()
        if not text or text.startswith('#') or text.startswith('::'):
            continue
        if not pending:
            began = number
        if text.endswith('\\') or text.endswith('^') or text.endswith('`'):
            pending = (pending + ' ' + text[:-1].strip()).strip()
            continue
        found.append((began, (pending + ' ' + text).strip() if pending else text))
        pending = ''

    inside = False
    for number, line in enumerate(lines, 1):
        if FENCE_RE.match(line) or (inside and FENCE_END_RE.match(line)):
            inside = not inside
            continue
        if inside or line.lstrip().startswith('#'):
            continue
        for command in INLINE_RE.findall(line):
            found.append((number, command.strip()))
    return sorted(found)


# The top level directories a repository path can start with. A path outside them is
# something the command makes or the reader supplies -- a project directory, a build
# output, a log file -- and this checker has nothing to say about it.
TRACKED_TOPS = ('tools/', 'docs/', 'framework/', 'examples/', 'conf/', 'tests/',
                '.github/', '.claude/')


def repository_paths(command):
    """The repository paths a command names. Everything else is left alone."""
    wanted = []
    for token in re.split(r'[\s"\'=]+', command):
        token = token.strip('`,;()')
        if not token or token.startswith('-'):
            continue
        token = token.replace('\\', '/')
        # '<areg-sdk>/tools/x.py' and './tools/x.py' both name tools/x.py.
        token = PLACEHOLDER_RE.sub('', token).lstrip('/')
        if token.startswith('./'):
            token = token[2:]
        if token.startswith(TRACKED_TOPS):
            wanted.append(token)
    return wanted


def substitute(command):
    """The command with the SDK root placeholder replaced by the working directory."""
    for placeholder in SDK_ROOT_PLACEHOLDERS:
        command = command.replace(placeholder + '/', '')
        command = command.replace(placeholder, '.')
    return command


# Flags a shell, not the script, consumes.
SHELL_FLAGS = ('-c', '-m', '-u')

_advertised = {}


def advertised_flags(script):
    """The option strings a tool's own --help prints, or None if it has none.

    Running --help asks the tool what it accepts instead of guessing. It parses
    arguments and exits, so it builds nothing and starts nothing.
    """
    if script in _advertised:
        return _advertised[script]
    path = os.path.join(ROOT, script)
    flags = None
    if os.path.isfile(path):
        try:
            result = subprocess.run([sys.executable, path, '--help'], cwd=ROOT,
                                    capture_output=True, text=True, timeout=60)
            if result.returncode == 0:
                flags = set(re.findall(r'(?<![\w-])(--?[A-Za-z][-A-Za-z0-9_]*)',
                                       result.stdout))
        except (OSError, subprocess.SubprocessError):
            flags = None
    _advertised[script] = flags
    return flags


def unknown_flags(command):
    """The flags a documented command carries that its tool does not advertise."""
    words = command.split()
    if not words or os.path.basename(words[0]).split('.')[0] not in ('python3', 'python'):
        return None, []
    scripts = [w for w in words[1:] if w.endswith('.py')]
    if not scripts:
        return None, []
    script = substitute(scripts[0]).lstrip('./')
    if not script.startswith(TRACKED_TOPS):
        return None, []
    known = advertised_flags(script)
    if known is None:
        return None, []
    used = []
    for word in words[1:]:
        word = word.split('=')[0].strip('`,;()')
        if word.startswith('-') and word not in SHELL_FLAGS and not PLACEHOLDER_RE.search(word):
            used.append(word)
    return script, [f for f in used if f not in known]


def classify(command):
    """The state of one command, and why. Does not run anything."""
    head = command.split()[0].strip('`')
    lowered = command.lower()

    holes = [p for p in repository_paths(command)
             if not os.path.exists(os.path.join(ROOT, p))]
    if holes:
        return 'HOLE', 'names ' + ', '.join(sorted(set(holes))) + ', not in the tree'

    if PLACEHOLDER_RE.search(command):
        return 'SKIP', 'carries a placeholder a reader has to fill in'
    if any(word in lowered for word in UNSAFE_WORDS):
        return 'SKIP', 'builds, starts or changes something'
    if head not in SAFE_HEAD:
        return 'SKIP', 'not one of the interpreters this checker drives'

    words = command.split()[1:]
    script = os.path.basename(words[0]) if words else ''
    if script not in READ_ONLY:
        return 'SKIP', 'not one of the read-only tools this checker runs'
    rule = READ_ONLY[script]
    flags = [w for w in words[1:] if w.startswith('-')]
    for word in flags:
        if word.split('=')[0] in rule.get('deny', ()):
            return 'SKIP', '"{}" makes {} do work rather than answer'.format(word, script)
    require = rule.get('require', ())
    if require and not any(flag in flags for flag in require):
        return 'SKIP', '{} is only run with {}'.format(script, ' or '.join(require))
    return 'RUN', ''


def run(command):
    """Runs one command that classify() cleared. Returns (ok, detail)."""
    try:
        result = subprocess.run(command, cwd=ROOT, shell=True, capture_output=True,
                                text=True, timeout=120)
    except subprocess.TimeoutExpired:
        return False, 'did not finish within 120s'
    if result.returncode == 0:
        return True, ''
    tail = (result.stdout + result.stderr).strip().splitlines()[-1:]
    return False, 'exit {}: {}'.format(result.returncode, tail[0] if tail else '')


def main():
    parser = argparse.ArgumentParser(
        description='Check every documented command, and run the safe ones.')
    parser.add_argument('--contrib', action='store_true',
                        help='the framework contributor corpus instead of the agent one')
    parser.add_argument('--verbose', action='store_true')
    parser.add_argument('docs', nargs='*', help='documents to read instead of a corpus')
    args = parser.parse_args()

    documents = args.docs or (CONTRIB_DOCS if args.contrib else AGENT_DOCS)
    documents = [d for d in documents if os.path.isfile(os.path.join(ROOT, d))]
    if not documents:
        sys.stderr.write('error: none of those documents exist\n')
        return 1

    counts = {'RUN': 0, 'RED': 0, 'HOLE': 0, 'SKIP': 0}
    flagged = 0
    problems = []
    for document in documents:
        for number, command in blocks(document):
            command = substitute(command)
            state, why = classify(command)
            # A command this checker cannot run still names flags, and a flag the
            # tool stopped accepting is a dead instruction whether it runs or not.
            script, bad = unknown_flags(command)
            if script:
                if bad:
                    state, why = 'RED', '{} does not accept {}'.format(
                        script, ', '.join(sorted(set(bad))))
                elif state != 'RUN':
                    flagged += 1
            if state == 'RUN':
                ok, detail = run(command)
                if not ok:
                    state, why = 'RED', detail
            counts[state] += 1
            where = '{}:{}'.format(document, number)
            if state in ('RED', 'HOLE'):
                problems.append('{:<5} {:<28} {}\n      {}\n      {}'.format(
                    state, where, command, why, ''))
            elif args.verbose:
                print('{:<5} {:<28} {}'.format(state, where, command))

    for problem in problems:
        print(problem.rstrip())
    total = sum(counts.values())
    print('{} command(s) in {} document(s): {} ran, {} flags checked against '
          '--help, {} red, {} unresolved, {} not run here'
          .format(total, len(documents), counts['RUN'], flagged, counts['RED'],
                  counts['HOLE'], counts['SKIP']))
    return 1 if (counts['RED'] or counts['HOLE']) else 0


if __name__ == '__main__':
    sys.exit(main())
