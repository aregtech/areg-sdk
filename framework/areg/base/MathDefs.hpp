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
#include "areg/base/GEGlobal.h"
#include "areg/base/IOStream.hpp"

#include <type_traits>

/**
 * \brief       Basic Math functions, helper classes and CRC
 **/
namespace NEMath
{
/************************************************************************/
// NEMath types
/************************************************************************/
    /**
     * \brief   NEMath::NumericSign
     *          The enumeration of digit signs
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
     *          positive, or '0' for zero.
     *
     * \param   sign    NumericSign value to convert.
     * \return  Returns '-' for negative sign, '+' for positive sign, '0' for zero.
     **/
    inline char as_char(NEMath::NumericSign sign );

    /**
     * \brief   NEMath::Ordering
     *          The enum defines comparing results
     **/
    enum class Ordering : int8_t
    {
          Smaller   = -1    //!< Comparing result is smaller
        , Equal     =  0    //!< Comparing result is equal
        , Bigger    =  1    //!< Comparing result is greater
    };

    /**
     * \brief   NEMath::Rect
     *          The structure of rectangle object, 
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
     * \brief   NEMath::Size
     *          The structure of the size object, which contains width and height values.
     **/
    struct Size
    {
        uint32_t    width   { 0 };  //!< The width of the size object.
        uint32_t    height  { 0 };  //!< The height of the size object.
    };

    /**
     * \brief   NEMath::Point
     *          The structure of the point object, which contains X- and Y-coordinates.
     **/
    struct Point
    {
        int32_t     x;  //!< The X-coordinate of the point.
        int32_t     y;  //!< The Y-coordinate of the point.
    };

    /**
     * \brief   NEMath::Coord
     *          The structure of the Coordinate structure, which contains X- and Y-coordinates of a position.
     **/
    struct Coord
    {
        int32_t     posX;   //!< The X-coordinate of a position.
        int32_t     posY;   //!< The Y-coordinate of a position.
    };

/************************************************************************/
// NEMath constants
/************************************************************************/

    /**
     * \brief   NEMath::CHECKSUM_IGNORE
     *          No checksum is set, ignore.
     **/
    constexpr uint32_t  CHECKSUM_IGNORE     { 0u };

/************************************************************************/
// NEMath namespace structures
/************************************************************************/

    /**
     * \brief   The union object of Large Integer,
     *          which consists of 32-bit low part integer and 32-bit high part integer.
     *          NEMath::Int64Parts
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
     * \brief   A 64-bit structure with separate high and low 32-bit values.
     **/
    struct AREG_API LargeInteger
    {
    //////////////////////////////////////////////////////////////////////////
    // member variables
    //////////////////////////////////////////////////////////////////////////
        uint32_t    hiBits;     // the hight 32-bit value of a Large Number structure
        uint32_t    loBits;     // the low   32-bit value of a Large Number structure

    //////////////////////////////////////////////////////////////////////////
    // Constructors
    //////////////////////////////////////////////////////////////////////////
        /**
         * \brief   Creates a large integer with zero values.
         **/
        inline LargeInteger();
        /**
         * \brief   Initializes the large integer with high and low 32-bit values.
         *
         * \param   hi      The high 32-bit value.
         * \param   lo      The low 32-bit value.
         **/
        inline LargeInteger( uint32_t hi, uint32_t lo );
        /**
         * \brief   Initializes the large integer by splitting a 64-bit value into high and low
         *          parts.
         *
         * \param   num     The 64-bit value to split into high and low 32-bit parts.
         **/
        inline LargeInteger( uint64_t num );
        /**
         * \brief
         *
         * \param   src     The source to copy from.
         * \note    Copy constructor.
         **/
        inline LargeInteger( const NEMath::LargeInteger & src );

        /**
         * \brief
         *
         * \param   src     The source to move from.
         * \note    Move constructor.
         **/
        inline LargeInteger( NEMath::LargeInteger && src ) noexcept;

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////

