#!/usr/bin/env python3
"""Read a benchmark run from its Claude Code transcript, request by request.

    python3 analyze_run.py <run-dir>
    python3 analyze_run.py <run-dir> --sdk <areg-sdk>

<run-dir> is what run-benchmark.sh writes: result.json, meta.txt, and the project in
work/. The SDK root is taken from --sdk, else from the "sdk" line of meta.txt, else
from this file's own location.

API requests   distinct message.id on assistant rows -- what is billed.
tool calls     distinct tool_use block ids.
context        cache_read + cache_creation + input, per request; peak reported.
fallback       a read of SDK internals: tools/agent/*.py source, conf/cmake/,
               framework/. The documentation routes around all of them, so each one
               names a page that failed.

The session id comes from result.json. When that file is missing or empty the
transcript is found from the run directory path instead, and wall time is taken
from the transcript timestamps. Prices are Sonnet 5 list prices.
"""
import argparse
import collections
import datetime
import difflib
import glob
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))

# Sonnet 5 list prices per million tokens: input, cache read, cache write (1 h), output.
PRICE_IN, PRICE_READ, PRICE_WRITE, PRICE_OUT = 2.00, 0.20, 4.00, 10.00


def rows(path):
    for line in open(path, encoding="utf-8"):
        try:
            yield json.loads(line)
        except ValueError:
            pass


def meta_of(run):
    """The key and value of every line of meta.txt."""
    found = {}
    try:
        for line in open(os.path.join(run, "meta.txt"), encoding="utf-8"):
            key, _, value = line.partition(" ")
            found[key.strip()] = value.strip()
    except OSError:
        pass
    return found


def sdk_of(run, given):
    if given:
        return os.path.abspath(given)
    return meta_of(run).get("sdk") or os.path.dirname(os.path.dirname(HERE))


def is_fallback(name, inp, sdk):
    """The kinds of SDK internals this call reads, or False."""
    blob = ""
    if name in ("Read", "Glob"):
        blob = str(inp.get("file_path") or inp.get("path") or inp.get("pattern") or "")
    elif name == "Grep":
        blob = str(inp.get("path") or "") + " " + str(inp.get("glob") or "")
    elif name == "Bash":
        cmd = str(inp.get("command") or "")
        # Only reading commands count; running a documented tool is not a fallback.
        if not re.search(r"\b(cat|head|tail|sed -n|less|grep|rg|awk|find|ls)\b", cmd):
            return False
        blob = cmd
    else:
        return False
    if sdk not in blob:
        return False
    hits = []
    if re.search(r"tools/agent/[a-z_]+\.py", blob) and "python3" not in blob:
        hits.append("tools/agent/*.py")
    if "conf/cmake" in blob:
        hits.append("conf/cmake")
    if re.search(r"(^|[\s/\"'])framework/", blob):
        hits.append("framework/")
    # The benchmark directory is the prompt's own material, never a fallback.
    if re.search(r"(^|[\s/\"'])examples/(?!ai-benchmark)", blob):
        hits.append("examples/")
    if "docs/agent/recipes" in blob:
        hits.append("recipes/")
    if re.search(r"(^|[\s/\"'])tests?/", blob):
        hits.append("tests/")
    return hits or False


def leaves_project(target, runabs):
    """True when a search target is an absolute path outside the run directory."""
    if not target.startswith("/"):
        return False
    return not os.path.abspath(target).startswith(runabs)


def read_result(run):
    """result.json, or an empty result when the run did not leave one."""
    try:
        with open(os.path.join(run, "result.json"), encoding="utf-8") as handle:
            return json.load(handle)
    except (IOError, OSError, ValueError):
        return {}


