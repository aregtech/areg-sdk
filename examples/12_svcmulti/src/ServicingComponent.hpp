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

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "areg/component/StubBase.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/component/Timer.hpp"

//! \brief   A demo of simple servicing component with timer without component servicing (request) methods.
class ServicingComponent    : public    Component
                            , protected StubBase
                            , protected IETimerConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////
private:
    static const unsigned int   TIMER_TIMEOUT   { NECommon::TIMEOUT_100_MS };   //!< Timer timeout in milliseconds
    static const int            TIMER_EVENTS    { 30 };   					    //!< Fired timer count.

public:

    //!< Init component
    ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & ownerThread);

protected:
/************************************************************************/
// StubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     * \param   holder  The holder component of service interface of Stub.
     **/
    virtual void startupServiceInterface( Component & holder ) override;

    /**
     * \brief   This function is triggered by Component when shuts down.
     * \param   holder  The holder component of service interface of Stub.
     **/
    virtual void shutdownServiceInterface ( Component & holder ) override;

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer ) override;

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
    virtual void sendNotification( unsigned int /*msgId*/ ) override
    {
    }

    /**
     * \brief   Sends error message to clients.
     **/
    virtual void errorRequest( unsigned int /*msgId*/, bool /*msgCancel*/ ) override
    {
    }

/************************************************************************/
// IEStubEventConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered to process service request event.
     **/
    virtual void processRequestEvent( ServiceRequestEvent & /*eventElem*/ ) override
    {
    }

    /**
     * \brief   Triggered to process attribute update notification event.
     **/
    virtual void processAttributeEvent( ServiceRequestEvent & /*eventElem*/ ) override
    {
    }

private:
    Timer   mTimer; //!< The timer to run in component thread.
    int     mCount; //!< The timer event count.

private:
    inline ServicingComponent & self( void )
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ServicingComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServicingComponent );
};
