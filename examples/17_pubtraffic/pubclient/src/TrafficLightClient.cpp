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

    , mTrafficDirection             ( std::any_cast<traffic::eTrafficDirection>(entry.getData()) )
{
}

bool TrafficLightClient::serviceConnected( NEService::ServiceConnectionState status, ProxyBase & proxy)
{
    bool result = SimpleTrafficLightClientBase::serviceConnected( status, proxy );

    if ( mTrafficDirection == traffic::eTrafficDirection::DirectionSouthNorth )
    {
        notifyOnSouthNorthUpdate( isConnected( ) );
    }
    else
    {
        notifyOnEastWestUpdate( isConnected( ) );
    }

    if ( isConnected( ) == false )
    {
        Application::signalAppQuit( );
    }

    return result;
}

void TrafficLightClient::onSouthNorthUpdate(SimpleTrafficLight::eTrafficLight SouthNorth, NEService::DataState state)
{
    if (state == NEService::DataState::DataIsOK)
    {
        outputState(SouthNorth);
    }
}

void TrafficLightClient::onEastWestUpdate(SimpleTrafficLight::eTrafficLight EastWest, NEService::DataState state)
{
    if (state == NEService::DataState::DataIsOK)
    {
        outputState(EastWest);
    }
}

inline void TrafficLightClient::outputState(SimpleTrafficLight::eTrafficLight lightState)
{
    switch (lightState)
    {
    case SimpleTrafficLight::eTrafficLight::LightRed:
        printf("Light: RED ...\r\n");
        break;
    case SimpleTrafficLight::eTrafficLight::LightYellow:
        printf("Light: Yellow ...\r\n");
        break;
    case SimpleTrafficLight::eTrafficLight::LightGreen:
        printf("Light: GREEN ...\r\n");
        break;
    case SimpleTrafficLight::eTrafficLight::LightOff:
    default:
        printf("Light: OFF ...\r\n");
        break;
    }
}
