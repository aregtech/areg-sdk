#ifndef AREG_LOGGER_APP_NELOGGERSETTINGS_HPP
#define AREG_LOGGER_APP_NELOGGERSETTINGS_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logger/app/NELoggerSettings.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Logger settings.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/GETrace.h"
#include "areg/appbase/NEApplication.hpp"

/**
 * \brief   Basic constants of the logger service
 **/
namespace NELoggerSettings
{
    /**
     * \brief   The ASCII name of the logger as a system service.
     **/
    extern char *       SERVICE_NAME_ASCII  /*= NEApplication::LOGGER_SERVICE_NAME_ASCII*/;
    /**
     * \brief   The UNICODE name of the logger as a system service.
     **/
    extern wchar_t *    SERVICE_NAME_WIDE   /*= NEApplication::LOGGER_SERVICE_NAME_WIDE*/;

    /**
     * \brief   The name of main thread in the logger.
     **/
    constexpr char      LoggerThreadName[]  { "_AREG_SERVICELOGGER_THREAD_" };

    /**
     * \brief   The title of application to output on console
     **/
    constexpr std::string_view  APP_TITLE   { "AREG Log Collector console application ..." };

} // namespace NELoggerSettings

#endif  // AREG_LOGGER_APP_NELOGGERSETTINGS_HPP
