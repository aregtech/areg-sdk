/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Version.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Version class
 *
 ************************************************************************/

#include "areg/base/Version.hpp"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/NECommon.hpp"

#include <sstream>

//////////////////////////////////////////////////////////////////////////
// Version class implementation
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Invalid Version object. The invalid version contains following version numbers (0, 0, 0)
 **/
const Version Version::INVALID_VERSION(0, 0, 0);

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
    convFromString(version);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
Version & Version::convFromString( const char * version )
{
    mMajor  = 0;
    mMinor  = 0;
    mPatch  = 0;

    std::string temp = version;

    auto pos1 = temp.find_first_of(NECommon::OBJECT_SEPARATOR);
    if (pos1 != std::string::npos)
    {
        const std::string major = temp.substr(0, pos1);
        mMajor = std::stoi(major);
    }

    auto pos2 = temp.find_last_of(NECommon::OBJECT_SEPARATOR);
    if (pos2 != std::string::npos && pos2 != pos1)
    {
        const std::string minor = temp.substr(pos1 + 1, pos2 - pos1 - 1);
        mMinor = std::stoi(minor);

        const std::string patch = temp.substr(pos2+1);
        mPatch  = (mMajor != 0) && (mMinor != 0) ? std::stoi(patch) : 0;
    }

    return (*this);
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

Version & Version::operator = (const char * version)
{
    return convFromString( version );
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

std::string Version::convToString() const
{
    std::stringstream ss;
    ss << mMajor << NECommon::OBJECT_SEPARATOR << mMinor << NECommon::OBJECT_SEPARATOR << mPatch;
    return ss.str();
}

//////////////////////////////////////////////////////////////////////////
// Version friend global operators to make Version object streamable
//////////////////////////////////////////////////////////////////////////

/**
 * \brief	Streams to input object, i.e. reads data from streaming object to string,
 *          and initialize string data.
 * \param	stream	Streaming object to read string data
 * \param	input	String object to initialize and write string data.
 * \return	Reference to stream object.
 **/
AREG_API const IEInStream & operator >> (const IEInStream & stream, Version& input)
{
    stream >> input.mMajor;
    stream >> input.mMinor;
    stream >> input.mPatch;
    return stream;
}

/**
 * \brief	Streams from output object, i.e. write data from string to streaming object.
 * \param	stream	Streaming object to write data.
 * \param	output	String object to read data from
 * \return	Reference to stream object.
 **/
AREG_API IEOutStream & operator << (IEOutStream& stream, const Version& output)
{
    stream << output.mMajor;
    stream << output.mMinor;
    stream << output.mPatch;
    return stream;
}
