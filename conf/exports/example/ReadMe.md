### Project Overview
This `example` is a small project that demonstrates linking with the `areg` library. It also outputs the `AREG_SHARE_INIT` constant, which shows the location of the `areg.init` file template.

> **Note:** AREG requires a minimum of C++17.

---

### Build and Run Instructions

Follow the steps below to build and run the project:

#### **Step 1: Copy the Example**

Make a copy of the `example` directory to any location on your computer, and open a terminal in that directory.

#### **Step 2: Configure the Project**

In the terminal, run the following command, replacing `<vcpkg-root>` with the correct path to your vcpkg installation:

```bash
cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
```

If this command runs without errors, it means the `areg` package has been successfully installed.

#### **Step 3: Compile the Code**

Next, compile the code by running the following command in the terminal:

```bash
cmake --build ./build
```

If the compilation is successful and there are no errors, you have the correct C++ compiler (C++17 or higher), and the linking with the `areg` library was successful.

You can now run the `example` binary located in the `./build` directory to see the results.
