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
 * \brief   An empty MACRO, does nothing
 **/
#define EMPTY_MACRO

/**
 * \brief   Success error code. Mainly used in POSIX methods
 **/
#define RETURNED_OK             0

/**
 * \brief   No copyable class declaration.
 **/
#ifndef DECLARE_NOCOPY
    #define DECLARE_NOCOPY(ClassName)                                       \
        ClassName( const ClassName & /*src*/ ) = delete;                    \
        ClassName & operator = ( const ClassName & /*src*/ ) = delete
#endif // !DECLARE_NOCOPY

/**
 * \brief   No movable class declaration.
 **/
#ifndef DECLARE_NOMOVE
    #define DECLARE_NOMOVE(ClassName)                                       \
        ClassName( ClassName && /*src*/ ) noexcept = delete;                \
        ClassName & operator = ( ClassName && /*src*/ ) noexcept = delete
#endif // !DECLARE_NOMOVE

/**
 * \brief   No copyable and no movable class declaration.
 **/
#ifndef DECLARE_NOCOPY_NOMOVE
    #define DECLARE_NOCOPY_NOMOVE( ClassName )                              \
                DECLARE_NOCOPY( ClassName );                                \
                DECLARE_NOMOVE(ClassName)
#endif // !DECLARE_NOCOPY_NOMOVE

/**
 * \brief   No copy for class templates.
 *          ClassName   The name of class
 *          Typenames   The name of class
 *      Example: DECLARE_NOCOPY_TEMPLATE(TEString, <CharType>)
 **/
