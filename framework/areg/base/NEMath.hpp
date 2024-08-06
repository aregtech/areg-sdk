#ifndef AREG_BASE_NEMATH_HPP
#define AREG_BASE_NEMATH_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/NEMath.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Basic Math functionalities.
 *              Functions in this namespace are global
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEIOStream.hpp"

/**
 * \brief       Basic Math functions, helper classes and CRC
 **/
namespace NEMath
{
/************************************************************************/
// NEMath types
/************************************************************************/
    /**
     * \brief   The enumeration of digit signs
     *          NEMath::eDigitSign
     **/
    typedef enum class E_DigitSign : int8_t
    {
          SignNegative  = -1    //!< The sign is negative
        , SignUndefined =  0    //!< The sign is undefined.<br>By default, undefined is considered as positive.
        , SignPositive  =  1    //!< The sign is positive
    } eDigitSign;

    /**
     * \brief   Returns the value of character for specified sign
     * \param   sign    Passed sign value to get character
     * \return  Returns '-' for negative sign, '+' for positive sign and '\0' (empty) for undefined.
     */
    inline char getChar(NEMath::eDigitSign sign );

    /**
     * \brief   The enum defines comparing results
     **/
    typedef enum class E_CompareResult : int8_t
    {
          Smaller   = -1    //!< Comparing result is smaller
        , Equal     =  0    //!< Comparing result is equal
        , Bigger    =  1    //!< Comparing result is greater
    } eCompare;

    typedef struct S_Rect
    {
        int32_t     x       { 0 };
        int32_t     y       { 0 };
        uint32_t    width   { 0 };
        uint32_t    height  { 0 };
    } sRect;

    typedef struct S_Size
    {
        uint32_t    width   { 0 };
        uint32_t    height  { 0 };
    } sSize;

    typedef struct S_Posint
    {
        int32_t     x;
        int32_t     y;
    } sPoint;

    typedef struct S_Coord
    {
        int32_t     posX;
        int32_t     posY;
    } sCoord;

/************************************************************************/
// NEMath constants
/************************************************************************/

    /**
     * \brief   NEMath::NUMBER_PI
     *          PI number constant.
     **/
    constexpr double        NUMBER_PI           { M_PI };

    /**
     * \brief   NEMath::CHECKSUM_IGNORE
     *          No checksum is set, ignore.
     **/
    constexpr unsigned int  CHECKSUM_IGNORE     { 0u };

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
        inline sLargeInteger( void );
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
        inline operator uint64_t ( void ) const;

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
        DECLARE_STREAMABLE(NEMath::sLargeInteger);
    };

/************************************************************************/
// Basic math global functions
/************************************************************************/
    /**
     * \brief   Returns minimum of 2 values passed to function.
     *          The Type should support operator less ( operator < )
     * \param   a       The left-side object to compare
     * \param   b       The right-side object to compare
     * \return  Returns the minimum object of 2 given.
     * \tparam  Type    The type of object. Can be primitive or an object,
     *                  which has defined operator less ( operator < )
     **/
    template <typename Type>
    inline Type & getMin( const Type & a, const Type & b );

    /**
     * \brief   Returns maximum of 2 values passed to function.
     *          The Type should support operator more ( operator < )
     * \param   a       The left-side object to compare
     * \param   b       The right-side object to compare
     * \return  Returns the maximum object of 2 given.
     * \tparam  Type    The type of object. Can be primitive or an object,
     *                  which has defined operator more ( operator > )
     **/
    template <typename Type>
    inline const Type & getMax( const Type & a, const Type & b );


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
    inline NEMath::eDigitSign getSign( const Type & val );

