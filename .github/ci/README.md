# CI scheduling

GitHub Actions does not provide a job-priority setting. Matrix order determines
creation order, not a guaranteed execution order when workflows share runner limits.

The CMake workflow admits the legs that run tests or benchmarks before its
build-only legs. `cmake-matrix.json` remains the single list of configurations:
pull requests and manual runs select every leg, and pushes to master select those
marked `master: true`. No build flags, test commands, artifacts or existing check
names change.

The plan partitions that selection into:

- Priority: `test: ON` and `norun` is not `ON`.
- Linux build-only: the other legs on `ubuntu-latest`.
- Windows build-only: the other legs on `windows-latest`.

Two admission jobs check the current workflow run through the read-only Actions
API. Each releases its build-only group once every selected priority leg on the
same runner label is running or completed. They do not wait for tests to finish.
Linux admission does not wait for Windows or macOS runner capacity. Sanitizers,
coverage, agent documentation, CodeQL and the standalone benchmarks remain
independently eligible; no cross-workflow completion barrier is added.

Each admission job uses one Linux runner while waiting, polls every 15 seconds,
retries transient API errors, and fails explicitly after an hour or an unrecovered
API error. Build-only jobs still run
after an admission failure, but `CMake result` reports that failure. Cancellation
does not release further builds. Re-run attempts bypass admission so retrying
only failed jobs cannot wait forever for successful jobs from an earlier attempt.

The build steps are shared through a YAML anchor. The aggregate `CMake result`
includes both build-only groups and both admission jobs. The existing path filter,
master subset and `fail-fast: false` behavior are preserved. Adding a build-only
runner label requires its own admission/build pair; the plan rejects an
unhandled label rather than omitting that configuration.

Run the admission regression cases from the repository root:

```text
node --test tools/intern/ci-admission.test.js
```

This prioritizes CMake's test-running legs over its build-only legs, not every job
in the account. Runner availability and other workflows still affect start times;
any wall-clock improvement must be measured on subsequent GitHub-hosted runs.

## Coverage tool setup

Cygwin installs [`python3-lxml`](https://cygwin.com/packages/summary/python3-lxml.html)
alongside `python3` and `python3-pip`. These packages select the same default Cygwin
Python version. The workflow imports `lxml.etree` before installing gcovr to check
the compiled module and its runtime libraries before the instrumented build.

gcovr remains pinned through `GCOVR_VERSION` and installed by pip on every platform.
`--only-binary=lxml` accepts the installed Cygwin package or a compatible wheel on
the other platforms, but never compiles lxml from source. The Cygwin setup no longer
requests the Python, libxml2 or libxslt development headers used by that compilation.
Coverage flags, test selection, counter resets, reports and uploads are unchanged.

The coverage driver converts Git Bash paths to native Windows drive paths before
passing them to Python, including the default history under `HOME`. Cygwin and
other POSIX environments retain their own paths. `tests/units/test_coverage_paths.py`
exercises history saving with default, environment and explicit work directories;
the Coverage workflow runs it on all four platforms before the instrumented build.
