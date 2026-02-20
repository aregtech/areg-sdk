# Areg SDK Coding Style Guide

**Version:** 2.1
**Date:** February 2026
**Target:** Cross-platform IPC middleware (Linux, macOS, Windows, Embedded Linux, HPC)

---

## Philosophy

Areg SDK follows modern C++ conventions optimized for cross-platform development. This style emphasizes:

- **Modern C++17** idioms
- **STL compatibility** (snake_case functions)
- **Clear type distinctions** (PascalCase types)
- **Pragmatic flexibility** without sacrificing consistency

**CRITICAL RULES - Always Follow:**
1. Types = `PascalCase`, Functions = `snake_case`, Members = `mPascalCase`
2. String parameters = `std::string_view` (NOT `const char*`)
3. Integer types = `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t` (NOT `unsigned int`)
4. Accessors = `count()` (NOT `get_count()` or `getCount()`)
5. Header guards = traditional (NOT `#pragma once`)
6. NO `IN`/`OUT`/`INOUT` empty macros
7. Avoid global variables. Exceptions: internal-linkage variables in anonymous namespaces and log scopes declared as `static`.

**Companion document:** `./docs/AREG_AI_CODING_RULES.md` (compact rule reference for AI agents)

---

## 1. Naming Conventions

### 1.1 Quick Reference

| Element                   | Convention                    | Example                                   |
|---------------------------|-------------------------------|-------------------------------------------|
| Classes                   | PascalCase                    | `ServiceManager`                          |
| Structs                   | PascalCase                    | `ConnectionData`                          |
| Functions/Methods         | snake_case                    | `start_service()`                         |
| Local Variables           | snake_case OR camelCase       | `connection_count` OR `connectionCount`   |
| Class Member Variables    | `m` + PascalCase              | `mSocketFd`                               |
| Struct Public Fields      | snake_case                    | `max_retries`, `timeout_ms`               |
| Static Members            | `s_` + snake_case             | `s_instance_count`                        |
| Static Local Variables    | `_` + snake_case              | `_instance`                               |
| Global Variables          | internal linkage, `_` + snake_case | `_max_connections` (avoid if possible)|
| Constants                 | constexpr UPPER_SNAKE         | `MAX_BUFFER_SIZE`                         |
| Enum Classes              | PascalCase                    | `ConnectionState`                         |
| Enum Values               | PascalCase                    | `ConnectionState::Connected`              |
| Macros                    | `AREG_` + UPPER_SNAKE         | `AREG_ASSERT`                             |
| Template Parameters       | PascalCase                    | `template<typename MessageType>`          |
| Namespaces                | snake_case                    | `areg`, `aregext`, `areg::os`             |

---

### 1.2 Classes and Structs

**RULE:** Classes and Structs use PascalCase for type names.

```cpp
// ✅ CORRECT
class ServiceManager { };
class TcpConnection { };
struct MessageHeader { };
struct ConnectionData { };

// ❌ WRONG
class service_manager { };  // No snake_case for types
class serviceManager { };   // No camelCase for types
```

**RULE:** Struct public fields use snake_case (no `m` prefix).

```cpp
// ✅ CORRECT - struct fields are snake_case
struct ConnectionConfig
{
    int         max_retries     { 3 };
    int         timeout_ms      { 5000 };
    bool        auto_reconnect  { true };
    std::string server_address  { "localhost" };
};

// Usage
ConnectionConfig config;
config.max_retries = 5;

// ❌ WRONG - don't use m prefix or camelCase for struct fields
struct ConnectionConfig
{
    int mMaxRetries{3};        // NO - structs don't use m prefix
    int maxRetries{3};         // NO - use snake_case, not camelCase
    int m_timeout_ms{5000};    // NO - wrong style
};
```

**Rationale:**
- Structs are simple data holders (POD-like)
- No `m` prefix needed (no private/public distinction)
- snake_case keeps them lightweight and consistent with function naming

---

### 1.3 Functions and Methods

**RULE:** Functions use snake_case. Drop `get_` prefix for simple accessors.

