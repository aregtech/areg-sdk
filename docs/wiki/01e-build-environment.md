# Setting Up the Build Environment

Areg needs **no third-party libraries**: SQLite ships in the repository, and the unit tests
fetch GoogleTest themselves. What a machine needs are the **build tools** below. This page
installs them from scratch on Linux, macOS and Windows, and shows how to check each one.

## The Setup Script

One command installs everything from a menu:

```bash
sh tools/setup-environment.sh        # Linux (apt, dnf, pacman) and macOS (Homebrew)
```

```powershell
tools\setup-environment.bat          # Windows (winget)
```

The menu preselects the required tools; the optional ones (Clang, Python, ncurses, cross
compilers) are a number away. The script prints the exact commands, asks before it runs
them, and checks the result with `tools/check-env.sh` (`.bat`). `--dry-run` prints the
commands only; `--with` and `--all` select items without the menu, and `--yes` runs them
without asking.

The script is part of the SDK. Without Git yet, download the SDK as a ZIP from
[GitHub](https://github.com/aregtech/areg-sdk). The sections below do the same by hand.

---

## Table of Contents

1. [The Setup Script](#the-setup-script)
2. [What You Need](#what-you-need)
3. [Linux](#linux)
4. [macOS](#macos)
5. [Windows](#windows)
6. [32-bit and ARM Targets](#32-bit-and-arm-targets)
7. [Check the Setup](#check-the-setup)
8. [Next Steps](#next-steps)

---

## What You Need

| Tool | Minimum | Needed for |
|---|---|---|
| **C++ compiler** | C++17: GCC, Clang, or MSVC from Visual Studio 2019 | everything; CI tests the compilers of the current GitHub runners |
| **CMake** | 3.20 | configuring and building (on Windows, the Visual Studio solution is an alternative) |
| **Java runtime** | 17 | the code generator, at build time only; nothing Java reaches your binaries |
| **Git** | any recent | cloning Areg, and CMake fetching it into your project |
| **Python** | 3.10 | agentic coding only; never needed to generate, build or run |

**Optional:** `ncurses` development files, only when you build with `AREG_EXTENDED=ON`
(Linux, macOS, Cygwin). It is off by default.

A Java **runtime** (JRE) is enough; a full JDK works too. CMake finds Java on the `PATH`
or through `JAVA_HOME`.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Linux

### Ubuntu and Debian

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git openjdk-17-jre-headless python3
```

`build-essential` brings GCC and `make`. For Clang as well: `sudo apt-get install -y clang`.

> [!NOTE]
> Ubuntu 20.04 ships CMake 3.16, which is too old. Install a current one with
> `sudo snap install cmake --classic` or `pip install cmake`, or use the
> [Kitware APT repository](https://apt.kitware.com/).

### Fedora, RHEL, Rocky, AlmaLinux

```bash
sudo dnf install -y gcc-c++ make cmake git java-21-openjdk-headless python3
```

Any Java from 17 up works; take the version your release offers.

### Arch Linux

```bash
sudo pacman -S --needed base-devel cmake git jre-openjdk-headless python
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## macOS

1. **Compiler:** install the Xcode Command Line Tools, which bring Apple Clang and `make`:

   ```bash
   xcode-select --install
   ```

2. **The rest,** through [Homebrew](https://brew.sh/):

   ```bash
   brew install cmake git openjdk@17 python
   ```

3. **Make Java visible.** Homebrew's `openjdk@17` is not on the `PATH` by itself. Link it
   once into the system Java folder:

   ```bash
   sudo ln -sfn "$(brew --prefix)/opt/openjdk@17/libexec/openjdk.jdk" \
                /Library/Java/JavaVirtualMachines/openjdk-17.jdk
   ```

   `java -version` must now print 17 or newer.

Apple Silicon and Intel Macs are both supported.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Windows

### Visual Studio (MSVC and clang-cl)

1. Install [Visual Studio 2019 or newer](https://visualstudio.microsoft.com/); the free
   Community edition is enough. In the installer, select the workload
   **Desktop development with C++**. It includes MSVC and **C++ CMake tools for Windows**.
   For `clang-cl`, also select **C++ Clang tools for Windows**.
2. Install Java, Git and Python, from a terminal with
   [winget](https://learn.microsoft.com/windows/package-manager/winget/):

   ```powershell
   winget install EclipseAdoptium.Temurin.17.JRE
   winget install Git.Git
   winget install Python.Python.3.12
   ```

   Or download them: [Temurin 17](https://adoptium.net/), [Git](https://git-scm.com/download/win),
   [Python](https://www.python.org/downloads/).
3. Build from the **Developer PowerShell for VS** (or the **x64 Native Tools Command Prompt**),
   which puts the compiler on the `PATH`. Visual Studio itself can also open the cloned
   folder (**File → Open → Folder**) or `areg-sdk.sln` directly.

A standalone CMake is optional: `winget install Kitware.CMake`.

### Cygwin

Install with the [Cygwin installer](https://cygwin.com/install.html), or from the command line:

```powershell
c:\cygwin\setup.exe -qgnO -s http://mirrors.kernel.org/sourceware/cygwin/ -l C:\cygwin\ -P cmake,make,gcc-g++,git,python3,libncurses-devel
```

Java comes from Windows, as above.

### MinGW

Areg also builds with MinGW-w64, for example from [MSYS2](https://www.msys2.org/):

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake git
```

Java comes from Windows, as above. MSYS2 does not inherit the Windows `PATH` by default:
start its shell with `MSYS2_PATH_TYPE=inherit` so that it finds `java`.

### WSL

To build the Linux version on Windows, see [Building in WSL](./01d-wsl-build.md); inside
WSL, follow the [Linux](#linux) steps.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 32-bit and ARM Targets

`AREG_ARCH` selects the target CPU. Each target needs its compiler on the host:

| Target | Host packages (Ubuntu and Debian) | CMake option |
|---|---|---|
| Linux x86 (32-bit) | `g++-15-multilib` (the version of your `g++`) | `-DAREG_ARCH=x86` |
| Linux ARMv7 (32-bit) | `g++-arm-linux-gnueabihf` | `-DAREG_ARCH=arm` |
| Linux ARM64 | `g++-aarch64-linux-gnu` | `-DAREG_ARCH=aarch64` |

```bash
cmake -B ./build -DAREG_ARCH=x86 -DAREG_COMPILER_FAMILY=gnu
cmake --build ./build -j
```

> [!NOTE]
> The `gcc-multilib` and `g++-multilib` packages conflict with the ARM cross compilers; the
> versioned `g++-15-multilib` does not. It leaves `<asm/...>` unlinked, so link it once:
> `sudo ln -s /usr/include/x86_64-linux-gnu/asm /usr/include/asm`. For `AREG_EXTENDED=ON`,
> also install `libncurses-dev:i386` after `sudo dpkg --add-architecture i386`.

On Windows, build the `Win32` platform of `areg-sdk.sln` in Visual Studio. Toolchain files
for every target are also in `conf/toolchains/`.

Details and the Clang variants: [Cross-Compilation](./01b-cmake-build.md#cross-compilation).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Check the Setup

Each command must print a version at or above the minimum:

```bash
g++ --version        # or: clang++ --version; on Windows, cl in the Developer PowerShell
cmake --version      # 3.20 or newer
java -version        # 17 or newer
git --version
python3 --version    # 3.10 or newer; only for agentic coding (on Windows: python --version)
```

If `java -version` fails although Java is installed, set `JAVA_HOME` to the Java folder, or
add its `bin` folder to the `PATH`, and open a new terminal.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Next Steps

- [Quick Project Setup](./02a-quick-project-setup.md): your own project, built and running
- [Building Areg SDK with CMake](./01b-cmake-build.md): the SDK, its examples and tests
- [Building with Visual Studio](./01c-msvc-build.md)
- [Installing as a Package](./01a-areg-package.md): install once, use with `find_package`

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech (Artak Avetyan), www.areg.tech, email: info[at]areg.tech
