# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Areg SDK is a C++ framework for building distributed, service-oriented systems with automated message routing and IPC. It uses Object RPC (ORPC) where services are stateful objects with methods, attributes (pub-sub), and events. The same code runs identically in single-threaded, multi-threaded, multi-process, and multi-device scenarios.

## Build Commands

```bash
# Configure (CMake 3.20+ required)
cmake -B build

# Build with parallel jobs
cmake --build build -j20

# Build with options
cmake -B build -DAREG_BUILD_TESTS=ON -DAREG_BUILD_EXAMPLES=ON -DAREG_BINARY=shared

# Windows MSVC alternative
MSBuild areg-sdk.sln /p:Configuration=Release /p:Platform=x64

# Install (requires sudo on Linux/macOS)
sudo cmake --install build
```

## Running Tests

```bash
# Build with tests enabled
cmake -B build -DAREG_BUILD_TESTS=ON
cmake --build build -j20

# Run tests
ctest --test-dir build

# Or run directly (path varies by platform/compiler)
./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/areg-unit-tests
```

## Key CMake Options

| Option | Default | Purpose |
|--------|---------|---------|
| `AREG_COMPILER_FAMILY` | auto | gnu, llvm, msvc |
| `AREG_BINARY` | shared | shared or static library |
| `AREG_BUILD_TYPE` | Release | Release or Debug |
| `AREG_BUILD_TESTS` | ON | Enable unit tests |
| `AREG_BUILD_EXAMPLES` | ON | Enable examples |
| `AREG_LOGS` | ON | Enable logging |

Full options documented in `docs/wiki/02d-cmake-config.md`.

## Code Generation

Service interfaces are defined in `.siml` files and code is generated:

```bash
# Linux/macOS
./tools/codegenerate.sh <input.siml> <output_dir>

# Windows
tools\codegenerate.bat <input.siml> <output_dir>
```

Requires Java 17+.

## Project Setup Tool

Create new projects with scaffolding:

```bash
./tools/setup-project.sh   # Linux/macOS
tools\setup-project.bat    # Windows
```

## Architecture

### Directory Structure

- `framework/areg/` - Core framework library
  - `appbase/` - Application lifecycle and model loading
  - `base/` - Utilities (String, Buffer, threading primitives)
  - `component/` - Component and thread management
  - `ipc/` - Socket communication and serialization
  - `logging/` - Scoped logging framework
  - `persist/` - Key-value configuration storage
  - `system/` - OS abstraction layer
- `framework/aregextend/` - Extended library (console UI, service base classes)
- `framework/areglogger/` - Log observer API library
- `framework/mtrouter/` - Message router for IPC (required for multiprocessing)
- `framework/logcollector/` - Log aggregation service
- `framework/logobserver/` - Real-time log monitoring
- `conf/cmake/` - Build system configuration
- `conf/exports/` - Installation templates (pkg-config, service files, CMake config)
- `examples/` - 29 example projects (01-29)
- `tests/units/` - Google Test unit tests
- `tools/` - Code generator and project scaffolding

### Build Configuration Flow

```
CMakeLists.txt → conf/cmake/setup.cmake → conf/cmake/user.cmake
→ conf/cmake/common.cmake → conf/cmake/{gnu,clang,msvc}.cmake
```

### Output Structure

Build artifacts go to `product/build/[compiler]/[platform]-[bitness]-[arch]-[config]-[libtype]/`

Examples:
- Linux: `product/build/gnu-g++/linux-64-x86_64-release-shared/bin/`
- macOS: `product/build/llvm-clang++/darwin-64-arm64-release-shared/bin/`
- Windows: `product/build/msvc-v143/windows_nt-x64-release/bin/`

## Platform Notes

### Platform-Specific Details

| Platform | Compiler | Executable Suffix | Shared Library | Service Manager |
|----------|----------|-------------------|----------------|-----------------|
| Linux | GCC/Clang | `.elf` | `.so` | systemd |
| macOS | Apple Clang | `.mac` | `.dylib` | launchd |
| Windows | MSVC | `.exe` | `.dll` | Windows Services |

### macOS-Specific Notes

- Uses `libc++` (not `libstdc++`) - link with `-lc++`
- Apple SDK uses `.tbd` stub libraries (text-based definition files)
- `cc`/`c++` are symlinks to Clang (not GCC)
- Timers use `dispatch_source` instead of POSIX `timer_create`
- Service files use `.plist` format (XML) in `/Library/LaunchDaemons/`
- Service naming convention: `tech.areg.<service>` (reverse domain for areg.tech)

### Key CMake Files for Platform Support

| File | Purpose |
|------|---------|
| `conf/cmake/common.cmake` | Platform detection, executable suffixes |
| `conf/cmake/clang.cmake` | Clang/Apple Clang compiler settings |
| `conf/cmake/functions.cmake` | Utility macros, library compatibility checks |
| `conf/cmake/install.cmake` | Installation rules (platform-specific service files) |

## System Services

### Service Files Location

| Platform | Template Files | Installed Location |
|----------|----------------|-------------------|
| Linux | `conf/exports/*.service.in` | `/usr/local/share/areg/service/` |
| macOS | `conf/exports/tech.areg.*.plist.in` | `/usr/local/share/areg/service/` |
| Windows | `conf/exports/*.service.*.bat.in` | `share/areg/service/` |

