#ifndef AREG_BASE_PRIVATE_POSIX_SYNCOBJECTBASEIX_HPP
#define AREG_BASE_PRIVATE_POSIX_SYNCOBJECTBASEIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SyncObjectPosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX based Synchronization object base.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/areg_global.h"

#if  defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SyncDefsPosix.hpp"
#include "areg/base/SyncObject.hpp"
#include "areg/base/String.hpp"
namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// Declared classes and hierarchies.
//////////////////////////////////////////////////////////////////////////
class SyncObjectPosix;
    class CriticalSectionPosix;
    class MutexPosix;
        class WaitablePosix;
            class WaitableEventPosix;
            class WaitableMutexPosix;
            class WaitableSemaphorePosix;
            class WaitableTimerPosix;

//////////////////////////////////////////////////////////////////////////
// SyncObjectPosix class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   POSIX-specific base class for all synchronization objects. Stores object type and name.
 **/
class SyncObjectPosix
{
//////////////////////////////////////////////////////////////////////////
// Constants, types and static members.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The length of synchronization object name, including null-termination symbol.
     **/
    static constexpr int32_t    _MAX_NAME_LENGTH    { 32 };

//////////////////////////////////////////////////////////////////////////
// Protected constructor.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes the POSIX synchronization object with the specified type and optional name.
     *
     * \param   syncType        The type of synchronization object.
     **/
    inline SyncObjectPosix( areg::os::SyncKind syncType );

//////////////////////////////////////////////////////////////////////////
// Public destructor.
//////////////////////////////////////////////////////////////////////////
public:
    virtual ~SyncObjectPosix() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// SyncObjectPosix attributes.
/************************************************************************/
    
    /**
     * \brief   Returns the type of synchronization object.
     **/
    [[nodiscard]]
    inline areg::os::SyncKind sync_type() const noexcept;

/************************************************************************/
// SyncObjectPosix overrides.
/************************************************************************/
    
    /**
     * \brief   Returns true if the synchronization object is valid. Pure virtual; must be
     *          implemented by subclasses.
     **/
    [[nodiscard]]
    virtual bool is_valid() const noexcept = 0;

    /**
     * \brief   Called when the synchronization object is being destroyed. Subclasses must implement
     *          resource cleanup.
     **/
    virtual void free_resources() = 0;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   The type of synchronization object.
     **/
    const areg::os::SyncKind  mSyncType;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    SyncObjectPosix() = delete;
    AREG_NOCOPY_NOMOVE( SyncObjectPosix );
};


//////////////////////////////////////////////////////////////////////////
// SyncObjectPosix class inline implementation
//////////////////////////////////////////////////////////////////////////
inline SyncObjectPosix::SyncObjectPosix(areg::os::SyncKind syncType)
    : mSyncType(syncType)
{
}

inline areg::os::SyncKind SyncObjectPosix::sync_type() const noexcept
{
    return mSyncType;
}

} // namespace areg::os

#endif //  defined(_POSIX) || defined(POSIX)

#endif // AREG_BASE_PRIVATE_POSIX_SYNCOBJECTBASEIX_HPP
