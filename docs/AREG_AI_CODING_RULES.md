# Areg SDK — AI Agent Coding Rules

**Target:** C++17 cross-platform IPC middleware
**Audience:** AI agents generating or refactoring Areg SDK code

This document defines mandatory rules, recommendations, and prohibitions.
When in doubt, match the style of the surrounding code. Do not introduce new style variants.

**Companion document:** `./docs/AREG_CODING_STYLE.md` (developer-oriented style guide with rationale)

---

## 1. Naming Rules (MUST)

Every identifier in generated code must follow this table exactly.

| Element                        | Convention          | Example                            |
|--------------------------------|---------------------|------------------------------------|
| Class / Struct / Alias         | `PascalCase`        | `ServiceManager`, `ConnectionData` |
| Enum class                     | `PascalCase`        | `ConnectionState`                  |
| Enum values                    | `PascalCase`        | `ConnectionState::Connected`       |
| Functions / Methods            | `snake_case`        | `start_service()`                  |
| Class data members             | `mPascalCase`       | `mSocketFd`, `mAddress`            |
| Struct public fields           | `snake_case`        | `service_address`, `timeout_ms`    |
| Static class members           | `s_snake_case`      | `s_instance_count`                 |
| Static local variables         | `_snake_case`       | `_instance`                        |
| Global vars (internal linkage) | `_snake_case`       | `_max_connections`                 |
| Constants (`constexpr`)        | `UPPER_SNAKE_CASE`  | `MAX_BUFFER_SIZE`                  |
| Template parameters            | `PascalCase`        | `MessageType`                      |
| Namespaces                     | `snake_case`        | `areg`, `aregext`, `areg::os`      |
| Macros                         | `AREG_UPPER_SNAKE`  | `AREG_NOCOPY`                      |
| Local variables                | `snake_case`        | `connection_count`                 |

**Note:** The above table is a strict requirement for generated new code. Tolerate existing `camelCase` local variable style when reviewing. 

### 1.1 Accessor naming

| Pattern            | Purpose        | Example                             |
|--------------------|----------------|-------------------------------------|
| `property()`       | Accessor       | `size()`, `count()`, `state()`      |
| `is_property()`    | Boolean query  | `is_valid()`, `is_connected()`      |
| `has_property()`   | Boolean query  | `has_data()`, `has_parent()`        |
| `set_property()`   | Mutator        | `set_size()`, `set_name()`          |
| `action_noun()`    | Command        | `start_service()`, `stop_timer()`   |

- No `get_` prefix: write `count()`, not `get_count()`.
- Boolean accessors keep `is_` or `has_` prefix.
- Mutators always use `set_` prefix.

### 1.2 Name length

Prefer shorter names when context makes meaning clear.

```cpp
// Good — function provides context
void load_config(std::string_view file);
void set_timeout(int ms);

// Bad — too long
void load_configuration_file(std::string_view configuration_file_path);
```

### 1.3 Namespace assignment

Assign namespaces based on module location. Do not invent new top-level names.

| Namespace  | Module / location                                                        |
|------------|--------------------------------------------------------------------------|
| `areg`     | All files in `framework/areg/` (core framework)                          |
| `areg::os` | OS-specific files (suffix `Posix` or `Win32`) within `framework/areg/`  |
| `aregext`  | All files in `framework/aregextend/`                                     |
| `areglog`  | All files in `framework/areglogger/`                                     |

Standalone applications (`logcollector`, `logobserver`, `mtrouter`) do not require a namespace. If one is used, it must match the module name.

Wrap every `.hpp` and `.cpp` file in the correct namespace:

```cpp
// framework/areg/ — core framework
namespace areg
{
    class ServiceManager { };
}  // namespace areg

// framework/areg/ private/ posix/ or win32/ — OS-specific
namespace areg::os
{
    class TimerPosix { };
}  // namespace areg::os

// framework/aregextend/
namespace aregext
{
    class SystemServiceConsole { };
}  // namespace aregext
```

---

## 2. Type Rules (MUST)

### 2.1 String parameters

Use `std::string_view` for read-only string parameters.
Use `const char*` only at C API boundaries (`extern "C"`).

```cpp
void load_config(std::string_view file);            // Correct
void load_config(const char* file);                 // Wrong
```

### 2.2 Integer types

