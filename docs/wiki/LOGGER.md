# Log Collector Service



 Configure [_areg.init_](https://github.com/aregtech/areg-sdk/blob/master/framework/areg/resources/areg.init) file copied into the binary directory after the build to start receiving log messages from applications. Following properties are relevant to `logger` and connected applications:




 (To configure logging for applications based on the AREG framework, you can compile them with or without logging. If you choose to compile with logging, you'll need to configure the [_areg.init_](https://github.com/aregtech/areg-sdk/blob/master/framework/areg/resources/areg.init) file to set logging scopes, priorities, and file names.

Here's an example configuration for the log file:
```
log::*::target              = file                          # Log targets: remote, file, debug output, database
log::*::enable::file        = true                          # File logging enable / disable flag
log::*::file::location      = ./logs/%appname%_%time%.log   # Log file location and masks

log::mcrouter::scope::*                 = NOTSET ;                         # disables logs for mcrouter.
log::my_app::scope::*                   = DEBUG | SCOPE ; # enables all logs of my_app
log::my_app::scope::ignore_this_scope   = NOTSET ;        # disables logs for a certain scope in my_app
log::my_app::scope::ignore_this_group_* = NOTSET ;        # disables logs for a certain scope group in my_app
```

Please refer to the [AREG Logging System](https://github.com/aregtech/areg-sdk/wiki/05.-AREG-Logging-System) Wiki page for detailed instructions on compiling applications with enabled or disabled logging, starting and stopping logs during runtime, creating and using logging scopes, and configuring the `areg.init` file.

> 💡 By default, the `areg.init` files are located in the `config` subfolder of the binaries.<br/>
> 💡 To enable logging for all applications, use `log::*::scope::*  = DEBUG | SCOPE ;`.<br/>
> 💡 Currently, logging is only possible in the file.

