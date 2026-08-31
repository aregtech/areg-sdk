#!/usr/bin/env python3
# ===========================================================================
# Creates a ready-to-build AREG project outside the SDK.
#
# The project skeleton is one of the recipes under docs/agent/recipes/, copied
# and renamed, so the code a new project starts from is the code the recipes
# already build.
#
#   python3 tools/setup_project.py --name myapp --root ~/myapp --mode local
#   python3 tools/setup_project.py --name myapp --root ~/myapp --mode ipc \
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

HERE = os.path.dirname(os.path.abspath(__file__))


def find_agent_docs():
    """Locates the directory holding the agent documentation and its recipes.

    A clone keeps it at <root>/docs/agent. An installation keeps the same tree
    under share/areg/sdk/docs/agent, next to the tools. Returns None when the
    tools were copied away from both.
    """
    candidates = [os.path.join(HERE, os.pardir, 'docs', 'agent'),
                  os.path.join(HERE, os.pardir, os.pardir,
                               'share', 'areg', 'sdk', 'docs', 'agent'),
                  os.path.join(HERE, os.pardir, os.pardir, os.pardir,
                               'share', 'areg', 'sdk', 'docs', 'agent')]
    for candidate in candidates:
        path = os.path.normpath(candidate)
        if os.path.isdir(os.path.join(path, 'recipes')):
            return path
    return None


SDK_ROOT = os.path.dirname(HERE)
AGENT_DOCS = find_agent_docs()
RECIPES = os.path.join(AGENT_DOCS, 'recipes') if AGENT_DOCS else ''
DEFAULT_TAG = 'master'

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


def rewrite_top_cmake(path, name, sdk_root, tag):
    with open(path, encoding='ascii') as handle:
        text = handle.read()

    old = ('    FetchContent_Declare(areg\n'
           '        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git\n'
           '        GIT_TAG "master")\n'
           '    FetchContent_MakeAvailable(areg)')
    if old not in text:
        fail('recipe CMakeLists.txt does not carry the expected FetchContent block')
    text = text.replace(old, fetch_block(sdk_root, tag))

    with open(path, 'w', encoding='ascii') as handle:
        handle.write(text)


def substitute(path, tokens):
    with open(path, encoding='ascii') as handle:
        text = handle.read()
    for old, new in tokens:
        text = text.replace(old, new)
    with open(path, 'w', encoding='ascii') as handle:
        handle.write(text)


def write_agents(root, name, mode, sdk_root, binaries):
    """The project's own AGENTS.md: what an agent working here loads first."""
    where = sdk_root if sdk_root else 'the SDK fetched into build/packages/areg-src'
    sdk = sdk_root if sdk_root else 'build/packages/areg-src'
    run = '\n'.join('./build/bin/{}.elf'.format(b) for b in binaries)
    if MODES[mode]['router']:
        run = './build/bin/mtrouter.elf --service &\n' + run

    text = """# {name}

An application built on the AREG framework. AREG generates the communication code
from a service contract; this project implements only the service logic.

## Build and run

```bash
cmake -B build
cmake --build build -j
{run}
```

Executables are written to `build/bin/`. The suffix is `.elf` on Linux, `.mac` on
macOS and `.exe` on Windows.

## Layout

```
src/services/*.siml   the service contract; the generator reads it at configure time
src/*.cpp             the components and the model that registers them
CMakeLists.txt        finds or fetches the AREG SDK
src/CMakeLists.txt    declares the service interface and the executables
```

## Where the framework documentation is

The SDK is at `{where}`. Read one page for the task, not the whole set:

| I need to ... | Read |
|---|---|
| **Anything ordinary** | `docs/agent/00-cheatsheet.md` - one page, most tasks end here |
| Change the service contract | `docs/agent/20-service-interface.md` |
| Implement a provider | `docs/agent/30-provider.md` |
| Implement a consumer | `docs/agent/31-consumer.md` |
| Register components and threads | `docs/agent/32-model.md` |
| Periodic or delayed work | `docs/agent/33-timers.md` |
| Log from application code | `docs/agent/34-logging.md` |
| Work out why it does not work | `docs/agent/51-debug.md` |

## Tools

```bash
python3 {sdk}/tools/gen_skeleton.py --doc src/services/X.siml --out src
python3 {sdk}/tools/run_scenarios.py
```

`gen_skeleton.py` writes the provider and consumer with every override already in
place, so only the logic has to be written. Both take `--help`.

## Never

- Never edit anything under `build/`; generated code is rewritten on every build.
- Never invent a method name on a generated base class. The naming rule is fixed.
- A consumer's `REGISTER_DEPENDENCY` string must equal the provider's role name exactly.
- Never call a request before `service_connected` reports a connected service.
- `Disconnected`, `ConnectionLost` and `Failed` are transient. Do not quit on them.
- Never block inside a handler. It stops every component in that thread.

## Done means

The build succeeds and the scenario passes:

```bash
python3 {sdk}/tools/run_scenarios.py
```

It starts the application, checks the output and the exit code, and returns 0 only
when everything matched. Edit `scenarios.json` when the expected output changes.
""".format(name=name, run=run, where=where, sdk=sdk)

    with open(os.path.join(root, 'AGENTS.md'), 'w', encoding='ascii') as handle:
        handle.write(text)


