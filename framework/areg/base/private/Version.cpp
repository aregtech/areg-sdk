/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Version.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Version class
 *
 ************************************************************************/

#include "areg/base/Version.hpp"

#include "areg/base/UtilityDefs.hpp"
#include "areg/base/CommonDefs.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// Version class implementation
//////////////////////////////////////////////////////////////////////////

const Version & Version::invalid_version()
{
    static const Version _invalidVersion( 0, 0, 0 );
    return _invalidVersion;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
Version::Version()
    : mMajor    (0)
    , mMinor    (0)
    , mPatch    (0)
{
}

Version::Version( uint32_t major, uint32_t minor, uint32_t patch /*= 0*/ )
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

Version::Version(const InStream & stream)
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
    from_string( version );
}

Version::Version(const String & version)
    : mMajor    (0)
    , mMinor    (0)
    , mPatch    (0)
{
    from_string(version);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
Version & Version::from_string( const String & version )
{
    mMajor  = 0;
    mMinor  = 0;
    mPatch  = 0;

    String major, minor, patch;
    areg::CharPos pos = areg::START_POS;
    pos = version.substring( major, areg::OBJECT_SEPARATOR, pos);
    pos = version.substring( minor, areg::OBJECT_SEPARATOR, pos);
    version.substring( patch, areg::OBJECT_SEPARATOR, pos);

    mMajor  = major.to_uint32();
    mMinor  = minor.to_uint32();
    mPatch  = (mMajor != 0) && (mMinor != 0) ? patch.to_uint32() : 0;

    return (*this);
}

Version & Version::from_string( const char * version )
{
	return from_string( String(version != nullptr ? version : areg::EmptyStringA) );
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

String Version::to_string() const
{
    constexpr const char * format{ "%d%c%d%c%d" };

    char buffer[ 128 ]{ 0 };
    int32_t len = String::format_string( buffer, 128, format, mMajor, areg::OBJECT_SEPARATOR, mMinor, areg::OBJECT_SEPARATOR, mPatch );
    return (len > 0 ? String( buffer, static_cast<uint32_t>(len) ) : String::empty_string());
}

} // namespace areg
