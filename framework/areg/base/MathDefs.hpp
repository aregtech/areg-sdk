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
     * \brief   Returns the value of character for specified sign
     * \param   sign    Passed sign value to get character
     * \return  Returns '-' for negative sign, '+' for positive sign and '\0' (empty) for undefined.
     */
    inline char getChar(NEMath::NumericSign sign );

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
     * \brief   NEMath::sRect
     *          The structure of rectangle object, 
     *          which contains coordinate of the top-left corner, width and height.
     **/
    typedef struct S_Rect
    {
        int32_t     x       { 0 };  //!< Top left corner, X-coordinate.
        int32_t     y       { 0 };  //!< Top left corner, Y-coordinate.
        uint32_t    width   { 0 };  //!< Width of the rectangle.
        uint32_t    height  { 0 };  //!< Height of the rectangle.
    } sRect;

    /**
     * \brief   NEMath::sSize
     *          The structure of the size object, which contains width and height values.
     **/
    typedef struct S_Size
    {
        uint32_t    width   { 0 };  //!< The width of the size object.
        uint32_t    height  { 0 };  //!< The height of the size object.
    } sSize;

    /**
     * \brief   NEMath::sPoint
     *          The structure of the point object, which contains X- and Y-coordinates.
     **/
    typedef struct S_Point
    {
        int32_t     x;  //!< The X-coordinate of the point.
        int32_t     y;  //!< The Y-coordinate of the point.
    } sPoint;

    /**
     * \brief   NEMath::sCoord
     *          The structure of the Coordinate structure, which contains X- and Y-coordinates of a position.
     **/
    typedef struct S_Coord
    {
        int32_t     posX;   //!< The X-coordinate of a position.
        int32_t     posY;   //!< The Y-coordinate of a position.
    } sCoord;

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
     *          NEMath::uLargeInteger
     **/
    union  uLargeInteger
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
     * \brief   NEMath::sLargeInteger
     *          Large Number. 64-bit structure
     *          with defined high and low bits values.
     **/
    struct AREG_API sLargeInteger
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
         * \brief   Default constructor
         **/
        inline sLargeInteger();
        /**
         * \brief   Constructor. Sets hight and low 32-bit values of Large Number structure
         * \param   hi      The high 32-bit integer value of Large Number
         * \param   lo      The low  32-bit integer value of Large Number
         **/
        inline sLargeInteger( uint32_t hi, uint32_t lo );
        /**
         * \brief   Constructor. Sets hight and low 32-bit values of Large Number structure
         *          taken from 64-bit source integer value
         * \param   num     The 64-bit integer value.
         *                  The high 32-bit integer value will be saved in hiBits
         *                  The low  32-bit integer value will be saved in loBits
         **/
        inline sLargeInteger( uint64_t num );
        /**
         * \brief   Copy constructor.
         * \param   src     The source to copy data.
         **/
        inline sLargeInteger( const NEMath::sLargeInteger & src );

        /**
         * \brief   Move constructor.
         * \param   src     The source to move data.
         **/
        inline sLargeInteger( NEMath::sLargeInteger && src ) noexcept;

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////

    /************************************************************************/
    // NEMath::sLargeInteger converting operators
    /************************************************************************/
        /**
         * \brief   Converts Large Number structure to uint64_t 64-bit
         *          integer value by copying hiBits value to high 32-bit
         *          and loBits value to low 32-bits value of 64-bit integer.
         **/
        inline operator uint64_t () const;

    /************************************************************************/
    // NEMath::sLargeInteger simple math operations
    /************************************************************************/
        /**
         * \brief   Assigning operator, assigns high and low 32-bit values
         *          taken from given Large Number source.
         * \param   src     The source of Large Number to take high and low 32-bit values.
         **/
        inline NEMath::sLargeInteger & operator = ( const NEMath::sLargeInteger & src );
        /**
         * \brief   Move operator, assigns high and low 32-bit values from given source.
         * \param   src     The source of Large Number to take high and low 32-bit values.
         **/
        inline NEMath::sLargeInteger & operator = ( NEMath::sLargeInteger && src ) noexcept;
        /**
         * \brief   Assigning operator, assigns high and low 32-bit values
         *          taken from given 64-bit integer source.
         * \param   src     The source of 64-bit integers value to take high and low 32-bit values.
         **/
        inline NEMath::sLargeInteger & operator = ( uint64_t src );

        /**
         * \brief   Sums 2 Large Number values. 2 values will be added as 2 64-bit integer
         *          values and returned as 64-bit integer.
         * \param   rhs     The right-hand operand of Large Number to add
         **/
        inline uint64_t operator + ( const NEMath::sLargeInteger & rhs ) const;
        /**
         * \brief   Sums Large Number and 64-bit integer values. 2 values will be added as 2 64-bit integer
         *          values and returned as 64-bit integer.
         * \param   rhs     The right-hand operand of 64-bit integer to add
         **/
        inline uint64_t operator + ( uint64_t rhs ) const;

        /**
         * \brief   Sums 2 Large Number values. 2 values will be added as 2 64-bit integer
         *          values and the result will be saved in left-hand operand Large Number structure
         * \param   rhs     The right-hand operand of Large Number to add
         **/
        inline NEMath::sLargeInteger & operator += ( const NEMath::sLargeInteger & rhs );
        /**
         * \brief   Sums Large Number and 64-bit integer values. 2 values will be added as 2 64-bit integer
         *          values and the result will be saved in left-hand operand Large Number structure
         * \param   rhs     The right-hand operand of 64-bit integer to add
         **/
        inline NEMath::sLargeInteger & operator += ( uint64_t rhs );

        /**
         * \brief   Subtracts 2 Large Number values. 2 values will be subtracted as 2 64-bit integer
         *          values and returned as 64-bit integer.
         * \param   rhs     The right-hand operand of Large Number to subtract
         **/
        inline uint64_t operator - ( const NEMath::sLargeInteger & rhs ) const;
        /**
         * \brief   Subtracts Large Number and 64-bit integer values. 2 values will be subtracted as 2 64-bit integer
         *          values and returned as 64-bit integer.
         * \param   rhs     The right-hand operand of 64-bit integer to subtract
         **/
        inline uint64_t operator - ( uint64_t rhs ) const;

        /**
         * \brief   Subtracts 2 Large Number values. 2 values will be subtracted as 2 64-bit integer
         *          values and the result will be saved in left-hand operand Large Number structure
         * \param   rhs     The right-hand operand of Large Number to subtract
         **/
        inline NEMath::sLargeInteger & operator -= ( const NEMath::sLargeInteger & rhs );
         /**
         * \brief   Subtracts Large Number and 64-bit integer values. 2 values will be subtracted as 2 64-bit integer
         *          values and the result will be saved in left-hand operand Large Number structure
         * \param   rhs     The right-hand operand of 64-bit integer to subtract
         **/
        inline NEMath::sLargeInteger & operator -= ( uint64_t rhs );

        /**
         * \brief   Compares 2 Large Number value and returns true if they are equal
         **/
        inline bool operator == ( const NEMath::sLargeInteger & other ) const;
        /**
         * \brief   Compares Large Number and 64-bit integer values, and returns true if they are equal
         **/
        inline bool operator == ( uint64_t other ) const;
        /**
         * \brief   Compares 2 Large Number value and returns true if they are not equal
         **/
        inline bool operator != ( const NEMath::sLargeInteger & other ) const;
        /**
         * \brief   Compares Large Number and 64-bit integer values, and returns true if they are not equal
         **/
        inline bool operator != ( uint64_t other ) const;

    /************************************************************************/
    // NEMath::sLargeInteger declare global operators to make streamable
    /************************************************************************/
        AREG_DECLARE_STREAMABLE(NEMath::sLargeInteger);
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
    inline NEMath::NumericSign getSign( const Type & val );