#ifndef DECLARE_NOCOPY_TEMPLATE
    #define DECLARE_NOCOPY_TEMPLATE(ClassName, Typenames)                                           \
        ClassName( const ClassName##Typenames & /*src*/ ) = delete;                                 \
        ClassName##Typenames & operator = ( const ClassName##Typenames & /*src*/ ) = delete
#endif // !DECLARE_NOCOPY_TEMPLATE

/**
 * \brief   No move for class templates.
 *          ClassName   The name of class
 *          Typenames   The name of class
 *      Example: DECLARE_NOMOVE_TEMPLATE(TEString, <CharType>)
 **/
#ifndef DECLARE_NOMOVE_TEMPLATE
    #define DECLARE_NOMOVE_TEMPLATE(ClassName, Typenames)                                           \
            ClassName( ClassName##Typenames && /*src*/ ) noexcept = delete;                         \
            ClassName##Typenames & operator = ( ClassName##Typenames && /*src*/ ) noexcept = delete
#endif // !DECLARE_NOMOVE_TEMPLATE

#ifndef DECLARE_NOCOPY_NOMOVE_TEMPLATE
    #define DECLARE_NOCOPY_NOMOVE_TEMPLATE( ClassName, Typenames )                                  \
                DECLARE_NOCOPY_TEMPLATE( ClassName, Typenames );                                    \
                DECLARE_NOMOVE_TEMPLATE( ClassName, Typenames )

#endif // !DECLARE_NOCOPY_NOMOVE

#ifndef DECLARE_UNUSED
    #define DECLARE_UNUSED(x)           (()(x))
#endif  // DECLARE_UNUSED

//!< MACRO to make strings.
#ifndef MACRO_MAKE_STRING
    //!< This macro makes a message string
    #define MACRO_MAKE_STRING(x)        #x
    //!< This macro converts a value to a string
    #define MACRO_CONV_STRING(x)        MACRO_MAKE_STRING(x)
#endif // !MACRO_MAKE_STRING

//!< This macro makes a message
#ifndef MACRO_MAKE_MESSAGE
    #define MACRO_MAKE_MESSAGE(x)       ">>> " ##x
#endif // !MACRO_MAKE_MESSAGE

#ifndef MACRO_DO_PRAGMA
    #ifdef MS_VISUAL_CPP
        #define MACRO_DO_PRAGMA(x) __pragma (#x)
    #else
        #define MACRO_DO_PRAGMA(x) _Pragma (#x)
    #endif
#endif // MACRO_DO_PRAGMA

//!< This macro creates and outputs compile time message
#ifndef MACRO_COMPILER_MESSAGE
    #define MACRO_COMPILER_MESSAGE(msg)     MACRO_DO_PRAGMA(message (">>> " #msg))
#endif // !MACRO_COMPILER_MESSAGE

//!< This macro creates and outputs compile time message with prefix "TODO" and the message,
//! followed with the file name and the line number to prompt.
#ifndef MACRO_TODO
    #define MACRO_TODO(msg)                 MACRO_DO_PRAGMA(">>> TODO :: " #msg ": here --> " __FILE__":" MACRO_CONV_STRING(__LINE__))
#endif // !MACRO_TODO


/**
 * \brief   Defined assertion macro.
 *          Valid only for Debug versions
 **/
#ifdef   _DEBUG

    #include <assert.h>

    #ifndef ASSERT
        #define ASSERT(x)                       assert(x)
    #endif   // ASSERT
    #ifndef ASSERT_MSG
        #define  ASSERT_MSG(x, msg)             ASSERT(x)
    #endif   // ASSERT_MSG

    #ifndef  VERIFY
        #define VERIFY(x)                       ASSERT(x)
    #endif   // VERIFY
    #ifndef  VERIFY_MSG
        #define VERIFY_MSG(x, msg)              ASSERT_MSG(x, msg)
    #endif   // VERIFY_MSG

#else    // _DEBUG

    #ifndef ASSERT
        #define ASSERT(x)                       EMPTY_MACRO
    #endif   // ASSERT
    #ifndef ASSERT_MSG
        #define  ASSERT_MSG(x, msg)             EMPTY_MACRO
    #endif   // ASSERT_MSG

    #ifndef  VERIFY
        #define VERIFY(x)                       static_cast<void>(x)
    #endif   // VERIFY

    #ifndef  VERIFY_MSG
        #define VERIFY_MSG(x, msg)              static_cast<void>(x)
    #endif   // VERIFY_MSG

#endif   // _DEBUG

/**
 * \brief   defines some switches and macros to use in debug version
 **/
#if defined(_DEBUG) && defined(_MSC_VER)
   // on non-Windows systems, there is no operator new which takes three parameters
   // this is defined in a platform specific overloaded new.h
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
 * \brief   Debugging message output macro.
 *          Valid for Debug version
 **/
#ifdef _DEBUG

#include "areg/base/private/NEDebug.hpp"

    /**
     * \brief   Output Debug message, will have "DBG" prefix
     **/
    #ifndef OUTPUT_DBG
        #if defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_DEBUG)
            #define OUTPUT_DBG(...)             NEDebug::outputConsole(NEDebug::eDegubPrio::PrioDbg, __VA_ARGS__)
        #else
            #define OUTPUT_DBG(...)             EMPTY_MACRO
        #endif
    #endif

    /**
     * \brief   Output Information message, will have "INFO" prefix
     **/
    #ifndef OUTPUT_INFO
        #if defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_INFO)
            #define OUTPUT_INFO(...)            NEDebug::outputConsole(NEDebug::eDegubPrio::PrioInfo, __VA_ARGS__)
        #else
            #define OUTPUT_INFO(...)            EMPTY_MACRO
        #endif
    #endif

    /**
     * \brief   Output Warning message, will have "WARN" prefix
     **/
    #ifndef OUTPUT_WARN
        #if defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_WARN)
            #define OUTPUT_WARN(...)            NEDebug::outputConsole(NEDebug::eDegubPrio::PrioWarn, __VA_ARGS__)
        #else
            #define OUTPUT_WARN(...)            EMPTY_MACRO
        #endif
    #endif

    /**
     * \brief   Output Error message, will have "ERR" prefix
     **/
    #ifndef OUTPUT_ERR
        #if defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_ERROR)
            #define OUTPUT_ERR(...)             NEDebug::outputConsole(NEDebug::eDegubPrio::PrioErr, __VA_ARGS__)
        #else
            #define OUTPUT_ERR(...)             EMPTY_MACRO
        #endif
    #endif

    /**
     * \brief   Output message text without change
     **/
    #ifndef OUTPUT_MSG
        #if defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL !=  OUTPUT_DEBUG_LEVEL_NONE)
            #define OUTPUT_MSG(...)             NEDebug::outputConsole( __VA_ARGS__)
        #else
            #define OUTPUT_MSG(...)             EMPTY_MACRO
        #endif
    #endif

    /**
     * \brief   Fills call-stack dump into the passed vector
     *          The exception should be raised and the passed vector should be of type LinkedList<String>
     **/
    #ifndef CALLSTACK_DUMP
        #define CALLSTACK_DUMP(exept, std_vector)   NEDebug::dumpExceptionCallStack(exept, std_vector)
    #endif

#else   // _DEBUG, no meaning for other than Debug build.

    #ifndef OUTPUT_DBG
        #define OUTPUT_DBG(...)                     EMPTY_MACRO
    #endif

    #ifndef OUTPUT_INFO
        #define OUTPUT_INFO(...)                    EMPTY_MACRO
    #endif

    #ifndef OUTPUT_WARN
        #define OUTPUT_WARN(...)                    EMPTY_MACRO
    #endif

    #ifndef OUTPUT_ERR
        #define OUTPUT_ERR(...)                     EMPTY_MACRO
    #endif

    #ifndef OUTPUT_MSG
        #define OUTPUT_MSG(...)                     EMPTY_MACRO
    #endif

    #ifndef CALLSTACK_DUMP
        #define CALLSTACK_DUMP(exept, std_vector)   EMPTY_MACRO
    #endif

#endif  // _DEBUG

#endif  // AREG_BASE_GEMACROS_H
