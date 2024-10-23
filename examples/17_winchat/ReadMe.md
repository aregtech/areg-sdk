## 17_winchat Project Overview

> [!IMPORTANT]
> The **17_winchat** project is designed exclusively for Windows and requires **MFC** (*Microsoft Foundation Classes*) for compilation with Microsoft Visual C++.

The **17_winchat** project is a Windows GUI application showcasing a fault-tolerant distributed system. It demonstrates dynamic model creation and subscription services during runtime.

### Sub-Projects:

1. **17_generated:** Generated from [CentralMessager.siml](./res/CentralMessager.siml), [ConnectionManager.siml](./res/ConnectionManager.siml), [DirectConnection.siml](./res/DirectConnection.siml), and [DirectMessager.siml](./res/DirectMessager.siml) during CMake configuration or as a pre-build action in Visual Studio's `dummy` project.

2. **[17_register](./register/):** Instantiates a *Public Service* used by other applications. Users manually input the IP address of the `mcrouter` during startup.

3. **[17_chatter](./chatter/):** Provides *Public Services* and *Service Consumers*. It dynamically creates models during runtime, requiring users to manually input the `mcrouter` IP address upon startup.

Communication is facilitated by `mcrouter`. The AREG SDK ensures automatic service discovery and fault tolerance, making the order of process startup irrelevant.

