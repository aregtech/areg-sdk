# The areg runtime API

The process and the threads it runs in: the application, a component's own identity,
threads, timers, time, files and buffers. Text and the containers are
`40-base-api.md`.

Every method is `snake_case` and every public type is in `namespace areg`. Every
signature was read from the header cited beside it, and the `Header.hpp:line` is
checked against that header on every push.

| Looking for | Section |
|---|---|
| a periodic or one-shot timer | 1, `areg::Timer` |
| start, stop, quit, load a model | 2, `areg::Application` |
| what a component knows about itself | 3, `areg::Component` |
| sleeping, thread identity, the current dispatcher | 4, `areg::Thread` |
| now, formatting, elapsed time | 5, `areg::DateTime` |
| reading and writing a file, paths | 6, `areg::File` |
| carrying bytes through an event | 7, `areg::SharedBuffer` |
| a name nothing else in the process has | 8, unique names |

---

## 1. `areg::Timer`

`#include "areg/component/Timer.hpp"`

Full treatment with the ownership rules: `33-timers.md`.

| Call | Signature | Header |
|---|---|---|
| constructor | `Timer(TimerConsumer & consumer, const String & timerName = String::empty_string(), uint32_t timeoutMs = areg::INVALID_TIMEOUT, int32_t maxQueued = Timer::IGNORE_TIMER_QUEUE, EventPriority prio = areg::DefaultPriority)` | `Timer.hpp:100` |
| start on this thread | `bool start_timer(uint32_t timeoutInMs, uint32_t eventCount = TimerBase::CONTINUOUSLY)` | `Timer.hpp:119` |
| start on a given thread | `bool start_timer(uint32_t timeoutInMs, DispatcherThread & whichThread, uint32_t eventCount = TimerBase::CONTINUOUSLY)` | `Timer.hpp:131` |
| stop | `void stop_timer()` | `Timer.hpp:136` |
| running? | `bool is_active() const noexcept` | `TimerBase.hpp:145` |

Repetition constants, both on `areg::TimerBase`:

| Constant | Value | Header |
|---|---|---|
| `TimerBase::CONTINUOUSLY` | `0xFFFFFFFF`, fire forever | `TimerBase.hpp:61` |
| `TimerBase::ONE_TIME` | `1`, fire once | `TimerBase.hpp:67` |

The handler you override, from `areg::TimerConsumer`:

```cpp
void process_timer(areg::Timer & timer) final;      // TimerConsumer.hpp:73
```

---

## 2. `areg::Application`

`#include "areg/appbase/Application.hpp"`

| Call | Signature | Header |
|---|---|---|
| `setup()` | `static void setup(bool startTracing = true, bool startServicing = true, bool startRouting = true, bool startTimer = true, bool startWatchdog = false, const char * configFile = areg::DEFAULT_CONFIG_FILE.data(), ConfigListener * configListener = nullptr)` | `Application.hpp:103` |
| `load_model()` | `static bool load_model(const char * modelName = nullptr)` | `Application.hpp:128` |
| `wait_quit()` | `static bool wait_quit(uint32_t waitTimeout = areg::WAIT_INFINITE)` | `Application.hpp:338` |
| `signal_quit()` | `static void signal_quit()` | `Application.hpp:344` |
| `unload_model()` | `static void unload_model(const char * modelName = nullptr)` | `Application.hpp:145` |
| `release()` | `static void release()` | `Application.hpp:114` |

Every argument of `setup()` has a default, so `setup()` alone is the normal call.
Nothing ends the application until something calls `signal_quit()`.

---

## 3. `areg::Component`

`#include "areg/component/Component.hpp"`

| Call | Signature | Header |
|---|---|---|
| constructor used by the model | `Component(const areg::ComponentEntry & regEntry, ComponentThread & ownerThread)` | `Component.hpp:167` |
| constructor by role name | `Component(const String & roleName, ComponentThread & ownerThread)` | `Component.hpp:159` |
| own role name | `const String & role_name() const noexcept` | `Component.hpp:317` |
| own address | `const ComponentAddress & address() const noexcept` | `Component.hpp:323` |
| owning thread | `ComponentThread & master_thread() noexcept` | `Component.hpp:311` |

Optional lifecycle overrides:
`startup_component(ComponentThread &)` (`Component.hpp:191`),
`shutdown_component(ComponentThread &)` (`:199`),
`notify_component_shutdown(ComponentThread &)` (`:207`),
`wait_component_completion(uint32_t)`.