```cpp
// ✅ CORRECT
class ServiceManager
{
public:
    void start_service();
    void stop_service();

    // Simple accessors - NO "get_" prefix
    int connection_count() const;          // NOT get_connection_count()
    int active_services() const;           // NOT get_active_services()
    State state() const;                   // NOT get_state()

    // Booleans - KEEP "is_" or "has_" prefix
    bool is_connected() const;
    bool has_pending_requests() const;

    // Mutators - ALWAYS use "set_"
    void set_timeout(int ms);
    void set_max_connections(int count);
};

// ❌ WRONG
void startService();           // No camelCase
void StartService();           // No PascalCase
int getConnectionCount();      // Drop "get_" prefix
int get_connection_count();    // Drop "get_" prefix
```

**Naming patterns:**

| Pattern           | Use               | Example                           |
|-------------------|-------------------|-----------------------------------|
| `property()`      | Simple accessor   | `size()`, `count()`, `state()`    |
| `is_property()`   | Boolean query     | `is_valid()`, `is_connected()`    |
| `has_property()`  | Boolean query     | `has_data()`, `has_parent()`      |
| `set_property()`  | Mutator           | `set_size()`, `set_name()`        |
| `action_noun()`   | Command           | `start_service()`, `stop_timer()` |

**Virtual methods (C.128):**

```cpp
// RULE: Use exactly ONE of: virtual, override, or final
// - Base class introducing a virtual: use 'virtual' alone
// - Derived class overriding: use 'override' alone (implies virtual)
// - Final override: use 'final' alone
class ServiceBase
{
public:
    virtual ~ServiceBase() = default;
    virtual void start_service();       // Base: 'virtual' alone
};

class DerivedService : public ServiceBase
{
public:
    void start_service() override;      // Derived: 'override' alone
};

class FinalService final : public DerivedService
{
public:
    void start_service() final;         // Final: 'final' alone
};

// In source (definition) - no override/virtual keyword
void DerivedService::start_service()
{
    // Implementation
}
```

---

### 1.4 Variables

#### Local Variables: snake_case OR camelCase (flexible)

**RULE:** Choose one style per file and be consistent within that file.

```cpp
// ✅ BOTH ACCEPTABLE
int connection_count = 0;           // snake_case
int connectionCount = 0;            // camelCase
std::string serverAddress;          // camelCase
auto service = std::make_unique<ServiceManager>();
```

#### Member Variables: `m` + PascalCase

**RULE:** All class member variables must use `m` prefix + PascalCase.

```cpp
class Connection
{
public:
    Connection(int fd, std::string addr)
        : mSocketFd (fd)
        , mIpAddress(std::move(addr))
        , mState    (ConnectionState::Disconnected)
    {
    }

private:
    // RULE: Optionally align member variables for better readability
    int             mSocketFd;
    std::string     mIpAddress;
    ConnectionState mState;
};
```

#### Static Members

**RULE:** Use `s_` + snake_case for static class members, `_` + snake_case for static local variables.

```cpp
class ServiceManager
{
    // Constants can be declared at the top
    static constexpr int MAX_SERVICES { 100 };

public:
    static ServiceManager& instance()
    {
        static ServiceManager _instance;
        return _instance;
    }

private:
    static int s_instance_count;
};
```

#### Global Variables: snake_case (avoid if possible)

Only global variables with internal linkage are allowed.
These are variables declared in anonymous namespace or variables declared as `static` within a source file.
These variables can be declared with `_` prefix. Typical case: log scopes declared via macro `DEF_LOG_SCOPE()`.
Avoid such variables if possible.

```cpp
namespace
{
    int _max_connections = 100;
}
```

---

### 1.5 Constants

**RULE:** Use constexpr with UPPER_SNAKE_CASE.

```cpp
// ✅ CORRECT - constexpr with UPPER_SNAKE_CASE
// Optionally align constants and use initializers
constexpr int       MAX_BUFFER_SIZE     { 4096 };
constexpr size_t    DEFAULT_TIMEOUT_MS  { 5000 };

// ❌ AVOID macros like this
#define MAX_BUFFER_SIZE 4096  // Use constexpr instead
```

**RULE:** Constants in classes can be declared at the top, even if private.

