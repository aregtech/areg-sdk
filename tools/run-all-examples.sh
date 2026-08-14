#!/usr/bin/env bash
# ===========================================================================
#  This file is part of the Areg SDK
#  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
#  Contact: info[at]areg.tech
#  Website: https://www.areg.tech
# ---------------------------------------------------------------------------
#  Areg SDK -- run the example applications as a test suite (Linux, macOS).
#
#  Thin wrapper around 'tools/run-all-examples.py'. Every argument is passed
#  through, so the options of the driver are available here as well:
#
#      tools/run-all-examples.sh                       # default set
#      tools/run-all-examples.sh --tier smoke          # single process only
#      tools/run-all-examples.sh --only 12_svcmulti --repeat 20
#      tools/run-all-examples.sh --list
#
#  Run it after the build and before the unit tests. Exit code 0 means every
#  scenario passed.
# ===========================================================================

set -u

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DRIVER="${SCRIPT_DIR}/run-all-examples.py"

if [ ! -f "${DRIVER}" ]; then
    echo "[error] driver not found: ${DRIVER}" >&2
    exit 1
fi

PYTHON=""
for candidate in python3 python; do
    if command -v "${candidate}" >/dev/null 2>&1; then
        if "${candidate}" -c 'import sys; sys.exit(0 if sys.version_info >= (3, 6) else 1)' 2>/dev/null; then
            PYTHON="${candidate}"
            break
        fi
    fi
done

if [ -z "${PYTHON}" ]; then
    echo "[error] Python 3.6 or newer is required to run the example test suite." >&2
    exit 1
fi

exec "${PYTHON}" "${DRIVER}" "$@"
