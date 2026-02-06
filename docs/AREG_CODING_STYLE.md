# Areg SDK Coding Style Guide

**Version:** 2.0  
**Date:** February 2026  
**Target:** Cross-platform IPC middleware (Linux, macOS, Windows, Embedded Linux, HPC)

---

## Philosophy

Areg SDK follows modern C++ conventions optimized for cross-platform development. This style emphasizes:

- **Modern C++17** idioms
- **STL compatibility** (snake_case functions)
- **Clear type distinctions** (PascalCase types)
- **Pragmatic flexibility** without sacrificing consistency

---

## 1. Naming Conventions

### 1.1 Quick Reference

| Element                   | Convention                | Example                                   |
|---------------------------|---------------------------|-------------------------------------------|
| Classes                   | PascalCase                | `ServiceManager`                          |
| Structs                   | PascalCase                | `ConnectionData`                          |
| Functions/Methods         | snake_case                | `start_service()`                         |
| Local Variables           | snake_case OR camelCase   | `connection_count` OR `connectionCount`   |
| Class Member Variables    | `m` + PascalCase          | `mSocketFd`                               |
| Struct Member Variables   | camelCase                 | `maxRetries`, `timeoutMs`                 |
| Static Members            | `s_` + snake_case         | `s_instance_count`                        |
| Global Variables          | `g_` + snake_case         | `g_max_connections` (avoid if possible)   |
| Constants                 | constexpr UPPER_SNAKE     | `MAX_BUFFER_SIZE`                         |
| Enum Classes              | PascalCase                | `ConnectionState`                         |
| Enum Values               | snake_case                | `ConnectionState::connected`              |
| Macros                    | `AREG_` + UPPER_SNAKE     | `AREG_ASSERT`                             |
| Template Parameters       | PascalCase                | `template<typename MessageType>`          |
| Namespaces                | snake_case                | `areg::ipc`                               |

---

### 1.2 Classes and Structs

**Classes and Structs:** PascalCase for type names

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

**Struct Members:** camelCase (no `m` prefix)

```cpp
// ✅ CORRECT - struct members are camelCase
struct ConnectionConfig
{
    int maxRetries{3};
    int timeoutMs{5000};
    bool autoReconnect{true};
    std::string serverAddress{"localhost"};
};

// Usage
ConnectionConfig config;
config.maxRetries = 5;

// ❌ WRONG - don't use m prefix for struct members
struct ConnectionConfig
{
    int mMaxRetries{3};        // NO - structs don't use m prefix
    int m_timeout_ms{5000};    // NO - wrong style
};
```

**Rationale:** 
- Structs are simple data holders (POD-like)
- No `m` prefix needed (no private/public distinction)
- camelCase keeps them lightweight and distinct from class members

---

### 1.3 Functions and Methods

```cpp
// ✅ CORRECT
class ServiceManager
{
public:
    void start_service();
    void stop_service();
    [[nodiscard]] int get_connection_count() const;
    [[nodiscard]] bool is_connected() const;
};

// Virtual methods - child classes always use both 'virtual' and 'override' for derived virtual methods
class DerivedService : public ServiceBase
{
public:
    virtual void start_service() override;
    virtual void stop_service() override;
};

// ❌ WRONG
void startService();    // No camelCase
void StartService();    // No PascalCase
```

**Note:** You may use `virtual` keyword with `override` for better readability.

---

### 1.4 Variables

#### Local Variables: snake_case OR camelCase (flexible)

```cpp
// ✅ BOTH ACCEPTABLE
int connection_count = 0;           // snake_case
int connectionCount = 0;            // camelCase
std::string serverAddress;          // camelCase
auto service = std::make_unique<ServiceManager>();

// Choose one style per file and be consistent within that file
```

#### Member Variables: `m` + PascalCase

```cpp
class Connection
{
public:
    Connection(int fd, std::string addr)
        : mSocketFd(fd)
        , mIpAddress(std::move(addr))
        , mState(ConnectionState::disconnected)
    {
    }

private:
    // 👉 Optionally, Align member variables for better readability
    int             mSocketFd;
    std::string     mIpAddress;
    ConnectionState mState;
};
```

#### Static Members

- Use **`s_` + snake_case** for class member variables.
- Use **`_` + snake_case** for static variables in methods.
- Use **`constexpr` + UPER_SNAKE** for constants declared as static.