```cpp
class Connection
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////

    // Built-in types declared first
    enum class ConnectionState
    {
          Disconnected
        , Connecting
        , Connected
        , Error
    };

    // Constants declared at the top
    static constexpr int MAX_RETRIES { 100 };
};
```

---

### 1.6 Enumerations

**RULE:** Use PascalCase for enum values. Use leading comma style for better alignment.

```cpp
// ✅ CORRECT
enum class ConnectionState
{
      Disconnected
    , Connecting
    , Connected
    , Error
};

// Usage
ConnectionState state = ConnectionState::Connected;
```

**With explicit values — align for readability:**

```cpp
enum class Priority
{
      Low       = 0
    , Normal    = 1
    , High      = 2
    , Critical  = 3
};
```

---

### 1.7 Template Parameters

**RULE:** Template parameters use PascalCase.

```cpp
// ✅ CORRECT
template<typename MessageType, typename HandlerType>
class MessageDispatcher { };

template<typename T>
class Container { };
```

---

### 1.8 Namespaces

**RULE:** Namespaces use snake_case and short namespace names. All `.hpp` and `.cpp` files of `areg` framework should be wrapped in `areg` namespace:

```cpp
// ✅ CORRECT
namespace areg
{
    // Implementation
}   // IPC, utilies, etc.

```

Exceptions are `areg` framework OS/Platform specific implementations, they should be wrapped in `areg::os` namespace.
```cpp
namespace areg::os
{
    // OS specific classes and methods.
}
```

These OS specific implementations are normally located in the files with suffix such as `Posix` and `Win32`. The list will grow when integrated in new OS.

The other modules can have namespace names different from `areg`. For example, the areg extended objects, located in `framework/aregextend` directory should have namespace `aregext`.  
Here is the list of namespace names and modules:

| Namespace name    | Modules, locations and meanings                                                                   |
|-------------------|---------------------------------------------------------------------------------------------------|
| `areg`            | All files located in `framework/areg`. These are `areg` framework files.                          |
| `areg::os`        | All OS specific implementations. Typically located in files with suffix `Posix` or `Win32`.       |
| `aregext`         | All files of areg extended library located in `framework/aregextend` sub-directory                |
| `areglog`         | All files of areg logger library located in `framework/areglogger`sub-directory                   |

Since `logcollector`, `logobserver` and `mtrouter` are stand-alone applications and they are not supposed to be integrated in other applications,
having namespace name for them is optional and can be escaped, or the same name of module should be used like `logcollector`, `logobserver` and `mtrouter`.

---

### 1.9 Name Length - Clear But Concise

**RULE:** Prefer shorter names when context makes meaning clear.

```cpp
// ✅ GOOD - Function provides context
void load_config(std::string_view file);        // NOT config_file
void connect(std::string_view address);         // NOT server_address
void set_timeout(int ms);                       // NOT timeout_milliseconds

// ✅ GOOD - Ambiguous context needs full name
void copy(std::string_view source, std::string_view destination);
void convert(std::string_view input, Format from, Format to);

// ❌ TOO SHORT - Unclear
void load(std::string_view f);     // What is 'f'?
void proc(int x, int y);           // What does it process?

// ❌ TOO LONG - Redundant
void load_configuration_file(std::string_view configuration_file_path);
void set_maximum_connection_timeout_value(int timeout_value);
```

---

## 2. Macros

### 2.1 Naming

**RULE:** Macros use `AREG_` prefix + UPPER_SNAKE_CASE.

```cpp
// ✅ CORRECT - AREG_ prefix
#define AREG_MAX_CONNECTIONS    100
#define AREG_ASSERT(x)          assert(x)

// Exception for common utilities (check before defining)
#ifndef ASSERT
    #define ASSERT(x)   assert(x)
#endif

#ifndef LOG_DBG
    #define LOG_DBG(msg) ((void)0)
#endif
```

### 2.2 Prefer Modern Alternatives

**RULE:** Avoid macros when modern C++ alternatives exist.

