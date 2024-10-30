# AreG SDK Logging Configuration

The AREG Framework provides a feature to configure logging settings using a log configuration file during the initialization of applications. This logging file allows developers to specify the names of log files, the active scopes, and their message priorities. Developers also have the option to customize the logging format within the log message file.

## Configuration File

By default, the AREG Framework utilizes the [areg.init](./../../framework/areg/resources/areg.init) file, which can be found in the `./config` folder after the compilation process. This file serves as the configuration file, containing vital data and offering an explanation of the format used. 

In this section, we will provide key information regarding the log configuration. The configuration file follows a straightforward syntax, consisting of *key* and *value* pairs. The *key* comprises a mandatory *section* and *property*, along with optional *module* and *position* elements. The syntax takes the form of `section::(module|*)::property[::position] = value`.

> [!TIP]
> For a more comprehensive understanding of the configuration syntax, please refer to the [Persistence Syntax](./persistence-syntax.md) section in the AREG SDK Wiki documentation. It provides detailed information and further guidance on configuring the initialization file.

## Logging mechanism

The logging configuration entries are listed within section `log`. Every application can make decision whether it logs messages in the text file, sends to remote log collector or uses both. The available logging mechanisms are described in the configuration file like this:
```text
log::*::version             = 2.0.0                         # Logging version
log::*::target              = remote | file | debug | db    # Log targets: remote, file, debug output, database
log::*::enable              = true                          # Global logging enable / disable flag
log::*::enable::remote      = true                          # Remote logging enable / disable flag
log::*::enable::file        = true                          # File logging enable / disable flag
log::*::enable::debug       = false                         # Debug output console logging enable / disable flag
log::*::enable::db          = false                         # Database logging enable / disable flag
log::*::file::location      = ./logs/%appname%_%time%.log   # Log file location and masks
log::*::file::append        = false                         # Append logs at the end of file
log::*::remote::queue       = 100                           # Queue stack size in remote logging, 0 means no queuing
log::*::remote::service     = logger                        # The service name of the remote logging
```

- The key `log::*::target` describes available logging mechanism:
  - target `remote`: the application can send log messages to the remote log collector;
  - target `file`: the application can log messages in the log file;
  - target `debug`: the logs are written in Debug output window (relevant to debugging with MS Visual Studio);
  - target `db`: the logs are written directly to the database.
- The key `log::*::enable` defines whether the logging is enabled or disabled and the following keys `log::*::enable:xxx` describe which one of the listed targets are enabled or disabled;
- The key `log::*::file::location` defines the location of logging files. It is used only if `log::*::enable::file` is enabled;
- The key `log::*::remote::queue` defines the length of the stack before the logging module is connected to the remote log collector. It takes a little time until the application activates the logging module and it is connected to the remote log collector service. If the size is set, the logging module will queue the messages and as soon as the logging module is active and connected, it sends the messages to the remote log collector. Otherwise, if the size is `0`, no message is queued. This is useful when the developer needs to analyze the logs during application startup.

## Logging in file

The applications may log the messages to the file. First of all in the configuration file, the key `log::*::enable::file` should be set `true`. Then the location of logging file should be indicated in the key `log::*::file::location`. The file name of the location can be relative or absolute, it may also contain the file name mas. For example, this entry `log::*::file::location = ./logs/%appname%_%time%.log` means that the messages are written in the file located in the `logs` subdirectory of executable and the file name with mask `%appname%_%time%.log` means that the files will have the name of the executable and the timestamp.

Additionally, it is possible to change the format of messages in the file. In `areg.init` file the message layout has following format:
```text
log::*::layout::enter       = %d: [ %t  %x.%z: Enter -->]%n    # Enter scope layout
log::*::layout::message     = %d: [ %t  %p >>> ] %m%n          # Log message layout
log::*::layout::exit        = %d: [ %t  %x.%z: Exit <-- ]%n    # Exit scope layout
```
Where 
- `log::*::layout::enter` defines the layout of message scope when it is created;
- `log::*::layout::message` defines the layout of the log message;
- `log::*::layout::exit` defines the layout of message scope when it is deleted.

