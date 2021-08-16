/************************************************************************
 * \file        ipcfsmsvc/src/TrafficLightController.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This is a Traffic Light controller, which uses finite-state machine (FSM),
 *              implementation of services and a thread consumer.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "ipcfsmsvc/src/TrafficLightController.hpp"

#include "generated/src/NECommon.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_requestPowerOn);
DEF_TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_requestPowerOff);
DEF_TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_requestStartTrafficLight);
DEF_TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_requestStopTrafficLight);
DEF_TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionPowerOff);
DEF_TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionPowerOn);
DEF_TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionVehicleYellow);
DEF_TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionVehicleRed);
DEF_TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionPedestrianRed);
DEF_TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionVehicleGreen);
DEF_TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionPedestrianGreen);
DEF_TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_startupComponent);
DEF_TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_shutdownComponent);

Component * TrafficLightController::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW TrafficLightController(entry, owner);
}

void TrafficLightController::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

TrafficLightController::TrafficLightController(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component                     ( owner, entry.mRoleName )
    , PowerManagerStub              ( static_cast<Component &>(self()) )
    , TrafficControllerStub         ( static_cast<Component &>(self()) )
    , IETrafficLightActionHandler   ( )

    , mConsoleInput                 ( entry.mRoleName, static_cast<Component &>(self()) )
    , mLightFsm                     ( static_cast<IETrafficLightActionHandler &>(self()) )
{

}

TrafficLightController::~TrafficLightController(void)
{
    mLightFsm.releaseFSM();
}

void TrafficLightController::requestPowerOn(void)
{
    TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_requestPowerOn);

    mLightFsm.powerOn();
}

void TrafficLightController::requestPowerOff(void)
{
    TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_requestPowerOff);

    mLightFsm.powerOff();
}

void TrafficLightController::requestStartTrafficLight(void)
{
    TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_requestStartTrafficLight);

    bool result = false;
    const NETrafficController::sTrafficLight & lights = getTrafficEastWest();

    if ((lights.trafficDirection != NETrafficController::DirectionUndefiend) && (lights.lightVehicle != NETrafficController::VhicleLightOFF))
    {
        mLightFsm.startTrafficControl();
        result = true;
    }

    responseStartTrafficLight(result);
}

void TrafficLightController::requestStopTrafficLight(void)
{
    TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_requestStopTrafficLight);

    bool result = false;
    const NETrafficController::sTrafficLight & lights = getTrafficEastWest();

    if ((lights.trafficDirection != NETrafficController::DirectionUndefiend) && (lights.lightVehicle != NETrafficController::VhicleLightOFF))
    {
        mLightFsm.stopTrafficControl();
        result = true;
    }

    responseStopTrafficLight(result);
}

void TrafficLightController::actionPowerOff(void)
{
    TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionPowerOff);
    TRACE_DBG("Handling traffic light power OFF");

    NETrafficController::sTrafficLight lights;
    lights.lightVehicle     = NETrafficController::VhicleLightOFF;
    lights.lightPedestrian  = NETrafficController::PedestrianLightOFF;

    setLightsPowerState(NEPowerManager::LightsOFF);

    lights.trafficDirection = NETrafficController::DirectionSouthNorth;
    setTrafficSouthNorth(lights);

    lights.trafficDirection = NETrafficController::DirectionEastWest;
    setTrafficEastWest(lights);

    broadcastEastWest(NETrafficController::VhicleLightOFF, NETrafficController::PedestrianLightOFF);
    broadcastSouthNorth(NETrafficController::VhicleLightOFF, NETrafficController::PedestrianLightOFF);
}

void TrafficLightController::actionPowerOn(void)
{
    TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionPowerOn);

    TRACE_DBG("Handling traffic light power ON");

    NETrafficController::sTrafficLight lights;
    lights.lightVehicle     = NETrafficController::VehicleLightsInit;
    lights.lightPedestrian  = NETrafficController::PedestrianLightOFF;

    setLightsPowerState(NEPowerManager::LightsON);

    lights.trafficDirection = NETrafficController::DirectionSouthNorth;
    setTrafficSouthNorth(lights);

    lights.trafficDirection = NETrafficController::DirectionEastWest;
    setTrafficEastWest(lights);

    broadcastEastWest(NETrafficController::VehicleLightsInit, NETrafficController::PedestrianLightOFF);
    broadcastSouthNorth(NETrafficController::VehicleLightsInit, NETrafficController::PedestrianLightOFF);
}

void TrafficLightController::actionVehicleYellow(void)
{
    TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionVehicleYellow);
    TRACE_DBG("Vehicle light is yellow, pedestrian is red");

    NETrafficController::sTrafficLight sn, ew;
    
    sn.lightVehicle     = NETrafficController::VehicleLightYellow;
    sn.lightPedestrian  = NETrafficController::PedestrianLightRed;
    sn.trafficDirection = NETrafficController::DirectionSouthNorth;

    ew.lightVehicle     = NETrafficController::VehicleLightYellow;
    ew.lightPedestrian  = NETrafficController::PedestrianLightRed;
    ew.trafficDirection = NETrafficController::DirectionEastWest;

    setTrafficSouthNorth(sn);
    setTrafficEastWest(ew);

    broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);
    broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);

    TRACE_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(sn.lightVehicle), NECommon::getName(sn.lightPedestrian));
    TRACE_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(ew.lightVehicle), NECommon::getName(ew.lightPedestrian));
}

void TrafficLightController::actionVehicleRed(void)
{
    TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionVehicleRed);
    TRACE_DBG("Vehicle light is red. Initially, pedestrian is red");

    NETrafficController::sTrafficLight sn = getTrafficSouthNorth();
    NETrafficController::sTrafficLight ew = getTrafficEastWest();

    sn.lightVehicle     = NETrafficController::VehicleLightRed;
    sn.lightPedestrian  = NETrafficController::PedestrianLightRed;

    setTrafficSouthNorth(sn);
    broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);

    TRACE_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(sn.lightVehicle), NECommon::getName(sn.lightPedestrian));
    TRACE_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(ew.lightVehicle), NECommon::getName(ew.lightPedestrian));
}

void TrafficLightController::actionPedestrianRed(void)
{
    TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionPedestrianRed);
    TRACE_DBG("Pedestrian light is red. Vehicle light is unchanged.");

    TRACE_DBG("Setting traffic light states for direction East-West");
    NETrafficController::sTrafficLight sn = getTrafficSouthNorth();
    NETrafficController::sTrafficLight ew = getTrafficEastWest();

    sn.lightPedestrian  = NETrafficController::PedestrianLightRed;
    ew.lightPedestrian  = NETrafficController::PedestrianLightRed;

    setTrafficSouthNorth(sn);
    setTrafficEastWest(ew);
    broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);
    broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);

    TRACE_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(sn.lightVehicle), NECommon::getName(sn.lightPedestrian));
    TRACE_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(ew.lightVehicle), NECommon::getName(ew.lightPedestrian));
}

void TrafficLightController::actionVehicleGreen( bool isEastWest )
{
    TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionVehicleGreen);

    NETrafficController::sTrafficLight sn = getTrafficSouthNorth();
    NETrafficController::sTrafficLight ew = getTrafficEastWest();

    if (isEastWest)
    {
        TRACE_DBG("East-West direction vehicle light is green. Pedestrian light is unchanged.");

        ew.lightVehicle = NETrafficController::VehicleLightGreen;
        setTrafficEastWest(ew);
        broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);
    }
    else
    {
        TRACE_DBG("North-South direction vehicle light is green. Pedestrian light is unchanged.");

        sn.lightVehicle = NETrafficController::VehicleLightGreen;
        setTrafficSouthNorth(sn);
        broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);
    }

    TRACE_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(sn.lightVehicle), NECommon::getName(sn.lightPedestrian));
    TRACE_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(ew.lightVehicle), NECommon::getName(ew.lightPedestrian));
}

void TrafficLightController::actionPedestrianGreen(bool isEastWest)
{
    TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_actionPedestrianGreen);

    NETrafficController::sTrafficLight sn = getTrafficSouthNorth();
    NETrafficController::sTrafficLight ew = getTrafficEastWest();

    if (isEastWest)
    {
        TRACE_DBG("East-West direction pedestrian light is green. Vehicle light is unchanged.");

        ew.lightPedestrian = NETrafficController::PedestrianLightGreen;
        setTrafficEastWest(ew);
        broadcastEastWest(ew.lightVehicle, ew.lightPedestrian);


        sn.lightVehicle = NETrafficController::VehicleLightRed;
        setTrafficSouthNorth( sn );
        broadcastSouthNorth( sn.lightVehicle, sn.lightPedestrian );
    }
    else
    {
        TRACE_DBG("North-South direction pedestrian light is green. Vehicle light is unchanged.");

        sn.lightPedestrian = NETrafficController::PedestrianLightGreen;
        setTrafficSouthNorth(sn);
        broadcastSouthNorth(sn.lightVehicle, sn.lightPedestrian);

        ew.lightVehicle = NETrafficController::VehicleLightRed;
        setTrafficEastWest( ew );
        broadcastEastWest( ew.lightVehicle, ew.lightPedestrian );
    }

    TRACE_INFO("SOUTH-NORTH : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(sn.lightVehicle), NECommon::getName(sn.lightPedestrian));
    TRACE_INFO("EAST-WEST   : Vehicle [ %6s ], pedestrian [ %6s ]", NECommon::getName(ew.lightVehicle), NECommon::getName(ew.lightPedestrian));
}

void TrafficLightController::startupComponent(ComponentThread & comThread)
{
    TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_startupComponent);

    Component::startupComponent(comThread);
    mLightFsm.initFSM(&comThread);
}

void TrafficLightController::shutdownComponent(ComponentThread & comThread)
{
    TRACE_SCOPE(ipcfsmsvc_src_TrafficLightController_shutdownComponent);

    mLightFsm.releaseFSM();
    Component::shutdownComponent(comThread);
}