    /************************************************************************/
    // NEMath::LargeInteger converting operators
    /************************************************************************/
        /**
         * \brief   Converts the large integer to a 64-bit unsigned integer.
         **/
        inline operator uint64_t () const;

    /************************************************************************/
    // NEMath::LargeInteger simple math operations
    /************************************************************************/
        /**
         * \brief   Assigns high and low 32-bit values from the source.
         *
         * \param   src     The source large integer to copy from.
         * \return  Reference to this large integer.
         **/
        inline NEMath::LargeInteger & operator = ( const NEMath::LargeInteger & src );
        /**
         * \brief   Moves high and low 32-bit values from the source.
         *
         * \param   src     The source large integer to move from.
         * \return  Reference to this large integer.
         **/
        inline NEMath::LargeInteger & operator = ( NEMath::LargeInteger && src ) noexcept;
        /**
         * \brief   Assigns the large integer by splitting the 64-bit source into high and low
         *          parts.
         *
         * \param   src     The 64-bit value to split and assign.
         * \return  Reference to this large integer.
         **/
        inline NEMath::LargeInteger & operator = ( uint64_t src );

        /**
         * \brief   Adds two large integers and returns the result as a 64-bit value.
         *
         * \param   rhs     The large integer to add.
         * \return  The sum as a 64-bit unsigned integer.
         **/
        inline uint64_t operator + ( const NEMath::LargeInteger & rhs ) const;
        /**
         * \brief   Adds this large integer to a 64-bit value and returns the result.
         *
         * \param   rhs     The 64-bit value to add.
         * \return  The sum as a 64-bit unsigned integer.
         **/
        inline uint64_t operator + ( uint64_t rhs ) const;

        /**
         * \brief   Adds a large integer to this object and stores the result.
         *
         * \param   rhs     The large integer to add.
         * \return  Reference to this large integer.
         **/
        inline NEMath::LargeInteger & operator += ( const NEMath::LargeInteger & rhs );
        /**
         * \brief   Adds a 64-bit value to this object and stores the result.
         *
         * \param   rhs     The 64-bit value to add.
         * \return  Reference to this large integer.
         **/
        inline NEMath::LargeInteger & operator += ( uint64_t rhs );

        /**
         * \brief   Subtracts a large integer from this object and returns the result as a 64-bit
         *          value.
         *
         * \param   rhs     The large integer to subtract.
         * \return  The difference as a 64-bit unsigned integer.
         **/
        inline uint64_t operator - ( const NEMath::LargeInteger & rhs ) const;
        /**
         * \brief   Subtracts a 64-bit value from this object and returns the result.
         *
         * \param   rhs     The 64-bit value to subtract.
         * \return  The difference as a 64-bit unsigned integer.
         **/
        inline uint64_t operator - ( uint64_t rhs ) const;

        /**
         * \brief   Subtracts a large integer from this object and stores the result.
         *
         * \param   rhs     The large integer to subtract.
         * \return  Reference to this large integer.
         **/
        inline NEMath::LargeInteger & operator -= ( const NEMath::LargeInteger & rhs );
        /**
         * \brief   Subtracts a 64-bit value from this object and stores the result.
         *
         * \param   rhs     The 64-bit value to subtract.
         * \return  Reference to this large integer.
         **/
        inline NEMath::LargeInteger & operator -= ( uint64_t rhs );

        /**
         * \brief   Returns true if two large integers are equal.
         *
         * \param   other       The large integer to compare.
         * \return  Returns true if both large integers are equal; false otherwise.
         **/
        inline bool operator == ( const NEMath::LargeInteger & other ) const;
        /**
         * \brief   Returns true if this large integer equals a 64-bit value.
         *
         * \param   other       The 64-bit value to compare.
         * \return  Returns true if the values are equal; false otherwise.
         **/
        inline bool operator == ( uint64_t other ) const;
        /**
         * \brief   Returns true if two large integers are not equal.
         *
         * \param   other       The large integer to compare.
         * \return  Returns true if the large integers are not equal; false otherwise.
         **/
        inline bool operator != ( const NEMath::LargeInteger & other ) const;
        /**
         * \brief   Returns true if this large integer does not equal a 64-bit value.
         *
         * \param   other       The 64-bit value to compare.
         * \return  Returns true if the values are not equal; false otherwise.
         **/
        inline bool operator != ( uint64_t other ) const;

