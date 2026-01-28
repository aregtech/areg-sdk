#ifndef AREG_PERSIST_PROPERTYVALUE_HPP
#define AREG_PERSIST_PROPERTYVALUE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/persist/PropertyValue.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Property Value object to persist data.
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
     * \param   value       The Value as a string to parse.
     **/
    explicit PropertyValue( const char * value );
    /**
     * \brief   Initializes Value from 32-bit unsigned integer.
     * \param   intValue    The Value as 32-bit unsigned integer to set.
     **/
    explicit PropertyValue( unsigned int intValue );
    /**
     * \brief   Initializes Value from digit with floating point.
     * \param   dValue      The Value as a digit with floating point to set.
     **/
    explicit PropertyValue( double dValue );
    /**
     * \brief   Initializes Value from digit with floating point.
     * \param   bValue    The Value as a boolean value.
     **/
    explicit PropertyValue(bool bValue);
    /**
     * \brief   Initializes Value from list of identifiers.
     * \param   idList  The Value as a list of identifiers to set.
     **/
    explicit PropertyValue( const std::vector<Identifier> & idList );
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
     * \param   intValue    The 32-bit digit data to convert.
     **/
    PropertyValue & operator = ( unsigned int intValue );

    /**
     * \brief   Copies property Value from given source as a digit with floating point
     * \param   dValue      The digit with the floating point to convert.
     **/
    PropertyValue & operator = ( double dValue );

    /**
     * \brief   Copies property Value from given source as a digit with floating point
     * \param   bValue      The boolean data to convert.
     **/
    PropertyValue& operator = (bool bValue);

    /**
     * \brief   Copies property Value from given source as a list of identifiers
     * \param   idList      The data to convert.
     **/
    PropertyValue & operator = ( const std::vector<Identifier> & idList );

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

    /**
     * \brief   Converts the property value into the 32-bit unsigned integer.
     **/
    operator unsigned int(void) const;

    /**
     * \brief   Converts the property value into the string.
     **/
    operator const String& (void) const;

    /**
     * \brief   Converts the property value into the floating point digit.
     **/
    operator double(void) const;

    /**
     * \brief   Converts the property value into the boolean.
     **/
    operator bool(void) const;

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
     * \brief   Returns the boolean value of the property value
     **/
    bool getBoolean(void) const;

    /**
     * \brief   Sets the boolean value.
     **/
    void setBoolean(bool newValue);

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
     * \brief   Returns combined digital value of the identifiers.
     * \param   lookupList  The lookup list to convert literal identifiers into integer values.
     * \return  Combined with logical OR operation digital value of the identifiers.
     **/
    unsigned int getIndetifier( const std::vector<Identifier> & lookupList) const;

    /**
     * \brief   Converts and sets value as a list of Identifiers
     * \param   idList      The list of Identifiers to convert and set.
     **/
    void setIndentifier( const std::vector<Identifier> & idList );

    /**
     * \brief   Parses the value and returns the list of identifiers.
     * \param   lookupList  The lookup list to convert literal identifiers into integer values.
     * \return  Combined with logical OR ('|') operation digital value of the identifiers.
     **/
    TEArrayList<Identifier> getIdentifierList(const std::vector<Identifier>& lookupList) const;

    /**
     * \brief   Sets as a string the list of values of the property.
     * \param   idBits      The bitwise set of IDs.
     * \param   lookupList  The lookup list to find entry and convert to the string.
     **/
    void setIdentifierList(unsigned int idBits, const std::vector<Identifier>& lookupList);

    /**
     * \brief   Parses the value and returns the list of string. The strings in the value should be
     *          separated by logical OR ('|') symbol to get the list. Otherwise, the returned array
     *          has one entry and that entry is equal to the value.
     * \param   makeUnique  Flag, indicating whether the entries in the result should be unique or not.
     *                      If the flag is 'false', the entries in the result list are not checked.
     * \return  Returns the list of strings.
     **/
    TEArrayList<String> getValueList(bool makeUnique = false) const;

    /**
     * \brief   Sets a list of string as a value.
     * \param   list    The list of strings to set as a value;
     **/
    void setValueList(const std::vector<String>& list);

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
