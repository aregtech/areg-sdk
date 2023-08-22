# Content

This folder contains the AREG extended library that can is compiled with or without extended features, which may require additional libraries and resources, which are considered additional and optional features. The extensions are compiled as `areg-extend` static library to link with projects that may use them. To compile project with the additional features, define `AREG_EXTENDED=1`. Otherwise, ignore or specify `AREG_EXTENDED=0`. If specify `AREG_EXTENDED=0`, the project is compiled as a static library, which can be linked, but no guarantee that all extended features will be available or will work as it is initially designed.

This project contains following folders:

* [console](./console)      -- The console extension to output messages on the certain positions and manipulate the cursor positions. For Linux platform it can be compiled with `ncurses` library to have better manipulation of cursors. For Windows platform it can be compiled with `Win32 API` to have better manipulation of cursor. If `AREG_EXTENDED=0` is set, the implementation is based on ANSI C API and ANSI escape code sequences. The behavior may differ in multithreading environment when simultaneously wait for user input and query the cursor position. Be careful to query position. Instead, try saving and restoring cursor position.
