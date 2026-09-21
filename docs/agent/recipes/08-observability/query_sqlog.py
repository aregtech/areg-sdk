#!/usr/bin/env python3
# -*- coding: ascii -*-
"""
Reads the .sqlog database logcollector writes.

A .sqlog file is an SQLite database. Nothing special is needed to read it: the
'logs' table holds one row per log record, and the two columns that answer most
questions are 'msg_module', the process the record came from, and 'msg_log', the
text. The scope name is part of the text.

Not every row came from an application. The collector writes rows of its own --
the header row it opens the file with, and a note for every instance that
connects or disconnects -- and a viewer can store a marker. Those rows carry the
PrioIgnore bit (0x0400) in 'msg_prio' and no scope, they belong to no process
under test, and they are left out unless '--collector' asks for them. Filtering
them in SQL is 'where (msg_prio & 1024) = 0'.

Produce one, from this recipe's directory:

    mtrouter --service &
    logcollector --service --log=db ./collected.sqlog &
    ./provider &
    ./consumer

On Windows the same three programs are mtrouter.exe, logcollector.exe and the two
built binaries; start each in its own console instead of with '&'.

Then read it:

    python3 query_sqlog.py collected.sqlog                  every application row
    python3 query_sqlog.py collected.sqlog --module provider   one process
    python3 query_sqlog.py collected.sqlog --scope service_connected
    python3 query_sqlog.py collected.sqlog --collector      the collector's rows too

Exit code 0 when the database holds at least one application row, 1 when it holds
none, which is the failure that matters: the collector ran and nothing reached it.
"""

import argparse
import sqlite3
import sys

# The bit the collector sets on the rows it writes itself. No application
# priority carries it: NOTSET, SCOPE, FATAL, ERROR, WARNING, INFO and DEBUG are
# all below 0x0400.
PRIO_IGNORE = 0x0400

# Every column, named by none of them: a file written by an older collector is
# missing the ones added since, and a reader that lists columns fails on it.
LOGS_QUERY = 'select * from logs order by time_created, id'


def is_collector_row(row):
    """True when the collector wrote the row itself rather than an application."""
    return ((row['msg_prio'] or 0) & PRIO_IGNORE) != 0


def all_rows(database):
    """Every row of the logs table, as dictionaries, in the order it happened."""
    connection = sqlite3.connect(database)
    try:
        connection.row_factory = sqlite3.Row
        return [dict(row) for row in connection.execute(LOGS_QUERY)]
    finally:
        connection.close()


def rows(database, module=None, scope=None, collector=False):
    """The log rows of the database as (process, text), filtered as asked."""
    found = [row for row in all_rows(database)
             if collector or not is_collector_row(row)]

    if module is not None:
        found = [row for row in found if module in (row['msg_module'] or '')]
    if scope is not None:
        found = [row for row in found if scope in (row['msg_log'] or '')]
    return [(row['msg_module'], row['msg_log']) for row in found]


def modules_and_messages(database):
    """The two columns as separate lists, in row order, applications only."""
    found = rows(database)
    return [row[0] for row in found], [row[1] for row in found]


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[1])
    parser.add_argument('database', help='the .sqlog file logcollector wrote')
    parser.add_argument('--module', help='only rows from this process')
    parser.add_argument('--scope', help='only rows whose text holds this scope')
    parser.add_argument('--collector', action='store_true',
                        help="include the collector's own rows")
    args = parser.parse_args()

    found = rows(args.database, args.module, args.scope, args.collector)
    for module, message in found:
        print('{:<12} {}'.format(module or '?', message or ''))
    print('{} row(s)'.format(len(found)))
    return 0 if found else 1


if __name__ == '__main__':
    sys.exit(main())
