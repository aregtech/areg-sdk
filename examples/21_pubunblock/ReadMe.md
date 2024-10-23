## 21_pubunblock Project Overview

The **21_pubunblock** project demonstrates how to manually unblock service requests. When a request is in progress with no response, it’s marked as *busy*. If another client makes the same request, it receives a *failure* response indicating the request is still busy. However, manual unblocking allows the service provider to continue processing new requests. Once ready, the provider sends responses to specific consumers.

The project consists of three sub-projects:

1. **21_generated:** This is generated from the [HelloUnblock.siml](./res/HelloUnblock.siml) Service Interface document during CMake configuration or as a pre-build action in Visual Studio's `dummy` project.
2. **[21_pubservice](./pubservice/):** This sub-project provides a *Public Service* that processes requests, unblocks them, and sends responses after a timeout. Each response includes sequence numbers to validate the process.
3. **[21_pubclient](./pubclient/):** This sub-project includes a *Service Client* that sends frequent requests, causing the Service Provider to accumulate pending requests marked as *busy*.

All communication occurs via `mcrouter`, with the AREG SDK ensuring automatic service discovery and fault tolerance, making process startup order irrelevant.
