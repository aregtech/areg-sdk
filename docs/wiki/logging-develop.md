# AREG Logging System

AREG Framework has powerful and flexible logging system to use for developers who want to monitor multiprocessing applications behavior during runtime. Since the multiprocessing applications may run on one or several machines, the debugging application is extremely hard, it is time and resource consuming. In this case, it is more effective to collect and analyze the logs. Additionally, the debugging always makes applications run slower, which makes time related bug analyzes in multithreaded and multiprocessing environment nearly impossible. This document explains how to enable compilation with logs, how to use it and how it works.

## Table of Contents

1. [General Information](#general-information)
2. [Compile-Time Log Enabling](#compile-time-log-enabling)
3. [Log Activation and Deactivation](#log-activation-and-deactivation)
   - [Activate via `Application` class](#activate-via-application-class)
   - [Activate via Macros](#activate-via-macros)
4. [Scopes and Logs](#scopes-and-logs)
5. [Logging Configuration](#logging-configuration)

---

## General Information

The [AREG communication engine](./../../framework/areg) (or AREG Framework) comes with an advanced [logging module](./../../framework/areg/trace) called `trace` that can be seamlessly integrated into any application. This logging module possesses several notable features, including:

1. Compile-time log enabling and disabling.
2. Runtime log enabling and disabling for all or specific log scopes.
3. Runtime log enabling and disabling by priority for all or specific log scopes.

This document aims to provide a comprehensive, step-by-step guide for compiling and configuring the AREG logging module.

---

## Compile-Time Log Enabling

Applications built using the AREG Framework have the ability to enable or disable logs at compile-time. This feature is particularly useful for developers who want to monitor the application's behavior in Debug mode, and minimize resource usage and achieving ultra-low latency in Release builds. By utilizing compile-time switches, developers can compile the applications with or without logs.

By default, applications are compiled with logs and enabled logging. To enable or disable logging during compilation, follow these steps:

1. Using `cmake`  : configure and compile with option `-DAREG_LOGS=ON` to enable and with option `-DAREG_LOGS=OFF` to disable logs.
   - **Example**: `cmake -B ./build -DAREG_BUILD_TYPE=Release -DAREG_LOGS:BOOL=OFF`
2. Using `MSBuild`: configure and compile with property `/property:AregLogs=1` to enable and with property `/property:AregLogs=0`to disable logs.
   - **Example**: `MSBuild /m /property:Configuration=Release /property:AregLogs=0 ./areg-sdk.sln`

> [!IMPORTANT]
> If the sources are compiled without logs, any calls to activate or deactivate the logs, or log messages will be ignored.

---

## Log Activation and Deactivation

When an application is compiled with logs enabled (option `AREG_LOGS` is `ON`), the logging module needs to be set up to initialize and start the logging engine. Once the logging engine is started, the application can begin to log the messages. Otherwise, the system will discard log messages. There are several methods available to configure and start logging.

### Activate via `Application` class

The most common approach to configuring, starting, or stopping the logging module is by calling the static methods of the [Application](./../../framework/areg/appbase/Application.hpp) class.

**Activating Logging:**

To activate the logging module using the `Application` class, invoke the static method `initApplication()`:

```cpp
static void initApplication(  bool startTracing                 /* starts logging module                                    */
                            , bool startServicing               /* starts service manager                                   */
                            , bool startRouting                 /* starts multi-cast router client                          */
                            , bool startTimer                   /* starts timer manager                                     */
                            , bool startWatchdog                /* starts watchdog manager                                  */
                            , const char * configFile           /* relative or absolute path of application configuration   */
                            , IEConfigurationListener * listen  /* configuration listener to be notify on configuration     */ );
```

Developers can specify whether logging is enabled or disabled, as well as provide the relative or absolute path to the configuration file. By default, applications start with logging enabled (`startTracing` parameter) and utilize the default configuration path (`./config/areg.init` for `fileTraceConfig` parameter).

Another method to activate logs using the `Application` class is by calling the static method `startTracer()`:

```cpp
static bool startTracer(const char* configFile = nullptr, bool force = false);
```

- The `configFile` parameter indicates the relative or absolute path to the application configuration file. By default, path (`./config/log.init`) is used.
- The `force` parameter determines whether logging should be forcibly started if the configuration file is not found. When set to `true`, if logging fails to configure, the application activates the logging module with default settings. By default, logging is not forced to start if there is a failure in configuring the logging.

Alternatively, developers have the option to configure logging separately by calling the `Application::tracerConfig()` and start logging by invoking `Application::startTracer()`.

**Deactivate Logging:**

To deactivate the logging module either call `Application::stopTracer()` or `Application::releaseApplication()`, which stops all services in the application.

Here's an example demonstrating build and release with default settings (enabled loging):

```cpp
#include "areg/appbase/Application.hpp"

int main(void)
{
    Application::initApplication();      // Initialize with default parameters
    // Your code is here...
    Application::releaseApplication();   // Stop all services and release resources
    return 0;
}
```

In the above example, logging is activated during the initialization phase using `Application::initApplication()`, and later stopped by calling `Application::releaseApplication()` to ensure proper cleanup and resource deallocation.

### Activate via Macros

The file [GETrace.h](./../../framework/areg/trace/GETrace.h) offers a collection of macros that simplify the activation and deactivation of the logging module. It's essential to understand that these macros are designed to handle the activation of the log module **only when the sources are compiled with logs enabled** (option `AREG_LOGS=1`). Consequently, developers can utilize these macros within their source code, knowing that the corresponding wrapped methods will be invoked only if the sources are compiled with logs (`AREG_LOGS=ON`).

**Activate Logging:**

To activate and start logging, developers can utilize either the `TRACER_START_LOGGING(configFile)` or `TRACER_CONFIGURE_AND_START(configFile)` macros. The `configFile` parameter allows for specifying the relative or absolute path to the log configuration file.

Alternatively, if developers wish to force the activation of logging, they can simply call the `TRACER_FORCE_LOGGING()` macro without any parameters. In this scenario, if logging has not been activated yet, it will start with default parameters, disregarding the configuration file.

**Deactivate Logging:**

To deactivate logging, developers can use the `TRACER_STOP_LOGGING()` macro without any parameters. This macro effectively stops the logging module and releases any associated resources.

Here's an example that demonstrates the configuration, starting, and stopping of logging using the provided macros:

```cpp
#include "areg/trace/GETrace.h"

int main(void)
{
    TRACER_CONFIGURE_AND_START("./config/areg.init");   // Configure and initialize logging
    // Your code is here...
    TRACER_STOP_LOGGING();                              // Stop logging
    return 0;
}
```

In the above example, logging is configured and started using the `TRACER_CONFIGURE_AND_START()` macro, specifying the relative path to the configuration file. Subsequently, logging is stopped using the `TRACER_STOP_LOGGING()` macro to ensure proper cleanup. It's important to note that if the sources are compiled without logs enabled (`AREG_LOGS=OFF`), these two calls will not invoke any methods related to logging.

---

## Scopes and Logs

The AREG Framework offers a robust logging mechanism that allows selective logging based on log names and message priority. This is accomplished through the use of *logging scopes* (or *trace scopes*) within the framework's logging module. *Logging scopes* are objects initialized during application startup, providing the ability to activate or deactivate specific logs, filter them by names and message priority. Each message log belongs to a *scope*, which should have a unique name. It is recommended to follow a naming convention similar to header file directives commonly used by C/C++ developers.

To demonstrate the declaration and usage of *logging scopes* and *messages*, let's consider an example involving a class named `SomeClass`, located in the folder `source/some/classes/`, which includes two methods: `foo()` and `bar()`. In order to log messages within the source code, we need to declare scopes with unique names and utilize them as shown in the following example:

```cpp
#include "source/some/classes/SomeClass.hpp"
#include "areg/trace/GETrace.h"

// Declare scopes for methods foo() and bar()
DEF_TRACE_SCOPE(source_some_classes_SomeClass_foo);
DEF_TRACE_SCOPE(source_some_classes_SomeClass_bar);

void SomeClass::foo()
{
    // This trace scope must be declared before logging a message
    TRACE_SCOPE(source_some_classes_SomeClass_foo);
    
    // The following are three messages with three different priorities, where DEBUG has the lowest priority
    TRACE_DBG("This message has DEBUG priority and is the lowest priority");
    TRACE_INFO("This message has INFO priority");
    TRACE_WARN("This message has WARNING priority");
}

void SomeClass::bar()
{
    // This trace scope must be declared before logging a message
    TRACE_SCOPE(source_some_classes_SomeClass_bar);
    
    // The following are two messages with two different priorities, where FATAL has the highest priority
    TRACE_ERR("This message has ERROR priority");
    TRACE_FATAL("This message has FATAL priority");
}
```

> [!NOTE]
> The utilization of macros offers a notable advantage in scenarios where developers compile applications without logs. In such cases, the `foo()` and `bar()` functions will be effectively empty, and no scope object will be instantiated. This allows for efficient handling of situations where logging is not required, resulting in streamlined and optimized code execution.

Once the scopes are instantiated and activated, the logging module can log all messages, only messages up to a specific priority, or no messages at all. Additionally, scopes can be used to log messages that allow users to track and measure method call times and execution durations. The log messages excerpt from the [04_trace](./../../examples/04_trace) example is provided below:

```log
2023-05-22 13:05:28,080: [ 0x00000000000021EC  04_trace.main_HelloThread_onThreadRuns: Enter --> ]
2023-05-22 13:05:28,607: [ 0x00000000000021EC  WARNING >>> ] The thread [ HelloThread ] runs, going to output messages ...
2023-05-22 13:05:28,607: [ 0x00000000000021EC  INFO >>> ] !!!Hello World!!!
2023-05-22 13:05:28,607: [ 0x00000000000021EC  DEBUG >>> ] !!!Hello Tracing!!!
2023-05-22 13:05:29,134: [ 0x00000000000021EC  04_trace.main_HelloThread_onThreadRuns: Exit <-- ]
```

In this log, `main_HelloThread_onThreadRuns` represents the name of the scope, while the logs `Enter -->` and `Exit <--` along with the corresponding timestamps `13:05:28,080` and `13:05:29,134` indicate the method `onThreadRuns` of the `HelloThread` class being called and exited. All messages in the log have the ID `0x00000000000021EC`, which corresponds to the ID of the scope that logged the messages. This is particularly useful when dealing with multithreading applications, where message logs are mixed in the file. By filtering messages based on their IDs, developers can easily track and analyze the logs.

> [!IMPORTANT]
> Currently, the AREG SDK log viewer is still in development. Once completed, it will be included as part of the SDK.<br/>

> [!NOTE] 
> Almost all the [examples](./../../examples) utilize logging. For further guidance on using logs, refer to these examples.

---

## Logging Configuration

The AREG Framework provides a feature to configure logging settings using a log configuration file during the initialization of applications. This logging file allows developers to specify the names of log files, the active scopes, and their message priorities. Developers also have the option to customize the logging format within the file.

By default, the AREG Framework utilizes the [areg.init](./../../framework/areg/resources/areg.init) file, which can be found in the `./config` folder after the compilation process. This file serves as the configuration file, containing vital data and offering an explanation of the format used. 

Refer to the document [Logging Configuration](./logging-config.md) to know more about logging configuration, which provides flexibility to initialize and cofigure the scopes on start, as well as to connect to the [Log Collector](./logger.md) service to log messages.
