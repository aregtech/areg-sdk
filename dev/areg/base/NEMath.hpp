#ifndef AREG_BASE_NEMATH_HPP
#define AREG_BASE_NEMATH_HPP
/************************************************************************
 * \file        areg/base/NEMath.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
 * 
 * \details     To support functionalities of different types of variables
 *              functions are defined and implemented as templates.
 *
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
    typedef enum E_DigitSign
    {
          SignNegative  = -1    //!< The sign is negative
        , SignUndefined = 0     //!< The sign is undefined.<br>By default, undefined is considered as positive.
        , SignPositive  = 1     //!< The sign is positive
    } eDigitSign;

    /**
     * \brief   Returns the value of character for specified sign
     * \param   Passed sign value to get character
     * \return  Returns '-' for negative sign, '+' for positive sign and '\0' (empty) for undefined.
     */
    inline char getChar(NEMath::eDigitSign sign );

    /**
     * \brief   The enum defines comparing results
     **/
    typedef enum E_CompareResult
    {
          CompSmaller   = -1    //!< Comparing result is smaller
        , CompEqual     =  0    //!< Comparing result is equal
        , CompGreater   =  1    //!< Comparing result is greater
    } eCompare;

/************************************************************************/
// NEMath constants
/************************************************************************/

    /**
     * \brief   NEMath::NUMBER_PI
     *          PI number constant.
     **/
    const double        NUMBER_PI           = M_PI;

    /**
     * \brief   NEMath::CHECKSUM_IGNORE
     *          No checksum is set, ignore.
     **/
    const unsigned int  CHECKSUM_IGNORE     = static_cast<unsigned int>(0);

/************************************************************************/
// NEMath namespace structures
/************************************************************************/

    /**
     * \brief   The union object of Large Integer,
     *          which consists of 32-bit low part integer and 32-bit high part integer.
     *          NEMath::uLargeInteger
     **/
    typedef union  U_LargeInteger
    {
        /**
         * \brief   Named structure
         **/
        struct _name
        {
            uint32_t    lowPart;    //!< Low part 32-bit integer
            uint32_t    highPart;   //!< High part 32-bit integer
        } u;

        uint64_t        quadPart;   //!< 64-bit integer
    } uLargeInteger;

    /**
     * \brief   NEMath::sLargeInteger
     *          Large Number. 64-bit structure
     *          with defined high and low bits values.
     **/
    typedef struct AREG_API S_LargeInteger
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
        inline S_LargeInteger( void );
        /**
         * \brief   Constructor. Sets hight and low 32-bit values of Large Number structure
         * \param   hi      The high 32-bit integer value of Large Number
         * \param   lo      The low  32-bit integer value of Large Number
         **/
        inline S_LargeInteger( uint32_t hi, uint32_t lo );
        /**
         * \brief   Constructor. Sets hight and low 32-bit values of Large Number structure
         *          taken from 64-bit source integer value
         * \param   num     The 64-bit integer value.
         *                  The high 32-bit integer value will be saved in hiBits
         *                  The low  32-bit integer value will be saved in loBits
         **/
        inline S_LargeInteger( uint64_t num );
        /**
         * \brief   Copy constructor.
         * \param   src     The source to copy data.
         **/
        inline S_LargeInteger( const NEMath::S_LargeInteger & src );

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
        const NEMath::S_LargeInteger & operator = ( const NEMath::S_LargeInteger & src );
        /**
         * \brief   Assigning operator, assigns high and low 32-bit values
         *          taken from given 64-bit integer source.
         * \param   src     The source of 64-bit integers value to take high and low 32-bit values.
         **/
        inline const NEMath::S_LargeInteger & operator = ( uint64_t src );

        /**
         * \brief   Sums 2 Large Number values. 2 values will be added as 2 64-bit integer
         *          values and returned as 64-bit integer.
         * \param   rhs     The right-hand operand of Large Number to add
         **/
        inline uint64_t operator + ( const NEMath::S_LargeInteger & rhs ) const;
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
        inline const NEMath::S_LargeInteger & operator += ( const NEMath::S_LargeInteger & rhs );
        /**
         * \brief   Sums Large Number and 64-bit integer values. 2 values will be added as 2 64-bit integer
         *          values and the result will be saved in left-hand operand Large Number structure
         * \param   rhs     The right-hand operand of 64-bit integer to add
         **/
        inline const NEMath::S_LargeInteger & operator += ( uint64_t rhs );

        /**
         * \brief   Subtracts 2 Large Number values. 2 values will be subtracted as 2 64-bit integer
         *          values and returned as 64-bit integer.
         * \param   rhs     The right-hand operand of Large Number to subtract
         **/
        inline uint64_t operator - ( const NEMath::S_LargeInteger & rhs ) const;
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
        inline const NEMath::S_LargeInteger & operator -= ( const NEMath::S_LargeInteger & rhs );
         /**
         * \brief   Subtracts Large Number and 64-bit integer values. 2 values will be subtracted as 2 64-bit integer
         *          values and the result will be saved in left-hand operand Large Number structure
         * \param   rhs     The right-hand operand of 64-bit integer to subtract
         **/
        inline const NEMath::S_LargeInteger & operator -= ( uint64_t rhs );

        /**
         * \brief   Compares 2 Large Number value and returns true if they are equal
         **/
        inline bool operator == ( const NEMath::S_LargeInteger & other ) const;
        /**
         * \brief   Compares Large Number and 64-bit integer values, and returns true if they are equal
         **/
        inline bool operator == ( uint64_t other ) const;
        /**
         * \brief   Compares 2 Large Number value and returns true if they are not equal
         **/
        inline bool operator != ( const NEMath::S_LargeInteger & other ) const;
        /**
         * \brief   Compares Large Number and 64-bit integer values, and returns true if they are not equal
         **/
        inline bool operator != ( uint64_t other ) const;

    /************************************************************************/
    // NEMath::sLargeInteger declare global operators to make streamable
    /************************************************************************/
        DECLARE_STREAMABLE(NEMath::S_LargeInteger)

    } sLargeInteger;

