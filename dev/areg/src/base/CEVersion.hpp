#ifndef AREG_BASE_CEVERSION_HPP
#define AREG_BASE_CEVERSION_HPP
/************************************************************************
 * \file        areg/src/base/CEVersion.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Version class.
 *              This class contains version information such as
 *              Major number, Minor number and Patch number.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/CEString.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;
class IEOutStream;

/**
 * \brief       The version class contains following information as
 *              numeric values: Major number, Minor number and Patch number.
 *              Class is streamable, can be serialized to streaming object 
 *              and can be passed between different threads / processes.
 *              Basic operations are supported by version class.
 * 
 **/
//////////////////////////////////////////////////////////////////////////
// CEVersion class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CEVersion
{
//////////////////////////////////////////////////////////////////////////
// Internals and constants
//////////////////////////////////////////////////////////////////////////
private:
    static const char * const   _VERSION_SEPARATOR  /*= "."*/;  //!< The version separator as a string.

public:
    /**
     * \brief   CEVersion::INVALID_VERSION
     *          Invalid Version object. The invalid version contains following version numbers (0, 0, 0)
     **/
    static const CEVersion   INVALID_VERSION;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Invalid Version will be created.
     **/
    CEVersion( void );
    /**
     * \brief   Initialization constructor. Major, Minor and Patch
     *          number will be initialized by give values.
     **/
    CEVersion(unsigned int major, unsigned int minor, unsigned int patch = 0);
    /**
     * \brief   Initialization constructor.
     *          Major, Minor and Patch versions are taken from readable string.
     **/
    CEVersion( const char * version );
    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CEVersion(const CEVersion & src);
    /**
     * \brief   Initialization constructor.
     *          Initializes data from streaming object.
     **/
    CEVersion(const IEInStream & stream);
    /**
     * \brief   Destructor
     **/
    ~CEVersion( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Return Major number of version.
     **/
    inline unsigned int GetMajor( void ) const;
    /**
     * \brief   Returns Minor number of version
     **/
    inline unsigned int GetMinor( void ) const;
    /**
     * \brief   Returns Patch number of version
     **/
    inline unsigned int GetPatch( void ) const;

    /**
     * \brief   Returns true, if version object is not invalid.
     *          The invalid version is equal to INVALID_VERSION.
     **/
    inline bool IsValid( void ) const;

    /**
     * \brief   Returns true, if passed version object is 
     *          compatible with existing version.
     *          2 versions are compatible if Major numbers
     *          are equal and the existing minor number is
     *          more or equal to minor number of passed version.
     * \param   version The version number to check compatibility.
     **/
    inline bool IsCompatible(const CEVersion& version) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts version object to string in format
     *          "major.minor.patch", and returns string.
     **/
    CEString ToString( void ) const;

    /**
     * \brief   Retrieves version information from given string
     *          The version information should be in following
     *          format: "major.minor.patch"
     **/
    const CEVersion & FromString( const char * version );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigning operator. Copies version information
     *          from given source.
     **/
    const CEVersion & operator = ( const CEVersion & src );

    /**
     * \brief   Assigning operator. Sets version from string
     **/
    const CEVersion & operator = ( const char * version );

    /**
     * \brief   Assigning operator. Sets version from string
     **/
    const CEVersion & operator = ( const CEString & version );

    /**
     * \brief   Determines equality of two versions.
     * \remark  This operator compares exact match of version number.
     *          For compatibility check, have a look appropriate IsCompatible method.
     * \param   other   The second version to compare
     * \return  Returns true if 2 versions are equal.
     * \see     IsCompatible
     **/
    inline bool operator == (const CEVersion & version) const;

    /**
     * \brief   Determines inequality of two versions.
     * \remark  This operator compares exact match of version number.
     *          For compatibility check, have a look appropriate IsCompatible method.
     * \param   other   The second version to compare
     * \return  Returns true if 2 versions are unequal.
     * \see     IsCompatible
     **/
    inline bool operator != (const CEVersion & version) const;

    /**
     * \brief   Determines whether existing version is smaller.
     * \remark  This operator compares exact match of version number.
     *          For compatibility check, have a look appropriate IsCompatible method.
     * \param   other   The second version to compare
     * \return  Returns true if existing version is smaller than given.
     * \see     IsCompatible
     **/
    bool operator < (const CEVersion & version) const;

    /**
     * \brief   Determines whether existing version is greater.
     * \remark  This operator compares exact match of version number.
     *          For compatibility check, have a look appropriate IsCompatible method.
     * \param   other   The second version to compare
     * \return  Returns true if existing version is greater than given.
     * \see     IsCompatible
     **/
    bool operator > (const CEVersion & version) const;

/************************************************************************/
// Friend global operators to make CEVersion object streamable
/************************************************************************/

    /**
     * \brief	Streams to input object, i.e. reads data from streaming object to version.
     * \param	stream	Streaming object to read data
     * \param	input	Version object to write version data.
     * \return	Reference to stream object.
     **/
    friend AREG_API const IEInStream & operator >> (const IEInStream & stream, CEVersion & input);

    /**
     * \brief	Streams to input object, i.e. write data from version to streaming object.
     * \param	stream	Streaming object to write data
     * \param	input	Version object to read version data.
     * \return	Reference to stream object.
     **/
    friend AREG_API IEOutStream & operator << (IEOutStream & stream, const CEVersion & output);

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
// CEVersion class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline unsigned int CEVersion::GetMajor( void ) const
{   return mMajor;                                                                                                          }

inline unsigned int CEVersion::GetMinor( void ) const
{   return  mMinor;                                                                                                         }

inline unsigned int CEVersion::GetPatch( void ) const
{   return mPatch;                                                                                                          }

inline bool CEVersion::IsValid( void ) const
{   return ( mMajor != 0 || mMinor != 0 );                                                                                  }

inline bool CEVersion::IsCompatible( const CEVersion & version ) const
{   return ((mMajor == version.mMajor)  && (mMinor >= version.mMinor));                                                     }

inline const CEVersion & CEVersion::operator = ( const CEString & version )
{   return (*this) = version.GetBuffer();                                                                                   }

inline bool CEVersion::operator == ( const CEVersion &version ) const
{   return  (this != &version ? mMajor == version.mMajor && mMinor == version.mMinor && mPatch == version.mPatch : true);   }

inline bool CEVersion::operator != ( const CEVersion &version ) const
{   return  (this != &version ? mMajor != version.mMajor || mMinor != version.mMinor || mPatch != version.mPatch : false);  }

#endif  // AREG_BASE_CEVERSION_HPP