    /************************************************************************/
    // NEMath::LargeInteger declare global operators to make streamable
    /************************************************************************/
        AREG_DECLARE_STREAMABLE(NEMath::LargeInteger);
    };

/************************************************************************/
// Basic math global functions
/************************************************************************/
    /**
     * \brief   Returns the sign of given object.
     *          The object should be possible to compare with 0 and should support
     *          operator smaller (operator <) and operator greater (operator >)
     * \param   val     The object to compare. Can be primitive or object.
     * \return  Returns sign of given object. The returns values are:
     *              a. NEMath::SignNegative     if the object is smaller than zero
     *              b. NEMath::SignPositive     if the object is greater than zero
     *              c. NEMath::SignUndefined    if the object is equal to zero
     **/
    template<typename Type>
    /**
     * \brief   Returns the sign of a value that supports comparison with zero and operators < and
     *          >.
     *
     * \param   val     Value to determine sign (primitive or object supporting < and > operators).
     * \return  Returns NEMath::SignNegative if val < 0, NEMath::SignPositive if val > 0,
     *          NEMath::SignUndefined if val == 0.
     **/
    inline NEMath::NumericSign sign( const Type & val );

/************************************************************************/
// NEMath namespace utility functions, operations with Large Number
/************************************************************************/
    /**
     * \brief   Sets high and low 32-bit values in a 64-bit LargeInteger structure.
     *
     * \param[out] out_num     LargeInteger structure to set bits in.
     * \param   hi          High 32 bits to set.
     * \param   lo          Low 32 bits to set.
     **/
    inline void set_bits( LargeInteger & out_num, uint32_t hi, uint32_t lo);

    /**
     * \brief   Returns the high 32-bit value from a 64-bit LargeInteger.
     *
     * \param   num     LargeInteger structure to extract high bits from.
     * \return  High 32-bit value.
     **/
    inline uint32_t high_bits(const LargeInteger & num);

    /**
     * \brief   Returns the low 32-bit value from a 64-bit LargeInteger.
     *
     * \param   num     LargeInteger structure to extract low bits from.
     * \return  Low 32-bit value.
     **/
    inline uint32_t low_bits(const LargeInteger & num);

/************************************************************************/
// NEMath namespace utility functions, check-sum operations
/************************************************************************/
    /**
     * \brief   Calculates 32-bit CRC using IEEE 802.3 standard with lookup table.
     *
     * \param   data    Binary data buffer to calculate CRC for.
     * \param   size    Size of buffer in bytes.
     * \return  32-bit CRC value.
     **/
    AREG_API uint32_t crc32_calculate( const uint8_t * data, int32_t size );

   /**
    * \brief   Calculates 32-bit CRC of a null-terminated string using IEEE 802.3 standard.
    *
    * \param   strData     Null-terminated string to calculate CRC for.
    * \return  32-bit CRC value.
    **/
   AREG_API uint32_t crc32_calculate( const char * strData  );
    /**
     * \brief   Calculates 32-bit CRC of a null-terminated wide string using IEEE 802.3 standard.
     *
     * \param   strData     Null-terminated wide string to calculate CRC for.
     * \return  32-bit CRC value.
     **/
    AREG_API uint32_t crc32_calculate( const wchar_t * strData );

    /**
     * \brief   Returns initial 32-bit CRC value for incremental calculations.
     **/
    AREG_API uint32_t crc32_init();

