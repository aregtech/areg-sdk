# Content

AREG engine source codes. The content of this folder:

* [appbase](./appbase)      -- The base class to create applications. Mainly contain the API to call to initialize the services and load models.
* [base](./base)            -- The collection of base helper objects.
* [component](./component)  -- The business logic of the engine to create service components and provide automations.
* [ipc](./ipc)              -- The objects to provide IPC (inter-process communication).
* [persist](./persist)      -- The objects to use to persist data in the files. Saves key and value pairs.
* [resources](./resources)  -- The folder with resources for AREG engine.
* [logging](./logging)      -- The logging service of AREG engine.

The objects may differ by POSIX and Win32 API, but the behavior of objects for all platforms are the same. The header files declared in `private` subdirectory are considered only for internal use and project should not include them in their files.
