========================================================================
    CONSOLE APPLICATION : 11_locmesh Project Overview
========================================================================

This project demonstrates execution of multiple instances of the same 
service and it is an extension of the project 10_locsvc. Every service 
instance must have a unique name and be instantiated in different threads.

The project consist of 2 sub-projects:
    1. generated  (11_generated)    -- generated code.
    2. locsvcmesh (11_locmesh)      -- The mixture of services and clients.

////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 11_generated

Generated code of the HelloWorld.siml service interface document located
in './res' sub-folder. The document is created by an interface design tool
and compiled as a static library.

////////////////////////////////////////////////////////////////////////

        2. Project 'locsvcmesh' / 11_locmesh

The application that instantiates multiple instances of the same service
and clients in different threads. The definition of servicing components,
service provider or service client are done in modeling that are loaded
on application start and unload when exit. 

One of the components is marked as 'main' and the application runs as long,
until the main component does not trigger 'quit' event to stop application 
and release the resources. Similar to previous project, it has request,
response, broadcast and subscription services features dynamically subscribe
and unsubscribe to data update messages during run-time.
