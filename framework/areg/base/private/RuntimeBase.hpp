#ifndef AREG_BASE_PRIVATE_RUNTIMEBASE_HPP
#define AREG_BASE_PRIVATE_RUNTIMEBASE_HPP

/************************************************************************
 * \file        areg/base/private/RuntimeBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Runtime Object base class
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class RuntimeClassID;

//////////////////////////////////////////////////////////////////////////
// RuntimeBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This is base class of Runtime objects.
 *          All runtime object should derive this object.
 *          The runtime objects give possibility to define type of class
 *          without dynamic casting.
 *          Every runtime objects have runtime ClassID and name.
 **/
class AREG_API RuntimeBase
{
//////////////////////////////////////////////////////////////////////////
// Hidden static calls.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns the ClassID object of RuntimeBase class.
     **/
    static const RuntimeClassID & _getClassId( void );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor, initializes runtime object.
     **/
    RuntimeBase( void );

    /**
     * \brief   Copy constructor. Does nothing. Declared to enable copy constructor.
     **/
    RuntimeBase( const RuntimeBase & /*src*/ );

    /**
     * \brief   Destructor.
     **/
    virtual ~RuntimeBase( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    
    /**
     * \brief   Assigning operator. Does nothing.
     *          Declared to enable assigning operator
     **/
    const RuntimeBase & operator = ( const RuntimeBase & src );

//////////////////////////////////////////////////////////////////////////
// Override attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief	Returns the Class Identifier object
     **/
    virtual const RuntimeClassID & getRuntimeClassId( void ) const;

    /**
     * \brief   Returns the class name, i.e. the name of Class Identifier
     **/
    virtual const char * getRuntimeClassName( void ) const;

    /**
     * \brief   Returns the calculated number of runtime class.
     **/
    virtual unsigned int getRuntimeClassNumber( void ) const;

    /**
     * \brief   Checks class instance by name.
     * \param   className   The name of class to check.
     **/
    virtual bool isInstanceOfRuntimeClass( const char* className ) const;

    /**
     * \brief   Checks class instance by name.
     * \param   className   The name of class to check.
     **/
    virtual bool isInstanceOfRuntimeClass( unsigned int classMagic ) const;

    /**
     * \brief   Checks class instance by Class Identifier.
     * \param   classId     The Class Identifier to check.
     **/
    virtual bool isInstanceOfRuntimeClass( const RuntimeClassID & classId ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    bool operator == ( const RuntimeBase & /*other*/ ) const;
    bool operator != ( const RuntimeBase & /*other*/ ) const;
};

#endif  // AREG_BASE_PRIVATE_RUNTIMEBASE_HPP