def write_scenarios(root, mode, binaries):
    """The scenario that proves the project works, for tools/run_scenarios.py."""
    expectations = MODES[mode]['expect']
    procs = []
    for index, binary in enumerate(binaries):
        spec = {'binary': binary, 'expect': expectations[index]}
        # Only the last process ends by itself; the others are stopped with it.
        if index == len(binaries) - 1:
            spec['exit'] = 0
        procs.append(spec)

    document = {'scenarios': [{'name': 'smoke',
                               'timeout': 60,
                               'router': MODES[mode]['router'],
                               'procs': procs}]}
    with open(os.path.join(root, 'scenarios.json'), 'w', encoding='ascii') as handle:
        json.dump(document, handle, indent=2)
        handle.write('\n')


def write_gitignore(root):
    with open(os.path.join(root, '.gitignore'), 'w', encoding='ascii') as handle:
        handle.write('build/\nbuild-*/\nproduct/\n*.sqlog\n*.log\n')


def write_run_script(root, name, binaries):
    """A start order for a multi-process project: router first, provider, consumer."""
    path = os.path.join(root, 'run.sh')
    lines = ['#!/usr/bin/env bash',
             '# Starts the router and the applications in the order they need.',
             'set -eu',
             'case "$(uname -s)" in Darwin) SFX=.mac ;; *) SFX=.elf ;; esac',
             'BIN=./build/bin',
             '# --service is the unattended mode; the console default draws a live',
             '# status display that only makes sense on a terminal.',
             '"$BIN/mtrouter$SFX" --service &',
             'ROUTER=$!',
             'trap "kill $ROUTER 2>/dev/null || true" EXIT',
             '# Wait for the router to accept a connection. Only one process can hold',
             '# 8181, so this also catches one that was already running.',
             'for _ in $(seq 1 100); do',
             '    (exec 3<>/dev/tcp/127.0.0.1/8181) 2>/dev/null && break',
             '    sleep 0.1',
             'done']
    lines += ['"$BIN/{}$SFX" &'.format(binaries[0]), 'sleep 1',
              '"$BIN/{}$SFX"'.format(binaries[1])]
    with open(path, 'w', encoding='ascii') as handle:
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
                        help='overwrite an existing directory')
    parser.add_argument('--no-agents', action='store_true',
                        help='do not write AGENTS.md into the project')
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
        fail('{} exists and is not empty; pass --force to overwrite'.format(root))

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
    write_gitignore(root)
    write_scenarios(root, mode, binaries)
    if MODES[mode]['router']:
        write_run_script(root, name, binaries)

    print('created {} ({} mode)'.format(root, mode))
    print('  cd {}'.format(root))
    print('  cmake -B build')
    print('  cmake --build build -j')
    if MODES[mode]['router']:
        print('  ./run.sh                 # starts the router and both applications')
    else:
        for binary in binaries:
            print('  ./build/bin/{}.elf     # .mac on macOS, .exe on Windows'.format(binary))
    return 0


if __name__ == '__main__':
    sys.exit(main())
