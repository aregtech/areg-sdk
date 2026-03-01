#ifndef AREG_BASE_PRIVATE_POSIX_WAITABLEEVENTIX_HPP
#define AREG_BASE_PRIVATE_POSIX_WAITABLEEVENTIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableEventPosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/areg_global.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/WaitablePosix.hpp"
namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// WaitableEventPosix class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Synchronization event object with manual-reset or auto-reset modes. Manual-reset events
 *          remain signaled until explicitly reset. Auto-reset events automatically reset after a
 *          waiting thread is released. Multiple threads can be released simultaneously.
 **/
class WaitableEventPosix : public WaitablePosix
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the synchronization event object, sets the type and initial signaled
     *          state.
     *
     * \param   isInitSignaled      If true, the event is initially in signaled state. If false, it
     *                              is initially in non-signaled state.
     * \param   is_auto_reset       If true, the event is auto-reset; otherwise, it is manual-reset.
     *                              Auto-reset events automatically reset to non-signaled after a
     *                              waiting thread is released. Manual-reset events remain signaled
     *                              until explicitly reset.
     * \param   asciiName           The name of the synchronization event.
     **/
    WaitableEventPosix(bool isInitSignaled, bool is_auto_reset, const char * asciiName = nullptr);

    /**
     * \brief   Destructor.
     **/
    virtual ~WaitableEventPosix() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the reset mode of the event, either manual-reset or auto-reset.
     **/
    inline areg::os::ResetMode  reset_info() const;

    /**
     * \brief   Sets the event to signaled state. If the event is auto-reset, it automatically
     *          resets after waiting threads are released. If manual-reset, it remains signaled
     *          until explicitly reset.
     *
     * \return  Returns true if operation succeeded.
     **/
    bool set_event();

    /**
     * \brief   Resets the event to non-signaled state. Only manual-reset events can be manually
     *          reset; calls on auto-reset events are ignored.
     *
     * \return  Returns true if operation succeeded; false if the event is auto-reset.
     **/
    bool reset();

    /**
     * \brief   Pulses the event: briefly signals it and immediately resets to non-signaled,
     *          releasing waiting threads.
     **/
    void pulse_event();

/************************************************************************/
// WaitablePosix callback overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the event is signaled; false otherwise.
     *
     * \param   contextThread       The thread ID where the lock and wait happened. Not used for
     *                              waitable events.
     * \return  Returns true if the object is signaled; false otherwise.
     **/
    bool check_signaled( pthread_t contextThread ) const override;

    /**
     * \brief   Callback triggered when a waiting thread is released to continue. Waitable events
     *          always return true.
     *
     * \param   ownerThread     The POSIX thread ID that completed the wait.
     * \return  Always returns true.
     **/
    bool notify_request_ownership( pthread_t ownerThread ) override;

    /**
     * \brief   Returns true to indicate that the event can signal multiple threads simultaneously.
     **/
    bool can_signal_threads() const override;

    /**
     * \brief   Notifies the event of how many threads were released when the event was in signaled
     *          state.
     *
     * \param   numThreads      The number of threads released when the event was signaled. Zero
     *                          means no thread was released.
     **/
    void notify_released_threads( int32_t numThreads ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Specifies whether the event is manual- or auto-reset.
     **/
    const areg::os::ResetMode   mEventReset;
    /**
     * \brief   Flag that indicates the signaled state of the event.
     **/
    mutable bool                            mIsSignaled;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    WaitableEventPosix() = delete;
    AREG_NOCOPY_NOMOVE( WaitableEventPosix );
};

//////////////////////////////////////////////////////////////////////////
// WaitableEventPosix class inline implementation
//////////////////////////////////////////////////////////////////////////

inline areg::os::ResetMode WaitableEventPosix::reset_info() const
{
    ObjectLockPosix lock(*this);
    return mEventReset;
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_WAITABLEEVENTIX_HPP
