========================================================================
    CONSOLE APPLICATION : 14_pubsvclight Project Overview
========================================================================

This project demonstrates the use of custom events used in multithreading communication.
It as well demonstrates how to dynamically create a model  during run-time that can be loaded and unloaded.

The project consists of 3 sub-projects:
    1. generated    (14_generated   ) -- is a static library of generated codes and common objects used by other processes.
    2. ipclightctrl (14_ipclightctrl) -- is a console application, containing instances of public and local services. It creates and processes custom events.
    3. ipctraffic   (14_ipctraffic  ) -- is a console application, containing instances of a public service client. It dynamically creates and loads a model during runtime.

All communications pass through mcrouter (multicasting router). Since in AREG SDK the service discovery is automated,
it does not matter the order of processes to start. The system will start working when logical client and logical server
get connection via mcrouter. This approach forms a mist network and extends the fog network to the extreme edge.

////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 14_generated

Generated code of the service interface documents located in './res' sub-folder.
The documents are created by the interface designing tool.
In addition, it contains implementation of components that are used in other projects.
The project is compiled as a static library.


////////////////////////////////////////////////////////////////////////

        2. Project 'ipclightctrl' / 14_ipclightctrl

This project creates an instance of traffic light service. It also defines, creates and processes custom events.
When the application starts, the user needs to type one of following commands:
- "start" to set lights ON and start the job.
- "stop" to stop the job and set light states OFF.
- "quit" or "q", to stop and exit applications. This will also close all clients.

Type "start" to start changing light states.


////////////////////////////////////////////////////////////////////////

        3. Project 'ipctraffic' / 14_ipctraffic

This project contains an instance of a traffic light client that gets the light state and outputs on console.
The traffic light has symbolic directions South-North and East-West.
This project creates a dynamic model. There can be several instances of application started.
When start application, select one of options to receive the appropriate traffic direction light:
- Type "sn" for the traffic light state that controls South-North direction.
- Type "ew" for the traffic light state that controls East-West direction.
