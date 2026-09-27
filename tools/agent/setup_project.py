#!/usr/bin/env python3
# ===========================================================================
# Creates a ready-to-build AREG project outside the SDK.
#
# The project skeleton is one of the recipes under docs/agent/recipes/, copied
# and renamed, so the code a new project starts from is the code the recipes
# already build.
#
#   python3 tools/agent/setup_project.py --name myapp --root ~/myapp --mode local
#   python3 tools/agent/setup_project.py --name myapp --root ~/myapp --mode ipc \
#           --sdk-root /opt/areg-sdk
#
# Run with no arguments on a terminal to be asked for them.
#
# Exit code 0 on success, 1 on a bad argument or a refusal to overwrite,
# 2 when a required tool is missing.
# ===========================================================================
import argparse
import json
import os
import re
import shutil
import stat
import sys
import textwrap

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import gen_docs  # noqa: E402


def find_agent_docs():
    """Locates the directory holding the agent documentation and its recipes.

    A clone keeps it at <root>/docs/agent, two levels above tools/agent. An
    installation keeps the same tree under share/areg/sdk/docs/agent, next to
    the tools. Returns None when the tools were copied away from both.
    """
    candidates = [os.path.join(HERE, os.pardir, os.pardir, 'docs', 'agent'),
                  os.path.join(HERE, os.pardir, os.pardir, os.pardir,
                               'share', 'areg', 'sdk', 'docs', 'agent'),
                  os.path.join(HERE, os.pardir, os.pardir, os.pardir, os.pardir,
                               'share', 'areg', 'sdk', 'docs', 'agent')]
    for candidate in candidates:
        path = os.path.normpath(candidate)
        if os.path.isdir(os.path.join(path, 'recipes')):
            return path
    return None


SDK_ROOT = os.path.dirname(os.path.dirname(HERE))
AGENT_DOCS = find_agent_docs()
RECIPES = os.path.join(AGENT_DOCS, 'recipes') if AGENT_DOCS else ''
# The revision to fetch when --tag is not given. api.json owns it; this literal is
# the answer when api.json cannot be read, and check_corpus.py holds the two equal.
FALLBACK_TAG = 'master'


def default_tag():
    """The git ref docs/agent/api.json says a new project should fetch."""
    path = os.path.join(AGENT_DOCS, 'api.json') if AGENT_DOCS else ''
    try:
        with open(path, 'r', encoding='utf-8') as handle:
            stated = json.load(handle).get('sdk', {}).get('fetch_ref')
    except (OSError, ValueError):
        return FALLBACK_TAG
    return stated or FALLBACK_TAG


DEFAULT_TAG = default_tag()

# Each mode names a recipe and the tokens that carry the project name in its
# CMake files. Only CMake files are rewritten; sources are left as written.
MODES = {
    'local': {
        'recipe': '01-local-single-process',
        'tokens': [('hello_local', '{name}')],
        'binaries': ['{name}'],
        'router': False,
        'expect': [['provider: hello,', 'consumer: greeted']],
    },
    'ipc': {
        'recipe': '02-ipc-two-processes',
        'tokens': [('hello_provider', '{name}_provider'),
                   ('hello_consumer', '{name}_consumer'),
                   ('hello_ipc', '{name}')],
        'binaries': ['{name}_provider', '{name}_consumer'],
        'router': True,
        'expect': [['provider: hello,'], ['consumer: greeted']],
    },
    'pubsub': {
        'recipe': '03-attributes-and-broadcast',
        'tokens': [('gen_monitor', 'gen_{name}'),
                   ('"monitor"', '"{name}"'),
                   ('macro_declare_executable(monitor ', 'macro_declare_executable({name} ')],
        'binaries': ['{name}'],
        'router': False,
        'expect': [['consumer: temperature is 45', 'consumer: alarm at 45',
                    'consumer: measurement taken']],
    },
}

NAME_PATTERN = re.compile(r'^[A-Za-z_][A-Za-z0-9_]*$')


def fail(message, code=1):
    # Output already printed is flushed first: stdout is block-buffered into a
    # pipe, so without this the error reaches the reader before the lines it is
    # about.
    sys.stdout.flush()
    sys.stderr.write('error: {}\n'.format(message))
    sys.exit(code)


