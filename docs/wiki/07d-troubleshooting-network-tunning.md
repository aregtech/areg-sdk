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

### Alternative: Apply via Terminal (No Text Editor Required)

If you prefer not to open a text editor, use one of the two options below.

#### Option A: Immediate Effect (Temporary – Resets on Reboot)

Open a terminal and paste this block. Each line takes effect instantly:

```bash
sudo sysctl -w net.core.rmem_max=16777216
sudo sysctl -w net.core.wmem_max=16777216
sudo sysctl -w net.ipv4.tcp_rmem="4096 87380 16777216"
sudo sysctl -w net.ipv4.tcp_wmem="4096 65536 16777216"
sudo sysctl -w net.core.netdev_max_backlog=10000
```

> **Note:** `net.ipv4.tcp_low_latency` was removed in Linux kernel 4.14. Skip it on modern systems – the kernel no longer recognises it.

#### Option B: Persistent (Survives Reboot)

This block appends the settings to `/etc/sysctl.conf` and activates them in one step. Run it once:

```bash
sudo tee -a /etc/sysctl.conf <<'EOF'

# Areg SDK – high-performance TCP settings
net.core.rmem_max=16777216
net.core.wmem_max=16777216
net.ipv4.tcp_rmem=4096 87380 16777216
net.ipv4.tcp_wmem=4096 65536 16777216
net.core.netdev_max_backlog=10000
EOF
sudo sysctl -p
```

> **Verify:** Run `sudo sysctl net.core.rmem_max` and confirm the output shows `16777216`.

---

## Part 2: Windows Optimization

Windows defaults to power efficiency settings that add latency to socket operations and throttle high-frequency packet delivery. All steps below must be applied to reach 1M+ messages/second.

> **Two shells are used in these steps. Always open the correct one for each step:**
> - **Command Prompt** – Press `Win+R`, type `cmd`, press `Ctrl+Shift+Enter` to open as Administrator.
> - **PowerShell** – Press `Win+R`, type `powershell`, press `Ctrl+Shift+Enter` to open as Administrator.
>
> PowerShell cmdlets (`Get-NetAdapter`, `New-ItemProperty`, `Set-NetTCPSetting`, etc.) are **not recognized** in Command Prompt and will fail with "not recognized" errors.

---

### Step 1: Set Power Plan to Maximum Performance

CPU frequency throttling adds microsecond jitter to every blocking socket call. Set the power plan to prevent this.

**Open PowerShell as Administrator and run:**

```powershell
# Create the Ultimate Performance plan and activate it in one step
$guid = (powercfg /duplicatescheme e9a42b02-d5df-448d-aa00-03f14749eb61 |
         Select-String -Pattern '[0-9a-f]{8}(-[0-9a-f]{4}){3}-[0-9a-f]{12}').Matches[0].Value
powercfg /setactive $guid

# Confirm the active plan
powercfg /getactivescheme
```

The output should show `Ultimate Performance` as the active scheme.

---

### Step 2: Configure Global TCP Stack Parameters

**Open Command Prompt as Administrator and run:**

```cmd
rem Check current state before changing anything
netsh int tcp show global

rem Apply high-throughput settings
netsh int tcp set global rss=enabled autotuninglevel=normal ecncapability=disabled timestamps=disabled rsc=enabled pacingprofile=off

rem Verify the changes were applied
netsh int tcp show global
```

Confirm the output matches the values you set. If it does not, close and reopen Command Prompt as Administrator and run again.

---

### Step 3: Disable Hardware RSC Per Network Adapter

The global TCP setting above permits RSC at the stack level for large-payload throughput. Each NIC also has its own hardware-level RSC toggle that operates independently. On many drivers, hardware RSC coalesces small packets and delays their delivery – disable it on all adapters.

**Open PowerShell as Administrator and run:**

```powershell
# Disable hardware RSC on all adapters
Get-NetAdapter | Disable-NetAdapterRsc -ErrorAction SilentlyContinue

# Verify – all IPv4Enabled and IPv6Enabled must show False
Get-NetAdapter | Get-NetAdapterRsc | Select-Object Name, IPv4Enabled, IPv6Enabled
```

---

### Step 4: Suppress Delayed ACK on All Network Interfaces

