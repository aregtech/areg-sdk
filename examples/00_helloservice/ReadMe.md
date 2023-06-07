# 00_helloservice Project Overview

The [00_helloservice](https://github.com/aregtech/areg-sdk/tree/master/examples/00_helloservice) project is an example included in the developer guide. It showcases a console **IPC** and **multithreading** application and demonstrates the creation of service and client objects along with various deployment scenarios using the framework.

The project consists of the following major sub-projects and applications:

## generated (00_generated)

The [00_generated](https://github.com/aregtech/areg-sdk/tree/master/examples/00_helloservice/generated) sub-project is a static library that contains the generated code based on the [HelloService.siml](https://github.com/aregtech/areg-sdk/blob/master/examples/00_helloservice/res/HelloService.siml) Service Interface document located in the [res](https://github.com/aregtech/areg-sdk/blob/master/examples/00_helloservice/res) sub-folder. The code is generated using a code generator tool and compiled as a static library.

To recover or update the generated sources, run `codegen.jar` from the `00_helloservice` folder using the following command:
```
java -jar ./../../tools/codegen.jar --root=./ --doc=res/HelloService.siml --target=generated/src
```

## common

The [common](https://github.com/aregtech/areg-sdk/tree/master/examples/00_helloservice/common/src) folder contains shared objects used across all other projects. It demonstrates the ease of changing the architecture of applications without requiring changes to the service providers and consumers.

## onethread (00_onethread)

The [00_onethread](https://github.com/aregtech/areg-sdk/tree/master/examples/00_helloservice/onethread) application demonstrates a scenario where the service provider and client coexist within a single thread of the same process. The implementation of the service provider and consumer is located in the [common/src](https://github.com/aregtech/areg-sdk/tree/master/examples/00_helloservice/common/src) folder.

## twothreads (00_twothreads)

The [00_twothreads](https://github.com/aregtech/areg-sdk/tree/master/examples/00_helloservice/twothreads) application showcases a scenario where the service provider and client operate in two separate threads within the same process. The implementation of the service provider and consumer is located in the [common/src](https://github.com/aregtech/areg-sdk/tree/master/examples/00_helloservice/common/src) folder.

## multiprocess

The [multiprocess](https://github.com/aregtech/areg-sdk/tree/master/examples/00_helloservice/multiprocess) project consists of two sub-projects that communicate via IPC using Object RPC (ORPC).

### serviceproc (00_pubservice)

The [00_pubservice](https://github.com/aregtech/areg-sdk/tree/master/examples/00_helloservice/multiprocess/serviceproc) application provides a Public Service that allows service consumers (clients) to make requests. The implementation of the service provider and consumer is located in the [common/src](https://github.com/aregtech/areg-sdk/tree/master/examples/00_helloservice/common/src) folder.

### clientproc (00_pubclient)

The [00_pubclient](https://github.com/aregtech/areg-sdk/tree/master/examples/00_helloservice/multiprocess/clientproc) application contains a client object for the Public Service. Multiple instances of `00_clientproc` can be instantiated, enabling multiple service consumer objects to make requests on the service provider process. The implementation of the service provider and consumer is located in the [common/src](https://github.com/aregtech/areg-sdk/tree/master/examples/00_helloservice/common/src) folder.

Please note that this project provides a comprehensive demonstration of different deployment scenarios and the usage of service and client objects.