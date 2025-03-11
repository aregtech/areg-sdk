#ifndef AREG_SYSTEM_MINGW_GEMINGW_H
#define AREG_SYSTEM_MINGW_GEMINGW_H

/************************************************************************
* \file        areg/system/mingw/GEMinGW.h
* \ingroup     Framework globals
* \author      Artak Avetyan
* \brief       AREG Framework, platform dependent includes. MinGW specific directives
*
************************************************************************/
/************************************************************************
* Include files.
************************************************************************/

#if defined(__MINGW32__) || defined(__MINGW64__)

#ifndef MINGW
    #define MINGW   1
#endif  // MINGW

#ifndef _MINGW
    #define _MINGW  MINGW
#endif // !_MINGW

#ifdef WINDOWS
    #ifndef _WINDOWS
        #define _WINDOWS
    #endif // !_WINDOWS
#elif defined (_WINDOWS)
    #ifndef WINDOWS
        #define WINDOWS
    #endif // !WINDOWS
#endif // WINDOWS

#if defined(_WIN32)
    #ifndef WIN32
        #define WIN32   _WIN32
    #endif  // WIN32
#elif defined(WIN32)
    #ifndef _WIN32
        #define _WIN32  WIN32
    #endif  // _WIN32
#endif // (defined(_WINDOWS) || defined(WINDOWS))

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER                      // Specifies that the minimum required platform is Windows Vista.
    #define WINVER          0x0600  // Change this to the appropriate value to target other versions of Windows.
#endif  // WINVER

#ifndef _WIN32_WINNT                // Specifies that the minimum required platform is Windows Vista.
    #define _WIN32_WINNT    WINVER  // Change this to the appropriate value to target other versions of Windows.
#endif  // _WIN32_WINNT

#ifndef  WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif   // WIN32_LEAN_AND_MEAN

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

#ifndef _POSIX_THREADS
    #define _POSIX_THREADS
#endif // !_POSIX_THREADS

#ifndef _UCRT
    #define _UCRT
#endif  // _UCRT

#include <sys/types.h>

#ifdef   __THREAD_LOCAL
    #undef  __THREAD_LOCAL
#endif  // __THREAD_LOCAL

#define __THREAD_LOCAL __thread

#ifdef BIT64

    #if !defined(WIN64) || (WIN64 != 1)
        #undef  WIN64
        #define WIN64       1
    #endif  // !defined(WIN64) || (WIN64 != 1)

    #if !defined(_WIN64) || (_WIN64 != 1)
        #undef  _WIN64
        #define _WIN64       1
    #endif  // !defined(_WIN64) || (_WIN64 != 1)

#endif  // BIT64

#ifdef __STDC_LIB_EXT1__
    #ifndef __STDC_WANT_LIB_EXT1__
        #define __STDC_WANT_LIB_EXT1__      1
    #endif  // __STDC_WANT_LIB_EXT1__
#else   // __STDC_LIB_EXT1__
        #define __STDC_WANT_LIB_EXT1__      0
#endif  // __STDC_LIB_EXT1__

#endif  // defined(__MINGW32__) || defined(__MINGW64__)

#endif // AREG_SYSTEM_MINGW_GEMINGW_H
