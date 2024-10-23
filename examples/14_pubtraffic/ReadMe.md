
## 14_pubtraffic Project Overview

The **14_pubtraffic** project demonstrates dynamic model creation, loading/unloading, and the handling of custom events in a multithreaded application.

### Sub-Projects:

1. **14_generated:** Generated from the [SimpleTrafficLight.siml](./res/SimpleTrafficLight.siml) Service Interface document during CMake configuration or as a pre-build action in Visual Studio's `dummy` project.
   
2. **[14_pubclient](./pubclient/):** A service consumer that dynamically creates models during runtime. Users can start multiple instances of this application and subscribe to different data sets using commands like `sn` (data of conventional *South-North* direction to subscribe) or `ew` (data of conventional *East-West* direction to subscribe).

3. **[14_pubservice](./pubservice/):** Provides both *Public* and *Local* services, handling custom events triggered by user commands like `start` (to begin service jobs), `stop` (to halt jobs), and `quit` (to exit).


Communication is handled via `mcrouter`. The AREG SDK ensures fault tolerance and service discovery automation, making the order of process startup irrelevant.
