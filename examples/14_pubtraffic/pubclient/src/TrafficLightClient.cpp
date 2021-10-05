/************************************************************************
 * \file        pubclient/src/TrafficLightClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "pubclient/src/TrafficLightClient.hpp"
#include "areg/appbase/Application.hpp"

Component * TrafficLightClient::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW TrafficLightClient(entry, owner);
}

void TrafficLightClient::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

TrafficLightClient::TrafficLightClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component                     ( owner, entry.mRoleName )
    , SimpleTrafficLightClientBase  ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )

    , mTrafficDirection             ( static_cast<NECommon::eTrafficDirection>(entry.getComponentData().alignInt.mElement) )
{
}

bool TrafficLightClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    bool result = SimpleTrafficLightClientBase::serviceConnected(isConnected, proxy);
    if (mTrafficDirection == NECommon::eTrafficDirection::DirectionSouthNorth)
    {
        notifyOnSouthNorthUpdate(isConnected);
    }
    else
    {
        notifyOnEastWestUpdate(isConnected);
    }

    notifyOnBroadcastLightChanged(isConnected);

    if (isConnected == false)
    {
        outputState(NESimpleTrafficLight::eTrafficLight::LightOff);

        Application::signalAppQuit();
    }

    return result;
}

void TrafficLightClient::onSouthNorthUpdate(NESimpleTrafficLight::eTrafficLight SouthNorth, NEService::eDataStateType state)
{
    if (state == NEService::eDataStateType::DataIsOK)
    {
        outputState(SouthNorth);

        notifyOnSouthNorthUpdate(false);
    }
}

void TrafficLightClient::onEastWestUpdate(NESimpleTrafficLight::eTrafficLight EastWest, NEService::eDataStateType state)
{
    if (state == NEService::eDataStateType::DataIsOK)
    {
        outputState(EastWest);

        notifyOnEastWestUpdate(false);
    }
}

void TrafficLightClient::broadcastLightChanged(NESimpleTrafficLight::eTrafficLight SouthNorth, NESimpleTrafficLight::eTrafficLight EastWest)
{
    outputState( mTrafficDirection == NECommon::eTrafficDirection::DirectionSouthNorth ? SouthNorth : EastWest );
}

inline void TrafficLightClient::outputState(NESimpleTrafficLight::eTrafficLight lightState)
{
    switch (lightState)
    {
    case NESimpleTrafficLight::eTrafficLight::LightRed:
        printf("Light: RED ...\n");
        break;
    case NESimpleTrafficLight::eTrafficLight::LightYellow:
        printf("Light: Yellow ...\n");
        break;
    case NESimpleTrafficLight::eTrafficLight::LightGreen:
        printf("Light: GREEN ...\n");
        break;
    case NESimpleTrafficLight::eTrafficLight::LightOff:
    default:
        printf("Light: OFF ...\n");
        break;
    }
}
