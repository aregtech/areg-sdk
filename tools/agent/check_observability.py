#!/usr/bin/env python3
# ===========================================================================
# Runs the loop the SDK exists to support and asserts it end to end: two
# processes reach each other through mtrouter, both log to logcollector instead
# of to their own consoles, the collector writes a .sqlog database, and the
# database is queried for the scopes both processes were supposed to write.
#
#   python3 tools/agent/check_observability.py --lib build/bin
#
# This is the one path an agent cannot invent: three programs and a database.
# Documenting it is not enough -- if it silently stops working, every page that
# describes debugging becomes wrong at once.
#
# No agent takes part, and no model is called. The recipe under test is
# docs/agent/recipes/08-observability.
#
# Exit code 0 when the database holds what the recipe logged, 1 otherwise.
# ===========================================================================
import argparse
import glob
import os
import shutil
import sqlite3
import subprocess
import sys
import tempfile
import time

SDK = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(SDK, 'tools', 'agent'))
import service_ports

RECIPE = os.path.join(SDK, 'docs', 'agent', 'recipes', '08-observability')
ROUTER_PORT = service_ports.ROUTER_PORT
COLLECTOR_PORT = service_ports.COLLECTOR_PORT
# The scopes the recipe declares. Each must reach the database from its own
# process, which is what proves the collector saw both.
WANTED_SCOPES = ['obs_Provider.request_hello_service',
                 'obs_Consumer.service_connected']
WANTED_INSTANCES = ['provider', 'consumer']
# Microseconds since the epoch, between 2020 and 2100. A column holding seconds
# or milliseconds lands far below the first of them.
EPOCH_LOW = 1577836800000000
EPOCH_HIGH = 4102444800000000
FAILURE_LIMIT = 6


def service_binary(lib, name):
    for suffix in ('.elf', '', '.exe'):
        candidate = os.path.join(lib, name + suffix)
        if os.path.isfile(candidate):
            return candidate
    return None


def wait_port(port, timeout=20.0):
    """Waits until the port accepts a connection. True when it did."""
    return service_ports.wait_listening(port, timeout)


def reader():
    """The recipe's own query_sqlog.py.

    What CI executes is the script the recipe hands an agent. A second copy here
    would be the one that goes stale.
    """
    sys.path.insert(0, RECIPE)
    import query_sqlog
    return query_sqlog


def query(database):
    """What the collector stored: the process of every log row, and its text."""
    return reader().modules_and_messages(database)


def contract_failures(database):
    """Every column contract 35-sqlog.md states, read back off a real file.

    The page is what an agent queries the database from, so a column that does
    not hold what the page says it holds sends the agent somewhere wrong. Both
    kinds of row are checked: the ones an application logged, and the ones the
    collector wrote itself.
    """
    query_sqlog = reader()
    rows = query_sqlog.all_rows(database)
    connection = sqlite3.connect(database)
    try:
        instances = {row[0] for row in connection.execute(
            'select cookie_id from instances')}
        scopes = {(row[0], row[1]) for row in connection.execute(
            'select scope_id, cookie_id from scopes')}
    finally:
        connection.close()

    failures = []

    def note(row, text):
        if len(failures) < FAILURE_LIMIT:
            failures.append('logs.id {}: {}'.format(row['id'], text))

    for row in rows:
        collector = query_sqlog.is_collector_row(row)
        text = row['msg_log'] or ''
        module = row['msg_module']
        if row['msg_type'] is None or row['msg_prio'] is None:
            note(row, 'msg_type or msg_prio is NULL')
            continue

        if row['msg_type'] not in (0, 1, 2, 4):
            note(row, 'msg_type is {}, and the page names 0, 1, 2 and 4'
                 .format(row['msg_type']))
        if not isinstance(module, str) or module.strip() == '' or module.isdigit():
            note(row, 'msg_module is {!r}, not an executable name'.format(module))
        # msg_len holds the length before the cut, so it is at or above the text
        # that survived. A file older than the column has no opinion.
        if 'msg_len' in row:
            try:
                length = int(row['msg_len'])
            except (TypeError, ValueError):
                length = -1
            if length < len(text):
                note(row, 'msg_len is {!r} for {} characters of text'
                     .format(row['msg_len'], len(text)))
        duration = row['time_duration']
        if duration is None:
            note(row, 'time_duration is NULL, and the page says 0 outside a scope')
        elif duration < 0:
            note(row, 'time_duration is {}'.format(duration))
        elif row['scope_id'] == 0 and duration != 0:
            note(row, 'time_duration is {} on a row that carries no scope'
                 .format(duration))
        for column in ('time_created', 'time_received'):
            stamp = row[column]
            if not isinstance(stamp, int) or not EPOCH_LOW < stamp < EPOCH_HIGH:
                note(row, '{} is {!r}, not microseconds since the epoch'
                     .format(column, stamp))

        if collector:
            if row['scope_id'] != 0:
                note(row, 'a collector row carries scope {}, and no scope of the '
                          'collector is registered'.format(row['scope_id']))
        else:
            if row['cookie_id'] not in instances:
                note(row, 'cookie_id {} joins no row of instances'
                     .format(row['cookie_id']))
            if row['scope_id'] and (row['scope_id'], row['cookie_id']) not in scopes:
                note(row, 'scope_id {} joins no row of scopes for cookie {}'
                     .format(row['scope_id'], row['cookie_id']))

    return failures, len(rows)


