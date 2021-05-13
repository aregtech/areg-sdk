/************************************************************************
 * \file        areg/base/private/Version.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Version class
 *
 ************************************************************************/

#include "areg/base/Version.hpp"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// Version class implementation
//////////////////////////////////////////////////////////////////////////

const char * const   Version::_VERSION_SEPARATOR  = ".";  //!< The version separator as a string.
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
    ; // do nothing
}

Version::Version( unsigned int major, unsigned int minor, unsigned int patch /*= 0*/ )
    : mMajor    (major)
    , mMinor    (minor)
    , mPatch    (patch)
{
    ; // do nothing
}

Version::Version( const Version &src )
    : mMajor    (src.mMajor)
    , mMinor    (src.mMinor)
    , mPatch    (src.mPatch)
{
    ; // do nothing
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

Version::~Version( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
const Version & Version::convFromString( const char * version )
{
    mMajor  = 0;
    mMinor  = 0;
    mPatch  = 0;

    String temp(version), major, minor, patch;
    NEString::CharPos pos = NEString::StartPos;
    pos = temp.substring( major, _VERSION_SEPARATOR, pos );
    pos = temp.substring( minor, _VERSION_SEPARATOR, pos );
    pos = temp.substring( patch, _VERSION_SEPARATOR, pos );

    mMajor  = major.convToUInt32();;
    mMinor  = minor.convToUInt32();
    mPatch  = (mMajor != 0) && (mMinor != 0) ? patch.convToUInt32() : 0;

    return (*this);
}

const Version & Version::operator = ( const Version &src )
{
    if (this != &src)
    {
        this->mMajor    = src.mMajor;
        this->mMinor    = src.mMinor;
        this->mPatch    = src.mPatch;
    }
    return (*this);
}

const Version & Version::operator = (const char * version)
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

String Version::convToString( void ) const
{
    String result;
    return result.formatString("%d.%d.%d", mMajor, mMinor, mPatch);
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
