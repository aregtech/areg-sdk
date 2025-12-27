/************************************************************************
 * \file        pubservice/src/TrafficLightService.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "pubservice/src/TrafficLightService.hpp"
#include "areg/component/ComponentThread.hpp"

//////////////////////////////////////////////////////////////////////////
// TrafficLightService::TrafficSwitchConsumer class implementation
//////////////////////////////////////////////////////////////////////////

void TrafficLightService::TrafficSwitchConsumer::processEvent(const TrafficSwitchData &data)
{
    if (data.getData( ))
    {
        mService.onTrafficLightSwitchedOn();
    }
    else
    {
        mService.onTrafficLightSwitchedOff();
    }
}

//////////////////////////////////////////////////////////////////////////
// TrafficLightService::TimerConsumer class implementation
//////////////////////////////////////////////////////////////////////////
void TrafficLightService::TimerConsumer::processTimer( Timer & timer )
{
    if (&timer == &mService.mTimer)
    {
        mService.onTimerExpired();
    }
}

//////////////////////////////////////////////////////////////////////////
// TrafficLightService class implementation
//////////////////////////////////////////////////////////////////////////

TrafficLightService::TrafficLightService(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component                 ( entry, owner )
    , SimpleTrafficLightStub    ( static_cast<Component &>(self()) )

    , mTimer                    ( static_cast<IETimerConsumer &>(mTimerConsumer), "SimpleTrafficTimer")
    , mPrevState                ( NESimpleTrafficLight::eTrafficLight::LightOff )
    , mEventConsumer            ( self() )
    , mTimerConsumer            ( self() )
{
    setSouthNorth(NESimpleTrafficLight::eTrafficLight::LightOff);
    setEastWest(NESimpleTrafficLight::eTrafficLight::LightOff);
}

void TrafficLightService::onTrafficLightSwitchedOn( void )
{
    if ( getSouthNorth( ) == NESimpleTrafficLight::eTrafficLight::LightOff )
    {
        setSouthNorth( NESimpleTrafficLight::eTrafficLight::LightYellow );
        setEastWest( NESimpleTrafficLight::eTrafficLight::LightYellow );
        mTimer.startTimer( NESimpleTrafficLight::TimeoutYellow, 1 );
    }
}

void TrafficLightService::onTrafficLightSwitchedOff( void )
{
    if ( getSouthNorth( ) != NESimpleTrafficLight::eTrafficLight::LightOff )
    {
        mTimer.stopTimer( );

        mPrevState = NESimpleTrafficLight::eTrafficLight::LightOff;
        setSouthNorth( NESimpleTrafficLight::eTrafficLight::LightOff );
        setEastWest( NESimpleTrafficLight::eTrafficLight::LightOff );
    }
}

void TrafficLightService::onTimerExpired( void )
{
    switch (getSouthNorth())
    {
    case NESimpleTrafficLight::eTrafficLight::LightRed:
        mPrevState  = NESimpleTrafficLight::eTrafficLight::LightRed;
        setSouthNorth(NESimpleTrafficLight::eTrafficLight::LightYellow);
        setEastWest(NESimpleTrafficLight::eTrafficLight::LightYellow);
        mTimer.startTimer(NESimpleTrafficLight::TimeoutYellow, 1);
        break;

    case NESimpleTrafficLight::eTrafficLight::LightYellow:
        if ((mPrevState == NESimpleTrafficLight::eTrafficLight::LightRed) || (mPrevState == NESimpleTrafficLight::eTrafficLight::LightOff))
        {
            setSouthNorth(NESimpleTrafficLight::eTrafficLight::LightGreen);
            setEastWest(NESimpleTrafficLight::eTrafficLight::LightRed);
            mTimer.startTimer(NESimpleTrafficLight::TimeoutGreen);
        }
        else
        {
            setSouthNorth(NESimpleTrafficLight::eTrafficLight::LightRed);
            setEastWest(NESimpleTrafficLight::eTrafficLight::LightGreen);
            mTimer.startTimer(NESimpleTrafficLight::TimeoutRed);
        }

        mPrevState  = NESimpleTrafficLight::eTrafficLight::LightYellow;
        break;

    case NESimpleTrafficLight::eTrafficLight::LightGreen:
        mPrevState  = NESimpleTrafficLight::eTrafficLight::LightGreen;
        setSouthNorth(NESimpleTrafficLight::eTrafficLight::LightYellow);
        setEastWest(NESimpleTrafficLight::eTrafficLight::LightYellow);
        mTimer.startTimer(NESimpleTrafficLight::TimeoutYellow, 1);
        break;

    case NESimpleTrafficLight::eTrafficLight::LightOff:
    default:
        mPrevState  = NESimpleTrafficLight::eTrafficLight::LightOff;
        setSouthNorth(NESimpleTrafficLight::eTrafficLight::LightOff);
        setEastWest(NESimpleTrafficLight::eTrafficLight::LightOff);
        break;
    }
}

void TrafficLightService::startupServiceInterface(Component & holder)
{
    SimpleTrafficLightStub::startupServiceInterface(holder);
    TrafficSwitchEvent::addListener( static_cast<IETrafficSwitchConsumer &>(mEventConsumer), holder.getMasterThread() );
}

void TrafficLightService::shutdownServiceInterface(Component & holder)
{
    SimpleTrafficLightStub::shutdownServiceInterface(holder);
    TrafficSwitchEvent::removeListener( static_cast<IETrafficSwitchConsumer &>(mEventConsumer), holder.getMasterThread() );
}
