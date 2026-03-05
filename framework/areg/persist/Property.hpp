#ifndef AREG_PERSIST_PROPERTY_HPP
#define AREG_PERSIST_PROPERTY_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/persist/Property.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Property object to persist data
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/persist/PropertyKey.hpp"
#include "areg/persist/PropertyValue.hpp"
#include "areg/base/KeyValuePair.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Containers.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// Property class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Property object of persistence, which consists of key and value pairs.
 **/
class AREG_API Property
{
//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The definition of type Property, which is a pair of Key and Value
     **/
    using Entry = KeyValuePair<PropertyKey, PropertyValue>;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    Property();

    /**
     * \brief   Initializes property key and value by parsing passed string.
     *
     * \param   strProperty     The string to parse. It should contain key and value. Otherwise, the
     *                          property remains invalid.
     **/
    Property(const String strProperty);

    /**
     * \brief   Initializes the property, sets the key, value, comment and the optional flag to
     *          indicate whether the property can be saved in the configuration for the next session
     *          or not.
     *
     * \param   key             The key of the property.
     * \param   value           The value of the property.
     * \param   comment         The comment to add to the property.
     * \param   is_temporary    The flag to indicate whether the property can be saved in the
     *                          configuration file or not.
     **/
    Property(const PropertyKey& key, const PropertyValue& value, const String & comment = String::EmptyString, bool is_temporary = false);

    /**
     * \brief   Moves objects during initialization.
     **/
    Property(PropertyKey && key, PropertyValue && value, String && comment);
    /**
     * \brief   Moves objects during initialization.
     **/
    Property(PropertyKey&& key, PropertyValue&& value);

    /**
     * \brief   Parses and initializes Key-Value and the comment data from given parameters.
     *
     * \param   keySet      The Key as a string to parse.
     * \param   valueSet    The Value as a string to parse.
     * \param   comment     The optional comment for the property.
     **/
    Property( const String & keySet, const String & valueSet, const String & comment = String::EmptyString, bool is_temporary = false);

    /**
     * \brief   Initializes the property, sets the key, value, comment and the optional flag to
     *          indicate whether the property can be saved in the configuration for the next session
     *          or not.
     *
     * \param   keySet          The key of the property.
     * \param   valueSet        The value of the property.
     * \param   comment         The comment to add to the property.
     * \param   is_temporary    The flag to indicate whether the property can be saved in the
     *                          configuration file or not.
     **/
    Property(const char* keySet, const char* valueSet, const char* comment = nullptr, bool is_temporary = false);
    /**
     * \brief   Initializes the property, sets the key, value, comment and the optional flag to
     *          indicate whether the property can be saved in the configuration for the next session
     *          or not.
     *
     * \param   keySet          The key of the property.
     * \param   valueSet        The value of the property.
     * \param   comment         The comment to add to the property.
     * \param   is_temporary    The flag to indicate whether the property can be saved in the
     *                          configuration file or not.
     **/
    Property(const std::string_view& keySet, const std::string_view& valueSet, const std::string_view& comment = String::EmptyString, bool is_temporary = false);

    /**
     * \brief   Initializes the property, sets all data and the optional flag to indicate whether
     *          the property can be saved in the configuration for the next session or not.
     *
     * \param   section         The section part of the property key.
     * \param   module          The module part of the property key.
     * \param   property        The property part of the property key.
     * \param   position        The position part of the property key.
     * \param   value           The value of the property.
     * \param   comment         The comment to add to the property.
     * \param   is_temporary    The flag to indicate whether the property can be saved in the
     *                          configuration file or not.
     **/
    Property( const std::string_view& section
            , const std::string_view& module
            , const std::string_view& property
            , const std::string_view& position
            , const std::string_view& value
            , const std::string_view& comment = String::EmptyString
            , bool is_temporary = false);

    /**
     * \brief   Initializes Key, value and the comment.
     *
     * \param   newProperty     The property as a key and value pair to set.
     * \param   comment         The optional comment for the property.
     **/
    Property( const Property::Entry & newProperty, const String & comment = String::EmptyString, bool is_temporary = false);

    /**
     * \brief   Copies data from given source.
     *
     * \param   source      The source to copy data from.
     **/
    Property( const Property & source );

    /**
     * \brief   Moves data from given source.
     *
     * \param   source      The source to move data from.
     **/
    Property( Property && source ) noexcept;

    ~Property() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies property data from given source.
     *
     * \param   source      The source to copy data from.
     **/
    Property & operator = ( const Property & source );

    /**
     * \brief   Moves property data from given source.
     *
     * \param   source      The source to move data from.
     **/
    Property & operator = ( Property && source ) noexcept;

    /**
     * \brief   Checks equality of two property objects.
     *
     * \param   other       The property object to check.
     * \return  Returns true if property objects are equal; false otherwise.
     **/
    bool operator == ( const Property & other ) const;

    /**
     * \brief   Checks inequality of two property objects.
     *
     * \param   other       The property object to check.
     * \return  Returns true if property objects are not equal; false otherwise.
     **/
    bool operator != ( const Property & other ) const;

