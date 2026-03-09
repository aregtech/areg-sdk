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
    [[nodiscard]]
    static const Version& invalid_version() noexcept;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    Version() = default;

    Version(const Version& src) = default;

    Version(Version&& src) noexcept = default;

    ~Version() = default;

    /**
     * \brief   Initializes the version with major, minor, and optional patch numbers.
     **/
    inline constexpr Version(uint32_t major, uint32_t minor, uint32_t patch = 0) noexcept;

    /**
     * \brief   Initializes the version from a version string.
     **/
    Version( const char * version );

    /**
     * \brief   Initializes the version from a version string.
     **/
    Version( const String & version );

    /**
     * \brief   Initializes the version from a stream.
     **/
    Version(const InStream& stream);

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    Version & operator = ( const Version & src ) noexcept = default;

    Version & operator = ( Version && src ) noexcept = default;

    Version & operator = ( const char * version );

    Version & operator = ( const String & version );

    inline constexpr bool operator == (const Version & version) const noexcept;

    inline constexpr bool operator != (const Version & version) const noexcept;

    /**
     * \brief   Returns true if this version is less than the given version.
     **/
    inline constexpr bool operator < (const Version & version) const noexcept;

    /**
     * \brief   Returns true if this version is greater than the given version.
     **/
    inline constexpr bool operator > (const Version & version) const noexcept;

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
    [[nodiscard]]
    inline constexpr uint32_t major() const noexcept;
    /**
     * \brief   Returns the minor version number.
     **/
    [[nodiscard]]
    inline constexpr uint32_t minor() const noexcept;
    /**
     * \brief   Returns the patch version number.
     **/
    [[nodiscard]]
    inline constexpr uint32_t patch() const noexcept;

    /**
     * \brief   Returns true if the version is valid (not 0.0.0).
     **/
    [[nodiscard]]
    inline constexpr bool is_valid() const noexcept;

    /**
     * \brief   Returns true if this version is compatible with the given version.
     *
     * \param   version     The version to check compatibility against.
     * \return  Returns true if major numbers are equal and this minor version is greater than or
     *          equal to the given minor version; false otherwise.
     **/
    [[nodiscard]]
    inline constexpr bool is_compatible(const Version & version) const noexcept;

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
    Version & from_string( const char * version );
    /**
     * \brief   Parses a version string and initializes the version object.
     *
     * \param   version     The version string in the format "major.minor.patch".
     * \return  Reference to this version.
     **/
    Version & from_string( const String & version );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    uint32_t    mMajor{ 0u }; //!< Major version number
    uint32_t    mMinor{ 0u }; //!< Minor version number
    uint32_t    mPatch{ 0u }; //!< Patching version number
};

//////////////////////////////////////////////////////////////////////////
// Version class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline constexpr Version::Version(uint32_t major, uint32_t minor, uint32_t patch /*= 0*/) noexcept
    : mMajor(major)
    , mMinor(minor)
    , mPatch(patch)
{
}

inline constexpr uint32_t Version::major() const noexcept
{
    return mMajor;
}

inline constexpr uint32_t Version::minor() const noexcept
{
    return  mMinor;
}

inline constexpr uint32_t Version::patch() const noexcept
{
    return mPatch;
}

inline constexpr bool Version::is_valid() const noexcept
{
    return ( mMajor != 0 || mMinor != 0 );
}

inline constexpr bool Version::is_compatible( const Version & version ) const noexcept
{
    return ((mMajor == version.mMajor)  && (mMinor >= version.mMinor));
}

inline Version & Version::operator = ( const char * version )
{
    return from_string(version);
}

inline Version & Version::operator = ( const String & version )
{
    return from_string(version);
}

inline constexpr bool Version::operator == ( const Version &version ) const noexcept
{
   return  (this != &version ? (mMajor == version.mMajor) && (mMinor == version.mMinor) && (mPatch == version.mPatch) : true);
}

inline constexpr bool Version::operator != ( const Version &version ) const noexcept
{
    return  (this != &version ? (mMajor != version.mMajor) || (mMinor != version.mMinor) || (mPatch != version.mPatch) : false);
}

inline constexpr bool Version::operator < ( const Version & version ) const noexcept
{
    return  (this == &version ? false :
                (mMajor < version.mMajor) || 
                (mMajor == version.mMajor && mMinor < version.mMinor) ||
                (mMajor == version.mMajor && mMinor == version.mMajor && mPatch < version.mPatch)
             );
}

inline constexpr bool Version::operator > ( const Version & version ) const noexcept
{
    return  (this == &version ? false :
                (mMajor > version.mMajor) ||
                (mMajor == version.mMajor && mMinor > version.mMinor) ||
                (mMajor == version.mMajor && mMinor == version.mMajor && mPatch > version.mPatch)
            );
}

inline const InStream & operator >> (const InStream & stream, Version& input)
{
    stream >> input.mMajor;
    stream >> input.mMinor;
    stream >> input.mPatch;
    return stream;
}

inline OutStream & operator << (OutStream& stream, const Version& output)
{
    stream << output.mMajor;
    stream << output.mMinor;
    stream << output.mPatch;
    return stream;
}

} // namespace areg
#endif  // AREG_BASE_VERSION_HPP
