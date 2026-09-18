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

# Sonnet 5 list prices per million tokens. A cache write costs 2.50 at the 5-minute
# TTL and 4.00 at the 1-hour one, and the harness picks the TTL, so two runs at
# different tiers are 60% apart on that line for a reason no tree change explains.
PRICE_IN, PRICE_READ, PRICE_OUT = 2.00, 0.20, 10.00
PRICE_W5, PRICE_W1 = 2.50, 4.00


def own_cost(r):
    """What one request was billed: its reads, its writes at their own tier, its output."""
    return (r["in"] * PRICE_IN + r["cr"] * PRICE_READ + r["w5"] * PRICE_W5 +
            r["w1"] * PRICE_W1 + r["out"] * PRICE_OUT) / 1e6


def write_price(r):
    """The per-token cache-write price this request paid, the 1-hour one if it wrote nothing."""
    return (r["w5"] * PRICE_W5 + r["w1"] * PRICE_W1) / r["cw"] if r["cw"] else PRICE_W1


def rows(path):
    for line in open(path, encoding="utf-8"):
        try:
            yield json.loads(line)
        except ValueError:
            pass


def tool_results(path):
    """What each tool call printed, keyed by the call's block id.

    A tool_result arrives in a later row than the tool_use it answers, so the
    transcript is read once for them before the requests are walked.
    """
    found = {}
    for e in rows(path):
        m = e.get("message")
        if not isinstance(m, dict) or e.get("type") != "user":
            continue
        for b in m.get("content") or []:
            if not isinstance(b, dict) or b.get("type") != "tool_result":
                continue
            c = b.get("content")
            if isinstance(c, list):
                c = " ".join(x.get("text") or "" for x in c if isinstance(x, dict))
            found[b.get("tool_use_id")] = c if isinstance(c, str) else ""
    return found


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


def run_facts(result, requests, tot):
    """Why the run stopped, whether it began on a warm cache, and the TTL it wrote at.

    Two runs compare as prices only when all three agree.
    """
    stop = result.get("terminal_reason") or "unknown"
    if result.get("api_error_status"):
        stop += " %s" % result.get("api_error_status")
    first = requests[0]["cr"]
    start = ("warm, %s tokens read from an earlier run's cache" % format(first, ",")
             if first else "cold")
    if tot["w5"] and tot["w1"]:
        ttl = "mixed, %d%% of writes at 5m" % round(
            100.0 * tot["w5"] / (tot["w5"] + tot["w1"]))
    else:
        ttl = "5m" if tot["w5"] else "1h"
    return {"stop": stop, "prefix": start, "ttl": ttl}


def record(run, meta, facts):
    """Appends the facts meta.txt does not hold yet."""
    lines = ["%-8s %s\n" % (key, facts[key]) for key in ("stop", "prefix", "ttl")
             if key not in meta]
    if lines:
        with open(os.path.join(run, "meta.txt"), "a", encoding="utf-8") as handle:
            handle.writelines(lines)


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
    # The benchmark directory is the prompt's own material, never a fallback. It was
    # examples/ai-prompts before b1510d2e, so both spellings are excluded or runs from
    # either side of the rename cannot be compared.
    if re.search(r"(^|[\s/\"'])examples/(?!ai-benchmark|ai-prompts)", blob):
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
        # The steps a design declares are generated too, so they are not hand-written.
        design = os.path.join(os.path.dirname(os.path.abspath(src)), "design.json")
        if os.path.isfile(design):
            cmd += ["--spec", design]
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
# build_project.py --run chains run_scenarios.py as its last step, and that is the
# spelling the documentation gives, so a run that never types run_scenarios.py still
# runs the scenarios. The step is reached only when every step before it passed, and
# build_project.py prints its header before starting it, so the call's own output is
# what says whether it ran.
BUILD_RUN = re.compile(r"build_project\.py(?=.*\s--run\b)")
SCEN_STEP = re.compile(r"^==\s*scenarios:", re.M)
COUNTING = re.compile(r"\bwc\b|\bdu\b|--stat\b|\bcloc\b|stat\s+-c|\bfind\b.*-name.*\|",
                      re.I)
