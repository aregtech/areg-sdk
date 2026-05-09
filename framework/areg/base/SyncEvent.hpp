#ifndef AREG_BASE_SYNCEVENT_HPP
#define AREG_BASE_SYNCEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SyncEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects
 *              Declared following synchronization objects:
 *              SyncEvent           - Event synchronization object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SyncObject.hpp"

namespace areg {
//////////////////////////////////////////////////////////////////////////
// class SyncEvent declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Synchronization event object with manual-reset or auto-reset modes. Manual-reset events
 *          remain signaled until explicitly reset. Auto-reset events automatically reset after a
 *          single waiting thread is released. Used to signal between threads that a particular
 *          event has occurred.
 **/
class AREG_API SyncEvent final  : public SyncObject
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Creates a manual-reset or auto-reset synchronization event with an initial state.
     *
     * \param   initLock        If true, the event is initially non-signaled (locked). If false, the
     *                          event is initially signaled. Default is true (non-signaled).
     * \param   autoReset       If true, creates an auto-reset event; otherwise, creates a
     *                          manual-reset event. Default is true (auto-reset).
     **/
    explicit SyncEvent ( bool initLock = true, bool autoReset = true );

    virtual ~SyncEvent();

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Waits for the event to be signaled. If already signaled, returns immediately. If
     *          auto-reset, the event is automatically set to non-signaled after this call.
     *
     * \param   timeout     The timeout in milliseconds to wait for the event to be signaled. Use
     *                      areg::WAIT_INFINITE for indefinite wait. Use areg::DO_NOT_WAIT
     *                      for non-blocking check.
     * \return  Returns true if the event was signaled before or during the timeout; false if
     *          timeout expired.
     **/
    inline bool lock( uint32_t timeout = areg::WAIT_INFINITE ) final;

    /**
     * \brief   Sets the event to signaled state.
     *
     * \return  Returns true if successfully set the event to signaled.
     **/
    inline bool unlock() final;

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets the event to signaled state. Equivalent to calling unlock().
     * \return  Returns true if operation succeeded.
     **/
    inline bool set_signaled() noexcept;

    /**
     * \brief   Resets the event to non-signaled state. Only manual-reset events can be manually reset.
     * \return  Returns true if operation succeeded.
     **/
    inline bool reset() noexcept;

    /**
     * \brief   Pulses the event: briefly signals it and immediately resets to non-signaled,
     *          releasing waiting threads.
     **/
    inline void pulse_event() noexcept;

    /**
     * \brief   Returns true if the event is auto-reset; false if manual-reset.
     **/
    [[nodiscard]]
    inline bool is_auto_reset() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden or OS specific implementations
//////////////////////////////////////////////////////////////////////////
private:
    
    /**
     * \brief   OS-specific implementation to create an event. The event is non-signaled if initLock
     *          is true; signaled otherwise.
     *
     * \param   initLock    If true, the event is created in non-signaled state. If false, the event
     *                      is created in signaled state.
     **/
    void _os_create_event( bool initLock );

    /**
     * \brief   OS-specific implementation to signal an event and release waiting threads.
     *
     * \param   eventHandle     The handle of the event object to signal.
     * \return  Returns true if operation succeeded.
     **/
    bool _os_unlock_event( void * eventHandle ) noexcept;

    /**
     * \brief   OS-specific implementation to wait for an event to be signaled.
     *
     * \param   timeout     The timeout in milliseconds to wait for the event.
     * \return  Returns true if the event was signaled during the timeout.
     **/
    bool _os_lock_event( uint32_t timeout );

    /**
     * \brief   OS-specific implementation to set the event to signaled state.
     *
     * \return  Returns true if operation succeeded.
     **/
    bool _os_set_event() noexcept;

    /**
     * \brief   OS-specific implementation to reset the event to non-signaled state.
     *
     * \return  Returns true if operation succeeded.
     **/
    bool _os_reset_event() noexcept;

    /**
     * \brief   OS-specific implementation to pulse the event: briefly signal it and immediately
     *          reset to non-signaled.
     **/
    void _os_pulse_event() noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Flag, indicating whether event is auto-reset or not
     **/
    const bool  mAutoReset;

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SyncEvent );
};

//////////////////////////////////////////////////////////////////////////
// SyncEvent class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool SyncEvent::is_auto_reset() const noexcept
{
    return mAutoReset;
}

inline bool SyncEvent::unlock()
{
    return (mSyncObject != nullptr ? _os_unlock_event(mSyncObject) : false);
}

inline bool SyncEvent::lock( uint32_t timeout /* = areg::WAIT_INFINITE */ )
{
    return (mSyncObject != nullptr ? _os_lock_event(timeout) : false);
}

inline bool SyncEvent::set_signaled() noexcept
{
    return (mSyncObject != nullptr ? _os_set_event( ) : false);
}

inline bool SyncEvent::reset() noexcept
{
    return (mSyncObject != nullptr ? _os_reset_event( ) : false);
}

inline void SyncEvent::pulse_event() noexcept
{
    if (mSyncObject != nullptr)
    {
        _os_pulse_event();
    }
}
} // namespace areq

#endif // AREG_BASE_SYNCEVENT_HPP

