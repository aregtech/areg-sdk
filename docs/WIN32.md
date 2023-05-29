```
This file is part of AREG SDK
Copyright (c) Aregtech, 2017-2022
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

# Win32 API

In order to build the AREG framework with the *Win32 API*, it is necessary to define the preprocessor symbol **WINDOWS**. Detailed information regarding the `WINDOWS` preprocessor symbol can be found in the [Preprocessor define symbols](https://github.com/aregtech/areg-sdk/wiki/02.-Preprocessor-define-symbols#windows-define) Wiki page of the AREG SDK. To utilize the Win32 API in the software development process, please consult the [Software build](https://github.com/aregtech/areg-sdk/wiki/03.-Software-build) Wiki page of the AREG SDK.

For proper functioning, the AREG SDK relies on the Windows Socket and Win32 API for networking, multithreading and synchronization. In addition to the standard runtime library, if the `AREG_EXTENDED` option is enabled during compilation, it may require minimum version Windows 2000 desktop apps. To illustrate, please consider the following example command sequence:

```batch
msbuild /m /property:Configuration=Debug /property:Platform=x64 /property:AregExtended=1 ./areg-sdk.sln
```

By executing the above commands, the AREG framework will be built, taking into account the extended functionality, which is available for Windows 2000 and higher.

**The list of POSIX API functions and macro used in AREG SDK (including multicast message router):**

```
TODO: Add Win32 API list here.
```
