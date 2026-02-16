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
#include "areg/logging/GELog.h"

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

TrafficLightService::TrafficLightService(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component                     ( entry, owner )
    , PowerManagerStub              ( static_cast<Component &>(self()) )
    , TrafficControllerStub         ( static_cast<Component &>(self()) )
    , IETrafficLightActionHandler   ( )

    , mConsoleInput                 ( entry.mRoleName, static_cast<Component &>(self()) )
    , mLightFsm                     ( static_cast<IETrafficLightActionHandler &>(self()) )
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

    if ((lights.trafficDirection != TrafficController::eTrafficDirection::DirectionUndefiend) && (lights.lightVehicle != TrafficController::eVehicleTrafficLight::VehicleLightOFF))
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

    if ((lights.trafficDirection != TrafficController::eTrafficDirection::DirectionUndefiend) && (lights.lightVehicle != TrafficController::eVehicleTrafficLight::VehicleLightOFF))
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
    lights.lightVehicle     = TrafficController::eVehicleTrafficLight::VehicleLightOFF;
    lights.lightPedestrian  = TrafficController::ePedestrianTrafficLight::PedestrianLightOFF;

    setLightsPowerState(PowerManager::ePoweredState::LightsOFF);

    lights.trafficDirection = TrafficController::eTrafficDirection::DirectionSouthNorth;
    setTrafficSouthNorth(lights);

    lights.trafficDirection = TrafficController::eTrafficDirection::DirectionEastWest;
    setTrafficEastWest(lights);

    broadcastEastWest(TrafficController::eVehicleTrafficLight::VehicleLightOFF, TrafficController::ePedestrianTrafficLight::PedestrianLightOFF);
    broadcastSouthNorth(TrafficController::eVehicleTrafficLight::VehicleLightOFF, TrafficController::ePedestrianTrafficLight::PedestrianLightOFF);
}

void TrafficLightService::actionPowerOn()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPowerOn);

    LOG_DBG("Handling traffic light power ON");

    TrafficController::sTrafficLight lights;
    lights.lightVehicle     = TrafficController::eVehicleTrafficLight::VehicleLightsInit;
    lights.lightPedestrian  = TrafficController::ePedestrianTrafficLight::PedestrianLightOFF;

    setLightsPowerState(PowerManager::ePoweredState::LightsON);

    lights.trafficDirection = TrafficController::eTrafficDirection::DirectionSouthNorth;
    setTrafficSouthNorth(lights);

    lights.trafficDirection = TrafficController::eTrafficDirection::DirectionEastWest;
    setTrafficEastWest(lights);

    broadcastEastWest(TrafficController::eVehicleTrafficLight::VehicleLightsInit, TrafficController::ePedestrianTrafficLight::PedestrianLightOFF);
    broadcastSouthNorth(TrafficController::eVehicleTrafficLight::VehicleLightsInit, TrafficController::ePedestrianTrafficLight::PedestrianLightOFF);
}

void TrafficLightService::actionVehicleYellow()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleYellow);
    LOG_DBG("Vehicle light is yellow, pedestrian is red");

    TrafficController::sTrafficLight sn, ew;
    
    sn.lightVehicle     = TrafficController::eVehicleTrafficLight::VehicleLightYellow;
    sn.lightPedestrian  = TrafficController::ePedestrianTrafficLight::PedestrianLightRed;
    sn.trafficDirection = TrafficController::eTrafficDirection::DirectionSouthNorth;

    ew.lightVehicle     = TrafficController::eVehicleTrafficLight::VehicleLightYellow;
    ew.lightPedestrian  = TrafficController::ePedestrianTrafficLight::PedestrianLightRed;
    ew.trafficDirection = TrafficController::eTrafficDirection::DirectionEastWest;

    setTrafficSouthNorth(sn);
    setTrafficEastWest(ew);

    broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);
    broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::getName(sn.lightVehicle), fsm::getName(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::getName(ew.lightVehicle), fsm::getName(ew.lightPedestrian));
}

