# Documentation Overview

```
This file is part of Areg SDK
Copyright (c) Aregtech (Artak Avetyan), 2021-2026
Contact: info[at]areg.tech
Website: https://www.areg.tech
```

This document provides an overview of the Areg SDK file structure and documentation locations.

## File Structure

```
areg-sdk/                Root directory of Areg SDK
├── conf/                Build configuration files (CMake, MSBuild, MSVC)
├── docs/                Documentation files
│   ├── img/             Images and screenshots
│   ├── templates/       MSVC project templates
│   └── wiki/            Detailed wiki pages
├── examples/            Working examples demonstrating framework features
├── framework/           Source code
│   ├── areg/            Core communication engine library
│   ├── aregextend/      Extended features library (optional dependencies)
│   ├── areglogger/      Log observer library for receiving logs
│   ├── logcollector/    Log collection service (console or system service)
│   ├── logobserver/     Real-time log monitoring console application
│   └── mtrouter/        Multitarget Router for IPC (console or system service)
├── tests/               Unit tests
├── thirdparty/          External dependencies
├── tools/               Code generator and project scaffolding
├── LICENSE.txt          License file
└── README.md            Project overview
```

Classes in the `framework/areg` subdirectories are available for application development. However, classes in `private` subdirectories are internal and should not be used directly.

After building with CMake or MSBuild, compiled binaries are placed in the `product` subdirectory.

> [!TIP]
> Visit the [Areg SDK Wiki](./wiki/README.md) for detailed documentation. Contributions to improve the wiki are welcome.

## Documentation

| Document | Description |
|----------|-------------|
| [Developer Guide](./DEVELOP.md) | Technical guidance for developing distributed, service-enabled applications |
| [How-To Guide](./HOWTO.md) | Instructions for building, configuring, and integrating projects |
| [Use Cases](./USECASES.md) | Real-world scenarios demonstrating Areg SDK capabilities |
| [Service Interface](./ServiceInterface.md) | Service interface structure and XML format |
| [Hello Service Tutorial](./HelloService.md) | Step-by-step tutorial for creating your first service |
| [POSIX Reference](./POSIX.md) | POSIX API configuration and function list |
| [Win32 Reference](./WIN32.md) | Windows API configuration and function list |
| [Wiki](./wiki/README.md) | Comprehensive documentation and guides |
