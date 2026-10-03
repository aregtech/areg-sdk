#!/usr/bin/env python3
"""The configuration a run actually built, as one line for meta.txt.

CMAKE_BUILD_TYPE does not answer this. A multi-config generator ignores it and builds
Debug unless the build step names a configuration, and areg applies its own
optimisation flags whether or not it is set. A run compared against another that built
differently measures the configuration and not the corpus, so the line is recorded.

    python3 build_config.py <run-directory>
"""

import os
import re
import sys

CONFIGS = ('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')


def cache_value(cache, key):
    """One CMakeCache.txt entry, or '' when it is absent or empty."""
    try:
        with open(cache, encoding='utf-8', errors='replace') as handle:
            for line in handle:
                if line.startswith(key + ':'):
                    return line.split('=', 1)[1].strip() if '=' in line else ''
    except OSError:
        return ''
    return ''


def config_dirs(build):
    """The per-configuration output directories a multi-config generator wrote."""
    found = set()
    for base, dirs, _ in os.walk(build):
        if base.count(os.sep) - build.count(os.sep) > 2:
            dirs[:] = []
            continue
        found.update(name for name in dirs if name in CONFIGS)
    return sorted(found)


def optimised(build):
    """Whether the compiler flags of this build carry NDEBUG, or None if unreadable."""
    for base, _, files in os.walk(build):
        if 'flags.make' not in files:
            continue
        try:
            with open(os.path.join(base, 'flags.make'), encoding='utf-8',
                      errors='replace') as handle:
                text = handle.read()
        except OSError:
            continue
        if re.search(r'^\s*CXX_FLAGS\s*=', text, re.M):
            return 'NDEBUG' in text
    return None


def describe(build):
    """One line naming what this build directory was compiled as."""
    cache = os.path.join(build, 'CMakeCache.txt')
    if not os.path.isfile(cache):
        return 'unknown, no CMakeCache.txt'
    generator = cache_value(cache, 'CMAKE_GENERATOR') or 'unknown generator'
    if cache_value(cache, 'CMAKE_CONFIGURATION_TYPES'):
        built = config_dirs(build)
        return '{}, multi-config {}'.format(
            ','.join(built) if built else 'not built', generator)
    named = cache_value(cache, 'CMAKE_BUILD_TYPE')
    flags = optimised(build)
    said = named or 'CMAKE_BUILD_TYPE unset'
    if flags is not None:
        said += ', {}'.format('optimised (NDEBUG)' if flags else 'unoptimised')
    return '{}, {}'.format(said, generator)


def main(argv):
    if len(argv) != 2:
        sys.stderr.write(__doc__)
        return 2
    build = os.path.join(argv[1], 'work', 'build')
    if not os.path.isdir(build):
        build = argv[1]
    print(describe(build))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