```cpp
class ServiceManager
{
    // constants can be declared on the top
    static constexpr int MAX_SERVICES   { 100 };
    
public:
    static ServiceManager& get_instance()
    {
        static ServiceManager _instance;
        return _instance;
    }
    
private:
    static int  s_instance_count;
};
```

#### Global Variables: `g_` + snake_case (avoid if possible)

```cpp
namespace areg::config
{
    int g_max_connections = 100;
}
```

---

### 1.5 Constants

```cpp
// ✅ CORRECT - constexpr with UPPER_SNAKE_CASE
// If possible, align constants and use initializers
constexpr int       MAX_BUFFER_SIZE     { 4096 };
constexpr size_t    DEFAULT_TIMEOUT_MS  { 5000 };

// ❌ AVOID macros
#define MAX_BUFFER_SIZE 4096  // Use constexpr instead
```

Constants in the classes can be declared at the top together with class specific types, even if they are `private`
```cpp
class Connection
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////

    // built-in types declared first
    enum class ConnectionState
    {
          disconnected
        , connecting
        , connected
        , error
    };

    // constants can be declared on the top
    static constexpr int MAX_RETRIES    { 100 };
};
```

---

### 1.6 Enumerations

**Leading comma style for better alignment:**

```cpp
// ✅ CORRECT
enum class ConnectionState
{
      disconnected
    , connecting
    , connected
    , error
};

// Usage
ConnectionState state = ConnectionState::connected;
```

---

### 1.7 Template Parameters

```cpp
// ✅ CORRECT
template<typename MessageType, typename HandlerType>
class MessageDispatcher { };

template<typename T>
class Container { };
```

---

### 1.8 Namespaces

```cpp
// ✅ CORRECT
namespace areg::ipc::internal
{
    // Implementation
}

// Or pre-C++17 style
namespace areg
{
namespace ipc
{
    // declarations
}
}
```

---

## 2. Macros

### 2.1 Naming

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

1. `constexpr` for constants
2. `inline` functions for utilities
3. Templates for type-generic code
4. Macros only for 
   * conditional compilation
     ```cpp
     #ifdef POSIX
     #endif // POSIX
     ```
   * visibility
     ```cpp
     #ifdef AREG_ENABLE_LOGS
        #define LOG_DBG(x)  logger.logDebug(x)
     #else
        #define LOG_DBG(x)
     #endif
     ```
   * code generation: always avoid using, except some special case.
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

---

## 3. Code Layout

### 3.1 Indentation

- **4 spaces** per level
- **NO tabs**

### 3.2 Braces - Allman Style

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
    void start()
    {
        initialize();
    }
};
```

### 3.3 Line Length

- **Soft limit:** 100 characters
- **Hard limit:** 120 characters

```cpp
// ✅ Leading comma for better alignment
auto connection = service_manager.create_connection(
      server_address
    , port_number
    , connection_timeout
);
```

### 3.4 Spacing

```cpp
// ✅ Spaces after keywords, around operators
if (condition)
{
    int result = a + b;
}

for (int i = 0; i < count; ++i)
{
    process(i);
}

// ✅ Parentheses for clarity
if ((value == 1) || ((value == 2) && ((flags & 0x0F) != 0)))
{
    process();
}
```

Align variables in the class if possible or looks nicer:
```cpp
class Connection
{
public:
    Connection(int fd, std::string addr)
        : mSocketFd(fd)
        , mIpAddress(std::move(addr))
        , mState(ConnectionState::disconnected)
    {
    }

private:
    // 👉 Optionally, Align member variables for better readability
    int             mSocketFd;
    std::string     mIpAddress;
    ConnectionState mState;
};
```

### 3.5 Inline Methods

```cpp
// ✅ PREFERRED - clean interface
class Size
{
public:
    //!< Returns width
    inline uint32_t width() const;
    //< Returns height
    inline uint32_t height() const;
    
private:
    uint32_t mWidth;
    uint32_t mHeight;
};

inline uint32_t Size::width() const
{
    return mWidth;
}

inline uint32_t Size::height() const
{
    return mHeight;
}
```

---

## 4. Comments and Documentation

### 4.1 Comment Style

```cpp
// Single-line comments

/**
 * Multi-line Doxygen comments.
 * Aligned asterisks.
 **/