def check_tools(needs_git):
    """Reports the tools the created project will need to build."""
    missing = [tool for tool in (['cmake'] + (['git'] if needs_git else []))
               if shutil.which(tool) is None]
    if missing:
        fail('not found on PATH: {}'.format(', '.join(missing)), 2)


def ask(prompt, default):
    try:
        answer = input('{} [{}]: '.format(prompt, default)).strip()
    except EOFError:
        return default
    return answer or default


def fetch_block(sdk_root, tag):
    """The FetchContent lines that bring in the SDK, local copy or GitHub."""
    if sdk_root:
        return ('    FetchContent_Declare(areg SOURCE_DIR "{}")\n'
                '    FetchContent_MakeAvailable(areg)'.format(sdk_root.replace('\\', '/')))
    return ('    FetchContent_Declare(areg\n'
            '        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git\n'
            '        GIT_TAG "{}")\n'
            '    FetchContent_MakeAvailable(areg)'.format(tag))


INCLUDE_ROOT = 'include_directories("${CMAKE_CURRENT_SOURCE_DIR}")'


def rewrite_top_cmake(path, name, sdk_root, tag):
    with open(path, encoding='utf-8') as handle:
        text = handle.read()

    # Built from the same source the recipe's literal is held to, so the block
    # this looks for cannot drift from the block the recipes carry.
    old = fetch_block(None, DEFAULT_TAG)
    if old not in text:
        fail('recipe CMakeLists.txt does not carry the expected FetchContent block')
    text = text.replace(old, fetch_block(sdk_root, tag))

    # The project root on the include path: a header of the project is included as
    # "src/x/Name.hpp" exactly as a generated one is, from any directory of src/.
    if INCLUDE_ROOT not in text:
        text = text.replace('add_subdirectory(src)', INCLUDE_ROOT + '\nadd_subdirectory(src)', 1)

    with open(path, 'w', encoding='utf-8') as handle:
        handle.write(text)


def substitute(path, tokens):
    with open(path, encoding='utf-8') as handle:
        text = handle.read()
    for old, new in tokens:
        text = text.replace(old, new)
    with open(path, 'w', encoding='utf-8') as handle:
        handle.write(text)


# A call written in prose, and a macro name. Both become code spans: a placeholder
# such as notify_on_broadcast_<name> is read as a tag by a Markdown renderer.
CALL_RE  = re.compile(r'\b[A-Za-z_][A-Za-z0-9_:]*(?:<[a-z]+>)?[A-Za-z0-9_]*\([^()]*\)')
MACRO_RE = re.compile(r'\b[A-Z][A-Z0-9]*(?:_[A-Z0-9]+)+\b')


def as_markdown(text):
    """Puts the code written inside a rule sentence into code spans."""
    text = CALL_RE.sub(lambda hit: '`%s`' % hit.group(0), text)
    return MACRO_RE.sub(lambda hit: '`%s`' % hit.group(0), text)


def prohibition_bullets():
    """The prohibitions of docs/agent/api.json, as bullets for the project page.

    Read at generation time rather than copied, so a project written today carries
    the list the SDK states today. Returns None when api.json cannot be read.
    """
    path = os.path.join(AGENT_DOCS, 'api.json') if AGENT_DOCS else ''
    try:
        with open(path, 'r', encoding='utf-8') as handle:
            stated = json.load(handle).get('prohibitions', [])
    except (OSError, ValueError):
        return None
    bullets = []
    for item in stated:
        rule = ' '.join(str(item.get('rule', '')).split())
        if rule:
            bullets.append(textwrap.fill('- ' + as_markdown(imperative(rule)), 88,
                                         subsequent_indent='  '))
    return '\n'.join(bullets) if bullets else None


# This page is resident in every request of a run, so a rule costs its bytes once per
# request. The sentence that states the rule prevents the mistake; the sentences that
# explain the mechanism are what check_contract.py prints at the file and the line the
# moment the rule is broken, which is where they are worth reading.
SENTENCE_END = re.compile(r'(.+?[.!])(?:\s|$)')


def imperative(rule):
    """The sentence that states a rule, without the mechanism behind it."""
    found = SENTENCE_END.match(rule)
    return found.group(1) if found else rule