/************************************************************************/
// NEMath namespace utility functions, operations with Large Number
/************************************************************************/
    /**
     * \brief	Sets High and Low bits in Large Number structure
     * \param	out_num Large Number to set bits.
     * \param	hi	    High bits to set
     * \param	lo	    Low bits to set
     **/
    inline void setBits( sLargeInteger & out_num, uint32_t hi, uint32_t lo);

    /**
     * \brief	Returns High Bits value of Large Number
     * \param	num	    Large Number structure
     * \return	Returns High Bits value of Large Number
     **/
    inline uint32_t getHighBits(const sLargeInteger & num);

    /**
     * \brief	Returns Low Bits value of Large Number
     * \param	num	    Large Number structure
     * \return	Returns Low Bits value of Large Number
     **/
    inline uint32_t getLowBits(const sLargeInteger & num);

/************************************************************************/
// NEMath namespace utility functions, check-sum operations
/************************************************************************/
    /**
     * \brief	Cyclic Redundancy Check (CRC) calculation function on 
     *          standard IEEE 802.3, using lookup table (fast calculate).
     *          Calculates and returns 32-bit CRC value of a binary data in one step.
     * \param	data	Pointer to data to calculate CRC
     * \param	size	The size in bytes of given buffer
     * \return	32-bit value of Cyclic Redundancy Check (CRC)
     **/
    AREG_API uint32_t crc32Calculate( const uint8_t * data, int32_t size );

     /**
     * \brief	Cyclic Redundancy Check (CRC) calculation function on 
     *          standard IEEE 802.3, using lookup table (fast calculate).
     *          Calculates and returns 32-bit CRC value of a null-terminated string in one step.
     * \param	strData Pointer to null-terminated string to calculate CRC
     * \return	32-bit value of Cyclic Redundancy Check (CRC)
     **/
   AREG_API uint32_t crc32Calculate( const char * strData  );
    /**
     * \brief	Cyclic Redundancy Check (CRC) calculation function on 
     *          standard IEEE 802.3, using lookup table (fast calculate).
     *          Calculates and returns 32-bit CRC value of a null-terminated wide-string in one step.
     * \param	strData Pointer to null-terminated wide-string to calculate CRC
     * \return	32-bit value of Cyclic Redundancy Check (CRC)
     **/
    AREG_API uint32_t crc32Calculate( const wchar_t * strData );

    /**
     * \brief	Return initial 32-bit value of Cyclic Redundancy Check (CRC)
     **/
    AREG_API uint32_t crc32Init();

    /**
     * \brief	Starts 32-bit Cyclic Redundancy Check (CRC) calculation.
     *          The function can be called cyclic on continues data.
     *          On first step 'crcInit' value should be initialized value,
     *          i.e. the value returned by crc32Init() function.
     *          On next steps the value of 'crcInit' should be same value
     *          returned by this function.
     * \param	crcInit     On first step this should be initialized value,
     *                      i.e. the value returned by crc32Init() function.
     *                      If unction is called cyclic, on next steps this
     *                      should be same value returned by this function.
     * \param	data	    Pointer to binary data to calculate 32-bit CRC
     * \param	size	    The size in bytes of given buffer.
     * \return	32-bit value.
     **/
    AREG_API uint32_t crc32Start( uint32_t crcInit, const uint8_t * data, int32_t size );

    /**
     * \brief   NEMath::Crc32Start
     *          Starts 32-bit Cyclic Redundancy Check (CRC) calculation for string.
     *          The calculation continues until end of string is not matched
     *          The function can be called cyclic on continues data.
     *          On first step 'crcInit' value should be initialized value,
     *          i.e. the value returned by crc32Init() function.
     *          On next steps the value of 'crcInit' should be same value
     *          returned by this function.
     * \param	crcInit	    On first step this should be initialized value,
     *                      i.e. the value returned by crc32Init() function.
     *                      If unction is called cyclic, on next steps this
     *                      should be same value returned by this function.
     * \param	data	    Pointer to binary data to calculate 32-bit CRC
     * \return	32-bit value.
     **/
    AREG_API uint32_t crc32Start( uint32_t crcInit, const char * data );
    
    /**
     * \brief   NEMath::Crc32Start
     *      	Starts 32-bit Cyclic Redundancy Check (CRC) calculation of a single byte  value.
     *          The function can be called cyclic on continues data.
     *          On first step 'crcInit' value should be initialized value,
     *          i.e. the value returned by crc32Init() function.
     *          On next steps the value of 'crcInit' should be same value
     *          returned by this function.
     * \param	crcInit	    On first step this should be initialized value,
     *                      i.e. the value returned by crc32Init() function.
     *                      If unction is called cyclic, on next steps this
     *                      should be same value returned by this function.
     * \param	uch	        The first unsigned 8-bit value to start calculating CRC.
     * \return	32-bit value.
     **/
    AREG_API uint32_t crc32Start( uint32_t crcInit, uint8_t uch );
    /**
     * \brief	Returns 32-bit value of Cyclic Redundancy Check (CRC)
     *          as an end of calculation. This function is expected to be called
     *          after crc32Start() function as a final CRC value.
     * \param	crc	        The value returned by crc32Start()
     * \return	32-bit CRC value
     **/
    AREG_API uint32_t crc32Finish( uint32_t crc );

    /**
     * \brief   Rounds passed double value to nearest integer
     * \param   val     A parameter to round. Normally float or double type.
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
    inline Type getAbs( const Type & val );

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
    inline constexpr bool isInRange(T value, T rangeMin, T rangeMax) noexcept;

    /**
     * \brief   Calculates and returns the nearest aligned value for the given length.
     * \param   len     The value to align.
     * \param   block   The alignment block size.
     * \return  The smallest multiple of block that is >= len.
     * \tparam  T       An arithmetic type.
     **/
    template <typename T>
    inline constexpr T alignSize(T len, T block) noexcept;

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
    inline constexpr uint8_t loByte(T value) noexcept;

    /**
     * \brief   Extracts the highest 8 bits (byte) from the given integral value.
     *          For a 16-bit value, returns bits [15:8].
     *          For a 32-bit value, returns bits [31:24].
     * \param   value   The integral value to extract the high byte from.
     * \return  The highest 8-bit value as uint8_t.
     * \tparam  T       An integral type (e.g., uint16_t, uint32_t).
     **/
    template <typename T>
    inline constexpr uint8_t hiByte(T value) noexcept;

    /**
     * \brief   Extracts the low 16 bits (word) from a 32-bit value.
     * \param   value   The 32-bit value to extract the low word from.
     * \return  The low 16-bit value as uint16_t.
     **/
    inline constexpr uint16_t loWord(uint32_t value) noexcept;

    /**
     * \brief   Extracts the high 16 bits (word) from a 32-bit value.
     * \param   value   The 32-bit value to extract the high word from.
     * \return  The high 16-bit value as uint16_t.
     **/
    inline constexpr uint16_t hiWord(uint32_t value) noexcept;

    /**
     * \brief   Extracts the low 32 bits (dword) from a 64-bit value.
     * \param   value   The 64-bit value to extract the low dword from.
     * \return  The low 32-bit value as uint32_t.
     **/
    inline constexpr uint32_t loDword(uint64_t value) noexcept;

    /**
     * \brief   Extracts the high 32 bits (dword) from a 64-bit value.
     * \param   value   The 64-bit value to extract the high dword from.
     * \return  The high 32-bit value as uint32_t.
     **/
    inline constexpr uint32_t hiDword(uint64_t value) noexcept;

    /**
     * \brief   Swaps the byte order of a 16-bit or 32-bit integral value.
     *          Uses compile-time branching (if constexpr) based on sizeof(T).
     * \param   value   The value whose bytes to swap.
     * \return  The byte-swapped value.
     * \tparam  T       An integral type, must be 2 or 4 bytes wide.
     **/
    template <typename T>
    inline constexpr T swapBytes(T value) noexcept;

    /**
     * \brief   Composes a 32-bit value from two 16-bit values.
     * \param   hi  The high 16 bits.
     * \param   lo  The low 16 bits.
     * \return  The composed 32-bit value.
     **/
    inline constexpr uint32_t make32(uint16_t hi, uint16_t lo) noexcept;

    /**
     * \brief   Composes a 64-bit value from two 32-bit values.
     * \param   hi  The high 32 bits.
     * \param   lo  The low 32 bits.
     * \return  The composed 64-bit value.
     **/
    inline constexpr uint64_t make64(uint32_t hi, uint32_t lo) noexcept;

    /**
     * \brief   Constructs a 32-bit value from four bytes (most significant first).
     * \param   b3  Bits [31:24].
     * \param   b2  Bits [23:16].
     * \param   b1  Bits [15:8].
     * \param   b0  Bits [7:0].
     * \return  The constructed 32-bit value.
     **/
    inline constexpr uint32_t construct32(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0) noexcept;

    /**
     * \brief   Constructs a 64-bit value from eight bytes (most significant first).
     * \param   b7  Bits [63:56].
     * \param   b6  Bits [55:48].
     * \param   b5  Bits [47:40].
     * \param   b4  Bits [39:32].
     * \param   b3  Bits [31:24].
     * \param   b2  Bits [23:16].
     * \param   b1  Bits [15:8].
     * \param   b0  Bits [7:0].
     * \return  The constructed 64-bit value.
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
    inline constexpr T delta(T a, T b) noexcept;

}
//////////////////////////////////////////////////////////////////////////
// NEMath namespace inline function implementation
//////////////////////////////////////////////////////////////////////////

template <typename Type>
inline Type NEMath::getAbs( const Type & val )
{
    return (val >= 0 ? val : static_cast<Type>(val) * static_cast<Type>(-1));
}

template<typename Type>
inline NEMath::NumericSign NEMath::getSign( const Type & val )
{
    return static_cast<NumericSign>((val > 0) - (val < 0));
}

template<typename Type>
inline NEMath::Ordering NEMath::compare(const Type & left, const Type & right)
{
    return (left == right ? NEMath::Ordering::Equal : (left > right ? NEMath::Ordering::Bigger : NEMath::Ordering::Smaller));
}

inline char NEMath::getChar(NEMath::NumericSign sign)
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

inline void NEMath::setBits( sLargeInteger &num, uint32_t hi, uint32_t lo )
{
    num.hiBits = hi;
    num.loBits = lo;
}

inline uint32_t NEMath::getHighBits( const sLargeInteger &num )
{
    return (num.hiBits);
}

inline uint32_t NEMath::getLowBits( const sLargeInteger &num )
{
    return (num.loBits);
}

/************************************************************************/
// NEMath numeric utility inline functions implementation
/************************************************************************/

