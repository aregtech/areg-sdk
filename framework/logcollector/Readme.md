# Directory Overview: Areg Log Collector Application

This directory contains the source code for the Areg Log Collector (`logcollector`) application, a robust tool for collecting logs from multiple processes across a network. The `logcollector` can operate as either a console application or an OS-managed service. As a key logging and debugging component of the Areg SDK, it is built alongside other SDK sources and can also be used independently for centralized log collection and application behavior analysis.

### Directory Contents

- **[app](./app)**
  Contains initialization code for running the `logcollector` as a console application or an OS-managed service.

- **[service](./service)**
  Implements communication with connected clients, such as log sources or observers, to receive and forward log messages. It also contains the optional SQLite storage that saves the collected logs in a `.sqlog` database file.

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

- **`--log=<target>`** or **`-d`**:
  Selects whether the collected logs are saved in a database file. The value overrides the setting of the configuration file. Valid values are `db` and `nodb`. When the value is `db`, an optional file path may follow.
  **Example**: `logcollector --log=db` or `logcollector --log=db ./logs/session_%time%.sqlog` or `logcollector --log=nodb`

- **`--service`** or **`-s`**:
  Runs the `logcollector` as an OS-managed service.
  **Example**: `logcollector --service`

- **`--uninstall`** or **`-u`**:
  Uninstalls the `logcollector` service (Windows only).
  **Example**: `logcollector --uninstall`

- **`--help`** or **`-h`**:
  Prints the list of the options and exits without starting the service.
  **Example**: `logcollector --help`


### Console Commands

When running as a console application, the following commands are available to manage logging and connected clients:

- **`--save`** or **`-a`**: Start saving the collected logs in a database file. An optional file path may follow.
  **Example**: `--save` or `--save ./logs/session_%time%.sqlog`

- **`--unsave`** or **`-b`**: Stop saving the collected logs in the database file.
- **`--query`** or **`-e`**: Query the list of logging scopes.
  **Example**: `--query *`

- **`--config`** or **`-f`**: Save the current configuration, including logging scopes, to a file. *Not implemented yet.*
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

## Saving the Collected Logs in a Database

Besides forwarding the logs to the connected log observers, the `logcollector` can save them in an SQLite database file with the extension `.sqlog`. The file has the same structure as the database written by the log observer, so the tools that read one file also read the other.

The database stores:
- every collected log message, with the identity of the log source that produced it;
- the connect and disconnect time of every log source;
- the list of log scopes and their priorities, as registered or updated by the log sources.

### Enabling the Database

There are three ways to switch the database on. The command line wins over the configuration file.

| Way | Effect |
| --- | --- |
| `log::logcollector::enable::db = true` in `areg.init` | The database starts together with the service. |
| `logcollector --log=db [<path>]` | The database starts together with the service, independent of `areg.init`. |
| Console command `--save [<path>]` | The database starts on a running `logcollector`. `--unsave` stops it. |

To make sure the database stays off, start the collector with `--log=nodb`.

### Configuration Properties

The properties are read from the configuration file [`areg.init`](../areg/resources/areg.init):

```ini
log::logcollector::enable::db   = false                         # save the collected logs in the database
log::logcollector::db::engine   = sqlite3                       # the only supported engine
log::logcollector::db::name     = logcollector_%time%.sqlog     # file name, may contain masks
log::logcollector::db::location = ./logs                        # directory of the file
```

The file name and the directory accept the masks `%time%` (the current timestamp), `%appname%` (the process name) and `%user%` (the home directory of the account that runs the collector). Because the shipped name contains `%time%`, every start creates a new file and no previous file is overwritten. Missing directories are created automatically.

> 💡 The logs are written by a dedicated thread in batches, so that saving them does not slow down the forwarding of the logs to the log observers.

> ⚠️ Saving the *collected* logs in a plain text file is not supported. The property `log::logcollector::enable::file` controls only the own logs of the `logcollector` process.

For the complete description see the [Log Collector](../../docs/wiki/04d-logcollector.md) guide and the [Areg Configuration File Reference](../../docs/wiki/05b-areg-configuration-file.md).

---