    /**
     * \brief   Continues 32-bit CRC calculation on binary data for incremental processing.
     *
     * \param   crcInit     Initial CRC value (from crc32_init() on first call, or value returned by
     *                      previous crc32_start() call).
     * \param   data        Binary data buffer to calculate CRC for.
     * \param   size        Size of buffer in bytes.
     * \return  Intermediate 32-bit CRC value to use in next call or with crc32_finish().
     **/
    AREG_API uint32_t crc32_start( uint32_t crcInit, const uint8_t * data, int32_t size );

    /**
     * \brief   Continues 32-bit CRC calculation on a null-terminated string for incremental
     *          processing.
     *
     * \param   crcInit     Initial CRC value (from crc32_init() on first call, or value returned by
     *                      previous crc32_start() call).
     * \param   data        Null-terminated string to calculate CRC for.
     * \return  Intermediate 32-bit CRC value to use in next call or with crc32_finish().
     **/
    AREG_API uint32_t crc32_start( uint32_t crcInit, const char * data );
    
    /**
     * \brief   Continues 32-bit CRC calculation on a single byte for incremental processing.
     *
     * \param   crcInit     Initial CRC value (from crc32_init() on first call, or value returned by
     *                      previous crc32_start() call).
     * \param   uch         Unsigned 8-bit value to process in CRC calculation.
     * \return  Intermediate 32-bit CRC value to use in next call or with crc32_finish().
     **/
    AREG_API uint32_t crc32_start( uint32_t crcInit, uint8_t uch );
    /**
     * \brief   Finalizes 32-bit CRC calculation and returns the complete checksum.
     *
     * \param   crc     CRC value from crc32_start() to finalize.
     * \return  Final 32-bit CRC value.
     **/
    AREG_API uint32_t crc32_finish( uint32_t crc );

    /**
     * \brief   Returns the rounded nearest integer value.
     *
     * \param   val     Floating-point value to round.
     * \return  Rounded value as double.
     **/
    AREG_API double round( double val );

    /**
     * \brief   Returns absolute value of passed parameter.
     *          The Type should support operator less to compare with 0
     *          and multiplication.
     * \param   val     A parameter to get absolute value
     * \tparam  Type    The type of object. Can be primitive or an object,
     *                  which has defined operator less ( operator > ),
     *                  should be possible to compare with integer 0
     *                  and should support multiplication.
     **/
    template <typename Type>
    /**
     * \brief   Returns the absolute value of a type that supports comparison with zero and
     *          multiplication.
     *
     * \param   val     Value to get absolute value of (requires < operator and multiplication
     *                  support).
     **/
    inline Type abs( const Type & val );

    /**
     * \brief   Compares 2 type of objects (primitives of objects) and return:
     *          -- NEMath::Ordering::Equal if `left` and `right` objects are equal.
     *          -- NEMath::Ordering::Bigger if `left object is bigger than the `right` object.
     *          -- NEMath::Ordering::Smaller if `left object is smaller than the `right` object.
     * \param   left    The left side object to compare.
     * \param   right   The right side object to compare.
     * \tparam  Type    Any type of object that is possible to compare with the comparing operator.
     **/
    template<typename Type>
    /**
     * \brief   Compares two values and returns NEMath::Ordering::Equal, Bigger, or Smaller.
     *
     * \param   left        Left-hand value to compare.
     * \param   right       Right-hand value to compare.
     * \return  NEMath::Ordering::Equal if equal, Bigger if left > right, Smaller if left < right.
     **/
    inline NEMath::Ordering compare(const Type & left, const Type & right);

/************************************************************************/
// Numeric utility functions
/************************************************************************/

    /**
     * \brief   Checks whether the value is in the range [rangeMin, rangeMax].
     * \param   value       The value to check.
     * \param   rangeMin    The minimum of the range (inclusive).
     * \param   rangeMax    The maximum of the range (inclusive).
     * \return  Returns true if value is within [rangeMin, rangeMax].
     * \tparam  T           Any type that supports operator <= .
     **/
    template <typename T>
    /**
     * \brief   Returns true if value is within the inclusive range [rangeMin, rangeMax].
     *
     * \param   value       Value to check.
     * \param   rangeMin    Minimum of the range (inclusive).
     * \param   rangeMax    Maximum of the range (inclusive).
     * \return  Returns true if value is within [rangeMin, rangeMax]; false otherwise.
     **/
    inline constexpr bool is_in_range(T value, T rangeMin, T rangeMax) noexcept;

