========================================================================
    CONSOLE APPLICATION : 18_locwatchdog Project Overview
========================================================================

This project demonstrates use of watchdog in local service.
The watchdog is a guard to control threads. When thread is defined in the
model, developer may set watchdog timeout in milliseconds. The watchdog with
timeout 0ms is disabling the watchdog for the specified thread.
-   If during the time the thread does not make a full cycle, the watchdog 
    manager assumes that the thread is stuck for any reason and kills the thread.
-   The system notifies all components that client or servers registered in 
    the thread are lost.
-   The system restarts the thread and instantiates the components defined in
    the model of the thread.
-   When components are instantiated, the system notifies clients that the 
    service is available again.

In this example, to trigger the watchdog the system puts the thread in a sleep,
so that when watchdog timeout is expired the system kills and restarts the thread
with components.

The demo creates a local service and the local client components in the
same process. The local services are not visible outside of the process
and used in multithreading communication.

The project consist of 2 sub-projects:
    1. generated  (18_generated)    -- generated code.
    2. locservice (18_locservice)   -- service provider and client.


////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 18_generated

Generated code of the HelloWatchdog.siml service interface document located
in './res' sub-folder. The document is created by an interface design tool
and compiled as a static library.

////////////////////////////////////////////////////////////////////////

        2. Project 'locservice' / 18_locservice

The application instantiates a simple local service and the client running in
differen thread, so that when the thread containing service is killed, the client
component receives 'service disconnected' notification. When thread is restarted
and the service component is instantiated, the system notifies client that the
service is available.