1. `constexpr` for constants
2. `inline` functions for utilities
3. Templates for type-generic code
4. Macros only for:
   * Conditional compilation
     ```cpp
     #ifdef POSIX
     #endif // POSIX
     ```
   * Visibility control
     ```cpp
     #ifdef AREG_ENABLE_LOGS
        #define LOG_DBG(x)  logger.logDebug(x)
     #else
        #define LOG_DBG(x)
     #endif
     ```
   * Code generation (avoid except special cases)
     ```cpp
        #define  START_PROCESSING( name )   \
            bool is_processed { false };    \
            if (start_processing(name))     \
            {                               \
                is_processed = true;        \
                do                          \
                {


        #define  END_PROCESSING( )          \
                    is_processed = false;   \
                } while (false);            \
              end_processing();             \
            }                               \
            return is_processed;
     ```

```cpp
// ✅ PREFER constexpr
constexpr int MAX_CONNECTIONS = 100;

// ✅ PREFER inline
inline void log_debug(const char* msg) { /* ... */ }

// ✅ Macros for conditional compilation
#ifdef AREG_DEBUG_BUILD
    #define AREG_LOG_DEBUG(msg) log_impl(__FILE__, __LINE__, msg)
#else
    #define AREG_LOG_DEBUG(msg) ((void)0)
#endif
```

### 2.3 Prohibited Macros

**RULE:** Never use empty IN/OUT/INOUT macros for parameters.

```cpp
// ❌ WRONG - Don't use these macros
void process(IN const char* input, OUT int& result, INOUT State& state);

// ✅ CORRECT - Use comments if needed
void process(const char* input, int& result, State& state);
//           ^input           ^output      ^input+output

// ✅ BETTER - Use return value or out-parameters clearly
struct ProcessResult { int code; State new_state; };
ProcessResult process(const char* input, const State& current_state);
```

**Why:** Empty macros add no value and hurt readability.

---

## 3. Code Layout

### 3.1 Indentation

**RULE:** Use 4 spaces per level. NO tabs.

### 3.2 Braces - Allman Style

**RULE:** Opening braces on new line.

```cpp
// ✅ CORRECT
void process_message()
{
    if (is_valid())
    {
        send_response();
    }
}

class ServiceManager
{
public:
    ServiceManager()
    {
    }
};

// ❌ WRONG - No K&R style
void process_message() {
    if (is_valid()) {
        send_response();
    }
}
```

### 3.3 Line Length

- **Preferred limit:** 120 characters — lines up to 120 need no breaking.
- **Hard limit:** 180 characters — lines between 120 and 180 are acceptable when breaking hurts readability.
- Lines exceeding 180 characters must be broken.

```cpp
// ✅ CORRECT - Break long lines
void configure_service_manager( std::string_view service_name
                              , uint16_t port
                              , int max_connections);

// ❌ WRONG - Too long (exceeds 180)
void configure_service_manager(std::string_view service_name, uint16_t port, int max_connections, bool enable_ssl, int timeout);
```

### 3.4 Leading Comma Style

**RULE:** Use leading comma for enums, parameter lists, initializer lists.

```cpp
// ✅ CORRECT - Leading comma
enum class State
{
      Stopped
    , Starting
    , Running
};

Connection(int fd, std::string addr)
    : mSocketFd (fd)
    , mAddress  (std::move(addr))
    , mState    (State::Stopped)
{
}
```

### 3.5 Parameter Alignment - Use When Function Name is Long

**RULE:** Align parameters on new lines ONLY when:
- Function name + all parameters exceed 120 characters
- Parameters are numerous and need grouping for clarity

```cpp
// ✅ Short function - NO alignment needed
void connect(std::string_view addr, uint16_t port, bool ssl);

// ✅ Long function name - align parameters vertically
void configure_service_manager_with_options(
              std::string_view service_name
            , uint16_t         port
            , int              max_connections
            , bool             enable_ssl);

// ✅ Logging with long message/params - align for readability
LOG_DBG("Connection failed: addr=%s port=%d ssl=%d retry=%d/%d"
    , address.data()
    , port
    , use_ssl
    , current_retry
    , max_retries);

// ✅ Traditional style is also acceptable
void configure_service_manager_with_options(
    std::string_view service_name,
    uint16_t port,
    int max_connections,
    bool enable_ssl
);
```

### 3.6 Member Variable Alignment (Recommended)