---

## 4. `areg::Thread`

`#include "areg/base/Thread.hpp"`

Application code normally only needs the static helpers. **Never call `sleep()` inside
a handler** - it blocks every component on that dispatcher thread.

| Call | Signature | Header |
|---|---|---|
| `sleep()` | `static void sleep(uint32_t msTimeout)` | `Thread.hpp:373` |
| `current_thread()` | `static Thread * current_thread() noexcept` | `Thread.hpp:445` |
| `current_thread_name()` | `static String current_thread_name() noexcept` | `Thread.hpp:451` |
| `current_thread_id()` | `static id_type current_thread_id() noexcept` | `Thread.hpp:439` |
| `switch_thread()` | `static void switch_thread() noexcept` | `Thread.hpp:424` |

---

## 5. `areg::DateTime`

`#include "areg/base/DateTime.hpp"`

| Call | Signature | Header |
|---|---|---|
| now, as an object | `static DateTime now() noexcept` | `DateTime.hpp:163` |
| now, as a raw stamp | `static TIME64 timestamp() noexcept` | `DateTime.hpp:169` |
| format | `String format_time(std::string_view formatName = areg::DEFAULT_TIME_FORMAT_OUTPUT) const` | `DateTime.hpp:210` |

```cpp
// areg-check: ignore
areg::String stamp = areg::DateTime::now().format_time();
LOG_INFO("at %s", stamp.as_string());
```

---

## 6. `areg::File`

`#include "areg/base/File.hpp"`

| Call | Signature | Header |
|---|---|---|
| constructor | `explicit File(const String & fileName, uint32_t mode = ...OpenAlways \| ...Binary)` | `File.hpp:142` |
| `open()` | `bool open()` | `File.hpp:158` |
| `open(name, mode)` | `bool open(const String & fileName, uint32_t mode)` | `File.hpp:168` |
| `close()` | `void close()` | `File.hpp:174` |
| `read(String &)` | `uint32_t read(String & ascii) const` | `File.hpp:240` |
| `read(bytes)` | `uint32_t read(uint8_t * buffer, uint32_t size) const noexcept` | `File.hpp:255` |
| `write(String)` | `uint32_t write(const String & ascii)` | `File.hpp:272` |
| `write(bytes)` | `uint32_t write(const uint8_t * buffer, uint32_t size) noexcept` | `File.hpp:287` |
| `flush()` | `void flush() noexcept` | `File.hpp:292` |
| `remove()` | `bool remove()` | `File.hpp:179` |
| `is_opened()` | `bool is_opened() const noexcept override` | `File.hpp:208` |

`read` and `write` also take a `SharedBuffer` and a `WideString`. File input and
output blocks; do it on a worker thread, never in a handler. Working example:
`../../examples/06_file/`.

---

## 7. `areg::SharedBuffer`

`#include "areg/base/SharedBuffer.hpp"`

Serialize with `operator <<` and `operator >>`; the members below are the rest.

| Call | Signature | Header |
|---|---|---|
| `write(bytes)` | `uint32_t write(const uint8_t * buf, uint32_t size)` | `SharedBuffer.hpp:247` |
| `read(bytes)` | `uint32_t read(uint8_t * buf, uint32_t size) const noexcept` | `SharedBuffer.hpp:282` |
| `reset()` | `void reset() const noexcept` | `SharedBuffer.hpp:285` |
| `block_size()` | `uint32_t block_size() const noexcept` | `SharedBuffer.hpp:429` |
| `is_end()` | `bool is_end() const noexcept` | `SharedBuffer.hpp:425` |
| `clone()` | `SharedBuffer clone() const` | `SharedBuffer.hpp:436` |

`reset()` rewinds the read position; call it before reading back what you just wrote.
Working example: `../../examples/05_buffer/`.

---

## 8. Unique names

`#include "areg/base/UtilityDefs.hpp"`

```cpp
areg::String generate_name(const char * prefix);        // UtilityDefs.hpp:512
```

Use it when two instances would otherwise claim one role name - for example several
consumer processes started from the same executable:

```cpp
const areg::String _consumer(areg::generate_name("ServiceConsumer"));
```

---

## When this is not enough

| You need | Go to |
|---|---|
| `areg::String` and the containers | `40-base-api.md` |
| Timer ownership and threading rules | `33-timers.md` |
| Worker threads, watchdogs, a model built at run time | `37-threads.md` |
| Anything else in the framework | The header. **Never a header under a `private/` folder.** |
