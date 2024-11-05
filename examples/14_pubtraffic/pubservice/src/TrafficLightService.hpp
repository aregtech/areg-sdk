#pragma once

/************************************************************************
 * \file        pubservice/src/TrafficLightService.hpp
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
#include "generate/examples/14_pubtraffic/SimpleTrafficLightStub.hpp"
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
{
    friend class TrafficSwitchConsumer;
    friend class TimerConsumer;

//////////////////////////////////////////////////////////////////////////
// Internal classes.
//////////////////////////////////////////////////////////////////////////
private:
    //////////////////////////////////////////////////////////////////////////
    // TrafficLightService::TrafficSwitchConsumer class declaration
    //////////////////////////////////////////////////////////////////////////
    //!< TrafficSwitchEvent consumer object declared as a private internal class
    class TrafficSwitchConsumer : public IETrafficSwitchConsumer
    {
    public:
        inline TrafficSwitchConsumer( TrafficLightService & service )
            : IETrafficSwitchConsumer   ( )
            , mService                  ( service )
            {
            }

        virtual ~TrafficSwitchConsumer( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Hidden methods.
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief  Override operation. Implement this function to receive events and make processing
         * \param  data    The data, which was passed as an event.
         **/
        virtual void processEvent( const TrafficSwitchData & data ) override;

    //////////////////////////////////////////////////////////////////////////
    // Hidden variables.
    //////////////////////////////////////////////////////////////////////////
        TrafficLightService &   mService;   //!< the instance of traffic lisght service

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls.
    //////////////////////////////////////////////////////////////////////////
        TrafficSwitchConsumer( void ) = delete;
        DECLARE_NOCOPY_NOMOVE(TrafficSwitchConsumer);
    };

    //////////////////////////////////////////////////////////////////////////
    // TrafficLightService::TimerConsumer class declaration
    //////////////////////////////////////////////////////////////////////////
    //!< Traffic Light timer consumer object
    class TimerConsumer : public    IETimerConsumer
    {
    public:
        TimerConsumer( TrafficLightService & service )
            : IETimerConsumer   ( )
            , mService          ( service )
            {
            }

        virtual ~TimerConsumer( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Hidden methods.
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Automatically triggered when event is dispatched by thread.
         * \param   timer   The Timer Event Data object containing Timer object.
         **/
        virtual void processTimer( Timer & timer ) override;

    //////////////////////////////////////////////////////////////////////////
    // Hidden variables.
    //////////////////////////////////////////////////////////////////////////
        TrafficLightService &   mService;   //!< the instance of traffic lisght service

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls.
    //////////////////////////////////////////////////////////////////////////
        TimerConsumer( void ) = delete;
        DECLARE_NOCOPY_NOMOVE(TimerConsumer);
    };

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
    //!< Called when receive event that the traffic light is switched on.
    void onTrafficLightSwitchedOn( void );

    //!< Called when receive event that the traffic light is switched off.
    void onTrafficLightSwitchedOff( void );

    //!< Automatically triggered when event is dispatched by thread.
    void onTimerExpired( void );

    inline TrafficLightService & self( void )
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:

    Timer                               mTimer;         //!< The timer to switch lights

    NESimpleTrafficLight::eTrafficLight mPrevState;     //!< Previous state for yellow light switch

    TrafficSwitchConsumer               mEventConsumer; //!< The event consumer object.

    TimerConsumer                       mTimerConsumer; //!< The timer consumer object.
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
    TrafficLightService( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( TrafficLightService );
};
