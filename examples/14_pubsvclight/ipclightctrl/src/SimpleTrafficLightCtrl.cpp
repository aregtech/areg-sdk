/************************************************************************
 * \file        ipclightctrl/src/SimpleTrafficLightCtrl.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "ipclightctrl/src/SimpleTrafficLightCtrl.hpp"
#include "areg/component/ComponentThread.hpp"

Component * SimpleTrafficLightCtrl::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW SimpleTrafficLightCtrl(entry, owner);
}

void SimpleTrafficLightCtrl::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

SimpleTrafficLightCtrl::SimpleTrafficLightCtrl(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component                 ( owner, entry.mRoleName )
    , SimpleTrafficLightStub    ( static_cast<Component &>(self()) )
    , SimpleTrafficSwitchStub   ( static_cast<Component &>(self()) )
    , IETimerConsumer           ( )
    , IETrafficSwitchConsumer   ( )

    , mTimer                    ( static_cast<IETimerConsumer &>(self()), "SimpleTrafficTimer")
    , mPrevState                ( NESimpleTrafficLight::LightOff )
{
    setSouthNorth(NESimpleTrafficLight::LightOff);
    setEastWest(NESimpleTrafficLight::LightOff);
    setIsSwitchedOn(false);
}


SimpleTrafficLightCtrl::~SimpleTrafficLightCtrl(void)
{

}

void SimpleTrafficLightCtrl::requestSwitchLight( bool switchOn )
{
    if (switchOn)
    {
        if (getSouthNorth() == NESimpleTrafficLight::LightOff)
        {
            setSouthNorth(NESimpleTrafficLight::LightYellow);
            setEastWest(NESimpleTrafficLight::LightYellow);
            broadcastLightChanged(getSouthNorth(), getEastWest());
            mTimer.startTimer(NESimpleTrafficLight::TimeoutYellow, 1);
        }
    }
    else
    {
        if (getSouthNorth() != NESimpleTrafficLight::LightOff)
        {
            mTimer.stopTimer();

            broadcastLightChanged(NESimpleTrafficLight::LightYellow, NESimpleTrafficLight::LightYellow);
            mPrevState = NESimpleTrafficLight::LightOff;
            setSouthNorth(NESimpleTrafficLight::LightOff);
            setEastWest(NESimpleTrafficLight::LightOff);
            broadcastLightChanged(NESimpleTrafficLight::LightOff, NESimpleTrafficLight::LightOff);
        }
    }
}

void SimpleTrafficLightCtrl::processEvent( const TrafficSwitchData & data )
{
    requestSwitchLight(data.getData());
}

void SimpleTrafficLightCtrl::processTimer(Timer & timer)
{
    if (&timer == &mTimer)
    {
        switch (getSouthNorth())
        {
        case NESimpleTrafficLight::LightRed:
            mPrevState  = NESimpleTrafficLight::LightRed;
            setSouthNorth(NESimpleTrafficLight::LightYellow);
            setEastWest(NESimpleTrafficLight::LightYellow);
            broadcastLightChanged(getSouthNorth(), getEastWest());
            mTimer.startTimer(NESimpleTrafficLight::TimeoutYellow, 1);
            break;

        case NESimpleTrafficLight::LightYellow:
            if ((mPrevState == NESimpleTrafficLight::LightRed) || (mPrevState == NESimpleTrafficLight::LightOff))
            {
                setSouthNorth(NESimpleTrafficLight::LightGreen);
                setEastWest(NESimpleTrafficLight::LightRed);
                mTimer.startTimer(NESimpleTrafficLight::TimeoutGreen);
            }
            else
            {
                setSouthNorth(NESimpleTrafficLight::LightRed);
                setEastWest(NESimpleTrafficLight::LightGreen);
                mTimer.startTimer(NESimpleTrafficLight::TimeoutRed);
            }

            broadcastLightChanged(getSouthNorth(), getEastWest());
            mPrevState  = NESimpleTrafficLight::LightYellow;
            break;

        case NESimpleTrafficLight::LightGreen:
            mPrevState  = NESimpleTrafficLight::LightGreen;
            setSouthNorth(NESimpleTrafficLight::LightYellow);
            setEastWest(NESimpleTrafficLight::LightYellow);
            broadcastLightChanged(getSouthNorth(), getEastWest());
            mTimer.startTimer(NESimpleTrafficLight::TimeoutYellow, 1);
            break;

        case NESimpleTrafficLight::LightOff:
        default:
            mPrevState  = NESimpleTrafficLight::LightOff;
            setSouthNorth(NESimpleTrafficLight::LightOff);
            setEastWest(NESimpleTrafficLight::LightOff);
            broadcastLightChanged(getSouthNorth(), getEastWest());
            break;
        }
    }
}

void SimpleTrafficLightCtrl::startupServiceInterface(Component & holder)
{
    SimpleTrafficLightStub::startupServiceInterface(holder);
    SimpleTrafficSwitchStub::startupServiceInterface(holder);

    TrafficSwitchEvent::addListener( static_cast<IETrafficSwitchConsumer &>(self()), holder.getMasterThread() );
}

void SimpleTrafficLightCtrl::shutdownServiceIntrface(Component & holder)
{
    SimpleTrafficLightStub::shutdownServiceIntrface(holder);
    SimpleTrafficSwitchStub::shutdownServiceIntrface(holder);

    TrafficSwitchEvent::removeListener( static_cast<IETrafficSwitchConsumer &>(self()), holder.getMasterThread() );
}
