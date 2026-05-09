#ifndef AREG_BASE_LOCABLE_H
#define AREG_BASE_LOCABLE_H
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/lockable/lockable.h
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects
 *              Declared following synchronization objects:
 *              Lockable      - blocking synchronization object interface.
 *              ResourceLock        - An OS specific resource lock object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SyncObject.hpp"

namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class SpinLock;

//////////////////////////////////////////////////////////////////////////
// ResuorceLock class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   OS-specific recursive synchronization lock for resource access. Supports recursive
 *          locking within the same thread context to avoid deadlocks.
 **/
using ResourceLock = SpinLock;

//////////////////////////////////////////////////////////////////////////
// Lockable class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base interface for objects that provide synchronization (lock, unlock, try_lock).
 **/
class AREG_API Lockable : public SyncObject
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor. Initialize with the specified synchronization object type.
     *          Accessible only to derived classes.
     *
     * \param   syncObjectType      The type of synchronization object.
     **/
    Lockable( SyncObject::SyncKind syncObjectType );

public:
    virtual ~Lockable() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
// Pure virtual functions to overwrite
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Attempts to acquire the synchronization object without blocking.
     *
     * \return  Returns true if ownership was acquired or is already held by the current thread;
     *          false if another thread owns it.
     **/
    virtual bool try_lock();

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    Lockable() = delete;
    AREG_NOCOPY_NOMOVE( Lockable );
};



} // namespace areq

#endif // AREG_BASE_LOCABLE_H


