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

    inline const char * name(TrafficController::VehicleTrafficLight value);

    inline const char * name(TrafficController::PedestrianTrafficLight value);
}

inline const char * fsm::name(TrafficController::VehicleTrafficLight value)
{
    switch (value)
    {
    case TrafficController::VehicleTrafficLight::Initializing:
        return "Blink YELLOW";
    case TrafficController::VehicleTrafficLight::Red:
        return "RED";
    case TrafficController::VehicleTrafficLight::Yellow:
        return "YELLOW";
    case TrafficController::VehicleTrafficLight::Green:
        return "GREEN";

    case TrafficController::VehicleTrafficLight::Off:
    default:
        return "OFF";
    }
}

inline const char * fsm::name(TrafficController::PedestrianTrafficLight value)
{
    switch (value)
    {
    case TrafficController::PedestrianTrafficLight::Red:
        return "RED";
    case TrafficController::PedestrianTrafficLight::Green:
        return "GREEN";
    case TrafficController::PedestrianTrafficLight::Off:
    default:
        return "OFF";
    }
}