/// Alternative single-line Doxygen
```

### 4.2 Function Documentation

```cpp
/**
 * \brief   Establishes connection to remote service.
 * 
 * Attempts to connect with retry logic.
 * 
 * \param   address     Service address (hostname or IP)
 * \param   port        Service port (1-65535)
 * \param   timeout_ms  Connection timeout in milliseconds
 * 
 * \return  0 on success, negative errno on failure:
 *          -EINVAL         Invalid parameters
 *          -ECONNREFUSED   Connection refused
 *          -ETIMEDOUT      Connection timeout
 */
int connect_service(const std::string& address, uint16_t port, uint32_t timeout_ms);
```

---

## 5. Modern C++ Features

### 5.1 Smart Pointers

```cpp
// ✅ CORRECT
auto service = std::make_unique<MessageService>();
auto shared_conn = std::make_shared<Connection>();

// ❌ AVOID manual memory management
MessageService* service = new MessageService();
```

### 5.2 References vs Pointers

```cpp
// ✅ References for non-optional parameters
bool start_connection(Connection& conn);

// ✅ Pointers for optional parameters
void log_with_prefix(const std::string& msg, const char* prefix = nullptr);

// ✅ std::optional for modern code
void connect(const std::string& addr, std::optional<uint16_t> port = std::nullopt);
```

### 5.3 auto Keyword

```cpp
// ✅ GOOD uses
auto conn = std::make_unique<Connection>();
auto iter = container.begin();

for (const auto& item : items)
{
    process(item);
}

// ✅ Prefer explicit when clarity matters
ConnectionState state = get_state();

// ❌ NEVER for return types, return type must be clear
auto create_connection();  // What does this return?
```

### 5.4 constexpr and const

```cpp
// ✅ Use constexpr for compile-time constants
constexpr int   BUFFER_SIZE = 4096;
constexpr auto  TIMEOUT = std::chrono::seconds(30);

// ✅ Mark non-modifying methods const
class ServiceManager
{
public:
    int get_count() const { return mCount; }
    bool is_running() const { return mRunning; }
    
private:
    int  mCount;
    bool mRunning;
};
```

### 5.5 Member Initialization

**Structs:** Use in-class initializers

```cpp
// ✅ GOOD for structs
struct ConnectionConfig
{
    int maxRetries      {3};
    int timeoutMs       {5000};
    bool autoReconnect  {true};
};
```

**Classes:** Prefer constructor initialization. In-class init acceptable for simple cases.

**✅ PREFERRED - Simple class with single visibility section**
```cpp
class SimpleConfig
{
private:
    int mMaxConnections{100};
    int mTimeoutMs{5000};
    bool mAutoRetry{true};
};
// All members in one section - in-class init is clear
```

**✅ PREFERRED - Complex class with mixed visibility**
```cpp
class Connection
{
public:
    Connection()
        : mPublicCounter(0)
        , mProtectedCounter(0)
        , mPrivateCounter(0)
    {}
    
public:
    int mPublicCounter;
    
protected:
    int mProtectedCounter;
    
private:
    int mPrivateCounter;
};
// Defaults scattered across sections - constructor keeps it in one place
```

**⚠️ AVOID - Mixed visibility with in-class init**
```cpp
class Connection
{
public:
    // functions here
    
    int mPublicCounter{0};      // Default here
    
protected:
    // functions here
    
    int mProtectedCounter{0};   // Default here
    
private:
    // functions here
    
    int mPrivateCounter{0};     // Default here
};
// Messy - defaults scattered across sections
```

**Why initialize classes in constructor:**

**Reason 1: One location for all initialization**
```cpp
// With in-class init - defaults scattered across class sections
class ServiceManager
{
public:
    int mPublicCounter{0};     // ← Default here
    
protected:
    int mProtectedCounter{0};  // ← Default here
    
private:
    int mPrivateCounter{0};    // ← Default here
};
// Where are the defaults? Scattered across public/protected/private sections

// With constructor init - all defaults in ONE place
class ServiceManager
{
public:
    ServiceManager()
        : mPublicCounter(0)
        , mProtectedCounter(0)
        , mPrivateCounter(0)
    {
    }
    // All initialization in one location - easy to review
    
public:
    int mPublicCounter;
    
protected:
    int mProtectedCounter;
    
private:
    int mPrivateCounter;
};
```

**Reason 2: Clarity - no confusion about overrides**
```cpp
// With in-class init - which value does member actually have?
class Connection
{
public:
    Connection(int timeout)
    // Question: Is mTimeout = timeout or 5000?
    // Answer: Need to check if constructor in source code overrides default
    
private:
    int mTimeout{5000};  // Default value here, but might be overridden
};

