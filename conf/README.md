# Configurations
This document is a short introduction of folder content and configuration for different toolsets.

## CMakeLists
The folder [./cmake](./cmkae/) contains project common and developer specific settings. Normally, it is expected to make custom changes only in the [user.cmake](./cmake/user.cmake) file. The content of the folder has impact when build project with `cmake` tool.

## Microsoft Visual Studio
The folder [./msvc](./msvc/) contains Microsoft Visual Studio specific property files that are included in projects. Normally, it is expected to make custom changes only in the [./msvc/user.props](./msvc/user.props) file. The content of the foler has impact with build project with `MSBuild` build.
