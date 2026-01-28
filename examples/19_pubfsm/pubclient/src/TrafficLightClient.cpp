/************************************************************************
 * \file        pubclient/src/TrafficLightClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This is a Traffic Light controller client to switch lights.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "pubclient/src/TrafficLightClient.hpp"

#include "common/NECommon.hpp"
#include "areg/logging/GELog.h"
#include "areg/appbase/Application.hpp"

DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_onTrafficSouthNorthUpdate);
DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_onTrafficEastWestUpdate);
DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_broadcastSouthNorth);
DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_broadcastEastWest);
DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_serviceConnected);

TrafficLightClient::TrafficLightClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component                     ( entry, owner )
    , TrafficControllerClientBase   (entry.mDependencyServices[0].mRoleName)
    , mIsEastWest                   (std::any_cast<bool>(entry.getComponentData()))
{
}

void TrafficLightClient::onTrafficSouthNorthUpdate(const NETrafficController::sTrafficLight & TrafficSouthNorth, NEService::eDataStateType state)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_onTrafficSouthNorthUpdate);

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
    LOG_SCOPE(pubclient_src_TrafficLightClient_onTrafficEastWestUpdate);

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
    LOG_SCOPE(pubclient_src_TrafficLightClient_broadcastSouthNorth);

    printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", NECommon::getName(LightVehicle), NECommon::getName(LightPedestrian));
}

void TrafficLightClient::broadcastEastWest(NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_broadcastEastWest);

    printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", NECommon::getName(LightVehicle), NECommon::getName(LightPedestrian));
}

bool TrafficLightClient::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_serviceConnected);

    bool result = TrafficControllerClientBase::serviceConnected( status, proxy );
    if ( isConnected( ) )
    {
        if ( mIsEastWest )
        {
            LOG_DBG( "The traffic light controller is connected, East-West direction" );
            notifyOnTrafficEastWestUpdate( true );
        }
        else
        {
            LOG_DBG( "The traffic light controller is connected, South-North direction" );
            notifyOnTrafficSouthNorthUpdate( true );
        }
    }
    else
    {
        LOG_WARN( "The traffic light controller is disconnected, set states OFF and close the application" );

        printf( "\tVehicle Light: %12s    |\tPedestrian Light: %s\n"
                , NECommon::getName( NETrafficController::eVehicleTrafficLight::VehicleLightOFF )
                , NECommon::getName( NETrafficController::ePedestrianTrafficLight::PedestrianLightOFF ) );
        printf( "\nClose the application ..." );

        notifyOnTrafficEastWestUpdate( false );
        notifyOnBroadcastEastWest( false );
        notifyOnBroadcastSouthNorth( false );
        notifyOnBroadcastSouthNorth( false );


        Application::signalAppQuit( );
    }

    return result;
}