# The file each harness reads on startup, and whether it needs one at all. A
# harness that reads AGENTS.md itself is not given a second file to disagree with.
# Anything else lands on the project's AGENTS.md only if it is pointed there.
HARNESS_FILES = {
    'claude':   'CLAUDE.md',
    'gemini':   'GEMINI.md',
    'aider':    'CONVENTIONS.md',
    'cline':    '.clinerules',
    'continue': '.continuerules',
    'windsurf': '.windsurfrules',
    'cursor':   '.cursor/rules/project.mdc',
    'copilot':  '.github/copilot-instructions.md',
    'codex':    None,
    'agents':   None,
}

REDIRECT = """{fence}# {name}

This file exists only so that this harness lands in the right place. It holds no
content of its own.

**Read [`AGENTS.md`]({up}AGENTS.md) in this project's root.** It is the whole guide
for this project: what it is, how to build and run it, which page of the AREG SDK
documentation answers which question, and what never to do.

Do not search this project or the SDK before reading it.

<!-- Written by the AREG SDK setup_project.py. Edit AGENTS.md, never this file. -->
"""

REDIRECT_FENCE = {
    '.cursor/rules/project.mdc':
        '---\ndescription: project entry point\nalwaysApply: true\n---\n',
}


def write_redirects(root, name, harnesses):
    """The pointer file each named harness reads, aimed at the project AGENTS.md.

    A project scaffolded for an unknown agent has to work for any of them. The
    harnesses that read AGENTS.md themselves are skipped, so nothing is written
    that could drift from it.
    """
    written = []
    for harness in harnesses:
        relative = HARNESS_FILES.get(harness)
        if not relative:
            continue
        path = os.path.join(root, *relative.split('/'))
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, 'w', encoding='utf-8') as handle:
            handle.write(REDIRECT.format(
                fence=REDIRECT_FENCE.get(relative, ''), name=name,
                up='../' * relative.count('/')))
        written.append(relative)
    return written


