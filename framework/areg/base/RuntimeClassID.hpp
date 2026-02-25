#ifndef AREG_BASE_RUNTIMECLASSID_HPP
#define AREG_BASE_RUNTIMECLASSID_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/RuntimeClassID.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Runtime Class ID
 *              This class contains information of Runtime Class ID.
 *              Every Runtime Class contains class ID value to identify and
 *              cast instances of Runtime Object.
 *              As an ID of Runtime class used class name.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/String.hpp"
#include <utility>

//////////////////////////////////////////////////////////////////////////
// RuntimeClassID class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Runtime Class ID is used in all objects derived from Runtime Objects. It contains the
 *          class ID to check and cast instances at runtime, particularly in Threads, Events, and
 *          other objects.
 **/
class AREG_API RuntimeClassID
{
/************************************************************************
 * friend classes to access default constructor.
 ************************************************************************/
    /**
     * \brief   Declare friend classes to access private default constructor
     *          required to initialize runtime class ID in hash map blocks.
     **/
    template < typename KEY, typename VALUE >
    friend class HashMap;
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Implement>
    friend class ResourceMapBase;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty Runtime Class ID object with default value BAD_CLASS_ID.
     **/
    static inline RuntimeClassID empty_id();

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the Runtime Class ID from a null-terminated class name string.
     *
     * \param   className       The null-terminated class name string.
     **/
    explicit RuntimeClassID( const char * className );
    /**
     * \brief   Initializes the Runtime Class ID from a String object containing the class name.
     *
     * \param   className       The String object containing the class name.
     **/
    explicit RuntimeClassID( const String& className );

    /**
     * \brief
     **/
    RuntimeClassID( const RuntimeClassID & src );

