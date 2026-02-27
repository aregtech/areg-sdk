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

TrafficLightClient::TrafficLightClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component                     ( entry, owner )
    , SimpleTrafficLightClientBase  ( entry.mDependencyServices[0], static_cast<Component &>(self()) )

    , mTrafficDirection             ( std::any_cast<traffic::TrafficDirection>(entry.getData()) )
{
}

bool TrafficLightClient::service_connected( NEService::ServiceConnectionState status, ProxyBase & proxy)
{
    bool result = SimpleTrafficLightClientBase::service_connected( status, proxy );

    if ( mTrafficDirection == traffic::TrafficDirection::SouthNorth )
    {
        notifyOnSouthNorthUpdate( isConnected( ) );
    }
    else
    {
        notifyOnEastWestUpdate( isConnected( ) );
    }

    if ( isConnected( ) == false )
    {
        Application::signal_quit( );
    }

    return result;
}

void TrafficLightClient::onSouthNorthUpdate(SimpleTrafficLight::TrafficLight SouthNorth, NEService::DataState state)
{
    if (state == NEService::DataState::DataIsOK)
    {
        outputState(SouthNorth);
    }
}

void TrafficLightClient::onEastWestUpdate(SimpleTrafficLight::TrafficLight EastWest, NEService::DataState state)
{
    if (state == NEService::DataState::DataIsOK)
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