def write_agents(root, name, mode, sdk_root, binaries):
    """The project's own AGENTS.md: what an agent working here loads first."""
    sdk = sdk_root if sdk_root else 'build/packages/areg-src'
    # The SDK lands somewhere different for a clone, a fetch and an installed
    # package, so the project is told how to ask rather than given one answer.
    where = ('`build/areg-sdk.paths` names where the SDK is on this machine, one '
             '`key = path` per line: `sdk_root`, `headers`, `agent_docs`, '
             '`agents_md`, `codegen` and `schema`. CMake writes it while it '
             'configures, so '
             'run `cmake -B build` first if it is not there; if it is still absent '
             'after that, ask for the SDK path and pass it as '
             '`-DAREG_SDK_ROOT=<path>`. The commands below assume `{}`.'
             .format(sdk))
    never = prohibition_bullets()
    if never is None:
        never = ('- The full list is section 6 of `{}/AGENTS.md`; api.json could not be '
                 'read when this project was created.'.format(sdk))
    if MODES[mode]['router']:
        manual = ('\n`./run.sh`, and `run.bat` on Windows, start the same processes '
                  'outside a scenario\nwhen one has to be watched live. They are the '
                  'only hand launchers this project\nhas; the check above stays the '
                  'route that decides whether it works.\n')
    else:
        manual = ('\n`./build/bin/{}` alone starts this project outside a scenario '
                  'when it has to be\nwatched live; the check above stays the route '
                  'that decides whether it works.\n'.format(binaries[0] + '.elf'))

    text = """# {name}

An application built on the AREG framework. AREG generates the communication code
from a service contract; this project implements only the service logic.

## What must be installed

CMake 3.20 or newer, a Java 17 or newer runtime (it runs the code generator), and a
C++17 or newer compiler: GCC, Clang, MSVC or MinGW. Tested on Linux and other POSIX,
macOS, Windows and Cygwin. Python 3 is not needed to build or run this project, only
for the SDK helper scripts listed below.

## Build and run

`design.json` is where the design goes: the scaffold wrote it with every key present
and empty, and every document of this project is generated from it. Fill its values
and keep its keys, then:

```bash
python3 {sdk}/tools/agent/build_project.py --spec design.json
python3 {sdk}/tools/agent/build_project.py --run
```

The same on Windows, where the interpreter is `python`:

```bat
python {sdk}/tools/agent/build_project.py --spec design.json
python {sdk}/tools/agent/build_project.py --run
```

The first call builds, and checks the design before anything else: a refused design
stops it within seconds. `--run` rebuilds whatever changed and then runs every scenario in
`scenarios.json`: it starts each process in the order that project needs, waits for
the lines the scenario expects and stops every process it started. These two commands
are the whole route, on both systems, and nothing else here starts a process.

**`--run` exiting 0 is what done means**, and the lines it prints are the evidence to
report. Every acceptance item belongs in `scenarios.json`, the peer going away as a
scenario-level `"stop"` and the console quit path as the `quit` scenario that is
already there. Never start the processes by hand with `&`, `sleep`, `pkill` or `ps`.
{manual}
Executables are written to `build/bin/`. The suffix is `.elf` on Linux, `.mac` on
macOS and `.exe` on Windows.

## Layout

```
src/services/         the documents; the generator reads them at configure time
src/<Name>.hpp/.cpp   one component each, named after its class
src/provider/main.cpp the model and main() of a process; two processes get a folder
                      each (provider/, consumer/), one process keeps src/ flat
areg-project.json     names the design files; build_project.py reads it when no
                      --spec is given, so an edited design still reaches the build
CMakeLists.txt        finds or fetches the AREG SDK
src/CMakeLists.txt    names the documents and each executable's sources
```

## Where the framework documentation is

{where} Read one page for the task, not the whole set:

| I need to ... | Read |
|---|---|
| **Build this application, start to finish** | `docs/agent/01-runbook.md` - every command in order, if you are not already following it |
| **Anything ordinary** | `docs/agent/00-cheatsheet.md` - what the tools do not write |
| Decide what the services are | `docs/agent/05-design.md`, before writing any file |
| Change the service contract | `docs/agent/20-service-interface.md` |
| Declare a structure, enum or container | `docs/agent/21-data-types.md` |
| Behaviour that depends on what happened before | `docs/agent/22-state-machine.md` (a `.fsml`) |
| `areg::String` and the containers | `docs/agent/40-base-api.md` -- before the first line of C++ |
| The signature of one framework name | `python3 {sdk}/tools/agent/api_help.py <name>` -- never a page, never a header |
| Implement a provider, a consumer, or the model | nothing: `gen_skeleton.py --app` wrote all three. Open `docs/agent/30-provider.md`, `docs/agent/31-consumer.md` or `docs/agent/32-model.md` only at a numbered section `51-debug.md` or `05-design.md` names |
| Periodic or delayed work | the consumer already owns a stepping timer; for a second timer `docs/agent/33-timers.md` |
| A custom event between threads | `docs/agent/23-events.md` |
| Worker threads, watchdogs, a run-time model | `docs/agent/37-threads.md` |
| The application, components, time, files | `docs/agent/42-runtime-api.md` |
| Log from application code | `docs/agent/34-logging.md` |
| Start the pieces in the right order | nothing: `run_scenarios.py` does it and `--app` wrote `scenarios.json`. `--only` is for iterating on a failure, never for confirming a pass. `docs/agent/50-running.md` for `router`, `lead` and `stop`/`after`/`signal` |
| Write a test | `docs/agent/52-testing.md` |
| Work out why it does not work | `docs/agent/51-debug.md` |
| **Anything this table does not cover** | `AGENTS.md` section 2 in the SDK -- it routes the full set. Never search the SDK by hand |

## Tools

The three commands above are the whole path; `docs/agent/01-runbook.md` sections 3 to 7
carry them step by step and nothing else has to be run by hand. These answer a
question those two cannot, and **reaching for one before you have that question costs
a turn**:

| Ask | Only when |
|---|---|
| `python3 {sdk}/tools/agent/gen_skeleton.py --doc <document> --contract` | you need the name a document generates -- a method, or a type a signature is written in. `--todos` lists the markers still open |
| `python3 {sdk}/tools/agent/api_help.py <name>` | you need the signature of a **framework** name. Never grep the SDK for one |
| `python3 {sdk}/tools/explain_rule.py <number> --at <Element>/@<Attribute>` | `gen_docs.py` refused a document and its `fix:` line was not enough |
| `python3 {sdk}/tools/schema_help.py <name> --document fsml` | `gen_docs.py` refused a document over a name. Nothing before that: you write `design.json`, and the generator writes every element name in it |

All take `--help`. On Windows the interpreter is `python`, not `python3`.

## Never

Each line closes a class of wrong code, not a style preference.
`check_contract.py .` reports the ones the sources of this project can show; the
generated code under `build/` it does not read.

{never}
""".format(name=name, manual=manual, where=where, sdk=sdk, never=never)

    with open(os.path.join(root, 'AGENTS.md'), 'w', encoding='utf-8') as handle:
        handle.write(text)