Windows delays sending TCP ACKs by up to 200 ms by default (delayed-ACK algorithm). At 1M+ messages/second, this stalls the sender's congestion window. There is no per-socket API for this on Windows – the registry is the only way.

The values must be written to **each interface subkey individually**, not to the parent key. The loop below handles all interfaces automatically.

**Open PowerShell as Administrator and run:**

```powershell
$ifPath = "HKLM:\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters\Interfaces"
Get-ChildItem $ifPath | ForEach-Object {
    New-ItemProperty -Path $_.PSPath -Name "TcpAckFrequency" -PropertyType DWORD -Value 1 -Force | Out-Null
    New-ItemProperty -Path $_.PSPath -Name "TCPNoDelay"      -PropertyType DWORD -Value 1 -Force | Out-Null
}

# Verify a sample interface – pick any GUID from the list
Get-ChildItem $ifPath | Select-Object -First 1 | Get-ItemProperty | Select-Object TcpAckFrequency, TCPNoDelay
```

> **`TcpAckFrequency=1`** – Forces an immediate ACK for every received TCP segment. This is the Windows equivalent of Linux `TCP_QUICKACK` and is the most impactful setting for high-frequency message rates.
>
> **`TCPNoDelay=1`** – System-wide default for `TCP_NODELAY`. Redundant for AREG SDK sockets (the SDK sets `TCP_NODELAY` per-socket programmatically) but harmless and helps other applications on the same machine.

---

### Step 5: Disable NIC Interrupt Moderation

Modern NICs batch hardware interrupts to reduce CPU load. At 1M+ messages/second, this batching delays packet delivery to the kernel. Disable interrupt moderation so every arriving packet triggers an immediate interrupt.

**Open PowerShell as Administrator and run:**

```powershell
# Check current state on all adapters
Get-NetAdapterAdvancedProperty -Name "*" |
    Where-Object DisplayName -match "Interrupt Moderation" |
    Select-Object Name, DisplayName, DisplayValue

# Disable interrupt moderation on all adapters
Set-NetAdapterAdvancedProperty -Name "*" -DisplayName "Interrupt Moderation" -DisplayValue "Disabled" -ErrorAction SilentlyContinue

# Verify
Get-NetAdapterAdvancedProperty -Name "*" |
    Where-Object DisplayName -match "Interrupt Moderation" |
    Select-Object Name, DisplayName, DisplayValue
```

> **Note:** The property name varies by vendor. If `"Interrupt Moderation"` returns no results, try `"Adaptive Inter-Frame Spacing"` or check the NIC's advanced properties in Device Manager.

---

### Step 6: Expand Winsock Driver Memory Buffers

The Windows socket driver (`afd.sys`) throttles payloads larger than its internal pool sizes. Increase all pools to handle 3 MB+ batches without kernel-level fragmentation.

**Open PowerShell as Administrator and run:**

```powershell
$afd = "HKLM:\SYSTEM\CurrentControlSet\Services\Afd\Parameters"
New-ItemProperty -Path $afd -Name "LargeBufferSize"      -PropertyType DWORD -Value 4194304  -Force | Out-Null
New-ItemProperty -Path $afd -Name "MediumBufferSize"     -PropertyType DWORD -Value 1048576  -Force | Out-Null
New-ItemProperty -Path $afd -Name "DefaultReceiveWindow" -PropertyType DWORD -Value 16777216 -Force | Out-Null
New-ItemProperty -Path $afd -Name "DefaultSendWindow"    -PropertyType DWORD -Value 16777216 -Force | Out-Null

# Verify
Get-ItemProperty $afd | Select-Object LargeBufferSize, MediumBufferSize, DefaultReceiveWindow, DefaultSendWindow
```

---

### Step 7: Disable OS Network Throttling

Windows throttles non-multimedia network traffic under CPU load. Disable this for sustained high message rates.

**Open PowerShell as Administrator and run:**

```powershell
$mm = "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Multimedia\SystemProfile"
New-ItemProperty -Path $mm -Name "NetworkThrottlingIndex" -PropertyType DWORD -Value 0xFFFFFFFF -Force | Out-Null
New-ItemProperty -Path $mm -Name "SystemResponsiveness"   -PropertyType DWORD -Value 0          -Force | Out-Null

# Verify
Get-ItemProperty $mm | Select-Object NetworkThrottlingIndex, SystemResponsiveness
```

