========================================================================
    CONSOLE APPLICATION : 15_pubworker Project Overview
========================================================================

This project demonstrates use of worker thread definition (description),
definition and processing of custom events in multithreading communication.

The project consists of 3 sub-projects:
    1. generated  (15_generated)  -- generated codes and common objects.
    2. pubservice (15_pubservice) -- Public service and the worker thread.
    3. pubclient  (15_pubclient)  -- Public service client and the worker thread.

All communications pass through mcrouter (multicasting router). Since in
AREG SDK the service discovery is automated and it provides fault tolerant
system, the sequence of processes to start does not play any role.

It automatically forms mist-network, where components receive connected or
disconnected notifications to start or stop calling remote methods.

////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 15_generated

Generated code of the HelloWorld.siml service interface document located
in './res' sub-folder. The document is created by an interface design tool
and compiled as a static library. In addition, it contains implementation
of objects that are used in other projects.

////////////////////////////////////////////////////////////////////////

        3. Project 'pubservice' / 15_pubservice

This project contains instance of public service and the worker thread
to receive input data from the console and update the attribute of the
service. The service and the worker threads are described in the static
model to load on startup and unload on exit.

////////////////////////////////////////////////////////////////////////

        2. Project 'pubclient' / 15_pubclient

This project contains service client and the worker thread described in
the model. The worker thread imitates the output device.
