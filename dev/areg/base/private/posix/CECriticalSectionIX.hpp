#ifndef AREG_BASE_PRIVATE_POSIX_CECRITICALSECTIONIX_HPP
#define AREG_BASE_PRIVATE_POSIX_CECRITICALSECTIONIX_HPP
/************************************************************************
 * \file        areg/base/private/posix/CECriticalSectionIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Spin Lock wrapper class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#ifdef  _POSIX
#include "areg/base/private/posix/IESynchObjectBaseIX.hpp"
#include <pthread.h>

//////////////////////////////////////////////////////////////////////////
// CECriticalSectionIX class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   POSIX Critical Section is a wrapper of POSIX spin lock, since
 *          it is specified as the  fastest locking / synchronization object.
 *          The Critical Section can be used only for the communication between threads.
 *          The Critical Section can be owned only by one thread at a time.
 **/
class CECriticalSectionIX   : protected IESynchObjectBaseIX
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Creates Critical Section object and initializes POSIX spin lock.
     * \param   initLock    If true, the critical section (spin lock) is initially locked.
     **/
    CECriticalSectionIX( bool initLock = false );

    /**
     * \brief   Destroy the object, free resources.
     **/
    virtual ~CECriticalSectionIX( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Locks the critical section and prevents other threads to
     *          access shared data until the locking thread does not release
     *          the critical section.
     * \return  Returns true if operation succeeded.
     **/
    inline bool Lock( void ) const;

    /**
     * \brief   Unlocks critical section so that, one of waiting threads is released
     *          to own critical section.
     *          
     **/
    inline void Unlock( void ) const;

    /**
     * \brief   Tests whether the critical section can be locked or not.
     *          It is locked if returned value is 'true'.
     * \return  Returns true if calling thread have got the ownership.
     **/
    inline bool TryLock( void ) const;

protected:
/************************************************************************/
// IESynchObjectBaseIX overrides.
/************************************************************************/
    
    /**
     * \brief   Returns true if synchronization object is valid.
     **/
    virtual bool IsValid( void ) const;

    /**
     * \brief   Triggered when synchronization object is going to be deleted.
     *          This should free all resources.
     **/
    virtual void FreeResources( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    mutable pthread_spinlock_t  mSpin;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CECriticalSectionIX( const CECriticalSectionIX & /*src*/ );
    const CECriticalSectionIX & operator = ( const CECriticalSectionIX & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CESpinLockIX class declaration.
//////////////////////////////////////////////////////////////////////////
class CESpinLockIX
{
public:
    CESpinLockIX( CECriticalSectionIX & lock );
    ~CESpinLockIX( void );

    inline bool Lock( void ) const;

    inline bool TryLock( void ) const;

    inline void Unlock( void ) const;

private:
    CECriticalSectionIX & mLocker;

private:
    CESpinLockIX( void );
    CESpinLockIX( const CESpinLockIX & /*src*/ );
    const CESpinLockIX & operator = ( const CESpinLockIX & /*src*/ );
};
//////////////////////////////////////////////////////////////////////////
// CECriticalSectionIX inline methods.
//////////////////////////////////////////////////////////////////////////

inline bool CECriticalSectionIX::Lock(void) const
{   return (RETURNED_OK == pthread_spin_lock(&mSpin));      }

inline void CECriticalSectionIX::Unlock(void) const
{   pthread_spin_unlock(&mSpin);                            }

inline bool CECriticalSectionIX::TryLock(void) const
{   return (RETURNED_OK == pthread_spin_trylock(&mSpin));   }

//////////////////////////////////////////////////////////////////////////
// CESpinLockIX inline methods.
//////////////////////////////////////////////////////////////////////////

inline bool CESpinLockIX::Lock(void) const
{   return mLocker.Lock();      }

inline bool CESpinLockIX::TryLock(void) const
{   return mLocker.TryLock();   }

inline void CESpinLockIX::Unlock(void) const
{   mLocker.Unlock();           }

#endif  // _POSIX

#endif  // AREG_BASE_PRIVATE_POSIX_CECRITICALSECTIONIX_HPP
