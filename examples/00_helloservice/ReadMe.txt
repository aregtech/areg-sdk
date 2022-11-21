========================================================================
    CONSOLE APPLICATION : 00_helloservice Project Overview
========================================================================

This project is an example in developer guide. It is used to explain the
basics to create the service and client objects. As well how to register
service and client in one thread, in two threads and in multiple (two)
processes to demonstrate the distribution possibility of the framework.

The project consist of 5 major projects, where 2 of them are dependent:
    1. generated    (00_generated   )       -- generated code.
    2. common       (               )       -- not a project, a common folder that contains service and client objects.
    3. onethread    (00_onethread   )       -- creating service and client in one thread.
    4. twothreads   (00_twothreads  )       -- creating service and client in two threads.
    5. multiprocess (00_serviceproc )       -- service and client in separate processes. It has subprojects:
        5.1. serviceproc (00_pubservice)    -- the service provider.
        5.2. clientproc  (00_pubclient )    -- the service client.


////////////////////////////////////////////////////////////////////////

        1. Project 'generated' / 00_generated

Generated code of the HelloService.siml service interface document located
in './res' sub-folder. The document is created by an interface design tool
and compiled as a static library.

Run codegen.jar from project root ( 00_helloservice) folder to recover or update generated sources.

$ java -jar ./../../tools/codegen.jar --root=./ --doc=res/HelloService.siml --target=generated/src

////////////////////////////////////////////////////////////////////////

        2. 'common'

It is not a project, but contains objects that are used in every project.
The agenda of this folder and objects is to show that all objects are the same.

////////////////////////////////////////////////////////////////////////

        3. Project 'onethread' / 00_onethread

Simple application that contains service and client in one thread of the same process.
The service and client are implemented in "common" folder.

////////////////////////////////////////////////////////////////////////

        4. Project 'twothreads' / 00_twothreads

Simple application that contains service and client in 2 threads of the same process.
The service and client are implemented in "common" folder.

////////////////////////////////////////////////////////////////////////

        5. Projects in 'multiprocess'


        5.1. Project 'serviceproc' / 00_pubservice

This project is part of "multiprocess".
Simple application that contains only Public service component, so that the clients may connect and call request.
The service is implemented in "common" folder.

        5.2. Project 'clientproc' / 00_pubclient

This project is part of "multiprocess".
Simple application that contains only client component of Public service to call request on service side.
It is as well possible to instantiate multiple instances of '00_clientproc' process.
The client is implemented in "common" folder.
