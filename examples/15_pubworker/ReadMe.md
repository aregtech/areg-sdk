## 15_pubworker Project Overview

The **15_pubworker** project demonstrates the use of *Worker Threads* and the processing of custom events in both Worker and Component threads.

The project consists of three sub-projects:

1. **15_generated:** This is generated from the [PatientInformation.siml](./res/PatientInformation.siml) Service Interface document during CMake configuration or as a pre-build action in Visual Studio's `dummy` project.

2. **[15_pubclient](./pubclient/):** This sub-project includes a *Service Consumer* and a *Worker Thread* in the model. The Worker Thread simulates an output device.

3. **[15_pubservice](./pubservice/):** This sub-project includes a *Public Service Provider* and a *Worker Thread*. The Worker Thread processes user inputs from the console and updates the Service's attributes. Both the Service and the Worker Thread are part of the static model, loaded at startup and unloaded at exit.

All communication is handled via `mcrouter`, a multicast router that works on any network machine. The AREG SDK automates service discovery and ensures fault tolerance, making the startup order of processes irrelevant.
