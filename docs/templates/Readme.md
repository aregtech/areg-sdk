# Templates

This folder contains the temaplates that developer may use to create projects using prefered IDE. Regardless by using `cmake` it is possible to generate appropriate project files for prefered IDEs, sometimes it is more useful to create them manually.

1. `msvc-static-lib.vcxproj` and `msvc-static-lib.vcxproj.filter` files -- use these files to compile static library (for example, generated codes) under Microsoft Visual Studio.
2. `msvc-exe.vcxproj` and `msvc-exe.vcxproj.filter` files -- use these files to compile executables with Microsoft Visual Studio, which is linked (importing) with `areg` library. Pay attnetion on importing preprocessor define wheather the application is compiled with static or shared library.
3. `eclipse-areg.project` and `eclipse-areg.cproject` files -- use these files to compile `areg` library under `Eclipse`.
    * Copy file `eclipse-areg.project` to `framework/areg` folder and rename it to `.project`.
    * Copy file `eclipse-areg.cpproject` to `framework/areg` folder and rename it to `.cpproject`.
    * Open `Eclipse` IDE with the workspace in `areg-sdk` folder and import `areg` project.
    * Compile the project either as a static lbirary or shared (required additional changes in the project settings).
4. `eclipse-mtrouter.cproject` and `eclipse-mtrouter.project` files -- use these files to compile `mtrouter` service linked with `areg` library.
    * Copy file `eclipse-mtrouter.project` to `framework/mtrouter`folder and rename to `.project`.
    * Copy file `eclipse-mtrouter.cproject` to `framework/mtrouter`folder and rename to `.cproject`.
    * Open `Eclipse` IDE with the workspace in `areg-sdk` folder and import `mtrouter` project.
    * Compile the project linked with the `areg` library and other dependent libraries.
5. `eclipse-static.cproject` and `eclipse-static.project` files -- use these files to compile projects as static libraries (for example, generated codes).
    * Copy `eclipse-static.project` to the folder, which sources should be compiled as static libraries, and rename as `.project`.
    * Copy `eclipse-static.cproject` to the same folder and rename as `.cproject`.
    * Open the files and replace all `replace-me-static` with the name of folder / project.
    * Open `Eclipse` IDE and import projects.
    * Compile the projects.
6. `eclipse-executable.cproject` and `eclipse-executable.project` files -- use these files to create executable, which is linked with `areg` and (optional) other libraries (forexample, static library of generated codes).
    * Copy `eclipse-executable.project` into the folder, which sources should be compiled as executable, and rename as `.project`.
    * Replace all `replace-me-executable` with the name of the project. 
    * If the executable is linked with other library (static), replace `replace-me-static` with the name of library to link.
    * Copy `eclipse-executable.cproject` into the same folder and rename as `.cproject`
    * Replace all `replace-me-static` with the (optional) name of static library to link.
    * Replace all `replace-me-executable` with the name of executable.
    * Open `Eclipse` IDE and import the projects.
    * Make sure all symbols are imported and incluse folders are set properly.
    * Copile the projects.
