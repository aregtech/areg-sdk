#pragma once

/************************************************************************
 * \file            generated/src/NECommon.hpp
 * \ingroup         Patient monitoring Service
 * \brief           Common constants and service names for Patient monitoring Service.
 ************************************************************************/

 /************************************************************************
  * Include files
  ************************************************************************/
#include "areg/base/GEGlobal.h"

/**
 * \brief   Common data for projects.
 **/
namespace NECommon
{
    /**
     * \brief   The Patient Information service name.
     **/
    constexpr char  ServiceNamePatientInfo[]    { "PatientInfo" };

    /**
     * \brief   The hardware manager service name.
     **/
    constexpr char  ServiceNameHwManager[]      { "PatientClient" };
}
