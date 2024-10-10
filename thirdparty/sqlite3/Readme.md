# SQLite Sources in AREG SDK

The AREG SDK uses SQLite to save log files in both the `logobserver` executable and the `areglogger` library. SQLite is used to create the database file and save logs. You can link SQLite in your application in two ways:

1. Build a **static** library from the SQLite source provided in the AREG SDK.
2. Use an SQLite package already available in your system.

By default, the AREG SDK will attempt to compile with the SQLite3 package found in the system.

---

## Using SQLite Source

The `sqlite3/amalgamation` directory contains the SQLite source code, which is downloaded from the [SQLite Download Page](https://sqlite.org/download.html). These sources are not owned by Aregtech. For more details on SQLite and its licenses, refer to the [official SQLite website](https://sqlite.org/).

If no SQLite3 package is found during the build process, the SDK automatically builds the static `sqlite3` library. However, you can force the use of the SQLite source by setting the `AREG_SQLITE_PACKAGE=OFF` compiler option.

- By default, `AREG_SQLITE_PACKAGE` is set to `ON`, meaning CMake will try to locate the SQLite3 package in your system. 
- If the package is not found, the SDK will build and link against the static `sqlite3` library from the provided sources.

---

## Using SQLite Package

Alternatively, you can link to a pre-installed SQLite package available in your system or through [vcpkg](https://github.com/microsoft/vcpkg).

### Using vcpkg

To use `vcpkg` for installing SQLite3:

1. Ensure SQLite3 is installed through `vcpkg`.
2. Set the correct path to the `vcpkg.cmake` file in the `CMAKE_TOOLCHAIN_FILE` option.

If the path to `vcpkg.cmake` is unknown, you can retrieve it by running:

```bash
./vcpkg integrate install
```

Below is a five-step guide for installing `vcpkg` and the SQLite3 binary to use with AREG SDK.

### Installation Guide

**Step 1: Install vcpkg**

Follow the [vcpkg official instructions](https://github.com/microsoft/vcpkg) to install `vcpkg`. Basic steps:

1. Clone the vcpkg repository:
   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   ```
2. Navigate to the `vcpkg` directory and run the bootstrap script:
   - On Linux:
     ```bash
     ./bootstrap-vcpkg.sh
     ```
   - On Windows:
     ```bash
     ./bootstrap-vcpkg.bat
     ```

> [!NOTE]
> Additional dependencies may be required during the process. Follow any prompts.

For further details, refer to Microsoft's [tutorial on using vcpkg with CMake](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell).

**Step 2: Install SQLite3**

- **For Windows (64-bit)**:
  ```bash
  ./vcpkg install sqlite3:x64-windows
  ```
- **For Linux (64-bit)**:
  ```bash
  ./vcpkg install sqlite3:x64-linux
  ```

> [!NOTE]
> For 32-bit systems, replace `x64` with `x86` in the command (`sqlite3:x86-windows` or `sqlite3:x86-linux`).

> [!IMPORTANT]
> You may be prompted to install additional dependencies during the package installation. Follow the recommendations provided.

**Step 3: Integrate vcpkg**

After installing the package, integrate it by running:
```bash
./vcpkg integrate install
```

**Step 4: Use the Toolchain File**

Once integration is complete, a message will appear with the `CMAKE_TOOLCHAIN_FILE` path. It should look like this:
```txt
CMake projects should use: "-DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
```
Save this path for later use.

Optionally, set the `VCPKG_ROOT` environment variable to point to the `vcpkg` directory, and include it in your `PATH` for easier access.

**Step 5: Build AREG SDK with SQLite3**

To build the AREG SDK with the installed SQLite3 package, use the following CMake options (example for Windows):
```bash
cmake -B ./build -DAREG_SQLITE_PACKAGE=ON -DCMAKE_TOOLCHAIN_FILE="$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows
```

If the SQLite3 package is found, CMake will display an output similar to:
```txt
-- Found SQLite3: /path/to/vcpkg/installed/x64-linux/include (found version "3.46.1")
```

In this case, CMake will link to the existing SQLite3 library instead of building a new one.
