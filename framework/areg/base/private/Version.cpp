/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Version.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Version class
 *
 ************************************************************************/

#include "areg/base/Version.hpp"

#include "areg/base/NEUtilities.hpp"
#include "areg/base/NECommon.hpp"

//////////////////////////////////////////////////////////////////////////
// Version class implementation
//////////////////////////////////////////////////////////////////////////

const Version & Version::getInvalidVersion( void )
{
    static const Version _invalidVersion( 0, 0, 0 );
    return _invalidVersion;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
Version::Version( void )
    : mMajor    (0)
    , mMinor    (0)
    , mPatch    (0)
{
}

Version::Version( unsigned int major, unsigned int minor, unsigned int patch /*= 0*/ )
    : mMajor    (major)
    , mMinor    (minor)
    , mPatch    (patch)
{
}

Version::Version( const Version &src )
    : mMajor    (src.mMajor)
    , mMinor    (src.mMinor)
    , mPatch    (src.mPatch)
{
}

Version::Version( Version && src ) noexcept
    : mMajor    ( src.mMajor )
    , mMinor    ( src.mMinor )
    , mPatch    ( src.mPatch )
{
}

Version::Version(const IEInStream & stream)
    : mMajor    ( 0 )
    , mMinor    ( 0 )
    , mPatch    ( 0 )
{
    stream >> mMajor;
    stream >> mMinor;
    stream >> mPatch;
}

Version::Version(const char * version)
    : mMajor    (0)
    , mMinor    (0)
    , mPatch    (0)
{
    convFromString( version );
}

Version::Version(const String & version)
    : mMajor    (0)
    , mMinor    (0)
    , mPatch    (0)
{
    convFromString(version);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
Version & Version::convFromString( const String & version )
{
    mMajor  = 0;
    mMinor  = 0;
    mPatch  = 0;

    String major, minor, patch;
    NEString::CharPos pos = NEString::START_POS;
    pos = version.substring( major, NECommon::OBJECT_SEPARATOR, pos);
    pos = version.substring( minor, NECommon::OBJECT_SEPARATOR, pos);
    version.substring( patch, NECommon::OBJECT_SEPARATOR, pos);

    mMajor  = major.toUInt32();;
    mMinor  = minor.toUInt32();
    mPatch  = (mMajor != 0) && (mMinor != 0) ? patch.toUInt32() : 0;

    return (*this);
}

Version & Version::convFromString( const char * version )
{
	return convFromString( String(version != nullptr ? version : NEString::EmptyStringA) );
}

Version & Version::operator = ( const Version &src )
{
    if (this != &src)
    {
        this->mMajor    = src.mMajor;
        this->mMinor    = src.mMinor;
        this->mPatch    = src.mPatch;
    }
    return (*this);
}

Version & Version::operator = ( Version && src ) noexcept
{
    if ( this != &src )
    {
        this->mMajor    = src.mMajor;
        this->mMinor    = src.mMinor;
        this->mPatch    = src.mPatch;
    }

    return (*this);
}

bool Version::operator < ( const Version & version ) const
{
    return  (this == &version ? false :
                (mMajor < version.mMajor) || 
                (mMajor == version.mMajor && mMinor < version.mMinor) ||
                (mMajor == version.mMajor && mMinor == version.mMajor && mPatch < version.mPatch)
             );
}

bool Version::operator > ( const Version & version ) const
{
    return  (this == &version ? false :
                (mMajor > version.mMajor) ||
                (mMajor == version.mMajor && mMinor > version.mMinor) ||
                (mMajor == version.mMajor && mMinor == version.mMajor && mPatch > version.mPatch)
            );
}

String Version::convToString( void ) const
{
    constexpr const char * format{ "%d%c%d%c%d" };

    char buffer[ 128 ]{ 0 };
    int len = String::formatString( buffer, 128, format, mMajor, NECommon::OBJECT_SEPARATOR, mMinor, NECommon::OBJECT_SEPARATOR, mPatch );
    return (len > 0 ? String( buffer, len ) : String::getEmptyString());
}

//////////////////////////////////////////////////////////////////////////
// Version friend global operators to make Version object streamable
//////////////////////////////////////////////////////////////////////////
