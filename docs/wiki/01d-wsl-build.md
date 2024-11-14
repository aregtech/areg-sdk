# Building AREG SDK on Windows Subsystem for Linux (WSL)

Following these steps will guide you through setting up, troubleshooting, and running AREG SDK on WSL effectively.

## Table of Contents
1. [General Information](#1-general-information)
2. [Installing a Linux Distribution in WSL](#2-installing-a-linux-distribution-in-wsl)
3. [Troubleshooting WSL Updates](#3-troubleshooting-wsl-updates)
4. [Installing Build Tools](#4-installing-build-tools)
5. [Cloning AREG SDK in WSL](#5-cloning-areg-sdk-in-wsl)
6. [Compiling AREG SDK](#6-compiling-areg-sdk)
   - [CMake with Clang](#cmake-with-clang)
   - [CMake with GCC](#cmake-with-gcc)
   - [CMake for 32-bit Build with Clang](#cmake-for-32bit-build-with-clang)
   - [CMake for 32-bit ARM Build with GNU](#cmake-for-32bit-arm-build-with-gnu)
   - [CMake for 64-bit ARM Build with GNU](#cmake-for-64bit-arm-build-with-gnu)
7. [Running Applications](#7-running-applications)

---

## 1. General Information

The Windows Subsystem for Linux (WSL) enables users of Windows 10 (version 2004+) and higher to install and use Linux utilities and command-line tools natively. WSL provides access to distributions like Ubuntu and OpenSUSE, removing the need to switch OS environments. 

The following sections provide setup, installation, and troubleshooting steps to prepare your WSL environment for building the AREG SDK.

---

## 2. Installing a Linux Distribution in WSL

Follow [Microsoft's WSL installation guide](https://learn.microsoft.com/en-us/windows/wsl/install-manual) for complete steps. To install Ubuntu, use:
```bash
wsl --install -d ubuntu
```
After installation, update Ubuntu packages:
```bash
sudo apt-get update && sudo apt-get upgrade -y
```

### Setting WSL Version to WSL2
1. Open Command Prompt as Administrator.
2. Run:
   ```bash
   wsl --set-default-version 2
   ```
---

## 3. Troubleshooting WSL Updates

Experiencing update issues? Common solutions include:

- **Error Code**: `Wsl/Service/CreateInstance/0x80040326`  
   Run:
   ```bash
   wsl --update
   ```

- **Network Resolution Issues**: DNS issues can cause update failures. You may see errors like `W: Failed to fetch http://archive.ubuntu.com/ubuntu/dists/jammy/InRelease Temporary failure resolving 'archive.ubuntu.com'`. This can stem from an incorrect DNS in `/etc/resolv.conf`. Run following command to check:
  ```bash
  cat /etc/resolv.conf
  ```
  If it displays wrong DNS server, like `nameserver 172.23.112.1`, which indicates, for example, the IP-address of your DSL, open `/etc/resolv.conf` in the WSL Terminal and change the *nameserver* to *Google DNS*. If you're using `vim`, press the `i` key to enter insert mode and make the necessary changes:
   ```bash
   sudo vim /etc/resolv.conf
   ```
   Update `nameserver` to `8.8.8.8`, save the file, and retry the update. If you're using `vim`, press the `Esc` key, then type `:wq` and press `Enter`.<br/>
   Alternatively, execute this command in WSL Terminal to set the *nameserver* in `/etc/resolv.conf`:
   ```bash
   sudo sh -c "echo nameserver 8.8.8.8 > /etc/resolv.conf"
   ```
    
---

## 4. Installing Build Tools

Install the essential tools and libraries to compile AREG SDK:
```bash
sudo apt-get install -y git cmake build-essential clang libncurses-dev openjdk-17-jre
```

You would need additional tools if you want to compile for 32-bit system:
```bash
sudo apt-get install -y gcc-multilib g++-multilib
```

For package specifics, consult your Linux distribution's package list, such as [Ubuntu Packages](https://packages.ubuntu.com/).<br/>
Refer to the **System Requirements** for Linux platforms in the [Building AREG SDK with CMake](./01b-cmake-build.md) document.

---

## 5. Cloning AREG SDK in WSL

### Cloning Directly in WSL
1. Create a `projects` directory and navigate to it:
   ```bash
   mkdir ~/projects && cd ~/projects
   ```
2. Clone AREG SDK with submodules:
   ```bash
   git clone https://github.com/aregtech/areg-sdk.git
   ```
3. Enter the AREG SDK directory:
   ```bash
   cd areg-sdk
   ```

### Using an Existing Windows Clone
Alternatively, navigate to the project path in WSL if already cloned on *Windows*. For instance, if **AREG SDK** is located at `C:\projects\areg-sdk\`:
```bash
cd /mnt/c/projects/areg-sdk/
```

---

## 5. Compiling AREG SDK

This section provides example commands for compiling AREG SDK using CMake in different configurations.

### CMake with Clang
To configure and build binaries with the `clang++` compiler in `Release` mode:
```bash
cmake -B ./build -DAREG_COMPILER_FAMILY=llvm -DAREG_BUILD_TYPE=Release
cmake --build ./build -j
```

### CMake with GCC
To configure and build binaries with the `g++` compiler in `Debug` mode:
```bash
cmake -B ./build -DAREG_COMPILER_FAMILY=gnu -DAREG_BUILD_TYPE=Debug
cmake --build ./build -j
```

### CMake for 32-bit Build with Clang
To compile for a 32-bit system, first install the required libraries:
```bash
sudo apt-get install -y gcc-multilib g++-multilib
```
Configure and build binaries with `clang++` for a 32-bit system:
```bash
cmake -B ./build -DAREG_PROCESSOR=x86 -DAREG_COMPILER_FAMILY=llvm
cmake --build ./build -j
```

> [!TIP]  
> To verify if an application is compiled for a 32-bit system, navigate to the build binary directory and use:
> ```bash
> file ./mcrouter.out
> ```
> This command shows the binary's architecture. Example output for a 32-bit `mcrouter` binary:
> > ./mcrouter.out: ELF **32-bit** LSB pie executable, **Intel 80386**, version 1 (GNU/Linux), dynamically linked, interpreter /lib/ld-linux.so.2, BuildID[sha1]=3df1d5e3d1b90b9533b93a906cece6ff95fa816c, for GNU/Linux 3.2.0, not stripped
> 
> Alternatively, run:
> ```bash
> od -t x1 -t c ./mcrouter | head -n 2
> ```
> Here, the 5th byte of the **ELF Header** should be `001` for a 32-bit executable and `002` for a 64-bit.
> ```plaintext
> 0000000  7f  45  4c  46  01  01  01  00  00  00  00  00  00  00  00  00
>         177   E   L   F 001 001 001  \0  \0  \0  \0  \0  \0  \0  \0  \0
> ```
> For more details, see the **[ELF Header](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#File_header)** documentation.

> [!IMPORTANT]
> **Troubleshooting**: If you encounter a "could not find `<asm/errno.h>`" error, resolve it by creating a symbolic link:
> ```bash
> sudo ln -s /usr/include/asm-generic/ /usr/include/asm
> ```

### CMake for 32-bit ARM Build with GNU
To compile for a 32-bit ARM system, first install the required toolchain:
```bash
sudo apt-get install -y gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf binutils-arm-linux-gnueabihf
```

> [!NOTE]
> Make sure that `gcc-arm-linux-gnueabihf` is better solution than `gcc-arm-linux-gnueab` for your device. By default, for ARM processors AREG compiles with `gcc-arm-linux-gnueabihf`.


Then configure and build AREG SDK binaries:
```bash
cmake -B ./build -DAREG_PROCESSOR=arm -DAREG_COMPILER_FAMILY=gnu
cmake --build ./build -j
```
Check the binary with `file ./mcrouter.out`. If successful, you'll see output like:
> ./mcrouter.out: ELF **32-bit** LSB executable, **ARM**, EABI5 version 1 (GNU/Linux), dynamically linked, interpreter /lib/ld-linux.so.3, BuildID[sha1]=c606ea5ce7be9cb1175fd87587b5975e235c084e, for GNU/Linux 3.2.0, not stripped

### CMake for 64-bit ARM Build with GNU
To compile for a 64-bit ARM system, install the appropriate toolchain:
```bash
sudo apt-get install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu binutils-aarch64-linux-gnu
```
Then configure and build AREG SDK binaries:
```bash
cmake -B ./build -DAREG_PROCESSOR=aarch64 -DAREG_COMPILER_FAMILY=gnu
cmake --build ./build -j
```
Check the binary with `file ./mcrouter.out`. Successful output will look like:
> ./mcrouter.out: ELF **64-bit** LSB pie executable, **ARM aarch64**, version 1 (GNU/Linux), dynamically linked, interpreter /lib/ld-linux-aarch64.so.1, BuildID[sha1]=bcf786a8c893b950868addecbc347d24518a25cd, for GNU/Linux 3.7.0, not stripped

For more information, refer to the **[AREG SDK Build Guide](./01b-cmake-build.md)**.

---

## 6. Running Applications

Navigate to the SDK root directory, e.g.,:
```bash
cd /mnt/c/projects/areg-sdk/
```

Run applications located in the `bin` directory, such as `10_locservice.out`:
```bash
./product/build/llvm-clang++/linux-64-x86_64-release/bin/10_locservice.out
```

Sample output:
```
A Demo to demonstrate simple request, response, and broadcast ...
"Hello client [ TestServiceClient ]!", remaining [ 36 ] to process.
...
Exit application; check logs for details.
```

> [!NOTE]  
> For a list of examples and detailed run instructions, see the [README](./../../examples/README.md) in the AREG SDK `examples` directory.
