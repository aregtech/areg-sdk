# Directory Overview: AREG Multitarget Router Application (`mcrouter`)

This directory contains the source code for the Multitarget Router (`mcrouter`), a core application that facilitates inter-process communication (IPC) for creating connected networks of Things at the network edge. Applications using the AREG framework require `mcrouter` to send and receive messages, making it an essential component of the AREG SDK. The `mcrouter` can operate as a console application or an OS-managed service, depending on the configuration.

### Directory Contents

- **[app](./app)**
  Contains the source code to initialize and start the `mcrouter` application. On Windows and Linux, it can run as a console application or system service.

- **[service](./service)**
  Implements the communication services for sending and receiving messages between connected applications.

- **[resource](./resource)**
  Contains resource files specific to the `mcrouter` application.

### Platform Support and Build Configurations

- **Linux**:
  The `mcrouter` runs as a console or OS-managed service application, and can be built with or without the `ncurses` library:
  - To enable `ncurses` features, compile the AREG extended static library by defining setting option `-DAREG_EXTENDED:BOOL=ON`.
  - To disable `ncurses`, omit `AREG_EXTENDED` option or set it to `OFF`, using only the ANSI C API.

- **Windows**:
  The `mcrouter` operates as a console application or OS-managed service, and can be built with or without the `Win32 API` for extended feature:
  - To enable `Win32 API` extended features, define `AREG_EXTENDED=1`.
  - To use only the ANSI C API, omit `AREG_EXTENDED` or set it to `0`.

> [!NOTE]
> It does not make sense to enable extended features if application runs as an OS-managed service, because extended features manage cursor position on console, which is not accessible when application runs as a service. To save resources and linking time, disable extended features in the build by setting option `-DAREG_EXTENDED:BOOL=OFF`

While the implementation varies between `Win32` and `POSIX` systems, all components maintain consistent behavior across platforms. Header files in `private` subdirectories are for internal use only and should not be included directly in external projects.

---

## Running Log Collector as a Service

### **Linux**:
To configure and run the `mcrouter` application as a Linux-managed service, follow these steps:

1. **Copy Service Configuration File**:
   - Copy the `mcrouter.service` file to the `/etc/systemd/system/` directory.

2. **Copy the Executable**:
   - Copy the built `mcrouter.elf` executable to the desired location, such as `/usr/local/bin`.

3. **Ensure Library Access (if applicable)**:
   - If the AREG Framework was built as a shared library, ensure that `mcrouter` has access to the `libareg.so` library (e.g., located in `/usr/lib`).

4. **Edit the Service Configuration**:
   - Open the `mcrouter.service` file and verify that the `ExecStart` path points to the correct location of the `mcrouter.elf` executable. For example:
     ```plaintext
     ExecStart=/usr/local/bin/mcrouter.elf --service
     ```
   - Ensure the `ExecStart` line includes the `--service` (or `-s`) as a command line option.

5. **Enable and Start the Service**:
   - Enable the service to start automatically at boot:
     ```bash
     sudo systemctl enable mcrouter.service
     ```
   - Start the service
     ```bash
     sudo systemctl start mcrouter.service
     ```

6. **Stop or Disable the Service**:
   - To stop the service:
     ```bash
     sudo systemctl stop mcrouter.service
     ```
   - To disable the service from starting at boot:
     ```bash
     sudo systemctl disable mcrouter.service
     ```

### **Windows**:
To configure and run the `mcrouter` application as a Windows-managed service, follow these steps:

1. **Copy the Binaries**:
   - Copy the `mcrouter.exe` and `areg.dll` binaries to the desired location.

2. **Install the Service**:
   - Open **PowerShell** as the Administrator.
   - Register the executable as a service by running:
     ```powershell
     .\mcrouter.exe --install
     ```

3. **Start the Service**:
   - Open the **Services** application (or run `services.msc` in the Command Prompt).
   - Locate the service named **AREG Log Collector Service**.
   - Start the service by right-clicking it and selecting **Start**.

4. **Stop and Uninstall the Service**:
   - Stop the service using the **Services** application or the `services.msc` command.
   - Uninstall the service by running the following command in PowerShell:
     ```powershell
     .\mcrouter.exe --uninstall
     ```

---
## Usage of AREG Multitarget Router (`mcrouter`)

The `mcrouter` provides flexible command-line and console commands for managing communication and connected instances.

### Command-Line Options

These options configure the startup mode of `mcrouter`:

- **`--console`** or **`-c`**:
  Runs `mcrouter` as a console application (default mode).
  **Example**: `mcrouter --console`

- **`--install`** or **`-i`**:
  Installs `mcrouter` as a system service (Windows only).
  **Example**: `mcrouter --install`

- **`--load`** or **`-l`**:
  Loads configuration from the specified initialization file.
  **Example**: `mcrouter --load=/path/to/config.file`

- **`--service`** or **`-s`**:
  Runs `mcrouter` as a system service.
  **Example**: `mcrouter --service`

- **`--uninstall`** or **`-u`**:
  Uninstalls `mcrouter` as a service (Windows only).
  **Example**: `mcrouter --uninstall`

### Console Commands

When running as a console application, the following commands are available:

- **`--help`** or **`-h`**:
  Displays a help message summarizing available commands.

- **`--instances`** or **`-n`**:
  Displays a list of connected instances.
  **Example**: `mcrouter --instances`

- **`--pause`** or **`-p`**:
  Pauses communication connections.
  **Example**: `mcrouter --pause`

- **`--quit`** or **`-q`**:
  Stops the router and exits the application.
  **Example**: `mcrouter --quit`

- **`--restart`** or **`-r`**:
  Restarts communication connections.
  **Example**: `mcrouter --restart`

- **`--silent`** or **`-t`**:
  Stops displaying data rates in the console.
  **Example**: `mcrouter --silent`

- **`--verbose`** or **`-v`**:
  Displays data rates in the console.
  **Example**: `mcrouter --verbose`

---