# A counting command measures the run only when it targets the run's own source.
COUNTED_SOURCE = re.compile(r"(^|[\s\"'])(src|scenarios\.json|design\.json|\*\.(cpp|hpp))")

# A counting command measures the RUN only when the thing it counts is the run's own
# source. wc on a file the run redirected output into is reading a log, not measuring
# itself, and a heredoc that opens scenarios.json to build a debug copy is not either.
COUNT_CALL = re.compile(r"\b(?:wc|cloc|du)\b((?:\s+-{1,2}[A-Za-z-]+)*)"
                        r"((?:\s+[^\s;|&<>]+)+)")

# A source file changed through the shell rather than through Edit or Write: sed -i,
# a redirect, tee, or a heredoc that opens the file for writing. A path under a
# temporary directory is scratch, not the project.
SOURCE_TOKEN = r"[^\s;|&<>'\"]+\.(?:cpp|hpp|h|json|txt|cmake)"
IN_PLACE = re.compile(r"\bsed\s+-i\b|\bpatch\b\s|\bdos2unix\b")
REDIRECT_WRITE = re.compile(r"(?:>>?\s*|\btee\b\s+(?:-a\s+)?)(" + SOURCE_TOKEN + r")")
OPEN_WRITE = re.compile(r"open\(\s*['\"](" + SOURCE_TOKEN + r")['\"]\s*,\s*['\"][wa]")
ANY_SOURCE = re.compile(SOURCE_TOKEN)
SCRATCH = re.compile(r"^/(?:tmp|var|dev|proc)/")


def counts_own_source(cmd):
    """True when a counting command's own operands are the run's source."""
    for match in COUNT_CALL.finditer(cmd):
        for operand in match.group(2).split():
            if COUNTED_SOURCE.search(operand):
                return True
    if re.search(r"--stat\b|stat\s+-c", cmd) and COUNTED_SOURCE.search(cmd):
        return True
    return False


def bash_edits(cmd):
    """The source files a shell command rewrites, scratch paths excluded."""
    found = [hit.group(1) for hit in REDIRECT_WRITE.finditer(cmd)]
    found += [hit.group(1) for hit in OPEN_WRITE.finditer(cmd)]
    # An in-place editor names its file after the script it applies, and that script
    # may itself hold the separators a single pattern would stop at.
    place = IN_PLACE.search(cmd)
    if place:
        found += ANY_SOURCE.findall(cmd[place.end():])
    return [p for p in found if not SCRATCH.search(p)]



def events(requests):
    """The discrete things that happen to a run, with the request each began at.

    A build is a fix cycle only when a source changed since the previous build; a
    build that follows no edit is a confirmation.
    """
    spec = None
    for r in requests:
        for nm, what, _ in r["calls"]:
            if nm == "Bash" and spec is None:
                hit = re.search(r"--spec\s+(\S+)", what)
                if hit:
                    spec = os.path.basename(hit.group(1).strip("'\""))
    builds, scenarios, respecs, counters = [], [], [], []
    build_fixes, scen_fixes = [], []
    edited_since_build = edited_since_scen = False
    last_change = -1
    for i, r in enumerate(requests):
        for nm, what, result in r["calls"]:
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
            for edited in bash_edits(what):
                edited_since_build = edited_since_scen = True
                last_change = i
                if spec and os.path.basename(edited) == spec and builds:
                    respecs.append(i)
            if BUILD_CALL.search(what):
                if builds and edited_since_build:
                    build_fixes.append(i)
                builds.append(i)
                edited_since_build = False
                last_change = max(last_change, i)
            # No captured output means the transcript did not keep it. The command
            # asked for the scenarios, so count it rather than lose it silently.
            ran = SCEN_CALL.search(what) or (BUILD_RUN.search(what) and
                                             (not result or SCEN_STEP.search(result)))
            if ran:
                if scenarios and edited_since_scen:
                    scen_fixes.append(i)
                scenarios.append(i)
                edited_since_scen = False
                last_change = max(last_change, i)
            if counts_own_source(what):
                counters.append(i)
    # A spec re-edit is not paid for by itself: the build that follows it is the
    # regeneration it forced, and that request is part of the event's price.
    regens = []
    for i in respecs:
        later = [b for b in builds if b > i]
        if later and later[0] not in regens:
            regens.append(later[0])
    return [("build invocations", builds, [], ""),
            ("build-and-fix cycles", build_fixes, [], "a source changed, then it rebuilt"),
            ("scenario runs", scenarios, [], ""),
            ("run-and-fix cycles", scen_fixes, [], "a source changed, then it ran again"),
            ("spec re-edits after a build", respecs, regens,
             "the design was changed after it had been generated"),
            ("self-measuring calls", counters, [], "wc, du, cloc: the run counting itself"),
            ("requests after the last change", list(range(last_change + 1, len(requests))),
             [], "the self-reporting tail")]


