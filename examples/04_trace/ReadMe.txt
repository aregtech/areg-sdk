========================================================================
    CONSOLE APPLICATION : 04_trace Project Overview
========================================================================

This project demonstrates how to configure the loging and how to enable logging (including, force to enable).
When starts, it uses default "./config/log.init" path to initialize logging. If the file does not exist,
it forces to start logging and use default logging settings.
By default, the logging is enabled for all scopes of the current process, and the log files contain the timestampt.
The application starts a separate thread and makes log oututs.