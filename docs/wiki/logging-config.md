# AREG SDK Logging Configuration Guide

The AREG SDK provides developers with a highly configurable logging system through the **AREG log configuration file** (`areg.init`). This file allows customization of log files, target destinations, scope settings, and message priority, enhancing debugging and runtime analysis.

---

## Key Configuration Options

Located by default in the `./config` folder, the `areg.init` file enables structured logging configurations with entries defined as `section::(module|*)::property[::position] = value`. Each entry combines mandatory and optional elements:

- **section**: Defines the area of logging (e.g., `log`, `logger`).
- **module**: Targets specific applications or globally (`*`).
- **property & position**: Specifies exact settings like file paths, target destinations, and logging levels.

For a full syntax guide, refer to the [AREG SDK Persistence Syntax documentation](./persistence-syntax.md).

---

## Log Mechanisms and Destinations

The configuration enables versatile logging outputs:

1. **Remote**: Routes logs to a remote collector.
2. **File**: Saves logs locally, defined by `log::*::file::location`.
3. **Debug Output**: Useful for real-time debugging within IDEs.
4. **Database**: Stores logs in a structured database for analysis.

Example settings:
```plaintext
log::*::target = remote | file | debug | db
log::*::enable::file = true
log::*::file::location = ./logs/%appname%_%time%.log
```

The `log::*::enable` setting controls log activation, while specific settings (e.g., `log::*::remote::queue` queuing) optimize for startup or delayed connections.
The logging file name may contain the file naming mask like `%appname%_%time%.log`, which means that the file name will conaint the name of the application and the timestamp.

---

## Configuring Log Message Layout

Customize the log file format using the following in `areg.init`:
```plaintext
log::*::layout::enter       = %d: [ %t  %x.%z: Enter -->]%n    # Enter scope layout
log::*::layout::message     = %d: [ %t  %p >>> ] %m%n          # Log message layout
log::*::layout::exit        = %d: [ %t  %x.%z: Exit <-- ]%n    # Exit scope layout
```
Available format specifiers:
| Format |            Description                                               |
|--------|----------------------------------------------------------------------|
|  `%a`  | Output the ID of a logging object set by the logger.                 |
|  `%c`  | Output tick-count value since process start.                         |
|  `%d`  | Output day and time data.                                            |
|  `%e`  | Output module (process) ID.                                          |
|  `%m`  | Output logging message. The `%m` and `%z` are mutually exclusive.    |
|  `%n`  | Output end-of-line character.                                        |
|  `%p`  | Output message priority.                                             |
|  `%s`  | Output message scope ID.                                             |
|  `%t`  | Output thread ID.                                                    |
|  `%x`  | Output module (process) name.                                        |
|  `%y`  | Output thread name, if available.                                    |
|  `%z`  | Output scope name. The `%z` and `%m` are mutual exclusive.           |

Adjusting formats enables clear, consistent log messages across applications.

---

## Remote Log Collector Configuration

To use a remote log collector, specify protocol, IP, and port:
```plaintext
logger::*::connect = tcpip
logger::*::address::tcpip = 127.0.0.1
logger::*::port::tcpip = 8282
```
This section ensures logs can be captured over a network by a central log collector service. Additionally, the log scopes and the message priorities can be configurable in real-time using the [Log Observer](./logobserver.md) console application.

---

## Scopes and Log Priorities

The AREG Framework offers a flexible logging system that enables selective logging by scope and message priority, using *logging scopes* to filter logs efficiently. Scopes are configured in the `log` section with the `scope` property (`log::*::scope::*`), allowing initial setup at startup.

Developers can enable, disable, and set priorities for individual or grouped scopes. For example:
```plaintext
log::app::scope::module_foo = DEBUG | SCOPE
log::app::scope::module_bar = NOTSET
```

Scopes may also be grouped with wildcards, such as `log::app::scope::module_*`, to apply priorities like `DEBUG | SCOPE` to all matching scopes. 

### Log Priority Levels

Configurable priority levels include:
- **NOTSET**: No logging
- **SCOPE**: Only logs scope entries/exits
- **DEBUG**: Logs debug and higher
- **INFO**: Logs informational messages and higher
- **WARN**: Logs warnings and higher
- **ERROR**: Logs errors and higher
- **FATAL**: Only logs fatal errors

For instance, `WARN | SCOPE` will log *Warnings*, *Errors*, *Fatal Errors* and scope entries/exits, excluding *Debug* and *Information*.

The AREG Framework supports runtime log adjustments through its `logobserver` tool. This console application enables real-time scope and priority manipulation for greater logging control. For further details, refer to the [Log Observer documentation](./logobserver.md).

---

## Conclusion

This guide outlines the AREG SDK logging system's configurability, enabling efficient monitoring and debugging across applications. Explore additional settings for optimal performance, and refer to the AREG SDK documentation for advanced configuration details.
