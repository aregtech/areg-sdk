## 24_pubsubmulti Project Overview

The **24_pubsubmulti** project demonstrates the use of a *Public Service* in a multi-process environment with one process acting as a data publisher and another containing two subscribers running in the same thread. The example shows how AREG's Pub/Sub feature minimizes event generation by delivering only necessary updates.

Subscribers are activated at different times but are subscribed to the same data. The system ensures that messages are delivered efficiently. There are two types of notifications:
- **On Change:** Notifications are sent only if the value changes.
- **Always:** Notifications are sent every time the value is set, regardless of whether it changes.

The project consists of three sub-projects:

1. **24_generated:** This is generated from the [PubSub.siml](./res/PubSub.siml) Service Interface document during CMake configuration or as a pre-build action in Visual Studio's `dummy` project.
2. **[24_publisher](./publisher/):** A console application providing a network-discoverable *Public Service* that publishes data.
3. **[24_subscribermulti](./subscriber/):** A console application with two *Service Subscribers* running in the same thread, subscribing to the data updates provided by the *Publisher*.

Communication occurs through `mcrouter`, with AREG SDK ensuring service discovery and fault tolerance, making process startup order irrelevant.
