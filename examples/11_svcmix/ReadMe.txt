========================================================================
    CONSOLE APPLICATION : 11_svcmix Project Overview
========================================================================

The project 11_svcmix is an extension of project 10_service.
This project demonstrates how can be same service interface instantiated in the same process multiple times.
To have fully operable instances of the same servicing component object in the same process, they should run in different threads and have different names.
There is no restriction for client part. The client can be instantiated in any thread multiple times.
This application defines model with 3 component threads. Each thread instantiates same service interface with different names.
Some components contain clients and the client objects refer to different services of the same service interfaces (SecondaryComponent).
One of components is marked as 'main' and the application runs as long, until the main component does not trigger 'quit' event to stop application and release resources.
 