#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableEventIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/IEWaitableBaseIX.hpp"

//////////////////////////////////////////////////////////////////////////
// WaitableEventIX class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The synchronization event object. As a waitable object, the event has signaled
 *          and non-signaled state. When the event is in signaled state, all threads that
 *          wait for the signal are notified and released depending whether they wait
 *          for all events or any event. There can be 2 types of events:
 *              - Manual-reset: The Event remains in signaled state so long, until signal is 
 *                              manually reset. The signal and non-signaled states are set
 *                              by explicit call. Multiple waiting threads can be released
 *                              at the same time.
 *              - Auto-reset:   The Event remains in signaled state so long, until any waiting
 *                              thread is not released. As soon as at least one thread is released
 *                              the event automatically switches to not signaled state. Multiple 
 *                              waiting threads can be released at the same time. The threads are
 *                              released by priority order.
 *          A thread can wait for one, any or all events. For more details see @SynchLockAndWaitIX.
 *          If Event is in signaled state, multiple calls to signal Event are ignored.
 *          If Event is in non-signaled state, multiple calls to reset signal state are ignored.
 *
 * \see     SynchLockAndWaitIX
 **/
class WaitableEventIX : public IEWaitableBaseIX
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the synchronization Event object, sets the type and initial signaled state.
     * \param   isInitSignaled  If true, the Event is initially in signaled state. While it is
     *                          in signaled state, the waiting threads can be released. If is it in 
     *                          non-signaled state, any thread that waits for signal is blocked.
     * \param   isAutoReset     If true, the Event object is auto-reset; otherwise, it is manual reset Event.
     *                          - manual-reset: The manual reset Events are manually switched to signaled and 
     *                                          non-signaled state. While the Event is in signaled state,
     *                                          the waiting thread is immediately released and continue run.
     *                                          Otherwise, it is in blocked until the Event is not manually signaled.
     *                          - auto-reset:   If the event is signaled, it remains in signaled state so long until
     *                                          at least one waiting thread is released. As soon as at least one thread
     *                                          is released, the signal state is automatically set to non-signaled.
     *                                          The auto-reset Events are manually signaled and all waiting threads 
     *                                          are released and the signal state is changed.
     * \param   asciiName       The name of synchronization Event.
     **/
    WaitableEventIX(bool isInitSignaled, bool isAutoReset, const char * asciiName = nullptr);

    /**
     * \brief   Destructor.
     **/
    virtual ~WaitableEventIX( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the reset information of synchronization Event.
     *          The Event object can be either NESynchTypesIX::EventResetManual
     *          or NESynchTypesIX::EventResetAutomatic. The EventResetManual
     *          reset Events are manually set to signaled and non-signaled state.
     *          The EventResetAutomatic Events are manually signaled and
     *          automatically reset as soon as any waiting thread is released.
     **/
    inline NESynchTypesIX::eEventResetInfo  getResetInfo( void ) const;

    /**
     * \brief   Call to set waitable Event signaled. If Event is signaled, all waiting threads
     *          are released. If Event type is NESynchTypesIX::EventResetAutomatic, it is
     *          automatically reset. Otherwise, it remains signaled until it is not manually reset.
     *          Waitable Event signal are received by all waiting threads.
     * \return  Returns true if operation succeeded.
     **/
    bool setEvent( void );

    /**
     * \brief   Call to reset the signal state of waitable Event, i.e. make non-signaled.
     *          Only NESynchTypesIX::EventResetManual type Events can be manually reset.
     *          The call is ignored if event type is NESynchTypesIX::EventResetAutomatic,
     *          because automatic events are automatically reset after threads get released.
     * \return  Returns true if operation succeeded. The operation may fail if the waitable
     *          Event is created with auto-reset flag.
     **/
    bool resetEvent( void );

    /**
     * \brief   Call to pulse event. If Event is in non-signaled state,
     *          indifferent whether it is manual- or auto-reset, it is
     *          switched to signaled state and immediately reset to non-signaled.
     *          If signaled, all waiting threads are immediately released.
     **/
    void pulseEvent( void );

/************************************************************************/
// IEWaitableBaseIX callback overrides.
/************************************************************************/

    /**
     * \brief   This callback is called to check the signals state of waitable object.
     * \param   contextThread   The thread ID where the lock and wait happend.
     *                          This parameter not used for waitable event.
     * \return  Returns true if the object is signaled. Otherwise, returns false.
     **/
    virtual bool checkSignaled( pthread_t contextThread ) const override;

    /**
     * \brief   This callback is triggered when a waiting thread is released to continue to run.
     *          Waitable Event always return true.
     * \param   ownerThread     Indicates the POSIX thread ID that completed to wait.
     * \return  Waitable Event always returns true.
     **/
    virtual bool notifyRequestOwnership( pthread_t ownerThread ) override;

    /**
     * \brief   This callback is triggered to when a system needs to know whether waitable
     *          can signal multiple threads. Returned 'true' value indicates that there can be
     *          multiple threads can get waitable signaled state. For example, waitable Mutex 
     *          signals only one thread, when waitable Event can signal multiple threads.
     **/
    virtual bool checkCanSignalMultipleThreads( void ) const override;

    /**
     * \brief   This callback is called to notify the object the amount of
     *          threads that were leased when the object is in signaled state.
     * \param   numThreads  The number of threads that where released when the
     *                      object is in signaled state. 0 means that no thread
     *                      was released by the object.
     **/
    virtual void notifyReleasedThreads( int numThreads ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Specifies whether the event is manual- or auto-reset.
     **/
    const NESynchTypesIX::eEventResetInfo   mEventReset;
    /**
     * \brief   Flag that indicates the signaled state of the event.
     **/
    mutable bool                            mIsSignaled;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    WaitableEventIX( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( WaitableEventIX );
};

//////////////////////////////////////////////////////////////////////////
// WaitableEventIX class inline implementation
//////////////////////////////////////////////////////////////////////////

inline NESynchTypesIX::eEventResetInfo WaitableEventIX::getResetInfo( void ) const
{
    ObjectLockIX lock(*this);
    return mEventReset;
}

#endif  // defined(_POSIX) || defined(POSIX)