---

### Step 8: Windows Server Only – Apply Datacenter TCP Profile

These cmdlets exist **only on Windows Server** editions. They will fail on Windows 10/11 with "not recognized" – skip this step on desktop systems.

**Open PowerShell as Administrator and run:**

```powershell
Set-NetTCPSetting -SettingName Datacenter     -AutoTuningLevelLocal Normal
Set-NetTCPSetting -SettingName InternetCustom -CongestionProvider   DCTCP
```

---

### Performance Regression Checklist

If message rates drop between runs (e.g. 1.5M → 1.25M msg/sec), check these first – Windows Updates commonly reset them:

**Open PowerShell as Administrator and run:**

```powershell
# 1. Check power plan – must show Ultimate Performance
powercfg /getactivescheme

# 2. Check RSC on adapters – all must show False
Get-NetAdapter | Get-NetAdapterRsc | Select-Object Name, IPv4Enabled, IPv6Enabled

# 3. Check ACK frequency on first interface – must show 1
$ifPath = "HKLM:\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters\Interfaces"
Get-ChildItem $ifPath | Select-Object -First 1 | Get-ItemProperty | Select-Object TcpAckFrequency, TCPNoDelay

# 4. Check interrupt moderation – all must show Disabled
Get-NetAdapterAdvancedProperty -Name "*" |
    Where-Object DisplayName -match "Interrupt Moderation" |
    Select-Object Name, DisplayValue
```

If any value is wrong, re-run the corresponding step above without rebooting – most settings take effect immediately except the Winsock buffer and throttling registry keys which require a restart.

---

⚠️ **Restart required.** Steps 6 and 7 (Winsock buffers and network throttling) write to registry keys loaded at boot. Steps 1–5 take effect immediately after running.

---

## Part 3: macOS Optimization

macOS applies conservative per-socket and system-wide buffer limits by default. The following settings increase TCP buffer sizes and eliminate acknowledgment delays that add latency to high-frequency message streams.

### Temporary Settings (Immediate Effect – Resets on Reboot)

Open a terminal and paste this block:

```bash
sudo sysctl -w kern.ipc.maxsockbuf=8388608
sudo sysctl -w net.inet.tcp.sendspace=1048576
sudo sysctl -w net.inet.tcp.recvspace=1048576
sudo sysctl -w net.inet.tcp.delayed_ack=0
sudo sysctl -w net.inet.tcp.mssdflt=1440
```

| Parameter | Default | Tuned | Effect |
|---|---|---|---|
| `kern.ipc.maxsockbuf` | 4 MB | 8 MB | Raises the system ceiling for per-socket buffers |
| `net.inet.tcp.sendspace` | 128 KB | 1 MB | Per-socket TCP send buffer |
| `net.inet.tcp.recvspace` | 128 KB | 1 MB | Per-socket TCP receive buffer |
| `net.inet.tcp.delayed_ack` | 3 | 0 | Disables delayed ACK; reduces round-trip latency |
| `net.inet.tcp.mssdflt` | 512 | 1440 | Default MSS closer to Ethernet MTU; reduces segment overhead |

> **Verify:** Run `sysctl net.inet.tcp.sendspace` and confirm the output shows `1048576`.

### Persistent Settings (Survives Reboot)

macOS reads `/etc/sysctl.conf` at boot. This file does not exist by default. The command below creates it (or appends to it if it already exists) and explains the reboot requirement:

```bash
sudo tee -a /etc/sysctl.conf <<'EOF'

# Areg SDK – high-performance TCP settings
kern.ipc.maxsockbuf=8388608
net.inet.tcp.sendspace=1048576
net.inet.tcp.recvspace=1048576
net.inet.tcp.delayed_ack=0
net.inet.tcp.mssdflt=1440
EOF
```

Reboot or apply settings immediately for the current session using the temporary block above.

> **Note:** On macOS Ventura (13) and later, System Integrity Protection (SIP) does not block `/etc/sysctl.conf`. If settings do not persist after reboot, verify SIP status with `csrutil status`.