def write_scenarios(root, mode, binaries):
    """The scenario that proves the project works, for tools/agent/run_scenarios.py."""
    expectations = MODES[mode]['expect']
    procs = []
    for index, binary in enumerate(binaries):
        spec = {'binary': binary, 'expect': expectations[index]}
        # Only the last process ends by itself; the others are stopped with it.
        if index == len(binaries) - 1:
            spec['exit'] = 0
        procs.append(spec)

    # The expectations below are the recipe's own and stop meaning anything the moment
    # the documents are replaced. The marker says so, and gen_skeleton.py --app
    # replaces them once and clears it. Without it the tool cannot tell a scaffolded
    # expectation from one that was written by hand, and would overwrite both.
    document = {'scenarios': [{'name': 'smoke',
                               'timeout': 60,
                               'router': MODES[mode]['router'],
                               'scaffold': True,
                               'procs': procs}]}
    with open(os.path.join(root, 'scenarios.json'), 'w', encoding='utf-8') as handle:
        json.dump(document, handle, indent=2)
        handle.write('\n')


def write_manifest(root):
    """The project's record of the files that describe it, for build_project.py.

    A later call given no --spec reads it, so the documents and the application
    are written from the design as it stands rather than left as they were.
    """
    with open(os.path.join(root, 'areg-project.json'), 'w', encoding='utf-8') as handle:
        json.dump({'spec': ['design.json']}, handle, indent=2)
        handle.write('\n')


def write_gitignore(root):
    with open(os.path.join(root, '.gitignore'), 'w', encoding='utf-8') as handle:
        handle.write('build/\nbuild-*/\nproduct/\n*.sqlog\n*.log\n')


def write_run_bat(root, name, binaries):
    """The same start order for Windows, where console mode needs its own window."""
    path = os.path.join(root, 'run.bat')
    start = ('for /f %%p in (\'powershell -NoProfile -Command '
             '"(Start-Process -FilePath \'%BIN%\\{}.exe\' -PassThru).Id"\') '
             'do set {}=%%p')
    lines = ['@echo off',
             'rem Starts the router and the applications in the order they need, and',
             'rem stops the ones it started once the consumer ends.',
             'setlocal',
             'set BIN=build\\bin',
             'rem The router runs in console mode, in a window of its own: --service is',
             'rem the Service Control Manager and returns at once from a command line.',
             start.format('mtrouter', 'ROUTER'),
             'rem Waits for the router to listen on 8181.',
             'for /l %%i in (1,1,20) do (',
             '    netstat -an | findstr /r /c:":8181 .*LISTENING" >nul && goto ready',
             '    timeout /t 1 /nobreak >nul',
             ')',
             ':ready',
             start.format(binaries[0], 'PROVIDER'),
             'timeout /t 1 /nobreak >nul',
             '"%BIN%\\{}.exe"'.format(binaries[1]),
             'set RC=%errorlevel%',
             'taskkill /pid %PROVIDER% /t /f >nul 2>&1',
             'taskkill /pid %ROUTER% /t /f >nul 2>&1',
             'exit /b %RC%']
    with open(path, 'w', encoding='utf-8', newline='\r\n') as handle:
        handle.write('\n'.join(lines) + '\n')


