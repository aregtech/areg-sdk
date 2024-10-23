
# 11_locmesh Project Overview

The **11_locmesh** project extends the [10_locsvc](./../10_locsvc) example to demonstrate the execution of multiple instances of the same service. Each instance runs in a separate thread and has a unique name.

### Sub-Projects:

1. **11_generated:** This is generated from the [HelloWorld.siml](./res/HelloWorld.siml) Service Interface document during configuration in CMake, or as a pre-build action in Visual Studio's `dummy` project.
   
2. **[11_locmesh](./locsvcmesh):** This sub-project instantiates multiple service provider and consumer objects in a multithreaded environment. It demonstrates how service providers and consumers interact. One of the providers is marked as *main* (`MainService`). The application runs until a *quit* message is sent to stop it.
