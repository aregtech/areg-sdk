## 20_pubdatarate Project Overview

[20_pubdatarate](https://github.com/aregtech/areg-sdk/tree/master/examples/20_pubdatarate) is a project that demonstrates network communication data rate. The service generates a simple gray bitmap image based on user options and delivers it to connected Service Consumers. Both the Service Provider and the Service Consumer applications output the network communication data rate information on the console, allowing developers to measure the most optimal data rate for the application.

The project consists of three sub-projects:

1. [generated](https://github.com/aregtech/areg-sdk/tree/master/examples/20_pubdatarate/generated) (20_generated): This static library contains the generated codes and common objects. It is generated from the [LargeData.siml](https://github.com/aregtech/areg-sdk/blob/master/examples/20_pubdatarate/res/LargeData.siml) Service Interface document using the `codegen.jar` tool. The library also includes common objects for generating the gray image and calculating data rates used by the Service Provider and Service Consumer applications.

2. [pubservice](https://github.com/aregtech/areg-sdk/tree/master/examples/20_pubdatarate/pubservice) (20_pubservice): This sub-project provides a Public Service that generates data and sends it to connected consumers. The Service Provider is responsible for generating the gray bitmap image based on user options.

3. [pubclient](https://github.com/aregtech/areg-sdk/tree/master/examples/20_pubdatarate/pubclient) (20_pubclient): This sub-project contains a Service Consumer that receives the image data sent by the Service Provider. The Service Consumer is responsible for receiving and processing the image data.

Communication within the system is facilitated by `mcrouter`, a multicast router that operates on any machine within the network. The AREG SDK automates service discovery and ensures a fault-tolerant system, making the order of process startup irrelevant.
