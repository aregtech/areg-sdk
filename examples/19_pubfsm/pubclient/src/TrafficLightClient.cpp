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
#include "areg/logging/areg_log.h"
#include "areg/appbase/Application.hpp"

DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_onTrafficSouthNorthUpdate);
DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_onTrafficEastWestUpdate);
DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_broadcastSouthNorth);
DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_broadcastEastWest);
DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_serviceConnected);

TrafficLightClient::TrafficLightClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component                     ( entry, owner )
    , TrafficControllerClientBase   (entry.mDependencyServices[0].mRoleName)
    , mIsEastWest                   (std::any_cast<bool>(entry.data()))
{
}

void TrafficLightClient::onTrafficSouthNorthUpdate(const TrafficController::sTrafficLight & TrafficSouthNorth, areg::DataState state)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_onTrafficSouthNorthUpdate);

    if (state == areg::DataState::DataIsOK)
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", fsm::name(TrafficSouthNorth.lightVehicle), fsm::name(TrafficSouthNorth.lightPedestrian));
        
        notifyOnTrafficSouthNorthUpdate(false);
        notifyOnBroadcastSouthNorth(true);
    }
    else
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n"
                    , fsm::name(TrafficController::VehicleTrafficLight::Off)
                    , fsm::name(TrafficController::PedestrianTrafficLight::Off));
    }
}

void TrafficLightClient::onTrafficEastWestUpdate(const TrafficController::sTrafficLight & TrafficEastWest, areg::DataState state)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_onTrafficEastWestUpdate);

    if (state == areg::DataState::DataIsOK)
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", fsm::name(TrafficEastWest.lightVehicle), fsm::name(TrafficEastWest.lightPedestrian));

        notifyOnTrafficEastWestUpdate(false);
        notifyOnBroadcastEastWest(true);
    }
    else
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n"
                    , fsm::name(TrafficController::VehicleTrafficLight::Off)
                    , fsm::name(TrafficController::PedestrianTrafficLight::Off));
    }
}

void TrafficLightClient::broadcastSouthNorth(TrafficController::VehicleTrafficLight LightVehicle, TrafficController::PedestrianTrafficLight LightPedestrian)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_broadcastSouthNorth);

    printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", fsm::name(LightVehicle), fsm::name(LightPedestrian));
}

void TrafficLightClient::broadcastEastWest(TrafficController::VehicleTrafficLight LightVehicle, TrafficController::PedestrianTrafficLight LightPedestrian)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_broadcastEastWest);

    printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", fsm::name(LightVehicle), fsm::name(LightPedestrian));
}

bool TrafficLightClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_serviceConnected);

    bool result = TrafficControllerClientBase::service_connected( status, proxy );
    if ( is_connected( ) )
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
                , fsm::name( TrafficController::VehicleTrafficLight::Off )
                , fsm::name( TrafficController::PedestrianTrafficLight::Off ) );
        printf( "\nClose the application ..." );

        notifyOnTrafficEastWestUpdate( false );
        notifyOnBroadcastEastWest( false );
        notifyOnBroadcastSouthNorth( false );
        notifyOnBroadcastSouthNorth( false );


        areg::Application::signal_quit( );
    }

    return result;
}