/************************************************************************/
// Basic math global functions
/************************************************************************/
    /**
     * \brief   Returns minimum of 2 values passed to function.
     *          The type Type should support operator less ( operator < )
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
     *          The type Type should support operator more ( operator < )
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
     * \return	
     **/
    AREG_API void setBits( sLargeInteger & out_num, unsigned int hi, unsigned int lo);

    /**
     * \brief	Returns High Bits value of Large Number
     * \param	num	    Large Number structure
     * \return	Returns High Bits value of Large Number
     **/
    AREG_API unsigned int getHighBits(const sLargeInteger & num);

    /**
     * \brief	Returns Low Bits value of Large Number
     * \param	num	    Large Number structure
     * \return	Returns Low Bits value of Large Number
     **/
    AREG_API unsigned int getLowBits(const sLargeInteger & num);

/************************************************************************/
// NEMath namespace utility functions, check-sum operations
/************************************************************************/
    /**
     * \brief	Cyclic Redundancy Check (CRC) calculation function on 
     *          standard IEEE 802.3, using lookup table (fast calculate).
     *          Calculates and returns 32-bit CRC value of a binary data in one step.
     * \param	data	Pointer to data to calculate CRC
     * \param	size	Size in bytes of given buffer
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
     * \param	size	    Size of given buffer
     * \return	32-bit value.
     **/
    AREG_API unsigned int crc32Start( unsigned int crcInit, const unsigned char * data, int size );

    /**
     * \brief   NEMath::Crc32Start
     * \brief	Starts 32-bit Cyclic Redundancy Check (CRC) calculation for string.
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
     * \param	size	    Size of given buffer
     * \return	32-bit value.
     **/
    AREG_API unsigned int crc32Start( unsigned int crcInit, const char * data );
    /**
     * \brief   NEMath::Crc32Start
     * \brief	Starts 32-bit Cyclic Redundancy Check (CRC) calculation of a single byte  value.
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
     * \param	size	    Size of given buffer
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
     *          The type Type should support operator less to compare with 0
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
     **/
    template<typename DigitType>
    inline DigitType makeAbsolute( DigitType number );

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
    return MACRO_ABS(val);
}

template<typename Type>
inline NEMath::eDigitSign NEMath::getSign( const Type & val )
{
    return static_cast<eDigitSign>(MACRO_SIGN_OF(val));
}

template<typename DigitType>
inline DigitType NEMath::makeAbsolute( DigitType number )
{
    DigitType mask = number >> (sizeof( DigitType ) * 8 - 1);
    return ((number + mask) ^ mask);
}

