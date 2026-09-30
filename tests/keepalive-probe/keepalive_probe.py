#!/usr/bin/env python3
"""
Drives areg-keepalive-probe: how long a connection configured by the Areg framework
takes to notice a dead peer, and whether a short loss kills a healthy one.

  report     every platform, no privileges: effective keepalive values and defaults.
  deadpeer   Linux (iptables on lo) and macOS (pf on lo0), root: all packets of one
             loopback connection are dropped; the time until each side reports an error,
             with the client idle and with the client sending 64 bytes every 500 ms.
  blip       Linux, root: the packets are dropped for N seconds, then let through again;
             does the idle connection survive.
  stall      every platform, no privileges: the receiver reads nothing for N seconds
             while the sender sends at full rate; does the connection survive.

Windows cannot drop loopback packets with its firewall, so it runs the report only.
The result is written as Markdown to --out and, on GitHub, to the job summary.
"""

import argparse
import os
import platform
import shutil
import subprocess
import sys
import tempfile
import threading
import time

PROBE_NAMES = ("areg-keepalive-probe", "areg-keepalive-probe.exe", "areg-keepalive-probe.elf",
               "areg-keepalive-probe.mac")


def find_probe(search_roots):
    """Returns the path of the probe executable under the given roots, or None."""
    for root in search_roots:
        for dirpath, _dirs, files in os.walk(root):
            for name in PROBE_NAMES:
                if name in files:
                    return os.path.join(dirpath, name)
    return None


def sudo_prefix():
    """Returns the prefix that runs a command as root, or None if that is not possible."""
    if os.name != "posix":
        return None
    if os.geteuid() == 0:
        return []
    if shutil.which("sudo") and subprocess.run(["sudo", "-n", "true"], capture_output=True).returncode == 0:
        return ["sudo", "-n"]
    return None


class LinuxDrop:
    """Drops the packets of one loopback port with iptables."""

    def __init__(self, sudo):
        self.sudo = sudo

    def _rules(self, port):
        return [["INPUT", "-i", "lo", "-p", "tcp", "--dport", str(port), "-j", "DROP"],
                ["INPUT", "-i", "lo", "-p", "tcp", "--sport", str(port), "-j", "DROP"]]

    def add(self, port):
        for rule in self._rules(port):
            subprocess.run(self.sudo + ["iptables", "-I"] + rule, check=True)

    def remove(self, port):
        for rule in self._rules(port):
            subprocess.run(self.sudo + ["iptables", "-D"] + rule, capture_output=True)

    def close(self):
        pass


class MacDrop:
    """Drops the packets of loopback ports with pf. A load replaces the whole rule set."""

    def __init__(self, sudo):
        self.sudo = sudo
        self.ports = set()
        self.lock = threading.Lock()
        self.token = None

    def _load(self):
        lines = []
        for port in sorted(self.ports):
            lines.append("block drop quick on lo0 proto tcp from any port %d to any" % port)
            lines.append("block drop quick on lo0 proto tcp from any to any port %d" % port)
        with tempfile.NamedTemporaryFile("w", suffix=".conf", delete=False) as conf:
            conf.write("\n".join(lines) + "\n")
        subprocess.run(self.sudo + ["pfctl", "-q", "-f", conf.name], check=True, capture_output=True)
        os.unlink(conf.name)
        if self.token is None:
            out = subprocess.run(self.sudo + ["pfctl", "-E"], capture_output=True, text=True)
            for line in (out.stdout + out.stderr).splitlines():
                if "Token" in line:
                    self.token = line.split(":")[-1].strip()

    def add(self, port):
        with self.lock:
            self.ports.add(port)
            self._load()

    def remove(self, port):
        with self.lock:
            self.ports.discard(port)
            self._load()

    def close(self):
        with self.lock:
            self.ports.clear()
            subprocess.run(self.sudo + ["pfctl", "-q", "-f", "/etc/pf.conf"], capture_output=True)
            if self.token:
                subprocess.run(self.sudo + ["pfctl", "-X", self.token], capture_output=True)


def run_scenario(probe, args, drop, lift_after, results, limit, drop_after=0.0):
    """Starts the probe, drops its port after drop_after s, sends GO, lifts the drop if asked, collects RESULT lines."""
    proc = subprocess.Popen([probe] + args, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, text=True, bufsize=1)
    port = None
    try:
        for line in proc.stdout:
            if line.startswith("READY port="):
                port = int(line.split("=", 1)[1])
                break
        if port is None:
            results.append("RESULT scenario=%s error=no READY line" % " ".join(args))
            return
        time.sleep(drop_after)
        if drop is not None:
            drop.add(port)
        proc.stdin.write("GO\n")
        proc.stdin.flush()
        if lift_after is not None:
            time.sleep(lift_after)
            if drop is not None:
                drop.remove(port)
                port = None
        try:
            out, _ = proc.communicate(timeout=limit + 60)
        except subprocess.TimeoutExpired:
            proc.kill()
            out, _ = proc.communicate()
        results.extend(l for l in out.splitlines() if l.startswith("RESULT"))
    finally:
        if (drop is not None) and (port is not None):
            drop.remove(port)
        if proc.poll() is None:
            proc.kill()


