#ifndef AREG_BASE_GESWITCHES_H
#define AREG_BASE_GESWITCHES_H
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/GESwitches.h
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Framework Switches.
 *              This header should be included before using any of 
 *              Framework classes.
 *
 *              1.  To build framework as a shared library (export symbols), define one of preprocessor definitions:
 *                  EXP_AREG_DLL, or _EXP_AREG_DLL, or EXPORT_SHARED_SYMBOLS
 *
 *              2.  To build framework as a static library (export symbols), define one of preprocessor definitions:
 *                  EXP_AREG_LIB, or _EXP_AREG_LIB, or EXPORT_STATIC_SYMBOLS
 *
 *              3.  To link application with shared framework library (import symbols), define one of preprocessor definitions:
 *                  IMP_AREG_DLL, or _IMP_AREG_DLL, or IMPORT_SHARED_SYMBOLS
 *
 *              4.  To link application with static framework library (import symbols), define one of preprocessor definitions:
 *                  IMP_AREG_LIB, or _IMP_AREG_DLL, or IMPORT_STATIC_SYMBOLS
 *
 *              If none of these is defined, by default it will assume "IMP_ARE_DLL"
 *
 ************************************************************************/

/**
 * Do not define AREG_API, this is the job of switcher
 */
#ifdef  AREG_API
   #error   "AREG_API is already defined. Undefine before include this file!"
#endif

#if defined(IMP_AREG_DLL) || defined(_IMP_AREG_DLL) || defined(IMPORT_SHARED_SYMBOLS) || defined(_IMPORT_SHARED_SYMBOLS)

    #undef  IMP_AREG_DLL
    #undef  IMP_AREG_LIB
    #undef  EXP_AREG_DLL
    #undef  EXP_AREG_LIB

    #undef  _IMP_AREG_DLL
    #undef  _IMP_AREG_LIB
    #undef  _EXP_AREG_DLL
    #undef  _EXP_AREG_LIB

    #undef  IMPORT_SHARED_SYMBOLS
    #undef  IMPORT_STATIC_SYMBOLS
    #undef  EXPORT_SHARED_SYMBOLS
    #undef  EXPORT_STATIC_SYMBOLS

    #define IMP_AREG_DLL
    #define _IMP_AREG_DLL

#endif  // defined(IMP_AREG_DLL) || defined(_IMP_AREG_DLL) || defined(IMPORT_SHARED_SYMBOLS) || defined(_IMPORT_SHARED_SYMBOLS)

#if defined(IMP_AREG_LIB) || defined(_IMP_AREG_LIB) || defined(IMPORT_STATIC_SYMBOLS) || defined(_IMPORT_STATIC_SYMBOLS)

    #undef  IMP_AREG_DLL
    #undef  IMP_AREG_LIB
    #undef  EXP_AREG_DLL
    #undef  EXP_AREG_LIB

    #undef  _IMP_AREG_DLL
    #undef  _IMP_AREG_LIB
    #undef  _EXP_AREG_DLL
    #undef  _EXP_AREG_LIB

    #undef  IMPORT_SHARED_SYMBOLS
    #undef  IMPORT_STATIC_SYMBOLS
    #undef  EXPORT_SHARED_SYMBOLS
    #undef  EXPORT_STATIC_SYMBOLS

    #define IMP_AREG_LIB
    #define _IMP_AREG_LIB

#endif  // defined(IMP_AREG_LIB) || defined(_IMP_AREG_LIB) || defined(IMPORT_STATIC_SYMBOLS) || defined(_IMPORT_STATIC_SYMBOLS)

#if defined(EXP_AREG_DLL) || defined(_EXP_AREG_DLL) || defined(EXPORT_SHARED_SYMBOLS) || defined(_EXPORT_SHARED_SYMBOLS)

    #undef  IMP_AREG_DLL
    #undef  IMP_AREG_LIB
    #undef  EXP_AREG_DLL
    #undef  EXP_AREG_LIB

    #undef  _IMP_AREG_DLL
    #undef  _IMP_AREG_LIB
    #undef  _EXP_AREG_DLL
    #undef  _EXP_AREG_LIB

    #undef  IMPORT_SHARED_SYMBOLS
    #undef  IMPORT_STATIC_SYMBOLS
    #undef  EXPORT_SHARED_SYMBOLS
    #undef  EXPORT_STATIC_SYMBOLS

    #define EXP_AREG_DLL
    #define _EXP_AREG_DLL

