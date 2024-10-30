# Developing with AREG Logging System

*This logging documentation provides guidance on activating, configuring, and effectively using AREG's advanced logging tools, allowing you to monitor and optimize multiprocess applications.*

The AREG Framework offers a powerful and flexible logging system, tailored for developers monitoring multiprocess applications. This system is particularly valuable when debugging complex, multithreaded environments across multiple machines, where conventional debugging methods may slow applications and make time-sensitive bug analysis difficult. This document explains the steps to enable and use the logging system in the AREG Framework.

## Table of Contents

1. [Overview](#overview)
2. [Compile-Time Log Activation](#compile-time-log-activation)
3. [Enabling & Disabling Logs](#enabling-disabling-logs)
4. [Scopes & Logging](#scopes-logging)
5. [Logging Configuration](#logging-configuration)

---

## Overview

The [AREG Framework](./../../framework/areg) includes a robust [logging module](./../../framework/areg/trace) called `trace`. Key features include:
   - Enabling/disabling logs at compile and runtime
   - Customizing logs by scope and priority for flexibility and precision

This guide provides steps to compile and configure the AREG logging module effectively.

---

## Compile-Time Log Activation

Developers can enable or disable logging during compilation, allowing efficient debugging in development while minimizing latency in production builds. By default, logs are enabled. To change this setting:

1. **Using `cmake`:**  
   - To enable: `cmake -B ./build -DAREG_LOGS=ON`
   - To disable: `cmake -B ./build -DAREG_LOGS=OFF`
   
2. **Using `MSBuild`:**  
   - To enable: `MSBuild /m /property:AregLogs=1 ./areg-sdk.sln`
   - To disable: `MSBuild /m /property:AregLogs=0 ./areg-sdk.sln`

> **Note:** When compiled without logs, all log-related calls are ignored.

---

## Enabling & Disabling Logs

Once compiled with logs enabled (`AREG_LOGS=ON`), logging can be activated in several ways.

### Activating Logs via `Application` Class

To initialize logging, use the static method `Application::initApplication()`:

```cpp
Application::initApplication(startTracing, startServicing, startRouting, startTimer, startWatchdog, configFile, listener);
```

Alternatively, use `Application::startTracer(configFile, force)` for a targeted approach, where the `configFile` parameter specifies the path to the configuration file. Deactivate logging using `Application::stopTracer()` or `Application::releaseApplication()`.

### Activating Logs via Macros

For macro-based control, `GETrace.h` offers several macros to activate and manage logging. To activate logging, use `TRACER_CONFIGURE_AND_START(configFile)` or `TRACER_FORCE_LOGGING()`. To deactivate, use `TRACER_STOP_LOGGING()`.

```cpp
#include "areg/trace/GETrace.h"

int main() {
    TRACER_CONFIGURE_AND_START("./config/areg.init");
    // Code here
    TRACER_STOP_LOGGING();
}
```

> [!NOTE]
> If compiled without logs (`AREG_LOGS=OFF`), these macros have no effect.

---

## Scopes & Logging

The AREG Framework’s logging system uses *logging scopes* to manage and filter logs by priority and message type. By assigning unique names to scopes, developers can enable specific logging areas within an application. Below is an example for a class `SomeClass` with methods `foo()` and `bar()`.

```cpp
#include "source/some/classes/SomeClass.hpp"
#include "areg/trace/GETrace.h"

// Define scopes
DEF_TRACE_SCOPE(source_some_classes_SomeClass_foo);
DEF_TRACE_SCOPE(source_some_classes_SomeClass_bar);

void SomeClass::foo() {
    TRACE_SCOPE(source_some_classes_SomeClass_foo);
    TRACE_DBG("DEBUG message");
    TRACE_INFO("INFO message");
}

void SomeClass::bar() {
    TRACE_SCOPE(source_some_classes_SomeClass_bar);
    TRACE_ERR("ERROR message");
}
```

When scopes are activated, the logging system records all relevant messages, enabling developers to trace the flow of method calls and monitor execution durations.

---

## Logging Configuration

Configure logging settings using a configuration file (e.g., `areg.init`) located in the `./config` folder post-compilation. This file allows customizations such as defining log files, activating specific scopes, and setting message priorities. Refer to [Logging Configuration](./logging-config.md) for detailed setup instructions, which enable tailored log management and connection to the [Log Collector](./logger.md).
