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
     **/
    explicit PropertyValue( const char * value );

    /**
     * \brief   Initializes the value from a 32-bit unsigned integer.
     **/
    explicit PropertyValue( uint32_t intValue );

    /**
     * \brief   Initializes the value from a floating-point number.
     **/
    explicit PropertyValue( double dValue );

    /**
     * \brief   Initializes the value from a boolean.
     **/
    explicit PropertyValue(bool bValue);

    /**
     * \brief   Initializes the value from a list of identifiers.
     **/
    explicit PropertyValue( const std::vector<Identifier> & idList );

    explicit PropertyValue( const String & value );

    explicit PropertyValue( String && value ) noexcept;

    PropertyValue( const PropertyValue & source );

    PropertyValue( PropertyValue && source ) noexcept;

    ~PropertyValue() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    inline PropertyValue & operator = ( const PropertyValue & source );

    inline PropertyValue & operator = ( PropertyValue && source ) noexcept;

    inline PropertyValue & operator = ( const String & value );

    inline PropertyValue & operator = ( String && value ) noexcept;

    inline PropertyValue & operator = ( uint32_t intValue );

    inline PropertyValue & operator = ( double dValue );

    inline PropertyValue& operator = (bool bValue);

    inline PropertyValue & operator = ( const std::vector<Identifier> & idList );

    [[nodiscard]]
    inline bool operator == ( const PropertyValue & other ) const noexcept;

    [[nodiscard]]
    inline bool operator != ( const PropertyValue & other ) const noexcept;

    [[nodiscard]]
    inline operator uint32_t() const noexcept;

    [[nodiscard]]
    inline operator const String& () const noexcept;

    [[nodiscard]]
    inline operator double() const noexcept;

    [[nodiscard]]
    inline operator bool() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations and properties
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the string value.
     **/
    [[nodiscard]]
    inline const String & value() const noexcept;

    /**
     * \brief   Sets the value from a C-string.
     **/
    inline void set_value( const char * value );

    /**
     * \brief   Sets the value from a String.
     **/
    inline void set_value( const String & value );

    /**
     * \brief   Sets the value from a String.
     **/
    inline void set_value( String && value );

    /**
     * \brief   Returns the string representation of the value.
     **/
    [[nodiscard]]
    inline const String & as_string() const noexcept;

    /**
     * \brief   Sets the value from a string.
     **/
    inline void set_string( const char * value );

    /**
     * \brief   Returns the value as a boolean.
     **/
    [[nodiscard]]
    inline bool as_boolean() const noexcept;

    /**
     * \brief   Sets the value from a boolean.
     **/
    inline void set_boolean(bool newValue);

    /**
     * \brief   Returns the value as a 32-bit unsigned integer.
     **/
    [[nodiscard]]
    inline uint32_t as_integer( areg::Radix radix = areg::Radix::Decimal ) const;

    /**
     * \brief   Sets the value from a 32-bit unsigned integer.
     *
     * \param   intValue    The integer value to set.
     * \param   radix       The radix for conversion; defaults to decimal.
     **/
    inline void set_integer( uint32_t intValue, areg::Radix radix = areg::Radix::Decimal );

    /**
     * \brief   Returns the value as a floating-point number.
     **/
    [[nodiscard]]
    inline double as_double() const noexcept;

    /**
     * \brief   Sets the value from a floating-point number.
     **/
    inline void set_double( double dValue );

    /**
     * \brief   Returns the identifier value as a combined bit mask of identifiers from the lookup list.
     **/
    [[nodiscard]]
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
    [[nodiscard]]
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
    [[nodiscard]]
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
    inline void parse_value( const char * value );

    /**
     * \brief   Parses a String and sets the value.
     *
     * \param   value       The string to parse.
     **/
    inline void parse_value( const String & value );

    /**
     * \brief   Parses a String and sets the value.
     *
     * \param   value       The string to parse.
     * \note    Move overload. Takes ownership of the string.
     **/
    inline void parse_value( String && value );

    /**
     * \brief   Clears the value.
     **/
    inline void reset();

    /**
     * \brief   Returns the string representation of the value.
     **/
    [[nodiscard]]
    String to_string() const;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:

    //! Parses and normalizes the value data.
    /**
     * \brief   Internal helper that parses the value string.
     **/
    void _parse_value();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   String as a data of Value.
     **/
    String mValue;
};