#endif  // defined(EXP_AREG_DLL) || defined(_EXP_AREG_DLL) || defined(EXPORT_SHARED_SYMBOLS) || defined(_EXPORT_SHARED_SYMBOLS)

#if defined(EXP_AREG_LIB) || defined(_EXP_AREG_LIB) || defined(EXPORT_STATIC_SYMBOLS) || defined(_EXPORT_STATIC_SYMBOLS)

    #undef  IMP_AREG_DLL
    #undef  IMP_AREG_LIB
    #undef  EXP_AREG_DLL
    #undef  EXP_AREG_LIB

    #undef  _IMP_AREG_DLL
    #undef  _IMP_AREG_LIB
    #undef  _EXP_AREG_DLL
    #undef  _EXP_AREG_LIB

    #undef  IMPORT_SHARED_SYMBOLS
    #undef  IMPORT_STATIC_SYMBOLS
    #undef  EXPORT_SHARED_SYMBOLS
    #undef  EXPORT_STATIC_SYMBOLS

    #define EXP_AREG_LIB
    #define _EXP_AREG_LIB

#endif  // defined(EXP_AREG_LIB) || defined(_EXP_AREG_LIB) || defined(EXPORT_STATIC_SYMBOLS) || defined(_EXPORT_STATIC_SYMBOLS)

#if !defined(EXP_AREG_DLL) && !defined(EXP_AREG_LIB) && !defined(IMP_AREG_DLL) && !defined(IMP_AREG_LIB)
    #pragma message("WARNING: No compiler option set for the AREG library. Using default IMP_AREG_DLL. To use a static library, define IMP_AREG_LIB. Refer to the GESwitches.h file in the AREG base for details.")
    #define IMP_AREG_DLL
#endif  // !defined(EXP_AREG_DLL) && !defined(EXP_AREG_LIB) && !defined(IMP_AREG_DLL) && !defined(IMP_AREG_LIB)

/**
 * \brief   Set Exports / Imports for dynamic and static libraries
 **/
#ifdef EXP_AREG_DLL
   #if defined(_WIN32)
      #define AREG_API          __declspec(dllexport)
   #elif defined(__CYGWIN__)
      #define AREG_API          __attribute__ ((dllexport))
   #elif defined(__GNUC__) && (__GNUC__ >= 4)
      #define AREG_API          __attribute__((visibility("default")))
   #else    // !defined(__GNUC__) && (__GNUC__ >= 4)
      #define AREG_API
   #endif   // (defined(_WINDOWS) || defined(WINDOWS) || defined(_WIN32))
#endif   // EXP_AREG_DLL

#ifdef IMP_AREG_DLL
   #if defined(_WIN32)
      #define AREG_API          __declspec(dllimport)
   #elif defined(__CYGWIN__)
      #define AREG_API          __attribute__ ((dllimport))
   #elif defined(__GNUC__) && (__GNUC__ >= 4)
      #define AREG_API          __attribute__((visibility("default")))
   #else    // !(__GNUC__ >= 4)
      #define AREG_API
   #endif   // (defined(_WINDOWS) || defined(WINDOWS) || defined(_WIN32))
#endif   // IMP_AREG_DLL

#if defined(EXP_AREG_LIB) || defined(IMP_AREG_LIB)
    #define AREG_API
#endif // defined(EXP_AREG_LIB) || defined(IMP_AREG_LIB)


#ifndef AREG_API
   #define AREG_API
#endif  // AREG_API

#ifdef WIN32
    #define AREG_API_IMPL   AREG_API
#else   // !WIN32
    #define AREG_API_IMPL
#endif   // WIN32

// By default, no AREG extended features.
#if !defined(AREG_EXTENDED) && !defined(AREG_EXTEND)
    #pragma message("The AREG_EXTENDED is not defined, setting default value 0")
    #define AREG_EXTENDED   0
#elif defined(AREG_EXTEND) && !defined(AREG_EXTENDED)
    #define AREG_EXTENDED   AREG_EXTEND
#endif  // AREG_EXTENDED

// By default, compile with logs
#ifndef AREG_LOGS
    #pragma message("The AREG_LOGS is not defined, setting default value 1")
    #define AREG_LOGS       1
#endif  // AREG_LOGS

#endif   // AREG_BASE_GESWITCHES_H