The following is the list of layout format:
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

Use this formatting to change the message layouts in the loggin file. This change will not affect the remote log collector. Additionally, instead of `*` which is relevant to all applications, you can specify changes only for your module, for example `log::someapp::layout::message = %s: %P >>> %m%n`. In this case the chagne will not affect any applications except `someapp`.

# Remote Log Collector

To log message to the remote log collector, the applications need to know the communication protocol, IP-address and the port number of the remote log collector. This information contains in the `areg.init` file within section `logger`:
```text
logger::*::service          = logger        # The name of the router service (process name)
logger::*::connect          = tcpip			# The list of supported communication protocols
logger::*::enable::tcpip    = true			# Communication protocol enable / disable flag
logger::*::address::tcpip   = 172.23.96.1   # Protocol specific connection IP-address, default IP is 127.0.0.1
logger::*::port::tcpip      = 8282			# Protocol specific connection port number, default port is 8282
```

This section describes:
- `logger::*::service` .......: The name of the remote log collector service
- `logger::*::connect` .......: The list of supported communication protocols, in this case it is TCP/IP.
- `logger::*::enable::tcpip` .: The flag, indicating whether for the application the TCP/IP protocol is enabled or not.
- `logger::*::address::tcpip` : The IP-address of the remote log collector service.
- `logger::*::port::tcpip` ...: The port number to connect to the remote log collector service.

Change the `areg.init` file and specify the IP-address and the port number of your remote collector service to send logs over the network. No more changes are required to send logs.

The [Log Collector](./logger.md) is described in the AREG SDK Wiki documentation.

# Log Scopes and Log Priorities

The AREG Framework offers a robust logging mechanism that allows selective logging based on log names and message priority. This is accomplished through the use of *logging scopes* (or *trace scopes*) within the framework's logging module. The logging configuration of AREG SDK enables to make initial configuration of the logging scopes to activate the scopes and filter the messages at startup. The logging scopes are defined with the property `scope` of section `log`: `log::*::scope::*`

The developers can enable or disable, and filter the message priority of a single scope or of the group of scopes. For example, if the configuration file contains entries like this, on startup, the `someapp` application activates `scope::some_class_foo` log scope and enables logging of all messages and deactivates the scope `some_class_bar`:
```text
log::someapp::scope::some_class_foo = DEBUG | SCOPE
log::someapp::scope::some_class_bar = NOTSET
```

The developers can activate, deactivate or set the log message priority of the scope group. The following syntax means to activate all scopes which name begins with `some_class__`:
```text
log::someapp::scope::some_class_* = DEBUG | SCOPE
```

or if deactivating all logs of the `someapp` application `log::someapp::scope::* = NOTSET`

The log message priorities in the configuration file are set as values of the property `scope`. The value can be a list or priorities. These are the log message priorities that are possible to set in the configuration file:

- **NOTSET**: No priority, no message is logged;
- **SCOPE**: Log only scope name with *created* and *deleted* timestamps;
- **DEBUG**: Log messages with *Debug* and higher priority;
- **INFO**: Log messages with *Information* and higher priority;
- **WARN**: Log messages with *Warning* priority and higher;
- **ERROR**: Log messages with *Error* priority and higher;
- **FATAL**: Log messages only with *Fatal Error* priority.

The developers can specify either one or multiple log priorities. For example, the priority `WARN | SCOPE` means to output all messages with *Warning* and higher priority and the messages generated by log scopes (enter / exit). The messages with *Debug* and *Information* priorities should be ignored.

The activation of scopes and the priority of messages in AREG Framework is possible to set during runtime. AREG SDK provides `logobserver` console application as a simple solution to collect logs and manipulate the priorities during runtime. For more details, see the description of the [Log Observer](./logobserver.md) in this Wiki documentation.
