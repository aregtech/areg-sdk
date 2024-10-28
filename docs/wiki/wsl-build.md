# Building AREG SDK in Windows Subsystem for Linux (WSL)

## Contents
1. [General Information](#general-information)
2. [Installing Linux Distribution in WSL](#installing-linux-distribution-in-wsl)
3. [Troubleshooting WSL Updates](#troubleshooting-wsl-updates)
4. [Installing Build Tools](#installing-build-tools)
5. [Cloning AREG SDK in WSL](#cloning-areg-sdk-in-wsl)
6. [Compiling AREG SDK](#compiling-areg-sdk)
7. [Running Applications](#running-applications)

---

## General Information

The Windows Subsystem for Linux (WSL) enables Windows 10 (version 2004+) and higher users to install and use Linux utilities and command-line tools natively. WSL provides access to distributions like Ubuntu, OpenSUSE, and others, removing the need to switch between OS environments. 

The following sections outline the setup, installation, and troubleshooting process for WSL to prepare your environment for building the AREG SDK.

---

## 1. Installing Linux Distribution in WSL

Follow [Microsoft’s WSL installation guide](https://learn.microsoft.com/en-us/windows/wsl/install-manual) for complete steps. For Ubuntu, use:
```bash
wsl --install -d ubuntu
```
Once installed, update packages in Ubuntu:
```bash
sudo apt-get update | sudo apt-get upgrade -y
```

### Setting WSL Version to WSL2
1. Open Command Prompt as Administrator.
2. Run:
   ```bash
   wsl --set-default-version 2
   ```

---

## 2. Troubleshooting WSL Updates

Encountering errors with updates? Common solutions include:

- **Error Code**: `Wsl/Service/CreateInstance/0x80040326`  
   Run:
   ```bash
   wsl --update
   ```

- **Network Resolution Issues**: Temporary DNS issues may affect updates.
  Normally, when try to update or install a package, the user sees messages like this: `W: Failed to fetch http://archive.ubuntu.com/ubuntu/dists/jammy/InRelease Temporary failure resolving 'archive.ubuntu.com'`. The problem might be related with wrong DNS in the `/etc/resolv.conf` file. For example, it could be written `nameserver 172.23.112.1`, which is a wrong DNS IP. Open `/etc/resolv.conf` in your WSL Terminal and change the *nameserver* to *Google DNS*:
   ```bash
   sudo vim /etc/resolv.conf
   ```
   Update `nameserver` to `8.8.8.8`, save the file, then retry the update.<br/>
   Alternatively, you can execute the following command in the WSL Terminal to set the *nameserver* IP in the `/etc/resolv.conf` file:
   ```bash
   sudo sh -c "echo nameserver 8.8.8.8 > /etc/resolv.conf"
   ```
    
---

## 3. Installing Build Tools

Install essential build tools and libraries for compiling AREG SDK:
```bash
sudo apt-get install -y git cmake build-essential clang libncurses-dev openjdk-17-jre 
```

For package details, refer to your Linux distribution’s package list (e.g., [Ubuntu Packages](https://packages.ubuntu.com/)).<br/>
Also see the **System Requirements** for Linux platform in the [Building AREG SDK with CMake](./cmake-build.md) document.

---

## 4. Cloning AREG SDK in WSL

### Cloning Directly in WSL
1. Create a `projects` folder and navigate to it:
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
Alternatively, instead of cloning, navigate to the project’s path in WSL. For instance, if on your *Windows* machine the sources of **AREG SDK** are located in `C:\projects\areg-sdk\`:
```bash
cd /mnt/c/projects/areg-sdk/
```

---

## 5. Compiling AREG SDK

Example commands to build AREG SDK using CMake or Make:
- **CMake with Clang and extended features (Release):**
   ```bash
   cmake -B ./build -DAREG_COMPILER_FAMILY=llvm -DAREG_BUILD_TYPE=Release -DAREG_EXTENDED:BOOL=ON
   cmake --build ./build -j
   ```
- **CMake with GCC (Debug):**
   ```bash
   cmake -B ./build -DAREG_COMPILER_FAMILY=gnu -DAREG_BUILD_TYPE=Debug -DAREG_EXTENDED:BOOL=ON
   cmake --build ./build -j
   ```

Refer to the **[AREG SDK build](./cmake-build.md)** document for details.

---

## 6. Running Applications

Navigate to the SDK’s root directory, e.g.,:
```bash
cd /mnt/c/projects/areg-sdk/
```

Run applications in the `bin` directory, such as `10_locservice.out`:
```bash
./product/build/llvm-clang++/linux-64-x86_64-release/bin/10_locservice.out
```

Example output:
```
A Demo to demonstrate simple request, response, and broadcast ...
"Hello client [ TestServiceClient ]!", remain to process [ 36 ]
...
Exit application, check the logs for details!
```

> [!NOTE]  
> For a complete list of examples and instructions on how to run them, see the [README](./../../examples/README.md) in the AREG SDK `examples` directory.

---

By following these steps, you will be able to set up, troubleshoot, and run AREG SDK on WSL effectively.