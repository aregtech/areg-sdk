========================================================================
    CONSOLE APPLICATION : 08_service Project Overview
========================================================================

This project demonstrates the use of simple local servicing object to create multithreading application.
It instantiates dummy (empty) local service interface, which is not visible outside of the process.
The service is created as an instance of Component, Stub (dummy Stub) and Timer consumer.
The service is registered in model that is loaded at start unloaded when application completes the job.
