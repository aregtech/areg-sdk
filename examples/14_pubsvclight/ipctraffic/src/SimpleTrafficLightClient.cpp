/************************************************************************
 * \file        ipctraffic/src/SimpleTrafficLightClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "ipctraffic/src/SimpleTrafficLightClient.hpp"
#include "areg/appbase/Application.hpp"

Component * SimpleTrafficLightClient::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW SimpleTrafficLightClient(entry, owner);
}

void SimpleTrafficLightClient::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

SimpleTrafficLightClient::SimpleTrafficLightClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component                     ( owner, entry.mRoleName )
    , SimpleTrafficLightClientBase  ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )

    , mTrafficDirection             ( static_cast<NECommon::eTrafficDirection>(entry.getComponentData().alignInt.mElement) )
{
}

SimpleTrafficLightClient::~SimpleTrafficLightClient(void)
{
}

bool SimpleTrafficLightClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    bool result = SimpleTrafficLightClientBase::serviceConnected(isConnected, proxy);
    if (mTrafficDirection == NECommon::DirectionSouthNorth)
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
        outputState(NESimpleTrafficLight::LightOff);

        Application::signalAppQuit();
    }

    return result;
}

void SimpleTrafficLightClient::onSouthNorthUpdate(NESimpleTrafficLight::eTrafficLight SouthNorth, NEService::eDataStateType state)
{
    if (state == NEService::DATA_OK)
    {
        outputState(SouthNorth);

        notifyOnSouthNorthUpdate(false);
    }
}

void SimpleTrafficLightClient::onEastWestUpdate(NESimpleTrafficLight::eTrafficLight EastWest, NEService::eDataStateType state)
{
    if (state == NEService::DATA_OK)
    {
        outputState(EastWest);

        notifyOnEastWestUpdate(false);
    }
}

void SimpleTrafficLightClient::broadcastLightChanged(NESimpleTrafficLight::eTrafficLight SouthNorth, NESimpleTrafficLight::eTrafficLight EastWest)
{
    outputState( mTrafficDirection == NECommon::DirectionSouthNorth ? SouthNorth : EastWest );
}

inline void SimpleTrafficLightClient::outputState(NESimpleTrafficLight::eTrafficLight lightState)
{
    switch (lightState)
    {
    case NESimpleTrafficLight::LightRed:
        printf("Light: RED ...\n");
        break;
    case NESimpleTrafficLight::LightYellow:
        printf("Light: Yellow ...\n");
        break;
    case NESimpleTrafficLight::LightGreen:
        printf("Light: GREEN ...\n");
        break;
    case NESimpleTrafficLight::LightOff:
    default:
        printf("Light: OFF ...\n");
        break;
    }
}
