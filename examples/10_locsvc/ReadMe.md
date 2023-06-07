# 10_locsvc Project Overview

The [10_locsvc](https://github.com/aregtech/areg-sdk/tree/master/examples/10_locsvc) project demonstrates the utilization of a *Local Service*. The interface definition can be found in the [HelloWorld.siml](https://github.com/aregtech/areg-sdk/blob/master/examples/10_locsvc/res/HelloWorld.siml) file, and the corresponding source code is generated using the `codegen.jar` application. 

The project showcases the creation of a provider and a consumer of the *Local Service*, which is only accessible within the process and cannot be accessed externally.

The project is composed of two sub-projects:

1. [`generated`](https://github.com/aregtech/areg-sdk/tree/master/examples/10_locsvc/generated) (10_generated) - This sub-project is a static library that contains the generated code utilized by the application.

2. [`locservice`](https://github.com/aregtech/areg-sdk/tree/master/examples/10_locsvc/locservice) (10_locservice) - This sub-project provides the implementation of the service provider and the service consumer. It offers various functionalities such as request handling, response processing, broadcast, and dynamic attribute subscription and unsubscription during runtime.
