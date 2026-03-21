/************************************************************************
 * \file        pubclient/src/TrafficLightClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of service client.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "pubclient/src/TrafficLightClient.hpp"
#include "areg/appbase/Application.hpp"

TrafficLightClient::TrafficLightClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component                     ( entry, owner )
    , SimpleTrafficLightConsumerBase  ( entry.mDependencyServices[0], static_cast<areg::Component &>(self()) )

    , mTrafficDirection             ( std::any_cast<traffic::TrafficDirection>(entry.data()) )
{
}

bool TrafficLightClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    bool result = SimpleTrafficLightConsumerBase::service_connected( status, proxy );

    if ( mTrafficDirection == traffic::TrafficDirection::SouthNorth )
    {
        notify_on_south_north_update( is_connected( ) );
    }
    else
    {
        notify_on_east_west_update( is_connected( ) );
    }

    if ( is_connected( ) == false )
    {
        areg::Application::signal_quit( );
    }

    return result;
}

void TrafficLightClient::on_south_north_update(SimpleTrafficLight::TrafficLight SouthNorth, areg::DataState state)
{
    if (state == areg::DataState::DataIsOK)
    {
        outputState(SouthNorth);
    }
}

void TrafficLightClient::on_east_west_update(SimpleTrafficLight::TrafficLight EastWest, areg::DataState state)
{
    if (state == areg::DataState::DataIsOK)
    {
        outputState(EastWest);
    }
}

inline void TrafficLightClient::outputState(SimpleTrafficLight::TrafficLight lightState)
{
    switch (lightState)
    {
    case SimpleTrafficLight::TrafficLight::LightRed:
        printf("Light: RED ...\r\n");
        break;
    case SimpleTrafficLight::TrafficLight::LightYellow:
        printf("Light: Yellow ...\r\n");
        break;
    case SimpleTrafficLight::TrafficLight::LightGreen:
        printf("Light: GREEN ...\r\n");
        break;
    case SimpleTrafficLight::TrafficLight::LightOff:
    default:
        printf("Light: OFF ...\r\n");
        break;
    }
}
