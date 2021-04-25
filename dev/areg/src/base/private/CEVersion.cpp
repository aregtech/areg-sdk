/************************************************************************
 * \file        areg/src/base/private/CEVersion.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Version class
 *
 ************************************************************************/

#include "areg/src/base/CEVersion.hpp"

#include "areg/src/base/IEIOStream.hpp"
#include "areg/src/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// CEVersion class implementation
//////////////////////////////////////////////////////////////////////////

const char * const   CEVersion::_VERSION_SEPARATOR  = ".";  //!< The version separator as a string.
/**
 * \brief   Invalid Version object. The invalid version contains following version numbers (0, 0, 0)
 **/
const CEVersion CEVersion::INVALID_VERSION(0, 0, 0);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CEVersion::CEVersion( void )
    : mMajor    (0)
    , mMinor    (0)
    , mPatch    (0)
{
    ; // do nothing
}

CEVersion::CEVersion( unsigned int major, unsigned int minor, unsigned int patch /*= 0*/ )
    : mMajor    (major)
    , mMinor    (minor)
    , mPatch    (patch)
{
    ; // do nothing
}

CEVersion::CEVersion( const CEVersion &src )
    : mMajor    (src.mMajor)
    , mMinor    (src.mMinor)
    , mPatch    (src.mPatch)
{
    ; // do nothing
}

CEVersion::CEVersion(const IEInStream & stream)
    : mMajor    ( 0 )
    , mMinor    ( 0 )
    , mPatch    ( 0 )
{
    stream >> mMajor;
    stream >> mMinor;
    stream >> mPatch;
}

CEVersion::CEVersion(const char * version)
    : mMajor    (0)
    , mMinor    (0)
    , mPatch    (0)
{
    FromString(version);
}

CEVersion::~CEVersion( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
const CEVersion & CEVersion::FromString( const char * version )
{
    mMajor  = 0;
    mMinor  = 0;
    mPatch  = 0;

    CEString temp(version), major, minor, patch;
    NEString::CharPos pos = NEString::StartPos;
    pos = temp.Substr( major, _VERSION_SEPARATOR, pos );
    pos = temp.Substr( minor, _VERSION_SEPARATOR, pos );
    pos = temp.Substr( patch, _VERSION_SEPARATOR, pos );

    mMajor  = major.ToUInt32();;
    mMinor  = minor.ToUInt32();
    mPatch  = (mMajor != 0) && (mMinor != 0) ? patch.ToUInt32() : 0;

    return (*this);
}

const CEVersion & CEVersion::operator = ( const CEVersion &src )
{
    if (this != &src)
    {
        this->mMajor    = src.mMajor;
        this->mMinor    = src.mMinor;
        this->mPatch    = src.mPatch;
    }
    return (*this);
}

const CEVersion & CEVersion::operator = (const char * version)
{
    return FromString( version );
}

bool CEVersion::operator < ( const CEVersion & version ) const
{
    return  (this == &version ? false :
                (mMajor < version.mMajor) || 
                (mMajor == version.mMajor && mMinor < version.mMinor) ||
                (mMajor == version.mMajor && mMinor == version.mMajor && mPatch < version.mPatch)
             );
}

bool CEVersion::operator > ( const CEVersion & version ) const
{
    return  (this == &version ? false :
                (mMajor > version.mMajor) ||
                (mMajor == version.mMajor && mMinor > version.mMinor) ||
                (mMajor == version.mMajor && mMinor == version.mMajor && mPatch > version.mPatch)
            );
}

CEString CEVersion::ToString( void ) const
{
    CEString result;
    return result.Format("%d.%d.%d", mMajor, mMinor, mPatch);
}

//////////////////////////////////////////////////////////////////////////
// CEVersion friend global operators to make CEVersion object streamable
//////////////////////////////////////////////////////////////////////////

/**
 * \brief	Streams to input object, i.e. reads data from streaming object to string,
 *          and initialize string data.
 * \param	stream	Streaming object to read string data
 * \param	input	String object to initialize and write string data.
 * \return	Reference to stream object.
 **/
AREG_API const IEInStream & operator >> (const IEInStream & stream, CEVersion& input)
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
AREG_API IEOutStream & operator << (IEOutStream& stream, const CEVersion& output)
{
    stream << output.mMajor;
    stream << output.mMinor;
    stream << output.mPatch;
    return stream;
}
