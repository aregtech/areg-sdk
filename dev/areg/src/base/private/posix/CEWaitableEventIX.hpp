#ifndef AREG_BASE_PRIVATE_POSIX_CEWAITABLEEVENTIX_HPP
#define AREG_BASE_PRIVATE_POSIX_CEWAITABLEEVENTIX_HPP
/************************************************************************
 * \file        areg/src/base/private/posix/CEWaitableEventIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/src/base/GEGlobal.h"

#ifdef _POSIX

#include "areg/src/base/private/posix/IEWaitableBaseIX.hpp"

//////////////////////////////////////////////////////////////////////////
// CEWaitableEventIX class declaration.
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
 *          A thread can wait for one, any or all events. For more details see @CESynchLockAndWaitIX.
 *          If Event is in signaled state, multiple calls to signal Event are ignored.
 *          If Event is in non-signaled state, multiple calls to reset signal state are ignored.
 *
 * \see     CESynchLockAndWaitIX
 **/
class CEWaitableEventIX : public IEWaitableBaseIX
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
    CEWaitableEventIX(bool isInitSignaled = false, bool isAutoReset = false, const char * asciiName = NULL);

    /**
     * \brief   Destructor.
     **/
    virtual ~CEWaitableEventIX( void );

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
    inline NESynchTypesIX::eEventResetInfo  GetResetInfo( void ) const;

    /**
     * \brief   Call to set waitable Event signaled. If Event is signaled, all waiting threads
     *          are released. If Event type is NESynchTypesIX::EventResetAutomatic, it is
     *          automatically reset. Otherwise, it remains signaled until it is not manually reset.
     *          Waitable Event signal are received by all waiting threads.
     * \return  Returns true if operation succeeded.
     **/
    bool SetEvent( void );

    /**
     * \brief   Call to reset the signal state of waitable Event, i.e. make non-signaled.
     *          Only NESynchTypesIX::EventResetManual type Events can be manually reset.
     *          The call is ignored if event type is NESynchTypesIX::EventResetAutomatic,
     *          because automatic events are automatically reset after threads get released.
     * \return  Returns true if operation succeeded. The operation may fail if the waitable
     *          Event is created with auto-reset flag.
     **/
    bool ResetEvent( void );

    /**
     * \brief   Call to pulse event. If Event is in non-signaled state,
     *          indifferent whether it is manual- or auto-reset, it is
     *          switched to signaled state and immediately reset to non-signaled.
     *          If signaled, all waiting threads are immediately released.
     **/
    void PulseEvent( void );

/************************************************************************/
// IEWaitableBaseIX callback overrides.
/************************************************************************/
#if 0
    /**
     * \brief   Call to set signal state of the object. If object is signaled, 
     *          all signal waiting threads are released. The NESynchTypesIX::EventResetAutomatic
     *          Events then are automatically reset and switch to non-signaled state.
     *          The NESynchTypesIX::EventResetAutomatic cannot be manually reset.
     * \param   isSignaled      - If true, the Event is signaled. The function is
     *                              is ignored if it was signaled. 
     *                          - If false, the Event is switched to non-signaled state
     *                              in case of EventResetManual Events. The flag is ignored
     *                              in case of EventResetAutomatic, because automatic reset
     *                              events are automatically switch to non-signaled state when
     *                              any waiting thread is released. The EventResetAutomatic
     *                              Events can be only manually set to signaled state.
     * \return  Returns true if object successfully changed the state.
     **/
    virtual bool SetSignaled( bool isSignaled );
#endif

    /**
     * \brief   Returns true if the object is signaled. Otherwise, returns false.
     **/
    virtual bool IsSignaled( void ) const;

    /**
     * \brief   This callback is triggered when a waiting thread is released to continue to run.
     *          Waitable Event always return true.
     * \param   ownerThread     Indicates the POSIX thread ID that completed to wait.
     * \return  Waitable Event always returns true.
     **/
    virtual bool RequestsOwnership( pthread_t ownerThread );

    /**
     * \brief   This callback is triggered to when a system needs to know whether waitable
     *          can signal multiple threads. Returned 'true' value indicates that there can be
     *          multiple threads can get waitable signaled state. For example, waitable Mutex 
     *          signals only one thread, when waitable Event can signal multiple threads.
     **/
    virtual bool CanSignalMultipleThreads( void ) const;

    /**
     * \brief   This callback is called to notify the object the amount of
     *          threads that were leased when the object is in signaled state.
     * \param   numThreads  The number of threads that where released when the
     *                      object is in signaled state. 0 means that no thread
     *                      was released by the object.
     **/
    virtual void ThreadsReleased( int numThreads );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Specifies whether the event is manual- or auto-reset.
     **/
    const NESynchTypesIX::eEventResetInfo   mEventReset;

    mutable bool                            mIsSignaled;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    CEWaitableEventIX( const CEWaitableEventIX & /*src*/ );
    const CEWaitableEventIX & operator = ( const CEWaitableEventIX & /*src*/);
};

//////////////////////////////////////////////////////////////////////////
// CEWaitableEventIX class inline implementation
//////////////////////////////////////////////////////////////////////////

inline NESynchTypesIX::eEventResetInfo CEWaitableEventIX::GetResetInfo( void ) const
{
    CEObjectLockIX lock(*this);
    return mEventReset;
}

#endif  // _POSIX

#endif  // AREG_BASE_PRIVATE_POSIX_CEWAITABLEEVENTIX_HPP
