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

void TrafficLightService::TrafficSwitchConsumer::process_event(const TrafficSwitchData &data)
{
    if (data.data( ))
    {
        mService.on_traffic_light_switched_on();
    }
    else
    {
        mService.on_traffic_light_switched_off();
    }
}

//////////////////////////////////////////////////////////////////////////
// TrafficLightService::TrafficLightTimerConsumer class implementation
//////////////////////////////////////////////////////////////////////////
void TrafficLightService::TrafficLightTimerConsumer::process_timer( areg::Timer & timer )
{
    if (&timer == &mService.mTimer)
    {
        mService.on_timer_expired();
    }
}

//////////////////////////////////////////////////////////////////////////
// TrafficLightService class implementation
//////////////////////////////////////////////////////////////////////////

TrafficLightService::TrafficLightService(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component                 ( entry, owner )
    , SimpleTrafficLightProviderBase    ( static_cast<areg::Component &>(self()) )

    , mTimer                    ( static_cast<areg::TimerConsumer &>(mTimerConsumer), "SimpleTrafficTimer")
    , mPrevState                ( SimpleTrafficLight::TrafficLight::LightOff )
    , mEventConsumer            ( self() )
    , mTimerConsumer            ( self() )
{
    set_south_north(SimpleTrafficLight::TrafficLight::LightOff);
    set_east_west(SimpleTrafficLight::TrafficLight::LightOff);
}

void TrafficLightService::on_traffic_light_switched_on()
{
    if ( south_north( ) == SimpleTrafficLight::TrafficLight::LightOff )
    {
        set_south_north( SimpleTrafficLight::TrafficLight::LightYellow );
        set_east_west( SimpleTrafficLight::TrafficLight::LightYellow );
        mTimer.start_timer( SimpleTrafficLight::TimeoutYellow, 1 );
    }
}

void TrafficLightService::on_traffic_light_switched_off()
{
    if ( south_north( ) != SimpleTrafficLight::TrafficLight::LightOff )
    {
        mTimer.stop_timer( );

        mPrevState = SimpleTrafficLight::TrafficLight::LightOff;
        set_south_north( SimpleTrafficLight::TrafficLight::LightOff );
        set_east_west( SimpleTrafficLight::TrafficLight::LightOff );
    }
}

void TrafficLightService::on_timer_expired()
{
    switch (south_north())
    {
    case SimpleTrafficLight::TrafficLight::LightRed:
        mPrevState  = SimpleTrafficLight::TrafficLight::LightRed;
        set_south_north(SimpleTrafficLight::TrafficLight::LightYellow);
        set_east_west(SimpleTrafficLight::TrafficLight::LightYellow);
        mTimer.start_timer(SimpleTrafficLight::TimeoutYellow, 1);
        break;

    case SimpleTrafficLight::TrafficLight::LightYellow:
        if ((mPrevState == SimpleTrafficLight::TrafficLight::LightRed) || (mPrevState == SimpleTrafficLight::TrafficLight::LightOff))
        {
            set_south_north(SimpleTrafficLight::TrafficLight::LightGreen);
            set_east_west(SimpleTrafficLight::TrafficLight::LightRed);
            mTimer.start_timer(SimpleTrafficLight::TimeoutGreen);
        }
        else
        {
            set_south_north(SimpleTrafficLight::TrafficLight::LightRed);
            set_east_west(SimpleTrafficLight::TrafficLight::LightGreen);
            mTimer.start_timer(SimpleTrafficLight::TimeoutRed);
        }

        mPrevState  = SimpleTrafficLight::TrafficLight::LightYellow;
        break;

    case SimpleTrafficLight::TrafficLight::LightGreen:
        mPrevState  = SimpleTrafficLight::TrafficLight::LightGreen;
        set_south_north(SimpleTrafficLight::TrafficLight::LightYellow);
        set_east_west(SimpleTrafficLight::TrafficLight::LightYellow);
        mTimer.start_timer(SimpleTrafficLight::TimeoutYellow, 1);
        break;

    case SimpleTrafficLight::TrafficLight::LightOff:
    default:
        mPrevState  = SimpleTrafficLight::TrafficLight::LightOff;
        set_south_north(SimpleTrafficLight::TrafficLight::LightOff);
        set_east_west(SimpleTrafficLight::TrafficLight::LightOff);
        break;
    }
}

void TrafficLightService::startup_service_interface(areg::Component & holder)
{
    SimpleTrafficLightProviderBase::startup_service_interface(holder);
    TrafficSwitchEvent::add_listener( static_cast<IETrafficSwitchConsumer &>(mEventConsumer), holder.master_thread() );
}

void TrafficLightService::shutdown_service_interface(areg::Component & holder) noexcept
{
    SimpleTrafficLightProviderBase::shutdown_service_interface(holder);
    TrafficSwitchEvent::remove_listener( static_cast<IETrafficSwitchConsumer &>(mEventConsumer), holder.master_thread() );
}
