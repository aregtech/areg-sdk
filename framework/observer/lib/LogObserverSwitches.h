#ifndef AREG_OBSERVER_LIB_LOGOBSERVERSWITCHES_H
#define AREG_OBSERVER_LIB_LOGOBSERVERSWITCHES_H
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        observer/lib/LogObserverSwitches.h
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer Switches.
 *              This header should be included before using AREG Log Observer API.
 *
 *              1.  To build library as a shared (export symbols), define EXP_OBSERVER_DLL
 *
 *              2.  To build library as a static (export symbols), define EXP_OBSERVER_LIB
 *
 *              3.  To link application with shared library (import symbols), define IMP_OBSERVER_DLL
 *
 *              3.  To link application with static library (import symbols), define IMP_OBSERVER_LIB
 *
 *              If none of these is defined, by default it will assume "IMP_OBSERVER_DLL"
 *
 ************************************************************************/

/**
 * Do not define OBSERVER_API, this is the job of switcher
 */
#ifdef  OBSERVER_API
   #error   "OBSERVER_API is already defined. Undefine before include this file!"
#endif

#ifdef  IMP_OBSERVER_DLL

    #undef  IMP_OBSERVER_DLL
    #undef  IMP_OBSERVER_LIB
    #undef  EXP_OBSERVER_DLL
    #undef  EXP_OBSERVER_LIB

    #define IMP_OBSERVER_DLL

#endif  // IMP_OBSERVER_DLL

#ifdef  IMP_OBSERVER_LIB

    #undef  IMP_OBSERVER_DLL
    #undef  IMP_OBSERVER_LIB
    #undef  EXP_OBSERVER_DLL
    #undef  EXP_OBSERVER_LIB

    #define IMP_OBSERVER_LIB

#endif  // IMP_OBSERVER_LIB

#ifdef  EXP_OBSERVER_DLL

    #undef  IMP_OBSERVER_DLL
    #undef  IMP_OBSERVER_LIB
    #undef  EXP_OBSERVER_DLL
    #undef  EXP_OBSERVER_LIB

    #define EXP_OBSERVER_DLL

#endif  // EXP_OBSERVER_DLL

#ifdef  EXP_OBSERVER_LIB

    #undef  IMP_OBSERVER_DLL
    #undef  IMP_OBSERVER_LIB
    #undef  EXP_OBSERVER_DLL
    #undef  EXP_OBSERVER_LIB

    #define EXP_OBSERVER_LIB

#endif  // EXP_OBSERVER_LIB

#if !defined(EXP_OBSERVER_DLL) && !defined(EXP_OBSERVER_LIB) && !defined(IMP_OBSERVER_DLL) && !defined(IMP_OBSERVER_LIB)
    #pragma message ("WARNING: None of log observer preprocessor defines are set. By default sets IMP_OBSERVER_DLL. See LogObserverSwitches.h file of AREG API Core")
    #define IMP_OBSERVER_DLL
#endif  // !defined(EXP_OBSERVER_DLL) && !defined(EXP_OBSERVER_LIB) && !defined(IMP_OBSERVER_DLL) && !defined(IMP_OBSERVER_LIB)

/**
 * \brief   Set Exports / Imports for dynamic and static libraries
 **/
#ifdef EXP_OBSERVER_DLL
   #if defined(_WIN32)
      #define OBSERVER_API      __declspec(dllexport)
   #elif defined(__CYGWIN__)
      #define OBSERVER_API      __attribute__ ((dllexport))
   #elif defined(__GNUC__) && (__GNUC__ >= 4)
      #define OBSERVER_API      __attribute__((visibility("default")))
   #else    // !defined(__GNUC__) && (__GNUC__ >= 4)
      #define OBSERVER_API
   #endif   // (defined(_WINDOWS) || defined(WINDOWS) || defined(_WIN32))
#endif   // EXP_OBSERVER_DLL

#ifdef IMP_OBSERVER_DLL
   #if defined(_WIN32)
      #define OBSERVER_API      __declspec(dllimport)
   #elif defined(__CYGWIN__)
      #define OBSERVER_API      __attribute__ ((dllimport))
   #elif defined(__GNUC__) && (__GNUC__ >= 4)
      #define OBSERVER_API      __attribute__((visibility("default")))
   #else    // !(__GNUC__ >= 4)
      #define OBSERVER_API
   #endif   // (defined(_WINDOWS) || defined(WINDOWS) || defined(_WIN32))
#endif   // IMP_OBSERVER_DLL

#if defined(EXP_OBSERVER_LIB) || defined(IMP_OBSERVER_LIB)
    #define OBSERVER_API
#endif // defined(EXP_OBSERVER_LIB) || defined(IMP_OBSERVER_LIB)


#ifndef OBSERVER_API
   #define OBSERVER_API
#endif  // OBSERVER_API

#ifdef WIN32
    #define OBSERVER_API_IMPL     OBSERVER_API
#else   // !WIN32
    #define OBSERVER_API_IMPL
#endif   // WIN32

#if !defined(IMP_AREG_DLL) && !defined(IMP_AREG_LIB)
    #pragma message ("WARNING: Neither IMP_AREG_DLL, nor IMP_AREG_LIB is defined. Make proper settings. By default setting IMP_AREG_DLL.")
    #define IMP_AREG_DLL
#endif  // !defined(IMP_AREG_DLL) && !defined(IMP_AREG_LIB)

#include "areg/base/GEGlobal.h"

#endif   // AREG_OBSERVER_LIB_LOGOBSERVERSWITCHES_H
