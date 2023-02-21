#ifndef AREG_BASE_GEMACROS_H
#define AREG_BASE_GEMACROS_H
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/GEMacros.h
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Predefined MACRO
 *
 ************************************************************************/

#if defined(BIT32)
    #define TARGET_PLATFORM     32
#elif defined(BIT64)
    #define TARGET_PLATFORM     64
#else
    #define TARGET_PLATFORM     0
#endif // BIT32

#ifndef IN
    #define IN
#endif  // !IN

#ifndef OUT
    #define OUT
#endif  // !OUT

#ifndef OPT
    #define OPT
#endif  // !OPT

#ifndef IGNORE_PARAM
    #define IGNORE_PARAM        nullptr
#endif // !IGNORE_PARAM

/**
 * \brief   An empty MACRO, does nothing
 **/
#define EMPTY_MACRO

/**
 * \brief   Success error code. Mainly used in POSIX methods
 **/
#define RETURNED_OK             0

/**
 * \brief   Return size of statically defined array
 **/
#ifndef MACRO_ARRAYLEN
    #define MACRO_ARRAYLEN(a)   (sizeof(a) / sizeof(a[0]))
#endif  // MACRO_ARRAYLEN

/**
 * \brief   extracts unsigned low-byte value of 16-bit argument
 **/
#ifndef MACRO_16_LO_BYTE8
    #define  MACRO_16_LO_BYTE8(n)   (static_cast<unsigned char>( static_cast<unsigned short>(n) >>  0 ) & 0xFF )
#endif  // MACRO_16_LO_BYTE8

/**
 * \brief   extracts unsigned low-byte value of 32-bit argument
 **/
#ifndef MACRO_32_LO_BYTE8
    #define  MACRO_32_LO_BYTE8(n)   (static_cast<unsigned char>( static_cast<unsigned int>(n) >>  0 ) & 0xFF )
#endif  // MACRO_32_LO_BYTE8

/**
 * \brief   extracts unsigned low 16-bits value of 32-bit argument
 **/
#ifndef MACRO_32_LO_BYTE16
    #define  MACRO_32_LO_BYTE16(n)  (static_cast<unsigned short>( static_cast<unsigned int>(n) >>  0 ) & 0xFFFF )
#endif  // MACRO_32_LO_BYTE16

/**
 * \brief   extracts unsigned high-byte value of 16-bit argument
 **/
#ifndef MACRO_16_HI_BYTE8
    #define  MACRO_16_HI_BYTE8(n)   (static_cast<unsigned char>( static_cast<unsigned short>(n) >>  8 ) & 0xFF )
#endif  // MACRO_16_HI_BYTE8

/**
 * \brief   extracts unsigned high-byte value of 32-bit argument
 **/
#ifndef MACRO_32_HI_BYTE8
    #define  MACRO_32_HI_BYTE8(n)   (static_cast<unsigned char>( static_cast<unsigned int>(n) >> 24 ) & 0xFF )
#endif  // MACRO_32_HI_BYTE8

/**
 * \brief   extracts unsigned high 16-bits value of 32-bit argument
 **/
#ifndef MACRO_32_HI_BYTE16
    #define  MACRO_32_HI_BYTE16(n)  (static_cast<unsigned short>( static_cast<unsigned int>(n) >> 16 ) & 0xFFFF )
#endif  // MACRO_32_HI_BYTE8

/**
 * \brief   swaps bytes in 16-bit number
 **/
#ifndef MACRO_SWAP16
    #define MACRO_SWAP16(n)                                             \
            (   ((static_cast<unsigned short>(n) << 8) & 0xFF00) |      \
                ((static_cast<unsigned short>(n) >> 8) & 0x00FF) )
#endif  // MACRO_SWAP16

/**
 * \brief   swaps bytes in 32-bit number
 **/
#ifndef MACRO_SWAP32
    #define MACRO_SWAP32(n)                                             \
            (   ((static_cast<unsigned int>(n) & 0x000000FF) << 24) |   \
                ((static_cast<unsigned int>(n) & 0x0000FF00) <<  8) |   \
                ((static_cast<unsigned int>(n) & 0x00FF0000) >>  8) |   \
                ((static_cast<unsigned int>(n) & 0xFF000000) >> 24) )
#endif  // MACRO_SWAP32

/**
 * \brief   Makes 64-bit number from 2 32-bit numbers
 **/
#ifndef MACRO_MAKE_64
    #define MACRO_MAKE_64(hi32, lo32)           \
            (   ( (static_cast<uint64_t>(hi32) << 32) & static_cast<uint64_t>(0xFFFFFFFF00000000) ) |   \
                ( (static_cast<uint64_t>(lo32) <<  0) & static_cast<uint64_t>(0x00000000FFFFFFFF) ) )
