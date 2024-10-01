# SQLite sources

These are _amalgamation_ sources of the SQLite database, which are taken from the [_Download_](https://sqlite.org/download.html) page of the [SQLite](https://sqlite.org/) site and not owned by Aregtech. For more information about the SQLite and licenses, please visit the official web site.

## Using SQLite package

Example for 64-bit Windows
```bash

vcpkg> ./vcpkg install sqlite3:x64-windows
vcpkg> ./vcpkg integrate install
vcpkg> cd areg-sdk
areg-sdk> cmake -B ./build -DAREG_SQLITE_PACKAGE=ON -DCMAKE_TOOLCHAIN_FILE=C:/DevTools/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows

```
