#ifndef AREG_SYSTEM_GEPLATFORM_H
#define AREG_SYSTEM_GEPLATFORM_H
/************************************************************************
 * \file        system/src/GEPlatform.h
 * \ingroup     platform dependent files
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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

#if defined(DEBUG)
    #undef  NDEBUG
    #undef _NDEBUG
    
    #ifndef _DEBUG
        #define _DEBUG
    #endif // !_DEBUG
#elif defined(_DEBUG)
    #undef  NDEBUG
    #undef _NDEBUG

    #ifndef DEBUG
        #define DEBUG
    #endif // !DEBUG
#elif defined(NDEBUG)
    #ifndef _NDEBUG
        #define _NDEBUG
    #endif // !_NDEBUG
#elif defined(_NDEBUG)
    #ifndef NDEBUG
        #define NDEBUG
    #endif // !NDEBUG
#endif // DEBUG


#if defined(_POSIX) || defined(POSIX)

    #undef WINDOWS
    #undef _WINDOWS

    #include "system/src/posix/GEPosix.h"

#elif defined(_WINDOWS) || defined(WINDOWS)


    #include "system/src/windows/GEWindows.h"

#endif // 

#endif // AREG_GEPLATFORM_H
