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
            bullets.append(textwrap.fill('- ' + as_markdown(rule), 88,
                                         subsequent_indent='  '))
    return '\n'.join(bullets) if bullets else None


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
    run = '\n'.join('./build/bin/{}.elf'.format(b) for b in binaries)
    run_win = '\n'.join(r'build\bin\{}.exe'.format(b) for b in binaries)
    if MODES[mode]['router']:
        run = './build/bin/mtrouter.elf --service &\n' + run
        run_win = 'start "" build\\bin\\mtrouter.exe --service\n' + run_win

    text = """# {name}

An application built on the AREG framework. AREG generates the communication code
from a service contract; this project implements only the service logic.

## What must be installed

CMake 3.20 or newer, a Java 17 or newer runtime (it runs the code generator), and a
C++17 or newer compiler: GCC, Clang, MSVC or MinGW. Tested on Linux and other POSIX,
macOS, Windows and Cygwin. Python 3 is not needed to build or run this project, only
for the SDK helper scripts listed below.

## Build and run

```bash
python3 {sdk}/tools/agent/build_project.py --spec design.json
{run}
```

The same on Windows, where the interpreter is `python`:

```bat
python {sdk}/tools/agent/build_project.py --spec design.json
{run_win}
```

Executables are written to `build/bin/`. The suffix is `.elf` on Linux, `.mac` on
macOS and `.exe` on Windows.

## Layout

```
src/services/         the documents; the generator reads them at configure time
src/<Name>.hpp/.cpp   one component each, named after its class
src/provider/main.cpp the model and main() of a process; two processes get a folder
                      each (provider/, consumer/), one process keeps src/ flat
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
| Implement a provider, a consumer, or the model | nothing: `gen_skeleton.py --app` wrote all three. `docs/agent/30-provider.md`, `docs/agent/31-consumer.md` and `docs/agent/32-model.md` describe a program someone else wrote: do not open them to fill a marker, and do not open them to design one either |
| Periodic or delayed work | the consumer already owns a stepping timer; for a second timer `docs/agent/33-timers.md` |
| A custom event between threads | `docs/agent/23-events.md` |
| Worker threads, watchdogs, a run-time model | `docs/agent/37-threads.md` |
| The application, components, time, files | `docs/agent/42-runtime-api.md` |
| Log from application code | `docs/agent/34-logging.md` |
| Start the pieces in the right order | nothing: `run_scenarios.py` does it, and `--app` wrote `scenarios.json`. It runs every scenario and names each one, so `--only` is for iterating on a failure, never for confirming a pass. `docs/agent/50-running.md` is its key reference, for `router`, `lead` and `stop`/`after`/`signal`, which `--app` does not write |
| Write a test | `docs/agent/52-testing.md` |
| Work out why it does not work | `docs/agent/51-debug.md` |
| **Anything this table does not cover** | `AGENTS.md` section 2 in the SDK -- it routes the full set. Never search the SDK by hand |

## Tools

Fill `design.json`, and these two are the whole path, in this order; nothing else is
needed to finish the task:

```bash
python3 {sdk}/tools/agent/build_project.py --spec design.json
python3 {sdk}/tools/agent/build_project.py --run        # rebuild, then the scenarios
```

**`build_project.py` is the build command of this project, first time and every
time.** It writes the documents from `design.json`, writes the application from them,
checks the contract, configures and builds -- five steps with no decision in any of
them -- and stops at the first failure naming the step and what to do.

**The first call compiles the framework too, so give that call a command timeout of
at least 15 minutes.** A shorter one is reported as a timeout or moved to the
background, and neither is a failure of the build. Every step is incremental, so an
interrupted call is simply run again and continues where it stopped. A second run
keeps `src/` as you have filled it in and only rebuilds; `--regenerate` writes the
application again and discards what is in it. Nothing below needs to be run by hand.

The application it writes is the whole of `src/` -- the components, every
subscription, the model, `main()` and its exit code -- and it compiles and runs as
generated. Every place your own rule belongs is one `TODO(you) <name>:` line, and
`bodies.txt` beside the project carries a section for each, naming the function it
sits in: **fill the sections and apply them all with
`python3 {sdk}/tools/agent/fill_markers.py --bodies bodies.txt`**, and never rewrite a
generated file. With a state machine in the spec the provider owns it, so there is no
host component to merge by hand.

`design.json` describes every `.dtml`, `.siml` and `.fsml` of the project, so no XML,
no `ID` and no `To` is written by hand and a type the service and its machine share is
declared once. It holds every key, empty, each section with a `#|` note: fill the
values. `gen_docs.py --example` prints a finished one for another application.
`gen_skeleton.py --doc <document> --contract` prints every name a document
generates -- the methods, and the data types the signatures are written in, including
the ones an included `.dtml` declares -- and writes no file; `--todos` lists the
markers still left. It is the answer to "what is this type called", so ask it rather
than reading a generated header.

Three more tools exist and **each answers a question you cannot already answer**.
Reaching for one before you have that question costs a turn and tells you nothing:

| Ask | Only when |
|---|---|
| `python3 {sdk}/tools/explain_rule.py <number> --at <Element>/@<Attribute>` | `gen_docs.py` refused a document and its `fix:` line was not enough |
| `python3 {sdk}/tools/schema_help.py <name> --document fsml` | you need a document to say something `design.json` has no key for. It answers one name out of the schema -- never read a `.xsd` |
| `python3 {sdk}/tools/agent/api_help.py <name>` | you need the signature of a **framework** name. Never grep the SDK for one |

All take `--help`. On Windows the interpreter is `python`, not `python3`.

## Never

Each line closes a class of wrong code, not a style preference.
`check_contract.py .` reports the ones the sources of this project can show; the
generated code under `build/` it does not read.

{never}

## Done means

The build succeeds, the contract check is clean and the scenarios pass. All of it is
inside one command:

```bash
python3 {sdk}/tools/agent/build_project.py --run
```

It rebuilds whatever changed and then runs the scenarios, so a source edit can never
be tested against the previous binary. It returns 0 only when everything matched, and
prints the line each expectation matched, so one run is the evidence. Edit
`scenarios.json` when the expected output changes.

**Every acceptance item belongs in `scenarios.json`.** The peer going away is a
scenario-level `"stop"`. The console quit path is already there: `--app` writes both
the loop in the provider's `main()` and the `quit` scenario that proves it, and that
scenario needs nothing from you. `gen_skeleton.py --app` wrote the file and printed
every key, so replacing each `TODO(you)` expectation with the line that proves a
requirement is all that is left. Never start the processes by hand with `&`, `sleep`,
`pkill` or `ps`.
""".format(name=name, run=run, run_win=run_win, where=where, sdk=sdk, never=never)

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


