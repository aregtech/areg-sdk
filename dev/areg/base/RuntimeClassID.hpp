#ifndef AREG_BASE_RUNTIMECLASSID_HPP
#define AREG_BASE_RUNTIMECLASSID_HPP
/************************************************************************
 * \file        areg/base/RuntimeClassID.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
    template < typename KEY, typename VALUE, typename KEY_TYPE, typename VALUE_TYPE, class Implement >
    friend class TEHashMap;
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
    friend class TEResourceMap;

//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   RuntimeClassID::BAD_CLASS_ID
     *          Bad Class ID. Defined as constant. Used to indicate invalid class ID name.
     **/
    static const char * const   BAD_CLASS_ID;

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
    RuntimeClassID( const char * className );

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    RuntimeClassID( const RuntimeClassID & src );

    /**
     * \brief   Destructor
     **/
    ~RuntimeClassID( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// friend global operators
/************************************************************************/

    /**
     * \brief   Friend global comparing operator.
     *          Compare null-terminated string with Runtime Class ID name.
     * \param   lhs     Left-Hand Operand, source of string to compare.
     * \param   rhs     Right-Hand Operand, source of Runtime Class ID containing name to compare.
     * \return  Returns true if string value is equal to Runtime Class ID.
     **/
    friend inline bool operator == ( const char * lhs, const RuntimeClassID & rhs );

    /**
     * \brief   Friend global comparing operator.
     *          Compare null-terminated string with Runtime Class ID name.
     * \param   lhs     Left-Hand Operand, source of string to compare.
     * \param   rhs     Right-Hand Operand, source of Runtime Class ID containing name to compare.
     * \return  Returns true if string value is not equal to Runtime Class ID.
     **/
    friend inline bool operator != ( const char * lhs, const RuntimeClassID & rhs );

/************************************************************************/
// class members operators
/************************************************************************/

    /**
     * \brief   Assigning operator. Copies Runtime Class ID name from given Runtime Class ID source
     * \param   src     The source of Runtime Class ID to copy.
     * \return  Returns Runtime Class ID object.
     **/
    inline const RuntimeClassID & operator = ( const RuntimeClassID & src );

    /**
     * \brief   Assigning operator. Copies Runtime Class ID name from given string buffer source
     * \param   src     The source of string buffer to copy.
     * \return  Returns Runtime Class ID object.
     **/
    inline const RuntimeClassID & operator = ( const char * src );

    /**
     * \brief   Assigning operator. Copies Runtime Class ID name from given string source
     * \param   src     The source of string to copy.
     * \return  Returns Runtime Class ID object.
     **/
    inline const RuntimeClassID & operator = ( const String & src );

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

    /**
     * \brief   Operator to convert the value or Runtime Class ID to unsigned integer value.
     *          Used to calculate hash value in hash map
     **/
    operator unsigned int ( void ) const;

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
    inline const char * getName( void ) const;

    /**
     * \brief   Sets the name of Runtime Class ID.
     **/
    void setName( const char * className );

    /**
     * \brief   Returns true if Runtime Class name is empty.
     **/
    inline bool isEmpty( void ) const;

//////////////////////////////////////////////////////////////////////////
// Private members
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
    String    mClassName;
};

//////////////////////////////////////////////////////////////////////////
// RuntimeClassID class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline RuntimeClassID RuntimeClassID::createEmptyClassID( void )
{
    return BAD_CLASS_ID;
}

inline const RuntimeClassID & RuntimeClassID::operator = ( const RuntimeClassID & src )
{
    if (static_cast<const RuntimeClassID *>(this) != &src)
        this->mClassName = src.mClassName;
    return (*this);
}

inline const RuntimeClassID & RuntimeClassID::operator = ( const char * src )
{
    setName(src);
    return (*this);
}

inline const RuntimeClassID & RuntimeClassID::operator = ( const String & src )
{
    setName(src);
    return (*this);
}

inline bool RuntimeClassID::operator == ( const RuntimeClassID & other ) const
{
    return (this != &other ? this->mClassName == other.mClassName : true);
}

inline bool RuntimeClassID::operator == ( const char * other ) const
{
    return mClassName == other;
}

inline bool RuntimeClassID::operator != ( const RuntimeClassID  & other ) const
{
    return (this != &other ? this->mClassName != other.mClassName : false);
}

inline bool RuntimeClassID::operator != ( const char* other ) const
{
    return mClassName != other;
}

inline bool RuntimeClassID::isValid( void ) const
{
    return ((mClassName.isEmpty() == false) && (mClassName != BAD_CLASS_ID));
}

inline const char* RuntimeClassID::getName( void ) const
{
    return mClassName.getString();
}

inline bool RuntimeClassID::isEmpty( void ) const
{
    return mClassName.isEmpty();
}

inline bool operator == ( const char * lhs, const RuntimeClassID & rhs )
{
    return rhs.mClassName   == lhs;
}

inline bool operator != ( const char* lhs, const RuntimeClassID & rhs )
{
    return rhs.mClassName != lhs;
}

#endif  // AREG_BASE_RUNTIMECLASSID_HPP