def write_run_script(root, name, binaries):
    """A start order for a multi-process project: router first, provider, consumer."""
    path = os.path.join(root, 'run.sh')
    lines = ['#!/usr/bin/env bash',
             '# Starts the router and the applications in the order they need.',
             'set -eu',
             'case "$(uname -s)" in Darwin) SFX=.mac ;; *) SFX=.elf ;; esac',
             'BIN=./build/bin',
             '# Every process this script starts in the background. The consumer runs',
             '# in the foreground and ends on its own; the router and the provider do',
             '# not, so a script that leaves either behind adds one orphan per run.',
             '# An orphan holds this script\'s stdout open, and "./run.sh | tail" then',
             '# never ends: the output looks like a hung application.',
             'STARTED=""',
             'stop() { [ -n "$STARTED" ] && kill $STARTED 2>/dev/null || true; }',
             'trap stop EXIT INT TERM',
             '# --service is the unattended mode; the console default draws a live',
             '# status display that only makes sense on a terminal.',
             '"$BIN/mtrouter$SFX" --service &',
             'STARTED="$!"',
             '# Wait for the router to accept a connection. Only one process can hold',
             '# 8181, so this also catches one that was already running.',
             'for _ in $(seq 1 100); do',
             '    (exec 3<>/dev/tcp/127.0.0.1/8181) 2>/dev/null && break',
             '    sleep 0.1',
             'done']
    lines += ['"$BIN/{}$SFX" &'.format(binaries[0]),
              'STARTED="$STARTED $!"',
              'sleep 1',
              '"$BIN/{}$SFX"'.format(binaries[1])]
    with open(path, 'w', encoding='utf-8', newline='\n') as handle:
        handle.write('\n'.join(lines) + '\n')
    os.chmod(path, os.stat(path).st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)