template <typename T>
inline constexpr bool NEMath::isInRange(T value, T rangeMin, T rangeMax) noexcept
{
    return (rangeMin <= value) && (value <= rangeMax);
}

template <typename T>
inline constexpr T NEMath::alignSize(T len, T block) noexcept
{
    return (((len) + (block - 1)) / block) * block;
}

/************************************************************************/
// NEMath bit manipulation inline functions implementation
/************************************************************************/

template <typename T>
inline constexpr uint8_t NEMath::loByte(T value) noexcept
{
    static_assert(std::is_integral_v<T>, "loByte requires an integral type");
    return static_cast<uint8_t>(static_cast<std::make_unsigned_t<T>>(value) & 0xFF);
}

template <typename T>
inline constexpr uint8_t NEMath::hiByte(T value) noexcept
{
    static_assert(std::is_integral_v<T>, "hiByte requires an integral type");
    return static_cast<uint8_t>((static_cast<std::make_unsigned_t<T>>(value) >> ((sizeof(T) - 1) * 8)) & 0xFF);
}

inline constexpr uint16_t NEMath::loWord(uint32_t value) noexcept
{
    return static_cast<uint16_t>(value & 0xFFFF);
}

inline constexpr uint16_t NEMath::hiWord(uint32_t value) noexcept
{
    return static_cast<uint16_t>((value >> 16) & 0xFFFF);
}

