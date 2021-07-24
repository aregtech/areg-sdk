#ifndef AREG_PERSIST_PROPERTYVALUE_HPP
#define AREG_PERSIST_PROPERTYVALUE_HPP
/************************************************************************
 * \file        areg/persist/PropertyValue.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Property Value object to persist data.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "areg/base/Identifier.hpp"
#include "areg/base/TEArrayList.hpp"

//////////////////////////////////////////////////////////////////////////
// PropertyValue class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Property Value object, which is saved as a string.
 **/
class AREG_API PropertyValue
{
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    PropertyValue( void );
    /**
     * \brief   Initializes Value from string.
     * \param   strValue    The Value as a string to parse.
     **/
    PropertyValue( const char * strValue );
    /**
     * \brief   Initializes Value from 32-bit unsigned integer.
     * \param   strValue    The Value as 32-bit unsigned integer to set.
     **/
    PropertyValue( unsigned int intValue );
    /**
     * \brief   Initializes Value from digit with floating point.
     * \param   strValue    The Value as a digit with floating point to set.
     **/
    PropertyValue( double dValue );
    /**
     * \brief   Initializes Value from list of identifiers.
     * \param   strValue    The Value as a list of identifiers to set.
     **/
    PropertyValue( const TEArrayList<Identifier, const Identifier &> idList );
    /**
     * \brief   Copies data from given source
     * \param   source  The source to copy data
     **/
    PropertyValue( const PropertyValue & source );
    /**
     * \brief   Destructor
     **/
    ~PropertyValue( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies property Value from given source
     * \param   source  The source to copy data.
     **/
    const PropertyValue & operator = ( const PropertyValue & source );

    /**
     * \brief   Copies property Value from given source as a string
     * \param   strValue    The source as string to parse and copy data.
     **/
    const PropertyValue & operator = ( const String & strValue );

    /**
     * \brief   Copies property Value from given source as a 32-bit unsigned integer
     * \param   intValue    The data to convert.
     **/
    const PropertyValue & operator = ( unsigned int intValue );

    /**
     * \brief   Copies property Value from given source as a digit with floating point
     * \param   dValue      The data to convert.
     **/
    const PropertyValue & operator = ( double dValue );

    /**
     * \brief   Copies property Value from given source as a list of identifiers
     * \param   idList      The data to convert.
     **/
    const PropertyValue & operator = ( const TEArrayList<Identifier, const Identifier &> idList );

    /**
     * \brief   Checks equality of two Value objects.
     * \param   other   The Value object to check.
     * \return  Returns true if Value objects are equal.
     **/
    bool operator == ( const PropertyValue & other ) const;

    /**
     * \brief   Checks inequality of two Value objects.
     * \param   other   The Value object to check.
     * \return  Returns true if Value objects are not equal.
     **/
    bool operator != ( const PropertyValue & other ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and properties
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns data of Value
     **/
    const String & getValue( void ) const;

    /**
     * \brief   Sets data of Value
     * \param   strValue    The value as string to set
     **/
    void setValue( const char * strValue );

    /**
     * \brief   Returns value as a string
     **/
    const char * getString( void ) const;

    /**
     * \brief   Sets value as a string
     * \param   strValue    The value as a string to set
     **/
    void setString( const char * strValue );

    /**
     * \brief   Returns value as a 32-bit unsigned integer
     **/
    unsigned int getInteger( NEString::eRadix radix = NEString::RadixDecimal ) const;

    /**
     * \brief   Converts and sets value as a 32-bit unsigned integer
     * \param   intValue    The value as a 32-bit unsigned integer to convert and set
     * \param   radix       The conversion criteria of integer. By default, the passed integer is converted as a decimal.
     **/
    void setInteger( unsigned int intValue, NEString::eRadix radix = NEString::RadixDecimal );

    /**
     * \brief   Returns value as a digit with floating point
     **/
    double getDouble( void ) const;

    /**
     * \brief   Converts and sets value as a digit with floating point
     * \param   dValue      The value as a digit with floating point to convert and set
     **/
    void setDouble( double dValue );

    /**
     * \brief   Returns value as a list of identifiers.
     * \param   idList  On output, this contains list of identifiers
     * \return  Number of identifiers in the list.
     **/
    unsigned int getIndetifier( const TEArrayList<Identifier, const Identifier &> idList ) const;

    /**
     * \brief   Converts and sets value as a list of Identifiers
     * \param   idList      The list of Identifiers to convert and set.
     **/
    void setIndentifier( const TEArrayList<Identifier, const Identifier &> idList );

    /**
     * \brief   Parses given string, extracts Value data.
     * \param   strValue    The string, which contains data for Value.
     **/
    void parseValue( const char * strValue );

    /**
     * \brief   Resets and invalidates Value
     **/
    void resetValue( void );

    /**
     * \brief   Converts and Value data to the string.
     **/
    String convToString( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   String as a data of Value.
     **/
    String mValue;
};

#endif  // AREG_PERSIST_PROPERTYVALUE_HPP