    /**
     * \brief   Converts to a 32-bit integer value.
     **/
    operator uint32_t () const;

//////////////////////////////////////////////////////////////////////////
// Operations and properties
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Sets and parses the key, which consists of several parts separated by key-separator
     *          symbol.
     *
     * \param   keySet      The key as a string to set and parse.
     **/
    void parse_key( const String & keySet );

    /**
     * \brief   Sets the key of property.
     *
     * \param   Key     The key value to set.
     **/
    void set_key( const PropertyKey & Key );

    /**
     * \brief   Sets the key of property.
     *
     * \param   Key     The key value to set.
     * \note    Move overload. Takes ownership of the key.
     **/
    void set_key( PropertyKey && Key );

    /**
     * \brief   Returns the property key.
     **/
    const PropertyKey & key() const;

    /**
     * \brief   Returns the configuration entry type.
     **/
    areg::ConfigEntry type() const;

    /**
     * \brief   Returns string value of the Key. If Key consists of several sections, they are
     *          concatenated and separated by key-separators symbol.
     **/
    String key_string() const;

    /**
     * \brief   Sets and parses the value of property.
     *
     * \param   valueSet    The value as a string to set and parse.
     **/
    void parse_value( const String & valueSet );

    /**
     * \brief   Sets Value of the property.
     *
     * \param   Value       The property value to set.
     **/
    void set_value( const PropertyValue & Value );

    /**
     * \brief   Sets Value of the property.
     *
     * \param   Value       The property value to set.
     * \note    Move overload. Takes ownership of the value.
     **/
    void set_value( PropertyValue && Value );

    /**
     * \brief   Returns the property value.
     **/
    const PropertyValue & value() const;
    /**
     * \brief   Returns the property value.
     * \note    Non-const overload. Allows modification of the returned value.
     **/
    PropertyValue& value();

    /**
     * \brief   Returns Value of Property as a string. If Value consists of several parts, all parts
     *          are concatenated by value-separator symbol.
     **/
    String value_string() const;

    /**
     * \brief   Sets property comment, which is optional.
     *
     * \param   comment     The comment string to set.
     **/
    void set_comment( const String & comment );

    /**
     * \brief   Adds comment to the property.
     *
     * \param   comment     The comment string to append.
     **/
    void add_comment( const String & comment );

    /**
     * \brief   Returns the property comment.
     **/
    const String & comment() const;

    /**
     * \brief   Sets the Key-Value pair of the property.
     *
     * \param   newPair     The Key-Value pair to set.
     **/
    void set_pair( const Property::Entry & newPair );

    /**
     * \brief   Sets the Key-Value pair of the property.
     *
     * \param   newPair     The Key-Value pair to set.
     * \note    Move overload. Takes ownership of the pair.
     **/
    void set_pair( Property::Entry && newPair );

    /**
     * \brief   Returns the Key-Value pair.
     **/
    const Property::Entry & pair() const;

    /**
     * \brief   Returns true if Property is valid. Valid property does not have empty Key.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

    /**
     * \brief   Parses given string, extracts Key and Value parts and initializes Property.
     *
     * \param   strProperties       The string, which contains data to set for Key and Value.
     * \return  Returns true if parsing succeeded and could extract property data; false otherwise.
     **/
    bool parse( const String & strProperties );

    /**
     * \brief   Converts Key-Value pair data to the string.
     **/
    String to_string() const;

    /**
     * \brief   Resets all property data to default values.
     **/
    void reset();

    /**
     * \brief   Returns true if the property is applicable to the specified module (process), i.e.
     *          either it is global and belongs to all modules or it belongs only to the specified
     *          module.
     *
     * \param   module      The name of the module to check the property.
     * \return  Returns true if the property is global or the module of the property key is equal to
     *          the specified module. Otherwise, returns false.
     **/
    [[nodiscard]]
    bool is_module_property(const String& module) const;

    /**
     * \brief   Sets or resets the temporary flag of the property. The properties with the temporary
     *          flag are not saved in the configuration file. Only properties with the specified
     *          module names are saved in the configuration file.
     *
     * \param   is_temporary    If true, the property is temporary.
     **/
    void set_temporary(bool is_temporary);

    /**
     * \brief   Returns true if the property is marked as temporary.
     **/
    [[nodiscard]]
    bool is_temporary() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The flag indicating whether the property is temporary or not.
     **/
    bool                mIsTemporary;

    /**
     * \brief   Comment of Property
     **/
    String              mComment;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Key-Value pair of the Property
     **/
    Property::Entry     mProperty;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER
};

// Declaration of ListProperties within areg namespace
//!< The type to initialize list of properties.
using ListProperties = ArrayList<Property>;

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Hasher of Property class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the Property.
 */
namespace std {
    template<>
    struct hash<areg::Property>
    {
        //! A function to convert Property object to uint32_t.
        inline uint32_t operator()(const areg::Property& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
} // namespace std

#endif  // AREG_PERSIST_PROPERTY_HPP
