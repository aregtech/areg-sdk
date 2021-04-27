/************************************************************************
 * \file        areg/persist/private/CEPropertyKey.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Property Key object to persist application data.
 ************************************************************************/
#include "areg/persist/CEPropertyKey.hpp"
#include "areg/persist/private/NEPersistence.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/NEMath.hpp"

CEPropertyKey::CEPropertyKey ( void )
    : mSection  ( )
    , mProperty ( )
    , mModule   ( )
    , mPosition ( )
{
    ; // do nothing
}

CEPropertyKey::CEPropertyKey( const CEPropertyKey & source )
    : mSection  ( source.mSection )
    , mProperty ( source.mProperty )
    , mModule   ( source.mModule )
    , mPosition ( source.mPosition )
{
    ; // do nothing
}

CEPropertyKey::CEPropertyKey( const CEString & key )
    : mSection  ( )
    , mProperty ( )
    , mModule   ( )
    , mPosition ( )
{
    ParseKey( key );
}

CEPropertyKey::~CEPropertyKey( void )
{
    ; // do nothing
}

const CEPropertyKey & CEPropertyKey::operator = ( const CEPropertyKey & source )
{
    if ( static_cast<const CEPropertyKey *>(this) != &source )
    {
        mSection    = source.mSection;
        mProperty   = source.mProperty;
        mModule     = source.mModule;
        mPosition   = source.mPosition;
    }
    return (*this);
}

const CEPropertyKey & CEPropertyKey::operator = ( const CEString & params )
{
    ParseKey(params);
    return (*this);
}

bool CEPropertyKey::operator == ( const CEPropertyKey & other ) const
{
    bool result = true;
    if ( this != &other )
    {
        result = mSection == other.mSection && mProperty == other.mProperty;
        if ( result && mModule.IsEmpty() == false )
            result = other.mModule.IsEmpty() || mModule == other.mModule;
        if ( result && mSection.IsEmpty() == false )
            result = other.mSection.IsEmpty() || mSection == other.mSection;
    }
    return result;
}

bool CEPropertyKey::operator != ( const CEPropertyKey & other ) const
{
    bool result = false;
    if ( this != &other )
    {
        result = mSection != other.mSection || mProperty != other.mProperty;
        if ( result == false && mModule.IsEmpty() == false )
            result = mModule != other.mModule;
        if ( result == false && mSection.IsEmpty() == false )
            result = mSection != other.mSection;
    }
    return result;
}

CEPropertyKey::operator unsigned int(void) const
{
    return (mSection.IsEmpty() == false ? NEMath::Crc32Calculate( mSection.GetBuffer()) : NEMath::CHECKSUM_IGNORE);
}

bool CEPropertyKey::ParseKey( const CEString & key )
{
    CEString temp;
    key.TrimAll(temp);
    if ( temp.IsEmpty() == false )
    {
        NEString::CharPos pos   = temp.FindFirstOf( NEPersistence::SYNTAX_OBJECT_SEPARATOR );
        NEString::CharPos oldPos= NEString::StartPos;
        if ( pos != NEString::InvalidPos )
        {
            mSection    = temp.Substr(oldPos, pos - oldPos);
            oldPos      = pos + 1;
            pos         = temp.FindFirstOf( NEPersistence::SYNTAX_OBJECT_SEPARATOR, oldPos );
            if ( pos != NEString::InvalidPos )
            {
                mProperty   = temp.Substr(oldPos, pos - oldPos);
                oldPos      = pos + 1;
                pos         = temp.FindFirstOf( NEPersistence::SYNTAX_OBJECT_SEPARATOR, oldPos );
                if ( pos != NEString::InvalidPos )
                {
                    mModule     = temp.Substr(oldPos, pos - oldPos);
                    oldPos      = pos + 1;
                    pos         = temp.FindFirstOf( NEPersistence::SYNTAX_OBJECT_SEPARATOR, oldPos );
                    mPosition   = pos != NEString::InvalidPos ? temp.Substr( oldPos, pos - oldPos ) : temp.Substr(oldPos);
                }
                else
                {
                    mModule     = temp.Substr(oldPos);
                }
            }
            else
            {
                mProperty       = temp.Substr(oldPos);
            }
        }
    }
    return IsValid();
}

CEString CEPropertyKey::ToString(void) const
{
    CEString result("");
    if ( IsValid() )
    {
        result += mSection;
        result += NEPersistence::SYNTAX_OBJECT_SEPARATOR;
        result += mProperty;
        if ( mModule.IsEmpty() == false )
        {
            result += NEPersistence::SYNTAX_OBJECT_SEPARATOR;
            result += mModule;
            if ( mPosition.IsEmpty() == false )
            {
                result += NEPersistence::SYNTAX_OBJECT_SEPARATOR;
                result += mPosition;
            }
        }
    }
    return result;
}

void CEPropertyKey::SetValues(const char * section, const char * property, const char * module /*= NULL*/, const char * position /*= NULL*/)
{
    mSection    = section != NULL ? section  : "";
    mProperty   = property!= NULL ? property : "";
    mModule     = module  != NULL ? module   : "";
    mPosition   = position!= NULL ? position : "";
}

const CEString & CEPropertyKey::GetSection(void) const
{
    return mSection;
}

const CEString & CEPropertyKey::GetProperty(void) const
{
    return mProperty;
}

const CEString & CEPropertyKey::GetModule(void) const
{
    return mModule;
}

const CEString & CEPropertyKey::GetPosition(void) const
{
    return mPosition;
}

bool CEPropertyKey::IsValid(void) const
{
    return ( mSection.IsEmpty() == false && mProperty.IsEmpty() == false );
}

bool CEPropertyKey::HasModule(void) const
{
    return mModule.IsEmpty() == false;
}

bool CEPropertyKey::HasPosition(void) const
{
    return mPosition.IsEmpty() == false;
}

void CEPropertyKey::ResetKey(void)
{
    mSection.Clear();
    mProperty.Clear();
    mModule.Clear();
    mPosition.Clear();
}
