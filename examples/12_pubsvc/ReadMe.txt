========================================================================
    CONSOLE APPLICATION : 12_pubsvc Project Overview
========================================================================

This project demonstrates the use of public service in a multi-processing
environment based on IPC. The demo creates 2 applications, where one
provides service and the other uses service by calling predefined remote
requests and receives replies (responses).

The project consists of 3 sub-projects:
    1. generated  (12_generated)    -- generated codes.
	2. pubservice (12_pubservice)   -- public service provider.
	3. pubclient  (12_pubclient)    -- public service client.

All communications pass through mcrouter (multicasting router). Since in
AREG SDK the service discovery is automated and it provides fault tolerant
system, the sequence of processes to start does not play any role.

It automatically forms mist-network, where components receive connected or
disconnected notifications to start or stop calling remote methods.

////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 12_generated
        
Generated code of the HelloWorld.siml service interface document located
in './res' sub-folder. The document is created by an interface design tool
and compiled as a static library.

////////////////////////////////////////////////////////////////////////

        2. Project 'pubservice' / 12_pubservice
        
The application provides network discoverable Public service, which 
predefined methods are called from remote clients.

////////////////////////////////////////////////////////////////////////

        3. Project 'pubclient' / 12_pubclient
        
The application contains a Public service client software component.
It receives connected notification and starts calling remote method of
the Public service. To make periodic calls, it uses a timer.
