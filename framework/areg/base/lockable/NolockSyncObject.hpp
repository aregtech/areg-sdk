#ifndef AREG_BASE_NOLOCKSYNCOBJECT_HPP
#define AREG_BASE_NOLOCKSYNCOBJECT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/locakble/NolockSyncObject.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects
 *              Declared following synchronization objects:
 *              NolockSyncObject    - No Locking synchronization object (makes no locking).
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/lockable/lockable.h"

namespace areg {
//////////////////////////////////////////////////////////////////////////
// NolockSyncObject class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Dummy synchronization object that performs no locking. Used as a no-op when
 *          synchronization is not required but a SyncObject interface is expected.
 **/
class AREG_API NolockSyncObject  final  : public Lockable
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    NolockSyncObject();

    virtual ~NolockSyncObject() = default;

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   No-op lock operation.
     * \return  Always returns true.
     **/
    inline bool lock();

    /**
     * \brief   No-op lock operation with timeout (timeout is ignored).
     * \return  Always returns true.
     **/
    inline bool lock( uint32_t /*timeout = areg::WAIT_INFINITE*/ ) final;

    /**
     * \brief   No-op unlock operation.
     * \return  Always returns true.
     **/
    inline bool unlock() final;

    /**
     * \brief   No-op try-lock operation.
     * \return  Always returns true.
     **/
    inline bool try_lock() final;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( NolockSyncObject );
};


//////////////////////////////////////////////////////////////////////////
// NolockSyncObject class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool NolockSyncObject::lock()
{
    return true;
}

inline bool NolockSyncObject::lock( uint32_t /*timeout = areg::WAIT_INFINITE*/ )
{
    return true;
}

inline bool NolockSyncObject::unlock()
{
    return true;
}

inline bool NolockSyncObject::try_lock()
{
    return true;
}


} // namepsace areq

#endif // AREG_BASE_NOLOCKSYNCOBJECT_HPP

