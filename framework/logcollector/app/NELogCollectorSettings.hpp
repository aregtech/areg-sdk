#ifndef AREG_LOGCOLLECTOR_APP_NELOGCOLLECTORSETTINGS_HPP
#define AREG_LOGCOLLECTOR_APP_NELOGCOLLECTORSETTINGS_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        logcollector/app/NELogCollectorSettings.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Collector settings.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/logging/GELog.h"
#include "areg/appbase/NEApplication.hpp"

/**
 * \brief   Basic constants of the Log Collector service
 **/
namespace NELogCollectorSettings
{
    /**
     * \brief   The ASCII name of the Log Collector as a system service.
     **/
    extern char *       SERVICE_NAME_ASCII  /*= NEApplication::LOGGER_SERVICE_NAME_ASCII*/;

    /**
     * \brief   The UNICODE name of the Log Collector as a system service.
     **/
    extern wchar_t *    SERVICE_NAME_WIDE   /*= NEApplication::LOGGER_SERVICE_NAME_WIDE*/;

    /**
     * \brief   The ASCII display name of the Log Collector service to show in the service list.
     **/
    extern char         SERVICE_DISPLAY_NAME_ASCII[];

    /**
     * \brief   The UNICODE name of the Log Collector service to show in the service list.
     **/
    extern wchar_t      SERVICE_DISPLAY_NAME_WIDE[];

    /**
     * \brief   The ASCII description of the Log Collector service to show in the service list.
     **/
    extern char         SERVICE_DESCRIBE_ASCII[];

    /**
     * \brief   The UNICODE description of the Log Collector service to show in the service list.
     **/
    extern wchar_t      SERVICE_DESCRIBE_WIDE[];

    /**
     * \brief   The name of main thread in the Log Collector.
     **/
    constexpr char      LoggerThreadName[]  { "_AREG_LOG_COLLECTOR_THREAD_" };

    /**
     * \brief   The title of application to output on console
     **/
    constexpr std::string_view  APP_TITLE   { "AREG Log Collector console application ..." };

} // namespace NELogCollectorSettings

#endif  // AREG_LOGCOLLECTOR_APP_NELOGCOLLECTORSETTINGS_HPP