inline constexpr uint32_t NEMath::loDword(uint64_t value) noexcept
{
    return static_cast<uint32_t>(value & 0xFFFFFFFF);
}

inline constexpr uint32_t NEMath::hiDword(uint64_t value) noexcept
{
    return static_cast<uint32_t>((value >> 32) & 0xFFFFFFFF);
}

template <typename T>
inline constexpr T NEMath::swapBytes(T value) noexcept
{
    static_assert(std::is_integral_v<T>, "swapBytes requires an integral type");
    static_assert(sizeof(T) == 2 || sizeof(T) == 4, "swapBytes supports only 16-bit and 32-bit types");

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
// NEMath::sLargeInteger inline functions implementation
/************************************************************************/

inline NEMath::sLargeInteger::sLargeInteger()
    : hiBits ( 0 ), loBits ( 0 ) 
{
}

inline NEMath::sLargeInteger::sLargeInteger( uint32_t hi, uint32_t lo )
    : hiBits ( hi ), loBits ( lo ) 
{
}

inline NEMath::sLargeInteger::sLargeInteger( uint64_t num )
    : hiBits ( 0), loBits ( 0 ) 
{
    uLargeInteger li{};
    li.quadPart = num;
    hiBits = li.u.highPart;
    loBits = li.u.lowPart;
}

inline NEMath::sLargeInteger::sLargeInteger( const NEMath::sLargeInteger & src )
    : hiBits ( src.hiBits ), loBits ( src.loBits ) 
{
}

inline NEMath::sLargeInteger::sLargeInteger( NEMath::sLargeInteger && src ) noexcept
    : hiBits ( src.hiBits ), loBits ( src.loBits )
{
}

inline NEMath::sLargeInteger::operator uint64_t () const
{
    uLargeInteger li{ {hiBits, loBits} };
    return li.quadPart;
}

inline NEMath::sLargeInteger & NEMath::sLargeInteger::operator = ( const uint64_t src )
{
    uLargeInteger li{ {0, 0} };
    li.quadPart = src;
    hiBits  = li.u.highPart;
    loBits  = li.u.lowPart;
    return (*this);
}

inline uint64_t NEMath::sLargeInteger::operator + ( const NEMath::sLargeInteger & rhs ) const
{
    return (static_cast<uint64_t>(*this) + static_cast<uint64_t>(rhs));
}

inline uint64_t NEMath::sLargeInteger::operator + ( uint64_t rhs ) const
{
    return ( (static_cast<uint64_t>(*this) + rhs) );
}

inline NEMath::sLargeInteger & NEMath::sLargeInteger::operator += ( const NEMath::sLargeInteger & rhs )
{
    uLargeInteger liLhs{ {this->hiBits, this->loBits} };
    uLargeInteger liRhs{ {rhs.hiBits, rhs.loBits} };
    liRhs.quadPart += liRhs.quadPart;
    this->hiBits    = liRhs.u.highPart;
    this->loBits    = liLhs.u.lowPart;

    return (*this);
}

inline NEMath::sLargeInteger & NEMath::sLargeInteger::operator += ( uint64_t rhs )
{
    uLargeInteger li{ {this->hiBits, this->loBits} };
    li.quadPart    += rhs;
    this->hiBits    = li.u.highPart;
    this->loBits    = li.u.lowPart;

    return (*this);
}

inline uint64_t NEMath::sLargeInteger::operator - ( const NEMath::sLargeInteger & rhs ) const
{
    return ( static_cast<uint64_t>(*this) - static_cast<uint64_t>(rhs) );
}

inline uint64_t NEMath::sLargeInteger::operator - ( uint64_t rhs ) const
{
    return ( static_cast<uint64_t>(*this) - rhs );
}

inline NEMath::sLargeInteger & NEMath::sLargeInteger::operator -= ( const NEMath::sLargeInteger & rhs )
{
    uLargeInteger liLhs{ {this->hiBits, this->loBits} };
    uLargeInteger liRhs{ {rhs.hiBits, rhs.loBits} };
    liRhs.quadPart -= liRhs.quadPart;
    this->hiBits    = liRhs.u.highPart;
    this->loBits    = liLhs.u.lowPart;

    return (*this);
}

inline NEMath::sLargeInteger & NEMath::sLargeInteger::operator -= ( uint64_t rhs )
{
    uLargeInteger li{ {this->hiBits, this->loBits} };
    li.quadPart -= rhs;
    this->hiBits = li.u.highPart;
    this->loBits = li.u.lowPart;

    return (*this);
}

inline bool NEMath::sLargeInteger::operator == ( const NEMath::sLargeInteger & other ) const
{
    return ((this == &other) || ((hiBits == other.hiBits) && (loBits == other.loBits)));
}

inline bool NEMath::sLargeInteger::operator == ( uint64_t other ) const
{
    return ( static_cast<uint64_t>(*this) == other );
}

inline bool NEMath::sLargeInteger::operator != (const NEMath::sLargeInteger& other) const
{
    return ((this != &other) && ((hiBits != other.hiBits) || (loBits != other.loBits)));
}

inline bool NEMath::sLargeInteger::operator != (uint64_t other) const
{
    return ( static_cast<uint64_t>(*this) != other );
}

inline NEMath::sLargeInteger & NEMath::sLargeInteger::operator =  ( const NEMath::sLargeInteger & src )
{
    hiBits  = src.hiBits;
    loBits  = src.loBits;

    return (*this);
}

inline NEMath::sLargeInteger & NEMath::sLargeInteger::operator =  ( NEMath::sLargeInteger && src ) noexcept
{
    hiBits  = src.hiBits;
    loBits  = src.loBits;

    return (*this);
}

/************************************************************************/
// NEMath::sLargeInteger declare global operators to make streamable
/************************************************************************/
AREG_IMPLEMENT_STREAMABLE(NEMath::sLargeInteger)

#endif  // AREG_BASE_MATHDEFS_HPP
