#ifndef AREG_BASE_PRIVATE_RUNTIMEBASE_HPP
#define AREG_BASE_PRIVATE_RUNTIMEBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/RuntimeBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Runtime Object base class
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class RuntimeClassID;
class String;

//////////////////////////////////////////////////////////////////////////
// RuntimeBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for objects with runtime type identification. Provides class ID and name
 *          without requiring dynamic casting.
 **/
class AREG_API RuntimeBase
{
//////////////////////////////////////////////////////////////////////////
// Hidden static calls.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns the RuntimeClassID of the RuntimeBase class.
     **/
    static const RuntimeClassID & _class_id();

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor initializes a runtime object.
     **/
    RuntimeBase() = default;

    /**
     * \brief   Destructor.
     **/
    virtual ~RuntimeBase() = default;

//////////////////////////////////////////////////////////////////////////
// Override attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the runtime class identifier.
     **/
    virtual const RuntimeClassID & runtime_class_id() const;

    /**
     * \brief   Returns the class name.
     **/
    virtual const String& runtime_class_name() const;

    /**
     * \brief   Returns the calculated hash number of the runtime class.
     **/
    virtual uint32_t runtime_class_number() const;

    /**
     * \brief   Returns true if instance is of the given class name.
     *
     * \param   className       The class name to check.
     **/
    virtual bool is_runtime_instance( const char* className ) const;
    /**
     * \brief   Returns true if instance is of the given class name.
     *
     * \param   className       The class name to check.
     **/
    virtual bool is_runtime_instance( const String& className ) const;

    /**
     * \brief   Returns true if instance is of the class with the given magic number.
     *
     * \param   classMagic      The class magic number to check.
     **/
    virtual bool is_runtime_instance( uint32_t classMagic ) const;

    /**
     * \brief   Returns true if instance is of the given class identifier.
     *
     * \param   classId     The class identifier to check.
     **/
    virtual bool is_runtime_instance( const RuntimeClassID & classId ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( RuntimeBase );
    /**
     * \brief   Equality operator is deleted.
     **/
    bool operator == ( const RuntimeBase & /*other*/ ) const = delete;
    /**
     * \brief   Inequality operator is deleted.
     **/
    bool operator != ( const RuntimeBase & /*other*/ ) const = delete;
};

} // namespace areg
#endif  // AREG_BASE_PRIVATE_RUNTIMEBASE_HPP