#endif  // MACRO_MAKE_64

/**
 * \brief   Makes 64-bit number from 2 16-bit numbers
 **/
#ifndef MACRO_MAKE_32
    #define MACRO_MAKE_32(hi16, lo16)                \
            (   ( (static_cast<uint32_t>(hi16) << 16) & static_cast<uint32_t>(0xFFFF0000) ) |   \
                ( (static_cast<uint32_t>(lo16) <<  0) & static_cast<uint32_t>(0x0000FFFF) ) )
#endif  // MACRO_MAKE_32

#ifndef MACRO_64_LO_BYTE32
    #define MACRO_64_LO_BYTE32(Bit64)           \
            (   static_cast<unsigned int>( ((static_cast<uint64_t>(Bit64) & static_cast<uint64_t>(0x00000000FFFFFFFF)) >>  0) & static_cast<unsigned int>(0xFFFFFFFF))  )
#endif  // MACRO_64_LO_BYTE32

#ifndef MACRO_64_HI_BYTE32
    #define MACRO_64_HI_BYTE32(Bit64)            \
            (   static_cast<unsigned int>( ((static_cast<uint64_t>(Bit64) & static_cast<uint64_t>(0xFFFFFFFF00000000)) >> 32) & static_cast<unsigned int>(0xFFFFFFFF))  )
#endif  // MACRO_64_HI_BYTE32

#ifndef MACRO_CONSTRUCT_32
    #define MACRO_CONSTRUCT_32(bits_32_24, bits_24_16, bits_16_8, bits_8_0)                                     \
            (   ( (static_cast<unsigned int>(bits_32_24) << 24) & static_cast<unsigned int>(0xFF000000) ) |     \
                ( (static_cast<unsigned int>(bits_24_16) << 16) & static_cast<unsigned int>(0x00FF0000) ) |     \
                ( (static_cast<unsigned int>(bits_16_8 ) <<  8) & static_cast<unsigned int>(0x0000FF00) ) |     \
                ( (static_cast<unsigned int>(bits_8_0  ) <<  0) & static_cast<unsigned int>(0x000000FF) )  )
#endif  // MACRO_CONSTRUCT_32


#ifndef MACRO_CONSTRUCT_64
    #define MACRO_CONSTRUCT_64(bits_64_56, bits_56_48, bits_48_40, bits_40_32, bits_32_24, bits_24_16, bits_16_8, bits_8_0) \
            (   ( (static_cast<uint64_t>(bits_64_56) << 56) & static_cast<uint64_t>(0xFF00000000000000) ) |                 \
                ( (static_cast<uint64_t>(bits_56_48) << 48) & static_cast<uint64_t>(0x00FF000000000000) ) |                 \
                ( (static_cast<uint64_t>(bits_48_40) << 40) & static_cast<uint64_t>(0x0000FF0000000000) ) |                 \
                ( (static_cast<uint64_t>(bits_40_32) << 32) & static_cast<uint64_t>(0x000000FF00000000) ) |                 \
                ( (static_cast<uint64_t>(bits_32_24) << 24) & static_cast<uint64_t>(0x00000000FF000000) ) |                 \
                ( (static_cast<uint64_t>(bits_24_16) << 16) & static_cast<uint64_t>(0x0000000000FF0000) ) |                 \
                ( (static_cast<uint64_t>(bits_16_8 ) <<  8) & static_cast<uint64_t>(0x000000000000FF00) ) |                 \
                ( (static_cast<uint64_t>(bits_8_0  ) <<  0) & static_cast<uint64_t>(0x00000000000000FF) )  )
#endif  // MACRO_CONSTRUCT_64

/**
 * \brief   Creates signed 64-bit integer literals
 **/
#ifndef MACRO_LITERAL_INT64
    #ifdef  BIT32
        #define MACRO_LITERAL_INT64(num)    (num ## i64)
    #else   // BIT32
        #define MACRO_LITERAL_INT64(num)    (num ## LL)
    #endif  // BIT32
#endif  // MACRO_LITERAL_INT64

/**
 * \brief   Creates unsigned 64-bit integer literals
 **/
#ifndef MACRO_LITERAL_UINT64
    #ifdef  BIT32
        #define MACRO_LITERAL_UINT64(num)   (num ## Ui64)
    #else   // BIT32
        #define MACRO_LITERAL_UINT64(num)   (num ## ULL)
    #endif  // BIT32
#endif  // MACRO_LITERAL_UINT64

