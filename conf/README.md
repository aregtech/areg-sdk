# Configurations
This document is a short introduction of folder content and configuration for different toolsets.

# Makefiles
The folder [./make](./make/) contains projects common and developer specific settings to change. Normally, it is expected to make custom changes only in [./make/user.mk](./make/user.mk) file.

# Microsoft Visual Studio
The folder [./msvc](./msvc/) contains Microsoft Visual Studio specific property files that are included in projects. Normally, developers need to change only [./msvc/user.props](./msvc/user.props), whihc is considered to set custom settings.
