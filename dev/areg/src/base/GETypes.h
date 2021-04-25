#ifndef AREG_BASE_GETYPES_H
#define AREG_BASE_GETYPES_H

/************************************************************************
 * \file        areg/src/base/GETypes.h
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
#include "system/src/GEPlatform.h"

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

/**
 * \brief   The type defining ID. It is 64-bit in 64-bit systems.
 **/
#ifdef BIT64
    #ifdef _UINTPTR_T_DEFINED
        typedef uintptr_t   id_type;
    #else   // !_UINTPTR_T_DEFINED
        typedef uint64_t    id_type;
    #endif  // _UINTPTR_T_DEFINED
#else   // defined(BIT32)
    #ifdef _UINTPTR_T_DEFINED
        typedef uintptr_t   id_type;
    #else   // !_UINTPTR_T_DEFINED
        typedef uint32_t    id_type;
    #endif  // _UINTPTR_T_DEFINED
#endif  // id_type

/**
 * \brief   The type defines an item ID.
 **/
typedef id_type         ITEM_ID;

/**
 * \brief   Type, defining position of element in Lists.
 **/
/**
 * \brief   Type definition of position in Linked List
 **/
typedef void *          LISTPOS;
/**
 * \brief   Type definition of position in Stack List
 **/
typedef void *          STACKPOS;
/**
 * \brief   Type definition of position in Hash Map
 **/
typedef void *          MAPPOS;

/**
 * \brief   Type definition of socket descriptor. Differs in on different platforms
 **/
#ifdef  _WINDOWS
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
 * \brief   The type of count.
 **/
typedef unsigned int	TCOUNT;

/**
 * \brief   The time value. Microseconds passed since January 1, 1970
 **/
typedef uint64_t        TIME64;

#endif // AREG_BASE_GETYPES_H