Use fixed-width types: `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`, `int8_t`, `int16_t`, `int32_t`, `int64_t`.
Never use `unsigned int`, `unsigned short`, or `unsigned char`.

**Exceptions:** `size_t` for STL compatibility; `int` for short loop counters.

### 2.3 Pointers and references

Attach `*` and `&` to the type. A space between type and `*`/`&` is also acceptable.

```cpp
const String& name;                                 // Correct
const String & name;                                // Also acceptable
const String &name;                                 // Wrong
```

### 2.4 nullptr

Always use `nullptr`. Never `NULL` or `0` for pointers.

### 2.5 auto

Use `auto` for complex types (iterators, lambdas, `make_unique` results).
Never use `auto` for return types.

```cpp
auto iter = connections.begin();                     // Correct
auto count() const { return mCount; }                // Wrong — no auto return types
```

### 2.6 std::optional

Use `std::optional` for "may not have value" return types.

### 2.7 Error handling (MUST)

Use no exceptions for error handling in Areg SDK code.  
Prefer these patterns:
- Return `bool` for success/failure and use out-parameters for results.
- Return `std::optional<T>` when “no value” is a valid outcome.
- Return an integer error code only if the surrounding module already uses that convention.
```cpp
bool parse_config(std::string_view path, Config& config);

std::optional<Connection> find_connection(uint32_t id);
```

---

## 3. Layout Rules (MUST)

### 3.1 Indentation

4 spaces per level. No tabs.

### 3.2 Braces

Allman style — opening brace on a new line.

```cpp
void process()
{
    if (is_valid())
    {
        send_response();
    }
}
```

### 3.3 Line length

- **Preferred limit:** 120 characters — lines up to 120 need no breaking.
- **Maximum limit:** 180 characters — lines between 120 and 180 are acceptable when breaking hurts readability.
- Lines exceeding 180 characters must be broken.

### 3.4 Leading comma

Use leading commas for enums, initializer lists, and multi-line parameter lists.

```cpp
enum class State
{
      Stopped   = 0
    , Running   = 1
};

Connection(int fd, std::string addr)
    : mSocketFd (fd)
    , mAddress  (std::move(addr))
    , mState    (State::Stopped)
{
}

void configure( std::string_view name
              , uint16_t port
              , int max_connections);
```

### 3.5 Alignment (RECOMMENDED)

Align member variables, constants, enum numeric values, and initializer list values on the same column.

```cpp
// Member variables
private:
    int             mSocketFd;
    std::string     mAddress;
    ConnectionState mState;

// Constants
constexpr int    MAX_CONNECTIONS    { 100 };
constexpr size_t DEFAULT_TIMEOUT_MS { 5000 };

// Enum numeric values
enum class Priority
{
      Low       = 0
    , Normal    = 1
    , High      = 2
    , Critical  = 3
};
```

---

## 4. Class Rules (MUST)

### 4.1 Member declaration order

Declare class members in this order:

1. Public types and constants
2. Static methods
3. Constructors and destructor
4. Public methods
5. Protected methods
6. Private methods
7. Protected member variables
8. Private member variables
9. Deleted constructors / operators

### 4.2 Constructor initialization

Use initializer lists with leading comma. Do not use in-class member initializers for classes.

```cpp
Connection(int fd, std::string addr)
    : mSocketFd (fd)
    , mAddress  (std::move(addr))
    , mState    (State::Stopped)
{
}
```

### 4.3 Structs (data carriers)

Structs are plain data holders. Public fields use `snake_case`. In-class initializers are allowed.

```cpp
struct ServiceConfig
{
    uint16_t    port            { 8080 };
    int         max_connections { 100 };
    bool        auto_reconnect  { true };
    std::string bind_address    { "0.0.0.0" };
};
```

### 4.4 Virtual methods (C.128)

Use exactly ONE of `virtual`, `override`, or `final` per declaration:
- Base class introducing a virtual function: `virtual` alone.
- Derived class overriding: `override` alone (it implies virtual).
- Final override: `final` alone.

Do not write `override` or `virtual` in the `.cpp` definition.

```cpp
class Base
{
public:
    virtual void process();             // Base: 'virtual' alone
    virtual ~Base() = default;
};

class Derived : public Base
{
public:
    void process() override;            // Derived: 'override' alone
};

class Final final : public Derived
{
public:
    void process() final;               // Final: 'final' alone
};

// In source — no keywords
void Derived::process()
{
    // Implementation
}
```