def main():
    parser = argparse.ArgumentParser(
        description='Create a ready-to-build AREG project.')
    parser.add_argument('--name', help='project name; a C identifier')
    parser.add_argument('--root', help='directory to create; defaults to ./<name>')
    parser.add_argument('--mode', choices=sorted(MODES), default=None,
                        help='local: one process. ipc: two processes. '
                             'pubsub: local, whose interface also declares '
                             'attributes and broadcasts.')
    parser.add_argument('--sdk-root', default=None,
                        help='use this local SDK copy instead of fetching from GitHub')
    parser.add_argument('--tag', default=DEFAULT_TAG,
                        help='SDK git tag to fetch (default: {})'.format(DEFAULT_TAG))
    parser.add_argument('--force', action='store_true',
                        help='scaffold into a directory that is not empty. Writes '
                             'the scaffolded files over any of the same name and '
                             'leaves every other file alone; nothing is deleted')
    parser.add_argument('--no-agents', action='store_true',
                        help='do not write AGENTS.md into the project')
    parser.add_argument('--harness', action='append', default=None,
                        metavar='NAME',
                        help='write only these harnesses startup files, pointing at '
                             'the project AGENTS.md. Repeatable. Default is every '
                             'one, because the agent that opens this project next is '
                             'not known here. Known: '
                             + ', '.join(sorted(HARNESS_FILES)))
    parser.add_argument('--no-harness', action='store_true',
                        help='write no harness startup file. Only an agent whose '
                             'harness reads AGENTS.md itself then finds the guide')
    parser.add_argument('--quiet', action='store_true',
                        help='do not list the harness files written')
    args = parser.parse_args()

    interactive = sys.stdin.isatty()
    name = args.name
    mode = args.mode
    root = args.root

    if name is None:
        if not interactive:
            fail('--name is required when there is no terminal to ask on')
        name = ask('Project name', 'areg_hello')
    if mode is None:
        mode = ask('Mode (local, ipc, pubsub)', 'local') if interactive else 'local'
    if mode not in MODES:
        fail('unknown mode: {}'.format(mode))
    if root is None:
        root = ask('Project directory', './' + name) if interactive else './' + name

    if not NAME_PATTERN.match(name):
        fail('project name "{}" is not a C identifier; it becomes a CMake target'.format(name))

    sdk_root = os.path.abspath(args.sdk_root) if args.sdk_root else None
    if sdk_root and not os.path.isfile(os.path.join(sdk_root, 'areg.cmake')):
        fail('no areg.cmake under --sdk-root {}'.format(sdk_root))

    check_tools(needs_git=sdk_root is None)

    root = os.path.abspath(root)
    if os.path.exists(root) and os.listdir(root) and not args.force:
        fail('{} exists and is not empty.\n'
             '  Pass --force to scaffold into it anyway. --force writes only the '
             'files this tool\n'
             '  scaffolds -- CMakeLists.txt, src/, AGENTS.md, scenarios.json, '
             'run.sh, .gitignore --\n'
             '  over any file of the same name. Every other file in the directory '
             'is left alone,\n'
             '  and nothing is deleted. Use a different --root to keep the '
             'existing files untouched.'.format(root))

    if not RECIPES:
        fail('agent documentation not found next to {}. Expected it at '
             '../docs/agent (a clone) or ../../share/areg/sdk/docs/agent '
             '(an installation).'.format(HERE))

    recipe = os.path.join(RECIPES, MODES[mode]['recipe'])
    if not os.path.isdir(recipe):
        fail('recipe not found: {}'.format(recipe))

    shutil.copytree(recipe, root, dirs_exist_ok=True)

    tokens = [(old, new.format(name=name)) for old, new in MODES[mode]['tokens']]
    for cmake in (os.path.join(root, 'CMakeLists.txt'),
                  os.path.join(root, 'src', 'CMakeLists.txt')):
        substitute(cmake, tokens)
    rewrite_top_cmake(os.path.join(root, 'CMakeLists.txt'), name, sdk_root, args.tag)

    binaries = [b.format(name=name) for b in MODES[mode]['binaries']]
    if not args.no_agents:
        write_agents(root, name, mode, sdk_root, binaries)
        harnesses = [] if args.no_harness else (args.harness or sorted(HARNESS_FILES))
        if 'all' in harnesses:
            harnesses = sorted(HARNESS_FILES)
        unknown = [h for h in harnesses if h not in HARNESS_FILES]
        if unknown:
            fail('unknown harness: {}. Known: {}'
                 .format(', '.join(unknown), ', '.join(sorted(HARNESS_FILES))))
        for relative in write_redirects(root, name, harnesses):
            if not args.quiet:
                print('  {} -> AGENTS.md'.format(relative))
    write_gitignore(root)
    write_scenarios(root, mode, binaries)
    template = gen_docs.write_template(os.path.join(root, 'design.json'))
    write_manifest(root)
    if MODES[mode]['router']:
        write_run_script(root, name, binaries)
        write_run_bat(root, name, binaries)

    tools = HERE.replace('\\', '/')
    print('created {} ({} mode)'.format(root, mode))
    print('  The sources under src/ are a placeholder: build_project.py replaces them with')
    print('  the application of your own design.json, and writes src/CMakeLists.txt.')
    if template == 'work':
        print('  design.json already carries a design, so it was left as it is.')
    else:
        print('  design.json holds every key of a design, empty: fill it, then')
    print('  python3 {}/build_project.py --spec design.json'.format(tools))
    print('    that first call compiles the framework too: give it a command timeout')
    print('    of 10 minutes (600000 ms). Every later call takes seconds.')
    print('  python3 {}/build_project.py --spec design.json --run'.format(tools))
    print('    once the bodies are filled: builds and runs every scenario in one call.')
    print_scenarios(root, mode)
    return 0


def print_scenarios(root, mode):
    """The scenarios this scaffold wrote, by name, with the processes each starts.

    A scenario is added by editing this file, and the names and process labels the
    edit has to match are in it. Printing them here is what a run otherwise spends a
    request reading back, every time, right before the edit.
    """
    try:
        with open(os.path.join(root, 'scenarios.json'), encoding='utf-8') as handle:
            scenarios = json.load(handle)['scenarios']
    except (OSError, ValueError, KeyError, TypeError):
        return
    if not scenarios:
        return
    print('  scenarios.json already carries {}, and a new one is added by editing it:'
          .format('one scenario' if len(scenarios) == 1
                  else '{} scenarios'.format(len(scenarios))))
    for scenario in scenarios:
        labels = [spec.get('name') or spec.get('binary', '?')
                  for spec in scenario.get('procs') or []]
        print('    "{}": {} -- the last one leads and ends it'
              .format(scenario.get('name', '?'), ', '.join(labels) or 'no process'))
    if mode == 'ipc':
        print('    build_project.py adds "quit" and "peer-lost" (the provider killed '
              'mid-run): write neither, and open no page for them.')
    else:
        print('    a peer going away is a scenario of its own, with "stop"; '
              'docs/agent/50-running.md has its keys.')


if __name__ == '__main__':
    sys.exit(main())
