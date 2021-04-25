#ifndef AREG_SYSTEM_GCC_GCC_H
#define AREG_SYSTEM_GCC_GCC_H
/************************************************************************
 * \file        system/gcc/gcc.h
 * \ingroup     Framework, GCC proxies
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Framework, platform dependent includes for GCC
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

/************************************************************************
 * \brief   This header file is included from system.h when using gcc
            do not include directly! 
 ************************************************************************/

#ifdef __GNUC__
    #ifndef GNUC
        #define GNUC    GNUC
    #endif // !GNUC
#endif

#ifdef GNUC
    #ifndef __GNUC__
        #defne  __GNUC__    GNUC
    #endif // !__GNUC__
#endif // GNUC

#if defined(__GNUC__) || defined(GNUC)

#ifdef _M64

    #undef  _M32
    #undef  BIT32
    #undef  _BIT32
    #undef  BIT64
    #undef  _BIT64

    #define  BIT64
    #define _BIT64

#define _M32
    #define BIT32
    #undef  BIT64
#else
    #include "gcc/gcc.h"
#endif  // defined(__GNUC__) || defined(GNUC)

#endif // AREG_SYSTEM_GCC_GCC_H
