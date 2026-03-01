#ifndef AREG_BASE_VERSION_HPP
#define AREG_BASE_VERSION_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/Version.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Version class.
 *              This class contains version information such as
 *              Major number, Minor number and Patch number.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/String.hpp"
#include "areg/base/IOStream.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class InStream;
class OutStream;

//////////////////////////////////////////////////////////////////////////
// Version class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Represents a version with major, minor, and patch numbers; supports serialization and
 *          comparison.
 **/
class AREG_API Version
{
//////////////////////////////////////////////////////////////////////////
// types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns a static invalid version (0.0.0).
     **/
    static const Version & invalid_version();

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an invalid version (0.0.0).
     **/
    Version();
    /**
     * \brief   Initializes the version with major, minor, and optional patch numbers.
     *
     * \param   major       The major version number.
     * \param   minor       The minor version number.
     * \param   patch       The patch version number (default 0).
     **/
    Version(uint32_t major, uint32_t minor, uint32_t patch = 0);
    /**
     * \brief   Initializes the version from a version string.
     *
     * \param   version     The version string in the format "major.minor.patch".
     **/
    Version( const char * version );
    /**
     * \brief   Initializes the version from a version string.
     *
     * \param   version     The version string in the format "major.minor.patch".
     **/
    Version( const String & version );
    /**
     * \brief
     *
     * \param   src     The source version to copy.
     * \note    Copy constructor.
     **/
    Version(const Version & src);
    /**
     * \brief
     *
     * \param   src     The source version to move.
     * \note    Move constructor.
     **/
    Version( Version && src ) noexcept;
    /**
     * \brief   Initializes the version by reading from a stream.
     *
     * \param   stream      The input stream to read from.
     **/
    Version(const InStream & stream);
    /**
     * \brief   Destructor
     **/
    ~Version() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Assigns version data from the source.
     *
     * \param   src     The source version to copy from.
     * \return  Reference to this version.
     **/
    Version & operator = ( const Version & src );

    /**
     * \brief   Moves version data from the source.
     *
     * \param   src     The source version to move from.
     * \return  Reference to this version.
     **/
    Version & operator = ( Version && src ) noexcept;

    /**
     * \brief   Assigns the version from a string.
     *
     * \param   version     The version string in the format "major.minor.patch".
     * \return  Reference to this version.
     **/
    Version & operator = ( const char * version );
    /**
     * \brief   Assigns the version from a string.
     *
     * \param   version     The version string in the format "major.minor.patch".
     * \return  Reference to this version.
     **/
    Version & operator = ( const String & version );

    /**
     * \brief   Returns true if two versions are equal.
     *
     * \param   version     The version to compare.
     * \return  Returns true if both versions are equal; false otherwise.
     **/
    inline bool operator == (const Version & version) const;

    /**
     * \brief   Returns true if two versions are not equal.
     *
     * \param   version     The version to compare.
     * \return  Returns true if the versions are not equal; false otherwise.
     **/
    inline bool operator != (const Version & version) const;

    /**
     * \brief   Returns true if this version is less than the given version.
     *
     * \param   version     The version to compare.
     * \return  Returns true if this version is less than the given version; false otherwise.
     **/
    bool operator < (const Version & version) const;

    /**
     * \brief   Returns true if this version is greater than the given version.
     *
     * \param   version     The version to compare.
     * \return  Returns true if this version is greater than the given version; false otherwise.
     **/
    bool operator > (const Version & version) const;

/************************************************************************/
// Friend global operators to make Version object streamable
/************************************************************************/

    /**
     * \brief   Reads version data from a stream.
     *
     * \param   stream      The input stream to read from.
     * \param[out] input       The version object to write the data into.
     * \return  Reference to the input stream.
     **/
    friend inline const InStream & operator >> (const InStream & stream, Version & input);

    /**
     * \brief   Writes version data to a stream.
     *
     * \param   stream      The output stream to write to.
     * \param   output      The version object to read the data from.
     * \return  Reference to the output stream.
     **/
    friend inline OutStream & operator << (OutStream & stream, const Version & output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the major version number.
     **/
    inline uint32_t major() const;
    /**
     * \brief   Returns the minor version number.
     **/
    inline uint32_t minor() const;
    /**
     * \brief   Returns the patch version number.
     **/
    inline uint32_t patch() const;

    /**
     * \brief   Returns true if the version is valid (not 0.0.0).
     **/
    inline bool is_valid() const;

    /**
     * \brief   Returns true if this version is compatible with the given version.
     *
     * \param   version     The version to check compatibility against.
     * \return  Returns true if major numbers are equal and this minor version is greater than or
     *          equal to the given minor version; false otherwise.
     **/
    inline bool is_compatible(const Version & version) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts the version to a string in the format "major.minor.patch".
     **/
    String to_string() const;

    /**
     * \brief   Parses a version string and initializes the version object.
     *
     * \param   version     The version string in the format "major.minor.patch".
     * \return  Reference to this version.
     **/
    Version & conv_from_string( const char * version );
    /**
     * \brief   Parses a version string and initializes the version object.
     *
     * \param   version     The version string in the format "major.minor.patch".
     * \return  Reference to this version.
     **/
    Version & conv_from_string( const String & version );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    uint32_t    mMajor; //!< Major version number
    uint32_t    mMinor; //!< Minor version number
    uint32_t    mPatch; //!< Patching version number
};

//////////////////////////////////////////////////////////////////////////
// Version class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline uint32_t Version::major() const
{
    return mMajor;
}

inline uint32_t Version::minor() const
{
    return  mMinor;
}

inline uint32_t Version::patch() const
{
    return mPatch;
}

inline bool Version::is_valid() const
{
    return ( mMajor != 0 || mMinor != 0 );
}

inline bool Version::is_compatible( const Version & version ) const
{
    return ((mMajor == version.mMajor)  && (mMinor >= version.mMinor));
}

inline Version & Version::operator = ( const char * version )
{
    return conv_from_string(version);
}

inline Version & Version::operator = ( const String & version )
{
    return conv_from_string(version);
}

inline bool Version::operator == ( const Version &version ) const
{
   return  (this != &version ? (mMajor == version.mMajor) && (mMinor == version.mMinor) && (mPatch == version.mPatch) : true);
}

inline bool Version::operator != ( const Version &version ) const
{
    return  (this != &version ? (mMajor != version.mMajor) || (mMinor != version.mMinor) || (mPatch != version.mPatch) : false);
}

/**
 * \brief	Streams to input object, i.e. reads data from streaming object to string,
 *          and initialize string data.
 * \param	stream	Streaming object to read string data
 * \param	input	String object to initialize and write string data.
 * \return	Reference to stream object.
 **/
inline const InStream & operator >> (const InStream & stream, Version& input)
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
inline OutStream & operator << (OutStream& stream, const Version& output)
{
    stream << output.mMajor;
    stream << output.mMinor;
    stream << output.mPatch;
    return stream;
}

} // namespace areg
#endif  // AREG_BASE_VERSION_HPP
