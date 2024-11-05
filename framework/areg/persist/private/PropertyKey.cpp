/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/persist/private/PropertyKey.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Property Key object to persist application data.
 ************************************************************************/
#include "areg/persist/PropertyKey.hpp"
#include "areg/persist/NEPersistence.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/NEMath.hpp"

#include <utility>

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
    , mKeyType  ( PropertyKey::_findKey(mSection, mModule, mProperty, mPosition))
{
}

PropertyKey::PropertyKey(const std::string_view& section, const std::string_view& module, const std::string_view& property, const std::string_view& position)
    : mSection  ( section   )
    , mModule   ( module    )
    , mProperty ( property  )
    , mPosition ( position  )
    , mKeyType  ( PropertyKey::_findKey(mSection, mModule, mProperty, mPosition))
{
}

PropertyKey::PropertyKey(const String& section, const String& module, const String& property, const String& position, NEPersistence::eConfigKeys keyType)
    : mSection  ( section   )
    , mModule   ( module    )
    , mProperty ( property  )
    , mPosition ( position  )
    , mKeyType  ( keyType == NEPersistence::eConfigKeys::EntryAnyKey ? _findKey(mSection, mModule, mProperty, mPosition) : keyType )
{
}

PropertyKey::PropertyKey(const std::string_view& section, const std::string_view& module, const std::string_view& property, const std::string_view& position, NEPersistence::eConfigKeys keyType)
    : mSection  ( section   )
    , mModule   ( module    )
    , mProperty ( property  )
    , mPosition ( position  )
    , mKeyType  ( keyType == NEPersistence::eConfigKeys::EntryAnyKey ? _findKey(mSection, mModule, mProperty, mPosition) : keyType )
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

PropertyKey::PropertyKey(void)
    : mSection  ( )
    , mModule   ( )
    , mProperty ( )
    , mPosition ( )
    , mKeyType  ( NEPersistence::eConfigKeys::EntryInvalid )
{
}

PropertyKey::PropertyKey( const String & key )
    : mSection  ( )
    , mModule   ( )
    , mProperty ( )
    , mPosition ( )
    , mKeyType  ( NEPersistence::eConfigKeys::EntryInvalid )
{
    parseKey( key );
}

PropertyKey::PropertyKey( String && key )
    : mSection  ( )
    , mModule   ( )
    , mProperty ( )
    , mPosition ( )
    , mKeyType  (NEPersistence::eConfigKeys::EntryInvalid)
{
    parseKey( std::move(key) );
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
    parseKey(source);
    return (*this);
}

PropertyKey & PropertyKey::operator = ( String && source )
{
    parseKey(std::move(source));
    return (*this);
}

bool PropertyKey::operator == ( const PropertyKey & other ) const
{
    bool result = false;
    if ( this != &other )
    {
        if (mSection == other.mSection)
        {
            if ((mModule == other.mModule) || (mModule == NEPersistence::SYNTAX_ALL_MODULES) || (other.mModule == NEPersistence::SYNTAX_ALL_MODULES))
            {
                result = mProperty == other.mProperty ? mPosition == other.mPosition : other.mSection.isEmpty() && PropertyKey::_isCompatible(mProperty, other.mProperty);
            }
        }
    }
    else
    {
        result = true;
    }

    return result;
}

bool PropertyKey::operator != ( const PropertyKey & other ) const
{
    return (*this == other ? false : true);
}

PropertyKey::operator unsigned int (void) const
{
    unsigned int result{ NEMath::CHECKSUM_IGNORE };
    if (mSection.isEmpty() == false)
    {
        result = NEMath::crc32Init();
        result = NEMath::crc32Start(result, mSection.getString());
        result = NEMath::crc32Start(result, mModule.getString());
        result = NEMath::crc32Start(result, mProperty.getString());
        result = NEMath::crc32Start(result, mPosition.getString());
        result = NEMath::crc32Finish(result);
    }

    return result;
}

bool PropertyKey::parseKey( const String & key )
{
    String temp(key);
    temp.trimAll();
    if ( temp.isEmpty() == false )
    {
        _parseKey(temp);
    }

    return isValid();
}

bool PropertyKey::parseKey( String && key )
{
    String temp(std::move(key));
    temp.trimAll();
    if ( temp.isEmpty() == false )
    {
        _parseKey(temp);
    }

    return isValid();
}

String PropertyKey::convToString(void) const
{
    String result(static_cast<uint32_t>(0xFF));
    if ( isValid() )
    {
        result.append(mSection)
              .append(NEPersistence::SYNTAX_OBJECT_SEPARATOR)
              .append(mModule)
              .append(NEPersistence::SYNTAX_OBJECT_SEPARATOR)
              .append(mProperty);

        if (mPosition.isEmpty() == false)
        {
            result.append(NEPersistence::SYNTAX_OBJECT_SEPARATOR).append(mPosition);
        }
    }

    return result;
}

bool PropertyKey::isExactModule(const String& section, const String& module) const
{
    return (isValid() && (mSection == section) && (mModule == module));
}

bool PropertyKey::isExactProperty(const String& section, const String& module, const String& property, const String& position) const
{
    return  (isValid() && (mSection == section) && (mModule == module) && (mProperty == property) && (mPosition == position));
}

