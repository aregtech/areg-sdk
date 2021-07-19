========================================================================
    CONSOLE APPLICATION : 12_ipchello Project Overview
========================================================================

This project demonstrates remote service interface functinality in a multiprocessing environment. By this, it forms simple model of servicing network.
The project consits of 3 separate sub-projects:
    1. shripchello (12_shripchello) -- is a static library, contains collection of servicing ultra-small server and client classes to be used in other processes.
	2. svcipchello (12_svcipchello) -- is a console application that contains instance of remote servicing component.
	3. usripchello (12_usripchello) -- is a console application that contains instance of remote service client.

All communications pass through mcrouter (multicastin router).
If processes start when router is not started yet, the system will do nothing, no messages will be forward, no communication will happen.
As soon as the router is available, the client will automatically discover the service in the network and start messaging.
It is recommended to start router before any process starts to make sure that the IP address and port number are correct and can be used.
But in principal, there is no restriction of sequences starting neither router, nor servicing processes.
The automatic service discovery will make sure to trigger notification when the appropriate service is available in the network.
This aproach forms a mist network and provides mist-computing on extreme edge, and extends fog network.

////////////////////////////////////////////////////////////////////////

        1. Project 'shripchello' / 12_shripchello
        
The 'shripchello' project contains collection of generated srver and client objects. The classes are generated out of "./res/HelloWorld.siml".
To generate servicing objects, see instructions in "tools" folder of AREG SDK.
The additional service interface design tool can be used to design the interface.
This project is a static library to include in other projects.

////////////////////////////////////////////////////////////////////////

        2. Project 'svcipchello' / 12_svcipchello
        
The 'svcipchello' project contains instance of remote servicing component that requests are triggered outside of the process.
The project defines model and specifies the implemented service interface name. 
All clients of the remote servicing compoennt should specify component as dependency and should instantiate one of servicing clients of the component.
The application waits for certain amount of request calls and triggers application quit event.
When application quits, the client is automatically closed, since gets "not connected" notification.

////////////////////////////////////////////////////////////////////////

        3. Project 'usripchello' / 12_usripchello
        
The 'usripchello' project contains instance of remote service client that periorically triggers request and gets response.
The client as well during runtime subscribes and unsubscribes on certain events such as attribute data update or subscribing on broadcast information.
The client runs as long, until the service is running. Then service completes job and shuts down from network, the client gets disconnect notification and completes application.
