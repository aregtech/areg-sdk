#ifndef  AREG_EXAMPLES_14_PUBSVCLIGHT_GENERATED_SRC_NECOMMON_HPP
#define  AREG_EXAMPLES_14_PUBSVCLIGHT_GENERATED_SRC_NECOMMON_HPP

/************************************************************************
 * \file            generated/src/NECommon.hpp
 * \ingroup         SimpleTrafficLight Service Interface
 * \brief           SimpleTrafficLight Service Interface common constants.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/**
 * \brief   Traffic light common names.
 **/
namespace NECommon
{
    /**
     * \brief   The service name of simple traffic light controller.
     **/
    const char * const  SimpleLightControllerName       = "SimpleLightController";

    /**
     * \brief   The prefix of simple traffic light client.
     **/
    const char * const  SimpleLightClientNamePrefix     = "SimpleLight_";

    /**
     * \brief   These are symbolic directions of traffics.
     **/
    typedef enum E_TrafficDirection
    {
          DirectionUnknown      //!< The direction is unknown
        , DirectionSouthNorth   //!< The direction is South-North
        , DirectionEastWest     //!< The direction is East-West.

    } eTrafficDirection;
}

#endif // AREG_EXAMPLES_14_PUBSVCLIGHT_GENERATED_SRC_NECOMMON_HPP
