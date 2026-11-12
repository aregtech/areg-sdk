#!/usr/bin/env python3
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""Collects the compiler and linker warnings of the GitHub Actions runs of one commit.

Reads the log of every job of every workflow run of a commit, finds the warnings that
GCC, Clang, MSVC and the linkers print, removes the duplicates and writes one report
that names, for each warning, every job that printed it. Nothing has to change in the
workflows: the job logs already hold the compiler output, and ccache replays it on a
cache hit.

    collect-warnings.py                         # newest commit with runs on the current branch
    collect-warnings.py --branch master
    collect-warnings.py --sha 3bf4ac63f
    collect-warnings.py --run 43108494705 --run 43108494703
    collect-warnings.py --log build.log         # a local build log, no GitHub access
    collect-warnings.py --md warnings.md --json warnings.json --exclude "configure\\.cpp$"

GitHub access goes through the 'gh' command line tool, which must be installed and
logged in ('gh auth login', or GH_TOKEN in a workflow). The repository is the one of
the current directory unless --repo names another, so the tool works for any project.

Paths are reported relative to the checkout root of the runner, so the same warning on
Linux, macOS, Windows and Cygwin is one entry. The exit code is 0 unless an error stops
the collection, or --fail-on-warnings is given and a warning remains after --exclude.
"""

import argparse
import concurrent.futures
import json
import re
import subprocess
import sys
from collections import OrderedDict

SOURCE_EXT = r"(?:c|cc|cp|cpp|cxx|c\+\+|h|hh|hpp|hxx|h\+\+|inl|ipp|tpp|m|mm)"

# GCC and Clang: path:line[:col]: warning: message [-Wflag]
RE_GNU = re.compile(
    r"(?P<file>(?:[A-Za-z]:)?[^\s:]*?[^\s:]\." + SOURCE_EXT + r"):(?P<line>\d+):(?:\d+:)?"
    r"\s+warning:\s+(?P<msg>.*?)(?:\s+\[(?P<flag>-W[^\]]+)\])?\s*$")

# MSVC and clang-cl: path(line[,col]): warning C1234: message [project]
RE_MSVC = re.compile(
    r"(?P<file>(?:[A-Za-z]:)?[^():\s][^():]*?\." + SOURCE_EXT + r")\((?P<line>\d+)(?:,\d+)?\)\s*:"
    r"\s+warning\s+(?P<flag>[A-Z]+\d+)\s*:\s+(?P<msg>.*?)(?:\s+\[[^\]]+\.(?:vcxproj|csproj|proj)\])?\s*$",
    re.IGNORECASE)

# GNU ld, lld, ld64: ...ld: warning: message
RE_LD = re.compile(r"(?:^|[\s/])(?:ld|ld\.lld|ld\.gold|ld64|lld-link)(?:\.exe)?:\s+warning:\s+(?P<msg>.+?)\s*$")

# MSVC linker and librarian: LINK : warning LNK4221: message
RE_LNK = re.compile(r"(?:^|\s)(?:LINK|LIB)\s*:\s+warning\s+(?P<flag>LNK\d+)\s*:\s+(?P<msg>.+?)(?:\s+\[[^\]]+\])?\s*$")

RE_TIMESTAMP = re.compile(r"^\d{4}-\d\d-\d\dT\d\d:\d\d:\d\d(?:\.\d+)?Z ")
RE_ANSI = re.compile(r"\x1b\[[0-9;]*[A-Za-z]")

# Quotes GCC prints in a UTF-8 locale, and the replacement character a wrong locale leaves.
QUOTE_MAP = str.maketrans({"\u2018": "'", "\u2019": "'", "\u201c": '"', "\u201d": '"',
                           "\ufffd": "'", "`": "'"})


class Warning_:
    """One distinct warning and the jobs that printed it."""

    __slots__ = ("kind", "path", "line", "flag", "message", "jobs")

    def __init__(self, kind, path, line, flag, message):
        self.kind = kind
        self.path = path
        self.line = line
        self.flag = flag
        self.message = message
        self.jobs = []

    def key(self):
        return (self.kind, self.path, self.line, self.flag, self.message)

    def as_dict(self):
        return {"kind": self.kind, "path": self.path, "line": self.line, "flag": self.flag,
                "message": self.message, "jobs": self.jobs}


def fail(text):
    print("collect-warnings: " + text, file=sys.stderr)
    sys.exit(2)


def run_gh(args, check=True):
    """Runs gh and returns its standard output as text."""
    try:
        proc = subprocess.run(["gh"] + args, capture_output=True)
    except FileNotFoundError:
        fail("the 'gh' command line tool is not installed or not on PATH")
    if proc.returncode != 0:
        if check:
            fail("gh " + " ".join(args) + " failed: " + proc.stderr.decode("utf-8", "replace").strip())
        return None
    return proc.stdout.decode("utf-8", "replace")


def gh_json(path):
    return json.loads(run_gh(["api", path]))


def git_output(args):
    try:
        proc = subprocess.run(["git"] + args, capture_output=True)
    except FileNotFoundError:
        return ""
    return proc.stdout.decode("utf-8", "replace").strip() if proc.returncode == 0 else ""


class PathNormalizer:
    """Turns an absolute runner path into a path relative to the checkout root."""

    def __init__(self, repo_name, local_root):
        name = re.escape(repo_name)
        # Linux and macOS: .../work/<repo>/<repo>/, Windows: X:\a\<repo>\<repo>\, Cygwin: /cygdrive/x/a/<repo>/<repo>/
        self._runner = re.compile(r"^.*?[/\\](?:work|a)[/\\]" + name + r"[/\\]" + name + r"[/\\]", re.IGNORECASE)
        self._local = None
        if local_root:
            self._local = re.compile("^" + re.escape(local_root.replace("\\", "/").rstrip("/") + "/"), re.IGNORECASE)

    def __call__(self, path):
        path = self._runner.sub("", path).replace("\\", "/")
        if self._local is not None:
            path = self._local.sub("", path)
        while path.startswith("./") or path.startswith("../"):
            path = path[path.index("/") + 1:]
        return path


def parse_line(text, normalize):
    """Returns (kind, path, line, flag, message) for a warning line, None otherwise."""
    if "warning" not in text and "WARNING" not in text:
        return None
    m = RE_GNU.search(text)
    if m:
        return ("compiler", normalize(m["file"]), int(m["line"]), m["flag"] or "", m["msg"].translate(QUOTE_MAP))
    m = RE_MSVC.search(text)
    if m:
        return ("compiler", normalize(m["file"]), int(m["line"]), m["flag"].upper(), m["msg"].translate(QUOTE_MAP))
    m = RE_LNK.search(text)
    if m:
        return ("linker", "", 0, m["flag"], m["msg"].translate(QUOTE_MAP))
    m = RE_LD.search(text)
    if m:
        return ("linker", "", 0, "", m["msg"].translate(QUOTE_MAP))
    return None


def parse_log(text, origin, normalize, found):
    """Adds the warnings of one log to 'found', returns how many lines matched."""
    hits = 0
    for raw in text.splitlines():
        line = RE_ANSI.sub("", RE_TIMESTAMP.sub("", raw, count=1))
        entry = parse_line(line, normalize)
        if entry is None:
            continue
        hits += 1
        item = found.get(entry)
        if item is None:
            item = Warning_(*entry)
            found[entry] = item
        if origin not in item.jobs:
            item.jobs.append(origin)
    return hits


def resolve_runs(repo, args):
    """Returns the list of workflow runs to read, newest attempt of each."""
    if args.run:
        return [gh_json("repos/{}/actions/runs/{}".format(repo, run_id)) for run_id in args.run]

    sha = args.sha
    if sha:
        if len(sha) < 40:
            sha = git_output(["rev-parse", "--verify", "--quiet", sha + "^{commit}"]) or gh_json("repos/{}/commits/{}".format(repo, sha))["sha"]
    else:
        branch = args.branch or git_output(["branch", "--show-current"])
        if not branch:
            fail("no branch: give --branch, --sha or --run")
        runs = gh_json("repos/{}/actions/runs?branch={}&per_page=1".format(repo, branch))["workflow_runs"]
        if not runs:
            fail("no workflow runs on branch '{}'".format(branch))
        sha = runs[0]["head_sha"]

    runs = gh_json("repos/{}/actions/runs?head_sha={}&per_page=100".format(repo, sha))["workflow_runs"]
    if args.workflow:
        wanted = [w.lower() for w in args.workflow]
        runs = [r for r in runs
                if r["name"].lower() in wanted or r["path"].rsplit("/", 1)[-1].lower() in wanted]
    if not args.all_runs:
        newest = OrderedDict()
        for run in sorted(runs, key=lambda r: r["created_at"], reverse=True):
            newest.setdefault(run["name"], run)
        runs = list(newest.values())
    if not runs:
        fail("no workflow runs for commit {}".format(sha))
    return runs


def collect_github(repo, args, normalize, found, jobs_seen):
    runs = resolve_runs(repo, args)
    jobs = []
    for run in runs:
        state = run["status"] if run["status"] != "completed" else run["conclusion"]
        print("run {} {:<20} {:<10} {}".format(run["id"], run["name"], state, run["head_sha"][:10]), file=sys.stderr)
        listing = gh_json("repos/{}/actions/runs/{}/jobs?per_page=100".format(repo, run["id"]))["jobs"]
        for job in listing:
            if job["status"] == "completed" and job["conclusion"] != "skipped":
                jobs.append((run["name"], job))

    def fetch(item):
        workflow, job = item
        return item, run_gh(["api", "repos/{}/actions/jobs/{}/logs".format(repo, job["id"])], check=False)

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        for (workflow, job), text in pool.map(fetch, jobs):
            origin = "{}/{}".format(workflow, job["name"])
            if text is None:
                print("  no log: " + origin, file=sys.stderr)
                jobs_seen[origin] = None
                continue
            jobs_seen[origin] = parse_log(text, origin, normalize, found)
    return runs


def collect_files(paths, normalize, found, jobs_seen):
    for path in paths:
        try:
            with open(path, "rb") as handle:
                text = handle.read().decode("utf-8", "replace")
        except OSError as error:
            fail("cannot read {}: {}".format(path, error))
        jobs_seen[path] = parse_log(text, path, normalize, found)


def write_markdown(handle, title, warnings, excluded, jobs_seen):
    handle.write("# {}\n\n".format(title))
    handle.write("{} distinct warning(s), {} excluded, {} job log(s) read.\n\n".format(
        len(warnings), len(excluded), sum(1 for v in jobs_seen.values() if v is not None)))

    by_file = OrderedDict()
    for item in warnings:
        by_file.setdefault(item.path or "(linker)", []).append(item)
    for path, items in by_file.items():
        handle.write("## `{}`\n\n| Line | Flag | Message | Jobs |\n|---:|---|---|---|\n".format(path))
        for item in items:
            message = item.message.replace("|", "\\|")
            jobs = "<br>".join(sorted(item.jobs)).replace("|", "\\|")
            handle.write("| {} | {} | {} | {} |\n".format(item.line or "", item.flag, message, jobs))
        handle.write("\n")

    if excluded:
        handle.write("## Excluded\n\n")
        for item in excluded:
            handle.write("- `{}:{}` {} {} ({} job(s))\n".format(item.path, item.line, item.flag, item.message, len(item.jobs)))
        handle.write("\n")

    counts = {}
    for item in warnings:
        for job in item.jobs:
            counts[job] = counts.get(job, 0) + 1
    handle.write("## Jobs\n\n| Job | Distinct warnings |\n|---|---:|\n")
    for job in sorted(jobs_seen):
        value = "no log" if jobs_seen[job] is None else str(counts.get(job, 0))
        handle.write("| {} | {} |\n".format(job.replace("|", "\\|"), value))


def main():
    parser = argparse.ArgumentParser(description="Collects the compiler and linker warnings of the GitHub Actions runs of one commit.")
    source = parser.add_mutually_exclusive_group()
    source.add_argument("--branch", help="newest commit with workflow runs on this branch (default: the current git branch)")
    source.add_argument("--sha", help="commit whose workflow runs are read")
    source.add_argument("--run", action="append", type=int, help="workflow run id, repeatable; reads exactly these runs")
    source.add_argument("--log", action="append", help="local log file, repeatable; no GitHub access")
    parser.add_argument("--repo", help="OWNER/NAME (default: the repository of the current directory)")
    parser.add_argument("--workflow", action="append", help="only this workflow, by name or file name, repeatable")
    parser.add_argument("--all-runs", action="store_true", help="read every run of a workflow for the commit, not only the newest")
    parser.add_argument("--exclude", action="append", default=[], help="regular expression on the relative path; matching warnings are listed apart, repeatable")
    parser.add_argument("--md", help="write the Markdown report to this file (default: standard output)")
    parser.add_argument("--json", help="write the report as JSON to this file")
    parser.add_argument("--jobs", type=int, default=8, help="job logs downloaded in parallel (default: 8)")
    parser.add_argument("--fail-on-warnings", action="store_true", help="exit with 1 when a warning remains after --exclude")
    args = parser.parse_args()

    found = OrderedDict()
    jobs_seen = OrderedDict()
    local_root = git_output(["rev-parse", "--show-toplevel"])

    if args.log:
        name = local_root.replace("\\", "/").rstrip("/").rsplit("/", 1)[-1] if local_root else "repository"
        collect_files(args.log, PathNormalizer(name, local_root), found, jobs_seen)
        title = "Warnings in " + ", ".join(args.log)
        source = {"logs": args.log}
    else:
        repo = args.repo
        if not repo:
            out = run_gh(["repo", "view", "--json", "nameWithOwner", "--jq", ".nameWithOwner"])
            repo = out.strip()
        runs = collect_github(repo, args, PathNormalizer(repo.split("/", 1)[1], local_root), found, jobs_seen)
        shas = sorted({r["head_sha"] for r in runs})
        title = "Warnings of {} at {}".format(repo, ", ".join(s[:10] for s in shas))
        source = {"repo": repo, "sha": shas,
                  "runs": [{"id": r["id"], "workflow": r["name"], "status": r["status"],
                            "conclusion": r["conclusion"], "url": r["html_url"]} for r in runs]}

    patterns = [re.compile(p) for p in args.exclude]
    items = sorted(found.values(), key=lambda w: (w.kind != "compiler", w.path, w.line, w.flag, w.message))
    warnings = [w for w in items if not any(p.search(w.path) for p in patterns)]
    excluded = [w for w in items if any(p.search(w.path) for p in patterns)]

    if args.md:
        with open(args.md, "w", encoding="utf-8", newline="\n") as handle:
            write_markdown(handle, title, warnings, excluded, jobs_seen)
    else:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
        write_markdown(sys.stdout, title, warnings, excluded, jobs_seen)

    if args.json:
        report = {"source": source, "warnings": [w.as_dict() for w in warnings],
                  "excluded": [w.as_dict() for w in excluded], "jobs": jobs_seen}
        with open(args.json, "w", encoding="utf-8", newline="\n") as handle:
            json.dump(report, handle, indent=2, ensure_ascii=False)
            handle.write("\n")

    print("{} distinct warning(s), {} excluded, {} job log(s)".format(len(warnings), len(excluded), len(jobs_seen)), file=sys.stderr)
    return 1 if args.fail_on_warnings and warnings else 0


if __name__ == "__main__":
    sys.exit(main())