void TrafficLightService::actionVehicleRed()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleRed);
    LOG_DBG("Vehicle light is red. Initially, pedestrian is red");

    TrafficController::sTrafficLight sn = getTrafficSouthNorth();
    TrafficController::sTrafficLight ew = getTrafficEastWest();

    sn.lightVehicle     = TrafficController::eVehicleTrafficLight::VehicleLightRed;
    sn.lightPedestrian  = TrafficController::ePedestrianTrafficLight::PedestrianLightRed;

    setTrafficSouthNorth(sn);
    broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::getName(sn.lightVehicle), fsm::getName(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::getName(ew.lightVehicle), fsm::getName(ew.lightPedestrian));
}

void TrafficLightService::actionPedestrianRed()
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPedestrianRed);
    LOG_DBG("Pedestrian light is red. Vehicle light is unchanged.");

    LOG_DBG("Setting traffic light states for direction East-West");
    TrafficController::sTrafficLight sn = getTrafficSouthNorth();
    TrafficController::sTrafficLight ew = getTrafficEastWest();

    sn.lightPedestrian  = TrafficController::ePedestrianTrafficLight::PedestrianLightRed;
    ew.lightPedestrian  = TrafficController::ePedestrianTrafficLight::PedestrianLightRed;

    setTrafficSouthNorth(sn);
    setTrafficEastWest(ew);
    broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);
    broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::getName(sn.lightVehicle), fsm::getName(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::getName(ew.lightVehicle), fsm::getName(ew.lightPedestrian));
}

void TrafficLightService::actionVehicleGreen( bool isEastWest )
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleGreen);

    TrafficController::sTrafficLight sn = getTrafficSouthNorth();
    TrafficController::sTrafficLight ew = getTrafficEastWest();

    if (isEastWest)
    {
        LOG_DBG("East-West direction vehicle light is green. Pedestrian light is unchanged.");

        ew.lightVehicle = TrafficController::eVehicleTrafficLight::VehicleLightGreen;
        setTrafficEastWest(ew);
        broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);
    }
    else
    {
        LOG_DBG("North-South direction vehicle light is green. Pedestrian light is unchanged.");

        sn.lightVehicle = TrafficController::eVehicleTrafficLight::VehicleLightGreen;
        setTrafficSouthNorth(sn);
        broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);
    }

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::getName(sn.lightVehicle), fsm::getName(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::getName(ew.lightVehicle), fsm::getName(ew.lightPedestrian));
}

void TrafficLightService::actionPedestrianGreen(bool isEastWest)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPedestrianGreen);

    TrafficController::sTrafficLight sn = getTrafficSouthNorth();
    TrafficController::sTrafficLight ew = getTrafficEastWest();

    if (isEastWest)
    {
        LOG_DBG("East-West direction pedestrian light is green. Vehicle light is unchanged.");

        ew.lightPedestrian = TrafficController::ePedestrianTrafficLight::PedestrianLightGreen;
        setTrafficEastWest(ew);
        broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);


        sn.lightVehicle = TrafficController::eVehicleTrafficLight::VehicleLightRed;
        setTrafficSouthNorth( sn );
        broadcastSouthNorth( sn.lightVehicle, sn.lightPedestrian );
    }
    else
    {
        LOG_DBG("North-South direction pedestrian light is green. Vehicle light is unchanged.");

        sn.lightPedestrian = TrafficController::ePedestrianTrafficLight::PedestrianLightGreen;
        setTrafficSouthNorth(sn);
        broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);

        ew.lightVehicle = TrafficController::eVehicleTrafficLight::VehicleLightRed;
        setTrafficEastWest( ew );
        broadcastEastWest( ew.lightVehicle, ew.lightPedestrian );
    }

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::getName(sn.lightVehicle), fsm::getName(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", fsm::getName(ew.lightVehicle), fsm::getName(ew.lightPedestrian));
}

void TrafficLightService::startupComponent(ComponentThread & comThread)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_startupComponent);

    Component::startupComponent(comThread);
    mLightFsm.initFSM(&comThread);
}

void TrafficLightService::shutdownComponent(ComponentThread & comThread)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_shutdownComponent);

    mLightFsm.releaseFSM();
    Component::shutdownComponent(comThread);
}