bool PropertyKey::isModuleProperty(const String& section, const String& module, const String& property, const String& position) const
{
    bool result{ false };

    if (isValid() && (mSection == section) && (mProperty == property) && (module.isEmpty() == false))
    {
        if ((mModule == module) || (mModule == NEPersistence::SYNTAX_ALL_MODULES) || (module == NEPersistence::SYNTAX_ALL_MODULES))
        {
            result = PropertyKey::_isCompatible(mPosition, position);
        }
    }

    return result;
}

bool PropertyKey::isModuleSection(const String& section, const String& module) const
{
    bool result{ false };

    if (isValid() && (mSection == section) && (module.isEmpty() == false))
    {
        result = ((mModule == module) || (mModule == NEPersistence::SYNTAX_ALL_MODULES) || (module == NEPersistence::SYNTAX_ALL_MODULES));
    }

    return result;
}

void PropertyKey::setValues(const char * section, const char * property, const char * module /*= nullptr*/, const char * position /*= nullptr*/)
{
    mSection    = section != nullptr ? section  : String::EmptyString;
    mModule     = module  != nullptr ? module   : NEPersistence::SYNTAX_ALL_MODULES;
    mProperty   = property!= nullptr ? property : String::EmptyString;
    mPosition   = position!= nullptr ? position : String::EmptyString;
    mKeyType    = PropertyKey::_findKey(mSection, mModule, mProperty, mPosition);
}

void PropertyKey::setValues(const String& section, const String& property, const String& module, const String& position)
{
    mSection    = section;
    mModule     = module;
    mProperty   = property;
    mPosition   = position;
    mKeyType    = PropertyKey::_findKey(section, module, property, position);
}

void PropertyKey::setValues(const String& section, const String& property, const String& module, const String& position, NEPersistence::eConfigKeys keyType)
{
    mSection    = section;
    mModule     = module;
    mProperty   = property;
    mPosition   = position;
    mKeyType    = keyType;
}

const String & PropertyKey::getSection(void) const
{
    return mSection;
}

const String & PropertyKey::getProperty(void) const
{
    return mProperty;
}

const String & PropertyKey::getModule(void) const
{
    return mModule;
}

const String & PropertyKey::getPosition(void) const
{
    return mPosition;
}

NEPersistence::eConfigKeys PropertyKey::getKeyType(void) const
{
    return mKeyType;
}

bool PropertyKey::isValid(void) const
{
    return ( mSection.isEmpty() == false && mModule.isEmpty() == false && mProperty.isEmpty() == false );
}

bool PropertyKey::hasModule(void) const
{
    return mModule.isEmpty() == false;
}

bool PropertyKey::hasProperty(void) const
{
    return (mProperty.isEmpty() == false);
}

bool PropertyKey::isAllModules(void) const
{
    return (mModule == NEPersistence::SYNTAX_ALL_MODULES);
}

bool PropertyKey::isGroupProperty(void) const
{
    return mPosition.endsWith(NEPersistence::SYNTAX_GROUP) || mProperty.endsWith(NEPersistence::SYNTAX_GROUP);
}

bool PropertyKey::hasSection(void) const
{
    return (mSection.isEmpty() == false);
}

bool PropertyKey::hasPosition(void) const
{
    return mPosition.isEmpty() == false;
}

void PropertyKey::resetKey(void)
{
    mSection.clear();
    mProperty.clear();
    mModule.clear();
    mPosition.clear();
    mKeyType = NEPersistence::eConfigKeys::EntryInvalid;
}

inline void PropertyKey::_parseKey(const String& key)
{
    std::vector<TEString<char>> list = key.split(NEPersistence::SYNTAX_OBJECT_SEPARATOR);
    resetKey();

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
                mProperty = NEPersistence::SYNTAX_ALL_MODULES;
            }
        }
        else
        {
            mModule = NEPersistence::SYNTAX_ALL_MODULES;
        }

        mKeyType = PropertyKey::_findKey(mSection, mModule, mProperty, mPosition);
    }
}

inline bool PropertyKey::_isCompatible(const String& left, const String& right) 
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
                 ((*lval == NEPersistence::SYNTAX_ANY_VALUE) || (*rval == NEPersistence::SYNTAX_ANY_VALUE));
    }

    return result;
}

inline NEPersistence::eConfigKeys PropertyKey::_findKey(const String& section, const String& module, const String& property, const String& position)
{
    NEPersistence::eConfigKeys result{ NEPersistence::eConfigKeys::EntryInvalid };

    for (int i = static_cast<int>(NEPersistence::eConfigKeys::EntryConfigVersion); i <= static_cast<int>(NEPersistence::eConfigKeys::EntryAnyKey); ++i)
    {
        const auto& key = NEPersistence::DefaultPropertyKeys[i];
        if (((NEPersistence::SYNTAX_ALL_MODULES == key.section) || (section == key.section))  &&
            ((NEPersistence::SYNTAX_ALL_MODULES == key.module)  || (module == key.module))    &&
            ((NEPersistence::SYNTAX_ALL_MODULES == key.property)|| (property == key.property))&&
            ((NEPersistence::SYNTAX_ALL_MODULES == key.position)|| (position == key.position)))
        {
            result = static_cast<NEPersistence::eConfigKeys>(i);
            break;
        }
    }

    return result;
}
