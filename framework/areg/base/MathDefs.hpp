#ifndef AREG_BASE_MATHDEFS_HPP
#define AREG_BASE_MATHDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/MathDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Basic Math functionalities.
 *              Functions in this namespace are global
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/IOStream.hpp"
#include "areg/base/TableDefs.hpp"

#include <algorithm>
#include <string_view>
#include <type_traits>
#include <cmath>

/**
 * \brief       Basic Math functions, helper classes and CRC
 **/
namespace areg {
/************************************************************************/
// Math types
/************************************************************************/
/**
 * \brief   The enumeration of digit signs
 *          
 **/
enum class NumericSign    : int8_t
{
      Negative  = -1    //!< The sign is negative
    , Undefined =  0    //!< The sign is undefined.<br>By default, undefined is considered as positive.
    , Positive  =  1    //!< The sign is positive
};

/**
 * \brief   Returns character representation of numeric sign: '-' for negative, '+' for
 *          positive, or '\0' for zero.
 **/
[[nodiscard]]
inline constexpr char as_char(areg::NumericSign sign ) noexcept;

/**
 * \brief   The enum defines comparing results
 **/
enum class Ordering : int8_t
{
      Smaller   = -1    //!< Comparing result is smaller
    , Equal     =  0    //!< Comparing result is equal
    , Bigger    =  1    //!< Comparing result is greater
};

/**
 * \brief   The structure of rectangle object, 
 *          which contains coordinate of the top-left corner, width and height.
 **/
struct Rect
{
    int32_t     x       { 0 };  //!< Top left corner, X-coordinate.
    int32_t     y       { 0 };  //!< Top left corner, Y-coordinate.
    uint32_t    width   { 0 };  //!< Width of the rectangle.
    uint32_t    height  { 0 };  //!< Height of the rectangle.
};

/**
 * \brief   The structure of the size object, which contains width and height values.
 **/
struct Size
{
    uint32_t    width   { 0 };  //!< The width of the size object.
    uint32_t    height  { 0 };  //!< The height of the size object.
};

/**
 * \brief   The structure of the point object, which contains X- and Y-coordinates.
 **/
struct Point
{
    int32_t     x;  //!< The X-coordinate of the point.
    int32_t     y;  //!< The Y-coordinate of the point.
};

/**
 * \brief   The structure of the Coordinate structure, which contains X- and Y-coordinates of a position.
 **/
struct Coord
{
    int32_t     posX;   //!< The X-coordinate of a position.
    int32_t     posY;   //!< The Y-coordinate of a position.
};

/************************************************************************/
// areg constants
/************************************************************************/

/**
 * \brief   No checksum is set, ignore.
 **/
constexpr uint32_t  CHECKSUM_IGNORE     { 0u };

/**
 * \brief   Invalid Checksum.
 **/
constexpr uint32_t  CHECKSUM_INVALID    { ~0u };

/************************************************************************/
// areg namespace structures
/************************************************************************/

/**
 * \brief   The union object of Large Integer,
 *          which consists of 32-bit low part integer and 32-bit high part integer.
 *          areg::Int64Parts
 **/
union  Int64Parts
{
    /**
     * \brief   Named structure
     **/
    struct _name
    {
        uint32_t    lowPart;   //!< Low part 32-bit integer
        uint32_t    highPart;  //!< High part 32-bit integer
    } u;

    uint64_t        quadPart;  //!< 64-bit integer
};

/**
 * \brief   Compares two 64-bit integer parts and returns the ordering result.
 **/
[[nodiscard]]
inline constexpr areg::Ordering compare_parts(const areg::Int64Parts& lhs, const areg::Int64Parts& rhs) noexcept;

/**
 * \brief   A 64-bit structure with separate high and low 32-bit values.
 **/
struct AREG_API LargeInteger
{
//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
    uint32_t    hiBits{ 0u };   // the hight 32-bit value of a Large Number structure
    uint32_t    loBits{ 0u };   // the low   32-bit value of a Large Number structure

//////////////////////////////////////////////////////////////////////////
// Constructors
//////////////////////////////////////////////////////////////////////////
    constexpr LargeInteger() noexcept = default;

    constexpr LargeInteger(const areg::LargeInteger& src) noexcept = default;

    constexpr LargeInteger(areg::LargeInteger&& src) noexcept = default;

    /**
     * \brief   Sets large integer high and low 32-bit values.
     **/
    inline constexpr LargeInteger( uint32_t hi, uint32_t lo ) noexcept;

