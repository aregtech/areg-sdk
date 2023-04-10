# Content

This folder contains the source codes of Multicast Router (`mcrouter`) used to create a network of connected Things and contains following folders:

* [app](./app)              -- The source codes to start application. Under Windows OS it may start as console application or system service.
* [resource](./resource)    -- The folder with resource files of `mcrouter`
* [tcp](./tcp)              -- The folder with source codes of TCP/IP connection of `mcrouter`.

The `mcrouter` runs as a console application under Linux. It may be compiler with and without `ncurses` library. To compiler with `ncurses`, use extensions of the AREG engine and specify `AREG_ENABLE_EXT=1`. To compile without `ncurses` and use only ANSI C API, ignore `AREG_ENABLE_EXT` or set `AREG_ENABLE_EXT=0`.

The `mcrouter` runs as a system service or console application under Windows OS. It may be compiler with and without `Win32 API`. To compiler with `Win32 API`, use extensions of the AREG engine and specify `AREG_ENABLE_EXT=1`. To compile without `Win32 API` and use only ANSI C API, ignore `AREG_ENABLE_EXT` or set `AREG_ENABLE_EXT=0`.
