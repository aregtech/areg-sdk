#ifndef AREG_BASE_GEMACROS_H
#define AREG_BASE_GEMACROS_H
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/GEMacros.h
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Predefined MACRO
 *
 ************************************************************************/

/**
 * \brief   No copyable class declaration.
 *          Deletes copy constructor and copy assignment operator.
 **/
#ifndef AREG_NOCOPY
    #define AREG_NOCOPY(ClassName)                                          \
        ClassName( const ClassName & /*src*/ ) = delete;                    \
        ClassName & operator = ( const ClassName & /*src*/ ) = delete
#endif // !AREG_NOCOPY

/**
 * \brief   No movable class declaration.
 *          Deletes move constructor and move assignment operator.
 **/
#ifndef AREG_NOMOVE
    #define AREG_NOMOVE(ClassName)                                          \
        ClassName( ClassName && /*src*/ ) noexcept = delete;                \
        ClassName & operator = ( ClassName && /*src*/ ) noexcept = delete
#endif // !AREG_NOMOVE

/**
 * \brief   No copyable and no movable class declaration.
 **/
#ifndef AREG_NOCOPY_NOMOVE
    #define AREG_NOCOPY_NOMOVE( ClassName )                                 \
                AREG_NOCOPY( ClassName );                                   \
                AREG_NOMOVE(ClassName)
#endif // !AREG_NOCOPY_NOMOVE

//!< Stringification macros. AREG_STRINGIFY_VALUE double-expands before stringifying.
#ifndef AREG_STRINGIFY
    #define AREG_STRINGIFY(x)           #x
    #define AREG_STRINGIFY_VALUE(x)     AREG_STRINGIFY(x)
#endif // !AREG_STRINGIFY

//!< Compiler pragma, portable across MSVC and GCC/Clang.
#ifndef AREG_PRAGMA
    #ifdef _MSC_VER
        #define AREG_PRAGMA(x) __pragma (x)
    #else
        #define AREG_PRAGMA(x) _Pragma (#x)
    #endif
#endif // AREG_PRAGMA

//!< Emits a compiler message prefixed with ">>> ".
#ifndef AREG_COMPILER_MSG
    #define AREG_COMPILER_MSG(msg)      AREG_PRAGMA(message (">>> " #msg))
#endif // !AREG_COMPILER_MSG

//!< Emits a compiler TODO message with file and line information.
#ifndef AREG_TODO
    #define AREG_TODO(msg)              AREG_PRAGMA(">>> TODO :: " #msg ": here --> " __FILE__":" AREG_STRINGIFY_VALUE(__LINE__))
#endif // !AREG_TODO

/**
 * \brief   Defined assertion macros.
 *          ASSERT / ASSERT_MSG compile to nothing in Release.
 *          VERIFY / VERIFY_MSG always evaluate the expression,
 *          but only assert in Debug.
 **/
#ifdef   _DEBUG

    #include <assert.h>

    #ifndef ASSERT
        #define ASSERT(x)                       assert((x))
    #endif   // ASSERT
    #ifndef ASSERT_MSG
        #define ASSERT_MSG(x, msg)              ASSERT((x))
    #endif   // ASSERT_MSG

    #ifndef  VERIFY
        #define VERIFY(x)                       ASSERT((x))
    #endif   // VERIFY
    #ifndef  VERIFY_MSG
        #define VERIFY_MSG(x, msg)              ASSERT_MSG((x), msg)
    #endif   // VERIFY_MSG

#else    // _DEBUG

    #ifndef ASSERT
        #define ASSERT(x)                       ((void)0)
    #endif   // ASSERT
    #ifndef ASSERT_MSG
        #define ASSERT_MSG(x, msg)              ((void)0)
    #endif   // ASSERT_MSG

    #ifndef  VERIFY
        #define VERIFY(x)                       static_cast<void>(x)
    #endif   // VERIFY

    #ifndef  VERIFY_MSG
        #define VERIFY_MSG(x, msg)              static_cast<void>(x)
    #endif   // VERIFY_MSG

#endif   // _DEBUG

/**
 * \brief   Debug new operator for MSVC CRT memory leak detection.
 *          In Release or non-MSVC builds, maps to plain new.
 **/
#if defined(_DEBUG) && defined(_MSC_VER)
    #include <crtdbg.h>
    #ifndef DEBUG_NEW
      #define DEBUG_NEW    new(_NORMAL_BLOCK, __FILE__, __LINE__)
   #endif   // DEBUG_NEW
#else // _DEBUG
   #ifndef DEBUG_NEW
      #define DEBUG_NEW    new
   #endif   // DEBUG_NEW
#endif   // _DEBUG

/**
 * \brief   Debug output macros. Active only in Debug builds.
 **/
#ifdef _DEBUG

#include "areg/base/private/NEDebug.hpp"

    #ifndef AREG_OUTPUT_DBG
        #if defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_DEBUG)
            #define AREG_OUTPUT_DBG(...)        NEDebug::outputConsole(NEDebug::eDegubPrio::PrioDbg, __VA_ARGS__)
        #else
            #define AREG_OUTPUT_DBG(...)        ((void)0)
        #endif
    #endif

    #ifndef AREG_OUTPUT_INFO
        #if defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_INFO)
            #define AREG_OUTPUT_INFO(...)       NEDebug::outputConsole(NEDebug::eDegubPrio::PrioInfo, __VA_ARGS__)
        #else
            #define AREG_OUTPUT_INFO(...)       ((void)0)
        #endif
    #endif

    #ifndef AREG_OUTPUT_WARN
        #if defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_WARN)
            #define AREG_OUTPUT_WARN(...)       NEDebug::outputConsole(NEDebug::eDegubPrio::PrioWarn, __VA_ARGS__)
        #else
            #define AREG_OUTPUT_WARN(...)       ((void)0)
        #endif
    #endif

    #ifndef AREG_OUTPUT_ERR
        #if defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_ERROR)
            #define AREG_OUTPUT_ERR(...)        NEDebug::outputConsole(NEDebug::eDegubPrio::PrioErr, __VA_ARGS__)
        #else
            #define AREG_OUTPUT_ERR(...)        ((void)0)
        #endif
    #endif

    #ifndef AREG_OUTPUT_MSG
        #if defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL !=  OUTPUT_DEBUG_LEVEL_NONE)
            #define AREG_OUTPUT_MSG(...)        NEDebug::outputConsole( __VA_ARGS__)
        #else
            #define AREG_OUTPUT_MSG(...)        ((void)0)
        #endif
    #endif

#else   // _DEBUG, no meaning for other than Debug build.

    #ifndef AREG_OUTPUT_DBG
        #define AREG_OUTPUT_DBG(...)            ((void)0)
    #endif

    #ifndef AREG_OUTPUT_INFO
        #define AREG_OUTPUT_INFO(...)           ((void)0)
    #endif

    #ifndef AREG_OUTPUT_WARN
        #define AREG_OUTPUT_WARN(...)           ((void)0)
    #endif

    #ifndef AREG_OUTPUT_ERR
        #define AREG_OUTPUT_ERR(...)            ((void)0)
    #endif

    #ifndef AREG_OUTPUT_MSG
        #define AREG_OUTPUT_MSG(...)            ((void)0)
    #endif

#endif  // _DEBUG

#endif  // AREG_BASE_GEMACROS_H