**RULE:** Align types and names in one column for better readability.

```cpp
// ✅ GOOD - Aligned
private:
    int             mSocketFd;
    std::string     mAddress;
    ConnectionState mState;
    uint64_t        mTimestamp;

// ✅ Also acceptable - No alignment
private:
    int mSocketFd;
    std::string mAddress;
    ConnectionState mState;
    uint64_t mTimestamp;
```

### 3.7 Constant Alignment (Recommended)

**RULE:** Align constant values for better readability.

```cpp
// ✅ GOOD - Aligned
constexpr int    MAX_CONNECTIONS    { 100 };
constexpr size_t DEFAULT_TIMEOUT_MS { 5000 };
constexpr bool   ENABLE_DEBUG       { true };
```

---

## 4. Comments and Documentation

### 4.1 Header Comments

Use Doxygen-style comments. Align for readability.

```cpp
/**
 * \brief   Manages IPC service lifecycle and connections.
 *          ServiceManager handles initialization, configuration, and shutdown
 *          of IPC services. Supports multiple concurrent connections.
 **/
class ServiceManager
{
    // ...
};
```

### 4.2 Function Comments

```cpp
/**
 * \brief   Starts the service with the specified configuration.
 * \param   config_file     Path to configuration file (empty for defaults)
 * \return  0 on success, negative error code on failure
 */
int start_service(std::string_view config_file = "");
```

### 4.3 Inline Comments

```cpp
// Good comment - explains WHY
mRetryCount = 0;  // Reset counter after successful connection

// Bad comment - states WHAT (obvious from code)
mRetryCount = 0;  // Set retry count to zero
```

---

## 5. Declarations and Definitions

### 5.1 One Declaration Per Line

```cpp
// ✅ CORRECT
int count = 0;
int index { 0 };
int total = 0;

// ❌ WRONG
int count = 0, index {0}, total = 0;
```

### 5.2 Pointer and Reference Alignment

**RULE:** Attach `*` and `&` to the type, with space after.
Acceptable if there is a space between type and `*` or `&`.

```cpp
// ✅ CORRECT
const String& name;
char* buffer;
int* ptr;

// ✅ Also acceptable - space between type
const String & name;
char * buffer;
int * ptr;

// ❌ WRONG
const String &name;
char *buffer;
int *ptr;
```

### 5.3 Initialize Variables

**RULE:** Always initialize variables at declaration.

```cpp
// ✅ CORRECT
int count           { 0 };
std::string name    { };
auto ptr = std::make_unique<Connection>();

// ❌ WRONG
int count;         // Uninitialized
Connection* ptr;   // Uninitialized pointer
```

---

## 6. Modern C++ Features

### 6.1 auto Usage

**RULE:** Use `auto` for complex types, NOT for return types.

```cpp
// ✅ CORRECT - Use auto for complex iterators/lambdas
auto iter = connections.begin();
auto lambda = [](int x) { return x * 2; };
auto ptr = std::make_unique<Connection>();

// ✅ CORRECT - Explicit return types
int count() const { return mCount; }
bool is_valid() const { return mState != State::Error; }

// ❌ WRONG - Don't use auto for return types
auto count() const { return mCount; }        // Hard to read API
auto is_valid() const { return mState != State::Error; }
```

### 6.2 nullptr

**RULE:** Always use `nullptr`, never `NULL` or `0`.

```cpp
// ✅ CORRECT
Connection* conn = nullptr;
if (ptr != nullptr)
{
    ptr->process();
}

// ❌ WRONG
Connection* conn = NULL;
Connection* conn = 0;
```

### 6.3 std::optional

**RULE:** Use `std::optional` for "may not have value" scenarios.

```cpp
// ✅ CORRECT - Use for optional return values
std::optional<Connection> find_connection(int id)
{
    auto iter = mConnections.find(id);
    if (iter != mConnections.end())
    {
        return iter->second;
    }
    return std::nullopt;
}
```

### 6.4 String Parameters - Use std::string_view

**RULE:** Always use `std::string_view` for read-only string parameters.
Use `const char*` only at C API boundaries (`extern "C"`).

