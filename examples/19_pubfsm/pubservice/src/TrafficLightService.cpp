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

DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_requestPowerOn);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_requestPowerOff);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_requestStartTrafficLight);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_requestStopTrafficLight);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPowerOff);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPowerOn);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleYellow);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleRed);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPedestrianRed);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleGreen);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPedestrianGreen);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_startupComponent);
DEF_LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_shutdownComponent);

TrafficLightService::TrafficLightService(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component                     ( entry, owner )
    , PowerManagerStub              ( static_cast<areg::Component &>(self()) )
    , TrafficControllerStub         ( static_cast<areg::Component &>(self()) )
    , TrafficLightActionHandler   ( )

    , mConsoleInput                 ( entry.mRoleName, static_cast<areg::Component &>(self()) )
    , mLightFsm                     ( static_cast<TrafficLightActionHandler &>(self()) )
{

}

TrafficLightService::~TrafficLightService()
{
    mLightFsm.releaseFSM();
}

void TrafficLightService::requestPowerOn()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_requestPowerOn);

    mLightFsm.powerOn();
}

void TrafficLightService::requestPowerOff()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_requestPowerOff);

    mLightFsm.powerOff();
}

void TrafficLightService::requestStartTrafficLight()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_requestStartTrafficLight);

    bool result = false;
    const TrafficController::sTrafficLight & lights = getTrafficEastWest();

    if ((lights.trafficDirection != TrafficController::TrafficDirection::Undefined) && (lights.lightVehicle != TrafficController::VehicleTrafficLight::Off))
    {
        mLightFsm.startTrafficControl();
        result = true;
    }

    responseStartTrafficLight(result);
}

void TrafficLightService::requestStopTrafficLight()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_requestStopTrafficLight);

    bool result = false;
    const TrafficController::sTrafficLight & lights = getTrafficEastWest();

    if ((lights.trafficDirection != TrafficController::TrafficDirection::Undefined) && (lights.lightVehicle != TrafficController::VehicleTrafficLight::Off))
    {
        mLightFsm.stopTrafficControl();
        result = true;
    }

    responseStopTrafficLight(result);
}

void TrafficLightService::actionPowerOff()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPowerOff);
    LOG_DBG("Handling traffic light power OFF");

    TrafficController::sTrafficLight lights;
    lights.lightVehicle     = TrafficController::VehicleTrafficLight::Off;
    lights.lightPedestrian  = TrafficController::PedestrianTrafficLight::Off;

    setLightsPowerState(PowerManager::PowerState::LightsOFF);

    lights.trafficDirection = TrafficController::TrafficDirection::SouthNorth;
    setTrafficSouthNorth(lights);

    lights.trafficDirection = TrafficController::TrafficDirection::EastWest;
    setTrafficEastWest(lights);

    broadcastEastWest(TrafficController::VehicleTrafficLight::Off, TrafficController::PedestrianTrafficLight::Off);
    broadcastSouthNorth(TrafficController::VehicleTrafficLight::Off, TrafficController::PedestrianTrafficLight::Off);
}

void TrafficLightService::actionPowerOn()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPowerOn);

    LOG_DBG("Handling traffic light power ON");

    TrafficController::sTrafficLight lights;
    lights.lightVehicle     = TrafficController::VehicleTrafficLight::Initializing;
    lights.lightPedestrian  = TrafficController::PedestrianTrafficLight::Off;

    setLightsPowerState(PowerManager::PowerState::LightsON);

    lights.trafficDirection = TrafficController::TrafficDirection::SouthNorth;
    setTrafficSouthNorth(lights);

    lights.trafficDirection = TrafficController::TrafficDirection::EastWest;
    setTrafficEastWest(lights);

    broadcastEastWest(TrafficController::VehicleTrafficLight::Initializing, TrafficController::PedestrianTrafficLight::Off);
    broadcastSouthNorth(TrafficController::VehicleTrafficLight::Initializing, TrafficController::PedestrianTrafficLight::Off);
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

    setTrafficSouthNorth(sn);
    setTrafficEastWest(ew);

    broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);
    broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(sn.lightVehicle), fsm::name(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(ew.lightVehicle), fsm::name(ew.lightPedestrian));
}

