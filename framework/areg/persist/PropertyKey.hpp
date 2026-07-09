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
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/persist/PropertyKey.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Property Key object to persist data.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"
#include "areg/persist/PersistenceDefs.hpp"

namespace areg {

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
    
    ~PropertyKey() = default;

    explicit PropertyKey( const String & key );

    explicit PropertyKey( String && key );

    PropertyKey( const PropertyKey & source );

    /**
     * \brief   Initializes the property key from individual parts; the key type is automatically determined.
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
    PropertyKey(std::string_view section, std::string_view module, std::string_view property, std::string_view position);

    /**
     * \brief   Initializes the property key from individual parts with explicit key type.
     *
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     * \param   keyType     The type of the property key.
     **/
    PropertyKey(const String& section, const String& module, const String& property, const String& position, areg::ConfigEntry keyType);

    /**
     * \brief   Initializes the property key from individual parts with explicit key type.
     *
     * \param   section     The section part of the property key.
     * \param   module      The module part of the property key.
     * \param   property    The property part of the property key.
     * \param   position    The position part of the property key.
     * \param   keyType     The type of the property key.
     **/
    PropertyKey(std::string_view section, std::string_view module, std::string_view property, std::string_view position, areg::ConfigEntry keyType);

    PropertyKey( PropertyKey && source ) noexcept;
    
//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    PropertyKey & operator = ( const PropertyKey & source );

    PropertyKey & operator = ( PropertyKey && source ) noexcept;

    inline PropertyKey & operator = ( const String & source );

    inline PropertyKey & operator = ( String && source );

    [[nodiscard]]
    bool operator == ( const PropertyKey & other ) const noexcept;

    [[nodiscard]]
    bool operator != ( const PropertyKey & other ) const noexcept;

    [[nodiscard]]
    explicit operator uint32_t () const noexcept;

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
    void set_values(const String& section, const String& property, const String& module, const String& position, areg::ConfigEntry keyType);

    /**
     * \brief   Returns the section part of the key.
     **/
    [[nodiscard]]
    inline const String & section() const noexcept;

    /**
     * \brief   Returns the property part of the key.
     **/
    [[nodiscard]]
    inline const String & property() const noexcept;

    /**
     * \brief   Returns the module part of the key.
     **/
    [[nodiscard]]
    inline const String & module() const noexcept;

    /**
     * \brief   Returns the position part of the key.
     **/
    [[nodiscard]]
    inline const String & position() const noexcept;

    /**
     * \brief   Returns the key type.
     **/
    [[nodiscard]]
    inline areg::ConfigEntry key_type() const noexcept;

    /**
     * \brief   Returns true if the key is valid (has both section and property parts).
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Returns true if the key is applicable to all modules.
     **/
    [[nodiscard]]
    inline bool is_all_modules() const noexcept;

    /**
     * \brief   Returns true if the key belongs to a group of properties (global with all modules or
     *          position ending with '*').
     **/
    [[nodiscard]]
    inline bool is_group_property() const noexcept;

    /**
     * \brief   Returns true if the key has a section part.
     **/
    [[nodiscard]]
    inline bool has_section() const noexcept;

    /**
     * \brief   Returns true if the key has a module part.
     **/
    [[nodiscard]]
    inline bool has_module() const noexcept;

    /**
     * \brief   Returns true if the key has a property part.
     **/
    [[nodiscard]]
    inline bool has_property() const noexcept;

    /**
     * \brief   Returns true if the key has a position part.
     **/
    [[nodiscard]]
    inline bool has_position() const noexcept;

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
    inline void reset() noexcept;

    /**
     * \brief   Converts the key to its string representation, with parts separated by the key
     *          separator.
     **/
    [[nodiscard]]
    String to_string() const;

    /**
     * \brief   Returns true if the key's section and module exactly match the specified values.
     *
     * \param   section     The section part to compare.
     * \param   module      The module part to compare.
     **/
    [[nodiscard]]
    inline bool is_exact_module(const String& section, const String& module) const noexcept;

