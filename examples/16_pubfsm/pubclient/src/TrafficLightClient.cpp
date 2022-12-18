/************************************************************************
 * \file        pubclient/src/TrafficLightClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This is a Traffic Light controller client to switch lights.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "pubclient/src/TrafficLightClient.hpp"

#include "generated/src/NECommon.hpp"
#include "areg/trace/GETrace.h"
#include "areg/appbase/Application.hpp"

DEF_TRACE_SCOPE(pubclient_src_TrafficLightClient_onTrafficSouthNorthUpdate);
DEF_TRACE_SCOPE(pubclient_src_TrafficLightClient_onTrafficEastWestUpdate);
DEF_TRACE_SCOPE(pubclient_src_TrafficLightClient_broadcastSouthNorth);
DEF_TRACE_SCOPE(pubclient_src_TrafficLightClient_broadcastEastWest);
DEF_TRACE_SCOPE(pubclient_src_TrafficLightClient_serviceConnected);

Component * TrafficLightClient::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW TrafficLightClient(entry, owner);
}

void TrafficLightClient::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete(&compObject);
}

TrafficLightClient::TrafficLightClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component                     ( entry, owner )
    , TrafficControllerClientBase   (entry.mDependencyServices[0].mRoleName)
    , mIsEastWest                   (entry.getComponentData().alignBool.mElement)
{
}

void TrafficLightClient::onTrafficSouthNorthUpdate(const NETrafficController::sTrafficLight & TrafficSouthNorth, NEService::eDataStateType state)
{
    TRACE_SCOPE(pubclient_src_TrafficLightClient_onTrafficSouthNorthUpdate);

    if (state == NEService::eDataStateType::DataIsOK)
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", NECommon::getName(TrafficSouthNorth.lightVehicle), NECommon::getName(TrafficSouthNorth.lightPedestrian));
        
        notifyOnTrafficSouthNorthUpdate(false);
        notifyOnBroadcastSouthNorth(true);
    }
    else
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n"
                    , NECommon::getName(NETrafficController::eVehicleTrafficLight::VehicleLightOFF)
                    , NECommon::getName(NETrafficController::ePedestrianTrafficLight::PedestrianLightOFF));
    }
}

void TrafficLightClient::onTrafficEastWestUpdate(const NETrafficController::sTrafficLight & TrafficEastWest, NEService::eDataStateType state)
{
    TRACE_SCOPE(pubclient_src_TrafficLightClient_onTrafficEastWestUpdate);

    if (state == NEService::eDataStateType::DataIsOK)
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", NECommon::getName(TrafficEastWest.lightVehicle), NECommon::getName(TrafficEastWest.lightPedestrian));

        notifyOnTrafficEastWestUpdate(false);
        notifyOnBroadcastEastWest(true);
    }
    else
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n"
                    , NECommon::getName(NETrafficController::eVehicleTrafficLight::VehicleLightOFF)
                    , NECommon::getName(NETrafficController::ePedestrianTrafficLight::PedestrianLightOFF));
    }
}

void TrafficLightClient::broadcastSouthNorth(NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian)
{
    TRACE_SCOPE(pubclient_src_TrafficLightClient_broadcastSouthNorth);

    printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", NECommon::getName(LightVehicle), NECommon::getName(LightPedestrian));
}

void TrafficLightClient::broadcastEastWest(NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian)
{
    TRACE_SCOPE(pubclient_src_TrafficLightClient_broadcastEastWest);

    printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", NECommon::getName(LightVehicle), NECommon::getName(LightPedestrian));
}

bool TrafficLightClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    TRACE_SCOPE(pubclient_src_TrafficLightClient_serviceConnected);

    bool result = false;
    if (TrafficControllerClientBase::serviceConnected(isConnected, proxy))
    {
        result = true;

        if (isConnected)
        {
            if ( mIsEastWest )
            {
                TRACE_DBG("The traffic light controller is connected, East-West direction");
                notifyOnTrafficEastWestUpdate(true);
            }
            else
            {
                TRACE_DBG("The traffic light controller is connected, South-North direction");
                notifyOnTrafficSouthNorthUpdate(true);
            }
        }
        else
        {
            TRACE_WARN("The traffic light controller is disconnected, set states OFF and close the application");

            printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n"
                        , NECommon::getName(NETrafficController::eVehicleTrafficLight::VehicleLightOFF)
                        , NECommon::getName(NETrafficController::ePedestrianTrafficLight::PedestrianLightOFF));
            printf("\nClose the application ...");

            notifyOnTrafficEastWestUpdate(false);
            notifyOnBroadcastEastWest(false);
            notifyOnBroadcastSouthNorth(false);
            notifyOnBroadcastSouthNorth(false);


            Application::signalAppQuit();
        }
    }

    return result;
}
