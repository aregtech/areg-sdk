/************************************************************************
 * \file        pubservice/src/TrafficLightService.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This is a Traffic Light controller, which uses finite-state machine (FSM),
 *              implementation of services and a thread consumer.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "pubservice/src/TrafficLightService.hpp"

#include "common/FsmDefs.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/logging/areg_log.h"

DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_request_power_on);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_request_power_off);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_request_start_traffic_light);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_request_stop_traffic_light);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPowerOff);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPowerOn);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleYellow);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleRed);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPedestrianRed);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleGreen);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPedestrianGreen);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_startup_component);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_shutdown_component);

TrafficLightService::TrafficLightService(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component             ( entry, owner )
    , PowerManagerProviderBase    ( static_cast<areg::Component &>(self()) )
    , TrafficControllerProviderBase( static_cast<areg::Component &>(self()) )
    , TrafficLightActionHandler   ( )

    , mConsoleInput               ( entry.mRoleName, static_cast<areg::Component &>(self()) )
    , mLightFsm                   ( static_cast<TrafficLightActionHandler &>(self()) )
{

}

TrafficLightService::~TrafficLightService()
{
    mLightFsm.releaseFSM();
}

void TrafficLightService::request_power_on()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_request_power_on);

    mLightFsm.powerOn();
}

void TrafficLightService::request_power_off()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_request_power_off);

    mLightFsm.powerOff();
}

void TrafficLightService::request_start_traffic_light()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_request_start_traffic_light);

    bool result = false;
    const TrafficController::sTrafficLight & lights = traffic_east_west();

    if ((lights.trafficDirection != TrafficController::TrafficDirection::Undefined) && (lights.lightVehicle != TrafficController::VehicleTrafficLight::Off))
    {
        mLightFsm.startTrafficControl();
        result = true;
    }

    response_start_traffic_light(result);
}

void TrafficLightService::request_stop_traffic_light()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_request_stop_traffic_light);

    bool result = false;
    const TrafficController::sTrafficLight & lights = traffic_east_west();

    if ((lights.trafficDirection != TrafficController::TrafficDirection::Undefined) && (lights.lightVehicle != TrafficController::VehicleTrafficLight::Off))
    {
        mLightFsm.stopTrafficControl();
        result = true;
    }

    response_stop_traffic_light(result);
}

void TrafficLightService::actionPowerOff()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPowerOff);
    LOG_DBG("Handling traffic light power OFF");

    TrafficController::sTrafficLight lights;
    lights.lightVehicle     = TrafficController::VehicleTrafficLight::Off;
    lights.lightPedestrian  = TrafficController::PedestrianTrafficLight::Off;

    set_lights_power_state(PowerManager::PowerState::LightsOFF);

    lights.trafficDirection = TrafficController::TrafficDirection::SouthNorth;
    set_traffic_south_north(lights);

    lights.trafficDirection = TrafficController::TrafficDirection::EastWest;
    set_traffic_east_west(lights);

    broadcast_east_west(TrafficController::VehicleTrafficLight::Off, TrafficController::PedestrianTrafficLight::Off);
    broadcast_south_north(TrafficController::VehicleTrafficLight::Off, TrafficController::PedestrianTrafficLight::Off);
}

void TrafficLightService::actionPowerOn()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPowerOn);

    LOG_DBG("Handling traffic light power ON");

    TrafficController::sTrafficLight lights;
    lights.lightVehicle     = TrafficController::VehicleTrafficLight::Initializing;
    lights.lightPedestrian  = TrafficController::PedestrianTrafficLight::Off;

    set_lights_power_state(PowerManager::PowerState::LightsON);

    lights.trafficDirection = TrafficController::TrafficDirection::SouthNorth;
    set_traffic_south_north(lights);

    lights.trafficDirection = TrafficController::TrafficDirection::EastWest;
    set_traffic_east_west(lights);

    broadcast_east_west(TrafficController::VehicleTrafficLight::Initializing, TrafficController::PedestrianTrafficLight::Off);
    broadcast_south_north(TrafficController::VehicleTrafficLight::Initializing, TrafficController::PedestrianTrafficLight::Off);
}

void TrafficLightService::actionVehicleYellow()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleYellow);
    LOG_DBG("Vehicle light is yellow, pedestrian is red");

    TrafficController::sTrafficLight sn, ew;
    
    sn.lightVehicle     = TrafficController::VehicleTrafficLight::Yellow;
    sn.lightPedestrian  = TrafficController::PedestrianTrafficLight::Red;
    sn.trafficDirection = TrafficController::TrafficDirection::SouthNorth;

    ew.lightVehicle     = TrafficController::VehicleTrafficLight::Yellow;
    ew.lightPedestrian  = TrafficController::PedestrianTrafficLight::Red;
    ew.trafficDirection = TrafficController::TrafficDirection::EastWest;

    set_traffic_south_north(sn);
    set_traffic_east_west(ew);

    broadcast_south_north(sn.lightVehicle, sn.lightPedestrian);
    broadcast_east_west(ew.lightVehicle, ew.lightPedestrian);

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(sn.lightVehicle), fsm::name(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(ew.lightVehicle), fsm::name(ew.lightPedestrian));
}

void TrafficLightService::actionVehicleRed()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleRed);
    LOG_DBG("Vehicle light is red. Initially, pedestrian is red");

    TrafficController::sTrafficLight sn = traffic_south_north();
    TrafficController::sTrafficLight ew = traffic_east_west();

    sn.lightVehicle     = TrafficController::VehicleTrafficLight::Red;
    sn.lightPedestrian  = TrafficController::PedestrianTrafficLight::Red;

    set_traffic_south_north(sn);
    broadcast_south_north(sn.lightVehicle, sn.lightPedestrian);

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(sn.lightVehicle), fsm::name(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(ew.lightVehicle), fsm::name(ew.lightPedestrian));
}

void TrafficLightService::actionPedestrianRed()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPedestrianRed);
    LOG_DBG("Pedestrian light is red. Vehicle light is unchanged.");

    LOG_DBG("Setting traffic light states for direction East-West");
    TrafficController::sTrafficLight sn = traffic_south_north();
    TrafficController::sTrafficLight ew = traffic_east_west();

    sn.lightPedestrian  = TrafficController::PedestrianTrafficLight::Red;
    ew.lightPedestrian  = TrafficController::PedestrianTrafficLight::Red;

    set_traffic_south_north(sn);
    set_traffic_east_west(ew);
    broadcast_south_north(sn.lightVehicle, sn.lightPedestrian);
    broadcast_east_west(ew.lightVehicle, ew.lightPedestrian);

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(sn.lightVehicle), fsm::name(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(ew.lightVehicle), fsm::name(ew.lightPedestrian));
}

void TrafficLightService::actionVehicleGreen( bool isEastWest )
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleGreen);

    TrafficController::sTrafficLight sn = traffic_south_north();
    TrafficController::sTrafficLight ew = traffic_east_west();

    if (isEastWest)
    {
        LOG_DBG("East-West direction vehicle light is green. Pedestrian light is unchanged.");

        ew.lightVehicle = TrafficController::VehicleTrafficLight::Green;
        set_traffic_east_west(ew);
        broadcast_east_west(ew.lightVehicle, ew.lightPedestrian);
    }
    else
    {
        LOG_DBG("North-South direction vehicle light is green. Pedestrian light is unchanged.");

        sn.lightVehicle = TrafficController::VehicleTrafficLight::Green;
        set_traffic_south_north(sn);
        broadcast_south_north(sn.lightVehicle, sn.lightPedestrian);
    }

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(sn.lightVehicle), fsm::name(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(ew.lightVehicle), fsm::name(ew.lightPedestrian));
}

void TrafficLightService::actionPedestrianGreen(bool isEastWest)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPedestrianGreen);

    TrafficController::sTrafficLight sn = traffic_south_north();
    TrafficController::sTrafficLight ew = traffic_east_west();

    if (isEastWest)
    {
        LOG_DBG("East-West direction pedestrian light is green. Vehicle light is unchanged.");

        ew.lightPedestrian = TrafficController::PedestrianTrafficLight::Green;
        set_traffic_east_west(ew);
        broadcast_east_west(ew.lightVehicle, ew.lightPedestrian);


        sn.lightVehicle = TrafficController::VehicleTrafficLight::Red;
        set_traffic_south_north( sn );
        broadcast_south_north( sn.lightVehicle, sn.lightPedestrian );
    }
    else
    {
        LOG_DBG("North-South direction pedestrian light is green. Vehicle light is unchanged.");

        sn.lightPedestrian = TrafficController::PedestrianTrafficLight::Green;
        set_traffic_south_north(sn);
        broadcast_south_north(sn.lightVehicle, sn.lightPedestrian);

        ew.lightVehicle = TrafficController::VehicleTrafficLight::Red;
        set_traffic_east_west( ew );
        broadcast_east_west( ew.lightVehicle, ew.lightPedestrian );
    }

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(sn.lightVehicle), fsm::name(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(ew.lightVehicle), fsm::name(ew.lightPedestrian));
}

void TrafficLightService::startup_component(areg::ComponentThread & comThread)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_startup_component);

    areg::Component::startup_component(comThread);
    mLightFsm.initFSM(&comThread);
}

void TrafficLightService::shutdown_component(areg::ComponentThread & comThread)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_shutdown_component);

    mLightFsm.releaseFSM();
    areg::Component::shutdown_component(comThread);
}
