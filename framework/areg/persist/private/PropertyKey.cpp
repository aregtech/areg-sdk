/************************************************************************
 * \file        areg/persist/private/PropertyKey.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Property Key object to persist application data.
 ************************************************************************/
#include "areg/persist/PropertyKey.hpp"
#include "areg/persist/private/NEPersistence.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/NEMath.hpp"

PropertyKey::PropertyKey ( void )
    : mSection  ( )
    , mProperty ( )
    , mModule   ( )
    , mPosition ( )
{
    ; // do nothing
}

PropertyKey::PropertyKey( const PropertyKey & source )
    : mSection  ( source.mSection )
    , mProperty ( source.mProperty )
    , mModule   ( source.mModule )
    , mPosition ( source.mPosition )
{
    ; // do nothing
}

PropertyKey::PropertyKey( const String & key )
    : mSection  ( )
    , mProperty ( )
    , mModule   ( )
    , mPosition ( )
{
    parseKey( key );
}

PropertyKey::~PropertyKey( void )
{
    ; // do nothing
}

const PropertyKey & PropertyKey::operator = ( const PropertyKey & source )
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

const PropertyKey & PropertyKey::operator = ( const String & params )
{
    parseKey(params);
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
    String temp;
    key.trimAll(temp);
    if ( temp.isEmpty() == false )
    {
        NEString::CharPos pos   = temp.findFirstOf( NEPersistence::SYNTAX_OBJECT_SEPARATOR );
        NEString::CharPos oldPos= NEString::StartPos;
        if ( pos != NEString::InvalidPos )
        {
            mSection    = temp.substring(oldPos, pos - oldPos);
            oldPos      = pos + 1;
            pos         = temp.findFirstOf( NEPersistence::SYNTAX_OBJECT_SEPARATOR, oldPos );
            if ( pos != NEString::InvalidPos )
            {
                mProperty   = temp.substring(oldPos, pos - oldPos);
                oldPos      = pos + 1;
                pos         = temp.findFirstOf( NEPersistence::SYNTAX_OBJECT_SEPARATOR, oldPos );
                if ( pos != NEString::InvalidPos )
                {
                    mModule     = temp.substring(oldPos, pos - oldPos);
                    oldPos      = pos + 1;
                    pos         = temp.findFirstOf( NEPersistence::SYNTAX_OBJECT_SEPARATOR, oldPos );
                    mPosition   = pos != NEString::InvalidPos ? temp.substring( oldPos, pos - oldPos ) : temp.substring(oldPos);
                }
                else
                {
                    mModule     = temp.substring(oldPos);
                }
            }
            else
            {
                mProperty       = temp.substring(oldPos);
            }
        }
    }
    return isValid();
}

String PropertyKey::convToString(void) const
{
    String result("");
    if ( isValid() )
    {
        result += mSection;
        result += NEPersistence::SYNTAX_OBJECT_SEPARATOR;
        result += mProperty;
        if ( mModule.isEmpty() == false )
        {
            result += NEPersistence::SYNTAX_OBJECT_SEPARATOR;
            result += mModule;
            if ( mPosition.isEmpty() == false )
            {
                result += NEPersistence::SYNTAX_OBJECT_SEPARATOR;
                result += mPosition;
            }
        }
    }
    return result;
}

void PropertyKey::setValues(const char * section, const char * property, const char * module /*= NULL*/, const char * position /*= NULL*/)
{
    mSection    = section != NULL ? section  : "";
    mProperty   = property!= NULL ? property : "";
    mModule     = module  != NULL ? module   : "";
    mPosition   = position!= NULL ? position : "";
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
