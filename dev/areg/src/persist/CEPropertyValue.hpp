#ifndef AREG_PERSIST_CEPROPERTYVALUE_HPP
#define AREG_PERSIST_CEPROPERTYVALUE_HPP
/************************************************************************
 * \file        areg/src/persist/CEPropertyValue.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Property Value object to persist data.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/CEIdentifier.hpp"
#include "areg/src/base/TEArrayList.hpp"

//////////////////////////////////////////////////////////////////////////
// CEPropertyValue class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Property Value object, which is saved as a string.
 **/
class AREG_API CEPropertyValue
{
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    CEPropertyValue( void );
    /**
     * \brief   Initializes Value from string.
     * \param   strValue    The Value as a string to parse.
     **/
    CEPropertyValue( const char * strValue );
    /**
     * \brief   Initializes Value from 32-bit unsigned integer.
     * \param   strValue    The Value as 32-bit unsigned integer to set.
     **/
    CEPropertyValue( unsigned int intValue );
    /**
     * \brief   Initializes Value from digit with floating point.
     * \param   strValue    The Value as a digit with floating point to set.
     **/
    CEPropertyValue( double dValue );
    /**
     * \brief   Initializes Value from list of identifiers.
     * \param   strValue    The Value as a list of identifiers to set.
     **/
    CEPropertyValue( const TEArrayList<CEIdentifier, const CEIdentifier &> idList );
    /**
     * \brief   Copies data from given source
     * \param   source  The source to copy data
     **/
    CEPropertyValue( const CEPropertyValue & source );
    /**
     * \brief   Destructor
     **/
    ~CEPropertyValue( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies property Value from given source
     * \param   source  The source to copy data.
     **/
    const CEPropertyValue & operator = ( const CEPropertyValue & source );

    /**
     * \brief   Copies property Value from given source as a string
     * \param   strValue    The source as string to parse and copy data.
     **/
    const CEPropertyValue & operator = ( const CEString & strValue );

    /**
     * \brief   Copies property Value from given source as a 32-bit unsigned integer
     * \param   intValue    The data to convert.
     **/
    const CEPropertyValue & operator = ( unsigned int intValue );

    /**
     * \brief   Copies property Value from given source as a digit with floating point
     * \param   dValue      The data to convert.
     **/
    const CEPropertyValue & operator = ( double dValue );

    /**
     * \brief   Copies property Value from given source as a list of identifiers
     * \param   idList      The data to convert.
     **/
    const CEPropertyValue & operator = ( const TEArrayList<CEIdentifier, const CEIdentifier &> idList );

    /**
     * \brief   Checks equality of two Value objects.
     * \param   other   The Value object to check.
     * \return  Returns true if Value objects are equal.
     **/
    bool operator == ( const CEPropertyValue & other ) const;

    /**
     * \brief   Checks inequality of two Value objects.
     * \param   other   The Value object to check.
     * \return  Returns true if Value objects are not equal.
     **/
    bool operator != ( const CEPropertyValue & other ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and properties
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns data of Value
     **/
    const CEString & GetValue( void ) const;

    /**
     * \brief   Sets data of Value
     * \param   strValue    The value as string to set
     **/
    void SetValue( const char * strValue );

    /**
     * \brief   Returns value as a string
     **/
    const char * GetString( void ) const;

    /**
     * \brief   Sets value as a string
     * \param   strValue    The value as a string to set
     **/
    void SetString( const char * strValue );

    /**
     * \brief   Returns value as a 32-bit unsigned integer
     **/
    unsigned int GetInteger( NEString::eRadix radix = NEString::RadixDecimal ) const;

    /**
     * \brief   Converts and sets value as a 32-bit unsigned integer
     * \param   intValue    The value as a 32-bit unsigned integer to convert and set
     * \param   radix       The conversion criteria of integer. By default, the passed integer is converted as a decimal.
     **/
    void SetInteger( unsigned int intValue, NEString::eRadix radix = NEString::RadixDecimal );

    /**
     * \brief   Returns value as a digit with floating point
     **/
    double GetDouble( void ) const;

    /**
     * \brief   Converts and sets value as a digit with floating point
     * \param   dValue      The value as a digit with floating point to convert and set
     **/
    void SetDouble( double dValue );

    /**
     * \brief   Returns value as a list of identifiers.
     * \param   idList  On output, this contains list of identifiers
     * \return  Number of identifiers in the list.
     **/
    unsigned int GetIndetifier( const TEArrayList<CEIdentifier, const CEIdentifier &> idList ) const;

    /**
     * \brief   Converts and sets value as a list of Identifiers
     * \param   idList      The list of Identifiers to convert and set.
     **/
    void SetIndentifier( const TEArrayList<CEIdentifier, const CEIdentifier &> idList );

    /**
     * \brief   Parses given string, extracts Value data.
     * \param   strValue    The string, which contains data for Value.
     **/
    void ParseValue( const char * strValue );

    /**
     * \brief   Resets and invalidates Value
     **/
    void ResetValue( void );

    /**
     * \brief   Converts and Value data to the string.
     **/
    CEString ToString( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   String as a data of Value.
     **/
    CEString mValue;
};

#endif  // AREG_PERSIST_CEPROPERTYVALUE_HPP