def parse_result(line):
    """Splits 'RESULT k=v k=v ...' into a dict; the error text after '(' is kept whole."""
    fields = {}
    body = line[len("RESULT "):]
    if " (" in body:
        body, reason = body.split(" (", 1)
        fields["reason"] = reason.rstrip(")")
    for token in body.split():
        if "=" in token:
            key, value = token.split("=", 1)
            fields[key] = value
    return fields


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--probe", help="path of areg-keepalive-probe; searched under --search if omitted")
    parser.add_argument("--search", nargs="*", default=["build", "product"], help="where to look for the probe")
    parser.add_argument("--out", default="keepalive-report.md", help="Markdown report to write")
    parser.add_argument("--limit", type=int, default=1200, help="seconds each dead-peer scenario may take")
    parser.add_argument("--blips", default="2,4,6,8,10", help="drop durations in seconds for the blip scenario")
    parser.add_argument("--blip-phase", type=float, default=4.5,
                        help="seconds the connection is idle before a blip starts; 4.5 puts a keepalive probe inside it")
    parser.add_argument("--stalls", default="10,20", help="seconds the receiver reads nothing, for the stall scenario")
    parser.add_argument("--no-drop", action="store_true",
                        help="run the dead-peer scenarios without a drop rule, to check the plumbing")
    opts = parser.parse_args()

    probe = opts.probe or find_probe(opts.search)
    if not probe:
        print("areg-keepalive-probe not found under %s" % ", ".join(opts.search))
        return 2

    system = platform.system()
    report = subprocess.run([probe, "report"], capture_output=True, text=True).stdout
    md = ["# Keepalive probe: %s (%s)" % (system, platform.release()), "", "## Report", "", "```", report.strip(), "```", ""]

    sudo = sudo_prefix()
    drop = None
    if opts.no_drop:
        pass
    elif system == "Linux" and sudo is not None and shutil.which("iptables"):
        drop = LinuxDrop(sudo)
    elif system == "Darwin" and sudo is not None:
        drop = MacDrop(sudo)

    results = []
    if system in ("Linux", "Darwin") and (drop is not None or opts.no_drop):
        threads = [threading.Thread(target=run_scenario,
                                    args=(probe, ["deadpeer", s, str(opts.limit)], drop, None, results, opts.limit))
                   for s in ("idle", "busy")]
        if system == "Linux":
            for hold in [int(x) for x in opts.blips.split(",") if x.strip()]:
                window = hold + 10
                threads.append(threading.Thread(target=run_scenario,
                                                args=(probe, ["blip", str(window)], drop, hold, results, window,
                                                      opts.blip_phase)))
        try:
            for t in threads:
                t.start()
            for t in threads:
                t.join()
        finally:
            if drop is not None:
                drop.close()

        md += ["## Dead peer", "", "| scenario | side | detected | seconds | error |", "|---|---|---|---|---|"]
        for line in sorted(r for r in results if "scenario=blip" not in r):
            f = parse_result(line)
            md.append("| %s | %s | %s | %s | %s |" % (f.get("scenario", "?"), f.get("side", "?"), f.get("detected", "?"),
                                                  f.get("seconds", "-"), f.get("reason", f.get("error", ""))))
        blips = [parse_result(r) for r in results if "scenario=blip" in r]
        if blips:
            md += ["", "## Blip (idle %.1f s, then drop for N s, then let through)" % opts.blip_phase, "",
                   "| drop, s | window, s | survived | client error | server error |", "|---|---|---|---|---|"]
            for f in sorted(blips, key=lambda x: int(x.get("window", 0))):
                md.append("| %d | %s | %s | %s | %s |" % (int(f.get("window", 10)) - 10, f.get("window"), f.get("survived"),
                                                         f.get("client_error"), f.get("server_error")))
    else:
        md += ["## Dead peer", "", "Not run: %s." % ("Windows cannot drop loopback packets" if system == "Windows"
                                                    else "root (or passwordless sudo) and a packet filter are required")]

    md += ["", "## Stall (the receiver reads nothing for N s while the sender sends)", "",
           "| stall, s | survived | send timeouts | client error | server error |", "|---|---|---|---|---|"]
    for seconds in [int(x) for x in opts.stalls.split(",") if x.strip()]:
        out = subprocess.run([probe, "stall", str(seconds)], capture_output=True, text=True).stdout
        for line in out.splitlines():
            if line.startswith("RESULT"):
                f = parse_result(line)
                md.append("| %s | %s | %s | %s | %s |" % (f.get("seconds"), f.get("survived"), f.get("send_timeouts"),
                                                         f.get("client_error"), f.get("server_error")))

    text = "\n".join(md) + "\n"
    with open(opts.out, "w") as out:
        out.write(text)
    summary = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary:
        with open(summary, "a") as out:
            out.write(text)
    print(text)
    return 0


if __name__ == "__main__":
    sys.exit(main())
