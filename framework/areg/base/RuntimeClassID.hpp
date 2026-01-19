#ifndef AREG_BASE_RUNTIMECLASSID_HPP
#define AREG_BASE_RUNTIMECLASSID_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/RuntimeClassID.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Runtime Class ID
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
 * \brief   Runtime Class ID is used in all objects derived from 
 *          Runtime Objects. It contains the class ID, so that during
 *          runtime can check and cast the instance of Runtime Object.
 *          In particular, Runtime Object and Runtime Class ID are used 
 *          in Threads,  Events and other objects to check the instance 
 *          of object during runtime.
 *
 * \see RuntimeObject
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
    friend class TEHashMap;
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
    friend class TEResourceMap;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Static function to create empty Runtime Class ID object.
     *          By default, the class ID value is BAD_CLASS_ID.
     *          Change the value after creating class ID object.
     **/
    static inline RuntimeClassID createEmptyClassID( void );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor.
     *          This constructor is initializing the name Runtime Class ID
     * \param   className   The name of Runtime Class ID
     **/
    explicit RuntimeClassID( const char * className );
    explicit RuntimeClassID( const String& className );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    RuntimeClassID( const RuntimeClassID & src );

    /**
     * \brief   Move constructor.
     * \param   src     The source to move data.
     **/
    RuntimeClassID( RuntimeClassID && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~RuntimeClassID( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// friend global operators
/************************************************************************/

    /**
     * \brief   Compares null-terminated string with Runtime Class ID name.
     * \param   lhs     Left-Hand Operand, string to compare.
     * \param   rhs     Right-Hand Operand, Runtime Class ID to compare the name.
     * \return  Returns true if string is equal to Runtime Class ID.
     **/
    friend inline bool operator == ( const char * lhs, const RuntimeClassID & rhs );
    friend inline bool operator == ( const String& lhs, const RuntimeClassID & rhs );

    /**
     * \brief   Compare null-terminated string with Runtime Class ID name.
     * \param   lhs     Left-Hand Operand, string to compare.
     * \param   rhs     Right-Hand Operand, Runtime Class to compare the name.
     * \return  Returns true if string is not equal to Runtime Class ID.
     **/
    friend inline bool operator != ( const char * lhs, const RuntimeClassID & rhs );
    friend inline bool operator != ( const String& lhs, const RuntimeClassID & rhs );

    /**
     * \brief   Compares number with Runtime Class ID calculated number.
     * \param   lhs     Left-Hand Operand, number to compare.
     * \param   rhs     Right-Hand Operand, Runtime Class ID to compare the calculated number.
     * \return  Returns true if number is equal to Runtime Class ID.
     **/
    friend inline bool operator == ( unsigned int lhs, const RuntimeClassID & rhs );

    /**
     * \brief   Compares number with Runtime Class ID calculated number.
     * \param   lhs     Left-Hand Operand, number to compare.
     * \param   rhs     Right-Hand Operand, Runtime Class ID to compare the calculated number.
     * \return  Returns true if number is not equal to Runtime Class ID.
     **/
    friend inline bool operator != ( unsigned int lhs, const RuntimeClassID & rhs );

/************************************************************************/
// class members operators
/************************************************************************/

    /**
     * \brief   Assigning operator. Copies Runtime Class ID name from given Runtime Class ID source
     * \param   src     The source of Runtime Class ID to copy.
     * \return  Returns Runtime Class ID object.
     **/
    inline RuntimeClassID & operator = ( const RuntimeClassID & src );

    /**
     * \brief   Move operator. Moves Runtime Class ID name from given Runtime Class ID source.
     * \param   src     The source of Runtime Class ID to move.
     * \return  Returns Runtime Class ID object.
     **/
    inline RuntimeClassID & operator = ( RuntimeClassID && src ) noexcept;

    /**
     * \brief   Assigning operator. Copies Runtime Class ID name from given string buffer source
     * \param   src     The source of string buffer to copy.
     * \return  Returns Runtime Class ID object.
     **/
    inline RuntimeClassID & operator = ( const char * src );
    inline RuntimeClassID & operator = ( const String& src );

    /**
     * \brief   Comparing operator. Compares 2 Runtime Class ID objects.
     * \param   other   The Runtime Class ID object to compare
     * \return  Returns true if 2 instance of Runtime Class ID have same Class ID value.
     **/
    inline bool operator == ( const RuntimeClassID & other ) const;
    /**
     * \brief   Comparing operator. Compares Runtime Class ID and string.
     * \param   other   The null-terminated string to compare
     * \return  Returns true if Runtime Class ID value is equal to null-terminated string.
     **/
    inline bool operator == ( const char * other ) const;
    inline bool operator == ( const String& other ) const;
    /**
     * \brief   Comparing operator. Compares 2 Runtime Class ID objects.
     * \param   other   The Runtime Class ID object to compare
     * \return  Returns true if 2 instance of Runtime Class ID have different Class ID values.
     **/
    inline bool operator != ( const RuntimeClassID & other ) const;
    /**
     * \brief   Comparing operator. Compares Runtime Class ID and string.
     * \param   other   The null-terminated string to compare
     * \return  Returns true if Runtime Class ID value is not equal to given null-terminated string.
     **/
    inline bool operator != (const char * other) const;
    inline bool operator != (const String& other) const;

    /**
     * \brief   Operator to convert the value or Runtime Class ID to unsigned integer value.
     *          Used to calculate hash value in hash map
     **/
    inline explicit operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Returns true if the value of Runtime Class ID is not equal to RuntimeClassID::BAD_CLASS_ID.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Returns the name of Runtime Class ID.
     **/
    inline const String& getName( void ) const;

    /**
     * \brief   Sets the name of Runtime Class ID.
     **/
    void setName( const char* className );
    void setName( const String& className );

    /**
     * \brief   Returns calculated number of runtime class.
     **/
    inline unsigned getMagic( void ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor. Private. Will create BAD_CLASS_ID
     *          object.
     **/
    RuntimeClassID( void );

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
    unsigned int    mMagicNum;
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
        //! A function to convert String object to unsigned int.
        inline unsigned int operator()(const RuntimeClassID& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// RuntimeClassID class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline RuntimeClassID RuntimeClassID::createEmptyClassID( void )
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
    setName(src);
    return (*this);
}

inline RuntimeClassID & RuntimeClassID::operator = ( const String & src )
{
    setName(src);
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

inline RuntimeClassID::operator unsigned int ( void ) const
{
    return mMagicNum;
}

inline bool RuntimeClassID::isValid( void ) const
{
    return (mMagicNum != NEMath::CHECKSUM_IGNORE);
}

inline const String & RuntimeClassID::getName( void ) const
{
    return mClassName;
}

inline unsigned RuntimeClassID::getMagic(void) const
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

inline bool operator == ( unsigned int lhs, const RuntimeClassID & rhs )
{
    return rhs.mMagicNum  == lhs;
}

inline bool operator != ( unsigned int lhs, const RuntimeClassID & rhs )
{
    return rhs.mMagicNum != lhs;
}

#endif  // AREG_BASE_RUNTIMECLASSID_HPP
