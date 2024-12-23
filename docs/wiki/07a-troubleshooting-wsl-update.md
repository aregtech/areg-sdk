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
3. [Issue 3: Configuring communication between Windows and WSL](#issue-3-configuring-communication-between-windows-and-wsl)
   - [Solution 1: Editing IP address settings](#solution-1-editing-ip-address-settings)

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

## Issue 3: Configuring communication between Windows and WSL
When using the defualt settings in ``areg.init`` router and server services such as ``mcrouter`` running on Windows will not connect to client services running on WSL. This can be remedied by changing the settings found in the ``areg.init`` file. 

> [!NOTE]
> When connecting a client on Windows to a server or router on WSL, the default ``localhost`` settings will allow the connection.

>[!IMPORTANT]
>This assumes WSL is using the default networking mode, NAT. Changing this setting to Mirror may allow ``localhost`` to work as intended, but this has not been tested. For more information about the Mirrored networking setting, see [this](https://learn.microsoft.com/en-us/windows/wsl/networking#mirrored-mode-networking) link. This setting can be changed either in the ``.wslconfig`` file, or in the WSL Settings application.]

### Solution 1: Editing IP address settings

Due to WSL not being able to access the Windows ``localhost`` IP address, routing services hosted on Windows must be configured differently to allow access by clients hosted on WSL.
Additionally, the services on WSL must be provided the correct IP address to connect to the Windows router.

1. To configure the WSL clients, the IP address of the connection to Windows must be specified. One way to find this IP address is to run the following in the WSL terminal:

   ```bash
      ip route show | grep -i default | awk '{print $3}'
   ```

2. In the WSL configuration file, the address for any services running as a router or server must be changed to the IP address found above. For example, to connect to an instance of ``mcrouter`` and ``logcollector`` running on Windows, the configuration file should have the lines
    ```plaintext
      router::*::address::tcpip = NEW_IP
   ```

   and 
   ```plaintext 
      logger::*::address::tcpip   = NEW_IP
   ```

   changed so that ``NEW_IP`` is replaced with the IP address found above.

3. Windows sees the WSL network as being an external network, so the IP address for any services running as a router or server must be set to an IP address which can accept connections from your LAN, such as ``0.0.0.0``. For example, to configure instances of ``mcrouter`` and ``logcollector`` running on Windows to accept connections from services on WSL, the Windows configuration files for these services should have the lines
   ```plaintext
      router::*::address::tcpip = 0.0.0.0
   ```
   and 
   ```plaintext 
       logger::*::address::tcpip   = 0.0.0.0
   ```

   with ``0.0.0.0`` or an IP address that is configured to accept external connections replacing the defualt ``localhost``.

>[!TIP]
> For more information about configuring network communication between WSL and Windows, this page may be helpful: [Accessing network applications with WSL](https://learn.microsoft.com/en-us/windows/wsl/networking).
