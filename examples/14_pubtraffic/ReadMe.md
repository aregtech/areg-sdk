# 14_pubtraffic Project Overview

The [14_pubtraffic](https://github.com/aregtech/areg-sdk/tree/master/examples/14_pubtraffic) project showcases dynamic model definition, loading and unloading of dynamic models, and the utilization of custom events in a multithreading application.

The project comprises three sub-projects:

1. [generated](https://github.com/aregtech/areg-sdk/tree/master/examples/14_pubtraffic/generated) (14_generated): This static library contains generated codes and common objects. It is created from the [SimpleTrafficLight.siml](https://github.com/aregtech/areg-sdk/blob/master/examples/14_pubtraffic/res/SimpleTrafficLight.siml) *Service Interface* document using the `codegen.jar` tool.

2. [pubservice](https://github.com/aregtech/areg-sdk/tree/master/examples/14_pubtraffic/pubservice) (14_pubservice): This sub-project provides both public and local services and handles custom events triggered by user interactions. Users can initiate commands such as `start` to begin service jobs, `stop` to halt service jobs, and `quit` or `q` to exit the applications (which also closes all others). To initiate the changing of light states, simply type `start`.

3. [pubclient](https://github.com/aregtech/areg-sdk/tree/master/examples/14_pubtraffic/pubclient) (14_pubclient): This sub-project features a service consumer that dynamically creates models during runtime. Multiple instances of the same application can be started and run, with each service provider instance generated with a unique name. Upon application startup, users can choose one of the following commands: `sn` to subscribe to one set of data updates or `ew` to subscribe to another set of data updates.

Communication within the system is facilitated by `mcrouter`, a multicast router that operates on any machine within the network. The AREG SDK automates service discovery and ensures a fault-tolerant system, making the order of process startup irrelevant.
