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

DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_on_traffic_south_north_update);
DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_on_traffic_east_west_update);
DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_broadcast_south_north);
DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_broadcast_east_west);
DEF_LOG_SCOPE(pubclient_src_TrafficLightClient_service_connected);

TrafficLightClient::TrafficLightClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component                     ( entry, owner )
    , TrafficControllerConsumerBase   (entry.mDependencyServices[0].mRoleName)
    , mIsEastWest                   (std::any_cast<bool>(entry.data()))
{
}

void TrafficLightClient::on_traffic_south_north_update(const TrafficController::sTrafficLight & TrafficSouthNorth, areg::DataState state)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_on_traffic_south_north_update);

    if (state == areg::DataState::DataIsOK)
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", fsm::name(TrafficSouthNorth.lightVehicle), fsm::name(TrafficSouthNorth.lightPedestrian));
        
        notify_on_traffic_south_north_update(false);
        notify_on_broadcast_south_north(true);
    }
    else
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n"
                    , fsm::name(TrafficController::VehicleTrafficLight::Off)
                    , fsm::name(TrafficController::PedestrianTrafficLight::Off));
    }
}

void TrafficLightClient::on_traffic_east_west_update(const TrafficController::sTrafficLight & TrafficEastWest, areg::DataState state)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_on_traffic_east_west_update);

    if (state == areg::DataState::DataIsOK)
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", fsm::name(TrafficEastWest.lightVehicle), fsm::name(TrafficEastWest.lightPedestrian));

        notify_on_traffic_east_west_update(false);
        notify_on_broadcast_east_west(true);
    }
    else
    {
        printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n"
                    , fsm::name(TrafficController::VehicleTrafficLight::Off)
                    , fsm::name(TrafficController::PedestrianTrafficLight::Off));
    }
}

void TrafficLightClient::broadcast_south_north(TrafficController::VehicleTrafficLight LightVehicle, TrafficController::PedestrianTrafficLight LightPedestrian)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_broadcast_south_north);

    printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", fsm::name(LightVehicle), fsm::name(LightPedestrian));
}

void TrafficLightClient::broadcast_east_west(TrafficController::VehicleTrafficLight LightVehicle, TrafficController::PedestrianTrafficLight LightPedestrian)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_broadcast_east_west);

    printf("\tVehicle Light: %12s    |\tPedestrian Light: %s\n", fsm::name(LightVehicle), fsm::name(LightPedestrian));
}

bool TrafficLightClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE(pubclient_src_TrafficLightClient_service_connected);

    bool result = TrafficControllerConsumerBase::service_connected( status, proxy );
    if ( is_connected( ) )
    {
        if ( mIsEastWest )
        {
            LOG_DBG( "The traffic light controller is connected, East-West direction" );
            notify_on_traffic_east_west_update( true );
        }
        else
        {
            LOG_DBG( "The traffic light controller is connected, South-North direction" );
            notify_on_traffic_south_north_update( true );
        }
    }
    else
    {
        LOG_WARN( "The traffic light controller is disconnected, set states OFF and close the application" );

        printf( "\tVehicle Light: %12s    |\tPedestrian Light: %s\n"
                , fsm::name( TrafficController::VehicleTrafficLight::Off )
                , fsm::name( TrafficController::PedestrianTrafficLight::Off ) );
        printf( "\nClose the application ..." );

        notify_on_traffic_east_west_update( false );
        notify_on_broadcast_east_west( false );
        notify_on_broadcast_south_north( false );
        notify_on_broadcast_south_north( false );


        areg::Application::signal_quit( );
    }

    return result;
}
