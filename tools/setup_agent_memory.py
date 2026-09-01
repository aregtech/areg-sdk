#!/usr/bin/env python3
# ---------------------------------------------------------------------------
# This file is part of the AREG SDK.
#
# Scaffolds and checks the local session-knowledge tree under product/.
#
# Why this exists
# ---------------
# Work on this repository runs across many sessions, and each one starts by
# rediscovering what the last one already established: how to invoke the tools
# on this machine, which invariants a plausible edit breaks, why a decision was
# taken the way it was. That knowledge is real and expensive, and it is also
# local: it belongs to one machine and one line of work, it would rot inside a
# release, and an application author who found it would be misled by it.
#
# So the protocol is tracked and the content is not. This tool writes the tree
# and the two index files; what goes in them is written by hand, by whoever
# learned it. product/* is in .gitignore, so nothing this tool creates can be
# committed by accident.
#
# The rule itself is CONTRIBUTING.md section 4g.
#
# Usage
# -----
#   python tools/setup_agent_memory.py --init     # create what is missing
#   python tools/setup_agent_memory.py --check    # report, exit 1 if incomplete
#   python tools/setup_agent_memory.py --list     # show the tree and its state
#
# Existing files are never overwritten. No third-party imports, no network, and
# nothing outside product/ is written.
# ---------------------------------------------------------------------------

from __future__ import annotations

import argparse
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BASE = os.path.join('product', 'agent')

# The session bootstrap. Loaded by hand at the start of a session; it is the only
# file outside product/agent/, because it is the thing a person opens.
BOOTSTRAP = os.path.join('product', 'copilot.md')

BOOTSTRAP_TEMPLATE = """# Session bootstrap -- areg-sdk

Load this file at the start of every session in this repository. It is local, it is
never committed, and it exists so a session does not begin by rediscovering what the
last one already knew.

## What this repository work is

Two different jobs live here, and they must not be confused.

| Job | Entry point |
|---|---|
| Build an application on top of areg | `AGENTS.md` |
| Change areg-sdk itself | `CONTRIBUTING.md` |

## Read order

1. This file.
2. `product/agent/memory/memory.md` -- what previous sessions established.
3. `product/agent/lessons/lessons.md` -- what previous sessions got wrong.

Read each index, then only the file the task needs.

## Standing rules

- Never commit without explicit approval.
- `product/` is untracked working space and never enters a commit.
- `AGENTS.md` and `docs/agent/` are agent-neutral; no harness is named in them.
- The corpus is measured: run `tools/ai_score.py` before and after, record the delta.
- Every claim in the agent corpus must be true of this commit.

## Keeping this current

Update the indexes and the file an entry belongs in at the end of any session that
established a durable fact or made a mistake worth not repeating. The protocol is in
`CONTRIBUTING.md` section 4g.
"""

MEMORY_TEMPLATE = """# Memory index

Durable facts established by previous sessions. Each entry is a fact that took work
to establish and would cost the same work to re-establish.

**This is an index. Read the row that matches the task, then that one file.**

Local only. `product/` is gitignored; nothing here is ever committed.

## Files

| File | Holds | Read it when |
|---|---|---|
| _(none yet)_ | | |

## How to update

Add a fact here when it is durable and expensive to rediscover. A fact the repository
already states is not a memory: point at the repository instead.

- New fact in an existing area -- append to that file, keep it one short section.
- New area -- add a file, add a row above, keep the row's "Read it when" honest.
- Fact that stopped being true -- delete it. A stale memory is worse than none,
  because it is trusted.
"""

LESSONS_TEMPLATE = """# Lessons index

Mistakes made in previous sessions, and the rule that would have prevented each one.
A lesson earns its place by having cost real time; a general principle nobody got
wrong does not belong here.

**This is an index. Read the file before doing the kind of work it covers.**

Local only. `product/` is gitignored; nothing here is ever committed.

## Files

| File | Covers | Read it before |
|---|---|---|
| _(none yet)_ | | |

## How to update

Add a lesson when something went wrong and the fix was not obvious. Write it as:

1. **What happened** -- the concrete failure, with the file or command.
2. **Why** -- the mechanism, not the symptom.
3. **The rule** -- one imperative sentence that would have prevented it.

Delete a lesson when the repository has been changed so the mistake is no longer
possible.
"""

# Each index, and the template it is created from when absent.
INDEXES = [
    (BOOTSTRAP, BOOTSTRAP_TEMPLATE),
    (os.path.join(BASE, 'memory', 'memory.md'), MEMORY_TEMPLATE),
    (os.path.join(BASE, 'lessons', 'lessons.md'), LESSONS_TEMPLATE),
]


def entries(kind):
    """The content files of one tree, index excluded, sorted."""
    folder = os.path.join(ROOT, BASE, kind)
    if not os.path.isdir(folder):
        return []
    index = '%s.md' % kind
    return sorted(name for name in os.listdir(folder)
                  if name.endswith('.md') and name != index)


def unlisted(kind):
    """Content files the index does not name.

    An index that has fallen behind its own directory is the failure mode this
    tool exists to catch: the files are still there, and nothing points at them.
    """
    index = os.path.join(ROOT, BASE, kind, '%s.md' % kind)
    if not os.path.isfile(index):
        return entries(kind)
    with open(index, 'r', encoding='utf-8') as handle:
        text = handle.read()
    return [name for name in entries(kind) if name not in text]


def init():
    written = []
    for kind in ('memory', 'lessons'):
        folder = os.path.join(ROOT, BASE, kind)
        if not os.path.isdir(folder):
            os.makedirs(folder)
            written.append(os.path.join(BASE, kind) + os.sep)
    for relative, template in INDEXES:
        path = os.path.join(ROOT, relative)
        if os.path.isfile(path):
            continue
        folder = os.path.dirname(path)
        if not os.path.isdir(folder):
            os.makedirs(folder)
        with open(path, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write(template)
        written.append(relative)
    if written:
        for item in written:
            print('created %s' % item)
    else:
        print('nothing to do; the tree is already in place')
    return 0


def report():
    """Prints the state of the tree. Returns the number of problems found."""
    problems = 0
    for relative, _ in INDEXES:
        ok = os.path.isfile(os.path.join(ROOT, relative))
        print('%-40s %s' % (relative, 'present' if ok else 'MISSING'))
        problems += 0 if ok else 1
    for kind in ('memory', 'lessons'):
        names = entries(kind)
        print('%-40s %d file(s)' % (os.path.join(BASE, kind) + os.sep, len(names)))
        for name in names:
            print('    %s' % name)
        for name in unlisted(kind):
            print('    ^ %s is not named in %s.md' % (name, kind))
            problems += 1
    return problems


def main():
    parser = argparse.ArgumentParser(
        description='Scaffold and check the local session-knowledge tree under '
                    'product/. Content is written by hand; this tool only creates '
                    'the tree and reports on it. Nothing here is ever committed.')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--init', action='store_true',
                       help='create the tree and the index files that are missing')
    group.add_argument('--check', action='store_true',
                       help='report, and exit 1 when something is missing or unlisted')
    group.add_argument('--list', action='store_true',
                       help='show the tree and its state')
    args = parser.parse_args()

    if args.init:
        return init()

    problems = report()
    if args.check and problems:
        print('\n%d problem(s); run --init to create what is missing, or add the '
              'unlisted files to their index' % problems, file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
