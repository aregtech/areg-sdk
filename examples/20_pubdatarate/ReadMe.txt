========================================================================
    CONSOLE APPLICATION : 20_pubdatarate Project Overview
========================================================================

This console project demonstrates the network communication data rate.
Base on the options given by the user, the service generates simple gray
bitmap image to deliver to connected clients. Both, servicing and client
applications output data rate information, so that the user by measuring
CPU load and RAM change may check the most optimal data rate for the application.
This application as well will be used in further versions as a benchmark
test to demonstrate communication improvements.

The project consists of 3 sub-projects:
    1. generated  (20_generated)         -- generated codes.
	2. pubservice (20_pubsvcdatarate)    -- public service provider.
	3. pubclient  (20_pubclientdatarate) -- public service client.

There is as well common objects used by server and client components.
The user may type on console commands to change options like
-   Image width, height and the number of lines per image block. 
    These parameters change the number of processing items per second.
-   Pixel dwell time, which is used as a time to generate 1 pixel data.
    These parameter change image data rate per second.

////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 20_generated
        
Generated code of Large Data service interface document located
in './res' sub-folder. The document is created by an interface design tool
and compiled as a static library.

////////////////////////////////////////////////////////////////////////

        2. Project 'pubservice' / 20_pubsvcdatarate
        
The application provides public service, which generates data and sends
to connected clients.

////////////////////////////////////////////////////////////////////////

        3. Project 'pubclient' / 20_pubclientdatarate
        
The application contains a service client component, which receives the
data from the service component.
