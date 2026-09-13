#!/usr/bin/env python3
"""Read the numbers off a headless prompt run.

    claude -p --output-format json ... > result.json
    python3 measure.py result.json
    python3 measure.py result.json --project .     # also count the sources written

Prints cost, turns, wall time and token usage from the harness, then counts the
tool calls from the session transcript. num_turns is assistant turns, not tool
calls; the two are not interchangeable and only the second is comparable across
runs. Needs nothing but python3 -- jq is not assumed.

A file that is not a Claude result.json is read as another agent's usage file.
GitHub Copilot's --usage-output-file is mapped to the same rows a Claude run is read
on; any other shape has every number it holds printed under the key the file itself
used, so nothing is guessed and nothing is silently wrong.

**Keep billing units explicit.** Claude reports USD; Copilot reports AI credits
(AIC), or premium requests on legacy billing. Neither Copilot unit is converted
to dollars by this tool.
"""

import collections
from decimal import Decimal
import glob
import json
import os
import sys

TARGETS = {"tempalarm": (50000, 20), "coffeemachine": (75000, 30)}
SPELLINGS = {"tempalarm": ("temperature-alarm", "temperaturealarm"),
             "coffeemachine": ("coffee-machine",)}

# Source, in any framework. A layout is the implementer's choice: areg writes
# src/provider and src/consumer, a gRPC arm writes machine/ client/ proto/, and a
# counter that knows only src/ reports 0 for the second and invites the wrong
# conclusion.
SOURCE_SUFFIXES = ('.cpp', '.hpp', '.h', '.cc', '.cxx', '.hxx', '.proto')

# Not the project's own source: build output, the tree a package manager fetched,
# and anything a generator rewrites on every build.
NOT_SOURCE = ('build', 'generate', 'generated', 'packages', '.git', 'out',
              'cmake-build-debug', 'cmake-build-release', '__pycache__')


def count_sources(root):
    """Every source file the project carries, wherever the implementer put it."""
    files = 0
    lines = 0
    per_suffix = collections.Counter()
    for base, folders, names in os.walk(root):
        folders[:] = [f for f in folders if f not in NOT_SOURCE and not f.startswith('.')]
        for name in names:
            if not name.endswith(SOURCE_SUFFIXES):
                continue
            path = os.path.join(base, name)
            try:
                with open(path, encoding='utf-8', errors='ignore') as handle:
                    lines += sum(1 for _ in handle)
            except OSError:
                continue
            files += 1
            per_suffix[os.path.splitext(name)[1]] += 1
    return files, lines, per_suffix


def numbers_in(blob, prefix=''):
    """Every number a JSON document holds, by the path the document gave it."""
    found = []
    if isinstance(blob, dict):
        for key in blob:
            found += numbers_in(blob[key], prefix + ('.' if prefix else '') + str(key))
    elif isinstance(blob, list):
        for index, item in enumerate(blob):
            found += numbers_in(item, '%s[%d]' % (prefix, index))
    elif isinstance(blob, (int, float)) and not isinstance(blob, bool):
        found.append((prefix, blob))
    return found


def copilot_usage(result):
    """Copilot's --usage-output-file, or None when the file is some other shape."""
    if not isinstance(result, dict):
        return None
    if not any(key in result for key in ("modelMetrics", "totalPremiumRequestCost",
                                         "totalNanoAiu")):
        return None
    return result


