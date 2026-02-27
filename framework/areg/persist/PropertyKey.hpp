#ifndef AREG_PERSIST_PROPERTYKEY_HPP
#define AREG_PERSIST_PROPERTYKEY_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/persist/PropertyKey.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Property Key object to persist data.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "areg/persist/PersistenceDefs.hpp"

//////////////////////////////////////////////////////////////////////////
// PropertyKey class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Structured key for configuration entries, composed of section, module, property, and
 *          position parts. Valid if at least section and property exist; other parts are optional.
 **/
class AREG_API PropertyKey
{
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    PropertyKey();
    
    /**
     * \brief   Destructor
     **/
    ~PropertyKey() = default;

    /**
     * \brief   Parses and initializes key.
     *
     * \param   key     The key as a string to parse.
     **/
    explicit PropertyKey( const String & key );
    /**
     * \brief   Parses and initializes key.
     *
     * \param   key     The key as a string to parse.
     * \note    Move overload. Takes ownership of the string.
     **/
    explicit PropertyKey( String && key );

    /**
     * \brief
     *
     * \param   source      The source to copy data from.
     **/
    PropertyKey( const PropertyKey & source );

    /**
     * \brief   Initializes the property key from individual parts; the key type is automatically
     *          determined.
     *
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     **/
    PropertyKey( const String & section, const String & module, const String & property, const String & position);
    /**
     * \brief   Initializes the property key from individual parts; the key type is automatically
     *          determined.
     *
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     **/
    PropertyKey(const std::string_view& section, const std::string_view& module, const std::string_view& property, const std::string_view& position);

    /**
     * \brief   Initializes the property key from individual parts with explicit key type.
     *
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     * \param   keyType     The type of the property key.
     **/
    PropertyKey(const String& section, const String& module, const String& property, const String& position, NEPersistence::ConfigEntry keyType);
    /**
     * \brief   Initializes the property key from individual parts with explicit key type.
     *
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     * \param   keyType     The type of the property key.
     **/
    PropertyKey(const std::string_view& section, const std::string_view& module, const std::string_view& property, const std::string_view& position, NEPersistence::ConfigEntry keyType);

    /**
     * \brief
     *
     * \param   source      The source to move data from.
     **/
    PropertyKey( PropertyKey && source ) noexcept;
    
//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief
     *
     * \param   source      The source to copy data from.
     **/
    PropertyKey & operator = ( const PropertyKey & source );

    /**
     * \brief
     *
     * \param   source      The source to move data from.
     **/
    PropertyKey & operator = ( PropertyKey && source ) noexcept;

    /**
     * \brief   Assigns the key from a string, parsing its components.
     *
     * \param   source      The source string to parse and copy.
     **/
    PropertyKey & operator = ( const String & source );
    /**
     * \brief   Assigns the key from a string, parsing its components.
     *
     * \param   source      The source string to parse and copy.
     * \note    Move overload. Takes ownership of the string.
     **/
    PropertyKey & operator = ( String && source );

    /**
     * \brief   Returns true if two keys are equal; false otherwise.
     *
     * \param   other       The key object to compare.
     **/
    bool operator == ( const PropertyKey & other ) const;

    /**
     * \brief   Returns true if two keys are not equal; false otherwise.
     *
     * \param   other       The key object to compare.
     **/
    bool operator != ( const PropertyKey & other ) const;

    /**
     * \brief   Converts the key to a 32-bit unsigned integer hash.
     **/
    explicit operator uint32_t () const;

//////////////////////////////////////////////////////////////////////////
// Operations and properties
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Sets key data from individual components. Module and position are optional.
     *
     * \param   section     The section part of the property key.
     * \param   property    The property part of the property key.
     * \param   module      The module part of the property key. Optional; defaults to empty.
     * \param   position    The position part of the property key. Optional; defaults to empty.
     **/
    void set_values( const char * section, const char * property, const char * module = nullptr, const char * position = nullptr);
    /**
     * \brief   Sets key data from individual components.
     *
     * \param   section     The section part of the property key.
     * \param   property    The property part of the property key.
     * \param   module      The module part of the property key.
     * \param   position    The position part of the property key.
     **/
    void set_values( const String & section, const String & property, const String & module, const String & position);

    /**
     * \brief   Sets key data from individual components with explicit key type.
     *
     * \param   section     The section part of the property key.
     * \param   property    The property part of the property key.
     * \param   module      The module part of the property key.
     * \param   position    The position part of the property key.
     * \param   keyType     The type of the property key.
     **/
    void set_values(const String& section, const String& property, const String& module, const String& position, NEPersistence::ConfigEntry keyType);

    /**
     * \brief   Returns the section part of the key.
     **/
    const String & section() const;

