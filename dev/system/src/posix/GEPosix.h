#ifndef AREG_SYSTEM_POSIX_GEPOSIX_H
#define AREG_SYSTEM_POSIX_GEPOSIX_H

/************************************************************************
* \file        system/src/posix/GEPosix.h
* \ingroup     Framework globals
* \author      Artak Avetyan (mailto:artak@aregtech.com)
* \brief       AREG Framework, platform dependent includes. POSIX specific directives
*
************************************************************************/
/************************************************************************
* Include files.
************************************************************************/

#ifdef _POSIX
    #ifndef POSIX
        #define POSIX   _POSIX
    #endif  // POSIX
#endif // _POSIX

#ifdef POSIX
    #ifndef _POSIX
        #define _POSIX  POSIX
    #endif  // _POSIX
#endif  // POSIX

#if __GNUC__
#if __x86_64__ || __ppc64__
    #undef BIT32
    #undef BIT64
    #undef _BIT32
    #undef _BIT64

    #define BIT64
    #define _BIT64
#else
    #undef BIT32
    #undef BIT64
    #undef _BIT32
    #undef _BIT64

    #define BIT32
    #define _BIT32
#endif
#endif  // __GNUC__
#if defined(POSIX) || defined(_POSIX)

    #ifndef _POSIX_THREADS
        #define _POSIX_THREADS
    #endif // !_POSIX_THREADS

    #ifndef _POSIX_TIMERS
        #define _POSIX_TIMERS
    #endif  // _POSIX_TIMERS

    #include <sys/types.h>

    #ifdef   __THREAD_LOCAL
        #undef  __THREAD_LOCAL
    #endif  // __THREAD_LOCAL

    #define __THREAD_LOCAL __thread

#endif // defined(POSIX) || defined(_POSIX)

#endif // AREG_SYSTEM_POSIX_GEPOSIX_H