```cpp
// ✅ CORRECT - Modern C++17
void load_config(std::string_view file_path);
bool connect(std::string_view address, uint16_t port);
void set_name(std::string_view name);

// ❌ WRONG - Don't use const char*
void load_config(const char* file_path);      // OLD STYLE
void connect(const char* address, int port);  // OLD STYLE

// ❌ WRONG - Don't use const std::string&
void load_config(const std::string& file_path);  // Unnecessary copy overhead
```

**Why:**
- Works with both C-strings and `std::string`
- No `strlen()` overhead
- Modern C++17 standard
- Type-safe

**Default values:**
```cpp
// ✅ Use empty string for optional parameters
bool load_config(std::string_view file = "");

// ✅ Or use std::optional for "truly optional"
bool connect(std::optional<std::string_view> proxy = std::nullopt);

// ✅ Or use overloading
bool load_config();                              // Use defaults
bool load_config(std::string_view file);         // Use specified file
```

### 6.5 Fixed-Width Integer Types

**RULE:** Always use fixed-width types from `<cstdint>` for portability.

```cpp
// ✅ CORRECT
#include <cstdint>

uint8_t  flags;
uint16_t port;
uint32_t size;
uint64_t timestamp;
int32_t  error_code;

// ❌ WRONG - Platform-dependent sizes
unsigned char flags;    // Use uint8_t
unsigned short port;    // Use uint16_t
unsigned int size;      // Use uint32_t
long timestamp;         // Use int64_t

// Exceptions:
// - Use size_t for sizes/indices (STL compatibility)
// - Use int for loop counters (short scope, common idiom)
for (int i = 0; i < 10; ++i)
{
    // ...
}
```

---

## 7. File Organization

### 7.1 File Names

**RULE:** Match file name to primary class/struct name.

```cpp
// ✅ CORRECT
ServiceManager.hpp      // For ServiceManager class
ServiceManager.cpp
```

### 7.2 Header Guards

