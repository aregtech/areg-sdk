/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/persist/private/PropertyKey.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Property Key object to persist application data.
 ************************************************************************/
#include "areg/persist/PropertyKey.hpp"
#include "areg/persist/private/NEPersistence.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/NEMath.hpp"

#include <utility>

PropertyKey::PropertyKey( const PropertyKey & source )
    : mSection  ( source.mSection   )
    , mProperty ( source.mProperty  )
    , mModule   ( source.mModule    )
    , mPosition ( source.mPosition  )
{
}

PropertyKey::PropertyKey( PropertyKey && source ) noexcept
    : mSection  ( std::move(source.mSection)    )
    , mProperty ( std::move(source.mProperty)   )
    , mModule   ( std::move(source.mModule)     )
    , mPosition ( std::move(source.mPosition)   )
{
}

PropertyKey::PropertyKey( const String & key )
    : mSection  ( )
    , mProperty ( )
    , mModule   ( )
    , mPosition ( )
{
    parseKey( key );
}

PropertyKey::PropertyKey( String && key )
    : mSection  ( )
    , mProperty ( )
    , mModule   ( )
    , mPosition ( )
{
    parseKey( std::move(key) );
}

PropertyKey & PropertyKey::operator = ( const PropertyKey & source )
{
    if ( static_cast<const PropertyKey *>(this) != &source )
    {
        mSection    = source.mSection;
        mProperty   = source.mProperty;
        mModule     = source.mModule;
        mPosition   = source.mPosition;
    }

    return (*this);
}

PropertyKey & PropertyKey::operator = ( PropertyKey && source ) noexcept
{
    mSection    = std::move(source.mSection);
    mProperty   = std::move(source.mProperty);
    mModule     = std::move(source.mModule);
    mPosition   = std::move(source.mPosition);

    return (*this);
}

PropertyKey & PropertyKey::operator = ( const String & params )
{
    parseKey(params);
    return (*this);
}

PropertyKey & PropertyKey::operator = ( String && params )
{
    parseKey(std::move(params));
    return (*this);
}

bool PropertyKey::operator == ( const PropertyKey & other ) const
{
    bool result = true;
    if ( this != &other )
    {
        result = mSection == other.mSection && mProperty == other.mProperty;
        if ( result && mModule.isEmpty() == false )
            result = other.mModule.isEmpty() || mModule == other.mModule;
        if ( result && mSection.isEmpty() == false )
            result = other.mSection.isEmpty() || mSection == other.mSection;
    }

    return result;
}

bool PropertyKey::operator != ( const PropertyKey & other ) const
{
    bool result = false;

    if ( this != &other )
    {
        result = mSection != other.mSection || mProperty != other.mProperty;
        if ( result == false && mModule.isEmpty() == false )
            result = mModule != other.mModule;
        if ( result == false && mSection.isEmpty() == false )
            result = mSection != other.mSection;
    }
    return result;
}

PropertyKey::operator unsigned int (void) const
{
    return (mSection.isEmpty() == false ? NEMath::crc32Calculate( mSection.getString()) : NEMath::CHECKSUM_IGNORE);
}

bool PropertyKey::parseKey( const String & key )
{
    String temp(key);
    temp.trimAll(temp);
    if ( temp.isEmpty() == false )
    {
        _parseKey(temp);
    }

    return isValid();
}

bool PropertyKey::parseKey( String && key )
{
    String temp(std::move(key));
    temp.trimAll(temp);
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
        result.append(mSection).append(NEPersistence::SYNTAX_OBJECT_SEPARATOR).append(mProperty);

        if ( mModule.isEmpty() == false )
        {
            result.append(NEPersistence::SYNTAX_OBJECT_SEPARATOR).append(mModule);
            if ( mPosition.isEmpty() == false )
            {
                result.append(NEPersistence::SYNTAX_OBJECT_SEPARATOR).append(mPosition);
            }
        }
    }

    return result;
}

void PropertyKey::setValues(const char * section, const char * property, const char * module /*= nullptr*/, const char * position /*= nullptr*/)
{
    mSection    = section != nullptr ? section  : NEString::EmptyStringA;
    mProperty   = property!= nullptr ? property : NEString::EmptyStringA;
    mModule     = module  != nullptr ? module   : NEString::EmptyStringA;
    mPosition   = position!= nullptr ? position : NEString::EmptyStringA;
}

void PropertyKey::setValues(const String& section, const String& property, const String& module, const String& position)
{
    mSection    = section;
    mProperty   = property;
    mModule     = module;
    mPosition   = position;
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

bool PropertyKey::isValid(void) const
{
    return ( mSection.isEmpty() == false && mProperty.isEmpty() == false );
}

bool PropertyKey::hasModule(void) const
{
    return mModule.isEmpty() == false;
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
}

inline void PropertyKey::_parseKey(String& key)
{
    NEString::CharPos pos = key.findFirst(NEPersistence::SYNTAX_OBJECT_SEPARATOR);
    NEString::CharPos oldPos = NEString::START_POS;
    if (key.isValidPosition(pos))
    {
        key.substring(mSection, oldPos, pos - oldPos);

        oldPos = pos + 1;
        pos = key.findFirst(NEPersistence::SYNTAX_OBJECT_SEPARATOR, oldPos);
        if (key.isValidPosition(pos))
        {
            key.substring(mProperty, oldPos, pos - oldPos);
            oldPos = pos + 1;
            pos = key.findFirst(NEPersistence::SYNTAX_OBJECT_SEPARATOR, oldPos);
            if (key.isValidPosition(pos))
            {
                key.substring(mModule, oldPos, pos - oldPos);
                oldPos = pos + 1;
                pos = key.findFirst(NEPersistence::SYNTAX_OBJECT_SEPARATOR, oldPos);

                key.substring(mPosition, oldPos, key.isValidPosition(pos) ? pos - oldPos : NEString::COUNT_ALL);
            }
            else
            {
                mModule = key.substring(oldPos);
            }
        }
        else
        {
            mProperty = key.substring(oldPos);
        }
    }
}
