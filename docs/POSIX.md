# Build with POSIX API
```
This file is part of AREG SDK.
Copyright (c) Aregtech, 2021-2022
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document describes the instruction to compile **AREG SDK** and example source codes under _Linux_ or any other POSIX_ 32- or 64-bit platform. The minimum requirement to run an application built with the AREG SDK on the target device are 256KB of free RAM and 2MB of free storage space.

The AREG SDK sources can be compiled under Linux or any other POSIX platform by using _Eclipse IDE_. Open _Eclipse IDE for C/C++_ and **import** one or all projects located in:
- `<areg-sdk-root>/framework/areg/` to build the AREG framework.
- `<areg-sdk-root>/framework/mcrouter/` to build the multicast router.
- `<areg-sdk-root>/examples/xxx` to build examples.

Select preferred toolchain to build project(s). By default, it is set _GNU GCC_ compile with the GCC compiler under Linux. AREG SDK sources can be compiled as a static or a shared library. By default, it is a static library. Integrate AREG framework in your project(s) and make sure that you link with _areg_ library. The [HOWTO.md](./HOWTO.md) document contains detailed description [to import Eclipse projects](./HOWTO.md#eclipse-for-cc-developer) and [to integrate AREG framework](./HOWTO.md#how-to-create-a-project-or-integrate-in-project) in the existing or new project.

To build AREG framework with _POSIX_ API, define **POSIX** (or **_POSIX**) preprocessor directive. 

**Other preprocessor directives to compile:**
- **DEBUG** to compile with debug configuration.
- **NDEBUG** to compile with release configuration.
- **ENABLE_TRACES** to compile with enabled logging.
- **EXP_AREG_LIB** to compile the AREG framework as a _static_ library.
- **EXP_AREG_DLL** to compile the AREG framework as a _shared_ library.
- **IMP_AREG_LIB** to link a project with the AREG framework _static_ library.
- **IMP_AREG_DLL** to link a project with the AREG framework _shared_ library.

AREG SDK requires _POSIX1.c_ API for multithreading and synchronization. The AREG framework beside standard runtime library, uses `pthread` and `ncurses`.

## The list of POSIX API and macro used in AREG framework (including multicast message router)

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
