# Directory Overview: AREG Multitarget Router Application (`mtrouter`)

This directory contains the source code for the Multitarget Router (`mtrouter`), a core application that facilitates inter-process communication (IPC) for creating connected networks of Things at the network edge. Applications using the AREG framework require `mtrouter` to send and receive messages, making it an essential component of the AREG SDK. The `mtrouter` can operate as a console application or an OS-managed service, depending on the configuration.

### Directory Contents

- **[app](./app)**
  Contains the source code to initialize and start the `mtrouter` application. On Windows and Linux, it can run as a console application or system service.

- **[service](./service)**
  Implements the communication services for sending and receiving messages between connected applications.

- **[resource](./resource)**
  Contains resource files specific to the `mtrouter` application.

### Platform Support and Build Configurations

- **Linux**:
  The `mtrouter` runs as a console or OS-managed service application, and can be built with or without the `ncurses` library:
  - To enable `ncurses` features, compile the AREG extended static library by defining setting option `-DAREG_EXTENDED:BOOL=ON`.
  - To disable `ncurses`, omit `AREG_EXTENDED` option or set it to `OFF`, using only the ANSI C API.

- **Windows**:
  The `mtrouter` operates as a console application or OS-managed service, and can be built with or without the `Win32 API` for extended feature:
  - To enable `Win32 API` extended features, define `AREG_EXTENDED=1`.
  - To use only the ANSI C API, omit `AREG_EXTENDED` or set it to `0`.

> [!NOTE]
> It does not make sense to enable extended features if application runs as an OS-managed service, because extended features manage cursor position on console, which is not accessible when application runs as a service. To save resources and linking time, disable extended features in the build by setting option `-DAREG_EXTENDED:BOOL=OFF`

While the implementation varies between `Win32` and `POSIX` systems, all components maintain consistent behavior across platforms. Header files in `private` subdirectories are for internal use only and should not be included directly in external projects.

---

## Running Log Collector as a Service

### **Linux**:
To configure and run the `mtrouter` application as a Linux-managed service, follow these steps:

1. **Copy Service Configuration File**:
   - Copy the `mtrouter.service` file to the `/etc/systemd/system/` directory.

2. **Copy the Executable**:
   - Copy the built `mtrouter.elf` executable to the desired location, such as `/usr/local/bin`.

3. **Ensure Library Access (if applicable)**:
   - If the AREG Framework was built as a shared library, ensure that `mtrouter` has access to the `libareg.so` library (e.g., located in `/usr/lib`).

4. **Edit the Service Configuration**:
   - Open the `mtrouter.service` file and verify that the `ExecStart` path points to the correct location of the `mtrouter.elf` executable. For example:
     ```plaintext
     ExecStart=/usr/local/bin/mtrouter.elf --service
     ```
   - Ensure the `ExecStart` line includes the `--service` (or `-s`) as a command line option.

5. **Enable and Start the Service**:
   - Enable the service to start automatically at boot:
     ```bash
     sudo systemctl enable mtrouter.service
     ```
   - Start the service
     ```bash
     sudo systemctl start mtrouter.service
     ```

6. **Stop or Disable the Service**:
   - To stop the service:
     ```bash
     sudo systemctl stop mtrouter.service
     ```
   - To disable the service from starting at boot:
     ```bash
     sudo systemctl disable mtrouter.service
     ```

### **Windows**:
To configure and run the `mtrouter` application as a Windows-managed service, follow these steps:

1. **Copy the Binaries**:
   - Copy the `mtrouter.exe` and `areg.dll` binaries to the desired location.

2. **Install the Service**:
   - Open **PowerShell** as the Administrator.
   - Register the executable as a service by running:
     ```powershell
     .\mtrouter.exe --install
     ```

3. **Start the Service**:
   - Open the **Services** application (or run `services.msc` in the Command Prompt).
   - Locate the service named **AREG Log Collector Service**.
   - Start the service by right-clicking it and selecting **Start**.

4. **Stop and Uninstall the Service**:
   - Stop the service using the **Services** application or the `services.msc` command.
   - Uninstall the service by running the following command in PowerShell:
     ```powershell
     .\mtrouter.exe --uninstall
     ```

---
## Usage of AREG Multitarget Router (`mtrouter`)

The `mtrouter` provides flexible command-line and console commands for managing communication and connected instances.

### Command-Line Options

These options configure the startup mode of `mtrouter`:

- **`--console`** or **`-c`**:
  Runs `mtrouter` as a console application (default mode).
  **Example**: `mtrouter --console`

- **`--install`** or **`-i`**:
  Installs `mtrouter` as a system service (Windows only).
  **Example**: `mtrouter --install`

- **`--load`** or **`-l`**:
  Loads configuration from the specified initialization file.
  **Example**: `mtrouter --load=/path/to/config.file`

- **`--service`** or **`-s`**:
  Runs `mtrouter` as a system service.
  **Example**: `mtrouter --service`

- **`--uninstall`** or **`-u`**:
  Uninstalls `mtrouter` as a service (Windows only).
  **Example**: `mtrouter --uninstall`

### Console Commands

When running as a console application, the following commands are available:

- **`--help`** or **`-h`**:
  Displays a help message summarizing available commands.

- **`--instances`** or **`-n`**:
  Displays a list of connected instances.
  **Example**: `mtrouter --instances`

- **`--pause`** or **`-p`**:
  Pauses communication connections.
  **Example**: `mtrouter --pause`

- **`--quit`** or **`-q`**:
  Stops the router and exits the application.
  **Example**: `mtrouter --quit`

- **`--restart`** or **`-r`**:
  Restarts communication connections.
  **Example**: `mtrouter --restart`

- **`--silent`** or **`-t`**:
  Stops displaying data rates in the console.
  **Example**: `mtrouter --silent`

- **`--verbose`** or **`-v`**:
  Displays data rates in the console.
  **Example**: `mtrouter --verbose`

---
