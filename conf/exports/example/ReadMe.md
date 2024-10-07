This `example` is a small project to check the linking with 'areg' library.
It as well outputs the 'AREG_SHARE_INIT' constant, to show the location of 'areg.init' file template.

**Important:** AREG requires minimum C++17.

To build and run the project, performa following steps:

**Step 1: Copy example** 
- Make a copy of the 'example' directory to any place and open there terminal.

**Step 2: Configure the example** 
- In the terminal run the command, but replace the `<vcpkg-root>` and specifying the path in your computer: 
```bash
    cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
```
If the command above runs without error, you have properly installed the `areg` package.

**Step 3: Compile the code**
- In the terminal run the command to compile the code:
```bash
    cmake --build ./build
```

If compiled without error, you have the correct C++ compiler (minimum C++17) and succeeded to link with 'areg' library.

To see the result, run binary: `./build/<Configuration>/example.exe`. Where `<Configuration>` is the value of `CMAKE_BUILD_TYPE` (normally, `Debug` or `Release`).
