# Low-Latency TCP Tuning Guide for High-Performance Networking

This document provides step-by-step instructions to configure Windows and Native Linux systems for maximum TCP throughput and message rates. Follow these steps if your application processes massive data loads (e.g., 3MB batches) or high-frequency message streams (e.g., 1M+ messages/second).

---

## Part 1: Native Linux Optimization

Linux handles local loopback (localhost) exceptionally well, but its default configurations restrict high-frequency physical network card performance.

### Step 1: Open the System Configuration File

   1. Open a terminal window.
   2. Open the system parameter file using your preferred text editor with administrative privileges:
   ```bash
   sudo nano /etc/sysctl.conf
   ```
   
### Step 2: Append High-Performance Parameters

Scroll to the very bottom of the file and paste the following configuration lines:

#### Maximize core socket buffer allocation memory pools (16MB)

```bash
net.core.rmem_max=16777216
net.core.wmem_max=16777216
```

#### Configure auto-tuning memory scaling limits for TCP reads/writes

```bash
net.ipv4.tcp_rmem=4096 87380 16777216
net.ipv4.tcp_wmem=4096 65536 16777216
```

#### Instruct the scheduler to prioritize raw network throughput over power-saving delay loops

```bash
net.ipv4.tcp_low_latency=1
```

#### Increase the maximum backlog queue size for processing incoming packets

```bash
net.core.netdev_max_backlog=10000
```

### Step 3: Save and Apply Settings

   1. If using nano, press Ctrl+O and then Enter to save the file. Press Ctrl+X to exit the editor.
   2. Run the following command to instantly force the Linux kernel to apply your new settings without restarting:
   ```bash
   sudo sysctl -p
   ```

---

## Part 1: Windows Optimization

Windows is configured by default for power efficiency and standard office web browsing. To achieve server-grade loopback and physical network performance, you must lift these restrictive OS boundaries.

### Step 1: Set the Power Plan to Maximum Performance

Windows power-saving profiles dynamically throttle the CPU clock speed, which adds microsecond delays to synchronous blocking socket operations.

   1. Open the Start Menu, type cmd, right-click Command Prompt, and select Run as Administrator.
   2. Paste the following command and press Enter:
   ```bash
   powercfg /duplicatescheme e9a42b02-d5df-448d-aa00-03f14749eb61
   ```
   3. Open the Windows Control Panel, navigate to Power Options, and select the newly unlocked Ultimate Performance power plan.

### Step 2: Configure Global TCP Stack Parameters

These settings ensure the Windows kernel allows TCP windows to expand smoothly and prevents the OS from intentionally slowing down large data packets.

   1. Open Command Prompt as Administrator.
   2. Copy and paste this exact command block, then press Enter:
   ```bash
   netsh int tcp set global rss=enabled autotuninglevel=normal ecncapability=disabled timestamps=disabled rsc=enabled pacingprofile=off
   ```
   
### Step 3: Choose Your Network Environment Profile

You must choose either Step 3a or Step 3b based on where your application is running. Do not run both.

#### Step 3a: For Enterprise Windows Server OS

If you are running on Windows Server, you can use the low-latency Datacenter template via PowerShell.

   1. Open the Start Menu, search for PowerShell, right-click it, and choose Run as Administrator.
   2. Run these two commands sequentially:
   ```bash
   Set-NetTCPSetting -SettingName Datacenter -AutoTuningLevelLocal Normal
   Set-NetTCPSetting -SettingName InternetCustom -CongestionProvider DCTCP
   ```
   
#### Step 3b: For Windows 10 or Windows 11 (Desktop/Mobile Classes)

Desktop editions do not support the Server cmdlets. Instead, use registry keys to force immediate packet delivery.

   1. Open PowerShell as Administrator.
   2. Run these commands to eliminate packet acknowledgment delays:
   ```bash
   New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters\Interfaces" -Name "TcpAckFrequency" -PropertyType DWORD -Value 1 -Force
   New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters\Interfaces" -Name "TCPNoDelay" -PropertyType DWORD -Value 1 -Force
   ```
   
### Step 4: Expand Winsock Driver Memory Buffers

The core Windows socket driver (afd.sys) throttles large 3MB payloads if its internal memory pools are too small. Expand them via the registry.

   1. Open PowerShell as Administrator.
   2. Paste and run this exact script block:
   ```bash
   New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Services\Afd\Parameters" -Name "LargeBufferSize" -PropertyType DWORD -Value 4194304 -Force
   New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Services\Afd\Parameters" -Name "MediumBufferSize" -PropertyType DWORD -Value 1048576 -Force
   New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Services\Afd\Parameters" -Name "DefaultReceiveWindow" -PropertyType DWORD -Value 16777216 -Force
   New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Services\Afd\Parameters" -Name "DefaultSendWindow" -PropertyType DWORD -Value 16777216 -Force
   ```
   
### Step 5: Disable OS Network Throttling

Windows deliberately throttles non-multimedia network traffic when the system is under heavy computational load. Disable this behavior.

   1. Open PowerShell as Administrator.
   2. Run these commands:
   ```bash
   New-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile" -Name "NetworkThrottlingIndex" -PropertyType DWORD -Value 4294967295 -Force
   New-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile" -Name "SystemResponsiveness" -PropertyType DWORD -Value 0 -Force
   ```
   
⚠️ CRITICAL STEP: You must Restart your computer now. Windows cannot load these registry and system driver modifications until a full system reboot occurs.
