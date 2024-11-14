# Developing with AREG Logging System

*This guide provides instructions for activating, configuring, and utilizing AREG's comprehensive logging tools to monitor and optimize multiprocess applications.*

The AREG Framework features a robust, adaptable logging system ideal for monitoring complex, multithreaded applications across distributed environments. This system allows developers to identify and troubleshoot timing-sensitive issues without slowing down application performance. This document explains how to enable and manage logging in the AREG Framework.

## Table of Contents

1. [General Information](#1-general-information)
2. [Compile-Time Log Activation](#2-compile-time-log-activation)
3. [Enabling \& Disabling Logs](#3-enabling--disabling-logs)
   - [Activating Logs via the `Application` Class](#activating-logs-via-the-application-class)
   - [Activating Logs via Macros](#activating-logs-via-macros)
4. [Scopes \& Logging](#4-scopes--logging)
5. [Logging Configuration](#5-logging-configuration)

---

## 1. General Information

The [AREG Framework](./../../framework/areg) includes a powerful [logging module](./../../framework/areg/logging). Key capabilities include:
   - Enabling/disabling logs at compile and runtime
   - Customizing logs by scope and priority for precise control

This guide provides steps to compile and configure the AREG logging module effectively.

---

## 2. Compile-Time Log Activation

Logging can be enabled or disabled during compilation, allowing effective debugging during development while optimizing performance in production. By default, logs are enabled. To adjust this setting:

1. **Using `cmake`:**  
   - Enable: `cmake -B ./build -DAREG_LOGS=ON`
   - Disable: `cmake -B ./build -DAREG_LOGS=OFF`
   
2. **Using `MSBuild`:**  
   - Enable: `MSBuild /m /property:AregLogs=1 ./areg-sdk.sln`
   - Disable: `MSBuild /m /property:AregLogs=0 ./areg-sdk.sln`

> [!NOTE]
> When compiled without logs, all log-related calls are ignored.

---

## 3. Enabling & Disabling Logs

Once compiled with logging enabled (`AREG_LOGS=ON`), logging can be managed as follows:

### Activating Logs via the `Application` Class

To initialize logging, use the `Application::initApplication()` method:

```cpp
Application::initApplication(startTracing, startServicing, startRouting, startTimer, startWatchdog, configFile, listener);
```

For specific configurations, use `Application::startLogging(configFile, force)`, where `configFile` specifies the configuration file path. To deactivate logging, use `Application::stopLogging()` or `Application::releaseApplication()`.

### Activating Logs via Macros

For macro-based control, `GELog.h` includes macros to activate and manage logging. Use `LOGGING_CONFIGURE_AND_START(configFile)` or `LOGGING_FORCE_START()` to start logging and `LOGGING_STOP()` to stop it.

```cpp
#include "areg/logging/GELog.h"

int main() {
    LOGGING_CONFIGURE_AND_START("./config/areg.init");
    // Code here
    LOGGING_STOP();
}
```

> ![NOTE]
> If compiled without logs (`AREG_LOGS=OFF`), these macros are non-functional.

---

## 4. Scopes & Logging

The AREG logging system uses *logging scopes* to manage logs by priority and message type. By assigning names to scopes, developers can focus on specific logging areas. For instance, in a `SomeClass` with methods `foo()` and `bar()`:

```cpp
#include "source/some/classes/SomeClass.hpp"
#include "areg/logging/GELog.h"

// Define scopes
DEF_LOG_SCOPE(source_some_classes_SomeClass_foo);
DEF_LOG_SCOPE(source_some_classes_SomeClass_bar);

void SomeClass::foo() {
    LOG_SCOPE(source_some_classes_SomeClass_foo);
    LOG_DBG("DEBUG message");
    LOG_INFO("INFO message");
}

void SomeClass::bar() {
    LOG_SCOPE(source_some_classes_SomeClass_bar);
    LOG_ERR("ERROR message");
}
```

Activated log scope enables developers to trace method calls and monitor execution times effectively.

---

## 5. Logging Configuration

Configure logging settings via a configuration file (e.g., `areg.init`) located in the `./config` folder post-compilation. This file allows defining log files, activating specific scopes, and setting message priorities. See the [Logging Configuration guide](./04a-logging-config.md) for detailed setup instructions, which enable customized log management and integration with the [Log Collector](./04d-logcollector.md).