def find_transcript(run, sid):
    """The transcript of this run: by session id, else by run directory path."""
    projects = os.path.expanduser(os.path.join("~", ".claude", "projects"))
    if sid:
        hit = glob.glob(os.path.join(projects, "*", "%s.jsonl" % sid))
        if hit:
            return hit[0]
    run = os.path.abspath(run).rstrip("/")
    found = []
    for base in (os.path.join(run, "work"), run):
        slug = re.sub(r"[^A-Za-z0-9]", "-", base)
        found += glob.glob(os.path.join(projects, slug, "*.jsonl"))
    if not found:
        return None
    return max(found, key=os.path.getmtime)


def span_minutes(path):
    """Wall time from the first and last timestamped row of a transcript."""
    stamps = [e.get("timestamp") for e in rows(path) if e.get("timestamp")]
    if len(stamps) < 2:
        return 0.0
    fmt = "%Y-%m-%dT%H:%M:%S"
    try:
        first = datetime.datetime.strptime(min(stamps)[:19], fmt)
        last = datetime.datetime.strptime(max(stamps)[:19], fmt)
    except ValueError:
        return 0.0
    return (last - first).total_seconds() / 60.0


CLASS_DEF = re.compile(r"^(?:class|struct)\s+(\w+)(?:\s+final)?\s*(?::[^;]*)?$", re.M)


def layout_findings(sources):
    """One class per file named after it, none beside main(), and a folder each with
    its own main.cpp for two processes."""
    found = []
    mains = sorted(n for n in sources if os.path.basename(n) == "main.cpp"
                   or "int main(" in sources[n])
    if len(mains) > 1:
        flat = [n for n in mains if os.path.dirname(n) == ""]
        if flat:
            found.append("%s: two processes, and %s is not in a folder of its own"
                         % (", ".join(mains), ", ".join(flat)))
        elif len(set(os.path.basename(n) for n in mains)) > 1:
            found.append("each process has a folder but not a main.cpp: %s"
                         % ", ".join(mains))
    for name, text in sorted(sources.items()):
        classes = CLASS_DEF.findall(text)
        if "int main(" in text and classes:
            found.append("%s defines %s beside main()" % (name, ", ".join(classes)))
        elif len(classes) > 1:
            found.append("%s defines %d classes: %s" % (name, len(classes), ", ".join(classes)))
        elif classes and os.path.splitext(os.path.basename(name))[0] != classes[0]:
            found.append("%s holds class %s" % (name, classes[0]))
    return found


def hand_written(src, sources, sdk):
    """Lines of src/ that a regeneration of the run's own documents does not produce."""
    services = os.path.join(src, "services")
    siml = sorted(glob.glob(os.path.join(services, "*.siml")))
    if not siml:
        return None
    fsml = sorted(glob.glob(os.path.join(services, "*.fsml")))
    mode = "local" if "main.cpp" in sources else "ipc"
    tmp = tempfile.mkdtemp()
    try:
        shutil.copytree(services, os.path.join(tmp, "src", "services"))
        cmd = [sys.executable, os.path.join(sdk, "tools", "agent", "gen_skeleton.py"),
               "--doc", "src/services/" + os.path.basename(siml[0]), "--app",
               "--mode", mode, "--force", "--scenarios", os.path.join(tmp, "none.json")]
        if fsml:
            cmd += ["--machine", "src/services/" + os.path.basename(fsml[0])]
        if subprocess.run(cmd, cwd=tmp, capture_output=True).returncode:
            return None
        generated = {}
        base = os.path.join(tmp, "src")
        for root, dirs, names in os.walk(base):
            dirs[:] = [d for d in dirs if d not in ("build", "services")]
            for n in names:
                if n.endswith((".hpp", ".cpp")):
                    key = os.path.relpath(os.path.join(root, n), base)
                    generated[key] = open(os.path.join(root, n), errors="ignore").read()
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
    written = 0
    for name, text in sources.items():
        before = generated.get(name, "").splitlines()
        for op, _, _, j1, j2 in difflib.SequenceMatcher(None, before, text.splitlines(),
                                                        autojunk=False).get_opcodes():
            if op in ("replace", "insert"):
                written += j2 - j1
    return written


# A run's price is a base that barely moves plus a handful of discrete events, and
# each event is a prediction failure or a prompt artefact. Counting them is what lets
# two runs settle a change.
BUILD_CALL = re.compile(r"build_project\.py|cmake\s+--build|\bmake\b")
SCEN_CALL = re.compile(r"run_scenarios\.py")
COUNTING = re.compile(r"\bwc\b|\bdu\b|--stat\b|\bcloc\b|stat\s+-c|\bfind\b.*-name.*\|",
                      re.I)
# A counting command measures the run only when it targets the run's own source.
COUNTED_SOURCE = re.compile(r"(^|[\s\"'])(src|scenarios\.json|design\.json|\*\.(cpp|hpp))")


def events(requests):
    """The discrete things that happen to a run, with the request each began at.

    A build is a fix cycle only when a source changed since the previous build; a
    build that follows no edit is a confirmation.
    """
    spec = None
    for r in requests:
        for nm, what in r["calls"]:
            if nm == "Bash" and spec is None:
                hit = re.search(r"--spec\s+(\S+)", what)
                if hit:
                    spec = os.path.basename(hit.group(1).strip("'\""))
    builds, scenarios, respecs, counters = [], [], [], []
    build_fixes, scen_fixes = [], []
    edited_since_build = edited_since_scen = False
    last_change = -1
    for i, r in enumerate(requests):
        for nm, what in r["calls"]:
            if nm in ("Edit", "Write", "NotebookEdit"):
                base = os.path.basename(what)
                if base.endswith((".cpp", ".hpp", ".h", ".json", ".txt", ".cmake")):
                    edited_since_build = edited_since_scen = True
                    last_change = i
                if spec and base == spec and builds:
                    respecs.append(i)
                continue
            if nm != "Bash":
                continue
            if BUILD_CALL.search(what):
                if builds and edited_since_build:
                    build_fixes.append(i)
                builds.append(i)
                edited_since_build = False
                last_change = max(last_change, i)
            if SCEN_CALL.search(what):
                if scenarios and edited_since_scen:
                    scen_fixes.append(i)
                scenarios.append(i)
                edited_since_scen = False
                last_change = max(last_change, i)
            if COUNTING.search(what) and COUNTED_SOURCE.search(what):
                counters.append(i)
    return [("build invocations", builds, ""),
            ("build-and-fix cycles", build_fixes, "a source changed, then it rebuilt"),
            ("scenario runs", scenarios, ""),
            ("run-and-fix cycles", scen_fixes, "a source changed, then it ran again"),
            ("spec re-edits after a build", respecs,
             "the design was changed after it had been generated"),
            ("self-measuring calls", counters, "wc, du, cloc: the run counting itself"),
            ("requests after the last change", list(range(last_change + 1, len(requests))),
             "the self-reporting tail")]