    /**
     * \brief
     * \note    Move overload.
     **/
    RuntimeClassID( RuntimeClassID && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~RuntimeClassID() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// friend global operators
/************************************************************************/

    /**
     * \brief   Compares a null-terminated string with a Runtime Class ID name.
     *
     * \param   lhs     The null-terminated string to compare.
     * \param   rhs     The Runtime Class ID object to compare.
     * \return  Returns true if the string is equal to the Runtime Class ID name.
     **/
    friend inline bool operator == ( const char * lhs, const RuntimeClassID & rhs );
    /**
     * \brief   Compares a String object with a Runtime Class ID name.
     *
     * \param   lhs     The String object to compare.
     * \param   rhs     The Runtime Class ID object to compare.
     * \return  Returns true if the String is equal to the Runtime Class ID name.
     **/
    friend inline bool operator == ( const String& lhs, const RuntimeClassID & rhs );

    /**
     * \brief   Compares a null-terminated string with a Runtime Class ID name for inequality.
     *
     * \param   lhs     The null-terminated string to compare.
     * \param   rhs     The Runtime Class ID object to compare.
     * \return  Returns true if the string is not equal to the Runtime Class ID name.
     **/
    friend inline bool operator != ( const char * lhs, const RuntimeClassID & rhs );
    /**
     * \brief   Compares a String object with a Runtime Class ID name for inequality.
     *
     * \param   lhs     The String object to compare.
     * \param   rhs     The Runtime Class ID object to compare.
     * \return  Returns true if the String is not equal to the Runtime Class ID name.
     **/
    friend inline bool operator != ( const String& lhs, const RuntimeClassID & rhs );

    /**
     * \brief   Compares a numeric value with the calculated hash value of a Runtime Class ID.
     *
     * \param   lhs     The numeric value to compare.
     * \param   rhs     The Runtime Class ID object to compare.
     * \return  Returns true if the numeric value is equal to the Runtime Class ID hash.
     **/
    friend inline bool operator == ( uint32_t lhs, const RuntimeClassID & rhs );

    /**
     * \brief   Compares a numeric value with the calculated hash value of a Runtime Class ID for
     *          inequality.
     *
     * \param   lhs     The numeric value to compare.
     * \param   rhs     The Runtime Class ID object to compare.
     * \return  Returns true if the numeric value is not equal to the Runtime Class ID hash.
     **/
    friend inline bool operator != ( uint32_t lhs, const RuntimeClassID & rhs );

/************************************************************************/
// class members operators
/************************************************************************/

    /**
     * \brief   Copies the Runtime Class ID name from the given source object.
     *
     * \param   src     The source Runtime Class ID object to copy from.
     * \return  Returns a reference to this Runtime Class ID object.
     **/
    inline RuntimeClassID & operator = ( const RuntimeClassID & src );

    /**
     * \brief   Moves the Runtime Class ID name from the given source object.
     *
     * \param   src     The source Runtime Class ID object to move from.
     * \return  Returns a reference to this Runtime Class ID object.
     **/
    inline RuntimeClassID & operator = ( RuntimeClassID && src ) noexcept;

    /**
     * \brief   Copies the Runtime Class ID name from a null-terminated string.
     *
     * \param   src     The null-terminated string to copy from.
     * \return  Returns a reference to this Runtime Class ID object.
     **/
    inline RuntimeClassID & operator = ( const char * src );
    /**
     * \brief   Copies the Runtime Class ID name from a String object.
     *
     * \param   src     The String object to copy from.
     * \return  Returns a reference to this Runtime Class ID object.
     **/
    inline RuntimeClassID & operator = ( const String& src );

    /**
     * \brief   Compares two Runtime Class ID objects for equality.
     *
     * \param   other       The other Runtime Class ID object to compare.
     * \return  Returns true if both objects have the same Class ID value.
     **/
    inline bool operator == ( const RuntimeClassID & other ) const;
    /**
     * \brief   Compares this Runtime Class ID with a null-terminated string.
     *
     * \param   other       The null-terminated string to compare.
     * \return  Returns true if the Runtime Class ID name equals the string.
     **/
    inline bool operator == ( const char * other ) const;
    /**
     * \brief   Compares this Runtime Class ID with a String object.
     *
     * \param   other       The String object to compare.
     * \return  Returns true if the Runtime Class ID name equals the String.
     **/
    inline bool operator == ( const String& other ) const;
    /**
     * \brief   Compares two Runtime Class ID objects for inequality.
     *
     * \param   other       The other Runtime Class ID object to compare.
     * \return  Returns true if both objects have different Class ID values.
     **/
    inline bool operator != ( const RuntimeClassID & other ) const;
    /**
     * \brief   Compares this Runtime Class ID with a null-terminated string for inequality.
     *
     * \param   other       The null-terminated string to compare.
     * \return  Returns true if the Runtime Class ID name does not equal the string.
     **/
    inline bool operator != (const char * other) const;
    /**
     * \brief   Compares this Runtime Class ID with a String object for inequality.
     *
     * \param   other       The String object to compare.
     * \return  Returns true if the Runtime Class ID name does not equal the String.
     **/
    inline bool operator != (const String& other) const;

    /**
     * \brief   Converts the Runtime Class ID to its calculated hash value for use in hash maps.
     **/
    inline explicit operator uint32_t () const;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns true if the Runtime Class ID value is not equal to BAD_CLASS_ID.
     **/
    inline bool is_valid() const;

    /**
     * \brief   Returns the class name of this Runtime Class ID.
     **/
    inline const String& name() const;

    /**
     * \brief   Sets the class name from a null-terminated string.
     *
     * \param   className       The null-terminated class name string.
     **/
    void set_name( const char* className );
    /**
     * \brief   Sets the class name from a String object.
     *
     * \param   className       The String object containing the class name.
     **/
    void set_name( const String& className );

    /**
     * \brief   Returns the calculated hash value of the runtime class.
     **/
    inline unsigned magic() const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor. Creates a BAD_CLASS_ID object.
     **/
    RuntimeClassID();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Runtime Class ID value.
     **/
    String          mClassName;
    /**
     * \brief   The calculated number of runtime class.
     **/
    uint32_t    mMagicNum;
};

//////////////////////////////////////////////////////////////////////////
// Hasher of RuntimeClassID class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the RuntimeClassID.
 */
namespace std
{
    template<>
    struct hash<RuntimeClassID>
    {
        //! A function to convert String object to uint32_t.
        inline uint32_t operator()(const RuntimeClassID& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// RuntimeClassID class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline RuntimeClassID RuntimeClassID::empty_id()
{
    return RuntimeClassID();
}

inline RuntimeClassID & RuntimeClassID::operator = ( const RuntimeClassID & src )
{
    this->mClassName= src.mClassName;
    this->mMagicNum = src.mMagicNum;

    return (*this);
}

inline RuntimeClassID & RuntimeClassID::operator = ( RuntimeClassID && src ) noexcept
{
    this->mClassName= std::move(src.mClassName);
    this->mMagicNum = src.mMagicNum;

    return (*this);
}

inline RuntimeClassID & RuntimeClassID::operator = ( const char * src )
{
    set_name(src);
    return (*this);
}

inline RuntimeClassID & RuntimeClassID::operator = ( const String & src )
{
    set_name(src);
    return (*this);
}

inline bool RuntimeClassID::operator == ( const RuntimeClassID & other ) const
{
    return (mMagicNum == other.mMagicNum);
}

inline bool RuntimeClassID::operator == ( const char * other ) const
{
    return mClassName == other;
}

inline bool RuntimeClassID::operator == (const String& other) const
{
    return mClassName == other;
}

inline bool RuntimeClassID::operator != ( const RuntimeClassID  & other ) const
{
    return (mMagicNum != other.mMagicNum);
}

inline bool RuntimeClassID::operator != ( const char* other ) const
{
    return mClassName != other;
}

inline bool RuntimeClassID::operator != (const String & other) const
{
    return mClassName != other;
}

inline RuntimeClassID::operator uint32_t () const
{
    return mMagicNum;
}

inline bool RuntimeClassID::is_valid() const
{
    return (mMagicNum != NEMath::CHECKSUM_IGNORE);
}

inline const String & RuntimeClassID::name() const
{
    return mClassName;
}

inline unsigned RuntimeClassID::magic() const
{
    return mMagicNum;
}

inline bool operator == ( const char * lhs, const RuntimeClassID & rhs )
{
    return rhs.mClassName   == lhs;
}

inline bool operator == ( const String & lhs, const RuntimeClassID & rhs )
{
    return rhs.mClassName   == lhs;
}

inline bool operator != ( const char* lhs, const RuntimeClassID & rhs )
{
    return rhs.mClassName != lhs;
}

inline bool operator != ( const String & lhs, const RuntimeClassID & rhs )
{
    return rhs.mClassName != lhs;
}

inline bool operator == ( uint32_t lhs, const RuntimeClassID & rhs )
{
    return rhs.mMagicNum  == lhs;
}

inline bool operator != ( uint32_t lhs, const RuntimeClassID & rhs )
{
    return rhs.mMagicNum != lhs;
}

#endif  // AREG_BASE_RUNTIMECLASSID_HPP
