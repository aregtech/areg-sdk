# Directory Overview: Areg Log Collector Application

This directory contains the source code for the Areg Log Collector (`logcollector`) application, a robust tool for collecting logs from multiple processes across a network. The `logcollector` can operate as either a console application or an OS-managed service. As a key logging and debugging component of the Areg SDK, it is built alongside other SDK sources and can also be used independently for centralized log collection and application behavior analysis.

### Directory Contents

- **[app](./app)**
  Contains initialization code for running the `logcollector` as a console application or an OS-managed service.

- **[service](./service)**
  Implements communication with connected clients, such as log sources or observers, to receive and forward log messages.

- **[resources](./resources)**
  Includes resources specific to the `logcollector` application.

### Platform Support and Internal Headers

The implementation differs between `Win32` and `POSIX` systems, but all components maintain consistent behavior across platforms. Header files in `private` subdirectories are for internal use only and should not be included in external projects.

---

## Running Log Collector as a Service

### **Linux**:
To configure and run the `logcollector` application as a Linux-managed service, follow these steps:

1. **Copy Service Configuration File**:
   - Copy the `logcollector.service` file to the `/etc/systemd/system/` directory.

2. **Copy the Executable**:
   - Copy the built `logcollector.elf` executable to the desired location, such as `/usr/local/bin`.

3. **Ensure Library Access (if applicable)**:
   - If the Areg Framework was built as a shared library, ensure that `logcollector` has access to the `libareg.so` library (e.g., located in `/usr/lib`).

4. **Edit the Service Configuration**:
   - Open the `logcollector.service` file and verify that the `ExecStart` path points to the correct location of the `logcollector.elf` executable. For example:
     ```plaintext
     ExecStart=/usr/local/bin/logcollector.elf --service
     ```
   - Ensure the `ExecStart` line includes the `--service` (or `-s`) as a command line option.

5. **Enable and Start the Service**:
   - Enable the service to start automatically at boot:
     ```bash
     sudo systemctl enable logcollector.service
     ```
   - Start the service:
     ```bash
     sudo systemctl start logcollector.service
     ```

6. **Stop or Disable the Service**:
   - To stop the service:
     ```bash
     sudo systemctl stop logcollector.service
     ```
   - To disable the service from starting at boot:
     ```bash
     sudo systemctl disable logcollector.service
     ```

### **Windows**:
To configure and run the `logcollector` application as a Windows-managed service, follow these steps:

1. **Copy the Binaries**:
   - Copy the `logcollector.exe` and `areg.dll` binaries to the desired location.

2. **Install the Service**:
   - Open **PowerShell** as the Administrator.
   - Register the executable as a service by running:
     ```powershell
     .\logcollector.exe --install
     ```

3. **Start the Service**:
   - Open the **Services** application (or run `services.msc` in the Command Prompt).
   - Locate the service named **Areg Log Collector Service**.
   - Start the service by right-clicking it and selecting **Start**.

4. **Stop and Uninstall the Service**:
   - Stop the service using the **Services** application or the `services.msc` command.
   - Uninstall the service by running the following command in PowerShell:
     ```powershell
     .\logcollector.exe --uninstall
     ```

---

## Usage of Areg Log Collector (`logcollector`)

The `logcollector` supports a range of commands for managing log collection and application settings, offering flexibility for different environments.

### Command-Line Options

These options configure how the `logcollector` operates:

- **`--console`** or **`-c`**:
  Runs the `logcollector` as a console application (default).
  **Example**: `logcollector --console`

- **`--install`** or **`-i`**:
  Installs the `logcollector` as a system service (Windows only).
  **Example**: `logcollector --install`

- **`--load=<path>`** or **`-l`**:
  Initializes the `logcollector` using the specified configuration file.
  **Example**: `logcollector --load=/path/to/config.file`

- **`--service`** or **`-s`**:
  Runs the `logcollector` as an OS-managed service.
  **Example**: `logcollector --service`

- **`--uninstall`** or **`-u`**:
  Uninstalls the `logcollector` service (Windows only).
  **Example**: `logcollector --uninstall`


### Console Commands

When running as a console application, the following commands are available to manage logging and connected clients:

- **`--save`** or **`-a`**: Save logs to a file.
- **`--unsave`** or **`-b`**: Stop saving logs to a file.
- **`--query`** or **`-e`**: Query the list of logging scopes.
  **Example**: `--query *`

- **`--config`** or **`-f`**: Save the current configuration, including logging scopes, to a file.
- **`--help`** or **`-h`**: Display the help message.
- **`--instances`** or **`-n`**: Display the list of connected instances.
- **`--scope`** or **`-o`**: Update log scope priorities.
  **Example**: `--scope *::areg_base_NESocket=NOTSET`

- **`--pause`** or **`-p`**: Pause logging connections.
- **`--quit`** or **`-q`**: Stop the `logcollector` and exit.
- **`--restart`** or **`-r`**: Restart logging connections.
- **`--silent`** or **`-t`**: Stop displaying data rates (only if build with enabled extended features).
- **`--verbose`** or **`-v`**: Display data rates (only if build with enabled extended features).

---
