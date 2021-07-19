========================================================================
    CONSOLE APPLICATION : 09_duplicate Project Overview
========================================================================

The project 09_duplicate is an extension of project 08_service.
This project demonstrates how to instantiate multiple times the same servicing component.
To instantiate same servicing component more than one time, they should run in separate threads and should have different names.
The application is using same servicing objecs that have different names.
The application uses dummy (empty) service interface and has no servicing client.
Every service is using periodic timer that is triggered to make output on console.
The threads and servicing components are described in the model "Test_MixedService".
The application can have multiple models that are either loaded dynamically or all.
See implementationof "08_service" project to compare the logic.