/**
 * \brief   Used in switch-case to generate strings
 **/
#ifndef CASE_MAKE_STRING
    #define CASE_MAKE_STRING(value)         case value: return #value
#endif  // CASE_MAKE_STRING

#ifndef CASE_DEFAULT
    #define CASE_DEFAULT(x)                 default:    return x
#endif  // CASE_DEFAULT

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

 /**
  * \brief   PI number
  **/
#ifndef M_PI
  #define M_PI              static_cast<double>(3.14159265358979323846) //  pi
#endif
#ifndef M_PI_2
  #define M_PI_2            static_cast<double>(1.57079632679489661923) //  pi/2
#endif
#ifndef M_PI_3
   #define M_PI_3           static_cast<double>(1.04719755119659774615) //  pi/3
#endif

#ifndef MAX_INT_32
    #define MAX_INT_32      static_cast<int32_t>(0x7FFFFFFF)
#endif  // MAX_INT_32

#ifndef MIN_INT_32
    #define MIN_INT_32      static_cast<int32_t>(0x80000000)
#endif  // MIN_INT_32

#ifndef MAX_UINT_32
    #define MAX_UINT_32     static_cast<uint32_t>(~0)
#endif  // MAX_UINT_32

#ifndef MIN_UINT_32
    #define MIN_UINT_32     static_cast<uint32_t>(0)
#endif  // MIN_UINT_32

#ifndef MAX_INT_64
    #define MAX_INT_64      static_cast<int64_t>(0x7FFFFFFFFFFFFFFF)
#endif  // MAX_INT_64

#ifndef MIN_INT_64
    #define MIN_INT_64      static_cast<int64_t>(0x8000000000000000)
#endif  // MIN_INT_64

#ifndef MAX_UINT_64
    #define MAX_UINT_64     static_cast<uint64_t>(~0)
#endif  // MAX_UINT_64

#ifndef MIN_UINT_64
    #define MIN_UINT_64     static_cast<uint64_t>(0)
#endif  // MIN_UINT_64

/************************************************************************
 * \brief   Simple numeric operations
 ************************************************************************/
/**
 * \brief   Returns the sign of given number
 **/
#ifndef MACRO_SIGN_OF
    #define  MACRO_SIGN_OF( val )               ( (val) == 0 ? 0 : ((val) > 0 ? 1 : -1) )
#endif  // MACRO_SIGN_OF

/**
 * \brief   Compares and returns maximum of 2 digits
 **/
#ifndef MACRO_MAX
    #define MACRO_MAX(a, b)                     ( (a) > (b) ? (a) : (b) )
#endif  // MACRO_MAX

/**
 * \brief   Compares and returns minimum of 2 digits
 **/
#ifndef MACRO_MIN
    #define MACRO_MIN(a, b)                     ( (a) < (b) ? (a) : (b) )
#endif  // MACRO_MIN

/**
 * \brief   Checks whether the x is in range of rmin and rmax
 **/
#ifndef MACRO_IN_RANGE
    #define MACRO_IN_RANGE(x, rmin, rmax)       ( ((rmin) <= (x)) && ((rmax) >= (x)) )
#endif // !MACRO_IN_RANGE

/**
 * \brief   Returns absolute value of digit. The return value is always positive
 **/
#ifndef MACRO_ABS
    #define MACRO_ABS(x)                        ( (x) < 0 ? -1 * (x) : (x) )
#endif  // MACRO_ABS

/**
 * \brief   Calculates and returns the nearest aligned value given length.
 * \param   len     The digit to align
 * \param   block   The value of alignment
 **/
#ifndef MACRO_ALIGN_SIZE
    #define MACRO_ALIGN_SIZE(len, block)        ( (((len) + ((block) - 1)) / (block)) * (block) )
#endif // MACRO_ALIGN_SIZE

/**
 * \brief   Returns the remaining value of 2 integer division operation, i.e. (x/a).
 *          For example, the remaining value of 105/27 is 24, i.e. (105 - 24) / 27 = 3.0
 **/
#ifndef MACRO_REMAIN_SIZE
    #define MACRO_REMAIN_SIZE(x, a)             ( (a) != 0 ? (x) - ((a) * ((x) / (a))) : 0 )
#endif  // MACRO_REMAIN_SIZE

#ifndef MACRO_MAKE_CONST_PTR
    #define MACRO_MAKE_CONST_PTR(ptr)		    ((const unsigned char *)(ptr))
#endif // !MACRO_MAKE_CONST_PTR

#ifndef MACRO_MAKE_PTR
    #define MACRO_MAKE_PTR(ptr)                 reinterpret_cast<unsigned char *>(ptr)
