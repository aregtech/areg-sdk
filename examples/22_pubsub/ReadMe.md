## 22_pubsub Project Overview

The **22_pubsub** project demonstrates a *Public Service* in a multi-process environment, following the Pub/Sub (Publish/Subscribe) model. It creates two processes: one publishes data, and the other subscribes to receive updates. There are two types of notifications:
- **On Change:** Notifications are sent only if the value changes.
- **Always:** Notifications are sent each time the value is set, regardless of whether it changes.

The system uses Object RPC (ORPC) for inter-process communication. Two applications are involved: one serves as a *Service Provider* (Publisher) and the other as a *Service Consumer* (Subscriber), which can dynamically subscribe or unsubscribe from data updates. The system guarantees that subscribers receive the latest data upon subscribing, even if it hasn't been recently updated.

The project consists of three sub-projects:

1. **22_generated:** This is generated from the [PubSub.siml](./res/PubSub.siml) Service Interface document during CMake configuration or as a pre-build action in Visual Studio's `dummy` project.
2. **[22_publisher](./publisher/):** A console application acting as a *Public Service* provider, publishing data for multiple remote subscribers.
3. **[22_subscriber](./subscriber/):** A console application acting as a *Public Service* consumer that subscribes to data provided by the *Publisher*. Multiple instances of this can run, ensuring that all receive the same data.

Communication is handled through `mcrouter`, with the AREG SDK automating service discovery and ensuring fault-tolerant system behavior, so process startup order doesn’t impact functionality.
