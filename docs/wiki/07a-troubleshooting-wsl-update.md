# Troubleshooting WSL Updates

This document provides solutions for common issues encountered while updating the Windows Subsystem for Linux (WSL).

## Table of Contents
1. [Issue 1: Update Error Code `Wsl/Service/CreateInstance/0x80040326`](#issue-1-update-error-code-wslservicecreateinstance0x80040326)
   - [Solution: Update WSL](#solution-update-wsl)
2. [Issue 2: Network Resolution (DNS) Issues](#issue-2-network-resolution-dns-issues)
   - [Solution: Update DNS Settings in `/etc/resolv.conf`](#solution-update-dns-settings-in-etcresolvconf)

## Issue 1: Update Error Code `Wsl/Service/CreateInstance/0x80040326`
If you receive this error code when trying to update, it typically indicates an issue with the WSL service. 

### Solution: Update WSL
To resolve it, run the following command in your PowerShell or Command Prompt with administrator privileges:
```bash
wsl --update
```
This command checks for and installs the latest WSL updates. If the problem persists, consider restarting your machine and running the command again.

## Issue 2: Network Resolution (DNS) Issues
If you experience network-related issues, such as the following error:
```plaintext
W: Failed to fetch http://archive.ubuntu.com/ubuntu/dists/jammy/InRelease Temporary failure resolving 'archive.ubuntu.com'
```
This is often happens due to DNS resolution errors in WSL. To check it, see the content of `/etc/resolv.conf` by running:
```bash
cat /etc/resolv.conf
```
If the `nameserver` entry indicates, for example, the IP-address of your DSL, like `nameserver 172.23.112.1`,  you need to change.

### Solution: Update DNS Settings in `/etc/resolv.conf`

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
