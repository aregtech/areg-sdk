#ifndef AREG_PERSIST_PROPERTYKEY_HPP
#define AREG_PERSIST_PROPERTYKEY_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/persist/PropertyKey.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Property Key object to persist data.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "areg/persist/NEPersistence.hpp"

//////////////////////////////////////////////////////////////////////////
// PropertyKey class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Property Key object.
 *          The Property Key object consists of following parts:
 *              - Section
 *              - Module
 *              - Property
 *              - Position
 *          The Property Key is valid if at least section and property exist.
 *          The other parts are optional and can be omitted if not used.
 *          The Property Key format:
 *              section.(module|*).property[.position]
 **/
class AREG_API PropertyKey
{
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    PropertyKey( void );
    
    /**
     * \brief   Destructor
     **/
    ~PropertyKey(void) = default;

    /**
     * \brief   Parses and initializes Key.
     * \param   key     The Key as a string to parse.
     **/
    explicit PropertyKey( const String & key );
    explicit PropertyKey( String && key );

    /**
     * \brief   Copies data from given source
     * \param   source  The source to copy data
     **/
    PropertyKey( const PropertyKey & source );

    /**
     * \brief   Initializes the property key by setting parts.
     *          The key type is automatically guessed by comparing the structure.
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     **/
    PropertyKey( const String & section, const String & module, const String & property, const String & position);
    PropertyKey(const std::string_view& section, const std::string_view& module, const std::string_view& property, const std::string_view& position);

    /**
     * \brief   Initializes the property key by setting parts.
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     * \param   keyType     The type of the property key.
     **/
    PropertyKey(const String& section, const String& module, const String& property, const String& position, NEPersistence::eConfigKeys keyType);
    PropertyKey(const std::string_view& section, const std::string_view& module, const std::string_view& property, const std::string_view& position, NEPersistence::eConfigKeys keyType);

    /**
     * \brief   Moves data from given source
     * \param   source  The source to move data
     **/
    PropertyKey( PropertyKey && source ) noexcept;
    
//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies property Key from given source
     * \param   source  The source to copy data.
     **/
    PropertyKey & operator = ( const PropertyKey & source );

    /**
     * \brief   Moves property Key from given source
     * \param   source  The source to move data.
     **/
    PropertyKey & operator = ( PropertyKey && source ) noexcept;

    /**
     * \brief   Copies property Key from given source as a string
     * \param   source  The source as string to parse and copy data.
     **/
    PropertyKey & operator = ( const String & params );
    PropertyKey & operator = ( String && source );

    /**
     * \brief   Checks equality of two Key objects.
     * \param   other   The Key object to check.
     * \return  Returns true if Key objects are equal.
     **/
    bool operator == ( const PropertyKey & other ) const;

    /**
     * \brief   Checks inequality of two Key objects.
     * \param   other   The Key object to check.
     * \return  Returns true if Key objects are not equal.
     **/
    bool operator != ( const PropertyKey & other ) const;

    /**
     * \brief   Converts and returns 32-bit integer value of Key object.
     **/
    explicit operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations and properties
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Sets Key data, which consists of section, property, module and position sections.
     *          Key 'module' and 'position' are optional and can be empty.
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     **/
    void setValues( const char * section, const char * property, const char * module = nullptr, const char * position = nullptr);
    void setValues( const String & section, const String & property, const String & module, const String & position);

    /**
     * \brief   Sets Key data, which consists of section, property, module and position sections.
     *          Key 'module' and 'position' are optional and can be empty.
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     * \param   keyType     The type of the property key.
     **/
    void setValues(const String& section, const String& property, const String& module, const String& position, NEPersistence::eConfigKeys keyType);

    /**
     * \brief   Returns section part of the Key
     **/
    const String & getSection( void ) const;

    /**
     * \brief   Returns property part of the Key
     **/
    const String & getProperty( void ) const;

    /**
     * \brief   Returns module part of the Key
     **/
    const String & getModule( void ) const;

    /**
     * \brief   Returns position part of the Key
     **/
    const String & getPosition( void ) const;

