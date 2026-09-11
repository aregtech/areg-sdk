#!/usr/bin/env python3
"""Read the numbers off a headless prompt run.

    claude -p --output-format json ... > result.json
    python3 measure.py result.json

Prints cost, turns, wall time and token usage from the harness, then counts the
tool calls from the session transcript. num_turns is assistant turns, not tool
calls; the two are not interchangeable and only the second is comparable across
runs. Needs nothing but python3 -- jq is not assumed.
"""

import collections
import glob
import json
import os
import sys

TARGETS = {"temperature-alarm": (50000, 20), "coffee-machine": (75000, 30)}


def load(path):
    with open(path, encoding="utf-8") as handle:
        return json.load(handle)


def transcript_of(session_id):
    """The session .jsonl, wherever the harness put it."""
    if not session_id:
        return None
    pattern = os.path.expanduser("~/.claude/projects/*/%s.jsonl" % session_id)
    found = glob.glob(pattern)
    return found[0] if found else None


def count_tool_calls(path):
    counts = collections.Counter()
    for line in open(path, encoding="utf-8"):
        try:
            entry = json.loads(line)
        except ValueError:
            continue
        message = entry.get("message")
        if not isinstance(message, dict):
            continue
        for block in message.get("content") or []:
            if isinstance(block, dict) and block.get("type") == "tool_use":
                counts[block.get("name", "?")] += 1
    return counts


def tokens_of(usage):
    """Every token the run was billed for, cached reads included."""
    if not isinstance(usage, dict):
        return None
    keys = ("input_tokens", "output_tokens",
            "cache_creation_input_tokens", "cache_read_input_tokens")
    values = [usage.get(k) for k in keys if isinstance(usage.get(k), int)]
    return sum(values) if values else None


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: measure.py <result.json>")

    result = load(sys.argv[1])
    usage = result.get("usage") or {}
    total = tokens_of(usage)
    fresh = None
    if isinstance(usage.get("input_tokens"), int):
        fresh = usage["input_tokens"] + (usage.get("output_tokens") or 0)

    print("== harness")
    for label, value in (("cost (USD)", result.get("total_cost_usd")),
                         ("assistant turns", result.get("num_turns")),
                         ("wall time (s)", (result.get("duration_ms") or 0) / 1000.0),
                         ("is_error", result.get("is_error"))):
        print("   %-28s %s" % (label, value))
    if total is not None:
        print("   %-28s %s" % ("tokens, all", format(total, ",")))
    if fresh is not None:
        print("   %-28s %s   (uncached in + out)" % ("tokens, fresh", format(fresh, ",")))
    for key in sorted(usage):
        if isinstance(usage[key], int):
            print("   %-28s %s" % (key, format(usage[key], ",")))

    print("\n== tool calls")
    path = transcript_of(result.get("session_id"))
    calls = 0
    if path is None:
        print("   transcript not found for session %s" % result.get("session_id"))
        print("   (count them by hand, or the comparison is invalid)")
    else:
        counts = count_tool_calls(path)
        calls = sum(counts.values())
        print("   %-28s %d" % ("total", calls))
        for name, n in counts.most_common():
            print("   %-28s %d" % (name, n))

    # Match on the task name with and without its hyphen: a run directory is as
    # likely to be called coffeemachine as coffee-machine.
    haystack = " ".join((sys.argv[1], os.getcwd(),
                         str(result.get("result", ""))[:4000])).lower()
    task = ""
    for name in TARGETS:
        if name in haystack or name.replace("-", "") in haystack:
            task = name
    if task:
        budget, turns = TARGETS[task]
        print("\n== against the %s budget" % task)
        if fresh is not None:
            verdict = "PASS" if fresh <= budget else "OVER by %s" % format(fresh - budget, ",")
            print("   %-28s %s / %s   %s" % ("fresh tokens", format(fresh, ","),
                                             format(budget, ","), verdict))
        if calls:
            verdict = "PASS" if calls <= turns else "OVER by %d" % (calls - turns)
            print("   %-28s %d / %d   %s" % ("tool calls", calls, turns, verdict))
    else:
        print("\n(no budget matched; targets are 50K/20 for temperature-alarm, "
              "75K/30 for coffee-machine)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
