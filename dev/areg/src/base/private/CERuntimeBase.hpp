#ifndef AREG_BASE_PRIVATE_CERUNTIMEBASE_HPP
#define AREG_BASE_PRIVATE_CERUNTIMEBASE_HPP

/************************************************************************
 * \file        areg/src/base/private/CERuntimeBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Runtime Object base class
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CERuntimeClassID;

//////////////////////////////////////////////////////////////////////////
// CERuntimeBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This is base class of Runtime objects.
 *          All runtime object should derive this object.
 *          The runtime objects give possibility to define type of class
 *          without dynamic casting.
 *          Every runtime objects have runtime ClassID and name.
 **/
class AREG_API CERuntimeBase
{
//////////////////////////////////////////////////////////////////////////
// Hidden static calls.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns the ClassID object of CERuntimeBase class.
     **/
    static const CERuntimeClassID & _getClassId( void );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor, initializes runtime object.
     **/
    CERuntimeBase( void );

    /**
     * \brief   Copy constructor. Does nothing. Declared to enable copy constructor.
     **/
    CERuntimeBase( const CERuntimeBase & /*src*/ );

    /**
     * \brief   Destructor.
     **/
    virtual ~CERuntimeBase( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    
    /**
     * \brief   Assigning operator. Does nothing.
     *          Declared to enable assigning operator
     **/
    const CERuntimeBase & operator = ( const CERuntimeBase & src );

//////////////////////////////////////////////////////////////////////////
// Override attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief	Returns the Class Identifier object
     **/
    virtual const CERuntimeClassID & GetRuntimeClassId( void ) const;

    /**
     * \brief   Returns the class name, i.e. the name of Class Identifier
     **/
    virtual const char * GetRuntimeClassName( void ) const;

    /**
     * \brief   Checks class instance by name.
     * \param   className   The name of class to check.
     **/
    virtual bool IsInstanceOfRuntimeClass( const char* className ) const;

    /**
     * \brief   Checks class instance by Class Identifier.
     * \param   classId     The Class Identifier to check.
     **/
    virtual bool IsInstanceOfRuntimeClass( const CERuntimeClassID & classId ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    bool operator == ( const CERuntimeBase & /*other*/ ) const;
    bool operator != ( const CERuntimeBase & /*other*/ ) const;
};

#endif  // AREG_BASE_PRIVATE_CERUNTIMEBASE_HPP