void TrafficLightService::actionVehicleRed()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleRed);
    LOG_DBG("Vehicle light is red. Initially, pedestrian is red");

    TrafficController::sTrafficLight sn = getTrafficSouthNorth();
    TrafficController::sTrafficLight ew = getTrafficEastWest();

    sn.lightVehicle     = TrafficController::VehicleTrafficLight::Red;
    sn.lightPedestrian  = TrafficController::PedestrianTrafficLight::Red;

    setTrafficSouthNorth(sn);
    broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(sn.lightVehicle), fsm::name(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(ew.lightVehicle), fsm::name(ew.lightPedestrian));
}

void TrafficLightService::actionPedestrianRed()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPedestrianRed);
    LOG_DBG("Pedestrian light is red. Vehicle light is unchanged.");

    LOG_DBG("Setting traffic light states for direction East-West");
    TrafficController::sTrafficLight sn = getTrafficSouthNorth();
    TrafficController::sTrafficLight ew = getTrafficEastWest();

    sn.lightPedestrian  = TrafficController::PedestrianTrafficLight::Red;
    ew.lightPedestrian  = TrafficController::PedestrianTrafficLight::Red;

    setTrafficSouthNorth(sn);
    setTrafficEastWest(ew);
    broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);
    broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(sn.lightVehicle), fsm::name(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(ew.lightVehicle), fsm::name(ew.lightPedestrian));
}

void TrafficLightService::actionVehicleGreen( bool isEastWest )
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleGreen);

    TrafficController::sTrafficLight sn = getTrafficSouthNorth();
    TrafficController::sTrafficLight ew = getTrafficEastWest();

    if (isEastWest)
    {
        LOG_DBG("East-West direction vehicle light is green. Pedestrian light is unchanged.");

        ew.lightVehicle = TrafficController::VehicleTrafficLight::Green;
        setTrafficEastWest(ew);
        broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);
    }
    else
    {
        LOG_DBG("North-South direction vehicle light is green. Pedestrian light is unchanged.");

        sn.lightVehicle = TrafficController::VehicleTrafficLight::Green;
        setTrafficSouthNorth(sn);
        broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);
    }

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(sn.lightVehicle), fsm::name(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(ew.lightVehicle), fsm::name(ew.lightPedestrian));
}

void TrafficLightService::actionPedestrianGreen(bool isEastWest)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPedestrianGreen);

    TrafficController::sTrafficLight sn = getTrafficSouthNorth();
    TrafficController::sTrafficLight ew = getTrafficEastWest();

    if (isEastWest)
    {
        LOG_DBG("East-West direction pedestrian light is green. Vehicle light is unchanged.");

        ew.lightPedestrian = TrafficController::PedestrianTrafficLight::Green;
        setTrafficEastWest(ew);
        broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);


        sn.lightVehicle = TrafficController::VehicleTrafficLight::Red;
        setTrafficSouthNorth( sn );
        broadcastSouthNorth( sn.lightVehicle, sn.lightPedestrian );
    }
    else
    {
        LOG_DBG("North-South direction pedestrian light is green. Vehicle light is unchanged.");

        sn.lightPedestrian = TrafficController::PedestrianTrafficLight::Green;
        setTrafficSouthNorth(sn);
        broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);

        ew.lightVehicle = TrafficController::VehicleTrafficLight::Red;
        setTrafficEastWest( ew );
        broadcastEastWest( ew.lightVehicle, ew.lightPedestrian );
    }

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(sn.lightVehicle), fsm::name(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::name(ew.lightVehicle), fsm::name(ew.lightPedestrian));
}

void TrafficLightService::startupComponent(areg::ComponentThread & comThread)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_startupComponent);

    areg::Component::startupComponent(comThread);
    mLightFsm.initFSM(&comThread);
}

void TrafficLightService::shutdownComponent(areg::ComponentThread & comThread)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_shutdownComponent);

    mLightFsm.releaseFSM();
    areg::Component::shutdownComponent(comThread);
}
