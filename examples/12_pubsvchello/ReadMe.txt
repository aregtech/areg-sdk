========================================================================
    CONSOLE APPLICATION : 12_pubsvchello Project Overview
========================================================================

This project demonstrates the use of public service in a multi-tasking / multi-processing environment.
The project consists of 3 sub-projects:
    1. generated (12_generated)     -- is a static library of generated codes used by other processes.
	2. ipcsvchello (12_ipcsvchello) -- is a console application, containing an instance of public service.
	3. ipcusrhello (12_ipcusrhello) -- is a console application, containing a service client.

All communications pass through mcrouter (multicasting router). Since in AREG SDK the service discovery is automated,
it does not matter the order of processes to start. The system will start working when logical client and logical server
get connection via mcrouter. This approach forms a mist network and extends the fog network to the extreme edge.

////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 12_generated
        
Generated code of the service interface document located in './res' sub-folder.
The document is created by an interface designing tool.
The project is compiled as a static library.

////////////////////////////////////////////////////////////////////////

        2. Project 'ipcsvchello' / 12_ipcsvchello
        
This project contains instances of public service, which is discoverable in the network. The connected clients should specify the name of
service they wait and as soon as the service is available in the network, the clients get notification and start triggering requests.

////////////////////////////////////////////////////////////////////////

        3. Project 'ipcusrhello' / 12_ipcusrhello
        
This project contains an instance of a client, which triggers request calls to run on a logical server.
