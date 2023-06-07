## 17_winchat Project Overview

**NOTE:** The 17_winchat project is designed specifically for Windows operating systems and compiled only with the Microsoft Visual C++ compiler.

[17_winchat](https://github.com/aregtech/areg-sdk/tree/master/examples/17_winchat) is a Windows GUI application that showcases a fault-tolerant and distributed computing system for embedded devices. It demonstrates the dynamic creation and definition of models and utilizes a dynamic subscription service during runtime.

The project consists of three sub-projects:

1. [generated](https://github.com/aregtech/areg-sdk/tree/master/examples/17_winchat/generated) (17_generated): This static library contains generated codes and common objects. It is generated from multiple Service Interface documents located in the [`res`](https://github.com/aregtech/areg-sdk/tree/master/examples/17_winchat/res) subfolder using the `codegen.jar` tool.

2. [register](https://github.com/aregtech/areg-sdk/tree/master/examples/17_winchat/register) (17_register): This sub-project instantiates a Public Service that is utilized by all other applications. Upon starting the application, the user is prompted to manually enter the IP address of the multicast router (`mcrouter`) for establishing a connection.

3. [chatter](https://github.com/aregtech/areg-sdk/tree/master/examples/17_winchat/chatter) (17_chatter): The chatter sub-project contains an instance of Public Services and Service Consumers. It dynamically creates and loads models during runtime. Similar to the `register` sub-project, the user needs to manually enter the IP address of the multicast router (`mcrouter`) upon application startup to establish a connection.

Communication within the system is facilitated by `mcrouter`, a multicast router that operates on any machine within the network. The AREG SDK automates service discovery and ensures a fault-tolerant system, making the order of process startup irrelevant.