// With constructor init - value is obvious
class Connection
{
public:
    Connection(int timeout) : mTimeout(timeout) {}
    // Clear: mTimeout = timeout
    
    Connection() : mTimeout(5000) {}
    // Clear: mTimeout = 5000
    
private:
    int mTimeout;  // Value set in constructor, look there
};
```

**Reason 3: Forces discipline and completeness**
```cpp
// Constructor initialization forces listing ALL members
class Connection
{
public:
    Connection()
        : mSocketFd(-1)
        , mBytesReceived(0)
        , mBytesSent(0)
        , mTimeout(5000)
        // Compiler warns if mIsConnected is missing
    {
    }
    
private:
    int         mSocketFd;
    uint32_t    mBytesReceived;
    uint32_t    mBytesSent;
    int         mTimeout;
    bool        mIsConnected;  // Forgot to initialize - compiler WARNING
};
```

**Reason 4: Not all members can be initialized in-class**
```cpp
class Connection
{
public:
    Connection(Socket& socket)
        : mSocket(socket)      // Reference - MUST be in constructor
        , mSocketFd(-1)        // Initialize with reference member
        , mBytesReceived(0)
        , mBytesSent(0)
    {
    }
    
private:
    Socket&     mSocket;           // Cannot use in-class init
    int         mSocketFd;
    uint32_t    mBytesReceived;
    uint32_t    mBytesSent;
};
// If some members MUST be in constructor (references, pointers),
// put ALL members there for consistency
```

**Summary:**
- **Structs:** Simple data holders, in-class init is fine
- **Classes:** Complex objects, all initialization in constructor (source code) keeps it clear and complete

### 5.6 Range-based For Loops

```cpp
// ✅ Prefer range-for
for (auto& connection : connections)
{
    connection.update();
}

for (const auto& message : message_queue)
{
    log(message);
}
```

### 5.7 Structured Bindings (C++17)

```cpp
// ✅ Clean unpacking
auto [success, result] = execute_operation();

for (const auto& [key, value] : config_map)
{
    apply_setting(key, value);
}
```

---

## 6. Error Handling

### 6.1 Return Codes

```cpp
// ✅ Return 0 for success, negative errno on failure
int connect_to_service(const std::string& address, uint16_t port)
{
    if (address.empty())
    {
        return -EINVAL;
    }
    
    if (socket_connect_failed)
    {
        return -ECONNREFUSED;
    }
    
    return 0;
}
```

### 6.2 Exceptions

```cpp
// ❌ DO NOT use exceptions (embedded incompatible)
void process_data()
{
    throw std::runtime_error("Failed");  // Don't do this
}

// ✅ Return error codes
int process_data()
{
    if (invalid_state)
    {
        return -EINVAL;
    }
    return 0;
}
```

### 6.3 std::optional

```cpp
// ✅ Use for "may not have value"
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

---

## 7. File Organization

### 7.1 File Names

```cpp
// ✅ CORRECT
ServiceManager.hpp      // For ServiceManager class
ServiceManager.cpp

string_utils.hpp        // For utilities
```

### 7.2 Header Guards

