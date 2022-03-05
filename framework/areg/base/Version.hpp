#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/Version.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Version class.
 *              This class contains version information such as
 *              Major number, Minor number and Patch number.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include <string>

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;
class IEOutStream;

//////////////////////////////////////////////////////////////////////////
// Version class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The version class contains major, minor, and the patch numbers.
 *          The class is streamable, can be serialized to streaming object 
 *          and can be passed between different threads and processes.
 **/
class AREG_API Version
{
//////////////////////////////////////////////////////////////////////////
// types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Version::INVALID_VERSION
     *          Invalid Version object. The invalid version contains following version numbers (0, 0, 0)
     **/
    static const Version   INVALID_VERSION;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Invalid Version will be created.
     **/
    Version( void );
    /**
     * \brief   Initialization constructor. Major, Minor and Patch
     *          number will be initialized by give values.
     **/
    Version(unsigned int major, unsigned int minor, unsigned int patch = 0);
    /**
     * \brief   Initialization constructor.
     *          Major, Minor and Patch versions are taken from readable string.
     **/
    Version( const char * version );
    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    Version(const Version & src);
    /**
     * \brief   Move constructor.
     * \param   src     The source to move data.
     **/
    Version( Version && src ) noexcept;
    /**
     * \brief   Initialization constructor.
     *          Initializes data from streaming object.
     **/
    Version(const IEInStream & stream);
    /**
     * \brief   Destructor
     **/
    ~Version( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Assigning operator, which copies version information from given source.
     **/
    Version & operator = ( const Version & src );

    /**
     * \brief   Move operator, which moves version information from given source.
     **/
    Version & operator = ( Version && src ) noexcept;

    /**
     * \brief   Assigning operator. Sets version from string
     * \brief   Move operator, which moves version information from given source.
     **/
    Version & operator = ( const char * version );

    /**
     * \brief   Assigning operator. Sets version from string
     **/
    Version & operator = ( const std::string & version );

    /**
     * \brief   Determines equality of two versions.
     * \remark  This operator compares exact match of version number.
     *          For compatibility check, have a look appropriate IsCompatible method.
     * \param   other   The second version to compare
     * \return  Returns true if 2 versions are equal.
     * \see     IsCompatible
     **/
    inline bool operator == (const Version & version) const;

    /**
     * \brief   Determines inequality of two versions.
     * \remark  This operator compares exact match of version number.
     *          For compatibility check, have a look appropriate IsCompatible method.
     * \param   other   The second version to compare
     * \return  Returns true if 2 versions are unequal.
     * \see     IsCompatible
     **/
    inline bool operator != (const Version & version) const;

    /**
     * \brief   Determines whether existing version is smaller.
     * \remark  This operator compares exact match of version number.
     *          For compatibility check, have a look appropriate IsCompatible method.
     * \param   other   The second version to compare
     * \return  Returns true if existing version is smaller than given.
     * \see     IsCompatible
     **/
    bool operator < (const Version & version) const;

    /**
     * \brief   Determines whether existing version is greater.
     * \remark  This operator compares exact match of version number.
     *          For compatibility check, have a look appropriate IsCompatible method.
     * \param   other   The second version to compare
     * \return  Returns true if existing version is greater than given.
     * \see     IsCompatible
     **/
    bool operator > (const Version & version) const;

/************************************************************************/
// Friend global operators to make Version object streamable
/************************************************************************/

    /**
     * \brief	Streams to input object, i.e. reads data from streaming object to version.
     * \param	stream	Streaming object to read data
     * \param	input	Version object to write version data.
     * \return	Reference to stream object.
     **/
    friend AREG_API const IEInStream & operator >> (const IEInStream & stream, Version & input);

    /**
     * \brief	Streams to input object, i.e. write data from version to streaming object.
     * \param	stream	Streaming object to write data
     * \param	input	Version object to read version data.
     * \return	Reference to stream object.
     **/
    friend AREG_API IEOutStream & operator << (IEOutStream & stream, const Version & output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Return Major number of version.
     **/
    inline unsigned int getMajor( void ) const;
    /**
     * \brief   Returns Minor number of version
     **/
    inline unsigned int getMinor( void ) const;
    /**
     * \brief   Returns Patch number of version
     **/
    inline unsigned int getPatch( void ) const;

    /**
     * \brief   Returns true, if version object is not invalid.
     *          The invalid version is equal to INVALID_VERSION.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Returns true, if passed version object is 
     *          compatible with existing version.
     *          2 versions are compatible if Major numbers
     *          are equal and the existing minor number is
     *          more or equal to minor number of passed version.
     * \param   version The version number to check compatibility.
     **/
    inline bool isCompatible(const Version & version) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts version object to string in format
     *          "major.minor.patch", and returns string.
     **/
    std::string convToString() const;

    /**
     * \brief   Retrieves version information from given string
     *          The version information should be in following
     *          format: "major.minor.patch"
     **/
    Version & convFromString( const char * version );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Major version number
     **/
    unsigned int    mMajor;
    /**
     * \brief   Minor version number
     **/
    unsigned int    mMinor;
    /**
     * \brief   Patching version number
     **/
    unsigned int    mPatch;
};

//////////////////////////////////////////////////////////////////////////
// Version class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline unsigned int Version::getMajor( void ) const
{
    return mMajor;
}

inline unsigned int Version::getMinor( void ) const
{
    return  mMinor;
}

inline unsigned int Version::getPatch( void ) const
{
    return mPatch;
}

inline bool Version::isValid( void ) const
{
    return ( mMajor != 0 || mMinor != 0 );
}

inline bool Version::isCompatible( const Version & version ) const
{
    return ((mMajor == version.mMajor)  && (mMinor >= version.mMinor));
}

inline Version & Version::operator = ( const std::string & version )
{
    return (*this) = version.c_str();
}

inline bool Version::operator == ( const Version &version ) const
{
   return  (this != &version ? (mMajor == version.mMajor) && (mMinor == version.mMinor) && (mPatch == version.mPatch) : true);
}

inline bool Version::operator != ( const Version &version ) const
{
    return  (this != &version ? (mMajor != version.mMajor) || (mMinor != version.mMinor) || (mPatch != version.mPatch) : false);
}
