#pragma once

/************************************************************************
 * \file        src/ServicingComponent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/component/StubBase.hpp"
#include "areg/component/TimerConsumer.hpp"

#include "areg/component/Timer.hpp"

//! \brief   A demo of simple servicing component with timer without component servicing (request) methods.
class ServicingComponent final  : public    areg::Component
                                , protected areg::StubBase
                                , protected areg::TimerConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////
private:
    static const uint32_t   TIMER_TIMEOUT   { areg::TIMEOUT_100_MS };   //!< Timer timeout in milliseconds
    static const int32_t            TIMER_EVENTS    { 10 };   					    //!< Fired timer count.

public:
    //!< Init component object.
    ServicingComponent(const areg::ComponentEntry & entry, areg::ComponentThread & ownerThread);

protected:
/************************************************************************/
// StubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     * \param   holder  The holder component of service interface of Stub.
     **/
    void startup_service_interface( areg::Component & holder ) override;

    /**
     * \brief   This function is triggered by Component when shuts down.
     * \param   holder  The holder component of service interface of Stub.
     **/
    void shutdown_service_interface ( areg::Component & holder ) noexcept override;

/************************************************************************/
// TimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired.
     * \param   timer   The timer object that is expired.
     **/
    void process_timer( areg::Timer & timer ) override;

//////////////////////////////////////////////////////////////////////////
// These methods must exist, but can have empty body
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// StubBase overrides. Public pure virtual methods
/************************************************************************/

    /**
     * \brief   Sends update notification message to all clients.
     **/
    void send_notification( uint32_t /*msgId*/ ) override
    {
    }

    /**
     * \brief   Sends error message to clients.
     **/
    void error_request( uint32_t /*msgId*/, bool /*msgCancel*/ ) override
    {
    }

/************************************************************************/
// StubEventConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered to process service request event.
     **/
    void process_request_event( areg::ServiceRequestEvent & /*eventElem*/ ) override
    {
    }

    /**
     * \brief   Triggered to process attribute update notification event.
     **/
    void process_attribute_event( areg::ServiceRequestEvent & /*eventElem*/ ) override
    {
    }

private:
    areg::Timer   mTimer; //!< The timer to run in component thread.
    int32_t     mCount; //!< The timer event count.

private:
    inline ServicingComponent & self()
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ServicingComponent() = delete;
    AREG_NOCOPY_NOMOVE( ServicingComponent );
};
