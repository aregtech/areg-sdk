## 23_pubsubmix Project Overview

The **23_pubsubmix** project demonstrates a Pub/Sub service in a multi-process environment with both Publishers and Subscribers running in the same process or across different processes. It shows how local Subscribers continue to receive updates even when the network is disconnected.

The project uses ORPC for inter-process communication, with a *Service Provider* (Publisher) and *Service Consumer* (Subscriber) dynamically subscribing to and receiving updates. The system ensures that subscribers always receive the latest data upon subscribing, even if the data hasn’t been updated recently.

The project consists of three sub-projects:

1. **23_generated:** This is generated from the [PubSubMix.siml](./res/PubSubMix.siml) Service Interface document during CMake configuration or as a pre-build action in Visual Studio's `dummy` project.
2. **[23_common](./common/):** A static library containing shared objects used by other processes.
3. **[23_pubsubctrl](./pubsubctrl/):** A console application acting as both a *Data Publisher* and *Subscriber*. It also sends a shutdown signal to all connected processes.
4. **[23_pubsubdyn](./pubsubdyn/):** A console application that acts as both a *Data Publisher* and *Subscriber*. It connects to the `23_pubsubctrl` service to receive data and subscribe to updates.

Communication is managed through `mcrouter`, and the AREG SDK ensures fault tolerance and service discovery, making process startup order irrelevant.
