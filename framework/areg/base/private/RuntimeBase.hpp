#ifndef AREG_BASE_PRIVATE_RUNTIMEBASE_HPP
#define AREG_BASE_PRIVATE_RUNTIMEBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/RuntimeBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
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
    RuntimeBase( void ) = default;

    /**
     * \brief   Destructor.
     **/
    virtual ~RuntimeBase( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Override attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the Class Identifier object
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
    DECLARE_NOCOPY_NOMOVE( RuntimeBase );
    bool operator == ( const RuntimeBase & /*other*/ ) const = delete;
    bool operator != ( const RuntimeBase & /*other*/ ) const = delete;
};

#endif  // AREG_BASE_PRIVATE_RUNTIMEBASE_HPP
