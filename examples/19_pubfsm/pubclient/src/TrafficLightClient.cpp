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

#include "common/FsmDefs.hpp"
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
    , mIsEastWest                   (std::any_cast<bool>(entry.getData()))
{
}

void TrafficLightClient::onTrafficSouthNorthUpdate(const TrafficController::sTrafficLight & TrafficSouthNorth, NEService::DataState state)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_onTrafficSouthNorthUpdate);

    if (state == NEService::DataState::DataIsOK)
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", fsm::getName(TrafficSouthNorth.lightVehicle), fsm::getName(TrafficSouthNorth.lightPedestrian));
        
        notifyOnTrafficSouthNorthUpdate(false);
        notifyOnBroadcastSouthNorth(true);
    }
    else
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n"
                    , fsm::getName(TrafficController::VehicleTrafficLight::Off)
                    , fsm::getName(TrafficController::PedestrianTrafficLight::Off));
    }
}

void TrafficLightClient::onTrafficEastWestUpdate(const TrafficController::sTrafficLight & TrafficEastWest, NEService::DataState state)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_onTrafficEastWestUpdate);

    if (state == NEService::DataState::DataIsOK)
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", fsm::getName(TrafficEastWest.lightVehicle), fsm::getName(TrafficEastWest.lightPedestrian));

        notifyOnTrafficEastWestUpdate(false);
        notifyOnBroadcastEastWest(true);
    }
    else
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n"
                    , fsm::getName(TrafficController::VehicleTrafficLight::Off)
                    , fsm::getName(TrafficController::PedestrianTrafficLight::Off));
    }
}

void TrafficLightClient::broadcastSouthNorth(TrafficController::VehicleTrafficLight LightVehicle, TrafficController::PedestrianTrafficLight LightPedestrian)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_broadcastSouthNorth);

    printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", fsm::getName(LightVehicle), fsm::getName(LightPedestrian));
}

void TrafficLightClient::broadcastEastWest(TrafficController::VehicleTrafficLight LightVehicle, TrafficController::PedestrianTrafficLight LightPedestrian)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_broadcastEastWest);

    printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", fsm::getName(LightVehicle), fsm::getName(LightPedestrian));
}

bool TrafficLightClient::serviceConnected( NEService::ServiceConnectionState status, ProxyBase & proxy)
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
                , fsm::getName( TrafficController::VehicleTrafficLight::Off )
                , fsm::getName( TrafficController::PedestrianTrafficLight::Off ) );
        printf( "\nClose the application ..." );

        notifyOnTrafficEastWestUpdate( false );
        notifyOnBroadcastEastWest( false );
        notifyOnBroadcastSouthNorth( false );
        notifyOnBroadcastSouthNorth( false );


        Application::signalAppQuit( );
    }

    return result;
}
