========================================================================
    CONSOLE APPLICATION : 10_svchello Project Overview
========================================================================

This project demonstrates how to initialize and use service.
The application initializes a local servicing component, so that it cannot be accessed outside of the process.
Andit is using the service client to trigger simple request and get response.
The service interface consists of the attributes, requests, response and broadcasting information.
There are 2 types of requests defined:
    - request that contains a response;
    - request without a response.
    
The client uses timer to trigger request to execute on servicing component. 
The client as well during runtime subsribes and unsubsribes on attribute data update and broadcasting information, which can be unsubscribed.
