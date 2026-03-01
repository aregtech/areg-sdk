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
#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"
#include "areg/base/Identifier.hpp"
#include "areg/base/ArrayList.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// PropertyValue class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Property value wrapper storing configuration values as strings; supports conversion to
 *          integer, boolean, double, and identifier formats.
 **/
class AREG_API PropertyValue
{
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    PropertyValue() = default;
    /**
     * \brief   Initializes the value from a C-string.
     *
     * \param   value       The string value to initialize from.
     **/
    explicit PropertyValue( const char * value );
    /**
     * \brief   Initializes the value from a 32-bit unsigned integer.
     *
     * \param   intValue    The integer value to initialize from.
     **/
    explicit PropertyValue( uint32_t intValue );
    /**
     * \brief   Initializes the value from a floating-point number.
     *
     * \param   dValue      The floating-point value to initialize from.
     **/
    explicit PropertyValue( double dValue );
    /**
     * \brief   Initializes the value from a boolean.
     *
     * \param   bValue      The boolean value to initialize from.
     **/
    explicit PropertyValue(bool bValue);
    /**
     * \brief   Initializes the value from a list of identifiers.
     *
     * \param   idList      The list of identifiers to initialize from.
     **/
    explicit PropertyValue( const std::vector<Identifier> & idList );
    /**
     * \brief
     *
     * \param   value       The source to copy value from.
     **/
    explicit PropertyValue( const String & value );
    /**
     * \brief
     *
     * \param   value       The source to copy value from.
     * \note    Move overload. Takes ownership of the string.
     **/
    explicit PropertyValue( String && value ) noexcept;
    /**
     * \brief
     *
     * \param   source      The source to copy data from.
     **/
    PropertyValue( const PropertyValue & source );
    /**
     * \brief
     *
     * \param   source      The source to move data from.
     **/
    PropertyValue( PropertyValue && source ) noexcept;
    /**
     * \brief   Destructor
     **/
    ~PropertyValue() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief
     *
     * \param   source      The source to copy data from.
     **/
    PropertyValue & operator = ( const PropertyValue & source );

    /**
     * \brief
     *
     * \param   source      The source to move data from.
     **/
    PropertyValue & operator = ( PropertyValue && source ) noexcept;

    /**
     * \brief   Assigns a value from a string, parsing its contents.
     *
     * \param   value       The string value to assign.
     **/
    PropertyValue & operator = ( const String & value );
    /**
     * \brief   Assigns a value from a string, parsing its contents.
     *
     * \param   value       The string value to assign.
     * \note    Move overload. Takes ownership of the string.
     **/
    PropertyValue & operator = ( String && value ) noexcept;

    /**
     * \brief   Assigns an integer value.
     *
     * \param   intValue    The integer value to assign.
     **/
    PropertyValue & operator = ( uint32_t intValue );

    /**
     * \brief   Assigns a floating-point value.
     *
     * \param   dValue      The floating-point value to assign.
     **/
    PropertyValue & operator = ( double dValue );

    /**
     * \brief   Assigns a boolean value.
     *
     * \param   bValue      The boolean value to assign.
     **/
    PropertyValue& operator = (bool bValue);

    /**
     * \brief   Assigns a value from a list of identifiers.
     *
     * \param   idList      The list of identifiers to assign.
     **/
    PropertyValue & operator = ( const std::vector<Identifier> & idList );

    /**
     * \brief   Returns true if two values are equal; false otherwise.
     *
     * \param   other       The value object to compare.
     **/
    bool operator == ( const PropertyValue & other ) const;

    /**
     * \brief   Returns true if two values are not equal; false otherwise.
     *
     * \param   other       The value object to compare.
     **/
    bool operator != ( const PropertyValue & other ) const;

    /**
     * \brief   Converts the value to a 32-bit unsigned integer.
     **/
    operator uint32_t() const;

    /**
     * \brief   Converts the value to a string.
     **/
    operator const String& () const;

    /**
     * \brief   Converts the value to a floating-point number.
     **/
    operator double() const;

    /**
     * \brief   Converts the value to a boolean.
     **/
    operator bool() const;

//////////////////////////////////////////////////////////////////////////
// Operations and properties
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the string value.
     **/
    const String & value() const;

