```
This file is part of AREG SDK.
Copyright (c) Aregtech, 2021-2023
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

# POSIX API

To build the AREG framework using the *POSIX API*, it is necessary to define the preprocessor symbol **POSIX**. Detailed information about the `POSIX` preprocessor symbol can be found in the [`POSIX` define](https://github.com/aregtech/areg-sdk/wiki/02.-Preprocessor-define-symbols#posix-define) page of the AREG SDK Wiki. To utilize the POSIX API in your software development process, please refer to the [Software build](https://github.com/aregtech/areg-sdk/wiki/02.-Software-build) page of the AREG SDK Wiki.

The AREG SDK relies on the _POSIX1.c_ API for multithreading and synchronization. Along with the standard runtime library, the AREG framework incorporates the use of `pthread`. If the `AREG_EXTENDED` option is enabled during compilation, it also utilizes `ncurses`. To demonstrate, consider the following example commands:

```bash
cmake -B ./build -DAREG_EXTENDED:BOOL=ON
cmake --build ./build
```

Executing the above commands will build the AREG framework, taking into account the extended functionality provided by `ncurses` when the `AREG_EXTENDED` option is enabled.

**The list of POSIX API functions and macro used in AREG SDK (including Multitarget message router):**

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
gets_s
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
wgetnstr
```

Make sure that the target platform supports all of these functions and macro.
