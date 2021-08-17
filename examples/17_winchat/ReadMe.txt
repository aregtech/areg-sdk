========================================================================
    WINDOWS GUI APPLICATION : 17_winchat Project Overview
========================================================================

This project is  designed to run under Windows. It demonstrates a fault tolerant and distributed computing system. 
It as well demonstrates how to dynamically create a model and use the dynamic subscription service during run-time.

The project consists of 3 sub-projects:
    1. generated(17_generated   ) -- is a static library of generated codes and common objects used by other processes.
    2. register (17_register    ) -- is a Windows GUI application, containing instances of public services.
    3. chatter  (17_chatter     ) -- is a Windows GUI application, contains instances of public services and dynamically creates models.

All communications pass through mcrouter (multicasting router). Since in AREG SDK the service discovery is 
automated, it does not matter the order of processes to start. The system will start working when logical client 
and logical server get connection via mcrouter. This approach forms a mist network and extends the fog network 
to the extreme edge.

////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 17_generated

Generated code of the service interface documents located in './res' sub-folder.
The documents are created by the interface designing tool. In addition, it contains implementation of components 
that are used in other projects. The project is compiled as a static library.


////////////////////////////////////////////////////////////////////////

        2. Project 'register' / 17_register

This project connects to the multicast router and instantiates the instance of public service used by other 
applications. When the application starts, the user needs to enter the IP address of the multicast router 
(mcrouter). The user can test dynamic subscription service during run-time.


////////////////////////////////////////////////////////////////////////

        3. Project 'chatter' / 17_chatter

This project contains an instance of public services. It dynamically creates models that are loaded during run-time. 
It as well creates dynamic services and the clients, demonstrates dynamic subscription service.  The user can 
instantiate several instances of application to run on the same machine or on different machines in the same 
network. The users can use an application for messaging.