    /**
     * \brief   Calculates and returns the nearest aligned value for the given length.
     * \param   len     The value to align.
     * \param   block   The alignment block size.
     * \return  The smallest multiple of block that is >= len.
     * \tparam  T       An arithmetic type.
     **/
    template <typename T>
    /**
     * \brief   Returns the smallest multiple of block that is greater than or equal to len.
     *
     * \param   len         Value to align.
     * \param   block       Alignment block size.
     * \return  Aligned value.
     **/
    inline constexpr T align_size(T len, T block) noexcept;

/************************************************************************/
// Bit manipulation functions
/************************************************************************/

    /**
     * \brief   Extracts the lowest 8 bits (byte) from the given integral value.
     * \param   value   The integral value to extract the low byte from.
     * \return  The lowest 8-bit value as uint8_t.
     * \tparam  T       An integral type (e.g., uint16_t, uint32_t).
     **/
    template <typename T>
    /**
     * \brief   Extracts the lowest 8 bits from the given integral value.
     *
     * \param   value       Integral value to extract low byte from.
     * \return  Lowest 8 bits as uint8_t.
     **/
    inline constexpr uint8_t lo_byte(T value) noexcept;

    /**
     * \brief   Extracts the highest 8 bits (byte) from the given integral value.
     *          For a 16-bit value, returns bits [15:8].
     *          For a 32-bit value, returns bits [31:24].
     * \param   value   The integral value to extract the high byte from.
     * \return  The highest 8-bit value as uint8_t.
     * \tparam  T       An integral type (e.g., uint16_t, uint32_t).
     **/
    template <typename T>
    /**
     * \brief   Extracts the highest 8 bits from the given integral value.
     *
     * \param   value       Integral value to extract high byte from.
     * \return  Highest 8 bits as uint8_t.
     **/
    inline constexpr uint8_t hi_byte(T value) noexcept;

    /**
     * \brief   Extracts the low 16 bits from a 32-bit value.
     *
     * \param   value       32-bit value to extract low word from.
     * \return  Low 16 bits as uint16_t.
     **/
    inline constexpr uint16_t lo_word(uint32_t value) noexcept;

    /**
     * \brief   Extracts the high 16 bits from a 32-bit value.
     *
     * \param   value       32-bit value to extract high word from.
     * \return  High 16 bits as uint16_t.
     **/
    inline constexpr uint16_t hi_word(uint32_t value) noexcept;

    /**
     * \brief   Extracts the low 32 bits from a 64-bit value.
     *
     * \param   value       64-bit value to extract low dword from.
     * \return  Low 32 bits as uint32_t.
     **/
    inline constexpr uint32_t lo_dword(uint64_t value) noexcept;

    /**
     * \brief   Extracts the high 32 bits from a 64-bit value.
     *
     * \param   value       64-bit value to extract high dword from.
     * \return  High 32 bits as uint32_t.
     **/
    inline constexpr uint32_t hi_dword(uint64_t value) noexcept;

    /**
     * \brief   Swaps the byte order of a 16-bit or 32-bit integral value.
     *          Uses compile-time branching (if constexpr) based on sizeof(T).
     * \param   value   The value whose bytes to swap.
     * \return  The byte-swapped value.
     * \tparam  T       An integral type, must be 2 or 4 bytes wide.
     **/
    template <typename T>
    /**
     * \brief   Swaps byte order of a 16-bit or 32-bit integral value.
     *
     * \param   value       Value whose bytes to swap.
     * \return  Byte-swapped value.
     **/
    inline constexpr T swap_bytes(T value) noexcept;