def main():
    parser = argparse.ArgumentParser(
        description='Run the router, the collector and the recipe, then query the '
                    'log database.')
    parser.add_argument('--lib', default=os.path.join('build', 'bin'),
                        help='directory holding libareg, mtrouter and logcollector')
    parser.add_argument('--compiler', default=os.environ.get('CXX', 'g++'))
    parser.add_argument('--keep', action='store_true', help='keep the work directory')
    args = parser.parse_args()

    lib = os.path.abspath(args.lib)
    if not glob.glob(os.path.join(lib, 'libareg*')):
        sys.stderr.write('error: no libareg under {}\n'.format(lib))
        return 1
    router_bin = service_binary(lib, 'mtrouter')
    collector_bin = service_binary(lib, 'logcollector')
    for name, path in (('mtrouter', router_bin), ('logcollector', collector_bin)):
        if path is None:
            sys.stderr.write('error: {} is not under {}; build that target first\n'
                             .format(name, lib))
            return 1

    sys.path.insert(0, os.path.join(SDK, 'tools', 'agent'))
    import check_recipes

    work = tempfile.mkdtemp(prefix='areg-observability-')
    root = os.path.join(work, '08-observability')
    shutil.copytree(RECIPE, root)
    database = os.path.join(root, 'collected.sqlog')
    router = None
    collector = None
    provider = None
    try:
        ok, binaries, detail = check_recipes.build_recipe(root, lib, args.compiler)
        if not ok:
            print('FAIL  the recipe did not build: ' + detail)
            return 1
        by_name = {os.path.basename(b): b for b in binaries}
        if 'provider' not in by_name or 'consumer' not in by_name:
            print('FAIL  expected a provider and a consumer, built {}'.format(
                sorted(by_name)))
            return 1

        if not service_ports.require_free([ROUTER_PORT, COLLECTOR_PORT]):
            print('FAIL  ports {} and {} must be free before this check runs'
                  .format(ROUTER_PORT, COLLECTOR_PORT))
            return 1

        router = subprocess.Popen([router_bin, '--service'], cwd=root,
                                  stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if not wait_port(ROUTER_PORT):
            print('FAIL  mtrouter did not listen on {}'.format(ROUTER_PORT))
            return 1

        # --log=db overrides the collector's own configuration, so the database
        # lands where this check can find it.
        collector = subprocess.Popen(
            [collector_bin, '--service', '--log=db', database], cwd=root,
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if not wait_port(COLLECTOR_PORT):
            print('FAIL  logcollector did not listen on {}'.format(COLLECTOR_PORT))
            return 1

        provider = subprocess.Popen([by_name['provider']], cwd=root,
                                    stdout=subprocess.DEVNULL,
                                    stderr=subprocess.DEVNULL)
        time.sleep(1.0)
        try:
            result = subprocess.run([by_name['consumer']], cwd=root,
                                    capture_output=True, text=True, timeout=60)
        except subprocess.TimeoutExpired:
            print('FAIL  the consumer did not finish within 60s')
            return 1
        if result.returncode != 0:
            print('FAIL  the consumer exited {}'.format(result.returncode))
            return 1
        if 'consumer: greeted' not in (result.stdout + result.stderr):
            print('FAIL  the two processes did not reach each other')
            return 1

        # The collector writes as it receives; give the last records their moment
        # before the process is asked to stop.
        time.sleep(2.0)
    finally:
        service_ports.stop([provider, collector, router],
                           [ROUTER_PORT, COLLECTOR_PORT])

    if not os.path.isfile(database):
        print('FAIL  logcollector wrote no database at {}'.format(database))
        return 1

    modules, messages = query(database)
    failures = []
    for wanted in WANTED_INSTANCES:
        if wanted not in modules:
            failures.append('no log row came from the {} process'.format(wanted))
    for scope in WANTED_SCOPES:
        if not any(scope in message for message in messages):
            failures.append('the database holds no record of scope ' + scope)
    broken, total = contract_failures(database)
    failures.extend(broken)

    if not args.keep:
        shutil.rmtree(work, ignore_errors=True)
    else:
        print('work directory: ' + work)

    for note in failures:
        print('FAIL  ' + note)
    if failures:
        return 1
    print('PASS  08-observability   router, collector and {} log rows in the '
          '.sqlog database, {} of them written by the collector itself, every '
          'documented column contract held'
          .format(total, total - len(messages)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