inline char NEMath::getChar(NEMath::eDigitSign sign)
{
    switch (sign)
    {
    case NEMath::SignNegative:
        return '-';
    case NEMath::SignPositive:
        return '+';
    case NEMath::SignUndefined: // fall through
    default:
        return '\0';
    }
}

/************************************************************************/
// NEMath::sLargeInteger inline functions implementation
/************************************************************************/

inline NEMath::S_LargeInteger::S_LargeInteger( void )
    : hiBits ( 0 ), loBits ( 0 ) 
{   ;   }

inline NEMath::S_LargeInteger::S_LargeInteger( uint32_t hi, uint32_t lo )
    : hiBits ( hi ), loBits ( lo ) 
{   ;   }

inline NEMath::S_LargeInteger::S_LargeInteger( uint64_t num )
    : hiBits ( MACRO_64_HI_BYTE32(num) ), loBits ( MACRO_64_LO_BYTE32(num) ) 
{   ;   }

inline NEMath::S_LargeInteger::S_LargeInteger( const NEMath::S_LargeInteger & src )
    : hiBits ( src.hiBits ), loBits ( src.loBits ) 
{   ;   }

inline NEMath::S_LargeInteger::operator uint64_t ( void ) const
{
    return MACRO_MAKE_64(hiBits, loBits);
}

inline const NEMath::S_LargeInteger & NEMath::sLargeInteger::operator = ( const uint64_t src )
{
    hiBits  = MACRO_64_HI_BYTE32(src); loBits  = MACRO_64_LO_BYTE32(src); return (*this);
}

inline uint64_t NEMath::sLargeInteger::operator + ( const NEMath::S_LargeInteger & rhs ) const
{
    return ( static_cast<uint64_t>(*this) + static_cast<uint64_t>(rhs) );
}

inline uint64_t NEMath::sLargeInteger::operator + ( uint64_t rhs ) const
{
    return ( (static_cast<uint64_t>(*this) + rhs) );
}

inline const NEMath::S_LargeInteger & NEMath::sLargeInteger::operator += ( const NEMath::S_LargeInteger & rhs )
{
    return (*this = static_cast<uint64_t>(*this) + static_cast<uint64_t>(rhs));
}

inline const NEMath::S_LargeInteger & NEMath::sLargeInteger::operator += ( uint64_t rhs )
{
    return ( *this = static_cast<uint64_t>(*this) + rhs );
}

inline uint64_t NEMath::sLargeInteger::operator - ( const NEMath::S_LargeInteger & rhs ) const
{
    return ( static_cast<uint64_t>(*this) - static_cast<uint64_t>(rhs) );
}

inline uint64_t NEMath::sLargeInteger::operator - ( uint64_t rhs ) const
{
    return ( static_cast<uint64_t>(*this) - rhs );
}

inline const NEMath::S_LargeInteger & NEMath::sLargeInteger::operator -= ( const NEMath::S_LargeInteger & rhs )
{
    return ( *this = static_cast<uint64_t>(*this) - static_cast<uint64_t>(rhs) );
}

inline const NEMath::S_LargeInteger & NEMath::sLargeInteger::operator -= ( uint64_t rhs )
{
    return ( *this = static_cast<uint64_t>(*this) - rhs );
}

inline bool NEMath::sLargeInteger::operator == ( const NEMath::S_LargeInteger & other ) const
{
    return (this != &other ? hiBits == other.hiBits && loBits == other.loBits : true );
}

inline bool NEMath::sLargeInteger::operator == ( uint64_t other ) const
{
    return ( static_cast<uint64_t>(*this) == other );
}

inline bool NEMath::sLargeInteger::operator != ( const NEMath::S_LargeInteger & other ) const
{
    return (this != &other ? hiBits != other.hiBits || loBits != other.loBits : false );
}

inline bool NEMath::sLargeInteger::operator != ( uint64_t other ) const
{
    return ( static_cast<uint64_t>(*this) != other );
}

/************************************************************************/
// NEMath::sLargeInteger declare global operators to make streamable
/************************************************************************/
IMPLEMENT_STREAMABLE(NEMath::S_LargeInteger)

#endif  // AREG_BASE_NEMATH_HPP