    /**
     * \brief   Composes a 32-bit value from two 16-bit values (high and low parts).
     *
     * \param   hi      High 16 bits.
     * \param   lo      Low 16 bits.
     * \return  Composed 32-bit value.
     **/
    inline constexpr uint32_t make32(uint16_t hi, uint16_t lo) noexcept;

    /**
     * \brief   Composes a 64-bit value from two 32-bit values (high and low parts).
     *
     * \param   hi      High 32 bits.
     * \param   lo      Low 32 bits.
     * \return  Composed 64-bit value.
     **/
    inline constexpr uint64_t make64(uint32_t hi, uint32_t lo) noexcept;

    /**
     * \brief   Constructs a 32-bit value from four bytes (most significant first).
     *
     * \param   b3      Bits [31:24].
     * \param   b2      Bits [23:16].
     * \param   b1      Bits [15:8].
     * \param   b0      Bits [7:0].
     * \return  Constructed 32-bit value.
     **/
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
     * \return  Constructed 64-bit value.
     **/
    inline constexpr uint64_t construct64(uint8_t b7, uint8_t b6, uint8_t b5, uint8_t b4,
                                          uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0) noexcept;

    /**
     * \brief   Computes the absolute difference (delta) between two values.
     *          Works with any type that supports comparison and subtraction.
     * \param   a   The first value.
     * \param   b   The second value.
     * \return  The absolute difference between a and b.
     * \tparam  T   A type that supports operator > and operator -.
     **/
    template <typename T>
    /**
     * \brief   Returns the absolute difference between two values.
     *
     * \param   a       First value.
     * \param   b       Second value.
     * \return  Absolute difference between a and b.
     **/
    inline constexpr T delta(T a, T b) noexcept;

}
//////////////////////////////////////////////////////////////////////////
// NEMath namespace inline function implementation
//////////////////////////////////////////////////////////////////////////

template <typename Type>
inline Type NEMath::abs( const Type & val )
{
    return (val >= 0 ? val : static_cast<Type>(val) * static_cast<Type>(-1));
}

template<typename Type>
inline NEMath::NumericSign NEMath::sign( const Type & val )
{
    return static_cast<NumericSign>((val > 0) - (val < 0));
}

template<typename Type>
inline NEMath::Ordering NEMath::compare(const Type & left, const Type & right)
{
    return (left == right ? NEMath::Ordering::Equal : (left > right ? NEMath::Ordering::Bigger : NEMath::Ordering::Smaller));
}

inline char NEMath::as_char(NEMath::NumericSign sign)
{
    switch (sign)
    {
    case NEMath::NumericSign::Negative:
        return '-';
    case NEMath::NumericSign::Positive:
        return '+';
    case NEMath::NumericSign::Undefined: // fall through
    default:
        return '\0';
    }
}

inline void NEMath::set_bits( LargeInteger &num, uint32_t hi, uint32_t lo )
{
    num.hiBits = hi;
    num.loBits = lo;
}

inline uint32_t NEMath::high_bits( const LargeInteger &num )
{
    return (num.hiBits);
}

inline uint32_t NEMath::low_bits( const LargeInteger &num )
{
    return (num.loBits);
}

/************************************************************************/
// NEMath numeric utility inline functions implementation
/************************************************************************/

template <typename T>
inline constexpr bool NEMath::is_in_range(T value, T rangeMin, T rangeMax) noexcept
{
    return (rangeMin <= value) && (value <= rangeMax);
}

template <typename T>
inline constexpr T NEMath::align_size(T len, T block) noexcept
{
    return (((len) + (block - 1)) / block) * block;
}

/************************************************************************/
// NEMath bit manipulation inline functions implementation
/************************************************************************/

template <typename T>
inline constexpr uint8_t NEMath::lo_byte(T value) noexcept
{
    static_assert(std::is_integral_v<T>, "lo_byte requires an integral type");
    return static_cast<uint8_t>(static_cast<std::make_unsigned_t<T>>(value) & 0xFF);
}

