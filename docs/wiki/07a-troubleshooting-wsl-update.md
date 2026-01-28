# Troubleshooting WSL Updates

This document outlines solutions to common issues encountered while updating the Windows Subsystem for Linux (WSL) and configuring network communication between Windows and WSL for Areg SDK applications.

---

## Table of Contents

1. [Issue 1: Update Error Code](#issue-1-update-error-code)
2. [Issue 2: Network Resolution (DNS) Issues](#issue-2-network-resolution-dns-issues)
3. [Issue 3: Configuring Communication Between Windows and WSL](#issue-3-configuring-communication-between-windows-and-wsl)

---

## Issue 1: Update Error Code

### Error: `Wsl/Service/CreateInstance/0x80040326`

If you receive this error code when trying to update, it typically indicates an issue with the WSL service.

### Solution 1: Update WSL

To resolve it, run the following command in PowerShell or Command Prompt with **administrator privileges**:

```powershell
wsl --update
```

After updating, restart your computer and verify the update is applied.

### Solution 2: Reinstall WSL

Ensure your Windows system is up-to-date, as outdated system components can interfere with WSL updates. If the error persists, reinstall WSL:

**Step 1: Uninstall WSL**

Open PowerShell as administrator and run:

```powershell
dism.exe /online /disable-feature /featurename:Microsoft-Windows-Subsystem-Linux /norestart
dism.exe /online /disable-feature /featurename:VirtualMachinePlatform /norestart
```

**Step 2: Reinstall WSL**

Re-enable the WSL and Virtual Machine Platform features:

```powershell
dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
```

**Step 3: Restart Your Machine**

Restart your computer to finalize the reinstallation.

**Step 4: Verify Installation**

After restart, verify WSL is working:

```powershell
wsl --status
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Issue 2: Network Resolution (DNS) Issues

### Symptoms

Network-related errors such as:

```plaintext
W: Failed to fetch http://archive.ubuntu.com/ubuntu/dists/jammy/InRelease 
   Temporary failure resolving 'archive.ubuntu.com'
```

This often occurs due to DNS resolution errors in WSL.

### Diagnosing DNS Issues

Check the current DNS configuration:

```bash
cat /etc/resolv.conf
```

If the `nameserver` entry shows your router's IP address (e.g., `nameserver 172.23.112.1`) instead of a public DNS server, you need to update the configuration.

### Solution 1: Update DNS Settings in `/etc/resolv.conf`

**Method A: Using Text Editor**

1. Open the WSL terminal
2. Edit the DNS resolver configuration file:
   ```bash
   sudo vim /etc/resolv.conf
   ```
3. Change the `nameserver` entry:
   - Press `i` to enter insert mode
   - Replace the existing IP with `8.8.8.8` (Google DNS)
   - Alternative DNS: `1.1.1.1` (Cloudflare), `8.8.4.4` (Google secondary)
4. Save and close:
   - Press `Esc`
   - Type `:wq`
   - Press `Enter`

**Method B: Using Command Line**

Set DNS directly with a single command:

```bash
sudo sh -c "echo nameserver 8.8.8.8 > /etc/resolv.conf"
```

**Verify DNS Resolution**

Test DNS resolution:

```bash
nslookup google.com
```

### Solution 2: Enable Automatic DNS Resolution

Configure WSL to automatically update DNS settings:

**Step 1: Check `/etc/wsl.conf`**

Edit the WSL configuration file:

```bash
sudo vim /etc/wsl.conf
```

Add or update the `[network]` section:

```ini
[network]
generateResolvConf = true
```

**Step 2: Restart WSL**

Restart the WSL service:

```bash
# In PowerShell (Windows)
wsl --shutdown

# Wait a few seconds, then start WSL again
wsl
```

### Solution 3: Reset WSL Network Configuration

If DNS issues persist, reset the network configuration:

**Step 1: Reset Winsock**

Open PowerShell as administrator:

```powershell
netsh winsock reset
```

**Step 2: Restart Machine**

Restart your computer to apply changes.

**Step 3: Verify Resolution**

After restart, launch WSL and test DNS:

```bash
ping google.com
```

> [!TIP]
> These solutions should address most update and DNS-related issues encountered with WSL. If problems persist, consult the [official WSL documentation](https://learn.microsoft.com/en-us/windows/wsl/) or seek assistance from community forums.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Issue 3: Configuring Communication Between Windows and WSL

### Background

When using Windows Subsystem for Linux (WSL) with Areg SDK, you may encounter communication issues between Windows and WSL services. By default, the [`areg.init`](./../../framework/areg/resources/areg.init) configuration file uses `localhost` for service endpoints, which works for services running on the same platform but not across Windows and WSL boundaries.

### Understanding WSL Networking

> [!IMPORTANT]
> **Key Networking Concepts:**
>
> - **Windows-to-WSL Connections**: Windows applications can connect to WSL services using `localhost` without additional configuration.
>
> - **WSL-to-Windows Connections**: WSL applications **cannot** connect to Windows services using `localhost` in the default NAT networking mode. You must use the Windows host IP address.
>
> - **WSL Networking Modes**:
>   - **NAT Mode** (default): WSL uses Network Address Translation. Requires IP address configuration for WSL-to-Windows connections.
>   - **Mirror Mode**: May allow `localhost` to work seamlessly between Windows and WSL (not fully tested).
>
> For more details, refer to the official documentation on **[Mirrored Mode Networking](https://learn.microsoft.com/en-us/windows/wsl/networking#mirrored-mode-networking)**. Networking modes can be configured in the `.wslconfig` file or via WSL Settings.

### Common Scenario

In the following examples, we configure communication for:
- **Message Router (`mtrouter`)**: Running on Windows
- **Log Collector (`logcollector`)**: Running on WSL
- **Applications**: Can run on either Windows or WSL

### Solution 1: Update All IP Addresses (Recommended)

This approach explicitly configures all service IP addresses, providing clear visibility and avoiding ambiguity.

#### Step 1: Identify IP Addresses

**Windows Host IP Address:**

Open PowerShell or Command Prompt:

```powershell
ipconfig
```

Look for the `Ethernet adapter vEthernet (WSL)` or `Wi-Fi` section and note the IPv4 Address.

**Example output:**
```
Ethernet adapter vEthernet (WSL):
   IPv4 Address. . . . . . . . . . . : 172.23.96.1
```

**WSL IP Address:**

Open WSL terminal:

```bash
ip addr show eth0
```

Or use `ifconfig`:

```bash
ifconfig eth0
```

Look for the `inet` address.

**Example output:**
```
inet 172.23.99.47/20
```

Note both IP addresses for use in configuration.

#### Step 2: Update WSL Configuration

Edit `areg.init` for applications running on **WSL**:

```bash
sudo vim ./config/areg.init
```

Update the service addresses:

```ini
# Message Router (running on Windows)
router::*::address::tcpip = 172.23.96.1    # Windows host IP

# Log Collector (running on WSL)
logger::*::address::tcpip = 172.23.99.47   # WSL IP
```

Replace the example IPs with your actual addresses from Step 1.

#### Step 3: Update Windows Configuration

Edit `areg.init` for applications running on **Windows**:

```ini
# Message Router (running on Windows)
router::*::address::tcpip = 172.23.96.1    # Windows host IP

# Log Collector (running on WSL)
logger::*::address::tcpip = 172.23.99.47   # WSL IP
```

Use the same IP addresses as in the WSL configuration.

#### Step 4: Restart Services

Restart all services to apply changes:

**On Windows:**
```powershell
# Stop services
taskkill /IM mtrouter.exe /F

# Start services
.\mtrouter.exe
```

**On WSL:**
```bash
# Stop services
pkill -9 logcollector

# Start services
./logcollector &
```

#### Step 5: Verify Communication

**Test from WSL:**
```bash
# Test connection to Windows router
telnet 172.23.96.1 8181
```

**Test from Windows:**
```powershell
# Test connection to WSL log collector
telnet 172.23.99.47 8282
```

### Solution 2: Update Only Necessary IP Addresses

This approach minimizes configuration changes by leveraging the fact that Windows applications can access WSL services via `localhost`.

> [!NOTE]
> **Key Principle**: Only the Windows host IP address needs to be configured because:
> - Windows → WSL: `localhost` works automatically
> - WSL → Windows: Requires Windows host IP address

#### Step 1: Identify Windows IP Address

Open PowerShell or Command Prompt:

```powershell
ipconfig
```

Note the IPv4 Address for the `Ethernet adapter vEthernet (WSL)` or active network adapter.

**Example:**
```
IPv4 Address. . . . . . . . . . . : 172.23.96.1
```

#### Step 2: Update WSL Configuration

Edit `areg.init` for applications running on **WSL**:

```bash
sudo vim ./config/areg.init
```

Update only the router address:

```ini
# Message Router (running on Windows)
router::*::address::tcpip = 172.23.96.1    # Windows host IP

# Log Collector (running on WSL) - leave as localhost
logger::*::address::tcpip = localhost
```

#### Step 3: Update Windows Configuration

Edit `areg.init` for applications running on **Windows**:

```ini
# Message Router (running on Windows)
router::*::address::tcpip = 172.23.96.1    # Windows host IP

# Log Collector (running on WSL) - leave as localhost
logger::*::address::tcpip = localhost
```

#### Step 4: Restart Services

Restart services as described in Solution 1, Step 4.

### Important Configuration Rules

> [!IMPORTANT]
> **You cannot mix Solution 1 and Solution 2.**
>
> For each service (router, logger), you must use the **same configuration** in both Windows and WSL `areg.init` files:
>
> - **Consistent**: Both files use `localhost` for a service
> - **Consistent**: Both files use specific IP address for a service
> - **Inconsistent** (❌ **will fail**): One file uses `localhost`, the other uses IP address
>
> **Example of incorrect configuration:**
> ```ini
> # Windows areg.init
> logger::*::address::tcpip = localhost
>
> # WSL areg.init
> logger::*::address::tcpip = 172.23.99.47    # ❌ WILL NOT WORK
> ```
>
> For detailed explanation, see [this discussion](https://github.com/aregtech/areg-sdk/pull/480#issuecomment-2568508064).

### Troubleshooting Communication Issues

**Firewall Blocking Connections**

If connections fail, check Windows Firewall:

```powershell
# Allow specific port
netsh advfirewall firewall add rule name="Areg Router" dir=in action=allow protocol=TCP localport=8181

# Allow specific application
netsh advfirewall firewall add rule name="Areg Router" dir=in action=allow program="C:\path\to\mtrouter.exe"
```

**IP Address Changes**

WSL IP addresses may change after system restart. If connections fail:

1. Re-check IP addresses using `ipconfig` and `ip addr`
2. Update `areg.init` files if addresses changed
3. Restart services

**Verify Port Availability**

Check if ports are listening:

**Windows:**
```powershell
netstat -an | findstr 8181
```

**WSL:**
```bash
netstat -tuln | grep 8282
```

### Additional Resources

For further details on configuring network communication between Windows and WSL, refer to the official documentation:
- [Accessing Network Applications with WSL](https://learn.microsoft.com/en-us/windows/wsl/networking)
- [Advanced Settings Configuration in WSL](https://learn.microsoft.com/en-us/windows/wsl/wsl-config)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Summary

This document covered three main categories of WSL issues:

1. **Update Errors**: Service initialization failures resolved by updating or reinstalling WSL
2. **DNS Issues**: Network resolution problems fixed by configuring DNS servers
3. **Windows-WSL Communication**: Cross-platform service connectivity configured via IP addresses

### Key Takeaways

- ✅ Always run WSL commands with administrator privileges when needed
- ✅ Test DNS resolution after configuration changes
- ✅ Use consistent IP address configuration across Windows and WSL
- ✅ Restart services after configuration changes
- ✅ Verify firewall rules allow required ports

For additional help, consult the [official WSL documentation](https://learn.microsoft.com/en-us/windows/wsl/) or community forums.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
