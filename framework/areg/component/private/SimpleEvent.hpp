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
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
 *
 *          The object holds no operating system handle: its whole state is one atomic word. A
 *          wait that finds the event already signaled, and a signal nobody waits for, are
 *          answered in user space. Only a thread that has to go to sleep enters the kernel,
 *          where it parks on that word with futex on Linux, __ulock on macOS and WaitOnAddress
 *          on Windows. A signal that arrives before a waiter is asleep is never lost, because
 *          the wait call compares the word once more inside the kernel.
 *
 * \note    On Windows this needs Windows 8 or newer. The wait and wake calls are imported from
 *          api-ms-win-core-synch-l1-2-0.dll through Synchronization.lib.
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
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    std::atomic<uint32_t>   mState;     //!< Wait word: 0 == non-signaled, 1 == signaled.
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER
    const bool              mValid;     //!< false => hollow NullTag object (all ops no-op).
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
