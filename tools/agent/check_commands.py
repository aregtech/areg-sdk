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

Most of a corpus's commands are neither dangerous nor free: they need a project to run
in, or a build in build/bin, or a minute of wall clock. --deep runs those too. It
scaffolds throw-away projects in a temporary directory -- one per precondition a
documented command has -- and gives every command its own copy of the one it needs, so
no command sees what another one wrote and the order the documents are read in does
not matter. Nothing outside that directory is written, nothing is compiled by the
checker itself, and a command naming a file the throw-away project does not carry
stays a SKIP rather than becoming a false RED.

    python3 tools/agent/check_commands.py                     the agent corpus
    python3 tools/agent/check_commands.py --contrib           CLAUDE.md and .claude/
    python3 tools/agent/check_commands.py --deep              also the slow, safe ones
    python3 tools/agent/check_commands.py --verbose           every command and its state

Exit code 0 when nothing is RED and nothing is a HOLE, 1 otherwise.
"""

import argparse
import glob
import os
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
FENCE_RE = re.compile(r'^```(bash|sh|shell|bat|cmd)\s*$')
FENCE_END_RE = re.compile(r'^```\s*$')
FENCE_ANY_RE = re.compile(r'^```\S*\s*$')
# An unlabelled fence is read as shell when its first command starts with one of these.
SHELL_HEADS = ('python3', 'python', 'cmake', 'ctest', 'bash', 'sh', 'git', 'java',
               'start', 'cd', './', 'tools/', 'tools\\', 'build/', 'build\\')
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

# What --deep adds, and where each command is run. A variant matches when none of its
# 'deny' flags is present and one of its 'require' flags is, so one script can be a
# root command in one spelling and a project command in another; the first variant
# that matches decides. 'needs' names what has to be in the tree before the command
# can answer at all.
#
#   root      the SDK checkout, like the default set
#   scaffold  a project setup_project.py has just written: design.json is the template
#   project   the same with its documents and application generated, and bodies.txt
#             carrying one comment body per section, unapplied
#   filled    the same with those bodies applied, so no marker is left open
#   empty     a private empty directory, for a command that creates a project
#
# What stays out, and why: anything that compiles the framework, starts a service,
# or edits the checkout. check_invariants.py --build is out as well -- it seeds a
# defect per invariant and rebuilds for each, which is hours, not minutes. So is
# everything in GATED below.
DEEP = {
    'check_contract.py': [
        {'require': ('--audit-prohibitions', '--audit-legacy'), 'where': 'root'},
        {'where': 'filled'},
    ],
    'api_help.py': [{'where': 'root'}],
    'schema_help.py': [{'where': 'root'}],
    'setup_agent_memory.py': [{'require': ('--list', '--check'), 'where': 'root'}],
    'setup_agent_redirect.py': [{'require': ('--list', '--check'), 'where': 'root'}],
    'gen_docs.py': [{'require': ('--template',), 'where': 'scaffold'},
                    {'where': 'project'}],
    'gen_skeleton.py': [{'where': 'project'}],
    'fill_markers.py': [{'where': 'project'}],
    'setup_project.py': [{'where': 'empty'}],
}

# Commands CI already executes as a step of its own, in the spelling the documents
# give. Running them again here buys no verdict and costs the wall clock twice: the
# eval self-check alone has a job of its own with a 60 minute budget. What this
# checker can still say about them it says without running them -- the paths they
# name resolve, and every flag is one the tool's own --help advertises.
GATED = {
    'check_recipes.py': 'the recipes job runs it',
    'check_mutations.py': 'the recipes job runs it',
    'check_observability.py': 'the recipes job runs it',
    'run_evals.py': 'the evaluation bank job runs it',
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
            if FENCE_RE.match(line) or (FENCE_END_RE.match(line) and
                                        shell_block(lines, number)):
                inside = True
                pending = ''
            elif FENCE_ANY_RE.match(line):
                inside = None
            continue
        if FENCE_END_RE.match(line):
            if pending:
                found.append((began, pending))
                pending = ''
            inside = False
            continue
        if inside is None:
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
        if FENCE_ANY_RE.match(line):
            inside = not inside
            continue
        if inside or line.lstrip().startswith('#'):
            continue
        for command in INLINE_RE.findall(line):
            found.append((number, command.strip()))
    return sorted(found)


# A fence with no language is the command on every platform, so it is read by an
# agent on Windows too. A trailing backslash there is a POSIX line continuation that
# neither cmd nor PowerShell joins, and a trailing caret is the mirror of it: each
# hands one platform half a command. A tagged fence is exempt -- bash says POSIX, and
# a bat fence ends a directory path in a backslash on purpose. An untagged fence that
# holds no command is exempt too: an ASCII diagram points at a line with a caret.
TAGGED_FENCE_RE = re.compile(r'^```(\S+)\s*$')
POSIX_ONLY = '\\'
WINDOWS_ONLY = '^'


def continuations(path):
    """Line continuations inside an untagged fence, as (line number, which, text)."""
    found = []
    with open(os.path.join(ROOT, path), encoding='utf-8', errors='replace') as handle:
        lines = handle.read().splitlines()
    tag = None
    inside = False
    for number, line in enumerate(lines, 1):
        if FENCE_END_RE.match(line) and inside:
            inside, tag = False, None
            continue
        marked = TAGGED_FENCE_RE.match(line)
        if not inside and (marked or FENCE_END_RE.match(line)):
            inside = True
            # shell_block() is this file's own answer to "does this untagged fence
            # hold commands", and blocks() reads the same fences by the same test.
            tag = (marked.group(1) if marked
                   else (None if shell_block(lines, number) else 'prose'))
            continue
        if not inside or tag is not None:
            continue
        text = line.rstrip()
        if text.endswith(POSIX_ONLY):
            found.append((number, 'a backslash, which cmd and PowerShell do not join',
                          text.strip()))
        elif text.endswith(WINDOWS_ONLY):
            found.append((number, 'a caret, which no POSIX shell joins', text.strip()))
    return found


def shell_block(lines, number):
    """True when the unlabelled fence opening at this line number holds shell commands."""
    for line in lines[number:]:
        text = line.strip()
        if FENCE_END_RE.match(text):
            return False
        if text and not text.startswith('#'):
            return text.startswith(SHELL_HEADS)
    return False


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

# A terminal escape sequence. Python 3.14 colours --help even into a pipe when
# FORCE_COLOR is set, and the "m" that ends each sequence hides the option after it.
ANSI_ESCAPE = re.compile(r'\x1b\[[0-9;]*[A-Za-z]')


def plain_env():
    """This environment with colour output turned off."""
    env = dict(os.environ)
    env.pop('FORCE_COLOR', None)
    env['NO_COLOR'] = '1'
    env['PYTHON_COLORS'] = '0'
    return env


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
                                    capture_output=True, text=True, timeout=60,
                                    env=plain_env())
            if result.returncode == 0:
                flags = set(re.findall(r'(?<![\w-])(--?[A-Za-z][-A-Za-z0-9_]*)',
                                       ANSI_ESCAPE.sub('', result.stdout)))
        except (OSError, subprocess.SubprocessError):
            flags = None
    _advertised[script] = flags
    return flags


def unknown_flags(command):
    """The flags a documented command carries that its tool does not advertise."""
    words = command.split()
    if not words:
        return None, []
    # A script is checked however it is spelled: "python3 tools/x.py", "./tools/x.py"
    # and "sudo python3 tools/x.py" all name the same flags. Requiring an interpreter
    # in front left every directly-invoked script unchecked.
    scripts = [w for w in words if w.endswith('.py')]
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


def variant_fits(rule, flags):
    """Whether one allowlist variant covers a command carrying these flags."""
    for word in flags:
        if word.split('=')[0] in rule.get('deny', ()):
            return False
    require = rule.get('require', ())
    return not require or any(flag in flags for flag in require)


def classify(command, deep=False):
    """The state of one command, where to run it, and why. Does not run anything."""
    head = command.split()[0].strip('`')
    lowered = command.lower()

    holes = [p for p in repository_paths(command)
             if not os.path.exists(os.path.join(ROOT, p))]
    if holes:
        return 'HOLE', 'root', 'names ' + ', '.join(sorted(set(holes))) + \
            ', not in the tree'

    if PLACEHOLDER_RE.search(command):
        return 'SKIP', 'root', 'carries a placeholder a reader has to fill in'
    if any(word in lowered for word in UNSAFE_WORDS):
        return 'SKIP', 'root', 'builds, starts or changes something'
    if head not in SAFE_HEAD:
        return 'SKIP', 'root', 'not one of the interpreters this checker drives'
    # "python" is the spelling Windows needs and most POSIX distributions do not
    # install. An interpreter this machine does not have says nothing about the
    # command, so it is a SKIP with the reason rather than an exit 127 reported as a
    # broken instruction.
    if shutil.which(head) is None:
        return 'SKIP', 'root', 'the interpreter "{}" is not on PATH here'.format(head)

    words = command.split()[1:]
    script = os.path.basename(words[0]) if words else ''
    flags = [w for w in words[1:] if w.startswith('-')]

    if script in READ_ONLY:
        rule = READ_ONLY[script]
        if variant_fits(rule, flags):
            return 'RUN', 'root', ''
        denied = [w for w in flags if w.split('=')[0] in rule.get('deny', ())]
        why = ('"{}" makes {} do work rather than answer'.format(denied[0], script)
               if denied else
               '{} is only run with {}'.format(script,
                                               ' or '.join(rule.get('require', ()))))
        if not deep:
            return 'SKIP', 'root', why
    else:
        why = 'not one of the tools this checker runs, even deep'

    if not deep:
        if script in DEEP:
            return 'SKIP', 'root', '{} is only run by --deep'.format(script)
        return 'SKIP', 'root', 'not one of the read-only tools this checker runs'

    if script in GATED:
        return 'SKIP', 'root', '{}: its path and its flags are checked here, and {}' \
            .format(script, GATED[script])

    for rule in DEEP.get(script, ()):
        if not variant_fits(rule, flags):
            continue
        missing = [n for n in rule.get('needs', ())
                   if not os.path.exists(os.path.join(ROOT, n))]
        if missing:
            return 'SKIP', 'root', '{} needs {}, which this tree does not carry'.format(
                script, ', '.join(missing))
        return 'RUN', rule.get('where', 'root'), ''
    return 'SKIP', 'root', why


# A command gets this long to answer, then a longer second chance. The budget is wall
# clock, so a loaded machine or a slow mount spends it without the command being at
# fault. Only a non-zero exit says a command is broken; running out of time says
# nothing about it either way.
FIRST_BUDGET = 120
SECOND_BUDGET = 600


# --deep runs the commands the default run leaves out because they are slow. A
# recipe sweep and an eval self-check are tens of minutes, and reporting either as
# SLOW would say nothing about it.
DEEP_BUDGET = 5400


def run(command, where='root', budgets=(FIRST_BUDGET, SECOND_BUDGET)):
    """Runs one command that classify() cleared, in the directory it belongs to.

    Returns (state, detail), where state is RUN for success, RED for a command that
    failed, and SLOW for one that ran out of time twice. A slow command is not
    reported as broken: the checker says it could not find out.
    """
    if where == 'root':
        folder = ROOT
    else:
        folder, why = sandbox_for(command, where)
        if folder is None:
            return 'SKIP', why
        command = absolute_scripts(command)
    for budget in budgets:
        try:
            result = subprocess.run(command, cwd=folder, shell=True,
                                    capture_output=True, text=True, timeout=budget)
        except subprocess.TimeoutExpired:
            continue
        if result.returncode == 0:
            return 'RUN', ''
        tail = (result.stdout + result.stderr).strip().splitlines()[-1:]
        return 'RED', 'exit {}: {}'.format(result.returncode,
                                           tail[0] if tail else '')
    return 'SLOW', ('no verdict: still running after {}s, then after {}s. The machine '
                    'was too loaded to time it, which is not a finding about the '
                    'command'.format(budgets[0], budgets[-1]))


# ---------------------------------------------------------------------------
# The throw-away projects --deep runs project commands in
#
# A documented command has a precondition -- a project that exists, a design that is
# still the template, a worksheet with bodies in it, a source tree with no marker
# left. Each is prepared once and copied per command, so a command that rewrites a
# source cannot change what the next one sees, the order the documents are read in
# does not decide the result, and a precondition the sandbox cannot meet is a SKIP
# rather than a RED. Nothing is compiled here: a command that needs a binary is a
# root command with a 'needs' of build/bin, or it is not run at all.
#
#   scaffold  setup_project.py has run, and nothing else: design.json is the template
#   project   its documents and its application are generated, and bodies.txt carries
#             one comment body per section, unapplied
#   filled    the same with those bodies applied, so no marker is left open
# ---------------------------------------------------------------------------
_holder = [None]
_prepared = {}
_made = [0]


def _call(base, argv, capture=None):
    """One tool of this SDK, run inside a prepared directory."""
    try:
        done = subprocess.run([sys.executable] + argv, cwd=base, capture_output=True,
                              text=True, timeout=SECOND_BUDGET)
    except (OSError, subprocess.SubprocessError):
        return False
    if done.returncode != 0:
        return False
    if capture:
        with open(os.path.join(base, capture), 'w', encoding='utf-8',
                  newline='\n') as handle:
            handle.write(done.stdout)
    return True


def _fill_worksheet(base):
    """One comment body under every section of the worksheet.

    The worksheet's own rule: a section that needs nothing is closed by one // line.
    Nothing here is compiled, so a comment is every body this checker has to write.
    """
    sheet = os.path.join(base, 'bodies.txt')
    if not os.path.isfile(sheet):
        return False
    with open(sheet, encoding='utf-8') as handle:
        lines = handle.read().splitlines()
    out = []
    for line in lines:
        out.append(line)
        if line.startswith('== '):
            out.append('// checked by check_commands.py')
    with open(sheet, 'w', encoding='utf-8', newline='\n') as handle:
        handle.write('\n'.join(out) + '\n')
    return True


def prepare(state):
    """The throw-away project one command needs, or '' when it cannot be built."""
    if state in _prepared:
        return _prepared[state]
    if _holder[0] is None:
        _holder[0] = tempfile.mkdtemp(prefix='check-commands-')
    base = os.path.join(_holder[0], state)
    tools = os.path.join(ROOT, 'tools', 'agent')

    if state == 'scaffold':
        os.makedirs(base)
        ok = _call(base, [os.path.join(tools, 'setup_project.py'), '--name', 'deep',
                          '--root', '.', '--mode', 'ipc', '--sdk-root', ROOT])
    elif state == 'project':
        earlier = prepare('scaffold')
        ok = bool(earlier)
        if ok:
            shutil.copytree(earlier, base)
            ok = _call(base, [os.path.join(tools, 'gen_docs.py'), '--example'],
                       'design.json')
            ok = ok and _call(base, [os.path.join(tools, 'gen_docs.py'), '--outdir',
                                     'src/services', '--force', '--chained', '--spec',
                                     'design.json'])
        if ok:
            docs = sorted(glob.glob(os.path.join(base, 'src', 'services', '*.siml')))
            machines = sorted(glob.glob(os.path.join(base, 'src', 'services', '*.fsml')))
            made = [os.path.join(tools, 'gen_skeleton.py'), '--doc',
                    os.path.relpath(docs[0], base).replace('\\', '/'), '--app',
                    '--mode', 'ipc', '--force'] if docs else None
            if made and machines:
                made += ['--machine',
                         os.path.relpath(machines[0], base).replace('\\', '/')]
            ok = bool(made) and _call(base, made) and _fill_worksheet(base)
    elif state == 'filled':
        earlier = prepare('project')
        ok = bool(earlier)
        if ok:
            shutil.copytree(earlier, base)
            ok = _call(base, [os.path.join(tools, 'fill_markers.py'), '--bodies',
                              'bodies.txt'])
    else:
        os.makedirs(base)
        ok = True
    if not ok:
        shutil.rmtree(base, ignore_errors=True)
    _prepared[state] = base if ok else ''
    return _prepared[state]


# A path a command names, as opposed to a flag or a bare word.
NAMED_PATH_RE = re.compile(r'[\w./\\-]+\.(?:py|sh|bat|siml|fsml|dtml|json|txt|xsd)$')


def sandbox_for(command, where):
    """A private directory for one command, or (None, why) when it cannot have one."""
    base = prepare(where)
    if not base:
        return None, 'the throw-away project for a "{}" command could not be ' \
            'prepared here'.format(where)
    if _holder[0] is None:                      # pragma: no cover - prepare() made it
        _holder[0] = tempfile.mkdtemp(prefix='check-commands-')
    _made[0] += 1
    folder = os.path.join(_holder[0], 'run{}'.format(_made[0]))
    shutil.copytree(base, folder)
    # A command naming a file of some other project is a SKIP, not a RED: the file
    # is missing from this sandbox, which says nothing about the command.
    for token in re.split(r'[\s"\'=]+', command):
        token = token.strip('`,;()')
        if not token or token.startswith('-') or not NAMED_PATH_RE.match(token):
            continue
        if token.replace('\\', '/').lstrip('./').startswith(TRACKED_TOPS):
            continue
        if not os.path.exists(os.path.join(folder, token)):
            shutil.rmtree(folder, ignore_errors=True)
            return None, 'names {}, which the throw-away project does not carry'.format(
                token)
    return folder, ''


def absolute_scripts(command):
    """The command with every tools/ script path made absolute.

    A project command runs in a directory that is not the checkout, so a script path
    relative to the checkout resolves to nothing there.
    """
    out = []
    for token in command.split():
        bare = token.strip('`').replace('\\', '/')
        if bare.startswith('./'):
            bare = bare[2:]
        if bare.startswith(TRACKED_TOPS) and bare.endswith(('.py', '.sh', '.bat')):
            out.append(os.path.join(ROOT, bare))
        else:
            out.append(token)
    return ' '.join(out)


def drop_sandbox():
    if _holder[0]:
        shutil.rmtree(_holder[0], ignore_errors=True)
        _holder[0] = None
        _prepared.clear()


def main():
    parser = argparse.ArgumentParser(
        description='Check every documented command, and run the safe ones.')
    parser.add_argument('--contrib', action='store_true',
                        help='the framework contributor corpus instead of the agent one')
    parser.add_argument('--deep', action='store_true',
                        help='also run the slow, safe ones: project commands in a '
                             'throw-away project, and the checkers that need build/bin')
    parser.add_argument('--verbose', action='store_true')
    parser.add_argument('docs', nargs='*', help='documents to read instead of a corpus')
    args = parser.parse_args()

    documents = args.docs or (CONTRIB_DOCS if args.contrib else AGENT_DOCS)
    documents = [d for d in documents if os.path.isfile(os.path.join(ROOT, d))]
    if not documents:
        sys.stderr.write('error: none of those documents exist\n')
        return 1

    counts = {'RUN': 0, 'RED': 0, 'HOLE': 0, 'SKIP': 0, 'SLOW': 0}
    flagged = 0
    problems = []
    for document in documents:
        for number, command in blocks(document):
            command = substitute(command)
            state, where, why = classify(command, args.deep)
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
                budgets = ((FIRST_BUDGET, DEEP_BUDGET) if args.deep
                           else (FIRST_BUDGET, SECOND_BUDGET))
                outcome, detail = run(command, where, budgets)
                if outcome != 'RUN':
                    state, why = outcome, detail
            counts[state] += 1
            where = '{}:{}'.format(document, number)
            if state in ('RED', 'HOLE', 'SLOW'):
                problems.append('{:<5} {:<28} {}\n      {}\n      {}'.format(
                    state, where, command, why, ''))
            elif args.verbose:
                label = 'FLAGS' if state == 'SKIP' and script and not bad else state
                print('{:<5} {:<28} {}'.format(label, where, command))

    drop_sandbox()

    # Static, and it needs no sandbox: a command that cannot be typed on one of the
    # platforms the page claims is broken whether or not this checker can run it.
    split = 0
    for document in documents:
        for number, which, text in continuations(document):
            split += 1
            problems.append('SPLIT {}:{}\n      {}\n      an untagged fence continues '
                            'a line with {}. Write it on one line, or tag the fence'
                            .format(document, number, text, which))

    for problem in problems:
        print(problem.rstrip())
    total = sum(counts.values())
    # The headline says coverage before it says verdicts. "0 red" out of 60 discovered
    # reads as sixty commands verified; it is a verdict on the five that ran.
    covered = counts['RUN'] + counts['RED']
    print('{} command(s) discovered in {} document(s): {}/{} executed{} ({}/{} of the '
          'rest had their flags checked against --help), {} passed, {} red, '
          '{} unresolved, {} not run here{}'
          .format(total, len(documents), covered, total,
                  '' if args.deep else ', and --deep runs more',
                  flagged, counts['SKIP'],
                  counts['RUN'], counts['RED'], counts['HOLE'], counts['SKIP'],
                  ', {} without a verdict'.format(counts['SLOW'])
                  if counts['SLOW'] else ''))
    print('{} untagged fenced line(s) continued in a way one platform cannot join.'
          .format(split))
    return 1 if (counts['RED'] or counts['HOLE'] or split) else 0


if __name__ == '__main__':
    sys.exit(main())