/************************************************************************/
// NEMath namespace utility functions, operations with Large Number
/************************************************************************/
    /**
     * \brief	Sets High and Low bits in Large Number structure
     * \param	out_num Large Number to set bits.
     * \param	hi	    High bits to set
     * \param	lo	    Low bits to set
     **/
    inline void setBits( sLargeInteger & out_num, unsigned int hi, unsigned int lo);

    /**
     * \brief	Returns High Bits value of Large Number
     * \param	num	    Large Number structure
     * \return	Returns High Bits value of Large Number
     **/
    inline unsigned int getHighBits(const sLargeInteger & num);

    /**
     * \brief	Returns Low Bits value of Large Number
     * \param	num	    Large Number structure
     * \return	Returns Low Bits value of Large Number
     **/
    inline unsigned int getLowBits(const sLargeInteger & num);

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
    AREG_API unsigned int crc32Calculate( const unsigned char * data, int size );

     /**
     * \brief	Cyclic Redundancy Check (CRC) calculation function on 
     *          standard IEEE 802.3, using lookup table (fast calculate).
     *          Calculates and returns 32-bit CRC value of a null-terminated string in one step.
     * \param	strData Pointer to null-terminated string to calculate CRC
     * \return	32-bit value of Cyclic Redundancy Check (CRC)
     **/
   AREG_API unsigned int crc32Calculate( const char * strData  );
    /**
     * \brief	Cyclic Redundancy Check (CRC) calculation function on 
     *          standard IEEE 802.3, using lookup table (fast calculate).
     *          Calculates and returns 32-bit CRC value of a null-terminated wide-string in one step.
     * \param	strData Pointer to null-terminated wide-string to calculate CRC
     * \return	32-bit value of Cyclic Redundancy Check (CRC)
     **/
    AREG_API unsigned int crc32Calculate( const wchar_t * strData );

    /**
     * \brief	Return initial 32-bit value of Cyclic Redundancy Check (CRC)
     **/
    AREG_API unsigned int crc32Init( void );

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
    AREG_API unsigned int crc32Start( unsigned int crcInit, const unsigned char * data, int size );

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
    AREG_API unsigned int crc32Start( unsigned int crcInit, const char * data );
    
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
    AREG_API unsigned int crc32Start( unsigned int crcInit, unsigned char uch );
    /**
     * \brief	Returns 32-bit value of Cyclic Redundancy Check (CRC)
     *          as an end of calculation. This function is expected to be called
     *          after crc32Start() function as a final CRC value.
     * \param	crc	        The value returned by crc32Start()
     * \return	32-bit CRC value
     **/
    AREG_API unsigned int crc32Finish( unsigned int crc );

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
     * \brief   Returns absolute value of given digit. The difference with getAbs() method
     *          is that it works only for digits.
     * \param   number  The number to get absolute value
     * \return  Returns absolute value of digit.
     * \tparam  Digit   Any primitive type
     **/
    template<typename Digit>
    inline Digit makeAbsolute( Digit number );

}
//////////////////////////////////////////////////////////////////////////
// NEMath namespace inline function implementation
//////////////////////////////////////////////////////////////////////////

template <typename Type>
inline Type & NEMath::getMin( const Type & a, const Type & b )
{
    return MACRO_MIN(a, b);
}

template <typename Type>
inline const Type & NEMath::getMax( const Type & a, const Type & b )
{
    return MACRO_MAX(a, b);
}

template <typename Type>
inline Type NEMath::getAbs( const Type & val )
{
    return (val >= 0 ? val : static_cast<Type>(val) * static_cast<Type>(-1));
}

template<typename Type>
inline NEMath::eDigitSign NEMath::getSign( const Type & val )
{
    return static_cast<eDigitSign>(MACRO_SIGN_OF(val));
}

template<typename Digit>
inline Digit NEMath::makeAbsolute( Digit number )
{
    Digit mask = number >> (sizeof( Digit ) * 8 - 1);
    return ((number + mask) ^ mask);
}

inline char NEMath::getChar(NEMath::eDigitSign sign)
{
    switch (sign)
    {
    case NEMath::eDigitSign::SignNegative:
        return '-';
    case NEMath::eDigitSign::SignPositive:
        return '+';
    case NEMath::eDigitSign::SignUndefined: // fall through
    default:
        return '\0';
    }
}

inline void NEMath::setBits( sLargeInteger &num, unsigned int hi, unsigned int lo )
{
    num.hiBits = hi;
    num.loBits = lo;
}

inline unsigned int NEMath::getHighBits( const sLargeInteger &num )
{
    return (num.hiBits);
}

inline unsigned int NEMath::getLowBits( const sLargeInteger &num )
{
    return (num.loBits);
}

/************************************************************************/
// NEMath::sLargeInteger inline functions implementation
/************************************************************************/

inline NEMath::sLargeInteger::sLargeInteger( void )
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

inline NEMath::sLargeInteger::operator uint64_t ( void ) const
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
IMPLEMENT_STREAMABLE(NEMath::sLargeInteger)

#endif  // AREG_BASE_NEMATH_HPP
