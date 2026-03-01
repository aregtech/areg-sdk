#pragma once

/************************************************************************
 * \file        pubservice/src/TrafficLightService.hpp
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
#include "examples/17_pubtraffic/services/SimpleTrafficLightStub.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/component/EventTemplate.hpp"

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
    inline bool data() const
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
AREG_DECLARE_EVENT(TrafficSwitchData, TrafficSwitchEvent, IETrafficSwitchConsumer);

//! \brief  Traffic light public service to demonstrate subscription on data update.
class TrafficLightService   : public    areg::Component
                            , protected SimpleTrafficLightStub
{
    friend class TrafficSwitchConsumer;
    friend class TrafficLightTimerConsumer;

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
        inline TrafficSwitchConsumer    ( TrafficLightService & service )
            : IETrafficSwitchConsumer   ( )
            , mService                  ( service )
            {
            }

        virtual ~TrafficSwitchConsumer() = default;

    //////////////////////////////////////////////////////////////////////////
    // Hidden methods.
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief  Override operation. Implement this function to receive events and make processing
         * \param  data    The data, which was passed as an event.
         **/
        void process_event( const TrafficSwitchData & data ) override;

    //////////////////////////////////////////////////////////////////////////
    // Hidden variables.
    //////////////////////////////////////////////////////////////////////////
        TrafficLightService &   mService;   //!< the instance of traffic lisght service

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls.
    //////////////////////////////////////////////////////////////////////////
        TrafficSwitchConsumer() = delete;
        AREG_NOCOPY_NOMOVE(TrafficSwitchConsumer);
    };

    //////////////////////////////////////////////////////////////////////////
    // TrafficLightService::TrafficLightTimerConsumer class declaration
    //////////////////////////////////////////////////////////////////////////
    //!< Traffic Light timer consumer object
    class TrafficLightTimerConsumer : public    areg::TimerConsumer
    {
    public:
        TrafficLightTimerConsumer( TrafficLightService & service )
            : areg::TimerConsumer   ( )
            , mService          ( service )
            {
            }

        virtual ~TrafficLightTimerConsumer() = default;

    //////////////////////////////////////////////////////////////////////////
    // Hidden methods.
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Automatically triggered when event is dispatched by thread.
         * \param   timer   The Timer Event Data object containing Timer object.
         **/
        void process_timer( areg::Timer & timer ) override;

    //////////////////////////////////////////////////////////////////////////
    // Hidden variables.
    //////////////////////////////////////////////////////////////////////////
        TrafficLightService &   mService;   //!< the instance of traffic lisght service

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls.
    //////////////////////////////////////////////////////////////////////////
        TrafficLightTimerConsumer() = delete;
        AREG_NOCOPY_NOMOVE(TrafficLightTimerConsumer);
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    TrafficLightService(const areg::ComponentEntry & entry, areg::ComponentThread & owner);

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
    void startup_service_interface( areg::Component & holder ) override;

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    void shutdown_service_interface ( areg::Component & holder ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    //!< Called when receive event that the traffic light is switched on.
    void onTrafficLightSwitchedOn();

    //!< Called when receive event that the traffic light is switched off.
    void onTrafficLightSwitchedOff();

    //!< Automatically triggered when event is dispatched by thread.
    void onTimerExpired();

    inline TrafficLightService & self()
    {
        return (*this);
    }

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:

    areg::Timer                               mTimer;         //!< The timer to switch lights

    SimpleTrafficLight::TrafficLight mPrevState;     //!< Previous state for yellow light switch

    TrafficSwitchConsumer               mEventConsumer; //!< The event consumer object.

    TrafficLightTimerConsumer           mTimerConsumer; //!< The timer consumer object.
    
//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
    TrafficLightService() = delete;
    AREG_NOCOPY_NOMOVE( TrafficLightService );
};
