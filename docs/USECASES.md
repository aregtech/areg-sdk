# AREG SDK use cases and benefits

AREG SDK can be used in a very large scope of multithreading and multiprocessing application development.  This document showcases several examples of how the SDK can be utilized to develop intelligent devices.

### Distributed solution

<details open><summary> Click to show / hide <code>distributed solution</code>.</summary><br/>

The AREG SDK is a distributed services solution that enables components to interact seamlessly across nodes on the network, appearing as if they are located within a single process. To define relationships and distribute services across processes, developers create runtime loadable models.

The following is a demonstration of 2 _models_ defined in 2 different processes, where one _model_ defines **service provider** and the second is at the same time is a **service consumer** (client) and a **service provider**. For simplicity, let's name them `service.cpp` and `mixed.cpp`.

A: _model of service provider component in **service.cpp**_:
```cpp
// service.cpp, define service provider
BEGIN_MODEL("MyModel")

  BEGIN_REGISTER_THREAD( "Thread1" )
    BEGIN_REGISTER_COMPONENT( "SystemShutdown", SystemShutdownService )
      REGISTER_IMPLEMENT_SERVICE( NESystemShutdown::ServiceName, NESystemShutdown::InterfaceVersion )
    END_REGISTER_COMPONENT( "SystemShutdown" )
  END_REGISTER_THREAD( "Thread1" )

END_MODEL("MyModel")
```

B: _model of service consumer and service provider (mixed) component in **mixed.cpp**_:
```cpp
// mixed.cpp, define service provider and service consumer.
BEGIN_MODEL("MyModel")

  BEGIN_REGISTER_THREAD( "Thread1" )
    BEGIN_REGISTER_COMPONENT( "RemoteRegistry", RemoteRegistryService )
      REGISTER_IMPLEMENT_SERVICE( NERemoteRegistry::ServiceName, NERemoteRegistry::InterfaceVersion )
      REGISTER_DEPENDENCY("SystemShutdown")
    END_REGISTER_COMPONENT( "RemoteRegistry" )
  END_REGISTER_THREAD( "Thread1" )

END_MODEL("MyModel")
```
After declaration of services, each project can load them in the `main()` method and load them as it is shown in the next example.

C: _common code_ for **service.cpp** and **mixed.cpp** files:
```cpp
int main()
{
    Application::initApplication( );    // initialize defaults
    Application::loadModel("MyModel");  // load model, start services
    Application::waitAppQuit( );        // wait for quit signal
    Application::unloadModel("MyModel");// unload model, stop services
    Application::releaseApplication();  // release resources

    return 0;
}
```

In these codes:
1. **service.cpp** contains model to register service `"SystemShutdown"` that implements interface `NESystemShutdown::ServiceName`.
2.  **mixed.cpp** contains model to register service `"RemoteRegistry"` that implements interface `NERemoteRegistry::ServiceName` and consumes (requires) service `"SystemShutdown"`.
3. `int main()` is identical in both files. It initializes resources, loads model and waits for the completion.

The developers should create `SystemShutdownService` and `RemoteRegistryService` objects that implement the business logic of _request_ methods of provided and _response_ methods of used service interfaces. When these processes start, the services are automatically discovered via `mcrouter`. With this technique, the projects easily develop multiprocessing applications where provided services can be distributed and accessed remotely within the network formed by `mcrouter`.

An example of developing a service provider and consumer in one and multiple processes is in [**Hello Service!**](https://github.com/aregtech/areg-sdk/blob/master/docs/HelloService.md) guide. As well there are multiple [examples](https://github.com/aregtech/areg-sdk/tree/master/examples/) of multiprocessing and multithreading applications.
</details>

### Driverless devices

<details open><summary> Click to show / hide <code>driverless devices</code>.</summary><br/>

Normally, the devices are supplied with the drivers to install in the system. 

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/driver-solution.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/driver-solution.png" alt="kernel-mode driver solution" style="width:70%;height:70%"/></a></div>

Proposed are driverless service-enabled devices that do not require installation of drivers in the system.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/driverless-solution.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/driverless-solution.png" alt="AREG SDK driverless solution" style="width:70%;height:70%"/></a></div>

The services, described in the interface prototype documents, are faster and easier to develop than drivers, as they do not require special techniques and can be developed like user mode applications. Service development reduces risks and the code generator simplifies client object generation from a service interface document.
</details>

### Real-time solutions

<details open><summary> Click to show / hide <code>real-time solutions</code>.</summary><br/>

AREG engine automatically generates and delivers messages to the target and invokes the exact methods of the target objects in real-time with ultra-low networking latency. This makes it ideal for developing time-sensitive applications for industries such as automotive, drone fleets, medtech, real-time manufacturing, and monitoring.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-sdk-features.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-sdk-features.png" alt="AREG SDK and multicast features" style="width:70%;height:70%"/></a></div>
</details>

### Digital twin

<details open><summary> Click to show / hide <code>digital twin</code>.</summary><br/>

AREG framework's event-driven and service-oriented architecture, coupled with real-time communication, offers a robust solution for digital twin applications. This framework allows for visualization, monitoring, and control of external devices, while also enabling immediate reaction to changes in the environment or device state. AREG's approach eliminates the need for additional communication layers, making it an ideal solution for emergency, security, and safety applications.

</details>

### Simulation and tests

<details open><summary> Click to show / hide <code>simulation and tests</code>.</summary></br>

Simulating a software environment is a practical solution for testing and checking rapidly changing software, especially when hardware provisioning is not feasible. Simulations are portable, accessible, and help to optimize solutions while avoiding unnecessary risks. Projects using simulations are better prepared for remote work and easier to outsource.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/software-layers.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/software-layers.png" alt="Software application 4 layers" style="width:70%;height:70%"/></a></div>

The distributed and service-oriented solution of AREG engine eases application testing in a simulated environment. For example, the Simulation application may provide identical Data layer services for seamless testing of the rest of the application.

The same technique of simulating data can be used to create API-driven test automations.
</details>
