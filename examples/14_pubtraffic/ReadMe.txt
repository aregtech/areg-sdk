========================================================================
    CONSOLE APPLICATION : 14_pubtraffic Project Overview
========================================================================

This project demonstrates the run-time dynamic model definition, load and
unload of dynamic model, use of custom events in multithreading communication.

The project consists of 3 sub-projects:
    1. generated  (14_generated ) -- generated codes and common objects.
    2. pubservice (14_pubservice) -- public and local services, custom events.
    3. pubclient  (14_pubclient ) -- dynamically creates a model during runtime.

All communications pass through mcrouter (multicasting router). Since in
AREG SDK the service discovery is automated and it provides fault tolerant
system, the sequence of processes to start does not play any role.

It automatically forms mist-network, where components receive connected or
disconnected notifications to start or stop calling remote methods.

////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 14_generated

Generated code of the HelloWorld.siml service interface document located
in './res' sub-folder. The document is created by an interface design tool
and compiled as a static library. In addition, it contains implementation
of objects that are used in other projects.

////////////////////////////////////////////////////////////////////////

        2. Project 'pubservice' / 14_pubservice

This project creates an instance of Public service. It also defines and 
processes custom events.

When the application starts, use one of commands:
- "start" to start the jobs in service.
- "stop" to stop the jobs in service.
- "quit" or "q", to exit applications. This also closes all clients.

Type "start" to start changing light states.


////////////////////////////////////////////////////////////////////////

        3. Project 'pubclient' / 14_pubclient

This project contains an instance service client, which model is defined
dynamically during run-time. This approach guaranties to start and run
several instance of the same applications, where each instance of service
client (or provider) have generated and unique names.

When the application starts, use one of commands:
- Type "sn" to subscribe to one set of data updates.
- Type "ew" to subscribe to another set of data updates.
