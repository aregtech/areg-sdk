# Troubleshooting CMake Builds on Linux

This guide provides solutions for common issues encountered when compiling projects with CMake on Linux, helping to ensure a smooth and successful build process.

---

## Table of Contents

- [Missing `<asm/errno.h>` File](#missing-asmerrnoh-file)
   - [Solution: Create a Symbolic Link](#solution-create-a-symbolic-link)

---

## Missing `<asm/errno.h>` File

If you receive an error during compilation indicating that `<asm/errno.h>` could not be found, it often means certain header files are missing or not correctly linked in the system’s include directories.

### Solution: Create a Symbolic Link

> [!NOTE]  
> You will need administrator (root) permissions for this solution.

To address this issue, create a symbolic link from the `asm-generic` directory to the `asm` directory in `/usr/include`. This enables the compiler to locate the required header files.

1. **Open Terminal**: Launch your terminal.
2. **Run the Command**:
   ```bash
   sudo ln -s /usr/include/asm-generic/ /usr/include/asm
   ```
   This command creates a symbolic link called `asm` in `/usr/include`, pointing to the `asm-generic` directory.

3. **Retry Compilation**: After creating the symbolic link, rerun the compilation command. This should resolve the error.

> [!TIP]  
> Alternatively, the `asm/errno.h` header file is included in some system packages. You can try installing it by running:
> ```bash
> sudo apt-get install -y linux-libc-dev
> ```