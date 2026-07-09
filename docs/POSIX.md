# POSIX API Reference

```
This file is part of Areg SDK
Copyright (c) Aregtech (Artak Avetyan), 2021-2026
Contact: info[at]areg.tech
Website: https://www.areg.tech
```

This document describes POSIX API usage in Areg SDK for Linux and macOS platforms.

---

## Building with POSIX

To build with the POSIX API, define the preprocessor symbol **POSIX**. See the [Preprocessor Definitions](./wiki/02f-preprocessor-definitions.md) guide for details.

### Dependencies

Areg SDK uses POSIX.1c for multithreading and synchronization:

| Library | Purpose |
|---------|---------|
| `pthread` | Threading and synchronization |
| `ncurses` | Extended console features (optional) |

### Build Example

```bash
# Standard build
cmake -B ./build
cmake --build ./build

# Build with extended features (ncurses)
cmake -B ./build -DAREG_EXTENDED:BOOL=ON
cmake --build ./build
```

When `AREG_EXTENDED` is enabled, the build links against `ncurses` for enhanced console functionality.

---

## Platform-Specific Notes

### Linux

- Uses `pthread_setname_np` for thread naming (Linux-specific)
- Supports `timer_create`/`timer_settime` for high-resolution timers
- System services managed by systemd

### macOS

- Uses `dispatch_source` for timers instead of `timer_create`
- Thread naming via `pthread_setname_np` (macOS variant)
- System services managed by launchd
- Links against `libc++` (not `libstdc++`)

---

## POSIX API Functions Used

The following POSIX functions and macros are used in Areg SDK (including the Multitarget Router):

### File and Directory Operations

```
access      chdir       close       fclose      fopen       fprintf
fsync       ftruncate   getcwd      lseek       mkdir       mkdtemp
open        opendir     read        readdir     realpath    rename
rmdir       stat        S_ISDIR     unlink      write
```

### Socket Operations

```
accept      bind        connect     freeaddrinfo    getaddrinfo
getpeername getsockopt  htons       inet_addr       inet_ntoa
inet_ntop   ioctl       listen      ntohs           poll
recv        select      send        setsockopt      shutdown
socket
```

### Threading (pthread)

```
pthread_attr_destroy        pthread_attr_init
pthread_attr_setdetachstate pthread_cancel
pthread_cond_destroy        pthread_cond_init
pthread_cond_signal         pthread_cond_timedwait
pthread_cond_wait           pthread_condattr_destroy
pthread_condattr_init       pthread_condattr_setpshared
pthread_create              pthread_mutex_destroy
pthread_mutex_init          pthread_mutex_lock
pthread_mutex_timedlock     pthread_mutex_trylock
pthread_mutex_unlock        pthread_mutexattr_destroy
pthread_mutexattr_init      pthread_mutexattr_settype
pthread_self                pthread_setcancelstate
pthread_setcanceltype       pthread_setname_np
pthread_setschedparam       pthread_spin_destroy
pthread_spin_init           pthread_spin_lock
pthread_spin_trylock        pthread_spin_unlock
```

### Time and Scheduling

```
ceil                clock_gettime       floor
gmtime              gmtime_r            localtime
localtime_r         mktime              nanosleep
sched_get_priority_max                  sched_get_priority_min
strftime            timer_create        timer_delete
timer_settime
```

### String and Memory

```
fflush      fgets       free        getenv      gets_s
isalnum     printf      snprintf    sprintf     strcpy
strlen      strtod      strtof      strtol      strtoll
strtoul     strtoull    swprintf    va_end      va_start
vsnprintf   wcstof      wcstol      wcstoll     wcstoul
wcstoull
```

### Process and User

```
execl       getpid      getpwuid    signal
```

### ncurses (Extended Features)

```
cbreak      clear       delwin      endwin      getyx
mvwaddstr   refresh     vw_scanw    waddstr     wclrtoeol
wgetnstr    wmove       wrefresh
```

### File Descriptors

```
FD_ISSET    FD_SET      FD_ZERO
```

---

## Platform Requirements

Ensure your target platform supports all listed functions. Most modern Linux distributions and macOS versions include full support. For embedded or minimal systems, verify availability of:

- pthread library
- Socket support
- Timer functions (or use macOS dispatch alternatives)
