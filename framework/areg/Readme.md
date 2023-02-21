# Content

AREG communicaiton engine source codes. The content of this folder:

* [appbase](./appbase)      -- The base class to create applications. Mainly contain the API to call to initialize the services and load models.
* [base](./base)            -- The collection of base helper objects.
* [component](./component)  -- The business logic of the engine to create service components and provide automations.
* [ipc](./ipc)              -- The objects to provide IPC (inter-process commonication).
* [persist](./persist)      -- The objects to use to persist data in the files. Saves key and value pairs.
* [resurces](./resources)   -- The folder with enresources for AREG engine.
* [trace](./trace)          -- The logging / traces service inside AREG engine.

The objects may differ by POSIX and Win32 API, but the behaviour of objects for all platforms are the same. The header files declared in `private` subfolders are considered only for internal use and project should not include them in their files.