    /**
     * \brief   Returns the type of the property key.
     **/
    NEPersistence::eConfigKeys getKeyType(void) const;

    /**
     * \brief   Returns true if the Key is valid.
     *          The Key is valid if at least section and property parts are not empty.
     **/
    bool isValid( void ) const;

    /**
     * \brief   Returns true if Key is global and applicable to all modules.
     **/
    bool isAllModules(void) const;

    /**
     * \brief   Returns true if Key belongs to a group of properties.
     *          The key is belongs to a group if either it is global and belongs to all modules,
     *          or the 'position' part of the key ends with '*'.
     **/
    bool isGroupProperty(void) const;

    /**
     * \brief   Returns true if Key has section part.
     **/
    bool hasSection(void) const;

    /**
     * \brief   Returns true if Key has module part.
     **/
    bool hasModule( void ) const;

    /**
     * \brief   Returns true if Key has position part.
     **/
    bool hasProperty( void ) const;

    /**
     * \brief   Returns true if Key has position part.
     **/
    bool hasPosition(void) const;

    /**
     * \brief   Parses given string, extracts and initializes Key data.
     * \param   key     The string, which contains data for Key.
     * \return  Returns true if parsing succeeded and could extract property data.
     **/
    bool parseKey( const String & key );
    bool parseKey( String && key );

    /**
     * \brief   Resets and invalidates Key
     **/
    void resetKey( void );

    /**
     * \brief   Converts Key data to the string.
     *          If Key consists of several parts, each part is concatenated by key-separator symbol.
     **/
    String convToString( void ) const;

    /**
     * \brief   Checks the exact match of the key by section and module names.
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \return  Returns true if the section and the module name of the key have exact match.
     *          Otherwise, returns false.
     **/
    bool isExactModule(const String& section, const String& module) const;

    /**
     * \brief   Checks the exact match of the key by all parameters.
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     **/
    bool isExactProperty(const String& section, const String& module, const String& property, const String& position) const;

    /**
     * \brief   Checks if the section of the key is applicable to the specified module.
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \return  Returns true if the section has exact match, and the module is either global or has exact match.
     *          Otherwise, returns false.
     **/
    bool isModuleSection(const String& section, const String& module) const;

    /**
     * \brief   Checks if the section of the key is applicable to the specified module.
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     * \return  Returns true if the section and the property have exact match, the module is either global or has exact match,
     *          and the position is compatible. The position is compatible if either it has exact match or the position
     *          start with the name of the specified position, which ends with '*'. Otherwise, returns false.
     *          For example, if the position name is "areg_blah_scope" it is compatible with 4 positions:
     *              1. "areg_blah_scope"
     *              2. "areg_blah_*"
     *              3. "areg_*"
     *              4. "*"
     **/
    bool isModuleProperty(const String& section, const String& module, const String& property, const String& position) const;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:

    //! Parses the passed key value and sets property key data.
    inline void _parseKey(const String & key);

    /**
     * \return  Returns true if the 'right' position is compatible with the 'left'. 
     *          The position is compatible if either it has exact match or the position
     *          start with the name of the specified position, which ends with '*'. Otherwise, returns false.
     *          For example, if the position name is "areg_blah_scope" it is compatible with 4 positions:
     *              1. "areg_blah_scope"
     *              2. "areg_blah_*"
     *              3. "areg_*"
     *              4. "*"
     * \param   left    The string value of the left position.
     * \param   right   The string value of the right position to compare.
     * \return  Returns true if specified positions are compatible.
     **/
    inline static bool _isCompatible(const String& left, const String& right);

    /**
     * \brief   Finds the key type that could match the property values.
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     * \param   Returns the key type, wich syntax matches the specified data.
     **/
    inline static NEPersistence::eConfigKeys _findKey(const String& section, const String& module, const String& property, const String& position);

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

    NEPersistence::eConfigKeys mKeyType;
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
        //! A function to convert PropertyKey object to unsigned int.
        inline unsigned int operator()(const PropertyKey& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}

#endif  // AREG_PERSIST_PROPERTYKEY_HPP
