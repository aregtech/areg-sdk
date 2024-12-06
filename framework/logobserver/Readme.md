# Directory Overview: AREG Log Observer Application

This directory contains the source code for the AREG Log Observer (`logobserver`), a console application designed to manage and analyze logs in real time. It allows for dynamic handling of log scopes and priorities during runtime. The received logs are stored in an SQLite database, enabling detailed post-analysis of application behavior and performance.

### Directory Contents

- **[app](./app)**
  Contains initialization code for the `logobserver`, enabling it to function as a console application.

- **[resources](./resources)**
  Includes resources specific to the `logobserver` application.

### Platform Support and Build Configurations

- **Linux**:
  The `logobserver` runs as a console or OS-managed service application, and can be built with or without the `ncurses` library:
  - To enable `ncurses` features, compile the AREG extended static library by defining setting option `-DAREG_EXTENDED:BOOL=ON`.
  - To disable `ncurses`, omit `AREG_EXTENDED` option or set it to `OFF`, using only the ANSI C API.

- **Windows**:
  The `logobserver` operates as a console application or OS-managed service, and can be built with or without the `Win32 API` for extended feature:
  - To enable `Win32 API` extended features, define `AREG_EXTENDED=1`.
  - To use only the ANSI C API, omit `AREG_EXTENDED` or set it to `0`.

> [!NOTE]
> It does not make sense to enable extended features if application runs as an OS-managed service, because extended features manage cursor position on console, which is not accessible when application runs as a service. To save resources and linking time, disable extended features in the build by setting option `-DAREG_EXTENDED:BOOL=OFF`

---

## Usage of AREG Log Observer (`logobserver`)

The `logobserver` application supports various commands for configuring and managing logging operations, offering flexibility for diverse use cases.


### Command-Line Options

Configure the `logobserver` using the following command-line option:

- **`--load=<path>`** or **`-l`**:
  Loads configuration from the specified initialization file.
  **Example**: `./logobserver --load=/path/to/config.file`


### Console Commands

When running as a console application, the following commands can be used to manage logging and connected instances:

- **`--query`** or **`-e`**:
  Query the list of logging scopes.
  **Example**: `--query *` (where `*` can be a cookie ID)

- **`--config`** or **`-f`**:
  Save the current configuration, including log scopes and priorities.

- **`--help`** or **`-h`**:
  Display a help message summarizing available commands.

- **`--instances`** or **`-n`**:
  Show a list of connected log instances.

- **`--scope`** or **`-o`**:
  Update log scope priorities.
  **Example**: `--scope *::areg_base_NESocket=NOTSET` (where `*` can be a cookie ID)

- **`--pause`** or **`-p`**:
  Pause log observation.

- **`--quit`** or **`-q`**:
  Stop and exit the `logobserver`.

- **`--restart`** or **`-r`**:
  Resume or restart log observation.

- **`--stop`** or **`-x`**:
  Halt log observation without exiting the application.

---
