# 04_trace Project Overview

The **[04_trace](./04_trace)** project demonstrates how to implement logging (tracing) in applications using the AREG SDK. To enable logging, the source code must be compiled with the `AREG_LOGS=ON` preprocessor definition. Additionally, logging must be activated through the [areg.init](./../framework/areg/resources/areg.init) configuration file, typically located in the `./config` folder relative to the executable.

If the configuration file is missing, logging can still be forcefully enabled with default settings. In this example, logging is pre-configured to be active and uses default settings, showing how to log application behavior effectively.
