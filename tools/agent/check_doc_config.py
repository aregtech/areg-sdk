#!/usr/bin/env python3
# ===========================================================================
# Runs the configuration block the documentation gives, and asserts it does
# what the documentation says it does.
#
#   python3 tools/agent/check_doc_config.py --lib build/bin
#
# Every other checker verifies code. This one verifies an *instruction*: the
# `ini` block in docs/agent/00-cheatsheet.md is lifted out of the page as it
# stands, dropped into a recipe that logs, and the recipe is run. If the block
# produces no log line, the page is wrong, and it is wrong on the fast path -
# the one an agent is told is "the whole reading list".
#
# This exists because a configuration block cannot be checked by reading it.
# The block shipped for a long time missing the key that turns output on; it
# compiled nothing, broke nothing, and every static check stayed green.
#
# No agent takes part.
#
# Exit code 0 when the documented block logs what it promises, 1 otherwise.
# ===========================================================================
import argparse
import glob
import os
import re
import shutil
import subprocess
import sys
import tempfile

SDK = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
PAGE = os.path.join(SDK, 'docs', 'agent', '00-cheatsheet.md')
# The recipe the block is tried on: it declares scopes and logs from two threads.
RECIPE = os.path.join(SDK, 'docs', 'agent', 'recipes', '07-worker-events')
# The page writes the application's own name as this; a real file names the module.
PLACEHOLDER = 'myapp'
# What the recipe logs. Finding it proves the block reached the application.
WANTED = 'scan_'


def logging_block(text):
    """The ini block of the page that configures logging, verbatim."""
    for block in re.findall(r'```ini\n(.*?)```', text, re.S):
        if 'log::' in block and 'scope::' in block:
            return block
    return None


def main():
    parser = argparse.ArgumentParser(
        description='Run the configuration block the documentation gives.')
    parser.add_argument('--lib', default=os.path.join('build', 'bin'),
                        help='directory holding libareg')
    parser.add_argument('--compiler', default=os.environ.get('CXX', 'g++'))
    args = parser.parse_args()

    lib = os.path.abspath(args.lib)
    if not glob.glob(os.path.join(lib, 'libareg*')):
        sys.stderr.write('error: no libareg under {}\n'.format(lib))
        return 1

    with open(PAGE, encoding='utf-8') as handle:
        block = logging_block(handle.read())
    if block is None:
        print('FAIL  docs/agent/00-cheatsheet.md carries no logging block to run')
        return 1
    if PLACEHOLDER not in block:
        print('FAIL  the block no longer writes the application name as "{}", so this '
              'check cannot aim it at the recipe'.format(PLACEHOLDER))
        return 1

    sys.path.insert(0, os.path.join(SDK, 'tools', 'agent'))
    import check_recipes

    work = tempfile.mkdtemp(prefix='areg-docconfig-')
    try:
        root = os.path.join(work, '07-worker-events')
        shutil.copytree(RECIPE, root)
        # The recipe compiles to "main", so the module the block names becomes that.
        config = os.path.join(root, 'config', 'areg.init')
        with open(config, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write(block.replace(PLACEHOLDER, 'main'))

        ok, binaries, detail = check_recipes.build_recipe(root, lib, args.compiler)
        if not ok:
            print('FAIL  the recipe did not build: ' + detail)
            return 1

        try:
            result = subprocess.run([binaries[0]], cwd=root, capture_output=True,
                                    text=True, timeout=60)
        except subprocess.TimeoutExpired:
            print('FAIL  the application did not finish')
            return 1
        if result.returncode != 0:
            print('FAIL  the application exited {}'.format(result.returncode))
            return 1

        console = result.stdout + result.stderr
        written = []
        for found in glob.glob(os.path.join(root, '**', '*.log'), recursive=True):
            with open(found, encoding='utf-8', errors='replace') as handle:
                written.append(handle.read())

        if not written and WANTED not in console:
            print('FAIL  the documented block produced no output at all: no log file '
                  'was written and the console carries none of the scopes. The block '
                  'in docs/agent/00-cheatsheet.md does not do what the page says.')
            return 1
        if not any(WANTED in text for text in written) and WANTED not in console:
            print('FAIL  output was produced but none of it is the application\'s own '
                  'scopes ({}*), so the scope keys in the block select nothing'
                  .format(WANTED))
            return 1

        lines = sum(text.count('\n') for text in written)
        print('PASS  00-cheatsheet.md   the documented block wrote {} log lines, '
              'including the application\'s own scopes'.format(lines))
        return 0
    finally:
        shutil.rmtree(work, ignore_errors=True)


if __name__ == '__main__':
    sys.exit(main())
