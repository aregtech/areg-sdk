
## 00_helloservice Project Overview

The **00_helloservice** project is part of the developer guide that demonstrates a console-based application using **IPC** and **multithreading**. It showcases the creation of service and client objects and illustrates various deployment scenarios using the AREG framework.

### Project Structure:

1. **00_generated:** This is automatically generated from the [HelloService.siml](./res/HelloService.siml) Service Interface during CMake configuration or as a pre-build action in Visual Studio.

2. **[common](./common/):** Contains shared implementations of the Service Provider and Service Consumer components used across all sub-projects. This demonstrates how developers can create multithreading or multiprocessing applications by simply modifying the *Application Model*.

3. **[00_onethread](./onethread/):** Demonstrates a scenario where the service provider and client coexist within a single thread of the same process. The implementations of both components are located in the *common* directory.

4. **[00_twothreads](./twothreads/):** Showcases a setup where the service provider and client operate in two separate threads within the same process. Again, the components' implementation resides in the *common* directory.

5. **[00_pubservice](./multiprocess/serviceproc/):** Provides a *Public Service* that allows multiple service consumers to make requests. The implementation of the service is found in the *common* directory.

6. **[00_pubclient](./multiprocess/clientproc/):** Contains a client object for the *Public Service*. Multiple instances can be created, allowing several service consumer objects to communicate with the service provider. The implementation is also located in the *common* directory.

---

This project offers a complete demonstration of different deployment strategies and the usage of service and client objects, showcasing how services and consumers can interact across various threading and process configurations.