def write_gitignore(root):
    with open(os.path.join(root, '.gitignore'), 'w', encoding='utf-8') as handle:
        handle.write('build/\nbuild-*/\nproduct/\n*.sqlog\n*.log\n')


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
    with open(path, 'w', encoding='utf-8') as handle:
        handle.write('\n'.join(lines) + '\n')
    os.chmod(path, os.stat(path).st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)


def main():
    parser = argparse.ArgumentParser(
        description='Create a ready-to-build AREG project.')
    parser.add_argument('--name', help='project name; a C identifier')
    parser.add_argument('--root', help='directory to create; defaults to ./<name>')
    parser.add_argument('--mode', choices=sorted(MODES), default=None,
                        help='local: one process. ipc: two processes. '
                             'pubsub: attributes and broadcasts.')
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
    if MODES[mode]['router']:
        write_run_script(root, name, binaries)

    tools = HERE.replace('\\', '/')
    print('created {} ({} mode)'.format(root, mode))
    print('  The sources under src/ are a placeholder: build_project.py replaces them with')
    print('  the application of your own design.json, and writes src/CMakeLists.txt.')
    if template == 'work':
        print('  design.json already carries a design, so it was left as it is.')
    else:
        print('  design.json holds every key of a design, empty: fill it, then')
    print('  python3 {}/build_project.py --spec design.json'.format(tools))
    print('  python3 {}/run_scenarios.py'.format(tools))
    return 0


if __name__ == '__main__':
    sys.exit(main())
