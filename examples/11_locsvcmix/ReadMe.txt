========================================================================
    CONSOLE APPLICATION : 11_locsvcmix Project Overview
========================================================================

This project demonstrates execution of multiple instances of the same service
and it is an extention of the project 10_locsvchello. Every instance of service 
must have unique role name and since they are instances of the same component, 
they must be instantiated in different threads.

////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 11_generated

Generated code of the service interface document located in './res' sub-folder.
The document is created by interface designing tool.
The project is compiled as a static library.

////////////////////////////////////////////////////////////////////////

        2. Project 'svcmix' / 11_svcmix

Contains the implementation of the services, which are described in the model.
One of the components is marked as 'main' and the application runs as long, 
until the main component does not trigger 'quit' event to stop application 
and release the resources. Similar to previous project, it has request, response,
broadcast and subscription features.
