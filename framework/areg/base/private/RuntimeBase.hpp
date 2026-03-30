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
 * \brief       Areg Platform, Runtime Object base class.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/MathDefs.hpp"
#include "areg/base/RuntimeClassID.hpp"
#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class RuntimeClassID;
    class String;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// RuntimeBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for objects with runtime type identification.
 *
 *          Provides virtual class_id(), class_name(), class_number(), and
 *          is_runtime() methods.  Derived classes use AREG_DECLARE_RUNTIME /
 *          AREG_IMPLEMENT_RUNTIME to override these with their own
 *          compile-time static constexpr RuntimeClassID.
 **/
class AREG_API RuntimeBase
{
//////////////////////////////////////////////////////////////////////////
// Hidden static CLASS_ID identity
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Full compile-time runtime class identifier for RuntimeBase.
     *          Single source of truth: name() and magic() both come from here.
     **/
    static constexpr RuntimeClassID CLASS_ID { "RuntimeBase" };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    RuntimeBase() = default;

    virtual ~RuntimeBase() = default;

//////////////////////////////////////////////////////////////////////////
// Virtual interface
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the runtime class identifier for this object.
     **/
    [[nodiscard]]
    virtual const RuntimeClassID & class_id() const noexcept;

    /**
     * \brief   Returns the class name.
     **/
    [[nodiscard]]
    virtual std::string_view class_name() const noexcept;

    /**
     * \brief   Returns the class name as a C-string.
     **/
    [[nodiscard]]
    virtual const char* class_string() const noexcept;

    /**
     * \brief   Returns the CRC32 magic number of the runtime class.
     **/
    [[nodiscard]]
    virtual uint32_t class_number() const noexcept;

    /**
     * \brief   Returns true if this object is an instance of the class with the given name.
     **/
    [[nodiscard]]
    virtual bool is_runtime( const char * className ) const noexcept;

    /**
     * \brief   Returns true if this object is an instance of the class with the given name.
     **/
    [[nodiscard]]
    virtual bool is_runtime( std::string_view className ) const noexcept;

    /**
     * \brief   Returns true if this object is an instance of the class with the given name.
     **/
    [[nodiscard]]
    virtual bool is_runtime( const String & className ) const noexcept;

    /**
     * \brief   Returns true if this object is an instance of the class with the given magic number.
     *          The magic number is a compile-time CRC32 of the class name.
     **/
    [[nodiscard]]
    virtual bool is_runtime( uint32_t classMagic ) const noexcept;

    /**
     * \brief   Returns true if this object is an instance of the given class.
     **/
    [[nodiscard]]
    virtual bool is_runtime( const RuntimeClassID & classId ) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( RuntimeBase );
    bool operator == ( const RuntimeBase & ) const = delete;
    bool operator != ( const RuntimeBase & ) const = delete;
};

} // namespace areg
#endif  // AREG_BASE_PRIVATE_RUNTIMEBASE_HPP