def print_events(requests, per_request):
    print("\n== events   (the base barely moves; these are what move the bill)")
    for name, hits, note in events(requests):
        cost = sum(per_request[i] for i in hits if i < len(per_request))
        where = ", ".join("r%d" % i for i in hits[:8]) + (" ..." if len(hits) > 8 else "")
        print("   %-30s %2d  $%.3f  %s" % (name, len(hits), cost, where or "-"))
        if hits and note:
            print("   %-30s     %s" % ("", note))


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("run", nargs="?", default=".", help="the run directory")
    parser.add_argument("--sdk", help="the SDK the run read (default: meta.txt, then here)")
    args = parser.parse_args()
    run = args.run
    sdk = sdk_of(run, args.sdk).rstrip("/")
    meta = meta_of(run)

    result = read_result(run)
    sid = result.get("session_id")
    tr = find_transcript(run, sid)
    if not tr:
        sys.exit("no transcript for %s" % (sid or run))
    if not sid:
        sid = os.path.basename(tr)[:-6] + "  (result.json missing; found by path)"

    runabs = os.path.abspath(run).rstrip("/")
    seen_msg, seen_block = set(), set()
    requests = []
    tools = collections.Counter()
    fallbacks = []
    searches = []
    tot = collections.Counter()
    visible = 0

    for e in rows(tr):
        m = e.get("message")
        if not isinstance(m, dict) or e.get("type") != "assistant":
            continue
        mid = m.get("id")
        u = m.get("usage") or {}
        names = []
        calls = []
        for b in m.get("content") or []:
            if not isinstance(b, dict):
                continue
            if b.get("type") == "text":
                visible += len(b.get("text") or "")
            if b.get("type") != "tool_use":
                continue
            bid = b.get("id")
            if bid in seen_block:
                continue
            seen_block.add(bid)
            nm = b.get("name", "?")
            inp = b.get("input") or {}
            tools[nm] += 1
            visible += len(json.dumps(inp))
            names.append(nm)
            calls.append((nm, str(inp.get("command") or inp.get("file_path")
                                  or inp.get("pattern") or "")))
            if nm in ("Bash", "Glob"):
                cmd = str(inp.get("command") or inp.get("pattern") or "")
                hunt = [t for t in re.findall(r"\b(?:find|locate)\s+(\S+)", cmd)
                        if leaves_project(t, runabs)]
                wide = re.findall(r"\bls\s+-[A-Za-z]*R\s+(\S+)", cmd)
                if hunt or [t for t in wide if leaves_project(t, runabs)]:
                    searches.append((len(requests), " ".join(cmd.split())[:110]))
            fb = is_fallback(nm, inp, sdk)
            if fb:
                what = inp.get("file_path") or inp.get("pattern") or inp.get("command") or ""
                fallbacks.append((len(requests), nm, ",".join(fb), str(what)[:110]))
        if mid and mid not in seen_msg:
            seen_msg.add(mid)
            ctx = (u.get("cache_read_input_tokens") or 0) + \
                  (u.get("cache_creation_input_tokens") or 0) + (u.get("input_tokens") or 0)
            for k in ("input_tokens", "output_tokens", "cache_read_input_tokens",
                      "cache_creation_input_tokens"):
                tot[k] += u.get(k) or 0
            requests.append({"id": mid, "ctx": ctx, "out": u.get("output_tokens") or 0,
                             "tools": names, "calls": calls,
                             "cr": u.get("cache_read_input_tokens") or 0,
                             "cw": u.get("cache_creation_input_tokens") or 0})
        elif mid:
            for r in requests:
                if r["id"] == mid:
                    r["tools"].extend(names)
                    r["calls"].extend(calls)
                    break
    if not requests:
        sys.exit("no assistant request in %s" % tr)

    fresh = tot["input_tokens"] + tot["output_tokens"]
    cost = (tot["input_tokens"] * PRICE_IN + tot["cache_read_input_tokens"] * PRICE_READ +
            tot["cache_creation_input_tokens"] * PRICE_WRITE +
            tot["output_tokens"] * PRICE_OUT) / 1e6
    print("== %s" % run)
    print("   %-26s %s" % ("session", sid))
    print("   %-26s %s" % ("sdk read", sdk))
    if meta.get("model") and meta.get("model") != "sonnet":
        print("   %-26s %s  (prices below are Sonnet 5's)" % ("model", meta.get("model")))
    print("   %-26s %d" % ("API requests", len(requests)))
    print("   %-26s %d" % ("tool calls", sum(tools.values())))
    print("   %-26s %s" % ("fresh tokens (in+out)", format(fresh, ",")))
    print("   %-26s %s" % ("  output", format(tot["output_tokens"], ",")))
    vis = visible // 4
    print("   %-26s ~%s   (text + tool JSON, chars/4)" % ("  visible output", format(vis, ",")))
    print("   %-26s ~%s   (output - visible)" % ("  reasoning", format(tot["output_tokens"] - vis, ",")))
    print("   %-26s %s" % ("  uncached input", format(tot["input_tokens"], ",")))
    print("   %-26s %s" % ("cache reads", format(tot["cache_read_input_tokens"], ",")))
    print("   %-26s %s" % ("cache writes", format(tot["cache_creation_input_tokens"], ",")))
    print("   %-26s %s" % ("peak context", format(max(r["ctx"] for r in requests), ",")))
    print("   %-26s $%.4f  (harness $%.4f)" % ("cost, recomputed", cost,
                                               result.get("total_cost_usd") or 0))
    wall = (result.get("duration_ms") or 0) / 60000.0
    print("   %-26s %.1f min%s" % ("wall", wall or span_minutes(tr),
                                   "" if wall else "   (from transcript timestamps)"))
    print("   %-26s %s" % ("permission denials", result.get("permission_denials")))
    print("   %-26s $%.4f" % ("cost per request", cost / len(requests)))
    print("\n== tool calls by name")
    for n, c in tools.most_common():
        print("   %-26s %d" % (n, c))
    per_request = [(r["cr"] * PRICE_READ + r["cw"] * PRICE_WRITE + r["out"] * PRICE_OUT) / 1e6
                   for r in requests]
    print_events(requests, per_request)
    print("\n== fallbacks to SDK internals: %d" % len(fallbacks))
    for i, nm, kind, what in fallbacks:
        print("   req %3d  %-6s %-16s %s" % (i, nm, kind, what))
    print("\n== filesystem searches: %d   (each one is a path the corpus failed to resolve)"
          % len(searches))
    for i, what in searches:
        print("   req %3d  %s" % (i, what))

    print("\n== the report rows, so the run does not have to count itself")
    opened, seen_path = [], set()
    for x in rows(tr):
        if x.get("type") != "assistant":
            continue
        for b in (x.get("message") or {}).get("content") or []:
            if not isinstance(b, dict) or b.get("type") != "tool_use":
                continue
            f = (b.get("input") or {}).get("file_path")
            if f and f.startswith(sdk + "/") and f not in seen_path:
                seen_path.add(f)
                opened.append(f)
    total = 0
    for f in opened:
        n = os.path.getsize(f) if os.path.exists(f) else 0
        total += n
        print("   %8d  %s" % (n, f[len(sdk) + 1:]))
    print("   %8d  total, %d file(s) opened inside the SDK" % (total, len(opened)))
    src = os.path.join(run, "work", "src")
    sources = {}
    for root, dirs, names in os.walk(src):
        dirs[:] = [d for d in dirs if d not in ("build", "services")]
        for n in names:
            if n.endswith((".hpp", ".cpp")):
                path = os.path.join(root, n)
                sources[os.path.relpath(path, src)] = open(path, errors="ignore").read()
    written = hand_written(src, sources, sdk)
    print("   %-26s %d" % ("C++ lines in src/", sum(len(t.splitlines()) for t in sources.values())))
    print("   %-26s %s" % ("  hand-written",
                             "%d  (not in a regeneration of the same documents)" % written
                             if written is not None else "unknown (regeneration failed)"))
    layout = layout_findings(sources)
    for finding in layout:
        print("   %-26s %s" % ("LAYOUT", finding))
    if not layout:
        print("   %-26s %s" % ("layout", "one class per file, no class beside main()"))

    spikes = sorted(enumerate(requests), key=lambda p: -p[1]["out"])[:2]
    print("\n== the reasoning spikes (the design and the implementation thought)")
    for i, r in sorted(spikes, key=lambda p: p[0]):
        # A token written at r is billed as output, then read again by every later request.
        price = r["out"] * (PRICE_OUT + PRICE_WRITE +
                            PRICE_READ * max(len(requests) - i - 1, 0)) / 1e6
        print("   r%-4d %8s output  %8s ctx  $%.3f fully priced"
              % (i, format(r["out"], ","), format(r["ctx"], ","), price))

    print("\n== per-request timeline (idx  ctx  out  tools)")
    for i, r in enumerate(requests):
        print("   %3d  %8s %6s  %s" % (i, format(r["ctx"], ","), format(r["out"], ","),
                                       " ".join(r["tools"]) or "-"))


if __name__ == "__main__":
    main()
