# AREG SDK Log Observer Application

The **AREG SDK Log Observer** is a console tool designed for real-time log monitoring, data management, and dynamic control over log entries received from multiple applications distributed in the network. Acting as a client to the [Log Collector](./logger.md), the Log Observer allows users to save logs to text files and/or SQLite databases while dynamically adjusting scope activation and filtering log priorities during runtime.

## Table of Contents
1. [General Information](#general-information)
2. [Log Observer Setup](#log-observer-setup)
3. [Log Observer Configuration](#log-observer-configuration)
4. [Command Line Options](#command-line-options)
5. [Connection Initialization](#connection-initialization)

---

## General Information

The AREG SDK Log Observer functions as a client application for the [Log Collector](./logger.md) service, seamlessly integrating within AREG SDK's logging ecosystem. It enables centralized log storage in plain text and/or SQLite databases, providing extensive analysis capabilities for logs gathered from multiple applications distributed in the network. Through bi-directional communication with logging sources, the Log Observer lets users adjust scope activation and message priorities during runtime to optimize network traffic, ensuring only relevant log data is captured for analysis.

---

## Log Observer Setup

The Log Observer is part of the [logobserver](./../../framework/logobserver/) module in AREG SDK, using the API of the `areglogger` utility library. It compiles as a standalone executable, operable on any networked machine with a General Purpose Operating System (GPOS) like Windows or Linux.

To start receiving log data, launch the console application on a networked GPOS machine and in opened console type `-r` to start receiving logs.

---

## Log Observer Configuration

As any other application based on AREG Framework, the Log Observer configuration, managed through the configuration file like AREG SDK standard `areg.init` file, uses key-value pairs formatted as `section::(module|*)::property[::(position|*)]`. Example configurations enable applications to connect to the Log Collector:

```plaintext
logger::*::service          = logger        # Log Collector process name
logger::*::connect          = tcpip         # Protocol for communication
logger::*::enable::tcpip    = true          # Enable TCP/IP
logger::*::address::tcpip   = 172.23.96.1   # Network IP address
logger::*::port::tcpip      = 8282          # Port number for connection
```
Setting descriptions:
|  Property Key Setting:        |   Description:                                        |
|-------------------------------|-------------------------------------------------------|
| `logger::*::service`          | Specifies the log collector process name.             |
| `logger::*::connect`          | Lists supported protocols (TCP/IP in this example).   |
| `logger::*::enable::tcpip`    | Activates or deactivates the protocol.                |
| `logger::*::address::tcpip`   | Specifies the Log Collector’s network-accessible IP.  |
| `logger::*::port::tcpip`      | Assigns the port number.                              |

**Additional Log Observer-Specific Settings:**

The following configuration settings enable file and database logging:

```plaintext
# ---------------------------------------------------------------------------
# Message layout for log observer
# ---------------------------------------------------------------------------
log::logobserver::layout::enter     = %d: [ %a.%x.%t.%z: Enter -->]%n
log::logobserver::layout::message   = %d: [ %a.%x.%t.%p >>> ] %m%n
log::logobserver::layout::exit      = %d: [ %a.%x.%t.%z: Exit <-- ]%n

# ---------------------------------------------------------------------------
# Log observer specific database logging settings
# ---------------------------------------------------------------------------
log::logobserver::enable::file      = true
log::logobserver::enable::db        = true
log::logobserver::db::name          = sqlite3
log::logobserver::db::location      = ./logs/log_%time%.sqlite3
```

In this setup, received log messages are saved in both a text file and a SQLite database, with logging layouts tailored for `logobserver`.

> [!TIP]
> To disable file logging, set `log::logobserver::enable::file = false`.

---

## Command Line Options

Below are the key command-line options for managing the Log Observer. Options marked **Console** can be used during active `logobserver` operation, while others initiate program actions during startup.

| Command:              | Platform: | Description:                  | Usage Example:                   |
|-----------------------|-----------|-------------------------------|----------------------------------|
| `-e, --query`         | Console   | Display list of log scopes    | `-e someapp`                     |
| `-f, --config`        | Console   | Save current configuration    | `-f`                             |
| `-h, --help`          | All       | Display help message          | `logobserver --help`             |
| `-l, --load`          | All       | Load configuration file       | `logobserver -l="config.init"`   |
| `-n, --instances`     | Console   | Show connected instances      | `-n`                             |
| `-o, --scope`         | Console   | Set scope priority            | `-o someapp::scope_*=DEBUG`      |
| `-p, --pause`         | Console   | Pause log observer            | `-p`                             |
| `-q, --quit`          | Console   | Quit console application      | `-q`                             |
| `-r, --restart`       | Console   | Restart connections           | `-r`                             |
| `-x, --stop`          | Console   | Stop log observer             | `-x`                             |

Commands may vary with version updates.

> [!NOTE]
> On start, the Log Observer does not start immediately receiving log data. Type command `-r` or `-restart` in the console of Log Obsercer to start receiving log data. And type `-x` or `--stop` to stop receiving.

---

## Connection Initialization

By default, `logobserver` loads settings from `areg.init`. To specify an alternative configuration file, use:

```bash
logobserver --load="./my/config/file.init"
```

> [!NOTE]
> In `areg.init`, default settings may disable `logger` and `logobserver` logging with:
> ```plaintext
> log::logger::scope::* = NOTSET
> log::logobserver::scope::* = NOTSET
> ```
