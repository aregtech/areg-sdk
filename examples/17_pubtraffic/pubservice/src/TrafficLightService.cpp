/************************************************************************
 * \file        pubservice/src/TrafficLightService.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
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
// TrafficLightService::TrafficLightTimerConsumer class implementation
//////////////////////////////////////////////////////////////////////////
void TrafficLightService::TrafficLightTimerConsumer::processTimer( Timer & timer )
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

    , mTimer                    ( static_cast<TimerConsumer &>(mTimerConsumer), "SimpleTrafficTimer")
    , mPrevState                ( SimpleTrafficLight::eTrafficLight::LightOff )
    , mEventConsumer            ( self() )
    , mTimerConsumer            ( self() )
{
    setSouthNorth(SimpleTrafficLight::eTrafficLight::LightOff);
    setEastWest(SimpleTrafficLight::eTrafficLight::LightOff);
}

void TrafficLightService::onTrafficLightSwitchedOn()
{
    if ( getSouthNorth( ) == SimpleTrafficLight::eTrafficLight::LightOff )
    {
        setSouthNorth( SimpleTrafficLight::eTrafficLight::LightYellow );
        setEastWest( SimpleTrafficLight::eTrafficLight::LightYellow );
        mTimer.startTimer( SimpleTrafficLight::TimeoutYellow, 1 );
    }
}

void TrafficLightService::onTrafficLightSwitchedOff()
{
    if ( getSouthNorth( ) != SimpleTrafficLight::eTrafficLight::LightOff )
    {
        mTimer.stopTimer( );

        mPrevState = SimpleTrafficLight::eTrafficLight::LightOff;
        setSouthNorth( SimpleTrafficLight::eTrafficLight::LightOff );
        setEastWest( SimpleTrafficLight::eTrafficLight::LightOff );
    }
}

void TrafficLightService::onTimerExpired()
{
    switch (getSouthNorth())
    {
    case SimpleTrafficLight::eTrafficLight::LightRed:
        mPrevState  = SimpleTrafficLight::eTrafficLight::LightRed;
        setSouthNorth(SimpleTrafficLight::eTrafficLight::LightYellow);
        setEastWest(SimpleTrafficLight::eTrafficLight::LightYellow);
        mTimer.startTimer(SimpleTrafficLight::TimeoutYellow, 1);
        break;

    case SimpleTrafficLight::eTrafficLight::LightYellow:
        if ((mPrevState == SimpleTrafficLight::eTrafficLight::LightRed) || (mPrevState == SimpleTrafficLight::eTrafficLight::LightOff))
        {
            setSouthNorth(SimpleTrafficLight::eTrafficLight::LightGreen);
            setEastWest(SimpleTrafficLight::eTrafficLight::LightRed);
            mTimer.startTimer(SimpleTrafficLight::TimeoutGreen);
        }
        else
        {
            setSouthNorth(SimpleTrafficLight::eTrafficLight::LightRed);
            setEastWest(SimpleTrafficLight::eTrafficLight::LightGreen);
            mTimer.startTimer(SimpleTrafficLight::TimeoutRed);
        }

        mPrevState  = SimpleTrafficLight::eTrafficLight::LightYellow;
        break;

    case SimpleTrafficLight::eTrafficLight::LightGreen:
        mPrevState  = SimpleTrafficLight::eTrafficLight::LightGreen;
        setSouthNorth(SimpleTrafficLight::eTrafficLight::LightYellow);
        setEastWest(SimpleTrafficLight::eTrafficLight::LightYellow);
        mTimer.startTimer(SimpleTrafficLight::TimeoutYellow, 1);
        break;

    case SimpleTrafficLight::eTrafficLight::LightOff:
    default:
        mPrevState  = SimpleTrafficLight::eTrafficLight::LightOff;
        setSouthNorth(SimpleTrafficLight::eTrafficLight::LightOff);
        setEastWest(SimpleTrafficLight::eTrafficLight::LightOff);
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
