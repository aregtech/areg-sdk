#pragma once

/************************************************************************
 * \file            res/NECommon.hpp
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
    constexpr char  SimpleLightControllerName[]     { "SimpleLightController" };

    /**
     * \brief   The prefix of simple traffic light client.
     **/
    constexpr char  SimpleLightClientNamePrefix[]   { "SimpleLight_" };

    /**
     * \brief   These are symbolic directions of traffics.
     **/
    enum class eTrafficDirection
    {
          DirectionUnknown      //!< The direction is unknown
        , DirectionSouthNorth   //!< The direction is South-North
        , DirectionEastWest     //!< The direction is East-West.

    };
}
