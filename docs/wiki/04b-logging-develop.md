# Developing with Areg Logging System

This guide provides instructions for activating, configuring, and utilizing Areg's comprehensive logging tools to monitor and debug multiprocess applications.

> [!TIP]
> Start with the Quick Start section to see logging in action, then explore advanced features for production deployment.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Overview](#overview)
3. [Compile-Time Configuration](#compile-time-configuration)
4. [Runtime Activation](#runtime-activation)
5. [Using Logging Scopes](#using-logging-scopes)
6. [Log Macros Reference](#log-macros-reference)
7. [Common Use Cases](#common-use-cases)
8. [Troubleshooting](#troubleshooting)

---

## Quick Start

### Add Logging in 2 Minutes

**Step 1: Enable logging in your application**

```cpp
#include "areg/appbase/Application.hpp"

int main()
{
    // Initialize with logging enabled (1st parameter = true)
    Application::initApplication(
        true,   // Enable logging ← Important
        true,   // Enable service manager
        false,  // Disable router (for this example)
        true,   // Enable timer
        false   // Disable watchdog
    );
    
    // Your application code
    Application::loadModel("MyModel");
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    
    Application::releaseApplication();
    return 0;
}
```

**Step 2: Add logging to your code**

```cpp
#include "areg/logging/GELog.h"

// Define log scope for this function
DEF_LOG_SCOPE(myapp_main_initialize);

void initialize()
{
    // Activate scope (tracks entry/exit)
    LOG_SCOPE(myapp_main_initialize);
    
    // Log messages at different levels
    LOG_DBG("Starting initialization");
    LOG_INFO("Configuration loaded");
    LOG_WARN("Using default settings");
    
    // ... your code ...
}
```

**Step 3: Run and see logs**

**Expected output in log file:**

```
2026-01-27 10:30:45.123: [ 12345  myapp.exe: Enter -->] Entering scope: myapp_main_initialize
2026-01-27 10:30:45.124: [ 12345  DEBUG >>> ] Starting initialization
2026-01-27 10:30:45.125: [ 12345  INFO >>> ] Configuration loaded
2026-01-27 10:30:45.126: [ 12345  WARN >>> ] Using default settings
2026-01-27 10:30:45.127: [ 12345  myapp.exe: Exit <-- ] Exiting scope: myapp_main_initialize
```

**Setup time:** ~2 minutes

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Overview

### What is Areg Logging System?

The Areg Framework features a robust, adaptable logging system designed for monitoring complex, multithreaded applications across distributed environments.

**Key capabilities:**
- ✅ Enable/disable logs at compile-time
- ✅ Control logs at runtime (no recompilation)
- ✅ Scope-based logging (track function entry/exit)
- ✅ Priority-based filtering (DEBUG, INFO, WARN, ERROR, FATAL)
- ✅ Multiple destinations (file, remote, debug output, database)
- ✅ Thread-safe logging
- ✅ Minimal performance overhead

---

### Logging Architecture

```
┌─────────────────┐
│  Application    │
│  ┌───────────┐  │
│  │ LOG_DBG() │──┼──► Log Message
│  └───────────┘  │
└─────────────────┘
         │
         ▼
┌─────────────────┐
│ Logging Module  │──► Scope Management
│                 │──► Priority Filtering
│                 │──► Message Formatting
└─────────────────┘
         │
         ▼
┌─────────────────────────────────────┐
│         Log Destinations            │
├─────────┬──────────┬────────┬───────┤
│  File   │  Remote  │ Debug  │  DB   │
└─────────┴──────────┴────────┴───────┘
```

---

### When to Use Logging

| Scenario                  | Use Logging   | Benefit                                                       |
|---------------------------|---------------|---------------------------------------------------------------|
| **Development**           | ✅ Always      | Debug issues, understand flow                                 |
| **Testing**               | ✅ Always      | Verify behavior, catch edge cases                             |
| **Production**            | ✅ Selective   | Monitor errors, track issues, reduce SCOPE, DEBUG and INFO    |
| **Performance Critical**  | ⚙️ Minimal     | Error/Fatal only, disable DEBUG and SCOPE                     |

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Compile-Time Configuration

### Enable or Disable Logging

Logging can be enabled or disabled during compilation for optimal performance.  
**Default:** Logging is **enabled**

---

### Using CMake

**Enable logging (default):**
```bash
cmake -B ./build -DAREG_LOGS=ON
cmake --build ./build
```

**Disable logging:**
```bash
cmake -B ./build -DAREG_LOGS=OFF
cmake --build ./build
```

**Expected output when enabled:**
```
-- Areg: >>> Logs: ENABLED
```

**Expected output when disabled:**
```
-- Areg: >>> Logs: DISABLED
```

---

### Using MSBuild (Visual Studio)

**Enable logging (default):**
```powershell
MSBuild /m /property:AregLogs=1 .\areg-sdk.sln
```

**Disable logging:**
```powershell
MSBuild /m /property:AregLogs=0 .\areg-sdk.sln
```

---

### Using Visual Studio Property

**Edit `msvc_setup.props`:**

```xml
<!-- Enable logging -->
<AregLogs>1</AregLogs>

<!-- Disable logging -->
<AregLogs>0</AregLogs>
```

**Location:** `areg-sdk/msvc_setup.props`

---

### Impact of Disabling Logs

**When compiled with `AREG_LOGS=OFF`:**
- ❌ All logging macros are compiled out (no-op)
- ❌ No runtime overhead
- ❌ Cannot enable logging at runtime
- ✅ Smaller binary size
- ✅ Maximum performance

**When compiled with `AREG_LOGS=ON`:**
- ✅ Logging available at runtime
- ✅ Can enable/disable dynamically
- ✅ Minimal overhead when disabled
- ⚙️ Slightly larger binary

> [!NOTE]
> For production builds where logging might be needed for debugging, compile with `AREG_LOGS=ON` but configure minimal logging in `areg.init`.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Runtime Activation

Once compiled with logging enabled, control logging at runtime using the Application class or macros.

---

### Method 1: Using Application Class (Recommended)

**Initialize with logging:**

```cpp
#include "areg/appbase/Application.hpp"

int main()
{
    // Initialize application with logging enabled
    Application::initApplication(
        true,   // startTracing   - Enable logging
        true,   // startServicing - Enable service manager
        false,  // startRouting   - Enable router client
        true,   // startTimer     - Enable timer manager
        false,  // startWatchdog  - Enable watchdog
        "./config/areg.init",  // Config file (optional)
        nullptr                // Log listener (optional)
    );
    
    // Application code
    Application::loadModel("MyModel");
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    
    // Cleanup (stops logging automatically)
    Application::releaseApplication();
    
    return 0;
}
```

**Parameters:**
- **startTracing (1st):** `true` = enable logging, `false` = disable
- **configFile (6th):** Path to `areg.init` configuration file
- **listener (7th):** Optional custom listener of configuration update

---

### Method 2: Explicit Logging Control

**Start logging explicitly:**

```cpp
#include "areg/appbase/Application.hpp"

int main()
{
    // Initialize without logging
    Application::initApplication(false, true, false, true, false);
    
    // Start logging later with specific config
    Application::startLogging("./config/areg.init", true);
    
    // Application code
    Application::loadModel("MyModel");
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    
    // Stop logging before cleanup
    Application::stopLogging();
    Application::releaseApplication();
    
    return 0;
}
```

**Methods:**
- `Application::startLogging(configFile, force)` - Start logging
  - `configFile` - Path to configuration file
  - `force` - Force restart if already running
- `Application::stopLogging()` - Stop logging

---

### Method 3: Using Macros

**For macro-based control:**

```cpp
#include "areg/logging/GELog.h"

int main()
{
    // Start logging with configuration
    LOGGING_CONFIGURE_AND_START("./config/areg.init");
    
    // Or force start without config file
    // LOGGING_FORCE_START();
    
    // Application code with logging
    LOG_INFO("Application started");
    
    // ... your code ...
    
    LOG_INFO("Application stopping");
    
    // Stop logging
    LOGGING_STOP();
    
    return 0;
}
```

**Available macros:**
- `LOGGING_CONFIGURE_AND_START(configFile)` - Start with config
- `LOGGING_FORCE_START()` - Start with default config
- `LOGGING_STOP()` - Stop logging

> [!IMPORTANT]
> If compiled with `AREG_LOGS=OFF`, these macros are no-ops and do nothing.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Using Logging Scopes

### What are Logging Scopes?

**Scopes** are named logging contexts that:
- Track function/method entry and exit
- Group related log messages
- Enable selective logging control
- Measure execution time

---

### Define and Use Scopes

**Step 1: Define scope (top of `.cpp` file or before each method)**

```cpp
#include "areg/logging/GELog.h"

// Define scope - use unique descriptive name
DEF_LOG_SCOPE(myapp_MyClass_processData);
DEF_LOG_SCOPE(myapp_MyClass_validate);
DEF_LOG_SCOPE(myapp_MyClass_save);
```

**Naming convention:**
```
<module_path_>_<class>_<function>
```

**Examples:**
- `network_tcp_HttpClient_sendRequest`
- `database_sqlite__Connection_executeQuery`
- `ui_view_MainWindow_handleClick`

---

**Step 2: Activate scope in function**

```cpp
void MyClass::processData(const Data& data)
{
    // Activate scope (logs entry/exit automatically)
    LOG_SCOPE(myapp_MyClass_processData);
    
    // Log messages within scope
    LOG_DBG("Processing data with ID: %d", data.id);
    
    if (!validate(data))
    {
        LOG_ERR("Data validation failed");
        return;
    }
    
    LOG_INFO("Data processed successfully");
}
```

**Expected output:**

```
2026-01-27 10:30:45.123: [ 12345  myapp.exe: Enter -->] Entering scope: myapp_MyClass_processData
2026-01-27 10:30:45.124: [ 12345  DEBUG >>> ] Processing data with ID: 42
2026-01-27 10:30:45.125: [ 12345  INFO >>> ] Data processed successfully
2026-01-27 10:30:45.126: [ 12345  myapp.exe: Exit <-- ] Exiting scope: myapp_MyClass_processData
```

---

### Nested Scopes

**Scopes can be nested:**

```cpp
DEF_LOG_SCOPE(myapp_MyClass_processData);
DEF_LOG_SCOPE(myapp_MyClass_validate);

void MyClass::validate(const Data& data)
{
    LOG_SCOPE(myapp_MyClass_validate);
    LOG_DBG("Validating data");
    
    // Validation logic
    
    LOG_DBG("Validation complete");
}

void MyClass::processData(const Data& data)
{
    LOG_SCOPE(myapp_MyClass_processData);
    LOG_INFO("Processing data");
    
    validate(data);  // Nested scope
    
    LOG_INFO("Processing complete");
}
```

**Expected output:**

```
2026-01-27 10:30:45.123: [ 12345  Enter -->] myapp_MyClass_processData
2026-01-27 10:30:45.124: [ 12345  INFO >>> ] Processing data
2026-01-27 10:30:45.125: [ 12345    Enter -->] myapp_MyClass_validate
2026-01-27 10:30:45.126: [ 12345    DEBUG >>> ] Validating data
2026-01-27 10:30:45.127: [ 12345    DEBUG >>> ] Validation complete
2026-01-27 10:30:45.128: [ 12345    Exit <-- ] myapp_MyClass_validate
2026-01-27 10:30:45.129: [ 12345  INFO >>> ] Processing complete
2026-01-27 10:30:45.130: [ 12345  Exit <-- ] myapp_MyClass_processData
```

---

### Scope Control in Configuration

**Enable/disable scopes in `areg.init`:**

```ini
# Enable all scopes
log::*::scope::* = DEBUG | SCOPE

# Enable specific scope
log::myapp::scope::myapp_MyClass_processData = DEBUG | SCOPE

# Disable specific scope
log::myapp::scope::myapp_MyClass_validate = NOTSET

# Enable scope with warnings only
log::myapp::scope::myapp_MyClass_save = WARN | SCOPE
```

**See:** [Logging Configuration Guide](./04a-logging-config.md) for details

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Log Macros Reference

### Priority Levels

| Macro         | Priority  | Use Case              | Example                                       |
|---------------|-----------|-----------------------|-----------------------------------------------|
| `LOG_DBG()`   | DEBUG     | Detailed debugging    | `LOG_DBG("Counter value: %d", count)`         |
| `LOG_INFO()`  | INFO      | General information   | `LOG_INFO("Connection established")`          |
| `LOG_WARN()`  | WARN      | Warnings              | `LOG_WARN("Retry attempt %d", retry)`         |
| `LOG_ERR()`   | ERROR     | Errors                | `LOG_ERR("Failed to open file: %s", path)`    |
| `LOG_FATAL()` | FATAL     | Fatal errors          | `LOG_FATAL("Out of memory")`                  |

---

### Logging Macros

**Basic logging:**

```cpp
LOG_DBG("Debug message");
LOG_INFO("Info message");
LOG_WARN("Warning message");
LOG_ERR("Error message");
LOG_FATAL("Fatal error message");
```

**Formatted logging:**

```cpp
LOG_DBG("Processing item %d of %d", current, total);
LOG_INFO("User '%s' logged in from %s", username, ipAddress);
LOG_WARN("Timeout after %d seconds", timeout);
LOG_ERR("Error code: %d, message: %s", errorCode, errorMsg);
```

---

### Scope Macros

```cpp
// Define scope (outside function, typically at top of file)
DEF_LOG_SCOPE(scope_name);

// Activate scope (inside function)
LOG_SCOPE(scope_name);
```

---

### Conditional Logging

**Log only if condition is true (conditional logging):**

```cpp
LOG_DBG_IF(condition, "Message only logged if condition is true");
LOG_INFO_IF(count > 0, "Processed %d items", count);
LOG_WARN_IF(retries > MAX_RETRIES, "Retry limit exceeded");
LOG_ERR_IF(result != SUCCESS, "Operation failed with code %d", result);
```

---

### Complete Example

```cpp
#include "areg/logging/GELog.h"

// Define scopes
DEF_LOG_SCOPE(network_HttpClient_connect);
DEF_LOG_SCOPE(network_HttpClient_sendRequest);

class HttpClient
{
public:
    bool connect(const String& url)
    {
        LOG_SCOPE(network_HttpClient_connect);
        LOG_INFO("Connecting to: %s", url.getString());
        
        bool success = performConnect(url);
        
        LOG_DBG_IF(success, "Connection established");
        LOG_ERR_IF(!success, "Connection failed");
        
        return success;
    }
    
    void sendRequest(const String& request)
    {
        LOG_SCOPE(network_HttpClient_sendRequest);
        LOG_DBG("Request: %s", request.getString());
        
        if (request.isEmpty())
        {
            LOG_WARN("Empty request");
            return;
        }
        
        // Send logic
        LOG_INFO("Request sent successfully");
    }
};
```

**Output:**

```
2026-01-27 10:30:45.123: [ 12345  Enter -->] network_HttpClient_connect
2026-01-27 10:30:45.124: [ 12345  INFO >>> ] Connecting to: http://example.com
2026-01-27 10:30:45.125: [ 12345  DEBUG >>> ] Connection established
2026-01-27 10:30:45.126: [ 12345  Exit <-- ] network_HttpClient_connect
2026-01-27 10:30:45.127: [ 12345  Enter -->] network_HttpClient_sendRequest
2026-01-27 10:30:45.128: [ 12345  DEBUG >>> ] Request: GET /api/data
2026-01-27 10:30:45.129: [ 12345  INFO >>> ] Request sent successfully
2026-01-27 10:30:45.130: [ 12345  Exit <-- ] network_HttpClient_sendRequest
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Common Use Cases

### Use Case 1: Basic Application Logging

**Goal:** Log application startup and main events

```cpp
#include "areg/appbase/Application.hpp"
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(main_application);

int main()
{
    // Enable logging
    Application::initApplication(true, true, false, true, false, "./config/areg.init", nullptr);
    
    LOG_SCOPE(main_application);
    LOG_INFO("Application started");
    
    // Load model
    LOG_DBG("Loading service model");
    Application::loadModel("MyServiceModel");
    LOG_INFO("Service model loaded");
    
    // Run application
    LOG_INFO("Application running, waiting for quit signal");
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    
    LOG_INFO("Application shutting down");
    Application::releaseApplication();
    
    return 0;
}
```

---

### Use Case 2: Class Method Logging

**Goal:** Track method calls and parameters

```cpp
#include "areg/logging/GELog.h"

// Define scopes for each method
DEF_LOG_SCOPE(database_Connection_connect);
DEF_LOG_SCOPE(database_Connection_execute);
DEF_LOG_SCOPE(database_Connection_disconnect);

class Connection
{
public:
    bool connect(const String& connectionString)
    {
        LOG_SCOPE(database_Connection_connect);
        LOG_INFO("Connecting to database");
        LOG_DBG("Connection string: %s", connectionString.getString());
        
        // Connection logic
        bool success = true;
        
        LOG_INFO_IF(success, "Connected successfully");
        LOG_ERR_IF(!success, "Connection failed");
        
        return success;
    }
    
    void execute(const String& query)
    {
        LOG_SCOPE(database_Connection_execute);
        LOG_DBG("Executing query: %s", query.getString());
        
        // Execute logic
        
        LOG_INFO("Query executed successfully");
    }
    
    void disconnect()
    {
        LOG_SCOPE(database_Connection_disconnect);
        LOG_INFO("Disconnecting from database");
        
        // Disconnect logic
        
        LOG_INFO("Disconnected");
    }
};
```

---

### Use Case 3: Error Handling with Logging

**Goal:** Log errors with context

```cpp
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(file_FileManager_openFile);

class FileManager
{
public:
    bool openFile(const String& filename)
    {
        LOG_SCOPE(file_FileManager_openFile);
        LOG_INFO("Opening file: %s", filename.getString());
        
        if (filename.isEmpty())
        {
            LOG_ERR("Invalid filename: empty string");
            return false;
        }
        
        File file(filename);
        if (!file.exists())
        {
            LOG_ERR("File not found: %s", filename.getString());
            return false;
        }
        
        if (!file.canRead())
        {
            LOG_ERR("No read permission for file: %s", filename.getString());
            return false;
        }
        
        LOG_DBG("File opened successfully");
        return true;
    }
};
```

---

### Use Case 4: Performance Monitoring

**Goal:** Track execution time with scopes

```cpp
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(processing_DataProcessor_processLargeDataset);

class DataProcessor
{
public:
    void processLargeDataset(const DataSet& data)
    {
        // Scope automatically tracks entry/exit time
        LOG_SCOPE(processing_DataProcessor_processLargeDataset);
        
        LOG_INFO("Starting processing of %d items", data.getSize());
        
        for (int i = 0; i < data.getSize(); ++i)
        {
            processItem(data.getItem(i));
            
            // Log progress every 1000 items
            if ((i + 1) % 1000 == 0)
            {
                LOG_INFO("Processed %d items", i + 1);
            }
        }
        
        LOG_INFO("Processing complete");
    }
    // Scope exit logged automatically with execution time
};
```

---

### Use Case 5: Conditional Debug Logging

**Goal:** Detailed logging only in debug builds

```cpp
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(algorithm_Sorter_sort);

class Sorter
{
public:
    void sort(Array<int>& array)
    {
        LOG_SCOPE(algorithm_Sorter_sort);
        LOG_INFO("Sorting array of size %d", array.getSize());
        
#ifdef DEBUG
        // Detailed logging only in debug builds
        LOG_DBG("Array before sort:");
        for (int i = 0; i < array.getSize(); ++i)
        {
            LOG_DBG("  [%d] = %d", i, array[i]);
        }
#endif
        
        // Sorting algorithm
        performSort(array);
        
#ifdef DEBUG
        LOG_DBG("Array after sort:");
        for (int i = 0; i < array.getSize(); ++i)
        {
            LOG_DBG("  [%d] = %d", i, array[i]);
        }
#endif
        
        LOG_INFO("Sorting complete");
    }
};
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### No Log Output

**Problem:** Application runs but no logs appear.  
**Solution:**

*1. Check logging is enabled at compile-time:*
```bash
# Verify AREG_LOGS is ON
cmake -B ./build -DAREG_LOGS=ON
cmake --build ./build
```

*2. Check logging is enabled at runtime:*

```cpp
Application::initApplication(
    true,   // Must be true for logging ← Check this
    true, true, false, true, false
);
```

*3. Check configuration file:*
```ini
# In areg.init
log::*::target = file
log::*::enable::file = true
log::*::scope::* = DEBUG | SCOPE
```

*4. Check log directory exists:*
```bash
mkdir -p ./logs
```

---

### Logs Not Showing for Specific Scope

**Problem:** Some scopes don't log.  
**Solution:**

*1. Check scope is defined:*
```cpp
// Must define before using
DEF_LOG_SCOPE(my_scope_name);
```

*2. Check scope is activated:*
```cpp
void myFunction()
{
    // Must activate scope
    LOG_SCOPE(my_scope_name);
    
    LOG_DBG("This message should appear");
}
```

*3. Check scope is enabled in config:*
```ini
# Verify scope not set to NOTSET
log::*::scope::my_scope_name = DEBUG | SCOPE
```

---

### Compile Errors with Logging Macros

**Problem:** Compiler errors with `LOG_DBG`, `LOG_INFO`, etc.  
**Solution:**

*1. Include logging header:*
```cpp
#include "areg/logging/GELog.h"
```

*2. Check AREG_LOGS is enabled:*
```bash
cmake -B ./build -DAREG_LOGS=ON
```

*3. Verify macro syntax:*
```cpp
// Correct
LOG_DBG("Message");
LOG_INFO("Value: %d", value);

// Wrong - missing quotes
LOG_DBG(Message);

// Wrong - missing format specifier
LOG_INFO(value);
```

---

### Performance Impact

**Problem:** Logging slows down application.  
**Solution:**

*1. Reduce log level in production:*
```ini
# Only warnings and errors
log::*::scope::* = WARN
```

*2. Disable verbose scopes:*
```ini
# Most scopes: errors only
log::*::scope::* = ERROR

# Critical scopes only: detailed
log::*::scope::critical_module = DEBUG | SCOPE
```

*3. Disable scope tracking:*
```ini
# Remove SCOPE from priorities
log::*::scope::* = WARN  # No | SCOPE
```

*4. For maximum performance, compile without logs:*
```bash
cmake -B ./build -DAREG_LOGS=OFF
```

---

### Scope Name Conflicts

**Problem:** Duplicate scope name errors.  
**Solution:**  
*Use unique, descriptive names:*
```cpp
// Wrong - too generic
DEF_LOG_SCOPE(process);
DEF_LOG_SCOPE(process);  // Duplicate!

// Correct - specific and unique
DEF_LOG_SCOPE(network_HttpClient_process);
DEF_LOG_SCOPE(database_Connection_process);
```

*Naming convention:*
```
<module_path_>_<class>_<function>
```

---

### Missing Log Messages

**Problem:** Some messages don't appear in logs.  
**Solution:**

*1. Check priority level:*
```ini
# If set to WARN, DEBUG and INFO won't show
log::*::scope::* = WARN

# Change to see all messages
log::*::scope::* = DEBUG | SCOPE
```

*2. Check conditional logging:*
```cpp
// Message only logged if condition true
LOG_DBG_IF(condition, "Message");

// Verify condition is actually true
```

*3. Flush logs before exit:*
```cpp
// Ensure all logs written before exit
Application::releaseApplication();
```

---

### Log File Permission Denied

**Problem:** Cannot write to log file.  
**Solution:**

*1. Check directory permissions:*
```bash
# Ensure write permissions
chmod 755 ./logs

# Or change log location
```

*2. Use different log location:*
```ini
# In areg.init
log::*::file::location = /tmp/myapp.log
```

*3. Run with appropriate permissions:*
```bash
# If logging to /var/log
sudo ./myapp
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Related Guides:**
- [Logging Configuration Guide](./04a-logging-config.md) - Detailed configuration options
- [Log Collector Guide](./XX-logcollector.md) - Remote log collection
- [Log Observer Guide](./04c-logobserver.md) - Runtime scope control
- [Lusan Live Log Viewer](./06f-lusan-live-logging.md) - GUI log viewer

**Source Code:**
- [Logging Module](../../framework/areg/logging) - Implementation
- [GELog.h](../../framework/areg/logging/GELog.h) - Logging macros

**Configuration:**
- [areg.init](../../framework/areg/resources/areg.init) - Default configuration

**Help:**
For questions, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech