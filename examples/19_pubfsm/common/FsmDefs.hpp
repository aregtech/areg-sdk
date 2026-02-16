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

#include "examples/19_pubfsm/services/NETrafficController.hpp"

namespace fsm
{

    constexpr char  ServiceLightController[]    { "LightController" };

    constexpr char  SerivceLightClient[]        { "LightUser" };

    inline const char * getName(NETrafficController::eVehicleTrafficLight value);

    inline const char * getName(NETrafficController::ePedestrianTrafficLight value);
}

inline const char * fsm::getName(NETrafficController::eVehicleTrafficLight value)
{
    switch (value)
    {
    case NETrafficController::eVehicleTrafficLight::VehicleLightsInit:
        return "Blink YELLOW";
    case NETrafficController::eVehicleTrafficLight::VehicleLightRed:
        return "RED";
    case NETrafficController::eVehicleTrafficLight::VehicleLightYellow:
        return "YELLOW";
    case NETrafficController::eVehicleTrafficLight::VehicleLightGreen:
        return "GREEN";

    case NETrafficController::eVehicleTrafficLight::VehicleLightOFF:
    default:
        return "OFF";
    }
}

inline const char * fsm::getName(NETrafficController::ePedestrianTrafficLight value)
{
    switch (value)
    {
    case NETrafficController::ePedestrianTrafficLight::PedestrianLightRed:
        return "RED";
    case NETrafficController::ePedestrianTrafficLight::PedestrianLightGreen:
        return "GREEN";
    case NETrafficController::ePedestrianTrafficLight::PedestrianLightOFF:
    default:
        return "OFF";
    }
}