template <typename T>
inline constexpr uint8_t NEMath::hi_byte(T value) noexcept
{
    static_assert(std::is_integral_v<T>, "hi_byte requires an integral type");
    return static_cast<uint8_t>((static_cast<std::make_unsigned_t<T>>(value) >> ((sizeof(T) - 1) * 8)) & 0xFF);
}

inline constexpr uint16_t NEMath::lo_word(uint32_t value) noexcept
{
    return static_cast<uint16_t>(value & 0xFFFF);
}

inline constexpr uint16_t NEMath::hi_word(uint32_t value) noexcept
{
    return static_cast<uint16_t>((value >> 16) & 0xFFFF);
}

inline constexpr uint32_t NEMath::lo_dword(uint64_t value) noexcept
{
    return static_cast<uint32_t>(value & 0xFFFFFFFF);
}

inline constexpr uint32_t NEMath::hi_dword(uint64_t value) noexcept
{
    return static_cast<uint32_t>((value >> 32) & 0xFFFFFFFF);
}

template <typename T>
inline constexpr T NEMath::swap_bytes(T value) noexcept
{
    static_assert(std::is_integral_v<T>, "swap_bytes requires an integral type");
    static_assert(sizeof(T) == 2 || sizeof(T) == 4, "swap_bytes supports only 16-bit and 32-bit types");

    using U = std::make_unsigned_t<T>;
    U n = static_cast<U>(value);

    if constexpr (sizeof(T) == 2)
    {
        return static_cast<T>(((n << 8) & 0xFF00) | ((n >> 8) & 0x00FF));
    }
    else
    {
        return static_cast<T>(((n & 0x000000FF) << 24) |
                              ((n & 0x0000FF00) <<  8) |
                              ((n & 0x00FF0000) >>  8) |
                              ((n & 0xFF000000) >> 24));
    }
}

inline constexpr uint32_t NEMath::make32(uint16_t hi, uint16_t lo) noexcept
{
    return (static_cast<uint32_t>(hi) << 16) | static_cast<uint32_t>(lo);
}

inline constexpr uint64_t NEMath::make64(uint32_t hi, uint32_t lo) noexcept
{
    return (static_cast<uint64_t>(hi) << 32) | static_cast<uint64_t>(lo);
}

inline constexpr uint32_t NEMath::construct32(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0) noexcept
{
    return (static_cast<uint32_t>(b3) << 24) |
           (static_cast<uint32_t>(b2) << 16) |
           (static_cast<uint32_t>(b1) <<  8) |
           (static_cast<uint32_t>(b0));
}

