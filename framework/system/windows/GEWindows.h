#ifndef AREG_SYSTEM_WINDOWS_GEWINDOWS_H
#define AREG_SYSTEM_WINDOWS_GEWINDOWS_H

/************************************************************************
 * \file        system/windows/GEWindows.h
 * \ingroup     Framework globals
 * \author      Artak Avetyan
 * \brief       AREG Framework, platform dependent includes. Win32 specific directives
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#if (defined(_WINDOWS) || defined(WINDOWS))

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

#if defined(BIT64) || defined(_BIT64) || defined(WIN64) || defined(_WIN64)

    #if defined(BIT32) || defined(_BIT32)
        #undef  BIT32
        #undef  _BIT32
    #endif // defined(BIT32) || defined(_BIT32)

    #ifndef BIT64
        #define BIT64
    #endif // !BIT64

    #ifndef _BIT64
        #define _BIT64
    #endif // !_BIT64

    #if !defined(WIN64) || (WIN64 != 1)
        #undef  WIN64
        #define WIN64       1
    #endif  // !defined(WIN64) || (WIN64 != 1)

    #if !defined(_WIN64) || (_WIN64 != 1)
        #undef  _WIN64
        #define _WIN64       1
    #endif  // !defined(_WIN64) || (_WIN64 != 1)

#elif defined(BIT32) || defined(_BIT32)

    #ifndef BIT32
        #define BIT32
    #endif // !BIT32

    #ifndef _BIT32
        #define _BIT32
    #endif // !_BIT32

#elif defined(_M_X64) || defined(_Wp64) || defined(_M_AMD64) || defined(__x86_64__)

    #ifndef BIT64
        #define BIT64
    #endif // !BIT64

    #ifndef _BIT64
        #define _BIT64
    #endif // !_BIT64

    #if !defined(WIN64) || (WIN64 != 1)
        #undef  WIN64
        #define WIN64       1
    #endif  // !defined(WIN64) || (WIN64 != 1)

    #if !defined(_WIN64) || (_WIN64 != 1)
        #undef  _WIN64
        #define _WIN64       1
    #endif  // !defined(_WIN64) || (_WIN64 != 1)

#elif defined(_M_IX86) || defined(_M_IX86_FP) || defined(__i386__)

    #ifndef BIT32
        #define BIT32
    #endif // !BIT32

    #ifndef _BIT32
        #define _BIT32
    #endif // !_BIT32

#else   // defined(_M_IX86) || defined(_M_IX86_FP) || defined(__i386__)

    #error  Cannot detect the target bitness

#endif  // defined(BIT64) || defined(_BIT64) || defined(WIN64) || defined(_WIN64)

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER                      // Specifies that the minimum required platform is Windows Vista.
    #define WINVER          0x0600  // Change this to the appropriate value to target other versions of Windows.
#endif  // WINVER

#ifndef _WIN32_WINNT                // Specifies that the minimum required platform is Windows Vista.
    #define _WIN32_WINNT    WINVER  // Change this to the appropriate value to target other versions of Windows.
#endif  // _WIN32_WINNT

/**
 * \brief   Switcher of MS Visual C++
 **/
#ifdef _MSC_VER
    #ifndef MS_VISUAL_CPP
        #define MS_VISUAL_CPP
    #endif  // MS_VISUAL_CPP
#endif  // _MSC_VER


#ifndef  WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif   // WIN32_LEAN_AND_MEAN

/************************************************************************
 * common defines
 ************************************************************************/
#if (defined(_WIN32) && !defined(__THREAD_LOCAL))
    #define __THREAD_LOCAL  __declspec( thread )
#else   // !(defined(_WIN32) && !defined(__THREAD_LOCAL))
    #define __THREAD_LOCAL 
#endif  // !(defined(_WIN32) && !defined(__THREAD_LOCAL))

#endif  // defined(_WINDOWS) || defined(WINDWOS)

#endif // AREG_SYSTEM_WINDOWS_GEWINDOWS_H