def report_copilot(result):
    """Copilot's usage file, on the rows a run is actually compared on.

    The top-level totalNanoAiu is the session total in billionths of an AI credit.
    Nested model/agent totals are breakdowns, not additional charges.

    TO CHECK, then either state the rate here or delete this note: GitHub prices an
    AI credit at USD 0.01 for overage, so totalNanoAiu / 1e11 would be the dollar
    figure. Nothing here converts, because the rate is a billing term rather than
    something the usage file states, and a printed number nobody verified is worse
    than a blank column. Verify against an invoice before using it to compare arms.
    """
    nano_aiu = result.get("totalNanoAiu")
    if nano_aiu is not None and (not isinstance(nano_aiu, int)
                                 or isinstance(nano_aiu, bool) or nano_aiu < 0):
        print("measure: totalNanoAiu must be a non-negative integer", file=sys.stderr)
        return 1
    models = result.get("modelMetrics") or {}
    print("== harness (GitHub Copilot)")
    print("   %-28s %s" % ("model(s)", ", ".join(sorted(models)) or
                           result.get("currentModel", "?")))
    if nano_aiu is not None:
        credits = format(Decimal(nano_aiu) / Decimal(1000000000), ",.9f").rstrip("0").rstrip(".")
        print("   %-28s %s" % ("AI credits (AIC)", credits))
    else:
        print("   %-28s %s" % ("AI credits (AIC)", "--   not reported in this usage file"))
    print("   %-28s %s" % ("cost (USD)", "--   not reported; no conversion from AIC or premium requests"))
    if "totalPremiumRequestCost" in result:
        print("   %-28s %s" % ("premium requests (legacy)",
                               result["totalPremiumRequestCost"]))
    print("   %-28s %s" % ("user prompts", result.get("totalUserRequests", "?")))
    duration = result.get("totalApiDurationMs")
    if isinstance(duration, (int, float)):
        print("   %-28s %s   (API time, not wall time)" % ("api time (s)", duration / 1000.0))

    fields = ("inputTokens", "outputTokens", "cacheReadTokens",
              "cacheWriteTokens", "reasoningTokens")
    totals = dict.fromkeys(fields, 0)
    requests = 0
    for entry in models.values():
        usage = entry.get("usage") or {}
        for field in fields:
            if isinstance(usage.get(field), int):
                totals[field] += usage[field]
        count = (entry.get("requests") or {}).get("count")
        if isinstance(count, int):
            requests += count
    print("   %-28s %d" % ("API requests", requests))
    fresh = totals["inputTokens"] + totals["outputTokens"]
    print("   %-28s %s   (uncached in + out)" % ("tokens, fresh", format(fresh, ",")))
    for field in fields:
        print("   %-28s %s" % (field, format(totals[field], ",")))
    print("   Reasoning is reported here directly; a Claude run has to infer it as")
    print("   output minus visible text.")

    changes = result.get("codeChanges") or {}
    if changes:
        print("   %-28s +%s -%s in %s file(s)"
              % ("code changes", changes.get("linesAdded", "?"),
                 changes.get("linesRemoved", "?"), changes.get("filesModifiedCount", "?")))

    agents = result.get("agentMetrics") or {}
    if len(agents) > 1:
        print("   %-28s %s" % ("agents", ", ".join(sorted(agents))))
        print("   More than one agent ran. Subagent work is in these rows too.")
    return 0


def report_foreign(path, result):
    """A usage file this tool does not have a schema for, read honestly.

    Printing every number under the key its own file used is the only way to read a
    format that may change without notice. A guessed mapping that silently goes
    stale is worse than no mapping.
    """
    print("== %s -- not a Claude result.json, read as a usage file" % os.path.basename(path))
    found = numbers_in(result)
    if not found:
        print("   no numbers in it. Pass the file the agent was told to write.")
        return 1
    for key, value in found:
        shown = format(value, ",") if isinstance(value, int) else "%.6f" % value
        print("   %-40s %s" % (key, shown))
    print("")
    print("   Read the cost and the token counts off the rows above by their own")
    print("   names. Compare with a Claude run only where the two measure the same")
    print("   thing: total cost, and tokens in and out.")
    return 0


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


def report_sources(root):
    """What the run actually wrote, counted the same way for every framework."""
    if not os.path.isdir(root):
        print("== sources\n   no such directory: %s" % root)
        return
    files, lines, per_suffix = count_sources(root)
    print("== sources under %s" % os.path.abspath(root))
    print("   %-28s %d" % ("files", files))
    print("   %-28s %s" % ("lines", format(lines, ",")))
    if per_suffix:
        print("   %-28s %s" % ("by kind", ", ".join(
            "%s %d" % (suffix, n) for suffix, n in sorted(per_suffix.items()))))
    print("   Build output and fetched packages are not counted. Generated sources")
    print("   are not either, so this is what the run wrote by hand.")


def main():
    args = [a for a in sys.argv[1:]]
    project = None
    if "--project" in args:
        index = args.index("--project")
        if index + 1 >= len(args):
            sys.exit("--project needs a directory")
        project = args[index + 1]
        del args[index:index + 2]
    if len(args) != 1:
        sys.exit("usage: measure.py <result.json> [--project <dir>]")

    result = load(args[0])
    if not isinstance(result, dict) or "session_id" not in result:
        if copilot_usage(result) is not None:
            code = report_copilot(result)
        else:
            code = report_foreign(args[0], result)
        if project:
            print("")
            report_sources(project)
        return code
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

    if project:
        print("")
        report_sources(project)

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

    # Match on the task key or one of its older spellings.
    haystack = " ".join((sys.argv[1], os.getcwd(),
                         str(result.get("result", ""))[:4000])).lower()
    task = ""
    for name in TARGETS:
        if any(n in haystack for n in (name,) + SPELLINGS.get(name, ())):
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
        print("\n(no budget matched; targets are 50K/20 for tempalarm, "
              "75K/30 for coffeemachine)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
