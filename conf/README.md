# Configuration Overview

This document provides a brief overview of the configuration directory's contents and instructions for setting up configurations for various toolsets.

## CMake

The [./cmake](./cmake/) directory contains project-wide and developer-specific settings for building with CMake. Direct modifications to files within this directory are generally unnecessary. Instead, use the options in [user.cmake](./cmake/user.cmake) to customize the build configuration.

## Microsoft Visual Studio

The [./msvc](./msvc/) directory includes Visual Studio-specific property files referenced by the project. Directly modifying `.props` files is not recommended. To customize your build, copy the [msvc_setup.props](./../msvc_setup.props) file into your solution directory and apply necessary changes there.

## Exports

The [./exports](./exports) directory contains list of files that are required by packaging, required to install and uninstall OS-manages services and examples of creating toolchain files.

For further information, consult the [wiki](./../docs/wiki/README.md) documentation.
