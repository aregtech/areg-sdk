# 08_service Project Overview

The [08_service](https://github.com/aregtech/areg-sdk/tree/master/examples/08_service) project provides a demonstration of using simple local *Service Provider* objects in **multithreaded** applications. It involves instantiating a *dummy* (empty) *Local Service Interface* object that is only accessible within the process and not visible externally.

The service is registered within a *model* that is loaded at the start of the application and unloaded upon completion of the job.