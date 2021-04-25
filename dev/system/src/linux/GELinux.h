#ifndef AREG_SYSTEM_LINUX_GELINUX_H
#define AREG_SYSTEM_LINUX_GELINUX_H

/************************************************************************
* \file         system/src/linux/GELinux.h
* \ingroup      Framework globals
* \author       Artak Avetyan (mailto:artak@aregtech.com)
* \brief        AREG Framework, platform dependent includes.
*               Linux specific directives
*
************************************************************************/
/************************************************************************
* Include files.
************************************************************************/

#ifdef _LINUX
    #ifndef LINUX
        #define LINUX   _LINUX
    #endif  // LINUX
#endif // _LINUX

#ifdef LINUX
    #ifndef _LINUX
        #define _LINUX  LINUX
    #endif  // _LINUX
#endif  // LINUX

#if defined(LINUX) || defined(_LINUX)

    #ifdef __GNUC__
        #include "gcc/gcc.h"
    #else // __GNUC__
        #error Your toolchain is not yet supported
    #endif // __GNUC__

    #ifdef   __THREAD_LOCAL
        #undef  __THREAD_LOCAL
    #endif  // __THREAD_LOCAL

    #define __THREAD_LOCAL __thread


#endif // defined(LINUX) || defined(_LINUX)

#endif // AREG_SYSTEM_LINUX_GELINUX_H
