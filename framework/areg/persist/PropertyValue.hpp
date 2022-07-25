#ifndef AREG_PERSIST_PROPERTYVALUE_HPP
#define AREG_PERSIST_PROPERTYVALUE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/persist/PropertyValue.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
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
    PropertyValue( void ) = default;
    /**
     * \brief   Initializes Value from string.
     * \param   value    The Value as a string to parse.
     **/
    explicit PropertyValue( const char * value );
    /**
     * \brief   Initializes Value from 32-bit unsigned integer.
     * \param   value    The Value as 32-bit unsigned integer to set.
     **/
    explicit PropertyValue( unsigned int intValue );
    /**
     * \brief   Initializes Value from digit with floating point.
     * \param   value    The Value as a digit with floating point to set.
     **/
    explicit PropertyValue( double dValue );
    /**
     * \brief   Initializes Value from list of identifiers.
     * \param   value    The Value as a list of identifiers to set.
     **/
    explicit PropertyValue( const TEArrayList<Identifier> idList );
    /**
     * \brief   Copies value from given source
     * \param   value   The source to copy value
     **/
    explicit PropertyValue( const String & value );
    explicit PropertyValue( String && value ) noexcept;
    /**
     * \brief   Copies data from given source
     * \param   source  The source to copy data
     **/
    PropertyValue( const PropertyValue & source );
    /**
     * \brief   Moves data from given source
     * \param   source  The source to move data
     **/
    PropertyValue( PropertyValue && source ) noexcept;
    /**
     * \brief   Destructor
     **/
    ~PropertyValue( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies property Value from given source
     * \param   source  The source to copy data.
     **/
    PropertyValue & operator = ( const PropertyValue & source );

    /**
     * \brief   Moves property Value from given source
     * \param   source  The source to move data.
     **/
    PropertyValue & operator = ( PropertyValue && source ) noexcept;

    /**
     * \brief   Copies property Value from given source as a string
     * \param   value   The source as string to parse and copy data.
     **/
    PropertyValue & operator = ( const String & value );
    PropertyValue & operator = ( String && value ) noexcept;

    /**
     * \brief   Copies property Value from given source as a 32-bit unsigned integer
     * \param   intValue    The data to convert.
     **/
    PropertyValue & operator = ( unsigned int intValue );

    /**
     * \brief   Copies property Value from given source as a digit with floating point
     * \param   dValue      The data to convert.
     **/
    PropertyValue & operator = ( double dValue );

    /**
     * \brief   Copies property Value from given source as a list of identifiers
     * \param   idList      The data to convert.
     **/
    PropertyValue & operator = ( const TEArrayList<Identifier> & idList );

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
     * \param   value    The value as string to set
     **/
    void setValue( const char * value );

    /**
     * \brief   Sets data of Value
     * \param   value    The value as string to set
     **/
    void setValue( const String & value );

    /**
     * \brief   Sets data of Value
     * \param   value    The value as string to set
     **/
    void setValue( String && value );

    /**
     * \brief   Returns value as a string
     **/
    const String & getString( void ) const;

    /**
     * \brief   Sets value as a string
     * \param   value    The value as a string to set
     **/
    void setString( const char * value );

    /**
     * \brief   Returns value as a 32-bit unsigned integer
     **/
    unsigned int getInteger( NEString::eRadix radix = NEString::eRadix::RadixDecimal ) const;

    /**
     * \brief   Converts and sets value as a 32-bit unsigned integer
     * \param   intValue    The value as a 32-bit unsigned integer to convert and set
     * \param   radix       The conversion criteria of integer. By default, the passed integer is converted as a decimal.
     **/
    void setInteger( unsigned int intValue, NEString::eRadix radix = NEString::eRadix::RadixDecimal );

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
    unsigned int getIndetifier( const TEArrayList<Identifier> idList ) const;

    /**
     * \brief   Converts and sets value as a list of Identifiers
     * \param   idList      The list of Identifiers to convert and set.
     **/
    void setIndentifier( const TEArrayList<Identifier> idList );

    /**
     * \brief   Parses given string, extracts Value data.
     * \param   value    The string, which contains data for Value.
     **/
    void parseValue( const char * value );

    /**
     * \brief   Parses given string, extracts Value data.
     * \param   value    The string, which contains data for Value.
     **/
    void parseValue( const String & value );

    /**
     * \brief   Parses given string, extracts Value data.
     * \param   value    The string, which contains data for Value.
     **/
    void parseValue( String && value );

    /**
     * \brief   Resets and invalidates Value
     **/
    void resetValue( void );

    /**
     * \brief   Converts and Value data to the string.
     **/
    String convToString( void ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:

    //! Parses and normalizes the value data.
    inline void _parseValue(void);

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
