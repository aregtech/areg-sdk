/************************************************************************
 * \file        pubservice/src/TrafficLightService.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This is a Traffic Light controller, which uses finite-state machine (FSM),
 *              implementation of services and a thread consumer.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "pubservice/src/TrafficLightService.hpp"

#include "common/NECommon.hpp"
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

Component * TrafficLightService::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW TrafficLightService(entry, owner);
}

void TrafficLightService::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & /* entry */)
{
    delete (&compObject);
}

TrafficLightService::TrafficLightService(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component                     ( entry, owner )
    , PowerManagerStub              ( static_cast<Component &>(self()) )
    , TrafficControllerStub         ( static_cast<Component &>(self()) )
    , IETrafficLightActionHandler   ( )

    , mConsoleInput                 ( entry.mRoleName, static_cast<Component &>(self()) )
    , mLightFsm                     ( static_cast<IETrafficLightActionHandler &>(self()) )
{

}

TrafficLightService::~TrafficLightService(void)
{
    mLightFsm.releaseFSM();
}

void TrafficLightService::requestPowerOn(void)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_requestPowerOn);

    mLightFsm.powerOn();
}

void TrafficLightService::requestPowerOff(void)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_requestPowerOff);

    mLightFsm.powerOff();
}

void TrafficLightService::requestStartTrafficLight(void)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_requestStartTrafficLight);

    bool result = false;
    const NETrafficController::sTrafficLight & lights = getTrafficEastWest();

    if ((lights.trafficDirection != NETrafficController::eTrafficDirection::DirectionUndefiend) && (lights.lightVehicle != NETrafficController::eVehicleTrafficLight::VehicleLightOFF))
    {
        mLightFsm.startTrafficControl();
        result = true;
    }

    responseStartTrafficLight(result);
}

void TrafficLightService::requestStopTrafficLight(void)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_requestStopTrafficLight);

    bool result = false;
    const NETrafficController::sTrafficLight & lights = getTrafficEastWest();

    if ((lights.trafficDirection != NETrafficController::eTrafficDirection::DirectionUndefiend) && (lights.lightVehicle != NETrafficController::eVehicleTrafficLight::VehicleLightOFF))
    {
        mLightFsm.stopTrafficControl();
        result = true;
    }

    responseStopTrafficLight(result);
}

void TrafficLightService::actionPowerOff(void)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPowerOff);
    LOG_DBG("Handling traffic light power OFF");

    NETrafficController::sTrafficLight lights;
    lights.lightVehicle     = NETrafficController::eVehicleTrafficLight::VehicleLightOFF;
    lights.lightPedestrian  = NETrafficController::ePedestrianTrafficLight::PedestrianLightOFF;

    setLightsPowerState(NEPowerManager::ePoweredState::LightsOFF);

    lights.trafficDirection = NETrafficController::eTrafficDirection::DirectionSouthNorth;
    setTrafficSouthNorth(lights);

    lights.trafficDirection = NETrafficController::eTrafficDirection::DirectionEastWest;
    setTrafficEastWest(lights);

    broadcastEastWest(NETrafficController::eVehicleTrafficLight::VehicleLightOFF, NETrafficController::ePedestrianTrafficLight::PedestrianLightOFF);
    broadcastSouthNorth(NETrafficController::eVehicleTrafficLight::VehicleLightOFF, NETrafficController::ePedestrianTrafficLight::PedestrianLightOFF);
}

void TrafficLightService::actionPowerOn(void)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPowerOn);

    LOG_DBG("Handling traffic light power ON");

    NETrafficController::sTrafficLight lights;
    lights.lightVehicle     = NETrafficController::eVehicleTrafficLight::VehicleLightsInit;
    lights.lightPedestrian  = NETrafficController::ePedestrianTrafficLight::PedestrianLightOFF;

    setLightsPowerState(NEPowerManager::ePoweredState::LightsON);

    lights.trafficDirection = NETrafficController::eTrafficDirection::DirectionSouthNorth;
    setTrafficSouthNorth(lights);

    lights.trafficDirection = NETrafficController::eTrafficDirection::DirectionEastWest;
    setTrafficEastWest(lights);

    broadcastEastWest(NETrafficController::eVehicleTrafficLight::VehicleLightsInit, NETrafficController::ePedestrianTrafficLight::PedestrianLightOFF);
    broadcastSouthNorth(NETrafficController::eVehicleTrafficLight::VehicleLightsInit, NETrafficController::ePedestrianTrafficLight::PedestrianLightOFF);
}

