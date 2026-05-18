#pragma once

/************************************************************************
 * \file        locservice/src/ServicingComponent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "examples/22_pubwatchdog/services/HelloWatchdogProviderBase.hpp"

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A demo of simple servicing component, which receives request to
            sleep. If sleeping time is bigger than the watchdog timeout, the
            system terminates the thread and restarts again.
 **/
class ServicingComponent final  : public    areg::Component
                                , protected HelloWatchdogProviderBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServicingComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner);

//////////////////////////////////////////////////////////////////////////
// HelloWorld Interface Requests
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Request call.
     *          The response triggered when the thread resumed from suspended mode.
     * \param   timeoutSleep    The timeout in milliseconds to suspend the thread.
     * \see     request_start_sleep
     **/
    void request_start_sleep( uint32_t timeoutSleep ) final;

    /**
     * \brief   Request call.
     *          Called to stop the service.
     * \note    Has no response
     **/
    void request_stop_service() final;

    /**
     * \brief   Request call.
     *          Called to shutdown service and exit application.
     * \note    Has no response
     **/
    void request_shutdown_service() final;

/************************************************************************/
// ProviderBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Provider,
     *                  which started up.
     **/
    void startup_service_interface( areg::Component & holder ) final;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline ServicingComponent & self();

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ServicingComponent() = delete;
    AREG_NOCOPY_NOMOVE( ServicingComponent );
};

//////////////////////////////////////////////////////////////////////////
// ServicingComponent inline methods
//////////////////////////////////////////////////////////////////////////
inline ServicingComponent & ServicingComponent::self()
{   return (*this);     }
