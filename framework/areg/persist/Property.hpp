#ifndef AREG_PERSIST_PROPERTY_HPP
#define AREG_PERSIST_PROPERTY_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/persist/Property.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Property object to persist data
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/persist/PropertyKey.hpp"
#include "areg/persist/PropertyValue.hpp"
#include "areg/base/TEProperty.hpp"
#include "areg/base/String.hpp"

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
    using Entry = TEProperty<PropertyKey, PropertyValue>;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    Property( void );

    /**
     * \brief   Initializes the property, sets the key, value, comment and the optional flag to indicate whether the
     *          property can be saved in the configuration for the next session or not.
     * \param   key         The key of the property.
     * \param   value       The value of the property.
     * \param   comment     The comment to add to the property.
     * \param   isTemporary The flag to indicate whether the property can be saved in the configuration file or not.
     **/
    Property(const PropertyKey& key, const PropertyValue& value, const String & comment = String::EmptyString, bool isTemporary = false);

    /**
     * \brief   Moves objects during initialization.
     **/
    Property(PropertyKey && key, PropertyValue && value, String && comment);
    Property(PropertyKey&& key, PropertyValue&& value);

    /**
     * \brief   Parses and initializes Key-Value and the comment data from given parameters.
     * \param   keySet      The Key as a string to parse.
     * \param   valueSet    The Value as a string to parse.
     * \param   comment     The optional comment for the property.
     **/
    Property( const String & keySet, const String & valueSet, const String & comment = String::EmptyString, bool isTemporary = false);

    /**
     * \brief   Initializes the property, sets the key, value, comment and the optional flag to indicate whether the
     *          property can be saved in the configuration for the next session or not.
     * \param   keySet      The key of the property.
     * \param   valueSet    The value of the property.
     * \param   comment     The comment to add to the property.
     * \param   isTemporary The flag to indicate whether the property can be saved in the configuration file or not.
     **/
    Property(const char* keySet, const char* valueSet, const char* comment = nullptr, bool isTemporary = false);
    Property(const std::string_view& keySet, const std::string_view& valueSet, const std::string_view& comment = String::EmptyString, bool isTemporary = false);

    /**
     * \brief   Initializes the property, sets all data and the optional flag to indicate whether the
     *          property can be saved in the configuration for the next session or not.
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     * \param   value       The value of the property.
     * \param   comment     The comment to add to the property.
     * \param   isTemporary The flag to indicate whether the property can be saved in the configuration file or not.
     **/
    Property( const std::string_view& section
            , const std::string_view& module
            , const std::string_view& property
            , const std::string_view& position
            , const std::string_view& value
            , const std::string_view& comment = String::EmptyString
            , bool isTemporary = false);

    /**
     * \brief   Initializes Key, value and the comment.
     * \param   newProperty The property as a key and value pair to set.
     * \param   comment     The optional comment for the property.
     **/
    Property( const Property::Entry & newProperty, const String & comment = String::EmptyString, bool isTemporary = false);

    /**
     * \brief   Copies data from given source
     * \param   source  The source to copy data
     **/
    Property( const Property & source );

    /**
     * \brief   Moves data from given source
     * \param   source  The source to move data
     **/
    Property( Property && source ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~Property( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies property data from given source
     * \param   source  The source to copy data.
     **/
    Property & operator = ( const Property & source );

    /**
     * \brief   Moves property data from given source
     * \param   source  The source to move data.
     **/
    Property & operator = ( Property && source ) noexcept;

    /**
     * \brief   Checks equality of two property objects.
     * \param   other   The property object to check.
     * \return  Returns true if property objects are equal.
     **/
    bool operator == ( const Property & other ) const;

    /**
     * \brief   Checks inequality of two property objects.
     * \param   other   The property object to check.
     * \return  Returns true if property objects are not equal.
     **/
    bool operator != ( const Property & other ) const;

    /**
     * \brief   Converts and returns 32-bit integer value of Property object.
     **/
    operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and properties
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Sets and parses the key, which consists of several parts
     *          separated by key-separator symbol
     * \param   keySet  The key as a string to set and parse.
     **/
    void parseKey( const String & keySet );

    /**
     * \brief   Sets the key of property.
     * \param   Key     The key value to set.
     **/
    void setKey( const PropertyKey & Key );

    /**
     * \brief   Sets the key of property.
     * \param   Key     The key value to set.
     **/
    void setKey( PropertyKey && Key );

    /**
     * \brief   Returns Key value of the property
     **/
    const PropertyKey & getKey( void ) const;

    /**
     * \brief   Returns string value of the Key. 
     *          If Key consists of several sections, they are concatenated 
     *          and separated by key-separators symbol
     **/
    String getKeyString( void ) const;

    /**
     * \brief   Sets and parses the value of property.
     * \param   valueSet    The value as a string to set and parse.
     **/
    void parseValue( const String & valueSet );

    /**
     * \brief   Sets Value of the property.
     **/
    void setValue( const PropertyValue & Value );

    /**
     * \brief   Sets Value of the property.
     **/
    void setValue( PropertyValue && Value );

    /**
     * \brief   Returns Value of the property
     **/
    const PropertyValue & getValue( void ) const;
    PropertyValue& getValue(void);

    /**
     * \brief   Returns Value of Property as a string.
     *          If Value consists of several perts, all parts are concatenated by value-separator symbol.
     **/
    String getValueString( void ) const;

    /**
     * \brief   Sets property comment, which is optional.
     **/
    void setComment( const String & comment );

    /**
     * \brief   Adds comment to the property
     **/
    void addComment( const String & comment );

    /**
     * \brief   Returns comment of the property
     **/
    const String & getComment( void ) const;

    /**
     * \brief   Sets the Key-Value pair of the property.
     * \param   newPair The Key-Value pair to set
     **/
    void setPropertyPair( const Property::Entry & newPair );

    /**
     * \brief   Sets the Key-Value pair of the property.
     * \param   newPair The Key-Value pair to set
     **/
    void setPropertyPair( Property::Entry && newPair );

    /**
     * \brief   Returns Key-Value pair of the property.
     **/
    const Property::Entry & getPropertyPair( void ) const;

    /**
     * \brief   Returns true if Property is valid.
     *          Valid property does not have empty Key.
     **/
    bool isValid( void ) const;

    /**
     * \brief   Parses given string, extracts Key and Value parts
     *          and initializes Property
     * \param   strProperties   The string, which contains data to set for Key and Value.
     * \return  Returns true if parsing succeeded and could extract property data.
     **/
    bool parseProperty( const String & strProperties );

    /**
     * \brief   Converts Key-Value pair data to the string.
     **/
    String convToString( void ) const;

    /**
     * \brief   Resets and invalidates Property
     **/
    void resetData( void );

    /**
     * \brief   Returns true if the property is applicable to the specified module (process),
     *          i.e. either it is global and belongs to all modules or it belongs
     *          only to the specified module.
     * \param   module  The name of the module to check the property.
     * \return  Returns true if the property is global or the module of the property key is equal
     *          to the specified module. Otherwise, returns false.
     **/
    bool isModuleProperty(const String& module) const;

    /**
     * \brief   Sets or resets the temporary flag of the property.
     *          The properties with the temporary flag are not saved in the configuration file.
     *          Only properties with the specified module names are saved in the configuration file.
     * \param   isTemporary     If true, the property is temporary.
     **/
    void setTemporary(bool isTemporary);

    /**
     * \brief   Returns true if the property is temporary.
     **/
    bool isTemporary(void) const;

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

//////////////////////////////////////////////////////////////////////////
// Hasher of Property class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the Property.
 */
namespace std
{
    template<>
    struct hash<Property>
    {
        //! A function to convert Property object to unsigned int.
        inline unsigned int operator()(const Property& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}

#include "areg/base/Containers.hpp"

// Declaration of ListProperties within NEPersistence namespace
namespace NEPersistence
{
    //!< The type to initialize list of properties.
    using ListProperties = TEArrayList<Property>;
}

#endif  // AREG_PERSIST_PROPERTY_HPP