def print_events(requests, own, resident):
    """Each event twice: what its own requests were billed, and what they left behind.

    A request pays for its own reads, writes and output once. What it adds to the
    conversation -- its output and the tool result it pulled in -- is then read back by
    every later request, and that second term is the one no bill itemises.
    """
    print("\n== events   (the base barely moves; these are what move the bill)")
    print("   %-30s %2s %8s %9s %9s  %s"
          % ("", "n", "own", "resident", "total", "where"))
    for name, hits, forced, note in events(requests):
        paid = [i for i in hits + forced if i < len(own)]
        o = sum(own[i] for i in paid)
        res = sum(resident[i] for i in paid)
        where = ", ".join("r%d" % i for i in hits[:8]) + (" ..." if len(hits) > 8 else "")
        if forced:
            where += "  forced %s" % ", ".join("r%d" % i for i in forced[:8])
        print("   %-30s %2d %8s %9s %9s  %s"
              % (name, len(hits), "$%.3f" % o, "$%.3f" % res, "$%.3f" % (o + res),
                 where or "-"))
        if hits and note:
            print("   %-30s %s" % ("", note))


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("run", nargs="?", default=".", help="the run directory")
    parser.add_argument("--sdk", help="the SDK the run read (default: meta.txt, then here)")
    parser.add_argument("--record", action="store_true",
                        help="also append stop, start and cache ttl to meta.txt")
    args = parser.parse_args()
    run = args.run
    sdk = sdk_of(run, args.sdk).rstrip("/")
    meta = meta_of(run)

    result = read_result(run)
    sid = result.get("session_id")
    tr = find_transcript(run, sid)
    if not tr:
        sys.exit("no transcript for %s" % (sid or run))
    results = tool_results(tr)
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
                                  or inp.get("pattern") or ""),
                          results.get(bid) or ""))
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
            tier = u.get("cache_creation") or {}
            w5 = tier.get("ephemeral_5m_input_tokens") or 0
            w1 = tier.get("ephemeral_1h_input_tokens") or 0
            if not (w5 or w1):
                w1 = u.get("cache_creation_input_tokens") or 0
            tot["w5"] += w5
            tot["w1"] += w1
            requests.append({"id": mid, "ctx": ctx, "out": u.get("output_tokens") or 0,
                             "think": (u.get("output_tokens_details")
                                       or {}).get("thinking_tokens") or 0,
                             "tools": names, "calls": calls,
                             "in": u.get("input_tokens") or 0,
                             "cr": u.get("cache_read_input_tokens") or 0,
                             "cw": u.get("cache_creation_input_tokens") or 0,
                             "w5": w5, "w1": w1})
        elif mid:
            for r in requests:
                if r["id"] == mid:
                    r["tools"].extend(names)
                    r["calls"].extend(calls)
                    break
    if not requests:
        sys.exit("no assistant request in %s" % tr)

    fresh = tot["input_tokens"] + tot["output_tokens"]
    base = (tot["input_tokens"] * PRICE_IN + tot["cache_read_input_tokens"] * PRICE_READ +
            tot["output_tokens"] * PRICE_OUT)
    # A summariser and any other model the harness used are billed too, and the
    # transcript does not carry them. Without them the recomputed total falls short of
    # the bill by a fraction of a cent and reads as a modelling error.
    aside = sum(v.get("costUSD") or 0 for v in (result.get("modelUsage") or {}).values()
                if "sonnet" not in str(v.get("canonicalModel") or ""))
    cost = (base + tot["w5"] * PRICE_W5 + tot["w1"] * PRICE_W1) / 1e6 + aside
    normal = (base + (tot["w5"] + tot["w1"]) * PRICE_W1) / 1e6 + aside
    facts = run_facts(result, requests, tot)
    if args.record:
        record(run, meta, facts)
    print("== %s" % run)
    if facts["stop"] != "completed":
        print("   INCOMPLETE: the run stopped on %s; its price is not a price point"
              % facts["stop"])
    print("   %-26s %s" % ("session", sid))
    print("   %-26s %s" % ("stop", facts["stop"]))
    print("   %-26s %s" % ("cache at start", facts["prefix"]))
    print("   %-26s %s" % ("cache ttl", facts["ttl"]))
    areg_arm = (meta.get("framework") or "areg").strip() == "areg"
    print("   %-26s %s" % ("sdk read" if areg_arm else "runner staged from", sdk))
    if meta.get("model") and meta.get("model") != "sonnet":
        print("   %-26s %s  (prices below are Sonnet 5's)" % ("model", meta.get("model")))
    print("   %-26s %d" % ("API requests", len(requests)))
    print("   %-26s %d" % ("tool calls", sum(tools.values())))
    print("   %-26s %s" % ("fresh tokens (in+out)", format(fresh, ",")))
    print("   %-26s %s" % ("  output", format(tot["output_tokens"], ",")))
    vis = visible // 4
    print("   %-26s ~%s   (text + tool JSON, chars/4)" % ("  visible output", format(vis, ",")))
    think_tot = sum(r["think"] for r in requests)
    if think_tot:
        print("   %-26s %s   (exact, from output_tokens_details)"
              % ("  reasoning", format(think_tot, ",")))
    else:
        print("   %-26s ~%s   (output - visible)"
              % ("  reasoning", format(tot["output_tokens"] - vis, ",")))
    print("   %-26s %s" % ("  uncached input", format(tot["input_tokens"], ",")))
    print("   %-26s %s" % ("cache reads", format(tot["cache_read_input_tokens"], ",")))
    split = ("   (%s @5m + %s @1h)" % (format(tot["w5"], ","), format(tot["w1"], ","))
             if tot["w5"] and tot["w1"] else "")
    print("   %-26s %s%s" % ("cache writes",
                             format(tot["cache_creation_input_tokens"], ","), split))
    print("   %-26s %s" % ("peak context", format(max(r["ctx"] for r in requests), ",")))
    print("   %-26s $%.4f  (harness $%.4f)" % ("cost, billed", cost,
                                               result.get("total_cost_usd") or 0))
    if tot["w5"]:
        print("   %-26s $%.4f  (+$%.4f, %d%% of writes at 5m: compare runs here)"
              % ("cost, normalised @1h", normal, normal - cost,
                 round(100.0 * tot["w5"] / max(tot["w5"] + tot["w1"], 1))))
    wall = (result.get("duration_ms") or 0) / 60000.0
    print("   %-26s %.1f min%s" % ("wall", wall or span_minutes(tr),
                                   "" if wall else "   (from transcript timestamps)"))
    print("   %-26s %s" % ("permission denials", result.get("permission_denials")))
    print("   %-26s $%.4f  (normalised)" % ("cost per request", normal / len(requests)))
    print("\n== tool calls by name")
    for n, c in tools.most_common():
        print("   %-26s %d" % (n, c))
    # What a request left in the conversation: its own output, plus the tool result it
    # pulled in, which is the rest of the context growth before the next request. Both
    # are read back by every later request, at PRICE_READ a token each time.
    count = len(requests)
    own = [own_cost(r) for r in requests]
    added = [max(requests[i + 1]["ctx"] - requests[i]["ctx"] - requests[i]["out"], 0)
             if i + 1 < count else 0 for i in range(count)]
    resident = [(requests[i]["out"] + added[i]) * PRICE_READ * (count - 1 - i) / 1e6
                for i in range(count)]
    print_events(requests, own, resident)
    print("\n== %s: %d" % ("fallbacks to SDK internals" if areg_arm
                           else "reads of areg internals", len(fallbacks)))
    for i, nm, kind, what in fallbacks:
        print("   req %3d  %-6s %-16s %s" % (i, nm, kind, what))
    print("\n== filesystem searches: %d   (%s)"
          % (len(searches), "each one is a path the corpus failed to resolve" if areg_arm
             else "toolchain and package probes: this arm has no corpus"))
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
    # The areg arm keeps its sources under work/src and generates the rest from its
    # documents. Another framework lays its tree out its own way -- the gRPC arm uses
    # a directory per process -- so the sources are found by walking the project.
    framework = (meta_of(run).get("framework") or "areg").strip()
    work = os.path.join(run, "work")
    src = os.path.join(work, "src")
    base = src if os.path.isdir(src) else work
    sources = {}
    for root, dirs, names in os.walk(base):
        dirs[:] = [d for d in dirs if d not in ("build", "services", ".git")]
        for n in names:
            if n.endswith((".hpp", ".cpp", ".h", ".cc")):
                path = os.path.join(root, n)
                sources[os.path.relpath(path, base)] = open(path, errors="ignore").read()
    where = os.path.relpath(base, run).replace(os.sep, "/")
    lines = sum(len(t.splitlines()) for t in sources.values())
    print("   %-26s %d   (%d file(s) under %s)" % ("C++ lines", lines, len(sources), where))
    if framework == "areg":
        written = hand_written(src, sources, sdk)
        print("   %-26s %s" % ("  hand-written",
                                 "%d  (not in a regeneration of the same documents)" % written
                                 if written is not None else "unknown (regeneration failed)"))
        layout = layout_findings(sources)
        for finding in layout:
            print("   %-26s %s" % ("LAYOUT", finding))
        if not layout:
            print("   %-26s %s" % ("layout", "one class per file, no class beside main()"))
    else:
        # Nothing here is generated from a document the run wrote: protoc output goes
        # to the build directory, which is not walked.
        specs = []
        for root, dirs, names in os.walk(work):
            dirs[:] = [d for d in dirs if d not in ("build", ".git")]
            specs += [n for n in names if n.endswith(".proto")]
        print("   %-26s %d  (every line under %s)" % ("  hand-written", lines, where))
        print("   %-26s %d  (protoc output is under build/, not counted)"
              % ("  .proto contract(s)", len(specs)))

    spikes = sorted(enumerate(requests), key=lambda p: -p[1]["out"])[:2]
    print("\n== the reasoning spikes (the design and the implementation thought)")
    for i, r in sorted(spikes, key=lambda p: p[0]):
        # A token written at r is billed as output, then as the cache write that admits
        # it at the tier the next request paid, then read again by every later request.
        w = write_price(requests[i + 1]) if i + 1 < len(requests) else 0.0
        price = r["out"] * (PRICE_OUT + w +
                            PRICE_READ * max(len(requests) - i - 1, 0)) / 1e6
        print("   r%-4d %8s output  %8s thinking  %8s ctx  $%.3f fully priced"
              % (i, format(r["out"], ","), format(r["think"], ","),
                 format(r["ctx"], ","), price))

    print("\n== per-request timeline (idx  ctx  out  tools)")
    for i, r in enumerate(requests):
        print("   %3d  %8s %6s  %s" % (i, format(r["ctx"], ","), format(r["out"], ","),
                                       " ".join(r["tools"]) or "-"))


if __name__ == "__main__":
    main()
