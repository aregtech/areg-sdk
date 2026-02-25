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

TrafficLightService::TrafficLightService(const areg::ComponentEntry & entry, ComponentThread & owner)
    : Component                 ( entry, owner )
    , SimpleTrafficLightStub    ( static_cast<Component &>(self()) )

    , mTimer                    ( static_cast<areg::TimerConsumer &>(mTimerConsumer), "SimpleTrafficTimer")
    , mPrevState                ( SimpleTrafficLight::TrafficLight::LightOff )
    , mEventConsumer            ( self() )
    , mTimerConsumer            ( self() )
{
    setSouthNorth(SimpleTrafficLight::TrafficLight::LightOff);
    setEastWest(SimpleTrafficLight::TrafficLight::LightOff);
}

void TrafficLightService::onTrafficLightSwitchedOn()
{
    if ( getSouthNorth( ) == SimpleTrafficLight::TrafficLight::LightOff )
    {
        setSouthNorth( SimpleTrafficLight::TrafficLight::LightYellow );
        setEastWest( SimpleTrafficLight::TrafficLight::LightYellow );
        mTimer.startTimer( SimpleTrafficLight::TimeoutYellow, 1 );
    }
}

void TrafficLightService::onTrafficLightSwitchedOff()
{
    if ( getSouthNorth( ) != SimpleTrafficLight::TrafficLight::LightOff )
    {
        mTimer.stopTimer( );

        mPrevState = SimpleTrafficLight::TrafficLight::LightOff;
        setSouthNorth( SimpleTrafficLight::TrafficLight::LightOff );
        setEastWest( SimpleTrafficLight::TrafficLight::LightOff );
    }
}

void TrafficLightService::onTimerExpired()
{
    switch (getSouthNorth())
    {
    case SimpleTrafficLight::TrafficLight::LightRed:
        mPrevState  = SimpleTrafficLight::TrafficLight::LightRed;
        setSouthNorth(SimpleTrafficLight::TrafficLight::LightYellow);
        setEastWest(SimpleTrafficLight::TrafficLight::LightYellow);
        mTimer.startTimer(SimpleTrafficLight::TimeoutYellow, 1);
        break;

    case SimpleTrafficLight::TrafficLight::LightYellow:
        if ((mPrevState == SimpleTrafficLight::TrafficLight::LightRed) || (mPrevState == SimpleTrafficLight::TrafficLight::LightOff))
        {
            setSouthNorth(SimpleTrafficLight::TrafficLight::LightGreen);
            setEastWest(SimpleTrafficLight::TrafficLight::LightRed);
            mTimer.startTimer(SimpleTrafficLight::TimeoutGreen);
        }
        else
        {
            setSouthNorth(SimpleTrafficLight::TrafficLight::LightRed);
            setEastWest(SimpleTrafficLight::TrafficLight::LightGreen);
            mTimer.startTimer(SimpleTrafficLight::TimeoutRed);
        }

        mPrevState  = SimpleTrafficLight::TrafficLight::LightYellow;
        break;

    case SimpleTrafficLight::TrafficLight::LightGreen:
        mPrevState  = SimpleTrafficLight::TrafficLight::LightGreen;
        setSouthNorth(SimpleTrafficLight::TrafficLight::LightYellow);
        setEastWest(SimpleTrafficLight::TrafficLight::LightYellow);
        mTimer.startTimer(SimpleTrafficLight::TimeoutYellow, 1);
        break;

    case SimpleTrafficLight::TrafficLight::LightOff:
    default:
        mPrevState  = SimpleTrafficLight::TrafficLight::LightOff;
        setSouthNorth(SimpleTrafficLight::TrafficLight::LightOff);
        setEastWest(SimpleTrafficLight::TrafficLight::LightOff);
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
