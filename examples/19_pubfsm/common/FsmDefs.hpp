#pragma once

/************************************************************************
 * \file            generated/src/FsmDefs.hpp
 * \ingroup         Patient monitoring Service
 * \brief           Common constants and service names for Patient monitoring Service.
 ************************************************************************/

 /************************************************************************
  * Include files
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "examples/19_pubfsm/services/TrafficController.hpp"


namespace fsm
{

    constexpr char  ServiceLightController[]    { "LightController" };

    constexpr char  SerivceLightClient[]        { "LightUser" };

    inline const char * getName(TrafficController::eVehicleTrafficLight value);

    inline const char * getName(TrafficController::ePedestrianTrafficLight value);
}

inline const char * fsm::getName(TrafficController::eVehicleTrafficLight value)
{
    switch (value)
    {
    case TrafficController::eVehicleTrafficLight::VehicleLightsInit:
        return "Blink YELLOW";
    case TrafficController::eVehicleTrafficLight::VehicleLightRed:
        return "RED";
    case TrafficController::eVehicleTrafficLight::VehicleLightYellow:
        return "YELLOW";
    case TrafficController::eVehicleTrafficLight::VehicleLightGreen:
        return "GREEN";

    case TrafficController::eVehicleTrafficLight::VehicleLightOFF:
    default:
        return "OFF";
    }
}

inline const char * fsm::getName(TrafficController::ePedestrianTrafficLight value)
{
    switch (value)
    {
    case TrafficController::ePedestrianTrafficLight::PedestrianLightRed:
        return "RED";
    case TrafficController::ePedestrianTrafficLight::PedestrianLightGreen:
        return "GREEN";
    case TrafficController::ePedestrianTrafficLight::PedestrianLightOFF:
    default:
        return "OFF";
    }
}
