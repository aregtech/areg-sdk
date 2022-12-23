/************************************************************************
 * \file        pubservice/src/TrafficLightService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
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

Component * TrafficLightService::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW TrafficLightService(entry, owner);
}

void TrafficLightService::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

TrafficLightService::TrafficLightService(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component                 ( entry, owner )
    , SimpleTrafficLightStub    ( static_cast<Component &>(self()) )
    , IETimerConsumer           ( )
    , IETrafficSwitchConsumer   ( )

    , mTimer                    ( static_cast<IETimerConsumer &>(self()), "SimpleTrafficTimer")
    , mPrevState                ( NESimpleTrafficLight::eTrafficLight::LightOff )
{
    setSouthNorth(NESimpleTrafficLight::eTrafficLight::LightOff);
    setEastWest(NESimpleTrafficLight::eTrafficLight::LightOff);
}

void TrafficLightService::processEvent( const TrafficSwitchData & data )
{
    bool switchOn{ data.getData( ) };
    if ( switchOn )
    {
        if ( getSouthNorth( ) == NESimpleTrafficLight::eTrafficLight::LightOff )
        {
            setSouthNorth( NESimpleTrafficLight::eTrafficLight::LightYellow );
            setEastWest( NESimpleTrafficLight::eTrafficLight::LightYellow );
            mTimer.startTimer( NESimpleTrafficLight::TimeoutYellow, 1 );
        }
    }
    else
    {
        if ( getSouthNorth( ) != NESimpleTrafficLight::eTrafficLight::LightOff )
        {
            mTimer.stopTimer( );

            mPrevState = NESimpleTrafficLight::eTrafficLight::LightOff;
            setSouthNorth( NESimpleTrafficLight::eTrafficLight::LightOff );
            setEastWest( NESimpleTrafficLight::eTrafficLight::LightOff );
        }
    }
}

void TrafficLightService::processTimer(Timer & timer)
{
    if (&timer == &mTimer)
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
}

void TrafficLightService::startupServiceInterface(Component & holder)
{
    SimpleTrafficLightStub::startupServiceInterface(holder);
    TrafficSwitchEvent::addListener( static_cast<IETrafficSwitchConsumer &>(self()), holder.getMasterThread() );
}

void TrafficLightService::shutdownServiceIntrface(Component & holder)
{
    SimpleTrafficLightStub::shutdownServiceIntrface(holder);
    TrafficSwitchEvent::removeListener( static_cast<IETrafficSwitchConsumer &>(self()), holder.getMasterThread() );
}
