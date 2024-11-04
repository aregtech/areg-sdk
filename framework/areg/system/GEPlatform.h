#ifndef AREG_SYSTEM_GEPLATFORM_H
#define AREG_SYSTEM_GEPLATFORM_H
/************************************************************************
 * \file        areg/system/GEPlatform.h
 * \ingroup     platform dependent files
 * \author      Artak Avetyan
 * \brief       AREG Framework, platform dependent includes
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

/************************************************************************
 * \brief   This header file is included internally as a wrapper for 
            system dependent header files 
 ************************************************************************/

#if !defined(DEBUG) && !defined(NDEBUG)
    #define DEBUG
#endif // !defined(DEBUG) && !defined(NDEBUG)

#if defined(DEBUG)
    #undef  NDEBUG
    #undef  _NDEBUG
    #ifndef _DEBUG
        #define _DEBUG
    #endif // !_DEBUG
#endif // DEBUG

#if defined(NDEBUG)
    #undef  DEBUG
    #undef _DEBUG
    #ifndef _NDEBUG
        #define _NDEBUG
    #endif // !_NDEBUG
#endif // NDEBUG


#if (defined(_POSIX) || defined(POSIX))

    #undef WINDOWS
    #undef _WINDOWS

    #include "areg/system/posix/GEPosix.h"

#elif (defined(_WINDOWS) || defined(WINDOWS))

    #include "areg/system/windows/GEWindows.h"

#elif (defined(__unix__) || defined(__linux__) || defined(__CYGWIN__) || defined(__FreeBSD__) || defined(__APPLE__))

    #define POSIX
    #include "areg/system/posix/GEPosix.h"

#elif(defined(_WIN32) || defined(_WIN64))

    #define WINDOWS
    #include "areg/system/windows/GEWindows.h"

#else // !(defined(_WINDOWS) || defined(WINDOWS))

    #error  Unsupported OS.

#endif // (defined(_WINDOWS) || defined(WINDOWS))

#endif // AREG_GEPLATFORM_H
