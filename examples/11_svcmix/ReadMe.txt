========================================================================
    CONSOLE APPLICATION : 11_svcmix Project Overview
========================================================================

This project demonstrates execution of multiple instances of the same service implementation.
The project 11_svcmix is an extension of project 10_service, where same instance of a servicing component is instantiated multiple times.
Every instance of the same implemented service must have unique role name and they must be instantiated in different threads.
One of components is marked as 'main' and the application runs as long, until the main component does not trigger 'quit' event to stop application and release resources.
