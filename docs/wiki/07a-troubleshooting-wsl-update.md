# Troubleshooting WSL Updates

This document outlines solutions to common issues encountered while updating the Windows Subsystem for Linux (WSL), helping ensure a smooth update process.

---

## Table of Contents
1. [Issue 1: Update Error Code `Wsl/Service/CreateInstance/0x80040326`](#issue-1-update-error-code-wslservicecreateinstance0x80040326)
   - [Solution 1: Update WSL](#solution-1-update-wsl)
   - [Solution 2: Reinstall WSL](#solution-2-reinstall-wsl)
2. [Issue 2: Network Resolution (DNS) Issues](#issue-2-network-resolution-dns-issues)
   - [Solution 1: Update DNS Settings in `/etc/resolv.conf`](#solution-1-update-dns-settings-in-etcresolvconf)
   - [Solution 2: Enable Automatic DNS Resolution](#solution-2-enable-automatic-dns-resolution)
   - [Solution 3: Reset WSL Network Configuration](#solution-3-reset-wsl-network-configuration)
3. [Issue 3: Configuring Communication Between Windows and WSL](#issue-3-configuring-communication-between-windows-and-wsl)
   - [Solution 1: Configuring WSL Applications to Access Windows Services](#solution-1-configuring-wsl-applications-to-access-windows-services)
   - [Solution 2: Solution 2: Running some servers on WSL and some on Windows](#solution-2-configuring-mixed-service-deployment-between-windows-and-wsl)

---

## Issue 1: Update Error Code `Wsl/Service/CreateInstance/0x80040326`
If you receive this error code when trying to update, it typically indicates an issue with the WSL service. 

### Solution 1: Update WSL
To resolve it, run the following command in your PowerShell or Command Prompt with administrator privileges:
```bash
wsl --update
```
After updating, restart your computer and try running the command again to ensure the update is applied.

### Solution 2: Reinstall WSL

Ensure your Windows system is up-to-date, as outdated system components can interfere with WSL updates. If the error still persists, consider reinstalling WSL:

1. **Uninstall WSL**:
   Open PowerShell as an administrator and run:
   ```bash
   dism.exe /online /disable-feature /featurename:Microsoft-Windows-Subsystem-Linux /norestart
   dism.exe /online /disable-feature /featurename:VirtualMachinePlatform /norestart
   ```

2. **Reinstall WSL**:
   Re-enable the WSL and Virtual Machine Platform features:
   ```bash
   dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
   dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
   ```

3. **Restart Your Machine**:
   Restart your computer to finalize the reinstallation.

---

## Issue 2: Network Resolution (DNS) Issues

If you experience network-related issues, such as the following error:
```plaintext
W: Failed to fetch http://archive.ubuntu.com/ubuntu/dists/jammy/InRelease Temporary failure resolving 'archive.ubuntu.com'
```
This is often happens due to DNS resolution errors in WSL. To check it, see the content of `/etc/resolv.conf` by running:
```bash
cat /etc/resolv.conf
```
If the `nameserver` entry indicates, for example, the IP-address of your DSL, like `nameserver 172.23.112.1`, you need to change.

### Solution 1: Update DNS Settings in `/etc/resolv.conf`

1. **Edit `/etc/resolv.conf`**:
   - Open the WSL Terminal.
   - Edit the DNS resolver configuration file:
     ```bash
     sudo vim /etc/resolv.conf
     ```
   - Change the `nameserver` entry (type `i` to enter insert mode) to use Google's DNS by replacing it with `8.8.8.8`.
   - Save and close the file (type `Esc`, type `:wq`, press `Enter`).

2. **Set DNS Using a Command**:
   Alternatively, set the DNS directly with this command:
   ```bash
   sudo sh -c "echo nameserver 8.8.8.8 > /etc/resolv.conf"
   ```
   This command updates the `nameserver` in `/etc/resolv.conf` to Google's DNS. Once updated, retry the WSL update command. 

Following these steps should resolve most DNS-related update issues in WSL.

### Solution 2: Enable Automatic DNS Resolution

If WSL is not automatically updating DNS settings, configure it to do so:

1. **Check `/etc/wsl.conf`**:
   Ensure the following configuration is present:
   ```bash
   sudo vim /etc/wsl.conf
   ```
   Add or update the `[network]` section to enable automatic DNS:
   ```plaintext
   [network]
   generateResolvConf = true
   ```

2. **Restart WSL**:
   Restart the WSL service:
   ```bash
   wsl --shutdown
   wsl
   ```

### Solution 3: Reset WSL Network Configuration

If the issue persists, reset the WSL network configuration:

1. Open PowerShell as an administrator and run:
   ```bash
   netsh winsock reset
   ```

2. Restart your machine to apply changes.

3. Restart WSL and check if the issue is resolved.

> [!TIP]
> These solutions should address most update and DNS-related issues encountered with WSL. If problems persist, consult the [official WSL documentation](https://learn.microsoft.com/en-us/windows/wsl/) or seek assistance from the community forums.

---

## Issue 3: Configuring Communication Between Windows and WSL

When using Windows Subsystem for Linux (WSL), it is common to encounter issues with communication between Windows and WSL services. This section provides solutions to help configure communication between Windows and WSL.

By default, the [`areg.init`](./../../framework/areg/resources/areg.init) configuration file uses the `localhost` IP address to run the `mcrouter` and `logcollector` services on the Windows platform. Applications running within WSL cannot connect to these services due to this configuration. To resolve this, you need to modify the [`areg.init`](./../../framework/areg/resources/areg.init) file settings to enable proper communication.

> [!IMPORTANT]
>
> - **Windows-to-WSL Connections**: If a Windows-based client needs to connect to a router running in WSL, the default `localhost` settings should work without additional changes.
>
> - **Networking Mode in WSL**:
>   - The default WSL networking mode is **NAT (Network Address Translation)**. With this mode, connections from WSL to Windows require changes to the `areg.init` configuration file.
>   - Switching WSL to **Mirror Networking Mode** may allow the `localhost` setting to work seamlessly between Windows and WSL. However, this has not been fully tested.
>  
> For more details, refer to the official documentation on **[Mirrored Mode Networking](https://learn.microsoft.com/en-us/windows/wsl/networking#mirrored-mode-networking)**. Networking modes can be adjusted either through the `.wslconfig` file or via the WSL Settings application.

### Solution 1: Configuring WSL Applications to Access Windows Services

When WSL applications cannot connect to services running on Windows due to the use of default `localhost` address, you can reconfigure the [`areg.init`](./../../framework/areg/resources/areg.init) settings to enable communication.

**Steps:**

1. **Identify the Windows IP Address for WSL**
   Run the following command in the WSL terminal to find the IP address for connecting to Windows:
   ```bash
   ip route show | grep -i default | awk '{print $3}'
   ```

2. **Update WSL Configuration**
   Edit the `areg.init` configuration file for applications running on WSL to specify the IP address of Windows services. Update the lines:
   ```
   router::*::address::tcpip = <windows-ip-address>
   logger::*::address::tcpip = <windows-ip-address>
   ```
   Replace `<windows-ip-address>` with the IP address found in **Step 1**.

3. **Update Windows Configuration**
   To allow external connections, Windows services must be configured to listen on an accessible IP address. Edit the `areg.init` file on Windows to replace `localhost` with `0.0.0.0` or another appropriate IP address:
   ```
   router::*::address::tcpip = 0.0.0.0
   logger::*::address::tcpip = 0.0.0.0
   ```
   Alternatively, use a specific IP address that permits external connections on your LAN.

4. **Restart Services**
   After updating the configuration files, restart the relevant services (`mcrouter`, `logcollector`, etc.) to apply the changes.


### Solution 2: Configuring Mixed Service Deployment Between Windows and WSL

For a setup where some services and/or applications run on Windows and others on WSL, ensure proper configuration of `areg.init` files for both environments. For example, let's consider a case if `mcrouter` runs on Windows and `logcollector` on WSL.

**Steps:**

1. **Identify IP Addresses**
   - Run `ipconfig` on Windows to find the IP address for the Windows host.
   - Run `ifconfig` in the WSL terminal to find the IP address for WSL.

2. **Update Configuration Files**
   Modify the `areg.init` file on both Windows and WSL to specify the correct IP addresses for services:
   - On WSL:
     ```
     router::*::address::tcpip = <windows-ip-address>
     ```
   - On Windows:
     ```
     logger::*::address::tcpip = <wsl-ip-address>
     ```

3. **Start Services**
   - Launch `mcrouter` on Windows and `logcollector` on WSL.

4. **Verify Communication**
   Start AREG-based applications or the `logobserver` tool on either Windows, WSL, or both, and confirm that communication between services is functional.

For further details on configuring network communication between Windows and WSL, refer to the official documentation: [Accessing Network Applications with WSL](https://learn.microsoft.com/en-us/windows/wsl/networking).
