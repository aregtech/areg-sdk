```
This file is part of AREG SDK, the multi-tasking application development platform.
Copyright (c) Aregtech, 2017-2021
Contact: info[at]aregtech.com
```

This file describes the instruction to compile **AREG SDK** and example source codes in _Linux_ or any other _POSIX_ supporting 32- or 64-bit platform. The minimum requirement to run application built with the help of **AREG SDK** on the target device are 256KB of free RAM and 2MB of free storage space.

The **AREG SDK** sources can be compiled under Linux or any other POSIX supporting platform by using _Eclipse IDE_. Open _Eclipse IDE for C/C++_ and **export** one or all projects located in:
- **“areg-sdk/dev/areg/”** to build AREG connectivity framework
- **“areg-sdk/dev/mcrouter/”** to build multicast message router
- **”areg-sdk/examples/xxx”** to build examples

Select preferred toolchain to build project(s). By default, it uses _GNU GCC_ compoiler. **AREG SDK** sources can be compiled as static or shared library. By default, under Linux it is compiled as static library. Integrate **AREG** connectivity framework in your project(s) and make sure that is linked with _libareg.a_ (or _areg.so_) library.


To build **AREG SDK** connectivity framework, use **POSIX** (or **_POSIX**) preprocessor directive. 

**Other preprocessor directives:**
- **DEBUG** to compile debug version
- **NDEBUG** to compile release version
- **ENABLE_TRACES** to enable tracing / logging (requires additional space)
- **EXP_AREG_LIB** or **EXPORT_STATIC_SYMBOLS** to compile sources as static library
- **EXP_AREG_DLL** or **EXPORT_SHARED_SYMBOLS** to compile sources as shared library
- **IMP_AREG_LIB** or **IMPORT_STATIC_SYMBOLS** to link static library
- **IMP_AREG_DLL** or **IMPORT_SHARED_SYMBOLS** to link shared library

_**AREG SDK** requires POSIX1.c_ for multithreading and synchronization.

## The list of POSIX API used in AREG SDK (including multicast message router)

```
accept
access
bind
ceil
chdir
clock_gettime
close
connect
execl
fclose
FD_ISSET
FD_SET
FD_ZERO
fgets
floor
fopen
free
freeaddrinfo
fsync
ftruncate
getaddrinfo
getcwd
getenv
getpeername
getpid
getsockopt
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
mkstemp
mktime
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
rename
rmdir
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
strftime
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
wcstof
wcstol
wcstoll
wcstoul
wcstoull
write
```
Make sure that the target platform supports all of these functions and macro.