    /**
     * \brief   Returns the property part of the key.
     **/
    const String & property() const;

    /**
     * \brief   Returns the module part of the key.
     **/
    const String & module() const;

    /**
     * \brief   Returns the position part of the key.
     **/
    const String & position() const;

    /**
     * \brief   Returns the key type.
     **/
    NEPersistence::ConfigEntry key_type() const;

    /**
     * \brief   Returns true if the key is valid (has both section and property parts).
     **/
    bool is_valid() const;

    /**
     * \brief   Returns true if the key is applicable to all modules.
     **/
    bool is_all_modules() const;

    /**
     * \brief   Returns true if the key belongs to a group of properties (global with all modules or
     *          position ending with '*').
     **/
    bool is_group_property() const;

    /**
     * \brief   Returns true if the key has a section part.
     **/
    bool has_section() const;

    /**
     * \brief   Returns true if the key has a module part.
     **/
    bool has_module() const;

    /**
     * \brief   Returns true if the key has a property part.
     **/
    bool has_property() const;

    /**
     * \brief   Returns true if the key has a position part.
     **/
    bool has_position() const;

    /**
     * \brief   Parses a key string and initializes the key components.
     *
     * \param   key     The string to parse as a key.
     * \return  Returns true if parsing succeeded; false otherwise.
     **/
    bool parse_key( const String & key );
    /**
     * \brief   Parses a key string and initializes the key components.
     *
     * \param   key     The string to parse as a key.
     * \return  Returns true if parsing succeeded; false otherwise.
     * \note    Move overload. Takes ownership of the string.
     **/
    bool parse_key( String && key );

    /**
     * \brief   Clears all key components.
     **/
    void reset();

    /**
     * \brief   Converts the key to its string representation, with parts separated by the key
     *          separator.
     **/
    String to_string() const;

    /**
     * \brief   Returns true if the key's section and module exactly match the specified values.
     *
     * \param   section     The section part to compare.
     * \param   module      The module part to compare.
     **/
    bool is_exact_module(const String& section, const String& module) const;

    /**
     * \brief   Returns true if all key components exactly match the specified values.
     *
     * \param   section     The section part to compare.
     * \param   module      The module part to compare.
     * \param   property    The property part to compare.
     * \param   position    The position part to compare.
     **/
    bool is_exact_property(const String& section, const String& module, const String& property, const String& position) const;

    /**
     * \brief   Returns true if the key applies to the specified section and module (module can be
     *          global or exact match).
     *
     * \param   section     The section part to compare.
     * \param   module      The module part to compare.
     **/
    bool is_module_section(const String& section, const String& module) const;

    /**
     * \brief   Returns true if the key applies to the specified components (module and position
     *          support wildcards).
     *
     * \param   section     The section part to compare.
     * \param   module      The module part to compare.
     * \param   property    The property part to compare.
     * \param   position    The position part to compare.
     **/
    bool is_module_property(const String& section, const String& module, const String& property, const String& position) const;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:

    //! Parses the passed key value and sets property key data.
    /**
     * \brief   Internal helper that parses a key string and sets the section, module, property, and
     *          position fields.
     *
     * \param   key     The key string to parse.
     **/
    inline void _parse_key(const String & key);

    /**
     * \brief   Returns true if the right position is compatible with the left (supports wildcard
     *          matching).
     *
     * \param   left        The position string to match against.
     * \param   right       The position string to test.
     **/
    inline static bool _is_compatible(const String& left, const String& right);

    /**
     * \brief   Returns the key type that matches the given components.
     *
     * \param   section     The section part of the key.
     * \param   module      The module part of the key.
     * \param   property    The property part of the key.
     * \param   position    The position part of the key.
     **/
    inline static NEPersistence::ConfigEntry _find_key(const String& section, const String& module, const String& property, const String& position);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Section part of the key, which is at position 1
     **/
    String  mSection;      // pos 1
    /**
     * \brief   Module part of the key, which is at position 3
     **/
    String  mModule;       // pos 2
    /**
     * \brief   Property part of the key, which is at position 2
     **/
    String  mProperty;     // pos 3
    /**
     * \brief   Position part of the key, which is at position 4
     **/
    String  mPosition;     // pos 4

    NEPersistence::ConfigEntry mKeyType;
};

//////////////////////////////////////////////////////////////////////////
// Hasher of PropertyKey class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the PropertyKey.
 */
namespace std
{
    template<>
    struct hash<PropertyKey>
    {
        //! A function to convert PropertyKey object to uint32_t.
        inline uint32_t operator()(const PropertyKey& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
}

#endif  // AREG_PERSIST_PROPERTYKEY_HPP
