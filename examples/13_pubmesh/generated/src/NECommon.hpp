#pragma once

/************************************************************************
 * \file        generated/src/NECommon.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"

/**
 * \brief   List of service names of the same remote and one local service interface that is present in every process.
 *          The agenda is to demonstrate uniqueness of services:
 *              - Remote services should be unique in the entire network.
 *              - Local services should be unique in the process.
 *            
 **/
namespace NECommon
{
    //!< The main service that is responsible to shutdown the complete system.
    constexpr char  MainService[]           { "TestPublicMainService" };

    //!< Third remote service that is implemented in "13_secondipcmix" process.
    constexpr char  PublicSecondaryService[]{ "TestPublicSecondService" };

    //!< Third remote service that is implemented in "13_secondipcmix" process.
    constexpr char  PublicThirdService[]    { "TestPublicThirdService" };

    //! At least one local service in each process.
    constexpr char  LocalService[]          { "TestLocalService" };
    
    //!< Client component to call services in each servicing component.
    constexpr char ClientServicePrefix[]    { "TestClientService" };

    //!< The name of model, common in every process.
    constexpr char  ModelName[]             { "TestModel" };
}
