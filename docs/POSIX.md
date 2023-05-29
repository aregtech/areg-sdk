```
This file is part of AREG SDK.
Copyright (c) Aregtech, 2021-2022
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

# POSIX API

In order to build the AREG framework with the *POSIX API*, it is necessary to define the preprocessor symbol **POSIX**. Detailed information regarding the `POSIX` preprocessor symbol can be found in the [Definition of `POSIX` Preprocessor Define](https://github.com/aregtech/areg-sdk/wiki/02.-Preprocessor-define-symbols#posix-define) Wiki page of the AREG SDK. To utilize the POSIX API in the software development process, please consult the [Software build](https://github.com/aregtech/areg-sdk/wiki/02.-Software-build) Wiki page of the AREG SDK.

For proper functioning, the AREG SDK relies on the _POSIX1.c_ API for multithreading and synchronization. In addition to the standard runtime library, the AREG framework incorporates the use of `pthread` and, if the `AREG_EXTENDED` option is enabled during compilation, it also utilizes `ncurses`. To illustrate, please consider the following example command sequence:

```bash
cmake -B ./build -DAREG_EXTENDED:BOOL=ON
cmake --build build
```

By executing the above commands, the AREG framework will be built, taking into account the extended functionality provided by `ncurses` if the `AREG_EXTENDED` option is enabled.

**The list of POSIX API functions and macro used in AREG SDK (including multicast message router):**

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
