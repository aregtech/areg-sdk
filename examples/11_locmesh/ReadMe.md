# 11_locmesh Project Overview

The [11_locmesh](https://github.com/aregtech/areg-sdk/tree/master/examples/11_locmesh) project demonstrates the execution of multiple instances of the same service, building upon the foundation of the [10_locsvc](https://github.com/aregtech/areg-sdk/tree/master/examples/10_locsvc) project. Each service instance must possess a unique name and be instantiated within different threads.

The project comprises two sub-projects:

1. [generated](https://github.com/aregtech/areg-sdk/tree/master/examples/11_locmesh/generated) (11_generated) - This sub-project contains the generated code from the [HelloWorld.siml](https://github.com/aregtech/areg-sdk/blob/master/examples/11_locmesh/res/HelloWorld.siml) *Service Interface* document, compiled as a static library.

2. [locsvcmesh](https://github.com/aregtech/areg-sdk/tree/master/examples/11_locmesh/locsvcmesh) (11_locmesh) - This sub-project is an application that instantiates multiple instances of the same service provider object and multiple instances of the service consumer within a multithreaded environment. It demonstrates the relationship between the service providers and service consumers. Notably, one of the instantiated providers possesses a special feature and is marked as *main* (`MainService`). The application continues to run until a *quit* message is triggered to halt its execution.
