========================================================================
    WINDOWS GUI APPLICATION : 17_winchat Project Overview
========================================================================

This project is  designed to run under Windows. It demonstrates a fault 
tolerant and distributed computing system. It as well demonstrates how 
dynamically create a model and use the dynamic subscription service 
during run-time.

The project consists of 3 sub-projects:
    1. generated (17_generated) -- generated codes and common objects.
    2. register  (17_register ) -- contains instances of public services.
    3. chatter   (17_chatter  ) -- dynamically creates models of services and clients.

All communications pass through mcrouter (multicasting router). Since in
AREG SDK the service discovery is automated and it provides fault tolerant
system, the sequence of processes to start does not play any role.

It automatically forms mist-network, where components receive connected or
disconnected notifications to start or stop calling remote methods.

////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 17_generated

Generated code of the HelloWorld.siml service interface document located
in './res' sub-folder. The document is created by an interface design tool
and compiled as a static library. In addition, it contains implementation
of objects that are used in other projects.

////////////////////////////////////////////////////////////////////////

        2. Project 'register' / 17_register

This project instantiates the instance of Public service used by all other 
applications. When the application starts, the user needs to enter the 
IP address of the multicast router (mcrouter) manuall to connect. 

////////////////////////////////////////////////////////////////////////

        3. Project 'chatter' / 17_chatter

This project contains an instance of public services and service clients,
where models are dynamically created and loaded during run-time.