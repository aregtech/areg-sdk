========================================================================
    CONSOLE APPLICATION : 15_pubsvcworker Project Overview
========================================================================

This project demonstrates use of worker thread to perform parallel jobs and custom events for multi-threading 
communication. It also demonstrates how to describe a worker thread in the model that is automatically created 
when the model is loaded.

The project consits of 3 sub-projects:
    1. generated     (15_generated    ) -- is a static library of generated codes and common data used by other processes.
    2. ipchwmgr      (15_ipchwmgr     ) -- is a console application, containing instances of the public service client and worker thread.
    3. ipcsvcpatient (15_ipcsvcpatient) -- is a console application, containing instances of the public service and the worker thread.

All communications pass through mcrouter (multicasting router). Since in AREG SDK the service discovery is 
automated, it does not matter the order of processes to start. The system will start working when logical client 
and logical server get connection via mcrouter. This approach forms a mist network and extends the fog network to 
the extreme edge.


////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 14_generated

Generated code of the service interface documents located in './res' sub-folder.
The documents are created by the interface designing tool. In addition, it contains implementation of components 
that are used in other projects. The project is compiled as a static library.


////////////////////////////////////////////////////////////////////////

        2. Project 'ipchwmgr' / 15_ipchwmgr

This project is describing worker thread in the model, where worker thread is automatically created when the model 
is loaded. The worker thread imitates the communication with the device (virtual device / console). The worker 
thread is bound to the component and is used for the communication between the component object and the worker 
thread.


////////////////////////////////////////////////////////////////////////

        3. Project 'ipcsvcpatient' / 15_ipcsvcpatient

This project contains instance of public service and the worker thread, which receives data from console 
(virtual device) and directly updates the attribute of the service. The service and the worker threads are 
described in the static model, and they are automatically instantiated when the model is loaded. The worker thread 
is bound to the component and is used for the communication between the component object and the worker thread.

The user inputs patient data on the console (worker thread context) and when the user is completed to input data, 
the appropriate attribute of the service is updated to send notification to the connected service client.
