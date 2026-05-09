#ifndef AREG_BASE_MUTEX_HPP
#define AREG_BASE_MUTEX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/lockable/Mutex.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects
 *              Declared following synchronization objects:
 *              Mutex               - Mutex synchronization object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/lockable/lockable.h"

#include <atomic>

namespace areg {
//////////////////////////////////////////////////////////////////////////
// Mutex class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Synchronization object for mutual exclusion across single or multiple processes. Allows
 *          one thread at a time to own it and supports timeout-based locking.
 **/
class AREG_API Mutex final  : public Lockable
{
//////////////////////////////////////////////////////////////////////////
// Friend objects
//////////////////////////////////////////////////////////////////////////
friend class MultiLock;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates and optionally initializes the mutex with ownership.
     *
     * \param   initLock    If true, the current thread acquires ownership on initialization.
     **/
    explicit Mutex( bool initLock = true );

    virtual ~Mutex();

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Acquires mutex ownership. Blocks for up to timeout milliseconds if owned by another
     *          thread.
     *
     * \param   timeout     Timeout in milliseconds; WAIT_INFINITE to wait indefinitely.
     * \return  Returns true if ownership was acquired.
     **/
    inline bool lock( uint32_t timeout = areg::WAIT_INFINITE ) final;

    /**
     * \brief   Releases mutex ownership. Caller must own the mutex.
     *
     * \return  Returns true if successful.
     **/
    inline bool unlock() final;

    /**
     * \brief   Attempts to acquire mutex ownership without blocking.
     *
     * \return  Returns true if acquired or already owned by current thread; false if owned by
     *          another.
     **/
    inline bool try_lock() final;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if the mutex is currently locked by any thread.
     **/
    [[nodiscard]]
    inline bool is_locked() const noexcept;

    /**
     * \brief   Returns the thread ID of the thread currently owning the mutex.
     **/
    [[nodiscard]]
    inline id_type owner_thread_id() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden or OS specific implementations
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   OS-specific implementation to lock and take ownership.
     *
     * \param   timeout     The timeout in milliseconds to wait for the mutex.
     **/
    bool _os_lock_mutex( uint32_t timeout );

    /**
     * \brief   OS-specific implementation to unlock and release ownership.
     **/
    bool _os_unlock_mutex();

    /**
     * \brief   OS-specific creation. If initLock is true, current thread acquires ownership.
     **/
    void _os_create_mutex( bool initLock );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    std::atomic<id_type>    mOwnerThreadId; //!< The ID of thread currently owning mutex

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( Mutex );
};



//////////////////////////////////////////////////////////////////////////
// Inline functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mutex class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool Mutex::lock( uint32_t timeout /* = areg::WAIT_INFINITE */ )
{
    ASSERT( mSyncObject != nullptr );
    return _os_lock_mutex( timeout );
}

inline bool Mutex::try_lock()
{
    return lock( areg::DO_NOT_WAIT );
}

inline bool Mutex::unlock()
{
    ASSERT( mSyncObject != nullptr );
    return _os_unlock_mutex( );
}

inline bool Mutex::is_locked() const noexcept
{
    return (mOwnerThreadId.load() != 0);
}

inline id_type Mutex::owner_thread_id() const noexcept
{
    return mOwnerThreadId.load();
}
} // namespace areq
#endif // AREG_BASE_MUTEX_HPP