//////////////////////////////////////////////////////////////////////////
// PropertyValue inline methods
//////////////////////////////////////////////////////////////////////////

inline PropertyValue& PropertyValue::operator = (const PropertyValue& source)
{
    mValue = source.mValue;
    return (*this);
}

inline PropertyValue& PropertyValue::operator = (PropertyValue&& source) noexcept
{
    mValue = std::move(source.mValue);
    return (*this);
}

inline PropertyValue& PropertyValue::operator = (const String& value)
{
    parse_value(value);
    return (*this);
}

inline PropertyValue& PropertyValue::operator = (String&& value) noexcept
{
    parse_value(std::move(value));
    return (*this);
}

inline PropertyValue& PropertyValue::operator = (uint32_t intValue)
{
    mValue.from_uint32(intValue);
    return (*this);
}

inline PropertyValue& PropertyValue::operator = (double dValue)
{
    mValue.from_double(dValue);
    return (*this);
}

inline PropertyValue& PropertyValue::operator = (bool bValue)
{
    mValue.from_bool(bValue);
    return (*this);
}

inline PropertyValue& PropertyValue::operator = (const std::vector<Identifier>& idList)
{
    set_identifier(idList);
    return (*this);
}

inline bool PropertyValue::operator == (const PropertyValue& other) const noexcept
{
    return (mValue == other.mValue);
}

inline bool PropertyValue::operator != (const PropertyValue& other) const noexcept
{
    return (mValue != other.mValue);
}

inline PropertyValue::operator uint32_t() const noexcept
{
    return as_integer(areg::Radix::Decimal);
}

inline PropertyValue::operator const String& () const noexcept
{
    return as_string();
}

inline PropertyValue::operator double() const noexcept
{
    return as_double();
}

inline PropertyValue::operator bool() const noexcept
{
    return as_boolean();
}

inline const String& PropertyValue::value() const noexcept
{
    return mValue;
}

inline void PropertyValue::set_value(const char* value)
{
    parse_value(value);
}

inline void PropertyValue::set_value(const String& value)
{
    parse_value(static_cast<const String&>(value));
}

inline void PropertyValue::set_value(String&& value)
{
    parse_value(std::move(value));
}

inline const String& PropertyValue::as_string() const noexcept
{
    return mValue;
}

inline uint32_t PropertyValue::as_integer(areg::Radix radix /*= areg::Decimal*/) const
{
    return mValue.to_uint32(static_cast<areg::Radix>(radix));
}

inline double PropertyValue::as_double() const noexcept
{
    return mValue.to_double();
}

inline void PropertyValue::set_string(const char* value)
{
    parse_value(value);
}

inline bool PropertyValue::as_boolean() const noexcept
{
    return mValue.to_bool();
}

inline void PropertyValue::set_boolean(bool newValue)
{
    mValue = String::make_string(newValue);
}

inline void PropertyValue::set_integer(uint32_t intValue, areg::Radix radix /*= areg::Decimal*/)
{
    mValue = String::make_string(intValue, radix);
}

inline void PropertyValue::set_double(double dValue)
{
    mValue = String::make_string(dValue);
}

inline void PropertyValue::parse_value(const char* value)
{
    mValue = value;
    _parse_value();
}

inline void PropertyValue::parse_value(const String& value)
{
    mValue = value;
    _parse_value();
}

inline void PropertyValue::parse_value(String&& value)
{
    mValue = std::move(value);
    _parse_value();
}

inline void PropertyValue::reset()
{
    mValue.clear();
}

} // namespace areg
#endif  // AREG_PERSIST_PROPERTYVALUE_HPP