    /**
     * \brief   Initializes the large integer by splitting a 64-bit value into high and low parts.
     **/
    inline constexpr LargeInteger( uint64_t num ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// areg::LargeInteger converting operators
/************************************************************************/
    /**
     * \brief   Converts the large integer to a 64-bit unsigned integer.
     **/
    [[nodiscard]]
    inline constexpr operator uint64_t () const noexcept;

/************************************************************************/
// areg::LargeInteger simple math operations
/************************************************************************/

    constexpr areg::LargeInteger& operator = ( const areg::LargeInteger & src ) noexcept = default;

    constexpr areg::LargeInteger& operator = ( areg::LargeInteger && src ) noexcept = default;

    [[nodiscard]]
    inline constexpr areg::LargeInteger & operator = ( uint64_t src ) noexcept;

    [[nodiscard]]
    inline constexpr uint64_t operator + ( const areg::LargeInteger & rhs ) const noexcept;

    [[nodiscard]]
    inline constexpr uint64_t operator + ( uint64_t rhs ) const noexcept;

    [[nodiscard]]
    inline constexpr areg::LargeInteger & operator += ( const areg::LargeInteger & rhs ) noexcept;

    [[nodiscard]]
    inline constexpr areg::LargeInteger & operator += ( uint64_t rhs ) noexcept;

    [[nodiscard]]
    inline constexpr uint64_t operator - ( const areg::LargeInteger & rhs ) const noexcept;

    [[nodiscard]]
    inline constexpr uint64_t operator - ( uint64_t rhs ) const noexcept;

    [[nodiscard]]
    inline constexpr areg::LargeInteger & operator -= ( const areg::LargeInteger & rhs ) noexcept;

    [[nodiscard]]
    inline constexpr areg::LargeInteger & operator -= ( uint64_t rhs ) noexcept;

    [[nodiscard]]
    inline constexpr bool operator == ( const areg::LargeInteger & other ) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator == ( uint64_t other ) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator != ( const areg::LargeInteger & other ) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator != ( uint64_t other ) const noexcept;

/************************************************************************/
// areg::LargeInteger declare global operators to make streamable
/************************************************************************/
    AREG_DECLARE_STREAMABLE(areg::LargeInteger);
};

/************************************************************************/
// Basic math global functions
/************************************************************************/
/**
 * \brief   Returns the sign of a value that supports comparison with zero and operators < and >.
 *
 * \param   val     Value to determine sign (primitive or object supporting < and > operators).
 * \return  Returns areg::SignNegative if val < 0, areg::SignPositive if val > 0,
 *          areg::SignUndefined if val == 0.
 **/
template<typename Type>
[[nodiscard]]
inline constexpr areg::NumericSign sign( const Type & val ) noexcept;

/************************************************************************/
// areg namespace utility functions, operations with Large Number
/************************************************************************/
/**
 * \brief   Sets high and low 32-bit values in a 64-bit LargeInteger structure.
 *
 * \param[out]  num     LargeInteger structure to set bits in.
 * \param       hi      High 32 bits to set.
 * \param       lo      Low 32 bits to set.
 **/
inline constexpr void set_bits( LargeInteger & num, uint32_t hi, uint32_t lo) noexcept;

/**
 * \brief   Returns the high 32-bit value from a 64-bit LargeInteger.
 *
 * \param   num     LargeInteger structure to extract high bits from.
 **/
[[nodiscard]]
inline constexpr uint32_t high_bits(const LargeInteger & num) noexcept;

/**
 * \brief   Returns the low 32-bit value from a 64-bit LargeInteger.
 *
 * \param   num     LargeInteger structure to extract low bits from.
 **/
[[nodiscard]]
inline constexpr uint32_t low_bits(const LargeInteger & num) noexcept;

/************************************************************************/
// areg namespace utility functions, check-sum operations
/************************************************************************/

/**
 * \brief   Calculates 32-bit CRC of a binary buffer using IEEE 802.3 standard.
 *          Usable at compile time when both data and size are constexpr.
 *
 * \param   data    Pointer to binary data buffer.
 * \param   size    Number of bytes to process. Negative or zero: returns CRC of empty input.
 * \return  32-bit CRC value.
 **/
[[nodiscard]]
inline constexpr uint32_t crc32_calculate( const uint8_t * data, int32_t size ) noexcept;

/**
 * \brief   Calculates 32-bit CRC of a null-terminated string using IEEE 802.3 standard.
 *          Usable at compile time when strData is a string literal or constexpr pointer.
 *
 * \param   strData     Null-terminated string. nullptr returns CRC of empty input.
 * \return  32-bit CRC value.
 **/
[[nodiscard]]
inline constexpr uint32_t crc32_calculate( const char * strData ) noexcept;

/**
 * \brief   Calculates 32-bit CRC of a null-terminated wide string using IEEE 802.3 standard.
 *          Characters in the Basic Multilingual Plane (U+0000-U+00FF) are processed as a
 *          single byte each; characters in U+0100-U+FFFF are processed as two bytes (low then
 *          high). Characters above U+FFFF are represented with their lower 16 bits only.
 *          Usable at compile time when strData is a wide string literal or constexpr pointer.
 *
 * \param   strData     Null-terminated wide string. nullptr returns CRC of empty input.
 * \return  32-bit CRC value.
 **/
[[nodiscard]]
inline constexpr uint32_t crc32_calculate( const wchar_t * strData ) noexcept;

/**
 * \brief   Continues 32-bit CRC calculation on a binary buffer for incremental processing.
 *          Usable at compile time when all arguments are constexpr.
 *
 * \param   crcInit     Seed value: use crc32_init() on the first call, or the value
 *                      returned by a previous crc32_start() call.
 * \param   data        Pointer to binary data. Returns crcInit unchanged when nullptr or size <= 0.
 * \param   size        Number of bytes to process.
 * \return  Intermediate CRC value -- pass to the next crc32_start() call or to crc32_finish().
 **/
[[nodiscard]]
inline constexpr uint32_t crc32_start( uint32_t crcInit, const uint8_t * data, int32_t size ) noexcept;

/**
 * \brief   Continues 32-bit CRC calculation on a null-terminated string for incremental processing.
 *          Usable at compile time when all arguments are constexpr.
 *
 * \param   crcInit     Seed value: use crc32_init() on the first call, or the value
 *                      returned by a previous crc32_start() call.
 * \param   data        Null-terminated string. Returns crcInit unchanged when nullptr or empty.
 * \return  Intermediate CRC value -- pass to the next crc32_start() call or to crc32_finish().
 **/
[[nodiscard]]
inline constexpr uint32_t crc32_start( uint32_t crcInit, const char * data ) noexcept;

/**
 * \brief   Continues 32-bit CRC calculation on a null-terminated string for incremental processing.
 *          Usable at compile time when all arguments are constexpr.
 *
 * \param   crcInit     Seed value: use crc32_init() on the first call, or the value
 *                      returned by a previous crc32_start() call.
 * \param   data        String view. Returns crcInit unchanged when nullptr or empty.
 * \return  Intermediate CRC value -- pass to the next crc32_start() call or to crc32_finish().
 **/
[[nodiscard]]
inline constexpr uint32_t crc32_start(uint32_t crcInit, std::string_view data) noexcept;

/**
 * \brief   Continues 32-bit CRC calculation on a single byte for incremental processing.
 *          Usable at compile time when all arguments are constexpr.
 *
 * \param   crcInit     Seed value: use crc32_init() on the first call, or the value
 *                      returned by a previous crc32_start() call.
 * \param   uch         Single byte to process.
 * \return  Intermediate CRC value -- pass to the next crc32_start() call or to crc32_finish().
 **/
[[nodiscard]]
inline constexpr uint32_t crc32_start( uint32_t crcInit, uint8_t uch ) noexcept;

/**
 * \brief   Calculates 32-bit CRC of a std::string_view using IEEE 802.3 standard.
 *          Does not rely on null termination; uses the view's length directly.
 *          Usable at compile time when str is a constexpr string_view.
 *
 * \param   str     String view to calculate CRC for. Empty view returns CRC of empty input.
 * \return  32-bit CRC value.
 **/
[[nodiscard]]
inline constexpr uint32_t crc32_calculate( std::string_view str ) noexcept;

/**
 * \brief   Returns the initial seed value for incremental CRC32 calculations.
 *          Pass the result to the first crc32_start() call.
 **/
[[nodiscard]]
inline constexpr uint32_t crc32_init() noexcept;

/**
 * \brief   Finalizes an incremental CRC32 calculation and returns the complete checksum.
 *
 * \param   crc     Intermediate CRC value from the last crc32_start() call.
 * \return  Final 32-bit CRC value.
 **/
[[nodiscard]]
inline constexpr uint32_t crc32_finish( uint32_t crc ) noexcept;

/**
 * \brief   Calculates 32-bit CRC of a binary buffer using hardware acceleration if available.
 *          Returns crc32_calculate() result if hardware support is not present.
 *
 * \param   data    Pointer to binary data buffer.
 * \param   size    Number of bytes to process. Negative or zero: returns CRC of empty input.
 * \return  32-bit CRC value.
 **/
[[nodiscard]]
inline constexpr uint32_t crc32_hardware(const uint8_t* data, int32_t size) noexcept;

/**
 * \brief   Return true if CRC32 number is valid, i.e. it is neither 0x00000000, nor 0xFFFFFFFF.
 **/
[[nodiscard]]
inline constexpr bool crc32_valid(const uint32_t crc32Number) noexcept;

/**
 * \brief   Returns the rounded nearest integer value.
 *
 * \param   val     Floating-point value to round.
 **/
[[nodiscard]]
inline constexpr double round( double val ) noexcept;

/**
 * \brief   Returns the absolute value of a type that supports comparison with zero and multiplication.
 *
 * \param   val     Value to get absolute value of (requires < operator and multiplication support).
 **/
template <typename Type>
[[nodiscard]]
inline constexpr Type abs(const Type& val) noexcept;

/**
 * \brief   Compares two values and returns areg::Ordering::Equal, Bigger, or Smaller.
 *
 * \param   left        Left-hand value to compare.
 * \param   right       Right-hand value to compare.
 * \return  areg::Ordering::Equal if equal, Bigger if left > right, Smaller if left < right.
 **/
template<typename Type>
[[nodiscard]]
inline constexpr areg::Ordering compare(const Type & left, const Type & right) noexcept;

/************************************************************************/
// Numeric utility functions
/************************************************************************/

/**
 * \brief   Returns true if value is within the inclusive range [rangeMin, rangeMax].
 *
 * \param   value       Value to check.
 * \param   rangeMin    Minimum of the range (inclusive).
 * \param   rangeMax    Maximum of the range (inclusive).
 **/
template <typename T>
[[nodiscard]]
inline constexpr bool is_in_range(T value, T rangeMin, T rangeMax) noexcept;

/**
 * \brief   Returns the smallest multiple of block that is greater than or equal to len.
 *
 * \param   len         Value to align.
 * \param   block       Alignment block size.
 * \return  Aligned value.
 **/
template <typename T>
[[nodiscard]]
inline constexpr T align_size(T len, T block) noexcept;

/************************************************************************/
// Bit manipulation functions
/************************************************************************/

/**
 * \brief   Extracts the lowest 8 bits from the given integral value.
 *
 * \param   value       Integral value to extract low byte from.
 **/
template <typename T>
[[nodiscard]]
inline constexpr uint8_t lo_byte(T value) noexcept;

/**
 * \brief   Extracts the highest 8 bits from the given integral value.
 *
 * \param   value       Integral value to extract high byte from.
 **/
template <typename T>
[[nodiscard]]
inline constexpr uint8_t hi_byte(T value) noexcept;

/**
 * \brief   Extracts the low 16 bits from a 32-bit value.
 *
 * \param   value       32-bit value to extract low word from.
 **/
[[nodiscard]]
inline constexpr uint16_t lo_word(uint32_t value) noexcept;

/**
 * \brief   Extracts the high 16 bits from a 32-bit value.
 *
 * \param   value       32-bit value to extract high word from.
 **/
[[nodiscard]]
inline constexpr uint16_t hi_word(uint32_t value) noexcept;

/**
 * \brief   Extracts the low 32 bits from a 64-bit value.
 *
 * \param   value       64-bit value to extract low dword from.
 **/
[[nodiscard]]
inline constexpr uint32_t lo_dword(uint64_t value) noexcept;

/**
 * \brief   Extracts the high 32 bits from a 64-bit value.
 *
 * \param   value       64-bit value to extract high dword from.
 **/
[[nodiscard]]
inline constexpr uint32_t hi_dword(uint64_t value) noexcept;

/**
 * \brief   Swaps byte order of a 16-bit or 32-bit integral value.
 *
 * \param   value       Value whose bytes to swap.
 **/
template <typename T>
[[nodiscard]]
inline constexpr T swap_bytes(T value) noexcept;

/**
 * \brief   Composes a 16-bit value from two 8-bit values (high and low parts).
 *
 * \param   hi      High 8 bits.
 * \param   lo      Low 8 bits.
 **/
[[nodiscard]]
inline constexpr uint16_t make16(uint8_t hi, uint8_t lo) noexcept;

/**
 * \brief   Composes a 32-bit value from two 16-bit values (high and low parts).
 *
 * \param   hi      High 16 bits.
 * \param   lo      Low 16 bits.
 **/
[[nodiscard]]
inline constexpr uint32_t make32(uint16_t hi, uint16_t lo) noexcept;

/**
 * \brief   Composes a 64-bit value from two 32-bit values (high and low parts).
 *
 * \param   hi      High 32 bits.
 * \param   lo      Low 32 bits.
 **/
[[nodiscard]]
inline constexpr uint64_t make64(uint32_t hi, uint32_t lo) noexcept;

/**
 * \brief   Constructs a 32-bit value from four bytes (most significant first).
 *
 * \param   b3      Bits [31:24].
 * \param   b2      Bits [23:16].
 * \param   b1      Bits [15:8].
 * \param   b0      Bits [7:0].
 **/
[[nodiscard]]
inline constexpr uint32_t construct32(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0) noexcept;

/**
 * \brief   Constructs a 64-bit value from eight bytes (most significant first).
 *
 * \param   b7      Bits [63:56].
 * \param   b6      Bits [55:48].
 * \param   b5      Bits [47:40].
 * \param   b4      Bits [39:32].
 * \param   b3      Bits [31:24].
 * \param   b2      Bits [23:16].
 * \param   b1      Bits [15:8].
 * \param   b0      Bits [7:0].
 **/
[[nodiscard]]
inline constexpr uint64_t construct64(uint8_t b7, uint8_t b6, uint8_t b5, uint8_t b4,
                                      uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0) noexcept;

/**
 * \brief   Returns the absolute difference between two values.
 *
 * \param   a       First value.
 * \param   b       Second value.
 **/
template <typename T>
[[nodiscard]]
inline constexpr T delta(T a, T b) noexcept;

/************************************************************************/
// areg::LargeInteger declare global operators to make streamable
/************************************************************************/
AREG_IMPLEMENT_STREAMABLE(areg::LargeInteger)

//////////////////////////////////////////////////////////////////////////
// areg namespace inline function implementation
//////////////////////////////////////////////////////////////////////////

inline constexpr uint32_t crc32_init() noexcept
{
    return static_cast<uint32_t>(~0u);
}

inline constexpr uint32_t crc32_finish(uint32_t crc) noexcept
{
    return (~crc);
}

inline constexpr uint32_t crc32_calculate( const uint8_t * data, int32_t size ) noexcept
{
    uint32_t crc { crc32_init() };
    for (; size > 0; --size, ++data)
    {
        crc = (crc >> 8) ^ areg::CRC32_TABLE[*data ^ static_cast<uint8_t>(crc & 0xFFu)];
    }

    return crc32_finish(crc);
}

inline constexpr uint32_t crc32_calculate( const char * strData ) noexcept
{
    uint32_t crc { crc32_init() };
    if ( strData != nullptr )
    {
        for (; *strData != '\0'; ++strData)
        {
            crc = (crc >> 8) ^ areg::CRC32_TABLE[static_cast<uint8_t>(*strData) ^ static_cast<uint8_t>(crc & 0xFFu)];
        }
    }
    return crc32_finish(crc);
}

inline constexpr uint32_t crc32_calculate( const wchar_t * strData ) noexcept
{
    uint32_t crc { crc32_init() };
    if ( strData != nullptr )
    {
        for ( ; *strData != L'\0'; ++strData )
        {
            // Represent the wchar as uint16_t.
            // Characters above U+FFFF (32-bit wchar on Linux) are folded to their lower 16 bits.
            const uint16_t wch { static_cast<uint16_t>(*strData) };
            if ( wch <= static_cast<uint16_t>(0x00FFu) )
            {
                // ASCII and Latin-1: single byte
                crc = (crc >> 8) ^ areg::CRC32_TABLE[ static_cast<uint8_t>(wch) ^ static_cast<uint8_t>(crc & 0xFFu) ];
            }
            else
            {
                // BMP above U+00FF: process low byte then high byte
                const uint8_t lo { lo_byte(wch) };
                const uint8_t hi { hi_byte(wch) };
                crc = (crc >> 8) ^ areg::CRC32_TABLE[ lo ^ static_cast<uint8_t>(crc & 0xFFu) ];
                crc = (crc >> 8) ^ areg::CRC32_TABLE[ hi ^ static_cast<uint8_t>(crc & 0xFFu) ];
            }
        }
    }
    return crc32_finish(crc);
}

inline constexpr uint32_t crc32_start( uint32_t crcInit, const uint8_t * data, int32_t size ) noexcept
{
    uint32_t crc { crcInit };
    if ( data != nullptr )
    {
        for (; size > 0; --size, ++data)
        {
            crc = (crc >> 8) ^ areg::CRC32_TABLE[*data ^ static_cast<uint8_t>(crc & 0xFFu)];
        }
    }
    return crc;
}

inline constexpr uint32_t crc32_start( uint32_t crcInit, const char * data ) noexcept
{
    uint32_t crc { crcInit };
    if ( data != nullptr )
    {
        for (; *data != '\0'; ++data)
        {
            crc = (crc >> 8) ^ areg::CRC32_TABLE[static_cast<uint8_t>(*data) ^ static_cast<uint8_t>(crc & 0xFFu)];
        }
    }

    return crc;
}

inline constexpr uint32_t crc32_start(uint32_t crcInit, std::string_view data) noexcept
{
    uint32_t crc{ crcInit };
    uint32_t len{ static_cast<uint32_t>(data.length()) };
    for (uint32_t i{ 0u }; i < len; ++i)
    {
        crc = (crc >> 8) ^ areg::CRC32_TABLE[static_cast<uint8_t>(data[i]) ^ static_cast<uint8_t>(crc & 0xFFu)];
    }

    return crc;
}

inline constexpr uint32_t crc32_start( uint32_t crcInit, uint8_t uch ) noexcept
{
    return (crcInit >> 8) ^ areg::CRC32_TABLE[ uch ^ static_cast<uint8_t>(crcInit & 0xFFu) ];
}

inline constexpr uint32_t crc32_calculate( std::string_view str ) noexcept
{
    uint32_t crc { crc32_init() };
    for (char ch : str)
    {
        crc = (crc >> 8) ^ areg::CRC32_TABLE[static_cast<uint8_t>(ch) ^ static_cast<uint8_t>(crc & 0xFFu)];
    }

    return crc32_finish(crc);
}

#if defined(__SSE4_2__) || (defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86)))
    #include <nmmintrin.h>
    // HW intrinsics cannot be evaluated at compile time, not `constexpr` method.
    // Computes CRC32C (Castagnoli) - same polynomial as CRC32_TABLE, so SW and HW results are identical.
    inline uint32_t _crc32_hardware(const uint8_t* data, int32_t size) noexcept
    {
        uint32_t crc = crc32_init();
    #if defined(__x86_64__) || defined(__amd64__) || defined(_M_X64)
        const uint8_t* end8 = data + (size & ~7);
        for (; data < end8; data += 8)
            crc = static_cast<uint32_t>(_mm_crc32_u64(crc, *reinterpret_cast<const uint64_t*>(data)));
        for (; data < end8 + (size & 7); ++data)
            crc = _mm_crc32_u8(crc, *data);
    #else
        const uint8_t* end4 = data + (size & ~3);
        for (; data < end4; data += 4)
            crc = _mm_crc32_u32(crc, *reinterpret_cast<const uint32_t*>(data));
        for (; data < end4 + (size & 3); ++data)
            crc = _mm_crc32_u8(crc, *data);
    #endif
        return crc32_finish(crc);
    }
#endif

// At compile time uses the pure-software path (`constexpr`); at runtime on SSE4.2 x86/x64 uses hardware intrinsics.
// __builtin_is_constant_evaluated() is a C++17 extension supported by all three toolchains this SDK targets.
inline constexpr uint32_t crc32_hardware(const uint8_t* data, int32_t size) noexcept
{
#if defined(__SSE4_2__) || (defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86)))
    if (__builtin_is_constant_evaluated())
        return crc32_calculate(data, size);  // compile-time: software path
    return _crc32_hardware(data, size);      // runtime: SIMD hardware path
#else
    return crc32_calculate(data, size);
#endif
}

constexpr bool crc32_valid(const uint32_t crc32Number) noexcept
{
    return (crc32Number != areg::CHECKSUM_INVALID) && (crc32Number != areg::CHECKSUM_IGNORE);
}

/************************************************************************/
// Compile-time CRC32C collision detection
/************************************************************************/

/**
 * \brief   Scans a compile-time array of names and returns the index of the
 *          first name whose CRC32C collides with any later name in the array.
 *          Returns N (the array size) when no collision is found.
 *
 * \tparam  N       Number of names in the array (deduced automatically).
 * \param   names   Array of string_view values to check.
 * \return  Index of the first colliding name, or N if all hashes are unique.
 **/
template<std::size_t N>
[[nodiscard]] constexpr std::size_t crc32_first_collision(const std::string_view (&names)[N]) noexcept
{
    for (std::size_t i = 0u; i < N; ++i)
    {
        const uint32_t ci { crc32_calculate(names[i]) };
        for (std::size_t j = i + 1u; j < N; ++j)
        {
            if (ci == crc32_calculate(names[j]))
                return i;
        }
    }

    return N;
}

/**
 * \brief   Compile-time CRC32C uniqueness guard.
 *
 *          Instantiate with the array size N and the result of
 *          crc32_first_collision(). When CollisionAt == N there is no
 *          collision and the type is well-formed. When CollisionAt < N the
 *          static_assert fires; the compiler error output includes the
 *          template arguments \<N, CollisionAt\>, so CollisionAt directly
 *          identifies the index of the first colliding name in the array.
 *
 *          Use the AREG_ASSERT_UNIQUE_CRC32 macro rather than instantiating
 *          this template directly.
 *
 * \tparam  N           Total number of names in the checked group.
 * \tparam  CollisionAt Index of the first CRC32C collision, or N if none.
 **/
template<std::size_t N, std::size_t CollisionAt>
struct ArRegUniqueCRC32
{
    static_assert(CollisionAt == N,
                  "CRC32C collision: two names in this group share the same hash value. "
                  "Inspect template argument 'CollisionAt' in the compiler error to identify "
                  "the index of the first colliding name in the array.");
};

/**
 * \def     AREG_ASSERT_UNIQUE_CRC32(names_array)
 * \brief   Verifies at compile time that no two entries in \a names_array
 *          produce the same CRC32C value.  Zero runtime cost.
 *
 *          Place this macro immediately after a \c constexpr \c string_view
 *          array in a namespace or struct scope:
 *
 *          \code
 *          namespace MyService {
 *              constexpr std::string_view METHOD_NAMES[] = {
 *                  "request_process_data",
 *                  "response_process_data",
 *                  "notify_status_changed"
 *              };
 *              AREG_ASSERT_UNIQUE_CRC32(METHOD_NAMES);
 *          }
 *          \endcode
 *
 *          If a collision exists the build fails with a message that includes
 *          the zero-based index of the first colliding name.
 **/
#ifdef DEBUG
#define AREG_ASSERT_UNIQUE_CRC32(names_array)                   \
    using AREG_CRC32_UNIQUE_ ## __LINE__ = ArRegUniqueCRC32<    \
          (sizeof(names_array) / sizeof((names_array)[0]))      \
        , crc32_first_collision(names_array)                    \
    >
#else   // NDEBUG
    #define AREG_ASSERT_UNIQUE_CRC32(names_array)
#endif  // DEBUG

inline constexpr double round(double val) noexcept
{
    return ((val) < 0.0 ? std::ceil(val - 0.5) : std::floor(val + 0.5));
}

template <typename Type>
inline constexpr Type abs(const Type& val) noexcept
{
    return (val >= 0 ? val : static_cast<Type>(val) * static_cast<Type>(-1));
}

template<typename Type>
inline constexpr areg::NumericSign sign( const Type & val ) noexcept
{
    return static_cast<areg::NumericSign>((val > 0) - (val < 0));
}

template<typename Type>
inline constexpr areg::Ordering compare(const Type & left, const Type & right) noexcept
{
    return (left == right ? areg::Ordering::Equal : (left > right ? areg::Ordering::Bigger : areg::Ordering::Smaller));
}

inline constexpr char as_char(areg::NumericSign sign) noexcept
{
    switch (sign)
    {
    case areg::NumericSign::Negative:
        return '-';
    case areg::NumericSign::Positive:
        return '+';
    case areg::NumericSign::Undefined: // fall through
    default:
        return '\0';
    }
}

constexpr areg::Ordering compare_parts(const areg::Int64Parts& lhs, const areg::Int64Parts& rhs) noexcept
{
    if (lhs.u.highPart < rhs.u.highPart)
        return areg::Ordering::Smaller;
    else if (lhs.u.highPart > rhs.u.highPart)
        return areg::Ordering::Bigger;
    else if (lhs.u.lowPart < rhs.u.lowPart)
        return areg::Ordering::Smaller;
    else if (lhs.u.lowPart > rhs.u.lowPart)
        return areg::Ordering::Bigger;
    else
        return areg::Ordering::Equal;
}

inline constexpr void set_bits( LargeInteger &num, uint32_t hi, uint32_t lo ) noexcept
{
    num.hiBits = hi;
    num.loBits = lo;
}

inline constexpr uint32_t high_bits( const LargeInteger &num ) noexcept
{
    return (num.hiBits);
}

inline constexpr uint32_t low_bits( const LargeInteger &num ) noexcept
{
    return (num.loBits);
}

/************************************************************************/
// areg numeric utility inline functions implementation
/************************************************************************/

template <typename T>
inline constexpr bool is_in_range(T value, T rangeMin, T rangeMax) noexcept
{
    return (rangeMin <= value) && (value <= rangeMax);
}

template <typename T>
inline constexpr T align_size(T len, T block) noexcept
{
    return (((len) + (block - 1)) / block) * block;
}

/************************************************************************/
// areg bit manipulation inline functions implementation
/************************************************************************/

template <typename T>
inline constexpr uint8_t lo_byte(T value) noexcept
{
    static_assert(std::is_integral_v<T>, "lo_byte requires an integral type");
    return static_cast<uint8_t>(static_cast<std::make_unsigned_t<T>>(value) & 0xFFu);
}

template <typename T>
inline constexpr uint8_t hi_byte(T value) noexcept
{
    static_assert(std::is_integral_v<T>, "hi_byte requires an integral type");
    return static_cast<uint8_t>((static_cast<std::make_unsigned_t<T>>(value) >> ((sizeof(T) - 1) * 8)) & 0xFFu);
}

inline constexpr uint16_t lo_word(uint32_t value) noexcept
{
    return static_cast<uint16_t>(value & 0xFFFFu);
}

inline constexpr uint16_t hi_word(uint32_t value) noexcept
{
    return static_cast<uint16_t>((value >> 16) & 0xFFFFu);
}

inline constexpr uint32_t lo_dword(uint64_t value) noexcept
{
    return static_cast<uint32_t>(value & 0xFFFFFFFFu);
}

inline constexpr uint32_t hi_dword(uint64_t value) noexcept
{
    return static_cast<uint32_t>((value >> 32) & 0xFFFFFFFFu);
}

template <typename T>
inline constexpr T swap_bytes(T value) noexcept
{
    static_assert(std::is_integral_v<T>, "swap_bytes requires an integral type");
    static_assert(sizeof(T) == 2 || sizeof(T) == 4, "swap_bytes supports only 16-bit and 32-bit types");

    using U = std::make_unsigned_t<T>;
    U n = static_cast<U>(value);

    if constexpr (sizeof(T) == 2)
    {
        return static_cast<T>(((n << 8) & 0xFF00u) | ((n >> 8) & 0x00FFu));
    }
    else
    {
        return static_cast<T>(((n & 0x000000FFu) << 24) |
                              ((n & 0x0000FF00u) <<  8) |
                              ((n & 0x00FF0000u) >>  8) |
                              ((n & 0xFF000000u) >> 24));
    }
}

inline constexpr uint16_t make16(uint8_t hi, uint8_t lo) noexcept
{
    return (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
}

inline constexpr uint32_t make32(uint16_t hi, uint16_t lo) noexcept
{
    return (static_cast<uint32_t>(hi) << 16) | static_cast<uint32_t>(lo);
}

inline constexpr uint64_t make64(uint32_t hi, uint32_t lo) noexcept
{
    return (static_cast<uint64_t>(hi) << 32) | static_cast<uint64_t>(lo);
}

inline constexpr uint32_t construct32(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0) noexcept
{
    return (static_cast<uint32_t>(b3) << 24) |
           (static_cast<uint32_t>(b2) << 16) |
           (static_cast<uint32_t>(b1) <<  8) |
           (static_cast<uint32_t>(b0));
}

inline constexpr uint64_t construct64(uint8_t b7, uint8_t b6, uint8_t b5, uint8_t b4,
                                      uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0) noexcept
{
    return (static_cast<uint64_t>(b7) << 56) |
           (static_cast<uint64_t>(b6) << 48) |
           (static_cast<uint64_t>(b5) << 40) |
           (static_cast<uint64_t>(b4) << 32) |
           (static_cast<uint64_t>(b3) << 24) |
           (static_cast<uint64_t>(b2) << 16) |
           (static_cast<uint64_t>(b1) <<  8) |
           (static_cast<uint64_t>(b0));
}

template <typename T>
inline constexpr T delta(T a, T b) noexcept
{
    return (a > b ? a - b : b - a);
}

/************************************************************************/
// areg::LargeInteger inline functions implementation
/************************************************************************/

inline constexpr areg::LargeInteger::LargeInteger( uint32_t hi, uint32_t lo ) noexcept
    : hiBits ( hi ), loBits ( lo ) 
{
}

inline constexpr areg::LargeInteger::LargeInteger( uint64_t num ) noexcept
    : hiBits ( 0), loBits ( 0 ) 
{
    Int64Parts li{};
    li.quadPart = num;
    hiBits = li.u.highPart;
    loBits = li.u.lowPart;
}

inline constexpr areg::LargeInteger::operator uint64_t () const noexcept
{
    Int64Parts li{ {hiBits, loBits} };
    return li.quadPart;
}

inline constexpr areg::LargeInteger& LargeInteger::operator = (uint64_t src) noexcept
{
    Int64Parts li{ };
    li.quadPart = src;
    hiBits = li.u.highPart;
    loBits = li.u.lowPart;
    return *this;
}

inline constexpr uint64_t areg::LargeInteger::operator + ( const areg::LargeInteger & rhs ) const noexcept
{
    return (static_cast<uint64_t>(*this) + static_cast<uint64_t>(rhs));
}

inline constexpr uint64_t areg::LargeInteger::operator + ( uint64_t rhs ) const noexcept
{
    return ( (static_cast<uint64_t>(*this) + rhs) );
}

inline constexpr areg::LargeInteger & areg::LargeInteger::operator += ( const areg::LargeInteger & rhs ) noexcept
{
    Int64Parts liLhs{ {this->hiBits, this->loBits} };
    Int64Parts liRhs{ {rhs.hiBits, rhs.loBits} };
    liRhs.quadPart += liRhs.quadPart;
    this->hiBits    = liRhs.u.highPart;
    this->loBits    = liLhs.u.lowPart;

    return (*this);
}

inline constexpr areg::LargeInteger & areg::LargeInteger::operator += ( uint64_t rhs ) noexcept
{
    Int64Parts li{ {this->hiBits, this->loBits} };
    li.quadPart    += rhs;
    this->hiBits    = li.u.highPart;
    this->loBits    = li.u.lowPart;

    return (*this);
}

inline constexpr uint64_t areg::LargeInteger::operator - ( const areg::LargeInteger & rhs ) const noexcept
{
    return ( static_cast<uint64_t>(*this) - static_cast<uint64_t>(rhs) );
}

inline constexpr uint64_t areg::LargeInteger::operator - ( uint64_t rhs ) const noexcept
{
    return ( static_cast<uint64_t>(*this) - rhs );
}

inline constexpr areg::LargeInteger & areg::LargeInteger::operator -= ( const areg::LargeInteger & rhs ) noexcept
{
    Int64Parts liLhs{ {this->hiBits, this->loBits} };
    Int64Parts liRhs{ {rhs.hiBits, rhs.loBits} };
    liRhs.quadPart -= liRhs.quadPart;
    this->hiBits    = liRhs.u.highPart;
    this->loBits    = liLhs.u.lowPart;

    return (*this);
}

inline constexpr areg::LargeInteger & areg::LargeInteger::operator -= ( uint64_t rhs ) noexcept
{
    Int64Parts li{ {this->hiBits, this->loBits} };
    li.quadPart -= rhs;
    this->hiBits = li.u.highPart;
    this->loBits = li.u.lowPart;

    return (*this);
}

inline constexpr bool areg::LargeInteger::operator == ( const areg::LargeInteger & other ) const noexcept
{
    return ((this == &other) || ((hiBits == other.hiBits) && (loBits == other.loBits)));
}

inline constexpr bool areg::LargeInteger::operator == ( uint64_t other ) const noexcept
{
    return ( static_cast<uint64_t>(*this) == other );
}

inline constexpr bool areg::LargeInteger::operator != (const areg::LargeInteger& other) const noexcept
{
    return ((this != &other) && ((hiBits != other.hiBits) || (loBits != other.loBits)));
}

inline constexpr bool areg::LargeInteger::operator != (uint64_t other) const noexcept
{
    return ( static_cast<uint64_t>(*this) != other );
}

} // namespace areg

#endif  // AREG_BASE_MATHDEFS_HPP
