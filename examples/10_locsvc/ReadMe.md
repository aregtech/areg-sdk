
# 10_locsvc Project Overview

The **10_locsvc** project demonstrates the use of a *Local Service*. The interface definition is located in the [HelloWorld.siml](./res/HelloWorld.siml) file, and the corresponding source code is generated during compilation using `codegen.jar`.

This project illustrates how to create both a provider and a consumer of the *Local Service*, which is only accessible within the process and not externally.

### Sub-Projects:

1. **10_generated:** This is generated during configuration in CMake, or as a pre-build action in Visual Studio's `dummy` project.
   
2. **[10_locservice](./locservice):** This sub-project provides the implementation for both the service provider and consumer. It showcases various features such as request handling, response processing, broadcasting, and dynamic attribute subscription/unsubscription during runtime.