void TrafficLightService::actionVehicleYellow(void)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleYellow);
    LOG_DBG("Vehicle light is yellow, pedestrian is red");

    NETrafficController::sTrafficLight sn, ew;
    
    sn.lightVehicle     = NETrafficController::eVehicleTrafficLight::VehicleLightYellow;
    sn.lightPedestrian  = NETrafficController::ePedestrianTrafficLight::PedestrianLightRed;
    sn.trafficDirection = NETrafficController::eTrafficDirection::DirectionSouthNorth;

    ew.lightVehicle     = NETrafficController::eVehicleTrafficLight::VehicleLightYellow;
    ew.lightPedestrian  = NETrafficController::ePedestrianTrafficLight::PedestrianLightRed;
    ew.trafficDirection = NETrafficController::eTrafficDirection::DirectionEastWest;

    setTrafficSouthNorth(sn);
    setTrafficEastWest(ew);

    broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);
    broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(sn.lightVehicle), NECommon::getName(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(ew.lightVehicle), NECommon::getName(ew.lightPedestrian));
}

void TrafficLightService::actionVehicleRed(void)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleRed);
    LOG_DBG("Vehicle light is red. Initially, pedestrian is red");

    NETrafficController::sTrafficLight sn = getTrafficSouthNorth();
    NETrafficController::sTrafficLight ew = getTrafficEastWest();

    sn.lightVehicle     = NETrafficController::eVehicleTrafficLight::VehicleLightRed;
    sn.lightPedestrian  = NETrafficController::ePedestrianTrafficLight::PedestrianLightRed;

    setTrafficSouthNorth(sn);
    broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(sn.lightVehicle), NECommon::getName(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(ew.lightVehicle), NECommon::getName(ew.lightPedestrian));
}

void TrafficLightService::actionPedestrianRed(void)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPedestrianRed);
    LOG_DBG("Pedestrian light is red. Vehicle light is unchanged.");

    LOG_DBG("Setting traffic light states for direction East-West");
    NETrafficController::sTrafficLight sn = getTrafficSouthNorth();
    NETrafficController::sTrafficLight ew = getTrafficEastWest();

    sn.lightPedestrian  = NETrafficController::ePedestrianTrafficLight::PedestrianLightRed;
    ew.lightPedestrian  = NETrafficController::ePedestrianTrafficLight::PedestrianLightRed;

    setTrafficSouthNorth(sn);
    setTrafficEastWest(ew);
    broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);
    broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(sn.lightVehicle), NECommon::getName(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(ew.lightVehicle), NECommon::getName(ew.lightPedestrian));
}

void TrafficLightService::actionVehicleGreen( bool isEastWest )
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionVehicleGreen);

    NETrafficController::sTrafficLight sn = getTrafficSouthNorth();
    NETrafficController::sTrafficLight ew = getTrafficEastWest();

    if (isEastWest)
    {
        LOG_DBG("East-West direction vehicle light is green. Pedestrian light is unchanged.");

        ew.lightVehicle = NETrafficController::eVehicleTrafficLight::VehicleLightGreen;
        setTrafficEastWest(ew);
        broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);
    }
    else
    {
        LOG_DBG("North-South direction vehicle light is green. Pedestrian light is unchanged.");

        sn.lightVehicle = NETrafficController::eVehicleTrafficLight::VehicleLightGreen;
        setTrafficSouthNorth(sn);
        broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);
    }

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(sn.lightVehicle), NECommon::getName(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(ew.lightVehicle), NECommon::getName(ew.lightPedestrian));
}

void TrafficLightService::actionPedestrianGreen(bool isEastWest)
{
    LOG_SCOPE(19_pubfsm_pubservice_src_TrafficLightService_actionPedestrianGreen);

    NETrafficController::sTrafficLight sn = getTrafficSouthNorth();
    NETrafficController::sTrafficLight ew = getTrafficEastWest();

    if (isEastWest)
    {
        LOG_DBG("East-West direction pedestrian light is green. Vehicle light is unchanged.");

        ew.lightPedestrian = NETrafficController::ePedestrianTrafficLight::PedestrianLightGreen;
        setTrafficEastWest(ew);
        broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);


        sn.lightVehicle = NETrafficController::eVehicleTrafficLight::VehicleLightRed;
        setTrafficSouthNorth( sn );
        broadcastSouthNorth( sn.lightVehicle, sn.lightPedestrian );
    }
    else
    {
        LOG_DBG("North-South direction pedestrian light is green. Vehicle light is unchanged.");

        sn.lightPedestrian = NETrafficController::ePedestrianTrafficLight::PedestrianLightGreen;
        setTrafficSouthNorth(sn);
        broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);

        ew.lightVehicle = NETrafficController::eVehicleTrafficLight::VehicleLightRed;
        setTrafficEastWest( ew );
        broadcastEastWest( ew.lightVehicle, ew.lightPedestrian );
    }

    LOG_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(sn.lightVehicle), NECommon::getName(sn.lightPedestrian));
    LOG_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(ew.lightVehicle), NECommon::getName(ew.lightPedestrian));
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
