# Building AREG SDK on Windows Subsystem for Linux (WSL)

Following these steps will guide you through setting up, troubleshooting, and running AREG SDK on WSL effectively.

## Contents
1. [General Information](#general-information)
2. [Installing a Linux Distribution in WSL](#installing-linux-distribution-in-wsl)
3. [Troubleshooting WSL Updates](#troubleshooting-wsl-updates)
4. [Installing Build Tools](#installing-build-tools)
5. [Cloning AREG SDK in WSL](#cloning-areg-sdk-in-wsl)
6. [Compiling AREG SDK](#compiling-areg-sdk)
7. [Running Applications](#running-applications)

---

## General Information

The Windows Subsystem for Linux (WSL) enables users of Windows 10 (version 2004+) and higher to install and use Linux utilities and command-line tools natively. WSL provides access to distributions like Ubuntu and OpenSUSE, removing the need to switch OS environments. 

The following sections provide setup, installation, and troubleshooting steps to prepare your WSL environment for building the AREG SDK.

---

## 1. Installing a Linux Distribution in WSL

Follow [Microsoft’s WSL installation guide](https://learn.microsoft.com/en-us/windows/wsl/install-manual) for complete steps. To install Ubuntu, use:
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

## 2. Troubleshooting WSL Updates

Experiencing update issues? Common solutions include:

- **Error Code**: `Wsl/Service/CreateInstance/0x80040326`  
   Run:
   ```bash
   wsl --update
   ```

- **Network Resolution Issues**: DNS issues can cause update failures. You may see errors like `W: Failed to fetch http://archive.ubuntu.com/ubuntu/dists/jammy/InRelease Temporary failure resolving 'archive.ubuntu.com'`. This can stem from an incorrect DNS in `/etc/resolv.conf`. Open `/etc/resolv.conf` in the WSL Terminal and change the *nameserver* to *Google DNS*:
   ```bash
   sudo vim /etc/resolv.conf
   ```
   Update `nameserver` to `8.8.8.8`, save the file, and retry the update.<br/>
   Alternatively, execute this command in WSL Terminal to set the *nameserver* in `/etc/resolv.conf`:
   ```bash
   sudo sh -c "echo nameserver 8.8.8.8 > /etc/resolv.conf"
   ```
    
---

## 3. Installing Build Tools

Install the essential tools and libraries to compile AREG SDK:
```bash
sudo apt-get install -y git cmake build-essential clang libncurses-dev openjdk-17-jre
```

For package specifics, consult your Linux distribution’s package list, such as [Ubuntu Packages](https://packages.ubuntu.com/).<br/>
Refer to the **System Requirements** for Linux platforms in the [Building AREG SDK with CMake](./cmake-build.md) document.

---

## 4. Cloning AREG SDK in WSL

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

Example commands to build AREG SDK with CMake:

- **CMake with Clang (Release):**
   ```bash
   cmake -B ./build -DAREG_COMPILER_FAMILY=llvm -DAREG_BUILD_TYPE=Release -DAREG_EXTENDED:BOOL=ON
   cmake --build ./build -j
   ```

- **CMake with GCC (Debug):**
   ```bash
   cmake -B ./build -DAREG_COMPILER_FAMILY=gnu -DAREG_BUILD_TYPE=Debug -DAREG_EXTENDED:BOOL=ON
   cmake --build ./build -j
   ```

Refer to the **[AREG SDK build](./cmake-build.md)** document for further details.

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
