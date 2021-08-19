#ifndef  AREG_EXAMPLES_16_PUBFSM_GENERATED_SRC_NECOMMON_HPP
#define  AREG_EXAMPLES_16_PUBFSM_GENERATED_SRC_NECOMMON_HPP

/************************************************************************
 * \file            generated/src/NECommon.hpp
 * \ingroup         Patient monitoring Service
 * \brief           Common constants and service names for Patient monitoring Service.
 ************************************************************************/

 /************************************************************************
  * Include files
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "generated/src/NETrafficController.hpp"

namespace NECommon
{

    const char * const  ServiceLightController      = "LightController";

    const char * const  SerivceLightClient          = "LightUser";

    inline const char * getName(NETrafficController::eVehicleTrafficLight value);

    inline const char * getName(NETrafficController::ePedestrianTrafficLight value);
}

inline const char * NECommon::getName(NETrafficController::eVehicleTrafficLight value)
{
    switch (value)
    {
    case NETrafficController::VehicleLightsInit:
        return "Blink YELLOW";
    case NETrafficController::VehicleLightRed:
        return "RED";
    case NETrafficController::VehicleLightYellow:
        return "YELLOW";
    case NETrafficController::VehicleLightGreen:
        return "GREEN";

    case NETrafficController::VhicleLightOFF:
    default:
        return "OFF";
    }
}

inline const char * NECommon::getName(NETrafficController::ePedestrianTrafficLight value)
{
    switch (value)
    {
    case NETrafficController::PedestrianLightRed:
        return "RED";
    case NETrafficController::PedestrianLightGreen:
        return "GREEN";
    case NETrafficController::PedestrianLightOFF:
    default:
        return "OFF";
    }
}

#endif  // AREG_EXAMPLES_16_PUBFSM_GENERATED_SRC_NECOMMON_HPP
