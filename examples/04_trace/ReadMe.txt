========================================================================
    CONSOLE APPLICATION : 04_trace Project Overview
========================================================================

This project demonstrates use of logging (tracing).
The logging requires compilation with ENABLE_TRACES preprocessor directive.
This is a mandatory option. Otherwise, the logging macro are not included in build.
Beside this, it requires enabling in configuration file (by default "./config/log.init" file).
If there is no configuration file, the logging can be forced to be enabled and the default configuration is applied.
This demo shows how to force to enable logging even if the log configuration file is missed.
By default configuration, all logs are enabled for the current process.

It is possible to apply log filters, enable / disable log of scopes, set logging output file names, etc.
All these are set in configuration "./config/log.init" file. Make changes in log.init file to see the changes.
