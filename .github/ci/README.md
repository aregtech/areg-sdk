# CI scheduling and caches

GitHub Actions has no job-priority setting. Matrix order controls job creation,
not guaranteed execution order when workflows share runner limits.

`cmake-matrix.json` remains the configuration source: pull requests and manual
runs select all 26 configurations; master selects the seven marked `master: true`.
The planner partitions them exactly once:

| Group | Selection | Maximum simultaneous jobs |
|---|---|---:|
| Test-running | `test: ON`, without `norun: ON` | 8 |
| Linux build-only | Remaining `ubuntu-latest` configurations | 2 |
| Windows build-only | Remaining `windows-latest` configurations | 2 |

These are resource caps, not a global priority guarantee. They limit CMake's
interference with Coverage, Sanitizers, CodeQL and agent integration builds,
without waiting for another workflow to finish. A cap can delay an individual
CMake configuration; compare total elapsed time across workflows, not only CMake.
The former admission pollers occupied two runners while waiting and coordinated
only CMake. They and their API permission are removed.

All actual build configurations, unit tests, examples, benchmarks, stress
repetitions, artifacts and existing check names remain. Only the two artificial
waiting jobs disappear. Build steps still share a YAML anchor; `CMake result`
covers every workload group. Unknown build-only runner labels fail planning
instead of silently dropping a configuration.

CodeQL cancels superseded pull-request runs; scheduled, manual and master scans
remain independent. Analysis has a 90-minute bound and still traces the complete
default build. No compiler cache is used in CodeQL.

## Compiler and download caches

The local `compiler-cache` action installs ccache for GCC/Clang Makefile and Ninja
builds on Linux and macOS. It is used by CMake's test-running configurations,
Coverage, Sanitizers, the x86/mixed-bitness build and Linux agent integrations.
Visual Studio builds and native Windows/Cygwin compiler outputs are not cached:
Visual Studio does not use CMake's compiler-launcher interface, and Cygwin's
packaged ccache is an older implementation not enabled for coverage.

Each workflow/configuration has a separate cache prefix including OS and runner
architecture, a 512 MB limit and a new snapshot key per run/attempt. An earlier
snapshot is restored on a subsequent run. ccache checks compiler content, flags
and headers; directory hashing remains enabled. No sloppiness, debug-path
rewriting or compiler masquerading is enabled. Existing instrumentation,
optimization flags and build generators are unchanged.

Only ccache's compiler outputs are saved, not CMake build trees, executables,
test results or `.gcda` execution counters. Coverage still starts in a fresh
tree and resets counters between unit tests and examples. Compilation cache
statistics are logged in CMake, Coverage, Sanitizers and Platforms jobs.
GitHub's repository-wide cache quota and eviction still apply; a miss performs
the normal complete build. A full Debug build with tests and examples compiles
610 objects, 913 MB raw and about 236 MB compressed, so the snapshots of one run
take roughly 5-6.5 GB of the 10 GB quota. Eviction is least recently used: the
latest snapshot of each configuration survives until the next run, while older
snapshots and CodeQL's TRAP caches are evicted first. Cold installation/transfer overhead and warm-hit
benefits must be measured on hosted runners, not assumed.

Cygwin Coverage caches downloaded package archives, not an installed Cygwin root.
The installer still refreshes package metadata and verifies archive checksums.

## Path decisions and failure evidence

All eight path planners call `tools/intern/ci-changes.sh`. It completes the Git
diff before matching paths, so an early match cannot make Git fail with SIGPIPE
under Bash `pipefail`. Diff and expression errors fail the planner, rather than
returning a successful skip. Agent integrations include root/nested CMake files
and third-party build dependencies.

Unit tests in CMake and Platforms run after a successful build even if the
examples fail. CMake benchmarks and Platforms' mixed-bitness check are also
independently guarded by their prerequisite builds. Failures still fail CI.
Tests remain serial and stress counts are unchanged; parallel execution requires
a separate shared-file/port isolation audit.

Run the planner regressions from the repository root:

```text
python tests/units/test_ci_plans.py
```

They execute the actual planner shell fragments against large relevant and
irrelevant diffs, failed diffs, invalid expressions and manual events. CMake's
invariant job runs them too.

## Coverage tool setup

Cygwin uses Python 3.12 and `python312-lxml=6.1.3-1`. The later `6.1.3-2` binary
imports `cygxml2-16.dll`, which the current stable libxml2 packages do not supply;
the pinned binary imports the available `cygxml2-2.dll`. The workflow verifies
the import, and the coverage regression suite checks XML/XSLT operations.
Revisit the pin only with a working, ABI-compatible package set.

gcovr remains pinned through `GCOVR_VERSION`. Pip's `--only-binary=lxml` accepts
the prebuilt Cygwin package or another platform's wheel, never a source build.
Python/libxml2/libxslt development headers are not requested.

The coverage driver converts Git Bash paths to native Windows drive paths before
passing them to Python, including history below `HOME`. Cygwin and other POSIX
paths remain unchanged. `tests/units/test_coverage_paths.py` checks default,
environment and explicit history directories. It is a local check of the driver;
no workflow runs it, so it can never stop a coverage job before the build.
