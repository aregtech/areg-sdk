# Build with POSIX
```
This file is part of AREG SDK.
Copyright (c) Aregtech, 2021-2022
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document describes the instruction to compile **AREG SDK** and example source codes under _Linux_ or other _POSIX_ compatible 32- or 64-bit platform. The minimum requirement to run an application built with the AREG SDK on the target device are 256KB of free RAM and 2MB of free storage space.

The AREG SDK sources can be compiled under Linux or any other POSIX platform by using `cmake`, `make` or `Eclipse IDE`. 

---

## Compile with CMake in 4 steps

In the opened _Terminal_ make following steps to compile with `cmake`:
```bash
# Step 1: Get sources from GitHub
$ git clone https://github.com/aregtech/areg-sdk.git
$ cd areg-sdk

# Step 2: Create a subdirectory for CMake cache files and change directory to it.
$ mkdir build && cd build

# Step 3: Initialize CMake cache and build systems configuration.
#         Enable examples compilation by using BUILD_EXAMPLES flag.
$ cmake .. -DBUILD_EXAMPLES=ON

# Step 4: Compile sources by calling: cmake --build [CMake cache dir] <optional> -j [concurrent jobs]
$ cmake --build . -j 8
 ```
After compilation you'll have 2 new folders: `build` and `product`. The compiled binaries are located in the new created folder `product`. The path to binaries depends on compile, platform and build configuration. It will look like `product/build/g++/linux-x86_64-debug/bin`.

---

## Compile with Make in 2 steps

In the opened _Terminal_ make following steps to compile with `cmake`:
```bash
# Step 1: Get sources from GitHub
$ git clone https://github.com/aregtech/areg-sdk.git
$ cd areg-sdk

# Step 2: Compile sources from terminal by calling: make [all] [framework] [examples]
$ make
 ```
After compilation you'll have 1 new folder `product`. The compiled binaries are located in the new created folder `product`. The path to binaries depends on compile, platform and build configuration. It will look like `product/build/g++/linux-x86_64-debug/bin`.

---

## Compile with Eclipse IDE

Clone the source codes from `areg-sdk` reponsitory:
```bash
$ git clone https://github.com/aregtech/areg-sdk.git
$ cd areg-sdk
```
Open _Eclipse IDE for C/C++_ and **import** projects located in:
- `<areg-sdk-root>/framework/areg/` to build the AREG framework.
- `<areg-sdk-root>/framework/mcrouter/` to build the multicast router.
- `<areg-sdk-root>/examples/` to build all examples.

By default, _Eclipse_ will compile with _GNU GCC_ compiler. If you prefer other toolchain, for each project select settings and change to compile with preferred toolchain.

## Compile areg framework library

The sources of AREG framework can be compiled as a static or a shared library. By default, it is a static library. Integrate AREG framework in your project(s) and make sure that you link with _areg_ library. The [HOWTO.md](./HOWTO.md) document contains detailed description [to import Eclipse projects](./HOWTO.md#eclipse-for-cc-developer) and [to integrate AREG framework](./HOWTO.md#how-to-create-a-project-or-integrate-in-project) in the existing or new project.

To build AREG framework with _POSIX_ API, define **POSIX** (or **_POSIX**) preprocessor directive. 

**Other preprocessor directives used to compile:**
| Directive | Description |
| --- | --- |
| **DEBUG** | Compile and build debug configuration. |
| **NDEBUG** | Compile and build release configuration. |
| **ENABLE_TRACES** | Compile with enabled logging. |
| **EXP_AREG_LIB** | Compile and build the AREG framework as a _static_ library. |
| **EXP_AREG_DLL** | Compile and build the AREG framework as a _shared_ library. |
| **IMP_AREG_LIB** | Link your project with the AREG framework _static_ library. |
| **IMP_AREG_DLL** | Link a project with the AREG framework _shared_ library. |

AREG SDK requires _POSIX1.c_ API for multithreading and synchronization. The AREG framework beside standard runtime library, uses `pthread` and `ncurses`.

---

## POSIX methods

The list of POSIX methods and macro used in AREG framework (including multicast message router)

```
accept
access
bind
cbreak
ceil
chdir
clear
clock_gettime
close
connect
execl
delwin
endwin
fclose
FD_ISSET
FD_SET
FD_ZERO
fflush
fgets
floor
fopen
fprintf
free
freeaddrinfo
fsync
ftruncate
getaddrinfo
getcwd
getenv
getpeername
getpid
getpwuid
getsockopt
getyx
gmtime
gmtime_r
htons
inet_addr
inet_ntoa
inet_ntop
ioctl
isalnum
listen
localtime
localtime_r
lseek
mkdir
mkdtemp
mktime
mvwaddstr
nanosleep
ntohs
open
opendir
poll
printf
pthread_attr_destroy
pthread_attr_init
pthread_attr_setdetachstate
pthread_cancel
pthread_cond_destroy
pthread_cond_init
pthread_cond_signal
pthread_cond_timedwait
pthread_cond_wait
pthread_condattr_destroy
pthread_condattr_init
pthread_condattr_setpshared
pthread_create
pthread_mutex_destroy
pthread_mutex_init
pthread_mutex_lock
pthread_mutex_timedlock
pthread_mutex_trylock
pthread_mutex_unlock
pthread_mutexattr_destroy
pthread_mutexattr_init
pthread_mutexattr_settype
pthread_self
pthread_setcancelstate
pthread_setcanceltype
pthread_setname_np (Linux)
pthread_setschedparam
pthread_spin_destroy
pthread_spin_init
pthread_spin_lock
pthread_spin_trylock
pthread_spin_unlock
read
readdir
realpath
recv
refresh
rename
rmdir
S_ISDIR
scanf
sched_get_priority_max
sched_get_priority_min
select
send
setsockopt
shutdown
signal
snprintf
socket
sprintf
stat
strcpy
strftime
strlen
strtod
strtof
strtol
strtoll
strtoul
strtoull
swprintf
timer_create
timer_delete
timer_settime
unlink
va_end
va_start
vsnprintf
vw_scanw
waddstr
wclrtoeol
wcstof
wcstol
wcstoll
wcstoul
wcstoull
write
wmove
wrefresh
wscanw
```
Make sure that the target platform supports all of these functions and macro.