    /**
     * \brief   Sets the value from a C-string.
     *
     * \param   value       The string value to set.
     **/
    void set_value( const char * value );

    /**
     * \brief   Sets the value from a String.
     *
     * \param   value       The string value to set.
     **/
    void set_value( const String & value );

    /**
     * \brief   Sets the value from a String.
     *
     * \param   value       The string value to set.
     * \note    Move overload. Takes ownership of the string.
     **/
    void set_value( String && value );

    /**
     * \brief   Returns the string representation of the value.
     **/
    const String & as_string() const;

    /**
     * \brief   Sets the value from a string.
     *
     * \param   value       The string value to set.
     **/
    void set_string( const char * value );

    /**
     * \brief   Returns the value as a boolean.
     **/
    bool as_boolean() const;

    /**
     * \brief   Sets the value from a boolean.
     *
     * \param   newValue    The boolean value to set.
     **/
    void set_boolean(bool newValue);

    /**
     * \brief   Returns the value as a 32-bit unsigned integer.
     *
     * \param   radix       The radix for conversion; defaults to decimal.
     **/
    uint32_t as_integer( areg::Radix radix = areg::Radix::Decimal ) const;

    /**
     * \brief   Sets the value from a 32-bit unsigned integer.
     *
     * \param   intValue    The integer value to set.
     * \param   radix       The radix for conversion; defaults to decimal.
     **/
    void set_integer( uint32_t intValue, areg::Radix radix = areg::Radix::Decimal );

    /**
     * \brief   Returns the value as a floating-point number.
     **/
    double as_double() const;

    /**
     * \brief   Sets the value from a floating-point number.
     *
     * \param   dValue      The floating-point value to set.
     **/
    void set_double( double dValue );

    /**
     * \brief   Returns the identifier value as a combined bit mask of identifiers from the lookup
     *          list.
     *
     * \param   lookupList      The list of identifiers for lookup.
     **/
    uint32_t identifier( const std::vector<Identifier> & lookupList) const;

    /**
     * \brief   Sets the value from a list of identifiers.
     *
     * \param   idList      The list of identifiers to set.
     **/
    void set_identifier( const std::vector<Identifier> & idList );

    /**
     * \brief   Returns the identifiers parsed from the value.
     *
     * \param   lookupList      The list of identifiers for lookup.
     **/
    ArrayList<Identifier> identifier_list(const std::vector<Identifier>& lookupList) const;

    /**
     * \brief   Sets the value from identifier bits converted to string form.
     *
     * \param   idBits          The bit mask of identifier IDs.
     * \param   lookupList      The list of identifiers for conversion.
     **/
    void set_identifier_list(uint32_t idBits, const std::vector<Identifier>& lookupList);

    /**
     * \brief   Returns a list of strings parsed from the value (separated by '|').
     *
     * \param   makeUnique      If true, returns unique entries; if false, returns all entries
     *                          as-is.
     **/
    ArrayList<String> value_list(bool makeUnique = false) const;

    /**
     * \brief   Sets the value from a list of strings (joined by '|').
     *
     * \param   list    The list of strings to set as the value.
     **/
    void set_value_list(const std::vector<String>& list);

    /**
     * \brief   Parses a C-string and sets the value.
     *
     * \param   value       The string to parse.
     **/
    void parse_value( const char * value );

    /**
     * \brief   Parses a String and sets the value.
     *
     * \param   value       The string to parse.
     **/
    void parse_value( const String & value );

    /**
     * \brief   Parses a String and sets the value.
     *
     * \param   value       The string to parse.
     * \note    Move overload. Takes ownership of the string.
     **/
    void parse_value( String && value );

    /**
     * \brief   Clears the value.
     **/
    void reset();

    /**
     * \brief   Returns the string representation of the value.
     **/
    String to_string() const;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:

    //! Parses and normalizes the value data.
    /**
     * \brief   Internal helper that parses the value string.
     **/
    inline void _parse_value();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   String as a data of Value.
     **/
    String mValue;
};

} // namespace areg
#endif  // AREG_PERSIST_PROPERTYVALUE_HPP
