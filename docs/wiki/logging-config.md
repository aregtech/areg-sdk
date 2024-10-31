# AREG SDK Logging Configuration Guide

The AREG SDK offers a highly configurable logging system via the **AREG log configuration file** (`areg.init`), allowing customization of log files, destinations, scope settings, and message priority to support enhanced debugging and runtime analysis.

---

## Key Configuration Options

The `areg.init` file, located in the `./config` folder by default, structures logging configurations using the format `section::(module|*)::property[::position] = value`. Each entry specifies:

- **section**: The log area (e.g., `log`, `logger`).
- **module**: Targets specific applications or globally (`*`).
- **property & position**: Defines settings like file paths, destinations, and logging levels.

For more syntax details, see the [AREG SDK Persistence Syntax documentation](./persistence-syntax.md).

---

## Log Mechanisms and Destinations

This configuration supports various logging outputs. Currently supported:

|  Target Name:         |  Value:   |  Description:                                                 |
|-----------------------|-----------|---------------------------------------------------------------|
| **Remote Collector**  | *remote*  | Remote centralized log collector service.                     |
| **Log File**          | *file*    | Stores logs locally, specified by `log::*::file::location`.   |
| **Debug Output**      | *debug*   | Provides real-time debugging within IDEs (for MSVS).          |
| **Database**          | *db*      | Logs data in database for structured analysis.                |

Example setup:
```plaintext
log::*::target = remote | file | debug | db
log::*::enable::file = true
log::*::file::location = ./logs/%appname%_%time%.log
```

The `log::*::enable` setting manages log activation, while parameters (e.g., `log::*::remote::queue`) optimize start-up or delayed connections. The file naming mask `%appname%_%time%.log` dynamically includes the application name and timestamp.

---

## Configuring Log Message Layout

Define log file formats in `areg.init` with the following:
```plaintext
log::*::layout::enter       = %d: [ %t  %x.%z: Enter -->]%n    # Enter scope layout
log::*::layout::message     = %d: [ %t  %p >>> ] %m%n          # Log message layout
log::*::layout::exit        = %d: [ %t  %x.%z: Exit <-- ]%n    # Exit scope layout
```

| Format:   | Description:                                   |
|-----------|------------------------------------------------|
| `%a`      | Logging object ID.                             |
| `%c`      | Tick-count since process start.                |
| `%d`      | Date and time.                                 |
| `%e`      | Module (process) ID.                           |
| `%m`      | Logging message (exclusive with `%z`).         |
| `%n`      | End-of-line character.                         |
| `%p`      | Message priority.                              |
| `%s`      | Message scope ID.                              |
| `%t`      | Thread ID.                                     |
| `%x`      | Module (process) name.                         |
| `%y`      | Thread name, if available.                     |
| `%z`      | Scope name (exclusive with `%m`).              |

Adjusting formats ensures clear, uniform log messages across applications.

---

## Remote Log Collector Configuration

For remote logging, configure protocol, IP, and port:
```plaintext
logger::*::connect = tcpip
logger::*::address::tcpip = 127.0.0.1
logger::*::port::tcpip = 8282
```

This setup enables central log collection over a network. Scopes and priorities are adjustable in real-time through the [Log Observer](./logobserver.md) console application.

---

## Scopes and Log Priorities

AREG’s logging system supports selective logging by scope and priority, enabling efficient log filtering. Configure scopes in the `log` section using the `scope` property (`log::*::scope::*`) for initial setup.

Scopes can be enabled, disabled, or grouped by priority. Example configurations:
```plaintext
log::app::scope::module_foo = DEBUG | SCOPE
log::app::scope::module_bar = NOTSET
```

You can apply priorities like `DEBUG | SCOPE` to scope groups using wildcards, such as `log::app::scope::module_*`.

### Log Priority Levels

Supported priorities include:
| Priority:  |  Explanation:                        |
|------------|--------------------------------------|
| **NOTSET** | No logging.                          |
| **SCOPE**  | Logs only scope *enter* and *exit*.  |
| **DEBUG**  | Logs debug and higher.               |
| **INFO**   | Logs informational and higher.       |
| **WARN**   | Logs warnings and higher.            |
| **ERROR**  | Logs errors and higher.              |
| **FATAL**  | Logs fatal errors only.              |

For instance, `WARN | SCOPE` logs *Warnings*, *Errors*, *Fatal Errors*, and scope *Enter/Exit*, excluding *Debug* and *Information* levels. The priority `DEBUG | SCOPE` will log all messages.

The AREG Framework allows runtime adjustments using the `logobserver` tool, providing real-time control over scopes and priorities. For details, see the [Log Observer documentation](./logobserver.md).

---

## Conclusion

This guide highlights the flexible configuration of the AREG SDK logging system, offering efficient application monitoring and debugging. Explore additional settings for optimal performance, and refer to the AREG SDK documentation for advanced configuration insights.
