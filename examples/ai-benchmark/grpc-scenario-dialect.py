# No copyright banner and no owner's name: this file is copied into the gRPC arm's
# sandbox, which is a cold start, and a banner there would name the checkout it
# came from.
#
# The wording run_scenarios.py prints when the project is a gRPC one. It is staged
# beside the runner as scenario_dialect.py. The runner itself names no framework,
# and this file names no framework but gRPC: the two arms of the benchmark are only
# comparable while neither is told the other exists.
#
# "{tools}" in a value is the directory run_scenarios.py was started from.

# The sources a stale binary is compared against, beyond .cpp/.hpp/.h/.cc/.cxx.
SOURCE_SUFFIXES = ('.proto',)

# The files that describe the project and whose edits outdate a build.
SPEC_FILES = ('CMakeLists.txt',)

# A stop whose trigger is still an open section.
FILL_HOLE = ('Print one line from the code that serves it, while {proc} is serving')

# A lead that finishes before the stop can match.
HOLD_THE_LEAD = ('hold it there until the stop can match: the lead must still be '
                 'running after the line the stop names has been printed')
HOLD_THE_LEAD_LATE = ('make the lead wait there, right after the output the stop '
                      'matches')

# Building.
BUILD_COMMAND = 'cmake --build build -j8'
BUILD_AND_RUN = 'Build again, then run this.'
NOTHING_TO_REBUILD = 'another build would rebuild nothing and run the same scenarios.'

# Markers left open.
FILL_MARKERS = 'Write the code in place of the marker and delete its line.'
