#ifndef AREG_EXAMPLES_13_PUBSVCMIX_GENERATED_SRC_IPCMIXCOMMON_HPP
#define AREG_EXAMPLES_13_PUBSVCMIX_GENERATED_SRC_IPCMIXCOMMON_HPP
/************************************************************************
 * \file        generated/src/IPCMixCommon.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
namespace IPCMixCommon
{
    //!< The main service that is responsible to shutdown the complete system.
    const char * const  MainService             = "TestPublicMainService";

    //!< Third remote service that is implemented in "13_secondipcmix" process.
    const char * const  PublicSecondaryService  = "TestPublicSecondService";

    //!< Third remote service that is implemented in "13_secondipcmix" process.
    const char * const  PublicThirdService      = "TestPublicThirdService";

    //! At least one local service in each process.
    const char * const  LocalService            = "TestLocalService";
    
    //!< Client component to call services in each servicing component.
    const char * const ClientServicePrefix      = "TestClientService";

    //!< The name of model, common in every process.
    const char * const  ModelName               = "TestModel";
}

#endif  // AREG_EXAMPLES_13_PUBSVCMIX_GENERATED_SRC_IPCMIXCOMMON_HPP
