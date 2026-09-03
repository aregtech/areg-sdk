# The areg base API: text and collections

`areg::String` and the containers, which almost every handler body uses. The rest of
the runtime -- the application, components, threads, timers, time, files and buffers
-- is `42-runtime-api.md`.

**Why this page exists.** Every method in this framework is `snake_case` and every
public type is in `namespace areg`. This changed recently. If you are recalling an
areg API from memory rather than reading it here, you are almost certainly recalling
the old `camelCase` spelling and it will not compile.

Only members that application code actually uses are listed. Every signature here was
read from the header cited beside it, and the `Header.hpp:line` beside each one is
checked against that header on every push.

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
// areg-check: ignore
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

A container in working code, built and run by CI: `recipes/09-shared-types/`, where the
collector keeps every reading it accepted and prints them by index.

---

## 3. What is not here

| You need | Go to |
|---|---|
| Names generated from a `.siml` | `20-service-interface.md` section 3 |
| Logging macros | `34-logging.md` |
| Timers, the application, components, threads, time, files | `42-runtime-api.md` |
| Synchronization primitives | `../../examples/10_sync/`, `../../examples/29_syncevent/` |
| Anything else in the framework | The header. **Never a header under a `private/` folder.** |

If a member you need is missing here, read the header rather than guessing the
spelling from another language's conventions.

