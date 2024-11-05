#ifndef AREG_MCROUTER_APP_NEMULTICASTROUTERSETTINGS_HPP
#define AREG_MCROUTER_APP_NEMULTICASTROUTERSETTINGS_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/NEMulticastRouterSettings.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing settings.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/GETrace.h"
#include "areg/appbase/NEApplication.hpp"

/**
 * \brief   Basic constants of the mulitast router service
 **/
namespace NEMulticastRouterSettings
{
    /**
     * \brief   The ASCII name of the multicast router as a system service.
     **/
    extern char *       SERVICE_NAME_ASCII  /*= NEApplication::ROUTER_SERVICE_NAME_ASCII*/;

    /**
     * \brief   The UNICODE name of the multicast router as a system service.
     **/
    extern wchar_t *    SERVICE_NAME_WIDE   /*= NEApplication::ROUTER_SERVICE_NAME_WIDE*/;

    /**
     * \brief   The ASCII display name of the multicast router service to show in the service list.
     **/
    extern char         SERVICE_DISPLAY_NAME_ASCII[];

    /**
     * \brief   The UNICODE name of the multicast router service to show in the service list.
     **/
    extern wchar_t      SERVICE_DISPLAY_NAME_WIDE[];

    /**
     * \brief   The ASCII description of the multicast router service to show in the service list.
     **/
    extern char         SERVICE_DESCRIBE_ASCII[];

    /**
     * \brief   The UNICODE description of the multicast router service to show in the service list.
     **/
    extern wchar_t      SERVICE_DESCRIBE_WIDE[];

    /**
     * \brief   The name of main thread in the multicast router.
     **/
    constexpr char      RouterThreadName[]  { "_AREG_SERVICEROUTER_THREAD_" };

    /**
     * \brief   The title to display on console when run application.
     **/
    constexpr std::string_view   APP_TITLE  { "AREG Message Router console application ..." };

} // namespace NEMulticastRouterSettings

#endif  // AREG_MCROUTER_APP_NEMULTICASTROUTERSETTINGS_HPP
