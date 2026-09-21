# Copyright (c) Aregtech
#
# The wording run_scenarios.py prints when the project is an areg one. The runner
# itself names no framework; every remedy that names a file or a command of this
# one is here. A project built with another framework carries its own file of this
# name, and neither ever sees the other's words.
#
# "{tools}" in a value is the directory run_scenarios.py was started from.

# The sources a stale binary is compared against, beyond .cpp/.hpp/.h/.cc/.cxx.
SOURCE_SUFFIXES = ('.siml', '.fsml', '.dtml')

# The files that describe the project and whose edits outdate a build.
SPEC_FILES = ('bodies.txt', 'design.json')

# A stop whose trigger is still an open section.
FILL_HOLE = ('Fill it in bodies.txt with one line the lead prints while '
             '{proc} serves it')

# A lead that finishes before the stop can match.
HOLD_THE_LEAD = ('hold it there with a {"name": "...", "wait": <ms>} step in '
                 'design.json, right after the step whose output the stop matches')
HOLD_THE_LEAD_LATE = ('add a {"name": "...", "wait": 300} step in design.json '
                      'right after the step whose output the stop matches')

# Building.
BUILD_COMMAND = 'python3 {tools}/build_project.py'
BUILD_AND_RUN = 'Build and run in one call with build_project.py --spec <design> --run.'
NOTHING_TO_REBUILD = ('build_project.py --run would rebuild nothing and run the '
                      'same scenarios.')

# Markers left open.
FILL_MARKERS = ('Fill it with fill_markers.py --bodies, or write it in place and '
                'delete the line.\ncheck_contract.py reports the same markers as P-17.')
