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

    , mTrafficDirection             ( std::any_cast<NECommon::eTrafficDirection>(entry.getComponentData()) )
{
}

bool TrafficLightClient::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy)
{
    bool result = SimpleTrafficLightClientBase::serviceConnected( status, proxy );

    if ( mTrafficDirection == NECommon::eTrafficDirection::DirectionSouthNorth )
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

void TrafficLightClient::onSouthNorthUpdate(NESimpleTrafficLight::eTrafficLight SouthNorth, NEService::eDataStateType state)
{
    if (state == NEService::eDataStateType::DataIsOK)
    {
        outputState(SouthNorth);
    }
}

void TrafficLightClient::onEastWestUpdate(NESimpleTrafficLight::eTrafficLight EastWest, NEService::eDataStateType state)
{
    if (state == NEService::eDataStateType::DataIsOK)
    {
        outputState(EastWest);
    }
}

inline void TrafficLightClient::outputState(NESimpleTrafficLight::eTrafficLight lightState)
{
    switch (lightState)
    {
    case NESimpleTrafficLight::eTrafficLight::LightRed:
        printf("Light: RED ...\r\n");
        break;
    case NESimpleTrafficLight::eTrafficLight::LightYellow:
        printf("Light: Yellow ...\r\n");
        break;
    case NESimpleTrafficLight::eTrafficLight::LightGreen:
        printf("Light: GREEN ...\r\n");
        break;
    case NESimpleTrafficLight::eTrafficLight::LightOff:
    default:
        printf("Light: OFF ...\r\n");
        break;
    }
}
