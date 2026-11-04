# areg-sdk – Code coverage exclusions
## Which framework lines are left out of the coverage count, and why

Coverage counts a line only when a test runs it. A few lines can never run in a test, and the
framework marks them with `GCOVR_EXCL_START` ... `GCOVR_EXCL_STOP`. Logic is never excluded: a
line that a test can reach, with a fault injected if needed, stays in the count.

The share of excluded lines is kept below 1.5 % of the `framework/areg` lines.

---

## 1. What is excluded

A `default:` arm of a `switch` over an enumeration, whose only statement is `ASSERT(false)`. Every
enumerator has its own `case`, so the arm runs only when a caller passes a value outside the
enumeration, which the API forbids.

| File | Block | Platform |
|---|---|---|
| `ipc/private/ServiceEventConsumer.cpp` | two `default` arms of the client and server command switches | all |
| `ipc/private/RouterClient.cpp` | `default` arm of the registration notification switch | all |
| `logging/private/LoggingEvent.hpp` | `default` arm of `LoggingEventData::as_string()` | all |
| `logging/private/NetTcpLogger.cpp` | `default` arm of the received message switch | all |
| `logging/private/FileLogger.cpp` | `default` arm of the logged message type switch | all |
| `appbase/private/Application.cpp` | `default` arm of the application state switch | all |

## 2. What is not excluded

- A branch after a failing system call or a failing allocation. The tests in
  `tests/fault-inject` make `socket`, `bind`, `listen`, `connect`, `send`, `recv`, `epoll_ctl`,
  `open`, `read`, `write`, `pthread_create`, `posix_spawnp` and `operator new(std::nothrow)`
  fail on Linux, and the branch is counted when the test reaches it.
- The lines of `win32/` and `macos/`. They are excluded only when the coverage of their own
  platform proves them unreachable.

## 3. Marker format

```cpp
default:
    // GCOVR_EXCL_START: unreachable default arm
    ASSERT(false);
    break;
    // GCOVR_EXCL_STOP
```
