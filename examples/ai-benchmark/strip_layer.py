#!/usr/bin/env python3
"""Removes the agent layer from a snapshot, for a run with --layer off.

What stays is what a plain checkout of the framework gives a developer: the sources,
the human documentation, the examples and the code generator. What goes is everything
written for a coding agent, and the two examples that are agent-built answers to the
benchmark's own tasks. The scenario runner is staged under scenario-runner/ with
framework-neutral wording, as the gRPC arm gets it.

    python3 strip_layer.py <checkout> <snapshot>

Prints the number of files removed. Exit code 1 if anything of the layer is left.
"""

import os
import re
import shutil
import sys

# Every path of the agent layer, relative to the checkout.
LAYER = ('AGENTS.md', 'CLAUDE.md', 'CODEBASE.md', 'docs/agent', 'tools/agent', 'tools/intern',
         'examples/ai-benchmark', 'docs/ai-readiness.md', 'docs/AREG_AI_CODING_RULES.md',
         'docs/wiki/09a-agentic-coding.md', '.github/copilot-instructions.md',
         '.github/instructions', '.github/workflows/agent-docs.yml')
# The examples an agent built from the benchmark's own tasks.
TASK_EXAMPLES = ('examples/33_tempalarm', 'examples/34_coffeemachine')
# The files that list the examples by name.
EXAMPLE_LISTS = ('examples/CMakeLists.txt', 'examples/README.md', 'README.md')
EXAMPLE_NAMES = re.compile(r'33_tempalarm|34_coffeemachine')
RUNNER_DIR = 'scenario-runner'


def count_files(path):
    """The number of files at a path: 1 for a file, every file below a directory."""
    if os.path.isfile(path):
        return 1
    return sum(len(files) for _, _, files in os.walk(path))


def remove(snapshot, relative):
    """Removes one file or directory of the snapshot; returns the files removed."""
    path = os.path.join(snapshot, relative)
    if not os.path.exists(path):
        return 0
    removed = count_files(path)
    if os.path.isdir(path):
        shutil.rmtree(path)
    else:
        os.remove(path)
    return removed


def scrub(snapshot, relative):
    """Deletes every line of one file that names a task example."""
    path = os.path.join(snapshot, relative)
    if not os.path.isfile(path):
        return
    with open(path, encoding='utf-8', newline='') as handle:
        lines = handle.readlines()
    with open(path, 'w', encoding='utf-8', newline='') as handle:
        handle.writelines(line for line in lines if not EXAMPLE_NAMES.search(line))


def stage_runner(checkout, snapshot):
    """Copies the scenario runner and a dialect that names no framework."""
    target = os.path.join(snapshot, RUNNER_DIR)
    os.makedirs(target, exist_ok=True)
    shutil.copy2(os.path.join(checkout, 'tools', 'agent', 'run_scenarios.py'),
                 os.path.join(target, 'run_scenarios.py'))
    source = os.path.join(checkout, 'examples', 'ai-benchmark', 'grpc-scenario-dialect.py')
    with open(source, encoding='utf-8') as handle:
        lines = [line for line in handle if not line.startswith('#')]
    text = ''.join(lines).lstrip('\n')
    text = re.sub(r"^SOURCE_SUFFIXES = .*$", "SOURCE_SUFFIXES = ('.siml', '.fsml', '.dtml')",
                  text, flags=re.M)
    with open(os.path.join(target, 'scenario_dialect.py'), 'w', encoding='utf-8',
              newline='\n') as handle:
        handle.write(text)


def leftovers(snapshot):
    """Every path of the layer, or mention of a task example, still in the snapshot."""
    found = [relative for relative in LAYER + TASK_EXAMPLES
             if os.path.exists(os.path.join(snapshot, relative))]
    for root, _, files in os.walk(snapshot):
        for name in files:
            if not name.endswith(('.md', '.txt', '.cmake')):
                continue
            path = os.path.join(root, name)
            with open(path, encoding='utf-8', errors='replace') as handle:
                if EXAMPLE_NAMES.search(handle.read()):
                    found.append(os.path.relpath(path, snapshot))
    return found


def main():
    if len(sys.argv) != 3:
        print(__doc__.strip())
        return 2
    checkout, snapshot = sys.argv[1], sys.argv[2]
    removed = sum(remove(snapshot, relative) for relative in LAYER + TASK_EXAMPLES)
    for relative in EXAMPLE_LISTS:
        scrub(snapshot, relative)
    stage_runner(checkout, snapshot)
    found = leftovers(snapshot)
    if found:
        print('strip_layer: the snapshot still carries the agent layer: ' +
              ', '.join(found[:5]))
        return 1
    print(removed)
    return 0


if __name__ == '__main__':
    sys.exit(main())
