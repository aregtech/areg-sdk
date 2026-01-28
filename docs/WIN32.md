```
This file is part of Areg SDK
Copyright (c) Aregtech, 2017-2026
Contact: info[at]areg.tech
Website: https://www.areg.tech
```

# Win32 API

To build the Areg framework with the *Win32 API*, it is necessary to define the preprocessor symbol **WINDOWS**. Detailed information about the `WINDOWS` preprocessor symbol can be found in the [Preprocessor define symbols](./wiki/02f-preprocessor-definitions.md) page of the Areg SDK Wiki. To utilize the Win32 API in your software development process, please refer to the [Building Areg SDK with CMake](./wiki/01b-cmake-build.md) or [Building the Areg SDK with Microsoft Visual Studio and MSBuild](./wiki/01c-msvc-build.md) page of the Areg SDK Wiki.

The Areg SDK relies on the Windows Socket and Win32 API for networking, multithreading, and synchronization. Along with the standard runtime library, if the `AREG_EXTENDED` option is enabled during compilation, it may require a minimum version of Windows 2000 for desktop apps. To demonstrate, consider the following example command:

```batch
msbuild /m /property:Configuration=Debug /property:Platform=x64 /property:AregExtended=1 ./areg-sdk.sln
```

By executing the above command, the Areg framework will be built, taking into account the extended functionality that is available for Windows 2000 and higher.

**The list of Win32 API functions and macro used in Areg SDK (including Multitarget message router):**

```
TODO: Add Win32 API list here.
```
