#pragma once

/************************************************************************
 * \file        pubservice/src/TrafficLightService.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
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
#include "generated/src/SimpleTrafficLightStub.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "areg/component/TEEvent.hpp"

#include "areg/component/Timer.hpp"

//! \brief  The traffic light switching event data.
class TrafficSwitchData
{
public:
    inline TrafficSwitchData( bool switchOn = true )
        : mSwitchOn( switchOn )
    {
    }

    //! \brief  Returns the data value.
    inline bool getData( void ) const
    {
        return mSwitchOn;
    }

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    bool    mSwitchOn;  //!< ON and OFF state of traffic light.
};

// declaration of custom event.
DECLARE_EVENT(TrafficSwitchData, TrafficSwitchEvent, IETrafficSwitchConsumer);

//! \brief  Traffic light public service to demonstrate subscription on data update.
class TrafficLightService   : public    Component
                            , protected SimpleTrafficLightStub
                            , protected IETimerConsumer
                            , protected IETrafficSwitchConsumer
{
//////////////////////////////////////////////////////////////////////////
// Statics and constants.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Private constructor, called from CreateComponent static method.
     * \param   entry   The component entry that contains role name, provided and dependent services.
     * \param   owner   The component owning thread.
     **/
    TrafficLightService(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   The destructor.
     **/
    virtual ~TrafficLightService( void ) = default;

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitch Interface Requests
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief  Override operation. Implement this function to receive events and make processing
     * \param  data    The data, which was passed as an event.
     **/
    virtual void processEvent( const TrafficSwitchData & data ) override;

    /**
     * \brief   Automatically triggered when event is dispatched by thread.
     * \param   data    The Timer Event Data object containing Timer object.
     **/
    virtual void processTimer( Timer & timer ) override;

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

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void shutdownServiceIntrface ( Component & holder ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    inline TrafficLightService & self( void )
    {
        return (*this);
    }

    Timer   mTimer; //! The timer to switch lights

    NESimpleTrafficLight::eTrafficLight mPrevState; //! Previous state for yellow light switch
//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
    TrafficLightService( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( TrafficLightService );
};
