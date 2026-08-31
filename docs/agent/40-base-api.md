# The areg base API

The generated bases give you the *contract*. This page gives you the *utilities* the
body of a handler is written with: strings, containers, time, files, threads.

**Why this page exists.** Every method in this framework is `snake_case` and every
public type is in `namespace areg`. This changed recently. If you are recalling an
areg API from memory rather than reading it here, you are almost certainly recalling
the old `camelCase` spelling and it will not compile.

Only members that application code actually uses are listed. Every signature here was
read from the header cited beside it.

---

## 1. `areg::String`

`#include "areg/base/String.hpp"`

`areg::String` derives from `areg::StringBase<char>`, and most of what you call is
declared on that base. That is an implementation detail; you call them all on a
`String`.

| Call | Signature | Header |
|---|---|---|
| `as_string()` | `const char * as_string() const noexcept` | `StringBase.hpp:289` |
| `is_empty()` | `bool is_empty() const noexcept` | `StringBase.hpp:254` |
| `length()` | `areg::CharCount length() const noexcept` | `StringBase.hpp:260` |
| `compare(...)` | `areg::Ordering compare(const StringBase<char> & other, bool caseSensitive = true) const noexcept` | `StringBase.hpp:473` |
| `find_first(...)` | `areg::CharPos find_first(char ch, areg::CharPos startPos = areg::START_POS, bool caseSensitive = true) const noexcept` | `StringBase.hpp:383` |
| `find_last(...)` | `areg::CharPos find_last(char ch, areg::CharPos startPos = areg::END_POS, bool caseSensitive = true) const noexcept` | `StringBase.hpp:423` |

Overloads of `compare`, `find_first` and `find_last` also take `const char *`,
`std::string` and `std::string_view`.

### Passing a String to a log macro

**This is the single most common compile error in application code.** The logging
macros are `printf` style, so `%s` needs a `const char *`, not a `String`:

```cpp
LOG_INFO("client [ %s ] connected", client.as_string());   // right
LOG_INFO("client [ %s ] connected", client);               // wrong
```

### Numbers

Conversions in both directions, all declared on `StringBase`:

| To a number | From a number |
|---|---|
| `to_int32(areg::Radix radix = areg::Radix::Decimal)` | `from_int32(int32_t, areg::Radix = areg::Radix::Decimal)` |
| `to_uint32(...)`, `to_int64(...)`, `to_uint64(...)` | `from_uint32(...)`, `from_int64(...)`, `from_uint64(...)` |
| `to_float()`, `to_double()`, `to_bool()` | `from_float(float)`, `from_double(double)`, `from_bool(bool)` |

Every `from_*` returns `String &`, so they chain.

---

## 2. Containers

`#include "areg/base/ArrayList.hpp"` / `HashMap.hpp` / `LinkedList.hpp`

**The header names have no `TE` prefix.** The types are `areg::ArrayList<VALUE>`,
`areg::HashMap<KEY, VALUE>` and `areg::LinkedList<VALUE>`.

> **These containers do not expose `begin()` and `end()`.**
> A range-`for` over one does not compile. Iterate by index.

```cpp
for (uint32_t i = 0; i < list.size(); ++ i)
{
    const auto & item = list[i];
}
```

| Container | Template | Size | Empty | Element access | Membership |
|---|---|---|---|---|---|
| `ArrayList` | `<VALUE>` | `uint32_t size() const noexcept` (`ArrayList.hpp:172`) | `bool is_empty() const noexcept` (`:166`) | `operator[](uint32_t)` (`:102`), `value_at(uint32_t)` (`:237`), `set_value_at(uint32_t, const VALUE &)` (`:221`) | `contains(const VALUE &, uint32_t startAt = 0)` (`:188`) |
| `HashMap` | `<KEY, VALUE>` | `uint32_t size() const noexcept` (`HashMap.hpp:181`) | `bool is_empty() const noexcept` (`:175`) | `operator[](const KEY &)` (`:105`), `value_at(const KEY &)` (`:504`), `set_value_at(const KEY &, const VALUE &)` (`:281`) | `contains(const KEY &)` (`:226`) |
| `LinkedList` | `<VALUE>` | `uint32_t size() const noexcept` (`LinkedList.hpp:195`) | `bool is_empty() const noexcept` (`:189`) | `operator[](uint32_t)` (`:89`), `value_at(uint32_t)` (`:405`) | `contains(const VALUE &)` (`:254`) |

`HashMap` and `LinkedList` also index by position (`MAPPOS`, `LISTPOS`) for cursor
style traversal. Index access is enough for ordinary code.

Common to all three: `add(...)`, `clear()`, `release()`, `find(...)`.

---

## 3. `areg::Timer`

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

## 4. `areg::Application`

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

## 5. `areg::Component`

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

## 6. `areg::Thread`

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

## 7. `areg::DateTime`

`#include "areg/base/DateTime.hpp"`

| Call | Signature | Header |
|---|---|---|
| now, as an object | `static DateTime now() noexcept` | `DateTime.hpp:163` |
| now, as a raw stamp | `static TIME64 timestamp() noexcept` | `DateTime.hpp:169` |
| format | `String format_time(std::string_view formatName = areg::DEFAULT_TIME_FORMAT_OUTPUT) const` | `DateTime.hpp:210` |

```cpp
areg::String stamp = areg::DateTime::now().format_time();
LOG_INFO("at %s", stamp.as_string());
```

---

## 8. `areg::File`

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

## 9. `areg::SharedBuffer`

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

## 10. Unique names

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

## 11. What is not here

| You need | Go to |
|---|---|
| Names generated from a `.siml` | `20-service-interface.md` section 3 |
| Logging macros | `34-logging.md` |
| Timer ownership and threading rules | `33-timers.md` |
| Synchronization primitives | `../../examples/10_sync/`, `../../examples/29_syncevent/` |
| Anything else in the framework | The header. **Never a header under a `private/` folder.** |

If a member you need is missing here, read the header rather than guessing the
spelling from another language's conventions.