    /**
     * \brief   Returns true if all key components exactly match the specified values.
     *
     * \param   section     The section part to compare.
     * \param   module      The module part to compare.
     * \param   property    The property part to compare.
     * \param   position    The position part to compare.
     **/
    [[nodiscard]]
    inline bool is_exact_property(const String& section, const String& module, const String& property, const String& position) const noexcept;

    /**
     * \brief   Returns true if the key applies to the specified section and module (module can be
     *          global or exact match).
     *
     * \param   section     The section part to compare.
     * \param   module      The module part to compare.
     **/
    [[nodiscard]]
    bool is_module_section(const String& section, const String& module) const noexcept;

    /**
     * \brief   Returns true if the key applies to the specified components (module and position
     *          support wildcards).
     *
     * \param   section     The section part to compare.
     * \param   module      The module part to compare.
     * \param   property    The property part to compare.
     * \param   position    The position part to compare.
     **/
    [[nodiscard]]
    bool is_module_property(const String& section, const String& module, const String& property, const String& position) const noexcept;

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
    inline static bool _is_compatible(const String& left, const String& right) noexcept;

    /**
     * \brief   Returns the key type that matches the given components.
     *
     * \param   section     The section part of the key.
     * \param   module      The module part of the key.
     * \param   property    The property part of the key.
     * \param   position    The position part of the key.
     **/
    inline static areg::ConfigEntry _find_key(const String& section, const String& module, const String& property, const String& position) noexcept;

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

    areg::ConfigEntry mKeyType;
};

//////////////////////////////////////////////////////////////////////////
// PropertyKey inline methods
//////////////////////////////////////////////////////////////////////////

inline PropertyKey& PropertyKey::operator = (const String& source)
{
    parse_key(source);
    return (*this);
}

inline PropertyKey& PropertyKey::operator = (String&& source)
{
    parse_key(std::move(source));
    return (*this);
}

inline bool PropertyKey::is_exact_module(const String& section, const String& module) const noexcept
{
    return (is_valid() && (mSection == section) && (mModule == module));
}

inline bool PropertyKey::is_exact_property(const String& section, const String& module, const String& property, const String& position) const noexcept
{
    return  (is_valid() && (mSection == section) && (mModule == module) && (mProperty == property) && (mPosition == position));
}

inline const String& PropertyKey::section() const noexcept
{
    return mSection;
}

inline const String& PropertyKey::property() const noexcept
{
    return mProperty;
}

inline const String& PropertyKey::module() const noexcept
{
    return mModule;
}

inline const String& PropertyKey::position() const noexcept
{
    return mPosition;
}

inline areg::ConfigEntry PropertyKey::key_type() const noexcept
{
    return mKeyType;
}

inline bool PropertyKey::is_valid() const noexcept
{
    return (mSection.is_empty() == false && mModule.is_empty() == false && mProperty.is_empty() == false);
}

inline bool PropertyKey::has_module() const noexcept
{
    return !mModule.is_empty();
}

inline bool PropertyKey::has_property() const noexcept
{
    return !mProperty.is_empty();
}

inline bool PropertyKey::is_all_modules() const noexcept
{
    return (mModule == areg::SYNTAX_ALL_MODULES);
}

inline bool PropertyKey::is_group_property() const noexcept
{
    return mPosition.ends_with(areg::SYNTAX_GROUP) || mProperty.ends_with(areg::SYNTAX_GROUP);
}

inline bool PropertyKey::has_section() const noexcept
{
    return !mSection.is_empty();
}

inline bool PropertyKey::has_position() const noexcept
{
    return !mPosition.is_empty();
}

inline void PropertyKey::reset() noexcept
{
    mSection.clear();
    mProperty.clear();
    mModule.clear();
    mPosition.clear();
    mKeyType = areg::ConfigEntry::Invalid;
}

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Hasher of PropertyKey class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the PropertyKey.
 */
namespace std {
    template<>
    struct hash<areg::PropertyKey>
    {
        //! A function to convert PropertyKey object to uint32_t.
        inline uint32_t operator()(const areg::PropertyKey& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
} // namespace std

#endif  // AREG_PERSIST_PROPERTYKEY_HPP
