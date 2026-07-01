#ifndef AREG_COMPONENT_PRIVATE_SIMPLEEVENT_HPP
#define AREG_COMPONENT_PRIVATE_SIMPLEEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/SimpleEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, lightweight single-object event (queue doorbell).
 ************************************************************************/
/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/CommonDefs.hpp"

#include <atomic>
#include <cstdint>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// class SimpleEvent declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Lightweight event for internal single-object wait points such as the
 *          EventQueue producer / consumer doorbells.
 *
 *          Unlike SyncEvent, SimpleEvent is NOT a SyncObject: it cannot be used
 *          with MultiLock or wait_any() (the WaitForMultipleObjects pattern).
 *
 *          Reset modes mirror SyncEvent:
 *            - auto-reset   : a single waiter is released per signal and the
 *                             event returns to non-signaled when consumed.
 *            - manual-reset : the event stays signaled until reset() is called.
 *
 *          SimpleEvent supports exactly one logical wait point per object.
 *          Auto-reset still tolerates several parked waiters (one is released
 *          per signal); callers that race for the same resource must re-check their
 *          own condition after lock() returns.
 **/
class AREG_API SimpleEvent
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a manual-reset or auto-reset event with an initial state.
     *
     * \param   initLock    If true, the event is initially non-signaled (locked). If false, the
     *                      event is initially signaled. Default is true (non-signaled).
     * \param   autoReset   If true, creates an auto-reset event; otherwise a manual-reset event.
     *                      Default is true (auto-reset).
     **/
    explicit SimpleEvent( bool initLock = true, bool autoReset = true ) noexcept;

    /**
     * \brief   Null constructor: creates a hollow SimpleEvent with no OS resource. All operations
     *          return false / no-op. Intended for zero-allocation sentinel objects.
     **/
    explicit SimpleEvent( areg::NullTag ) noexcept;

    ~SimpleEvent();

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Waits for the event to be signaled. If already signaled, returns immediately.
     *          If auto-reset, the event is set to non-signaled after this call consumes the signal.
     *
     * \param   timeout     The timeout in milliseconds to wait. Use areg::WAIT_INFINITE for an
     *                      indefinite wait, or areg::DO_NOT_WAIT for a non-blocking check.
     * \return  Returns true if the event was signaled before or during the timeout; false otherwise.
     **/
    bool lock( uint32_t timeout = areg::WAIT_INFINITE ) noexcept;

    /**
     * \brief   Sets the event to signaled state, releasing a parked waiter (auto-reset) or all
     *          parked waiters (manual-reset).
     * \return  Returns true if operation succeeded.
     **/
    bool set_signaled() noexcept;

    /**
     * \brief   Resets the event to non-signaled state.
     * \return  Returns true if operation succeeded.
     **/
    bool reset() noexcept;

    /**
     * \brief   Returns true if the event is auto-reset; false if manual-reset.
     **/
    [[nodiscard]]
    inline bool is_auto_reset() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
#if defined(_WIN32) && !defined(__CYGWIN__)
    void *                  mHandle;    //!< Native auto / manual-reset event HANDLE (nullptr => hollow).
#else
    std::atomic<uint32_t>   mState;     //!< Futex word: 0 == non-signaled, 1 == signaled.
    const bool              mValid;     //!< false => hollow NullTag object (all ops no-op).
#endif  // defined(_WIN32) && !defined(__CYGWIN__)

    const bool              mAutoReset; //!< true => auto-reset, false => manual-reset.

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SimpleEvent );
};

//////////////////////////////////////////////////////////////////////////
// SimpleEvent inline methods
//////////////////////////////////////////////////////////////////////////
inline bool SimpleEvent::is_auto_reset() const noexcept
{
    return mAutoReset;
}

}   // namespace areg

#endif  // AREG_COMPONENT_PRIVATE_SIMPLEEVENT_HPP
