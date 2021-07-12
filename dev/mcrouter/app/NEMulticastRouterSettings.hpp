#ifndef MCROUTER_APP_NEMULTICASTROUTERSETTINGS_HPP
#define MCROUTER_APP_NEMULTICASTROUTERSETTINGS_HPP
/************************************************************************
 * \file        mcrouter/app/NEMulticastRouterSettings.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Multicast routing to run as process or service.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/GETrace.h"
#include "areg/appbase/NEApplication.hpp"

namespace NEMulticastRouterSettings
{
    typedef enum E_RouterState
    {
          RouterStopped
        , RouterStarting
        , RouterStopping
        , RouterRunning
        , RouterContinuing
        , RouterPausing
        , RouterPaused
    } eRouterState;

    const char * const GetString( const NEMulticastRouterSettings::eRouterState & RouterState );

    const char * const      ServiceNameA            = NEApplication::ROUTER_SERVICE_NAME_ASCII;

    const wchar_t * const   ServiceNameW            = NEApplication::ROUTER_SERVICE_NAME_WIDE;

    const char * const      ServiceCommandInstall   = "--install";

    const char * const      ServiceCommandUninstall = "--uninstall";

    const char * const      ServiceCommandConsole   = "--console";

    const char * const      ServiceCommandService   = "--service";

    const char * const      RouterThreadName        = "_AREG_SERVICEROUTER_THREAD_";
}

inline const char * const NEMulticastRouterSettings::GetString( const NEMulticastRouterSettings::eRouterState & RouterState )
{
    switch ( RouterState )
    {
    case NEMulticastRouterSettings::RouterStopped:
        return "NEMulticastRouterSettings::RouterStopped";
    case NEMulticastRouterSettings::RouterStarting:
        return "NEMulticastRouterSettings::RouterStarting";
    case NEMulticastRouterSettings::RouterStopping:
        return "NEMulticastRouterSettings::RouterStopping";
    case NEMulticastRouterSettings::RouterRunning:
        return "NEMulticastRouterSettings::RouterRunning";
    case NEMulticastRouterSettings::RouterContinuing:
        return "NEMulticastRouterSettings::RouterContinuing";
    case NEMulticastRouterSettings::RouterPausing:
        return "NEMulticastRouterSettings::RouterPausing";
    case NEMulticastRouterSettings::RouterPaused:
        return "NEMulticastRouterSettings::RouterPaused";
    default:
        ASSERT(false);
        return "ERR: Undefined NEMulticastRouterSettings::eRouterState value!!!";
    }
}

#endif  // MCROUTER_APP_NEMULTICASTROUTERSETTINGS_HPP