inline constexpr uint64_t NEMath::construct64(uint8_t b7, uint8_t b6, uint8_t b5, uint8_t b4,
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
inline constexpr T NEMath::delta(T a, T b) noexcept
{
    return (a > b ? a - b : b - a);
}

/************************************************************************/
// NEMath::LargeInteger inline functions implementation
/************************************************************************/

inline NEMath::LargeInteger::LargeInteger()
    : hiBits ( 0 ), loBits ( 0 ) 
{
}

inline NEMath::LargeInteger::LargeInteger( uint32_t hi, uint32_t lo )
    : hiBits ( hi ), loBits ( lo ) 
{
}

inline NEMath::LargeInteger::LargeInteger( uint64_t num )
    : hiBits ( 0), loBits ( 0 ) 
{
    Int64Parts li{};
    li.quadPart = num;
    hiBits = li.u.highPart;
    loBits = li.u.lowPart;
}

inline NEMath::LargeInteger::LargeInteger( const NEMath::LargeInteger & src )
    : hiBits ( src.hiBits ), loBits ( src.loBits ) 
{
}

inline NEMath::LargeInteger::LargeInteger( NEMath::LargeInteger && src ) noexcept
    : hiBits ( src.hiBits ), loBits ( src.loBits )
{
}

inline NEMath::LargeInteger::operator uint64_t () const
{
    Int64Parts li{ {hiBits, loBits} };
    return li.quadPart;
}

inline NEMath::LargeInteger & NEMath::LargeInteger::operator = ( const uint64_t src )
{
    Int64Parts li{ {0, 0} };
    li.quadPart = src;
    hiBits  = li.u.highPart;
    loBits  = li.u.lowPart;
    return (*this);
}

inline uint64_t NEMath::LargeInteger::operator + ( const NEMath::LargeInteger & rhs ) const
{
    return (static_cast<uint64_t>(*this) + static_cast<uint64_t>(rhs));
}

inline uint64_t NEMath::LargeInteger::operator + ( uint64_t rhs ) const
{
    return ( (static_cast<uint64_t>(*this) + rhs) );
}

inline NEMath::LargeInteger & NEMath::LargeInteger::operator += ( const NEMath::LargeInteger & rhs )
{
    Int64Parts liLhs{ {this->hiBits, this->loBits} };
    Int64Parts liRhs{ {rhs.hiBits, rhs.loBits} };
    liRhs.quadPart += liRhs.quadPart;
    this->hiBits    = liRhs.u.highPart;
    this->loBits    = liLhs.u.lowPart;

    return (*this);
}

inline NEMath::LargeInteger & NEMath::LargeInteger::operator += ( uint64_t rhs )
{
    Int64Parts li{ {this->hiBits, this->loBits} };
    li.quadPart    += rhs;
    this->hiBits    = li.u.highPart;
    this->loBits    = li.u.lowPart;

    return (*this);
}

inline uint64_t NEMath::LargeInteger::operator - ( const NEMath::LargeInteger & rhs ) const
{
    return ( static_cast<uint64_t>(*this) - static_cast<uint64_t>(rhs) );
}

inline uint64_t NEMath::LargeInteger::operator - ( uint64_t rhs ) const
{
    return ( static_cast<uint64_t>(*this) - rhs );
}

inline NEMath::LargeInteger & NEMath::LargeInteger::operator -= ( const NEMath::LargeInteger & rhs )
{
    Int64Parts liLhs{ {this->hiBits, this->loBits} };
    Int64Parts liRhs{ {rhs.hiBits, rhs.loBits} };
    liRhs.quadPart -= liRhs.quadPart;
    this->hiBits    = liRhs.u.highPart;
    this->loBits    = liLhs.u.lowPart;

    return (*this);
}

inline NEMath::LargeInteger & NEMath::LargeInteger::operator -= ( uint64_t rhs )
{
    Int64Parts li{ {this->hiBits, this->loBits} };
    li.quadPart -= rhs;
    this->hiBits = li.u.highPart;
    this->loBits = li.u.lowPart;

    return (*this);
}

inline bool NEMath::LargeInteger::operator == ( const NEMath::LargeInteger & other ) const
{
    return ((this == &other) || ((hiBits == other.hiBits) && (loBits == other.loBits)));
}

inline bool NEMath::LargeInteger::operator == ( uint64_t other ) const
{
    return ( static_cast<uint64_t>(*this) == other );
}

inline bool NEMath::LargeInteger::operator != (const NEMath::LargeInteger& other) const
{
    return ((this != &other) && ((hiBits != other.hiBits) || (loBits != other.loBits)));
}

inline bool NEMath::LargeInteger::operator != (uint64_t other) const
{
    return ( static_cast<uint64_t>(*this) != other );
}

inline NEMath::LargeInteger & NEMath::LargeInteger::operator =  ( const NEMath::LargeInteger & src )
{
    hiBits  = src.hiBits;
    loBits  = src.loBits;

    return (*this);
}

inline NEMath::LargeInteger & NEMath::LargeInteger::operator =  ( NEMath::LargeInteger && src ) noexcept
{
    hiBits  = src.hiBits;
    loBits  = src.loBits;

    return (*this);
}

/************************************************************************/
// NEMath::LargeInteger declare global operators to make streamable
/************************************************************************/
AREG_IMPLEMENT_STREAMABLE(NEMath::LargeInteger)

#endif  // AREG_BASE_MATHDEFS_HPP
