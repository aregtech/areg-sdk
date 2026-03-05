/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/persist/private/PropertyKey.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Property Key object to persist application data.
 ************************************************************************/
#include "areg/persist/PropertyKey.hpp"
#include "areg/persist/PersistenceDefs.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/MathDefs.hpp"

#include <utility>
namespace areg {

PropertyKey::PropertyKey( const PropertyKey & source )
    : mSection  ( source.mSection   )
    , mModule   ( source.mModule    )
    , mProperty ( source.mProperty  )
    , mPosition ( source.mPosition  )
    , mKeyType  ( source.mKeyType   )
{
}

PropertyKey::PropertyKey(const String& section, const String& module, const String& property, const String& position)
    : mSection  ( section   )
    , mModule   ( module    )
    , mProperty ( property  )
    , mPosition ( position  )
    , mKeyType  ( PropertyKey::_find_key(mSection, mModule, mProperty, mPosition))
{
}

PropertyKey::PropertyKey(const std::string_view& section, const std::string_view& module, const std::string_view& property, const std::string_view& position)
    : mSection  ( section   )
    , mModule   ( module    )
    , mProperty ( property  )
    , mPosition ( position  )
    , mKeyType  ( PropertyKey::_find_key(mSection, mModule, mProperty, mPosition))
{
}

PropertyKey::PropertyKey(const String& section, const String& module, const String& property, const String& position, areg::ConfigEntry keyType)
    : mSection  ( section   )
    , mModule   ( module    )
    , mProperty ( property  )
    , mPosition ( position  )
    , mKeyType  ( keyType == areg::ConfigEntry::AnyKey ? _find_key(mSection, mModule, mProperty, mPosition) : keyType )
{
}

PropertyKey::PropertyKey(const std::string_view& section, const std::string_view& module, const std::string_view& property, const std::string_view& position, areg::ConfigEntry keyType)
    : mSection  ( section   )
    , mModule   ( module    )
    , mProperty ( property  )
    , mPosition ( position  )
    , mKeyType  ( keyType == areg::ConfigEntry::AnyKey ? _find_key(mSection, mModule, mProperty, mPosition) : keyType )
{
}

PropertyKey::PropertyKey( PropertyKey && source ) noexcept
    : mSection  ( std::move(source.mSection)    )
    , mModule   ( std::move(source.mModule)     )
    , mProperty ( std::move(source.mProperty)   )
    , mPosition ( std::move(source.mPosition)   )
    , mKeyType  ( source.mKeyType               )
{
}

PropertyKey::PropertyKey()
    : mSection  ( )
    , mModule   ( )
    , mProperty ( )
    , mPosition ( )
    , mKeyType  ( areg::ConfigEntry::Invalid )
{
}

PropertyKey::PropertyKey( const String & key )
    : mSection  ( )
    , mModule   ( )
    , mProperty ( )
    , mPosition ( )
    , mKeyType  ( areg::ConfigEntry::Invalid )
{
    parse_key( key );
}

PropertyKey::PropertyKey( String && key )
    : mSection  ( )
    , mModule   ( )
    , mProperty ( )
    , mPosition ( )
    , mKeyType  (areg::ConfigEntry::Invalid)
{
    parse_key( std::move(key) );
}

PropertyKey & PropertyKey::operator = ( const PropertyKey & source )
{
    if ( static_cast<const PropertyKey *>(this) != &source )
    {
        mSection    = source.mSection;
        mModule     = source.mModule;
        mProperty   = source.mProperty;
        mPosition   = source.mPosition;
        mKeyType    = source.mKeyType;
    }

    return (*this);
}

PropertyKey & PropertyKey::operator = ( PropertyKey && source ) noexcept
{
    mSection    = std::move(source.mSection);
    mModule     = std::move(source.mModule);
    mProperty   = std::move(source.mProperty);
    mPosition   = std::move(source.mPosition);
    mKeyType    = source.mKeyType;

    return (*this);
}

PropertyKey & PropertyKey::operator = ( const String & source )
{
    parse_key(source);
    return (*this);
}

PropertyKey & PropertyKey::operator = ( String && source )
{
    parse_key(std::move(source));
    return (*this);
}

bool PropertyKey::operator == ( const PropertyKey & other ) const noexcept
{
    bool result = false;
    if ( this != &other )
    {
        if (mSection == other.mSection)
        {
            if ((mModule == other.mModule) || (mModule == areg::SYNTAX_ALL_MODULES) || (other.mModule == areg::SYNTAX_ALL_MODULES))
            {
                result = mProperty == other.mProperty ? mPosition == other.mPosition : other.mSection.is_empty() && PropertyKey::_is_compatible(mProperty, other.mProperty);
            }
        }
    }
    else
    {
        result = true;
    }

    return result;
}

bool PropertyKey::operator != ( const PropertyKey & other ) const noexcept
{
    return (*this == other ? false : true);
}

PropertyKey::operator uint32_t () const noexcept
{
    uint32_t result{ areg::CHECKSUM_IGNORE };
    if (mSection.is_empty() == false)
    {
        result = areg::crc32_init();
        result = areg::crc32_start(result, mSection.as_string());
        result = areg::crc32_start(result, mModule.as_string());
        result = areg::crc32_start(result, mProperty.as_string());
        result = areg::crc32_start(result, mPosition.as_string());
        result = areg::crc32_finish(result);
    }

    return result;
}

bool PropertyKey::parse_key( const String & key )
{
    String temp(key);
    temp.trim_all();
    if ( temp.is_empty() == false )
    {
        _parse_key(temp);
    }

    return is_valid();
}

bool PropertyKey::parse_key( String && key )
{
    String temp(std::move(key));
    temp.trim_all();
    if ( temp.is_empty() == false )
    {
        _parse_key(temp);
    }

    return is_valid();
}

String PropertyKey::to_string() const
{
    String result(static_cast<uint32_t>(0xFF));
    if ( is_valid() )
    {
        result.append(mSection)
              .append(areg::SYNTAX_OBJECT_SEPARATOR)
              .append(mModule)
              .append(areg::SYNTAX_OBJECT_SEPARATOR)
              .append(mProperty);

        if (mPosition.is_empty() == false)
        {
            result.append(areg::SYNTAX_OBJECT_SEPARATOR).append(mPosition);
        }
    }

    return result;
}

bool PropertyKey::is_exact_module(const String& section, const String& module) const noexcept
{
    return (is_valid() && (mSection == section) && (mModule == module));
}

bool PropertyKey::is_exact_property(const String& section, const String& module, const String& property, const String& position) const noexcept
{
    return  (is_valid() && (mSection == section) && (mModule == module) && (mProperty == property) && (mPosition == position));
}

bool PropertyKey::is_module_property(const String& section, const String& module, const String& property, const String& position) const noexcept
{
    bool result{ false };

    if (is_valid() && (mSection == section) && (mProperty == property) && (module.is_empty() == false))
    {
        if ((mModule == module) || (mModule == areg::SYNTAX_ALL_MODULES) || (module == areg::SYNTAX_ALL_MODULES))
        {
            result = PropertyKey::_is_compatible(mPosition, position);
        }
    }

    return result;
}

bool PropertyKey::is_module_section(const String& section, const String& module) const noexcept
{
    bool result{ false };

    if (is_valid() && (mSection == section) && (module.is_empty() == false))
    {
        result = ((mModule == module) || (mModule == areg::SYNTAX_ALL_MODULES) || (module == areg::SYNTAX_ALL_MODULES));
    }

    return result;
}

void PropertyKey::set_values(const char * section, const char * property, const char * module /*= nullptr*/, const char * position /*= nullptr*/)
{
    mSection    = section != nullptr ? section  : String::EmptyString;
    mModule     = module  != nullptr ? module   : areg::SYNTAX_ALL_MODULES;
    mProperty   = property!= nullptr ? property : String::EmptyString;
    mPosition   = position!= nullptr ? position : String::EmptyString;
    mKeyType    = PropertyKey::_find_key(mSection, mModule, mProperty, mPosition);
}

void PropertyKey::set_values(const String& section, const String& property, const String& module, const String& position)
{
    mSection    = section;
    mModule     = module;
    mProperty   = property;
    mPosition   = position;
    mKeyType    = PropertyKey::_find_key(section, module, property, position);
}

void PropertyKey::set_values(const String& section, const String& property, const String& module, const String& position, areg::ConfigEntry keyType)
{
    mSection    = section;
    mModule     = module;
    mProperty   = property;
    mPosition   = position;
    mKeyType    = keyType;
}

const String & PropertyKey::section() const noexcept
{
    return mSection;
}

const String & PropertyKey::property() const noexcept
{
    return mProperty;
}

const String & PropertyKey::module() const noexcept
{
    return mModule;
}

const String & PropertyKey::position() const noexcept
{
    return mPosition;
}

areg::ConfigEntry PropertyKey::key_type() const noexcept
{
    return mKeyType;
}

bool PropertyKey::is_valid() const noexcept
{
    return ( mSection.is_empty() == false && mModule.is_empty() == false && mProperty.is_empty() == false );
}

bool PropertyKey::has_module() const noexcept
{
    return !mModule.is_empty();
}

bool PropertyKey::has_property() const noexcept
{
    return !mProperty.is_empty();
}

bool PropertyKey::is_all_modules() const noexcept
{
    return (mModule == areg::SYNTAX_ALL_MODULES);
}

bool PropertyKey::is_group_property() const noexcept
{
    return mPosition.ends_with(areg::SYNTAX_GROUP) || mProperty.ends_with(areg::SYNTAX_GROUP);
}

bool PropertyKey::has_section() const noexcept
{
    return !mSection.is_empty();
}

bool PropertyKey::has_position() const noexcept
{
    return !mPosition.is_empty();
}

void PropertyKey::reset() noexcept
{
    mSection.clear();
    mProperty.clear();
    mModule.clear();
    mPosition.clear();
    mKeyType = areg::ConfigEntry::Invalid;
}

inline void PropertyKey::_parse_key(const String& key)
{
    std::vector<StringBase<char>> list = key.split(areg::SYNTAX_OBJECT_SEPARATOR);
    reset();

    if (list.size() != 0)
    {
        mSection = list[0];
        if (list.size() > 1)
        {
            mModule = list[1];
            if (list.size() > 2)
            {
                mProperty = list[2];
                if (list.size() > 3)
                {
                    mPosition = list[3];
                }
                else
                {
                    mPosition = String::EmptyString;
                }
            }
            else
            {
                mProperty = areg::SYNTAX_ALL_MODULES;
            }
        }
        else
        {
            mModule = areg::SYNTAX_ALL_MODULES;
        }

        mKeyType = PropertyKey::_find_key(mSection, mModule, mProperty, mPosition);
    }
}

inline bool PropertyKey::_is_compatible(const String& left, const String& right) noexcept
{
    bool result{ true };
    if (left != right)
    {
        const char* lval = left;
        const char* rval = right;
        while ((*lval != String::EmptyChar) && (*rval != String::EmptyChar) && (*lval == *rval))
        {
            ++lval;
            ++rval;
        }

        result = ((*lval == String::EmptyChar) && (*rval == String::EmptyChar)) ||
                 ((*lval == areg::SYNTAX_ANY_VALUE) || (*rval == areg::SYNTAX_ANY_VALUE));
    }

    return result;
}

inline areg::ConfigEntry PropertyKey::_find_key(const String& section, const String& module, const String& property, const String& position) noexcept
{
    areg::ConfigEntry result{ areg::ConfigEntry::Invalid };

    for (int i = static_cast<int>(areg::ConfigEntry::ConfigVersion); i <= static_cast<int>(areg::ConfigEntry::AnyKey); ++i)
    {
        const auto& key = areg::DefaultPropertyKeys[i];
        if (((areg::SYNTAX_ALL_MODULES == key.section) || (section == key.section))  &&
            ((areg::SYNTAX_ALL_MODULES == key.module)  || (module == key.module))    &&
            ((areg::SYNTAX_ALL_MODULES == key.property)|| (property == key.property))&&
            ((areg::SYNTAX_ALL_MODULES == key.position)|| (position == key.position)))
        {
            result = static_cast<areg::ConfigEntry>(i);
            break;
        }
    }

    return result;
}

} // namespace areg
