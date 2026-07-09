#ifndef AREG_BASE_SYNCOBJECT_HPP
#define AREG_BASE_SYNCOBJECT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/SyncObject.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Base Synchronization object interface.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/CommonDefs.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SyncObject class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for synchronization objects. Provides locking and unlocking interface and
 *          basic attributes for all synchronization objects.
 **/
class AREG_API SyncObject
{
//////////////////////////////////////////////////////////////////////////
// Class typed and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Type of supported Synchronization objects
     **/
    enum class SyncKind  : int16_t
    {
          SoUnknown     = -1    //!< Unknown type of synchronization object.
        , SoMutex       =  0    //!< Synchronization object is a Mutex.
        , SoEvent               //!< Synchronization object is an Event.
        , SoSemaphore           //!< Synchronization object is a Semaphore.
        , SoCritical            //!< Synchronization object is a Critical Section
        , SoSpinlock            //!< Synchronization object is a Spin-Lock.
        , SoNolock              //!< Empty / Dummy Synchronization object, no real locking operations are supported
        , SoTimer               //!< Synchronization object is a Waiting Timer.
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes synchronization object. Protected; only accessible to derived classes.
     * \param   syncObjectType  Type of synchronization object
     **/
    explicit SyncObject( SyncObject::SyncKind syncObjectType );

public:
    virtual ~SyncObject();

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts synchronization object instance to void pointer handle.
     **/
    [[nodiscard]]
    inline operator SYNCHANDLE () noexcept;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the synchronization object handle value.
     **/
    [[nodiscard]]
    inline SYNCHANDLE handle() const noexcept;

    /**
     * \brief   Returns the type of synchronization object.
     **/
    [[nodiscard]]
    inline SyncObject::SyncKind type() const noexcept;

    /**
     * \brief   Returns true if the synchronization object is valid.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Acquires ownership of the synchronization object. If the object is already owned,
     *          waits up to the timeout interval. Different synchronization objects may perform
     *          different actions.
     *
     * \param   timeout     The timeout in milliseconds to wait for ownership. WAIT_INFINITE means
     *                      wait indefinitely.
     * \return  Returns true if the thread successfully acquired ownership; false if timeout expired.
     **/
    virtual bool lock( uint32_t timeout = areg::WAIT_INFINITE );

    /**
     * \brief   Releases ownership of the synchronization object.
     *
     * \return  Returns true if the thread successfully released ownership.
     **/
    virtual bool unlock();

//////////////////////////////////////////////////////////////////////////
// Operations, private
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Destroys the synchronization object. Normally called in the destructor.
     **/
    void _os_destroy();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    SYNCHANDLE                  mSyncObject;    //!< Handle to synchronization object
    const SyncObject::SyncKind  mSyncObjectType;//!< Synchronization object type

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    SyncObject() = delete;
    AREG_NOCOPY_NOMOVE( SyncObject );
};

//////////////////////////////////////////////////////////////////////////
// Inline functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SyncObject class inline functions
//////////////////////////////////////////////////////////////////////////
inline SyncObject::operator SYNCHANDLE () noexcept
{
    return mSyncObject;
}

inline SYNCHANDLE SyncObject::handle() const noexcept
{
    return mSyncObject;
}

inline SyncObject::SyncKind SyncObject::type() const noexcept
{
    return mSyncObjectType;
}

inline bool SyncObject::is_valid() const noexcept
{
    return (mSyncObjectType == SyncObject::SyncKind::SoNolock) || (mSyncObjectType == SyncObject::SyncKind::SoSpinlock) || (mSyncObject != nullptr);
}

} // namespace areg
#endif  // AREG_BASE_SYNCOBJECT_HPP