### 4.5 Inline methods

- Use `inline` for simple getters/setters: declare in class, implement outside the class body in the same header.
- Never use `inline` with `virtual` methods.
- Avoid `inline` for static methods that instantiate static members.
- Implement complex private inline methods in the `.cpp` file to reduce header dependencies.

```cpp
// In header — declare inline in class
class Connection
{
public:
    inline int32_t count() const;
    inline void set_count(int32_t value);
};

// In header — implement after class
inline int32_t Connection::count() const
{
    return mCount;
}

inline void Connection::set_count(int32_t value)
{
    mCount = value;
}
```

### 4.6 Variable initialization

Always initialize variables at declaration.

```cpp
int count           { 0 };
std::string name    { };
auto ptr = std::make_unique<Connection>();

int count;                                           // Wrong — uninitialized
```

---

## 5. Header File Rules (MUST)

### 5.1 Include guards

Use traditional guards. Format: `AREG_<PATH>_<FILENAME>_HPP`.
Never use `#pragma once`.

```cpp
#ifndef AREG_IPC_SERVICE_MANAGER_HPP
#define AREG_IPC_SERVICE_MANAGER_HPP
// ...
#endif  // AREG_IPC_SERVICE_MANAGER_HPP
```

### 5.2 Include order

Separate groups with blank lines. Order:

1. Header files (`.hpp`): always include `areg/base/GEGlobal.h` first (if used in the file), then other includes grouped.
2. Source files (`.cpp`): always include the corresponding header first.
3. Then include remaining headers grouped in this order: 
   - Areg SDK headers
   - C++ standard library headers
   - C system headers
   - Third-party library headers

---

## 6. Macro Rules (MUST)

- Prefix project macros with `AREG_`.
- Never define empty `IN` / `OUT` / `INOUT` parameter macros.
- Prefer `constexpr`, `inline` functions, or templates over macros.
- Use macros only for: conditional compilation, visibility control, code generation (rare).

---

## 7. Container and Buffer API Naming (MUST)

Pick one semantic category per type. Do not mix.

### 7.1 Container semantics (elements)

| Method         | Returns                                            |
|----------------|----------------------------------------------------|
| `size()`       | Number of elements stored                          |
| `capacity()`   | Maximum elements without reallocation              |
| `available()`  | `capacity() - size()`                              |
| `is_empty()`   | Whether `size() == 0`                              |

Prefer `size()` over `count()` for element count.
Use `count()` only for domain-specific counters: `retry_count()`, `error_count()`.

### 7.2 Buffer semantics (bytes)

| Method         | Returns                                            |
|----------------|----------------------------------------------------|
| `size()`       | Used bytes                                         |
| `capacity()`   | Total allocated bytes                              |
| `available()`  | Remaining free bytes                               |
| `is_empty()`   | Whether `size() == 0`                              |

Avoid `length()` for buffers. Use `length()` only for string character count.

### 7.3 Disambiguation

When a type exposes multiple size/capacity concepts, always qualify with a domain noun:
`payload_size()`, `header_size()`, `buffer_capacity()`, `scope_count()`.

### 7.4 Unit documentation

If the unit is not obvious, document it: "Returns size in bytes", "Returns number of elements".

---

## 8. Recommendations (SHOULD)

These are preferred practices but not mandatory:

1. Align member variables, constants, and enum values on columns for readability.
2. Prefer shorter meaningful names: `setup()` over `initialize_application()`.
3. Use `std::optional` for optional return values.
4. Prefer `constexpr` over `#define` for constants.
5. Prefer `inline` functions over function-like macros.
6. Prefer templates over code-generating macros.
7. Remove deprecated methods and variables; update dependent files.
8. Document units in comments when not obvious.
9. Use `snake_case` for local variables in AI-generated code.
10. Do not complain about `camelCase` local variables in existing code; consider them acceptable if consistent within the file.
11. Prefer private data with protected accessors over `protected` data members (C.133). Protected data is acceptable when it reduces verbosity and the hierarchy is small.

---

## 9. Prohibited (NEVER)

