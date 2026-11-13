# Fault injection: tests of failing system calls

This folder makes chosen system calls fail on request, so a test reaches the branch the
framework takes when the operating system refuses: no thread, no socket, no file, no
directory, no child process. The framework is not changed for it; the test calls the
public API, and the system call below it fails.

- [Goal and agenda](#goal-and-agenda)
- [What is here](#what-is-here)
- [How a call is replaced](#how-a-call-is-replaced)
- [Writing a test](#writing-a-test)
- [Adding a system call](#adding-a-system-call)
- [Building and running](#building-and-running)
- [Status per platform](#status-per-platform)
- [When a test says "did not replace"](#when-a-test-says-did-not-replace)
- [Planned tests](#planned-tests)

---

## Goal and agenda

A failure branch after a system call is kept for safety, even when the call almost never fails.
Coverage reports it as never run, and only two honest answers exist: reach it with a test, or
leave it red. This folder is how such a branch is reached:

1. **Real path.** The test calls the same public function an application calls. Only the
   system call at the bottom fails.
2. **No framework change.** No test-only hook, no exclusion marker, no `#ifdef` for tests.
3. **Same test on every platform.** One API, one test file; each platform replaces the call
   in its own way.
4. **Isolated.** Every fault is armed inside a child process (`units/ChildProcess.hpp`), so a
   failure that leaves the process unusable does not affect the next test.
5. **Proven.** Every test checks that the replaced call was really hit. A replacement that
   silently does not work fails the test; it never passes it.

---

## What is here

| File | Purpose |
|---|---|
| `FaultInject.hpp` | The API: `FaultPoint`, `arm()`, `failed()`, `FaultScope`; the C functions the library exports |
| `fault_inject.cpp` | The library `areg-fault-inject`: counters, and the replacements for Linux, macOS and Windows |
| `fault_inject_test.cpp` | The tests, executable `areg-fault-inject-test` |
| `fault_inject_win32_test.cpp` | The Windows tests: sockets, multiplexer wake-up pair, synchronization objects, files, timers, send backlog, service control manager |
| `FaultSocketPair.hpp` | Loopback socket pairs and listeners opened with plain system calls, Linux and Windows |
| `CMakeLists.txt` | Both targets; included from `tests/CMakeLists.txt` except on Cygwin |
| `areg-fault-inject.vcxproj`, `areg-fault-inject-test.vcxproj` (+ `.filters`), `packages.config` | The Visual Studio projects, in the `tests` folder of `areg-sdk.sln` |

The library does not link areg. The test executable links the library, areg and googletest.

### The API

```cpp
namespace areg_fault
{
    enum class FaultPoint { ThreadCreate, SocketCreate, FileOpen, DirOpen, ProcessSpawn, SocketConnect, SocketSend, SocketRecv, SocketBind, SocketListen, EpollCtl, FileRead, FileWrite, Allocation, AlignedAllocation, PipeCreate, Poll, TermGet, TermSet
                          , Fcntl, SocketGetOption, SocketSetOption, SocketAccept, HostName, EpollCreate, EpollWait, EventCreate
                          , MutexInit, MutexAttrInit, MutexAttrSetType, CondInit, CondAttrInit, CondAttrSetClock, ThreadAttrInit
                          , TimerFdCreate, TimerFdSet, SignalAction
                          , SocketInit, SocketName, SemaphoreCreate, WaitableTimerCreate, FileSeek, ServiceManagerOpen, ServiceOpen, ConsoleMode, Count };

    bool     arm( FaultPoint point, int error, uint32_t skip = 0u, uint32_t count = 1u );
    uint32_t failed( FaultPoint point );
    class    FaultScope;     // disarms every point when it goes out of scope
}
```

- `arm()`: the first `skip` calls pass, the next `count` calls fail with `error`, later calls
  pass again. `count = 0` fails every call until disarmed. Returns false if the point is not
  replaced on this platform.
- `failed()`: how many calls the point failed since it was armed.
- A disarmed point forwards every call to the system; its cost is one atomic load.

### The points

| Point | Linux | macOS | Windows | `error` means |
|---|---|---|---|---|
| `ThreadCreate` | `pthread_create` | `pthread_create` | `CreateThread` | return value (POSIX), `GetLastError()` (Windows) |
| `SocketCreate` | `socket` | `socket` | `socket` (ws2_32) | `errno` / `WSAGetLastError()` |
| `FileOpen` | `open`, `open64` | `open` | `CreateFileA`, `CreateFileW` | `errno` / `GetLastError()` |
| `DirOpen` | `opendir` | `opendir` | not replaced | `errno` |
| `ProcessSpawn` | `posix_spawnp` | `posix_spawnp` | not replaced | return value |
| `SocketConnect` | `connect` | not replaced | `connect` | `errno` / `WSAGetLastError()` |
| `SocketSend` | `send`, `sendmsg` | not replaced | `send`, `WSASend` | `errno` / `WSAGetLastError()` |
| `SocketRecv` | `recv` | not replaced | `recv` | `errno` / `WSAGetLastError()` |
| `SocketBind` | `bind` | not replaced | `bind` | `errno` / `WSAGetLastError()` |
| `SocketListen` | `listen` | not replaced | `listen` | `errno` / `WSAGetLastError()` |
| `EpollCtl` | `epoll_ctl` | not replaced | not replaced | `errno` |
| `FileRead` | `read` | not replaced | `ReadFile` | `errno` / `GetLastError()` |
| `FileWrite` | `write` | not replaced | `WriteFile` | `errno` / `GetLastError()` |
| `Allocation` | `operator new(std::nothrow)`, `operator new[](std::nothrow)` | not replaced | not replaced | ignored; the call returns null. The tests are skipped in sanitizer builds, whose runtimes define the operators |
| `AlignedAllocation` | aligned `operator new(std::nothrow)`, `operator new[](std::nothrow)` | not replaced | not replaced | ignored; the call returns null. Skipped in sanitizer builds, as `Allocation` |
| `PipeCreate` | `pipe` | not replaced | not replaced | `errno` |
| `Poll` | `poll` | not replaced | `WSAPoll`, `select` | `errno` / `WSAGetLastError()`. The AddressSanitizer interceptor does not reach the replacement, so its tests are skipped in that build. `HostName` (`gethostname`) behaves the same |
| `TermGet` | `tcgetattr` | not replaced | not replaced | `errno` |
| `TermSet` | `tcsetattr` | not replaced | not replaced | `errno` |
| `Fcntl`, `SocketGetOption`, `SocketSetOption`, `SocketAccept`, `HostName` | `fcntl` and `fcntl64`, `getsockopt`, `setsockopt`, `accept4`, `gethostname` | not replaced | `ioctlsocket`, `getsockopt`, `setsockopt`, `accept`, `gethostname` | `errno` / `WSAGetLastError()` |
| `EpollCreate`, `EpollWait`, `EventCreate` | `epoll_create1`, `epoll_wait`, `eventfd` | not replaced | `CreateIoCompletionPort`, `GetQueuedCompletionStatusEx`, `CreateEventW` | `errno` / `GetLastError()` |
| `MutexInit`, `MutexAttrInit`, `MutexAttrSetType`, `CondInit`, `CondAttrInit`, `CondAttrSetClock`, `ThreadAttrInit` | `pthread_mutex_init`, `pthread_mutexattr_init`, `pthread_mutexattr_settype`, `pthread_cond_init`, `pthread_condattr_init`, `pthread_condattr_setclock`, `pthread_attr_init` | not replaced | `MutexInit` only: `CreateMutexW` | return value / `GetLastError()` |
| `TimerFdCreate`, `TimerFdSet` | `timerfd_create`, `timerfd_settime` | not replaced | `TimerFdCreate` only: `CreateThreadpoolTimer`, the system timer of a `Timer` | `errno` / `GetLastError()` |
| `SignalAction` | `sigaction` | not replaced | not replaced | `errno` |
| `SocketInit`, `SocketName` | not replaced | not replaced | `WSAStartup`, `getsockname` | the returned error (`WSAStartup`) / `WSAGetLastError()` |
| `SemaphoreCreate`, `WaitableTimerCreate`, `FileSeek` | not replaced | not replaced | `CreateSemaphoreW`, `CreateWaitableTimerExW`, `SetFilePointer` | `GetLastError()` |
| `ServiceManagerOpen`, `ServiceOpen` | not replaced | not replaced | `OpenSCManagerW`, `OpenServiceW` | `GetLastError()` |
| `ConsoleMode` | not replaced | not replaced | `SetConsoleMode` | `GetLastError()` |

Use the error the system really reports in that situation, for example `EMFILE` or
`ERROR_TOO_MANY_OPEN_FILES` for an exhausted descriptor table. The tests keep one constant per
platform at the top of `fault_inject_test.cpp`.

`areg_fault_count_throwing_new( threadId )` (Linux) counts the calls of the throwing `operator new` and `new[]`
made by one thread, and `areg_fault_throwing_new_count()` returns the count. Such an allocation ends the process
when it fails, and no fault point can make it fail, so a test counts it and asserts 0.

`delay( FaultPoint::SocketListen, milliseconds )` makes `listen` wait instead of fail, until
disarmed, so a test can hold a thread at that call and check what is true meanwhile. It returns
false where the point is not replaced.

---

## How a call is replaced

**Linux.** The library defines the functions under their system names, with default visibility,
and the test executable links it. The dynamic linker searches the executable and then its
libraries in link order, and the C library comes last, so `libareg.so` (and a static areg in the
executable) binds to the library's definitions. A replacement that is not armed calls the next
definition, found with `dlsym(RTLD_NEXT, name)`: the C library, or a sanitizer interceptor. No
`LD_PRELOAD` is needed.

**macOS.** The library holds a `__DATA,__interpose` section: pairs of (replacement, replaced
function). dyld redirects every call of the replaced function in every other image to the
replacement; inside the library, the call reaches the original. The pairs are written with
`AREG_FAULT_INTERPOSE( replacement, replaced )`.

**Windows.** Arming the first point patches the import address tables. For every loaded module
that is not under the Windows directory and is not the fault library itself, every import slot
that holds the address of a replaced function gets the replacement. Slots are matched by
address, not by name, so an import through `kernel32.dll` or through an API set is found alike.
`areg.dll`, or the executable when areg is static, is patched this way. A call that a system
DLL makes internally is not replaced.

---

## Writing a test

```cpp
TEST( FaultInjectTest, file_open_fails_and_leaves_the_file_closed_when_the_system_refuses )
{
    AREG_EXPECT_IN_CHILD(
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileOpen, FILE_ERROR ) );

        areg::File file;
        AREG_CHILD_CHECK( file.open( "fault_inject_file.txt", mode ) == false );        // the reported failure
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::FileOpen ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( file.is_opened( ) == false );                                 // the state after it

        AREG_CHILD_CHECK( file.open( "fault_inject_file.txt", mode ) );                 // recovery
        file.close( );
    );
}
```

The checklist:

1. **Arm inside `AREG_EXPECT_IN_CHILD`.** Never before it: the code before the macro runs in the
   parent too (see [tests/README.md](../README.md#tests-in-a-child-process)).
2. **Hold a `FaultScope`** so the points are disarmed before the child's exit code runs.
3. **Check what areg reports**: the return value, the state of the object, the error the
   caller can read.
4. **Check `failed(point)`** with the `&& NOT_REPLACED` message, so a replacement that did not
   work reads as such in the output.
5. **Check the recovery**: the next call, once the fault is used up, succeeds. A failure that
   leaves the object unusable for good is a defect, unless the contract says so.
6. **Arm with `skip`** to fail a later call of the same kind, for example the second socket of a
   pair or the thread of a second dispatcher.
7. **Guard platform-only cases** with `#if !defined(_WIN32)` or `#if defined(__linux__)`, and
   only when the framework really calls something else on the other platforms. Check what it
   calls first, see below.
8. **No sleep and no timing.** A fault test is deterministic: the call fails or it does not.
9. **A body with a comma outside parentheses goes into a function**, as in
   `AREG_EXPECT_IN_CHILD( my_case( ) )`. A brace initializer such as `data[2]{ 1u, 2u }` splits
   the macro arguments under clang-cl; MSVC accepts it, so only the clang-cl build shows it.

Before writing a test, find the system call the framework really makes on each platform:

```bash
grep -rn '::socket\s*(' framework/areg --include=*.cpp       # the call sites
nm -D --undefined-only build/bin/libareg.so | grep -w open   # the symbol it imports (open64 here)
```

On Windows: `dumpbin /imports areg.dll`.

---

## Adding a system call

Example: make `bind()` fail.

1. **`FaultInject.hpp`:** add `SocketBind` to `FaultPoint` before `Count`, with a comment that
   names the replaced call per platform and what `error` means.
2. **Linux, `fault_inject.cpp`:** in `extern "C"`, define `bind` with the exact signature and
   exception specification of the glibc declaration (`socket` and `bind` are declared
   `noexcept` in C++; `open` and `opendir` are not). Fail with `_should_fail()`, otherwise call
   `_next<Func>("bind")`. Watch the header renames: `_FILE_OFFSET_BITS=64` turns `open` into
   `open64` (the file undefines it for this reason), and fortified headers add inline wrappers
   (the library builds with `-U_FORTIFY_SOURCE`). Replace every name the framework imports;
   check with `nm -D --undefined-only`.
3. **macOS:** add `_fault_bind` in the macOS block, calling `::bind` when not failing, and
   `AREG_FAULT_INTERPOSE( _fault_bind, bind );`.
4. **Windows:** add a function pointer type, a `g` original, a `WSAAPI`/`WINAPI` replacement
   that sets the error with `SetLastError` / `WSASetLastError`, and a `Hook` entry with the DLL
   and the export name. Add the point to `_is_supported()`.
5. **A point that one platform does not have** returns false from `_is_supported()` there, and
   its tests are guarded on that platform.
6. **Write the first test** that uses it, run it on Linux and Windows, and update the tables
   in this file.

---

## Building and running

Linux and macOS (CMake, `AREG_TESTS=ON`):

```bash
cmake --build build --target areg-fault-inject-test
ctest --test-dir build -R FaultInject --output-on-failure
./build/bin/areg-fault-inject-test.elf           # Linux; on macOS without .elf
```

Windows (Visual Studio): build `tests\areg-fault-inject-test` in `areg-sdk.sln`, or

```bat
msbuild areg-sdk.sln /m /t:tests\areg-fault-inject-test /p:Configuration=Release /p:Platform=x64
<build output>\bin\areg-fault-inject-test.exe
```

The CMake build on Windows defines the same targets. It is built with clang-cl and Ninja by `tools/intern/coverage.sh`.

---

## Status per platform

| Platform | State |
|---|---|
| Linux, GCC, Debug | Built and run: 4 of 4 pass, through ctest and directly, repeated. The console cases (`PipeCreate`, `Poll`, `TermGet`, `TermSet`, `FileRead`) pass in Release, in a coverage build, and with ThreadSanitizer; with AddressSanitizer the `Poll` case is skipped |
| Windows, MSVC 2022, Debug and Release x64 | Built through the `.vcxproj` files and run, all pass. The CMake build with clang-cl and coverage also runs them |
| macOS, AddressSanitizer | Built and run: 4 of 4 pass, the interposing pairs of the linked library apply |
| macOS, ThreadSanitizer | The library is built without instrumentation, as on every POSIX platform. The runtime opens its suppressions file through the replaced `open` while it initializes, before its thread state exists, and an instrumented replacement crashes there |
| Linux, GCC, Release, 32-bit x86 | Built and run: 4 of 4 pass |
| Linux, GCC, ThreadSanitizer, Debug and RelWithDebInfo | Built and run: all pass. The replacement sits behind the sanitizer interceptor (found through `RTLD_NEXT`) and is not instrumented: a blocking interceptor switches off the guard interceptors, so the first-use statics of the replacements read as races. The library is also built without link-time optimization, because GCC applies the `-fsanitize=thread` of the link line to LTO objects and ignores the `-fno-sanitize=thread` of their compile. The sanitizer creates its own helper thread through `pthread_create` at the first thread creation of a process, so a test that arms `ThreadCreate` starts and stops one thread first. The fork case of `fault_inject_posix_base_test.cpp` is skipped: the sanitizer stops a child forked from a multithreaded process when it starts a thread |
| Cygwin | Not built: Cygwin is neither ELF nor a native Win32 build, and no replacement is written for it. Patching the import tables of the Cygwin DLLs, as on Windows, is the way to add it |
| Coverage | A passing child exits through `std::exit(0)`, which writes its coverage data. Confirmed on Linux, GCC, Debug with `tools/intern/coverage.sh`: lines reached only in a child count. Confirmed on Windows, clang-cl: Codecov counts a line that only the `ThreadCreate` fault reaches. macOS not yet confirmed. A child that dies (a signal, `abort`) writes nothing |

Update this table when a platform is verified.

---

## When a test says "did not replace"

The output `child check failed: areg_fault::failed( ... ) == 1u && NOT_REPLACED` means areg
made the call but the replacement did not see it. Check, in this order:

1. **The imported name.** The framework may import a different symbol than the one replaced
   (`open64` for `open`, `opendir$INODE64` on an Intel Mac, `__open_2` from a fortified call,
   `WSASocketW` for `socket`). `nm -D --undefined-only` or `dumpbin /imports` shows it.
2. **Link order (Linux).** `readelf -d areg-fault-inject-test.elf | grep NEEDED` must list
   `libareg-fault-inject.so`. A static runtime or `-Bsymbolic` linking of the C library defeats
   the replacement.
3. **Interposing (macOS).** Run with `DYLD_INSERT_LIBRARIES=<path>/libareg-fault-inject.dylib`.
   System Integrity Protection removes `DYLD_*` variables when the process is started through
   a protected binary such as `/bin/sh`.
4. **The module (Windows).** The import must sit in a module outside the Windows directory.
   A call made inside a system DLL (for example `CreateThread` called by the CRT for
   `_beginthreadex`) is not replaced; replace the function the areg module imports.
5. **The call happened before arming**, or in the parent: arm inside the child, right before
   the call under test.
6. **A runtime made the call first.** A sanitizer runtime calls the replaced function itself,
   for example ThreadSanitizer creates a helper thread at the first `pthread_create`. That call
   takes the armed failure. Make the same call once before arming.
7. **The compiler removed the allocation.** A `new (std::nothrow)` whose memory the test never
   reads may be omitted by an optimizing build, which then treats it as successful; the call is
   missing from `objdump -dC` of the test function. Which allocation goes depends on the inlining
   around it (a different googletest build is enough). Pass the object to `_keep_allocation()`
   (`fault_inject_alloc_sweep_test.cpp`) right after the call and before its check.

---

## Planned tests

Each line is a failure branch that is reached with the points that exist, or with one new
point.

| Test | Point | Platforms |
|---|---|---|
| A full descriptor table (`RLIMIT_NOFILE` lowered in the child) at router accept and client connect | none, a resource limit | Linux, macOS |
| The second socket of the Windows multiplexer wake-up pair fails | `SocketCreate`, `skip = 1` | Windows done |
| A dispatcher thread or the timer service thread cannot start | `ThreadCreate` | all |
| The log file of the file logger cannot be opened | `FileOpen` | all |
| `launchctl` cannot be spawned for the router service | `ProcessSpawn` | macOS |
| `bind()` and `listen()` fail in the router | new `SocketBind`, `SocketListen` | all |
| `connect()` fails with `ECONNREFUSED` / `WSAECONNREFUSED` at once | `SocketConnect` | Linux, Windows done; macOS open |
| `send()` / `recv()` return an error mid-message | `SocketSend`, `SocketRecv` | Linux, Windows done; macOS open |
| Memory allocation fails in a `new (std::nothrow)` path | `Allocation` | Linux done (no sanitizer); macOS, Windows open. On Windows `operator new` is linked into each module and calls `malloc` of the C runtime, so failing it means failing every allocation of the module |
| The console of `aregextend` cannot create its interrupt pipe, read or set the terminal modes, or wait for or read a key | `PipeCreate`, `TermGet`, `TermSet`, `Poll`, `FileRead` | Linux done; macOS open |
