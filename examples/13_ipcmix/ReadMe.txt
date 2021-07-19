========================================================================
    CONSOLE APPLICATION : 13_ipcmix Project Overview
========================================================================

13_ipcmix is an extension of 12_ipchello to demonstrate more mixed and complex network of services.
This project demonstrates multiple remote and local service interface functinalities in a multiprocessing environment. 
By this, it forms complex form of mist and creates distributed network of services.

The project consits of 4 separate sub-projects:
    1. shareipcmix  (13_shareipcmix ) -- is a static library, contains collection of servicing ultra-small server and client classes to be used in other processes.
    2. mainipcmix   (13_mainipcmix  ) -- is a console application that contains instances of remote and local servicing component and the instance of local service client.
    3. secondipcmix (13_secondipcmix) -- is a console application that contains instances of remote and local servicing components, as well as remote service clients.
    4. clientipcmix (13_clientipcmix) -- is a console application that contains instance of local servicing component and several instances of remote servicing clients.

All communications pass through mcrouter (multicastin router).
If processes start when router is not started yet, the system will do nothing, no messages will be forward, no communication will happen.
As soon as the router is available, every client automatically gets service available notification to start messaging.
It is recommended to start router before any process starts to make sure that the IP address and port number are correct and can be used.
But in principal, there is no restriction of sequences starting neither router, nor servicing processes.
The automatic service discovery will make sure to trigger notification when the appropriate service is available in the network.
This project forms a complex mist network, provides mist-computing on extreme edge, and extends fog network.
The project instantiates the RemoteRegistry service interface, and since the service interface is accessible from other processes,
each service has own unique name to be accesses in the network.
In addition, every process contains instance of LocalHelloWorld local service and since the service is local and cannot be accessed outside of
process, in each project all services have same name.
Unlike remote service that should have unique name in the network, the uniqueness of local service is provided within one process.

////////////////////////////////////////////////////////////////////////

        1. Project 'shareipcmix' / 13_shareipcmix
        
The 'shareipcmix' project contains collection of generated srver and client objects. 
The classes are generated out of service inteface documents located in "./res/" folder.
To generate servicing objects, see instructions in "tools" folder of AREG SDK.
The additional service interface design tool can be used to design the interface.
This project is a static library to include in other projects.

////////////////////////////////////////////////////////////////////////

        2. Project 'mainipcmix' / 13_mainipcmix
        
The 'mainipcmix' project contains instance of remote servicing component that plays role of the main service. 
The component contains instance of SystemShutdown service interface to register clients and trigger system shutdown.
Every process contains at least one client of the SystemShutdown service interface to trigger "application quit" when service initiates "shutdown".
The project in addition contains local service with the fixed name that is not accessible outside of process.
The project defines model and specifies the list of implemented service intefaces.
A sngle service can be an implementation of several service intefaces.
All clients of the remote servicing compoennt should specify component name as dependency and should instantiate one of service interface clients.
The application waits for certain amount of request calls and triggers system shutdown, so that, all processes complete job.

////////////////////////////////////////////////////////////////////////

        3. Project 'secondipcmix' / 13_secondipcmix
        
The 'secondipcmix' project contains instance of RemoteRegistry remote services
Each such instance of service should have unique name within the network.
In addition, it has several clients of RemoteRegistry service interface (service names "TestRemoteSecondService" and "TestRemoteThirdService").
Each of RemoteRegistry interface services have unique name within the network and run in separate threads.
It as well has instance of local service with the fixed name (same name as it is in 'mainipcmix').
The components are loaded via mode that contains list of implemented and dependent services.
The application runs until does not get "system shutdown" notification from main servicing component.
All implemented services and dependencies are described in the model that is loaded during runtime.

////////////////////////////////////////////////////////////////////////

        4. Project 'clientipcmix' / 13_clientipcmix
        
The 'clientipcmix' project contains instances of clients of all remote services to interact.
In addition, it has instance of local service with the fixed name (as as it is in 'mainipxmix' or 'secondipxmix').
The process runs as long, until gets "system shutdown" notificaton from main servicing components.
All depdendencies of remote services and the local service are described in the model that is loaded during runtime.