#endif // !MACRO_MAKE_PTR

#ifndef MACRO_MAKE_NUM_PTR
    #define MACRO_MAKE_NUM_PTR(type, num)       ((type)(num))
#endif  // MACRO_MAKE_NUM_PTR

#ifndef MACRO_MAKE_NUMBER64
    #define MACRO_MAKE_NUMBER64(num)            ((uint64_t)(num))
#endif // !MACRO_MAKE_NUMBER64

#ifndef MACRO_MAKE_NUMBER32
    #define MACRO_MAKE_NUMBER32(num)	        (uint32_t)MACRO_MAKE_NUMBER64(num)
#endif // !MACRO_MAKE_NUMBER32

#ifndef MACRO_MAKE_NUMBER
    #define MACRO_MAKE_NUMBER(num)              ((size_t)(num))
#endif  // !MACRO_MAKE_NUMBER

#ifndef MACRO_PTR2NUMBER
    #define MACRO_PTR2NUMBER(ptr)               static_cast<size_t>( MACRO_MAKE_CONST_PTR(ptr) - MACRO_MAKE_CONST_PTR(nullptr) )
#endif  // MACRO_PTR2NUMBER

#ifndef MACRO_PTR2INT32
    #define MACRO_PTR2INT32(ptr)                static_cast<uint32_t>( MACRO_PTR2NUMBER(ptr) )
#endif  // MACRO_PTR2INT32

#ifndef MACRO_COUNT_SIZE
    #define MACRO_COUNT_SIZE(first, last)       static_cast<int32_t>( MACRO_MAKE_CONST_PTR(last) - MACRO_MAKE_CONST_PTR(first) )
#endif  // MACRO_COUNT_SIZE

#ifndef MACRO_ELEM_COUNT
    #define MACRO_ELEM_COUNT(first, last)       static_cast<int32_t>( last - first )
#endif // !MACRO_ELEM_COUNT

#ifndef MACRO_OFFSETOF
    #define MACRO_OFFSETOF(Cls, mem )           static_cast<uint32_t>((size_t)&(((Cls *)0)->mem))
#endif // !MACRO_OFFSETOF

//!< MACRO to make strings.
#ifndef MACRO_MAKE_STRING
    #ifdef MS_VISUAL_CPP
        //!< This macro makes a message string
        #define MACRO_MAKE_STRING(x)        #x
        //!< This macro converts a value to a string
        #define MACRO_CONV_STRING(x)        MACRO_MAKE_STRING(x)
    #else   // MS_VISUAL_CPP
        #define MACRO_MAKE_STRING(x)
        #define MACRO_CONV_STRING(x)
    #endif  // MS_VISUAL_CPP
#endif // !MACRO_MAKE_STRING

//!< This macro makes a message
#ifndef MACRO_MAKE_MESSAGE
    #ifdef MS_VISUAL_CPP
        #define MACRO_MAKE_MESSAGE(x)       ">>> " MACRO_MAKE_STRING(x)
    #else   // MS_VISUAL_CPP
        #define MACRO_MAKE_MESSAGE(msg)
    #endif  // MS_VISUAL_CPP
#endif // !MACRO_MAKE_MESSAGE

//!< This macro creates and outputs compile time message
#ifndef MACRO_COMPILER_MESSAGE
    #ifdef MS_VISUAL_CPP
        #define MACRO_COMPILER_MESSAGE(msg) __pragma(message(MACRO_MAKE_MESSAGE(msg)))
    #else   // MS_VISUAL_CPP
        #define MACRO_COMPILER_MESSAGE(msg)
    #endif  // MS_VISUAL_CPP
#endif // !MACRO_COMPILER_MESSAGE

//!< This macro creates and outputs compile time message with prefix "TODO" and the message,
//! followed with the file name and the line number to prompt.
#ifndef MACRO_TODO
    #ifdef MS_VISUAL_CPP
        #define MACRO_TODO(msg)             __pragma(message(">>> TODO :: " MACRO_MAKE_STRING(msg) ": here --> " __FILE__":"MACRO_CONV_STRING(__LINE__)))
    #else   // MS_VISUAL_CPP
        #define MACRO_TODO(msg)
    #endif  // MS_VISUAL_CPP
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
        #define VERIFY(x)                       (x)
    #endif   // VERIFY

    #ifndef  VERIFY_MSG
        #define VERIFY_MSG(x, msg)              (x)
    #endif   // VERIFY_MSG

#endif   // _DEBUG

/**
 * \brief   defines some switches and macros to use in debug version
 **/
#if defined(_DEBUG) && defined(_WINDOWS)
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
