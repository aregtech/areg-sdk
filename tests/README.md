# Areg SDK tests

This folder holds every automated test of the areg framework: the unit tests, the tests
that need a process of their own, the failure-path tests, and the harnesses a person
or a script drives. This page says what each project is for, how to run it, and how to
add a test so that it gives the expected result on Linux, macOS and Windows.

- [Kinds of tests](#kinds-of-tests)
- [The projects](#the-projects)
- [Running the tests](#running-the-tests)
- [Rules every test follows](#rules-every-test-follows)
- [Tests in a child process](#tests-in-a-child-process)
- [Tests of failing system calls](#tests-of-failing-system-calls)
- [Adding a test](#adding-a-test)

---

## Kinds of tests

| Kind | Where | When to use it |
|---|---|---|
| Unit test | `units/`, one executable `areg-unit-tests` | A class or a function, with no process-wide state, or with process-wide state run in a child process |
| Child-process unit test | `units/`, with `units/ChildProcess.hpp` | A case that sets up the application, ends the process, or leaves the process unusable |
| Own executable | one folder per test, e.g. `release-unload/` | A whole application lifecycle that needs its own `main()`, generated service code, or a fake router |
| Failure-path test | `fault-inject/` | The branch taken when a system call fails: thread, socket, file, directory, process spawn |
| Harness, not in ctest | `bitness/`, `conn-scenario/`, `keepalive-probe/`, `benchmarks/` | Needs a router, root rights, a packet filter, two builds or minutes of run time; started by a script or a person |

`Application::setup()` and `Application::release()` act on the whole process, and the
configuration is read once per process. A test that needs its own configuration calls
`ConfigManager::release_properties()` around the setup, or runs in a child process or in
an executable of its own.

---

## The projects

Built with `-DAREG_TESTS=ON` (the default). "ctest" says whether `ctest` runs it.

| Project | Checks | Built when | ctest |
|---|---|---|---|
| `units/` -> `areg-unit-tests` | Classes and functions of `areg`, the logging runtime, the log database wrappers, the child-process template | always | yes, one test per case |
| `fault-inject/` -> `areg-fault-inject-test` | How areg reports a failed system call (see [its README](fault-inject/README.md)) | not on Cygwin | yes |
| `release-unload/` | `Application::release()` without `unload_model()` joins the component threads and deletes the components | always | yes |
| `timer-churn/` | Timers and the watchdog armed, restarted and destroyed from several threads while firing, then teardown | always | yes |
| `internal-events/` | Internal events sent outside a dispatch and from foreign threads | always | yes |
| `stub-teardown/` | Service events routed from a foreign thread while the target is loaded, unloaded, deleted, created | always | yes |
| `router-io/` | The router service in the test process, driven by raw TCP clients, per case and pool size | always | yes |
| `client-stop/` | A router client stopped in every way an application can, against a fake router | always | yes |
| `client-link/` | A client built without LTO links and finds every devirtualized target | shared `areg` only | yes |
| `fatal-create/` | A lock the system cannot create ends the process through `areg::fatal_error()` | Linux only | yes |
| `proxy-churn/`, `proxy-attach/` | Service clients created and deleted while the service goes up and down; attaching a client | examples on (uses example 01) | yes |
| `request-failure/`, `same-thread/`, `system-lifecycle/` | Unanswerable requests; provider and consumer in one thread; load, unload, watchdog, start and stop deadlines | examples on (uses example 02) | yes |
| `foreign-calls/`, `worker-proxy/` | Calls from foreign threads; a proxy in a worker thread | examples on (uses example 33) | yes |
| `request-error/` | A provider error reaches only the caller, as `request_X_failed` | examples on (uses example 24) | yes |
| `consumer-delete/` | A client deletes another client of the same proxy, or itself, in a callback | examples on (uses example 25) | yes |
| `conn-scenario/` | Sequence gaps, stalls and damaged blocks while a process pauses or packets drop; `conn_scenario.py` | examples on | no |
| `bitness/` | A 32-bit and a 64-bit build exchange every data kind; `bitness.sh` / `bitness.ps1` | always | no |
| `keepalive-probe/` | How fast a dead peer is noticed; `keepalive_probe.py`, needs root | always | no |
| `benchmarks/` | Micro benchmarks on google_benchmark / CodSpeed (see [its README](benchmarks/README.md)) | `-DAREG_BENCHMARKS=ON` | no |

---

## Running the tests

Linux and macOS, from the build folder:

```bash
ctest --output-on-failure -j"$(nproc)"          # everything registered
ctest -R 'FaultInject|ChildProcess'             # by name
./bin/areg-unit-tests.elf --gtest_filter='FileTest.*'
```

`ctest` runs each case in its own process. `areg-unit-tests` also runs as a whole in one
process, which is what the Test Explorer of Visual Studio does.

The CI regressions use Python's standard `unittest` and Bash, independently of the
C++ build. The coverage dependency case on Cygwin also requires its prebuilt lxml:

```text
python tests/units/test_coverage_paths.py
python tests/units/test_ci_plans.py
```

They run the report phase with fixture gcovr outputs and the real report writer.
Coverage runs the path/dependency cases on every platform before building,
including Git Bash launching native Windows Python. The planner cases execute
the actual workflow filters with large diffs and explicit error conditions;
CMake's invariant job runs them.

Windows, Visual Studio: build `areg-sdk.sln`; the executables are in the `bin` folder of the
build output (for example `product\build\msvc-v143\windows_nt-x64-release\bin`). From a
command line, the solution targets carry the solution folder:

```bat
msbuild areg-sdk.sln /m /t:tests\areg-fault-inject-test;tests\units\areg-unit-tests /p:Configuration=Release /p:Platform=x64
```

---

## Rules every test follows

- **Independent of machine speed.** No sleep to wait for something, no latency limit in an
  assertion. Wait on the event that proves the state (a sync event, a condition, a counter),
  with a generous deadline only as a hang guard. A slow or loaded CI runner must not fail it.
- **A bug fix comes with a test that reproduces it.** Red on the code before the fix, green
  after it. Its comment starts with `Reproduces the defect:`. If the defect is unreachable from
  a test, extract a seam in the framework instead of skipping the test.
- **No exclusion markers.** A line that coverage reports as never run is reached by a test or
  stays red. Failure branches of system calls are reached with
  [fault injection](#tests-of-failing-system-calls).
- **The good case is the default.** Stress, overload and failure are explicit cases, each with
  a control case that passes on the same machine.
- **One case, one fact.** The test name says the behaviour, e.g.
  `file_open_fails_and_leaves_the_file_closed_when_the_system_refuses`.
- **Code is ASCII; comments say what, not why.** As in the framework.

---

## Tests in a child process

`units/ChildProcess.hpp` runs a test body in a new process started from the test executable.
It uses the death tests of googletest, so it works the same on Linux, macOS and Windows and
needs no platform code.

```cpp
#include "units/ChildProcess.hpp"

TEST( MySuite, application_setup_in_a_child )
{
    AREG_EXPECT_IN_CHILD(
        areg::Application::setup( false, false, false, true, false );
        AREG_CHILD_CHECK( areg::Application::system_state( ) == areg::AppState::Ready );
        areg::Application::release( );
    );
}

TEST( MySuite, fatal_path_ends_the_process )
{
    AREG_EXPECT_CHILD_DIES( areg::fatal_error( "reason" ), "areg fatal error: reason" );
}
```

| Macro | Passes when |
|---|---|
| `AREG_EXPECT_IN_CHILD( statements )` | the statements return and the child exits with 0 |
| `AREG_CHILD_CHECK( condition )` | (inside a child) the condition is true; otherwise the child prints `child check failed: <condition> (file:line)` and exits with 97 |
| `AREG_EXPECT_CHILD_DIES( statements, pattern )` | the child ends abnormally and its standard error output matches the pattern |

What a test author must know:

1. **The child runs the test body from its start.** The child is the same executable started
   again with a filter for the current test, so every line before the macro runs in the parent
   and again in the child. Put state that belongs to the child inside the macro. A value the
   parent computed before the macro is computed again by the child, not inherited.
2. **Use `AREG_CHILD_CHECK` inside the child.** A googletest `EXPECT_*` or `ASSERT_*` in the
   child does not reach the parent's result. When a check fails, the parent prints the child's
   output, which holds the failed condition.
3. **A passing child exits through `std::exit(0)`**, so the coverage data of the child is
   written. A failing check exits through `std::_Exit`, because the process may be broken.
4. **Every child is a fresh process.** It may set up and release the application, arm a fault,
   exhaust a resource or crash, and the next test is not affected.
5. **The cost is a process start per child**, a few milliseconds on Linux and tens of
   milliseconds on Windows. Use a child only when the case needs one.

The template's own tests are in `units/ChildProcessTest.cpp`.

---

## Tests of failing system calls

A branch such as

```cpp
DIR* const dir{ ::opendir("/proc") };
if (dir == nullptr)
    return false;
```

is right to keep and hard to reach: the call almost never fails on a test machine.
`fault-inject/` makes chosen system calls fail on request inside a child process, so such a
branch is reached by a real call of the framework code, on Linux, macOS and Windows, without
changing the framework. How it works, how to write a test and how to add a new system call:
[fault-inject/README.md](fault-inject/README.md).

---

## Adding a test

### A unit test

1. Add `units/<Name>Test.cpp`. Include `units/GUnitTest.hpp` (or `units/ChildProcess.hpp`).
2. Add the file to `macro_add_unit_test(...)` in `units/CMakeLists.txt`.
3. Add it to `areg-unit-tests.vcxproj` (`<ClCompile Include="units\<Name>Test.cpp" />`) and to
   `areg-unit-tests.vcxproj.filters` (filter `Source Files`).
4. Build and run it with `ctest -R <Suite>` on Linux; build the solution on Windows.

### A test that needs its own executable

1. Create `tests/<name>/` with the source, a `CMakeLists.txt` that calls `addExecutableEx`
   and `add_test` (or `gtest_discover_tests`), and a header comment that says what it checks.
2. Include it from `tests/CMakeLists.txt`, under the condition it needs (examples on, Linux
   only, shared library only), with one comment line that names the condition.
3. Add `<name>.vcxproj` and `.filters` beside it, a project entry in `areg-sdk.sln` in the
   `tests` solution folder, and the four `Debug|Release x Win32|x64` configuration lines.
   A googletest executable also needs `packages.config` and the googletest import, as in
   `fault-inject/areg-fault-inject-test.vcxproj`.
4. Add a row to [the projects](#the-projects) table above.

### A failure-path test

See [fault-inject/README.md](fault-inject/README.md#writing-a-test).
