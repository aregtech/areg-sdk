# Start a new project

An AREG application lives outside the SDK repository and pulls the SDK in through
CMake. The fastest correct start is the setup script:

```bash
python3 <areg-sdk>/tools/setup_project.py --name myapp --root ~/myapp --mode local
cd ~/myapp
cmake -B build
cmake --build build -j
./build/bin/myapp.elf
```

On Windows the same five commands are `python` instead of `python3` and
`build\bin\myapp.exe` instead of the last line. Needed before any of it: CMake 3.20+,
a Java 17+ runtime and a C++17 compiler. Without Python, copy a recipe from
`recipes/` by hand instead; they are ordinary project files.

It copies one of the recipes in `recipes/`, renames it, and writes the new
project's own AGENTS.md and .gitignore.

| Option | Meaning |
|---|---|
| `--mode local` | One process; provider and consumer in two threads |
| `--mode ipc` | Two processes; needs `mtrouter` and writes a `run.sh` |
| `--mode pubsub` | Attributes and broadcasts in one process |
| `--sdk-root <path>` | Build against a local SDK copy instead of fetching from GitHub |
| `--tag <tag>` | The SDK tag to fetch |
| `--force` | Overwrite an existing directory |

The rest of this page describes what it writes, for when a project has to be built
by hand or an existing one has to be changed.

---

## 1. Project layout

```
<project-root>/
  CMakeLists.txt            top level: finds or fetches areg
  src/
    CMakeLists.txt          declares the service interface and the executables
    services/
      HelloService.siml     the service contract
    provider.cpp            the providing component and its model
    consumer.cpp            the consuming component and its model
```

For a single process application, use one `main.cpp` instead of `provider.cpp` and
`consumer.cpp`, holding both components and one model.

---

## 2. Top level `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME    "myproject")
set(PROJECT_VERSION "1.0.0")
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

find_package(areg CONFIG)

if (NOT areg_FOUND)
    set(AREG_BUILD_DIR "${CMAKE_BINARY_DIR}")
    set(AREG_DEPS_DIR  "${CMAKE_BINARY_DIR}/packages")
    set(AREG_LIB_TYPE  shared)
    option(AREG_TESTS         "Build areg-sdk tests"    OFF)
    option(AREG_EXAMPLES      "Build areg-sdk examples" OFF)
    option(AREG_SYSTEM_GTEST  "Build GTest"             OFF)
    option(AREG_OUTPUT_LAYOUT "Areg build structure"    OFF)

    include(FetchContent)
    set(FETCHCONTENT_BASE_DIR "${AREG_DEPS_DIR}")
    FetchContent_Declare(areg
        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
        GIT_TAG "master")
    FetchContent_MakeAvailable(areg)

    set(AREG_SDK_ROOT         "${areg_SOURCE_DIR}")
    set(AREG_CMAKE_CONFIG_DIR "${AREG_SDK_ROOT}/conf/cmake")
    set(AREG_CMAKE            "${AREG_SDK_ROOT}/areg.cmake")
endif()

include(${AREG_CMAKE})

add_subdirectory(src)
```

Turning the SDK's own examples and tests off matters: it keeps the configure step
offline and fast.

---

## 3. `src/CMakeLists.txt`

Two kinds of line, and nothing else.

```cmake
addServiceInterface(gen_myproject src/services/HelloService.siml)

macro_declare_executable(myproject_provider gen_myproject provider.cpp)
macro_declare_executable(myproject_consumer gen_myproject consumer.cpp)
```

| Function | What it does |
|---|---|
| `addServiceInterface(<target> <path.siml>)` | Runs the generator at configure time and builds the generated code into a static library named `<target>`. |
| `macro_declare_executable(<name> <sources, targets, resources...>)` | Declares an executable. Everything after the name is sorted automatically into source files, libraries to link and resources, in any order. Name the generated target here to link it. |

You never call the generator by hand and never add generated files to a source list.

---

## 4. The remaining files

Write them from the task pages, in this order:

0. what the services are, if not yet decided - see `05-design.md`
1. `src/services/HelloService.siml` - see `20-service-interface.md`
2. the providing component - see `30-provider.md`
3. the consuming component - see `31-consumer.md`
4. the model and `main` in each executable - see `32-model.md`

---

## 5. Build and run

```bash
cmake -B ./build
cmake --build ./build -j
```

Binaries are written to `<project-root>/build/bin/`, with a platform suffix: `.elf`
on Linux, `.mac` on macOS, `.exe` on Windows. The framework libraries and `mtrouter`
are placed there too, so a multi process project needs nothing installed.

**Single process:** run the executable.

**Several processes:** start `mtrouter` first, then the provider, then the consumer.
It is built into your own `build/bin/`. Consumers may start before providers; they
wait and connect.

```bash
./build/bin/mtrouter.elf --service &      # POSIX: background it
```

```bat
start "" build\bin\mtrouter.exe --service
```

---

## 6. Before you move on

- [ ] `find_package` / FetchContent block is present and examples and tests are off.
- [ ] Every `.siml` has an `addServiceInterface` line.
- [ ] Every executable is declared with `macro_declare_executable` and names its
      generated target.
- [ ] No generated file is listed as a source and none was edited.
- [ ] The build produced the expected binaries.
