#!/usr/bin/env python3
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ===========================================================================
"""Turns on the file log and the framework scopes of a shipped areg.init.

TEMPORARY, bug-fixing phase. Delete together with the defects it was written for.

The shipped configuration writes no log file at all ('log::*::enable::file = false')
and silences every scope of the framework itself ('log::*::scope::areg_* = NOTSET').
Both are right for a release and wrong for a hunt: the scopes that say how far a run
got -- the watchdog expiry, the termination and the recreation of a component thread,
the registration of a service provider, the connection of a proxy -- are framework
scopes, and they have to reach a file to survive a process that is killed.

    enable-framework-logs.py <path to areg.init> [...] [--all]

By default only the scopes of the subsystems that decide whether a component thread was
recreated and whether a provider and a consumer found each other again are switched on.
Switching on every framework scope instead ('--all') costs a log record per dispatched
event, which is enough to change the timing of a race, so it is not the default.

An application still has to call LOGGING_CONFIGURE_AND_START for any of this to apply.
Nothing here may fail a build: a missing file is reported and is not an error.
"""

import os
import re
import sys


#!< The scope groups that carry the restart of a component and the reconnection of the
#!< proxy that follows it. Everything a watchdog failure passes through is in this list,
#!< and nothing that is merely traffic is.
#!<
#!< A scope is named '<path>.<method>', and a group pattern is honoured only when what
#!< stands before the '*' ends in '_' or '.' (ScopeController::set_group_priority). A
#!< pattern such as 'areg_component_ProxyBase*' is silently ignored, which is why every
#!< entry below ends in '.*' or '_*'.
#!<
#!< 'areg_ipc_*' and the dispatcher were in this list until run 1501 and are deliberately
#!< out of it now. They produce a record per dispatched event, and the job that used them
#!< has failed to reproduce the macOS hang twice while the matrix jobs, which log nothing,
#!< reproduced it eight times out of eight. What is left is one record per restart, not
#!< per message.
TARGETED_SCOPES = [
    'areg_component_private_WatchdogManager.*',
    'areg_component_private_ServiceManager.*',
    'areg_component_private_ServiceManagerEventProcessor.*',
    'areg_component_private_ServerList.*',
    'areg_component_ProxyBase.*',
    'areg_component_StubBase.*',
    'areg_ipc_private_ServiceClientConnectionBase.*',
    'examples_21_*',
    'examples_22_*',
]


def settings(everything):
    """Returns the lines the configuration has to end up with."""
    lines = [(r'^\s*log::\*::enable::file\s*=.*$', 'log::*::enable::file        = true')]
    if everything:
        lines.append((r'^\s*log::\*::scope::areg_\*\s*=.*$',
                      'log::*::scope::areg_*       = DEBUG | SCOPE ;'))
    else:
        # The catch-all stays silent; the groups below are switched on one by one.
        lines.append((r'^\s*log::\*::scope::areg_\*\s*=.*$',
                      'log::*::scope::areg_*       = NOTSET ;\n'
                      + '\n'.join('log::*::scope::%s = DEBUG | SCOPE ;' % scope
                                  for scope in TARGETED_SCOPES)))
    return lines


def patch(path, everything):
    """Rewrites one configuration file. Returns True when it was changed."""
    if not os.path.isfile(path):
        print('::warning title=framework-logs::no %s' % path)
        return False

    with open(path, 'r') as source:
        text = source.read()

    for pattern, replacement in settings(everything):
        text, count = re.subn(pattern, replacement.replace('\\', '\\\\'), text, flags=re.M)
        if count == 0:
            text = text.rstrip('\n') + '\n' + replacement + '\n'

    with open(path, 'w') as target:
        target.write(text)

    print('framework logging enabled in %s' % path)
    for line in text.splitlines():
        if line.startswith('log::') and (('enable::file' in line) or ('scope::areg_' in line)):
            print('    %s' % line)
    return True


def main():
    paths = [arg for arg in sys.argv[1:] if arg != '--all']
    if not paths:
        print(__doc__)
        return 1
    for path in paths:
        patch(path, '--all' in sys.argv[1:])
    return 0


if __name__ == '__main__':
    sys.exit(main())