| Prohibited                                | Use instead                                   |
|-------------------------------------------|-----------------------------------------------|
| `#pragma once`                            | Traditional include guards                    |
| `unsigned int/short/char`                 | `uint32_t`, `uint16_t`, `uint8_t`             |
| `get_` prefix on accessors                | `count()` not `get_count()`                   |
| `inline virtual` methods                  | Non-inline virtual in base; `override` alone in derived |
| `virtual` + `override` together           | Use exactly one: `virtual`, `override`, or `final` |
| In-class member initializers for classes  | Constructor initializer lists                 |
| Exceptions for error handling             | Return codes, `std::optional`, error states   |
| Global variables with external linkage    | Anonymous namespace or file-static            |
| Tabs                                      | 4 spaces                                      |
| `auto` for return types                   | Explicit return types                         |
| `NULL` or `0` for pointers                | `nullptr`                                     |
| `IN` / `OUT` / `INOUT` empty macros       | Comments or named return types                |
| `const char*` for string parameters       | `std::string_view` (except C API boundaries)  |
| Macros for constants                      | `constexpr`                                   |
| Deprecated dead code                      | Remove and update dependent files             |

---

## 10. Reference Example

```cpp
// areg/ipc/ServiceManager.hpp
#ifndef AREG_IPC_SERVICE_MANAGER_HPP
#define AREG_IPC_SERVICE_MANAGER_HPP

#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace areg
{

class Connection;

enum class ServiceState
{
      Stopped     = 0
    , Starting    = 1
    , Running     = 2
    , Stopping    = 3
};

struct ServiceConfig
{
    uint16_t    port            { 8080 };
    int         max_connections { 100 };
    std::string bind_address    { "0.0.0.0" };
};

/**
 * \brief   Manages lifecycle of IPC services.
 **/
class ServiceManager
{
//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
public:
    static constexpr int MAX_SERVICES { 64 };

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    static ServiceManager& instance();

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServiceManager();
    virtual ~ServiceManager();

//////////////////////////////////////////////////////////////////////////
// Public methods
//////////////////////////////////////////////////////////////////////////
public:
    int start(std::string_view config_path);
    void stop();

    inline ServiceState state() const;
    inline bool is_running() const;
    size_t connection_count() const;

//////////////////////////////////////////////////////////////////////////
// Protected overrides
//////////////////////////////////////////////////////////////////////////
protected:
    virtual void on_start();

//////////////////////////////////////////////////////////////////////////
// Private methods
//////////////////////////////////////////////////////////////////////////
private:
    void initialize();
    void cleanup();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    String      mAddress;
    uint16_t    mPort;

private:
    ServiceState                                mState;
    std::vector<std::unique_ptr<Connection>>    mConnections;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(ServiceManager);
};

//////////////////////////////////////////////////////////////////////////
// Inline methods
//////////////////////////////////////////////////////////////////////////

inline ServiceState ServiceManager::state() const
{
    return mState;
}

inline bool ServiceManager::is_running() const
{
    return mState == ServiceState::Running;
}

}  // namespace areg

#endif  // AREG_IPC_SERVICE_MANAGER_HPP
```

```cpp
// areg/ipc/ServiceManager.cpp
#include "areg/ipc/ServiceManager.hpp"

#include "areg/ipc/Connection.hpp"

#include <algorithm>

namespace areg
{

ServiceManager::ServiceManager()
    : mAddress      ()
    , mPort         (0)
    , mState        (ServiceState::Stopped)
    , mConnections  ()
{
    initialize();
}

ServiceManager::~ServiceManager()
{
    if (is_running())
    {
        stop();
    }
}

int ServiceManager::start(std::string_view config_path)
{
    if (config_path.empty())
    {
        return -1;
    }

    mState = ServiceState::Starting;
    on_start();
    mState = ServiceState::Running;
    return 0;
}

void ServiceManager::stop()
{
    mState = ServiceState::Stopping;
    cleanup();
    mState = ServiceState::Stopped;
}

size_t ServiceManager::connection_count() const
{
    return mConnections.size();
}

void ServiceManager::on_start()
{
    // Override in derived classes
}

void ServiceManager::initialize()
{
    // Implementation
}

void ServiceManager::cleanup()
{
    mConnections.clear();
}

}  // namespace areg
```

---

**Full style guide with rationale:** `./docs/AREG_CODING_STYLE.md`
