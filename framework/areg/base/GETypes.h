#ifndef AREG_BASE_GETYPES_H
#define AREG_BASE_GETYPES_H
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/GETypes.h
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, global include header file
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

/**
 * \brief   this header defines basic types, which should be used 
 *          everywhere instead of the default types.
 *          included from GGlobal.h
 **/
#include "areg/system/GEPlatform.h"

#if defined(_MSC_VER) && (_MSC_VER < 1600 ) // MS express/studio 2008 or earlier

#error  This platfrom does not support MS express/studio 2008 or earlier. Missing parts are type definitions.

    typedef signed   char       int8_t;
    typedef unsigned char       uint8_t;
    typedef signed   short      int16_t;
    typedef unsigned short      uint16_t;
    typedef signed   int        int32_t;
    typedef unsigned int        uint32_t;
    typedef signed   long long  int64_t;
    typedef unsigned long long  uint64_t;

#elif   !defined(_INTSAFE_H_INCLUDED_)

    #include <stdint.h>

#else

    typedef signed   char       int8_t;
    typedef unsigned char       uint8_t;
    typedef signed   short      int16_t;
    typedef unsigned short      uint16_t;
    typedef signed   int        int32_t;
    typedef unsigned int        uint32_t;
    typedef signed   long long  int64_t;
    typedef unsigned long long  uint64_t;

#endif  // defined(_MSC_VER) && (_MSC_VER < 1600 )

#include <stddef.h>

//! The digital value type of the pointer.
#ifdef BIT64

    //! The type defining ID. It is 64-bit in 64-bit systems.
    typedef uint64_t    id_type;

    #ifdef _UINTPTR_T_DEFINED
        typedef uintptr_t   ptr_type;
    #else   // !_UINTPTR_T_DEFINED
        typedef uint64_t    ptr_type;
    #endif  // _UINTPTR_T_DEFINED
    
#else   // defined(BIT32)

    //! The type defining ID. It is 64-bit in 64-bit systems.
    typedef uint32_t    id_type;

    #ifdef _UINTPTR_T_DEFINED
        typedef uintptr_t       ptr_type;
    #else   // !_UINTPTR_T_DEFINED
        typedef unsigned long   ptr_type;
    #endif  // _UINTPTR_T_DEFINED

#endif  // BIT64

//! The sequence number type.
typedef uint64_t        SequenceNumber;

//!< The signed sequence type.
typedef int64_t         SignedSequence;

/**
 * \brief   The type defines an item ID.
 **/
typedef uint64_t        ITEM_ID;

/**
 * \brief   Type definition of socket descriptor. Differs in on different platforms
 **/
#ifdef  _WIN32
    typedef size_t      SOCKETHANDLE;
#else
    typedef int         SOCKETHANDLE;
#endif

/**
 * \brief   The handle of thread
 **/
typedef void *          THREADHANDLE;

/**
 * \brief   The handle of file
 **/
typedef void *          FILEHANDLE;    //!< Definition of file handle

/**
 * \brief   The type of Timer Handle.
 **/
typedef void *          TIMERHANDLE;

/**
 * \brief   The type of process handle
 **/
typedef void *          PROCESSHANDLE;

/**
 * \brief   The type of synchronization object handle
 **/
typedef void *          SYNCHANDLE;

/**
 * \brief   The type of count.
 **/
typedef unsigned int	TCOUNT;

/**
 * \brief   The time value. Microseconds passed since January 1, 1970
 **/
typedef uint64_t        TIME64;

/**
 * \brief   The message ID type.
 **/
typedef uint32_t        msg_id;

#endif  // AREG_BASE_GETYPES_H
