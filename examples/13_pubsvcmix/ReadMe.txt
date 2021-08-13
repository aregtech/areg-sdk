========================================================================
    CONSOLE APPLICATION : 13_pubsvcmix Project Overview
========================================================================

This project demonstrates the use of mixed public and local services.
It is an extension of 12_pubsvchello project with multiple public and multiple local services 
to create a complex network and provide IPC. This forms a mesh of services and enables distributed computing.

The project consists of 4 sub-projects:
    1. generated    (13_generated   ) -- is a static library of generated codes and common objects used by other processes.
    2. ipcmixmain   (13_ipcmixmain  ) -- is a console application, containing instances of public and local services, and a local client.
    3. ipcmixsecond (13_ipcmixsecond) -- is a console application, containing mixed instances of public and local services and clients.
    4. ipcmixclient (13_ipcmixclient) -- is a console application, containing instances of local and public service clients.

All communications pass through mcrouter (multicasting router). Since in AREG SDK the service discovery is automated,
it does not matter the order of processes to start. The system will start working when logical client and logical server
get connection via mcrouter. This approach forms a mist network and extends the fog network to the extreme edge.

The multiple instances of public services are instantiated in processes and must have unique names in the network.
The multiple instances of local services must have unique names in the same process, but can have same names in different processes.

////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 13_generated

Generated code of the service interface documents located in './res' sub-folder.
The documents are created by an interface designing tool.
In addition, it contains implementation of components that are used in other projects.
The project is compiled as a static library.


////////////////////////////////////////////////////////////////////////

        2. Project 'ipcmixmain' / 13_ipcmixmain
        
This project contains an instance of the main service component, which is able to initiate the system shutdown procedure.
It also contains local service and local clients. The application initiates system shutdown when certain
amounts of request calls are done. The system shutdown closes all other applications.

////////////////////////////////////////////////////////////////////////

        3. Project 'ipcmixsecond' / 13_ipcmixsecond
        
This project contains instances of public and local services, and the clients that communicate with Main Service.
The application is closed when Main Service initiates system shutdown.

////////////////////////////////////////////////////////////////////////

        4. Project 'ipcmixclient' / 13_ipcmixclient
        
This project contains instances of public clients,  and local service and a client for multithreading communication.
The application is closed when Main Service initiates system shutdown.
