#ifndef AREG_BASE_PRIVATE_POSIX_IEWAITABLEBASEIX_HPP
#define AREG_BASE_PRIVATE_POSIX_IEWAITABLEBASEIX_HPP
/************************************************************************
 * \file        areg/src/base/private/posix/IEWaitableBaseIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, waitable base object.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/src/base/GEGlobal.h"

#ifdef _POSIX

#include "areg/src/base/private/posix/CEMutexIX.hpp"

//////////////////////////////////////////////////////////////////////////
// CESynchWaitable class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for all waitable objects.
 *          The waitable objects are locked and released by signal.
 *          The waitable objects can be combined in the group, so that
 *          the waiting thread can wait for one or more objects to be
 *          signaled. This object cannot be directly instantiated.
 *          Instead, instantiate one of child classes.
 **/
class IEWaitableBaseIX : public CEMutexIX
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected instantiation constructor. Call to set the synchronization
     *          object type, recursive flag and the name.
     **/
    IEWaitableBaseIX( NESynchTypesIX::eSynchObject synchType, bool isRecursive, const char* asciiName = NULL );

public:
    /**
     * \brief   Destructor.
     **/
    virtual ~IEWaitableBaseIX( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the object is signaled. Otherwise, returns false.
     **/
    virtual bool IsSignaled( void ) const = 0;

    /**
     * \brief   This callback is triggered when a waiting thread is released to continue to run.
     * \param   ownerThread     Indicates the POSIX thread ID that completed to wait.
     * \return  Returns true if waitable successfully has taken thread ownership.
     **/
    virtual bool RequestsOwnership( pthread_t ownerThread ) = 0;

    /**
     * \brief   This callback is triggered to when a system needs to know whether waitable
     *          can signal multiple threads. Returned 'true' value indicates that there can be
     *          multiple threads can get waitable signaled state. For example, waitable Mutex 
     *          signals only one thread, when waitable Event can signal multiple threads.
     **/
    virtual bool CanSignalMultipleThreads( void ) const = 0;

    /**
     * \brief   This callback is called to notify the object the amount of
     *          threads that were leased when the object is in signaled state.
     * \param   numThreads  The number of threads that where released when the
     *                      object is in signaled state. 0 means that no thread
     *                      was released by the object.
     **/
    virtual void ThreadsReleased( int numThreads ) = 0;

    /**
     * \brief   Call when synchronization object is going to be deleted.
     *          This releases all waiting threads with failure code.
     **/
    virtual void FreeResources( void );

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    IEWaitableBaseIX( void );
    IEWaitableBaseIX( const IEWaitableBaseIX & /*src*/ );
    const IEWaitableBaseIX & operator = ( const IEWaitableBaseIX & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// IEWaitableBaseIX class inline implementation
//////////////////////////////////////////////////////////////////////////

#endif  // _POSIX

#endif  // AREG_BASE_PRIVATE_POSIX_IEWAITABLEBASEIX_HPP
