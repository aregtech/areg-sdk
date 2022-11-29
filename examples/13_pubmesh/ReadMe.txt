========================================================================
    CONSOLE APPLICATION : 13_pubmesh Project Overview
========================================================================

This project demonstrates the use of mixture of multiple Public and Local
services, where they form and easily handle complex networks. This complex
network is formed by mesh of Public services, where AREG SDK engine handles
and enables distributed computing.

The project consists of 4 sub-projects:
    1. generated  (13_generated)  -- generated codes and common objects.
    2. pubservice (13_pubservice) -- instances of public and local services.
    3. pubsvcmesh (13_pubsvcmesh) -- mixed instances of services and clients.
    4. pubclients (13_pubclient) -- public and local service clients.

All communications pass through mcrouter (multicasting router). Since in
AREG SDK the service discovery is automated and it provides fault tolerant
system, the sequence of processes to start does not play any role.

It automatically forms mist-network, where components receive connected or
disconnected notifications to start or stop calling remote methods.

////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 13_generated

Generated code of the HelloWorld.siml service interface document located
in './res' sub-folder. The document is created by an interface design tool
and compiled as a static library. In addition, it contains implementation
of objects that are used in other projects.

////////////////////////////////////////////////////////////////////////

        2. Project 'pubservice' / 13_pubservice
        
This project is marked as 'main' and contains instances of the Public
service components, where one service (ServiceShtodwn) handles the 
system shutdown logic, so that the service clients get notification 
when the 'main' application exits and can as well stop working.

////////////////////////////////////////////////////////////////////////

        3. Project 'pubsvcmesh' / 13_pubsvcmesh
        
This project contains multiple instances of Public and Local services
that form a mesh of services and make cross-communication. The application
shuts down when the 'main' service completes the job and exits.

////////////////////////////////////////////////////////////////////////

        4. Project 'pubclients' / 13_pubclient
        
This project contains multiple instances of Public service clients to 
invoke remote method calls. The system guarantees that the each call
is delivered to the target and the messages are not mixed or lost. The 
application shuts down when the 'main' service completes the job and
exits.
