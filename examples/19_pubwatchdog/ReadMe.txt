========================================================================
    CONSOLE APPLICATION : 19_pubwatchdog Project Overview
========================================================================

This project demonstrates use of watchdog in punlic service.
The watchdog is a guard to control threads. When thread is defined in the
model, developer may set watchdog timeout in milliseconds. The watchdog with
timeout 0ms is disabling the watchdog for the specified thread.
-   If during the time the thread does not make a full cycle, the watchdog 
    manager assumes that the thread is stuck for any reason and kills the thread.
-   The system notifies all components that client or servers registered in 
    the thread are lost connection.
-   The system restarts the thread and instantiates the components defined in
    the model of the thread.
-   When components are instantiated, the system notifies clients that the 
    service is available again.

In this example, to trigger the watchdog the system puts the thread in a sleep,
so that when watchdog timeout is expired the system kills and restarts the thread
with components.

The demo creates a public service and the remote client components

The project consist of 3 sub-projects:
    1. generated  (19_generated)    -- generated code.
    2. pubservice (19_pubservice)   -- public service provider.
    3. pubclient  (19_pubclient)    -- remote client provider.

////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 18_generated

Generated code of the HelloWatchdog.siml service interface document located
in './res' sub-folder. The document is created by an interface design tool
and compiled as a static library.

////////////////////////////////////////////////////////////////////////

        2. Project 'pubservice' / 19_pubservice

The application instantiates the public service to be triggered by the 
request of the client. The service provider thread of the application has
registered watchdog timeout. By the request of the client, the thread goes
in sleep, so that the watchdog kills and restarts the thread. All connected
clients are notifies that the service is disconnected.

////////////////////////////////////////////////////////////////////////

        3. Project 'pubclient' / 19_pubclient

The application instantiates the remote client to connect to the public service
running in the thread with watchdog. The client send a message to the public
service to put thread to the sleep. When service thread is killed and the servicing
component is deleted, the client receives notification that the service is disconnected.
When public service is available, the client is notified that service is available.