**RULE:** Always use traditional include guards (NOT #pragma once).

```cpp
// ✅ CORRECT
#ifndef AREG_IPC_SERVICE_MANAGER_HPP
#define AREG_IPC_SERVICE_MANAGER_HPP

namespace areg
{
class ServiceManager { };
} // namespace areg

#endif  // AREG_IPC_SERVICE_MANAGER_HPP

// ❌ WRONG - Do not use #pragma once
#pragma once
namespace areg::ipc
{
    class ServiceManager { };
}
```

**Why traditional guards for libraries:**

**Problem 1: Symlinks and multiple paths**
```bash
# Same file accessible via different paths
areg-sdk/include/areg/base/String.hpp
areg-sdk/include/areg/base/../base/String.hpp  (via symlink)
/usr/local/include/areg/base/String.hpp         (installed copy)

# With #pragma once: Compiler may treat these as DIFFERENT files
# With traditional guards: AREG_BASE_STRING_HPP prevents duplication
```

**Problem 2: Build system file copying**
```bash
# Build systems may copy headers to build directory
src/areg/base/String.hpp        (source)
build/cache/include/areg/base/String.hpp   (copied)

# With #pragma once: Both may be included
# With traditional guards: Second include is blocked
```

**Problem 3: Cross-platform reliability**
```cpp
// #pragma once relies on filesystem path comparison
// Different filesystems (case-sensitive vs insensitive) cause issues

// Windows: String.hpp == string.hpp (case-insensitive)
// Linux:   String.hpp != string.hpp (case-sensitive)

// Traditional guards: Always work regardless of filesystem
```

**Industry practice:** Google, LLVM, Boost, Qt all use traditional guards for libraries.

**Format:** `AREG_<PATH>_<FILENAME>_HPP`

### 7.3 Include Order

**RULE:** Follow this exact order, with blank lines between groups.

```cpp
// 1. Global header (if not already included transitively)
#include "areg/base/GEGlobal.h"

// 2. Corresponding header (for .cpp files)
#include "areg/ipc/ServiceManager.hpp"

// 3. Areg SDK headers
#include "areg/base/String.hpp"
#include "areg/ipc/Connection.hpp"

// 4. C++ standard library
#include <memory>
#include <string>
#include <vector>

// 5. C system headers
#include <sys/socket.h>
#include <unistd.h>

// 6. Third-party libraries
#include <boost/algorithm/string.hpp>
```

**Note:** In headers, `GEGlobal.h` should come first. In `.cpp` files, the corresponding header comes first (which typically includes `GEGlobal.h` transitively).

---

## 8. Class Design

### 8.1 Member Declaration Order

**RULE:** Follow this exact order within classes.

```cpp
class ServiceManager
{
//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
public:
    enum class State
    {
          Stopped
        , Running
    };

    static constexpr int MAX_CONNECTIONS { 100 };

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
    void start();
    void stop();

    inline State state() const;
    inline int connection_count() const;

//////////////////////////////////////////////////////////////////////////
// Protected methods
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
    String      mIpAddress;
    uint16_t    mPort;

private:
    State                                       mState;
    std::vector<std::unique_ptr<Connection>>    mConnections;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(ServiceManager);
};
```

**Section order:**
1. Public types and constants
2. Static methods
3. Constructors and destructor
4. Public methods
5. Protected methods
6. Private methods
7. Protected member variables
8. Private member variables
9. Deleted constructors / operators

**Recommendation (C.133):** Prefer private data with protected accessors over protected data members. Protected data creates tight coupling between base and derived classes — changing the member type or semantics silently breaks all subclasses. Protected accessors allow the base class to evolve its internals. However, protected data is acceptable when it significantly reduces verbosity and the inheritance hierarchy is small and controlled.

### 8.2 Constructor Initialization

**RULE:** Use initializer list with leading comma style. Do not use in-class member initializers for classes.

```cpp
// ✅ CORRECT - use initializer list
class Connection
{
public:
    Connection(int fd, std::string addr)
        : mSocketFd (fd)
        , mAddress  (std::move(addr))
        , mState    (ConnectionState::Disconnected)
    {
    }

private:
    int             mSocketFd;
    std::string     mAddress;
    ConnectionState mState;
};

// ❌ WRONG - Don't use in-class initializers for classes
class Connection
{
private:
    int mSocketFd = 0;              // NO - use initializer list instead
    std::string mAddress = "";      // NO - use initializer list instead
};
```

**Note:** In-class initializers are OK for structs only.

### 8.3 Inline Methods

- Use `inline` for simple getters/setters: declare in class, implement outside the class body in the same header.
- Never use `inline` with `virtual` methods.
- Avoid `inline` for static methods that instantiate static members.

```cpp
// In header - declare inline in class
class Connection
{
public:
    inline int32_t count() const;
    inline void set_count(int32_t value);
};

// In header - implement after class
inline int32_t Connection::count() const
{
    return mCount;
}

inline void Connection::set_count(int32_t value)
{
    mCount = value;
}
```

---

## 9. Complete Example

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

## 10. Quick Reference

### ✅ DO:
- Use PascalCase for types (classes, structs, enums, enum values)
- Use snake_case for functions and methods
- Use `mVariable` (mPascalCase) for class members
- Use `snake_case` for struct public fields (no prefix)
- Use `s_variable` for static class members
- Use `constexpr UPPER_CASE` for constants
- Use `std::string_view` for read-only string parameters
- Use `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t` for integers
- Use traditional header guards (NOT `#pragma once`)
- Use leading comma for enums/parameters/initializers
- Initialize all variables at declaration
- Use initializer lists for constructors
- Use in-class init for structs only
- Document public APIs
- Drop `get_` prefix for simple accessors: `count()` not `get_count()`

### ❌ DON'T:
- Use `#pragma once`
- Use `const char*` for parameters (use `std::string_view`)
- Use `unsigned int`, `unsigned short` (use `uint32_t`, `uint16_t`)
- Use `get_` prefix for accessors (use `count()` not `get_count()`)
- Use `IN`/`OUT`/`INOUT` empty macros
- Use in-class initializers for classes
- Use exceptions for error handling
- Use macros when alternatives exist (prefer `constexpr`, `inline`)
- Mix tabs and spaces
- Use `auto` for return types
- Use `NULL` or `0` for pointers (use `nullptr`)
- Use global variables with external linkage

---

**End of Style Guide**