**Always use traditional include guards (NOT #pragma once)**.

```cpp
// ✅ CORRECT
#ifndef AREG_IPC_SERVICE_MANAGER_HPP
#define AREG_IPC_SERVICE_MANAGER_HPP

namespace areg::ipc
{
    class ServiceManager { };
}

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

```cpp
// 1. Corresponding header
#include "areg/ipc/ServiceManager.hpp"

// 2. C system headers
#include <sys/socket.h>
#include <unistd.h>

// 3. C++ standard library
#include <memory>
#include <string>
#include <vector>

// 4. Third-party libraries
#include <boost/algorithm/string.hpp>

// 5. Areg SDK headers
#include "areg/base/String.hpp"
#include "areg/ipc/Connection.hpp"
```

---

## 8. Class Design

### 8.1 Member Declaration Order

```cpp
class ServiceManager
{
// 1. Public types and constants
public:
    enum class State
    {
          stopped
        , running
    };
    
    static constexpr int MAX_CONNECTIONS = 100;

// 2. Static methods
public:
    static ServiceManager& get_instance();

// 3. Constructors and destructor
public:
    ServiceManager();
    virtual ~ServiceManager();
    
// 4. Public methods
public:
    void start();
    void stop();
    
    State get_state() const;
    int get_connection_count() const;

// 5. Protected methods
protected:
    virtual void on_start();

// 6. Private methods
private:
    void initialize_subsystems();
    void cleanup_connections();

// 7. Member variables
protected:
    String      mIpAddress;
    uint16_t    mPort;
private:
    State mState;
    int mConnectionCount;
    std::vector<std::unique_ptr<Connection>> mConnections;

// 8. deleted constructors and operators
private:
    ServiceManager(const ServiceManager&) = delete;
    ServiceManager& operator=(const ServiceManager&) = delete;
};
```

### 8.2 Constructor Initialization

```cpp
// ✅ CORRECT - use initializer list
class Connection
{
public:
    Connection(int fd, std::string addr)
        : mSocketFd(fd)
        , mAddress(std::move(addr))
        , mState(ConnectionState::disconnected)
    {
    }
    
private:
    int             mSocketFd;
    std::string     mAddress;
    ConnectionState mState;
};
```

---

## 9. Complete Example

```cpp
// areg/ipc/ServiceManager.hpp
#ifndef AREG_IPC_SERVICE_MANAGER_HPP
#define AREG_IPC_SERVICE_MANAGER_HPP

#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace areg::ipc
{

class Connection;

enum class ServiceState
{
      stopped
    , starting
    , running
    , stopping
};

/**
 * \brief   Manages lifecycle of IPC services.
 */
class ServiceManager
{
public:
    ServiceManager();
    ~ServiceManager();
    
public:
    int start_service(const std::string& config_path);
    void stop_all_services();
    
    std::optional<Connection> create_connection(const std::string& address);
    void remove_connection(int connection_id);
    
    ServiceState get_state() const { return mState; }
    size_t get_connection_count() const { return mConnections.size(); }
    bool is_running() const { return mState == ServiceState::running; }
    
private:
    void initialize_subsystems();
    void cleanup_connections();
    
protected:
    String      mIpAddress;
    uint16_t    mPort;
private:
    ServiceState mState;
    std::vector<std::shared_ptr<Connection>> mConnections;

private:
    ServiceManager(const ServiceManager&) = delete;
    ServiceManager& operator=(const ServiceManager&) = delete;    
};

}  // namespace areg::ipc

#endif  // AREG_IPC_SERVICE_MANAGER_HPP
```

```cpp
// areg/ipc/ServiceManager.cpp
#include "areg/ipc/ServiceManager.hpp"

#include <errno.h>

#include <algorithm>

#include "areg/ipc/Connection.hpp"

namespace areg::ipc
{

ServiceManager::ServiceManager()
    : mState(ServiceState::stopped)
    , mConnections()
{
    initialize_subsystems();
}

ServiceManager::~ServiceManager()
{
    if (is_running())
    {
        stop_all_services();
    }
}

int ServiceManager::start_service(const std::string& configPath)
{
    if (configPath.empty())
    {
        return -EINVAL;
    }
    
    mState = ServiceState::starting;
    
    // Implementation...
    
    mState = ServiceState::running;
    return 0;
}

void ServiceManager::stop_all_services()
{
    mState = ServiceState::stopping;
    cleanup_connections();
    mState = ServiceState::stopped;
}

std::optional<Connection> ServiceManager::create_connection(const std::string& address)
{
    if (!is_running())
    {
        return std::nullopt;
    }
    
    auto connection = std::make_shared<Connection>(address);
    mConnections.push_back(connection);
    
    return *connection;
}

void ServiceManager::cleanup_connections()
{
    mConnections.clear();
}

void ServiceManager::initialize_subsystems()
{
    // Implementation...
}

}  // namespace areg::ipc
```

---

## 10. Quick Reference

### ✅ DO:
- Use PascalCase for types
- Use snake_case for functions
- Use `mVariable` for members
- Use `s_variable` for static members
- Use constexpr UPPER_CASE for constants
- Use traditional header guards
- Use leading comma for enums/params
- Initialize class members in constructor
- Use in-class init for structs only
- Document public APIs

### ❌ DON'T:
- Use #pragma once
- Use in-class initializers for classes
- Use exceptions
- Use macros when alternatives exist
- Mix tabs and spaces
- Use auto for return types

---

**End of Style Guide**
