#pragma once

/************************************************************************
 * \file        locservice/src/ServicingComponent.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "examples/21_locwatchdog/services/HelloWatchdogStub.hpp"

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A demo of simple servicing component, which receives request to
            sleep. If sleeping time is bigger than the watchdog timeout, the
            system terminates the thread and restarts again.
 **/
class ServicingComponent    : public    Component
                            , protected HelloWatchdogStub
{
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

//////////////////////////////////////////////////////////////////////////
// HelloWorld Interface Requests
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Request call.
     *          The response triggered when the thread resumed from suspended mode.
     * \param   timeoutSleep    The timeout in milliseconds to suspend the thread.
     * \see     responseStartSleep
     **/
    virtual void requestStartSleep( unsigned int timeoutSleep ) override;

/************************************************************************/
// StubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    virtual void startupServiceInterface( Component & holder ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline ServicingComponent & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ServicingComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServicingComponent );
};

//////////////////////////////////////////////////////////////////////////
// ServicingComponent inline methods
//////////////////////////////////////////////////////////////////////////
inline ServicingComponent & ServicingComponent::self( void )
{   return (*this);     }
