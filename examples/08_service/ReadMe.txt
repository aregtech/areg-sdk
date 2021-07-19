========================================================================
    CONSOLE APPLICATION : 08_service Project Overview
========================================================================

This project demonstrates how to create and use simple servicing object within one process.
It creates dummy (empty) local service interface that is visible only within the process, so that several instances of the same process can be started to perform same job.
The service is created as ServicingComponent object, which is an instance of Component object, Stub (dummy Stub) and is a Timer consumer.
On start, the prcess is initializing Logging, Service Manager and the Timer Manager (Timer Service).
It uses servicing model to load and initialize service and uses periodic timer to make "Hello Service!" output on console.
There can be several models used in application and they can be dynamically loaded by name or can be all loaded.
Each servicing threads and components shuld be described in the model.
When timer is not active anymore, it outputs "Goodbye Service..." and completes application.
The application does not have a client of the service.
When job is completed, the main thread unloads model and release the resources (stops Service Manager, Timer Manager and the logging).
