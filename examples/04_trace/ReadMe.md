# 04_trace Project Overview

The [04_trace](https://github.com/aregtech/areg-sdk/tree/master/examples/04_trace) project showcases the usage of logging (tracing) in applications. To enable logging, the source code needs to be compiled with the `AREG_LOGS` preprocessor define. Additionally, logging should be enabled in the [log.init](https://github.com/aregtech/areg-sdk/blob/master/framework/areg/resources/log.init) configuration file. By default, the `log.init` file should be located in the `./config` sub-directory relative to the executable file. If the configuration file is not present, logging can be forcefully enabled with default settings.

In the demonstration, logging is forced to be enabled, and it utilizes the default logging settings.
