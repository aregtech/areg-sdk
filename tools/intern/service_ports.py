#!/usr/bin/env python3
# -*- coding: ascii -*-
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""
The service ports the checkers share, and the reaping that keeps them independent.

Three checkers start mtrouter and logcollector on fixed ports. Run one after another
in the order CLAUDE.md prints them and the second inherits the first one's router: a
'terminate()' returns before the process has released the port, and the next readiness
poll connects to the corpse's successor instead of to its own child. The symptom is a
failure that passes on a rerun, which is the least useful failure there is.

Two rules make the run order irrelevant:

1. Before starting anything, the port must be free. If it is not, a stray service of
   ours is reaped; anything else is reported and the checker stops rather than
   measuring someone else's process.
2. After stopping, the checker waits for the port to be free again, so the next
   checker starts from the same state whatever it inherits.

Nothing here is specific to a checkout: ports come from the recipes and the tools are
found on PATH.
"""

import os
import shutil
import socket
import subprocess
import sys
import time

ROUTER_PORT = 8181          #!< mtrouter, the port every recipe connects to
COLLECTOR_PORT = 8282       #!< logcollector, the port every logging recipe connects to

# Process names a checker is allowed to reap. Exact names, never a prefix: an
# application of the user's that merely mentions the router is not ours to kill.
OWNED_PROCESSES = ('mtrouter', 'mtrouter.elf', 'mtrouter.exe',
                   'logcollector', 'logcollector.elf', 'logcollector.exe')


def is_listening(port, host='127.0.0.1'):
    """Tells whether anything accepts a connection on the port right now."""
    probe = socket.socket()
    probe.settimeout(0.5)
    try:
        probe.connect((host, port))
        return True
    except OSError:
        return False
    finally:
        probe.close()


def wait_listening(port, timeout=20.0):
    """Waits until the port accepts a connection. True when it did."""
    deadline = time.time() + timeout
    while time.time() < deadline:
        if is_listening(port):
            return True
        time.sleep(0.2)
    return False


def wait_free(port, timeout=15.0):
    """Waits until nothing accepts a connection on the port. True when nothing does."""
    deadline = time.time() + timeout
    while time.time() < deadline:
        if not is_listening(port):
            return True
        time.sleep(0.2)
    return False


def reap_services():
    """Stops every mtrouter and logcollector of this user. Returns the names stopped."""
    stopped = []
    if os.name == 'nt':
        killer = shutil.which('taskkill')
        for name in OWNED_PROCESSES:
            if not name.endswith('.exe') or killer is None:
                continue
            result = subprocess.run([killer, '/F', '/IM', name],
                                    stdout=subprocess.DEVNULL,
                                    stderr=subprocess.DEVNULL)
            if result.returncode == 0:
                stopped.append(name)
        return stopped

    killer = shutil.which('pkill')
    if killer is None:
        return stopped
    for name in OWNED_PROCESSES:
        result = subprocess.run([killer, '-u', str(os.getuid()), '-x', name],
                                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if result.returncode == 0:
            stopped.append(name)
    return stopped


def require_free(ports, stream=sys.stderr):
    """Makes the ports free before a checker starts. Returns True when they are.

    A stray service of ours is reaped. A port still held after that belongs to
    something else, and the checker is told to stop rather than to measure it.
    """
    busy = [port for port in ports if is_listening(port)]
    if not busy:
        return True

    stopped = reap_services()
    for port in busy:
        wait_free(port, timeout=10.0)

    still = [port for port in ports if is_listening(port)]
    if not still:
        stream.write('note: reaped a stray {} before starting\n'
                     .format(', '.join(stopped) if stopped else 'service'))
        return True

    stream.write('error: port {} is held by a process that is not ours; stop it and '
                 'run again\n'.format(', '.join(str(port) for port in still)))
    return False


def stop(handles, ports=(), timeout=10.0):
    """Stops the handles and waits for the ports to be free.

    'terminate()' on its own returns before the socket is released, which is what
    lets one checker poison the next. Every caller ends through here.
    """
    live = [handle for handle in handles if handle is not None]
    for handle in live:
        try:
            handle.terminate()
        except OSError:
            pass
    deadline = time.time() + timeout
    for handle in live:
        remaining = max(0.5, deadline - time.time())
        try:
            handle.wait(timeout=remaining)
        except subprocess.TimeoutExpired:
            try:
                handle.kill()
                handle.wait(timeout=5.0)
            except (OSError, subprocess.TimeoutExpired):
                pass
    return all(wait_free(port) for port in ports)
