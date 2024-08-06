#ifndef AREG_SYSTEM_POSIX_GEPOSIX_H
#define AREG_SYSTEM_POSIX_GEPOSIX_H

/************************************************************************
* \file        system/posix/GEPosix.h
* \ingroup     Framework globals
* \author      Artak Avetyan
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

#ifndef	_POSIX_C_SOURCE
    #define _POSIX_C_SOURCE  200809L
#endif	// _POSIX_C_SOURCE

#ifndef _DEFAULT_SOURCE
    #define _DEFAULT_SOURCE
#endif	// _DEFAULT_SOURCE

#if defined(BIT32) || defined(_BIT32)

    #ifndef BIT32
        #define BIT32
    #endif // !BIT32

    #ifndef _BIT32
        #define _BIT32
    #endif // !_BIT32

#elif defined(BIT64) || defined(_BIT64)

    #ifndef BIT64
        #define BIT64
    #endif // !BIT64

    #ifndef _BIT64
        #define _BIT64
    #endif // !_BIT64

#else // defined(BIT64) || defined(_BIT64)

    #if defined(__GNUC__) && (__GNUC__)
        #if __x86_64__ || __ppc64__
            #define BIT64
            #define _BIT64
        #else
            #define BIT32
            #define _BIT32
        #endif
    #endif  // __GNUC__

#endif  // defined(BIT32) || defined(_BIT32)

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

// enable using safe POSIX calls.
#ifdef __STDC_LIB_EXT1__
    #ifndef __STDC_WANT_LIB_EXT1__
        #define __STDC_WANT_LIB_EXT1__      1
    #endif  // __STDC_WANT_LIB_EXT1__
#else   // __STDC_LIB_EXT1__
        #define __STDC_WANT_LIB_EXT1__      0
#endif  // __STDC_LIB_EXT1__

#endif // AREG_SYSTEM_POSIX_GEPOSIX_H