### Service Management Commands

**Linux (systemd):**
```bash
sudo systemctl enable/start/stop/status mtrouter.service
```

**macOS (launchd):**
```bash
sudo launchctl load/unload -w /Library/LaunchDaemons/tech.areg.mtrouter.plist
sudo launchctl list | grep tech.areg
```

**Windows:**
```powershell
mtrouter.exe --install/--uninstall
Start-Service/Stop-Service "mtrouter.service"
```

## Examples

Examples are numbered 01-29, progressing from simple to complex:
- **01-14**: Multithreading (single process, no mtrouter needed)
- **15-27**: Multiprocessing/IPC (requires mtrouter running)

Run examples from `product/build/[compiler]/[platform]/bin/NN_examplename`

## Dependencies

Third-party libraries are automatically fetched via CMake FetchContent:
- **SQLite 3** - Persistence (or use system: `AREG_SQLITE_PACKAGE=ON`)
- **Google Test** - Testing (or use system: `AREG_GTEST_PACKAGE=ON`)

## Coding Rules (MANDATORY)

**Before writing or modifying any C++ code, read and follow `docs/AREG_AI_CODING_RULES.md`.** That file is the authoritative coding standard for all AI-generated code. Key rules summarized here — the full document has details, examples, and a reference implementation.

- **Naming**: Types = `PascalCase`, functions/methods = `snake_case`, members = `mPascalCase`, constants = `UPPER_SNAKE_CASE`
- **Accessors**: `count()` not `get_count()`, `is_valid()` for booleans, `set_name()` for mutators
- **Types**: `std::string_view` for string params (not `const char*`), fixed-width integers (`uint32_t` not `unsigned int`), `nullptr` not `NULL`
- **Layout**: Allman braces, 4 spaces (no tabs), leading commas in lists, 120 char preferred / 180 max line length
- **Headers**: Traditional include guards (`AREG_<PATH>_<FILE>_HPP`), never `#pragma once`
- **Classes**: Constructor initializer lists (no in-class member initializers), `virtual` + `override` together in declarations
- **Errors**: No exceptions — use `bool`, `std::optional`, or error codes
- **Macros**: Prefix with `AREG_`, prefer `constexpr`/`inline`/templates over macros, no `IN`/`OUT`/`INOUT` empty macros

**Full rules with examples:** `docs/AREG_AI_CODING_RULES.md`
**Style guide with rationale:** `docs/AREG_CODING_STYLE.md`

## Code Patterns and Conventions

### Platform-Specific Code Organization

Platform-specific implementations follow this pattern:
```
framework/<module>/
├── <Component>.hpp           # Public header
├── private/
│   ├── <Component>.cpp       # Common implementation
│   ├── posix/
│   │   └── <Component>Posix.cpp  # Linux + macOS
│   └── win32/
│       └── <Component>Win32.cpp  # Windows
```

### Preprocessor Macros for Platform Detection

| Macro | Platform |
|-------|----------|
| `_WIN32`, `WINDOWS` | Windows |
| `_POSIX`, `POSIX` | Linux and macOS |
| `__APPLE__` | macOS only |
| `__linux__` | Linux only |

### Adding New Platform Support (e.g., Zephyr RTOS)

1. **CMake configuration**: Add to `conf/cmake/setup.cmake` and create `conf/cmake/zephyr.cmake`
2. **Platform detection**: Update `conf/cmake/common.cmake` with new platform variables
3. **OS abstraction**: Implement platform-specific files in `framework/areg/base/private/zephyr/`
4. **Threading**: Implement in `framework/areg/base/private/zephyr/` (Thread, Mutex, Semaphore)
5. **Timers**: Implement in `framework/areg/component/private/zephyr/` (TimerPosix-like)
6. **Sockets**: Implement in `framework/areg/ipc/private/zephyr/` if network support needed
7. **Service support**: Add service management if applicable

### Key Abstraction Layers

| Layer | Location | Purpose |
|-------|----------|---------|
| Threading | `framework/areg/base/` | Thread, Mutex, SynchEvent, SpinLock |
| Timers | `framework/areg/component/private/posix/TimerPosix.*` | Timer callbacks |
| Sockets | `framework/areg/ipc/private/` | Network communication |
| Console | `framework/aregextend/console/` | Terminal I/O |
| Services | `framework/aregextend/service/` | System service base classes |

## Debugging Tips

### Common Build Issues

1. **Duplicate library warnings**: Check `PRIVATE` vs `PUBLIC` linking in CMakeLists.txt
2. **Timer not triggering**: Check callback registration in `TimerPosix.cpp`
3. **Library compatibility**: Use `macro_check_module_architect` in CMake
4. **Compiler detection**: `cc`/`c++` may be GCC or Clang - check `--version` output

### Finding Platform-Specific Bugs

1. Check `#ifdef` blocks for missing platform cases
2. Look for `_POSIX` code that assumes Linux-specific APIs (e.g., `timer_create`)
3. Verify callback signatures match platform expectations
4. Test service mode with `--service` flag on target platform

## Documentation

- Wiki: `docs/wiki/` - Comprehensive guides
- Code comments: Doxygen-style in headers
- READMEs: In each major directory

When updating documentation, ensure all three platforms (Linux, macOS, Windows) are covered.
