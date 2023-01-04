========================================================================
    CONSOLE APPLICATION : 21_pubunblock Project Overview
========================================================================

This console project demonstrates the request manual unblock.
While request is processing and no response is sent, it is marked as 'busy'.
While request is busy, any client that sends same request will be replied
with the failure "request is busy". It is possible manually unblock the
request, so that the service can process more requests and when the response
is ready, can prepare the system to send appropriate response as if it
would work in a usual way.

The project consists of 3 sub-projects:
    1. generated  (21_generated)    -- generated codes.
	2. pubservice (21_pubservice)   -- public service provider.
	3. pubclient  (21_pubclient)    -- public service client.


////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 21_generated
        
Generated code of 'HelloUnblock' service interface document located
in './res' sub-folder. The document is created by an interface design tool
and compiled as a static library.

////////////////////////////////////////////////////////////////////////

        2. Project 'pubservice' / 21_pubservice
        
The application provides public service, which receives requests, unblocks,
and replies with the response after some timeout. Each request will receive
appropriate response, so that no response is missed or mixed. To check this,
the service resplies to the client with the generated sequence number.

////////////////////////////////////////////////////////////////////////

        3. Project 'pubclient' / 21_pubclient
        
The application contains a service client component, which sends request more
frequent than the service is processing them. So that, each time the client sends
request, the service already has a pending request. And since the service manually
unblocks the request, it is able to process more and later reply with the response.
