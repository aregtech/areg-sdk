## 21_pubunblock Project Overview

[21_pubunblock](https://github.com/aregtech/areg-sdk/tree/master/examples/21_pubunblock) is a project that demonstrates the manual unblocking of requests in a service. When a request is being processed and no response has been sent, it is marked as *busy*. If another client sends the same request, they will receive a *failure* response indicating that the request is busy. However, it is possible to manually unblock the request, allowing the service provider to process more requests. Once the service provider is ready to send the response to a specific consumer, it prepares and sends the response to the intended target.

The project consists of three sub-projects:

1. [generated](https://github.com/aregtech/areg-sdk/tree/master/examples/21_pubunblock/generated) (21_generated): This static library contains the generated codes and common objects. It is generated from the [LargeData.siml](https://github.com/aregtech/areg-sdk/blob/master/examples/20_pubdatarate/res/LargeData.siml) Service Interface document using the `codegen.jar` tool.

2. [pubservice](https://github.com/aregtech/areg-sdk/tree/master/examples/21_pubunblock/pubservice) (21_pubservice): This sub-project provides a Public Service that receives requests, unblocks them, and replies with the response after a certain timeout. Each request is replied to the specific consumer with the appropriate response. The service includes generated sequence numbers in the responses to validate the process.

3. [pubclient](https://github.com/aregtech/areg-sdk/tree/master/examples/21_pubunblock/pubclient) (21_pubclient): This sub-project contains a Service Client that triggers requests at a higher frequency than the Service Provider can process. As a result, after a very short period of time, the Service Provider accumulates requests that are marked as *pending*.

Communication within the system is facilitated by `mcrouter`, a multicast router that operates on any machine within the network. The AREG SDK automates service discovery and ensures a fault-tolerant system, making the order of process startup irrelevant.
