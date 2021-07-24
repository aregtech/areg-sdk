#ifndef AREG_APPBASE_NEAPPLICATION_HPP
#define AREG_APPBASE_NEAPPLICATION_HPP
/************************************************************************
 * \file        areg/appbase/NEApplication.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Application global namespace and configurations
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/**
 * \brief   In this namespace contains application default configuration values.
 **/
namespace NEApplication
{
    /**
     * \brief   The default path of tracing configuration file.
     *          The path is relative to the executable folder.
     **/
    const char * const      DEFAULT_TRACING_CONFIG_FILE     = "./config/log.init";

    /**
     * \brief   The default path of message routing configuration file.
     *          The path is relative to the executable folder.
     **/
    const char * const      DEFAULT_ROUTER_CONFIG_FILE      = "./config/router.init";

    /**
     * \brief   ASCII version of service interface router service name
     **/
    const char * const      ROUTER_SERVICE_NAME_ASCII       = "AREG TCP/IP connection Message Multicast Router Service";

    /**
     * \brief   Unicode version of service interface router service name
     **/
    const wchar_t * const   ROUTER_SERVICE_NAME_WIDE        = L"AREG TCP/IP connection Message Multicast Router Service";

    /**
     * \brief   ASCII version of trace manager server service name
     **/
    const char * const      TRACING_SERVICE_NAME_ASCII      = "AREG Remote Logging Service";

    /**
     * \brief   Unicode version of trace manager server service name
     **/
    const wchar_t * const   TRACING_SERVICE_NAME_WIDE       = L"AREG Remote Logging Service";

    /**
     * \brief   The default name of Multicast Router.
     **/
    const char * const      DEFAULT_ROUTER_SERVICE_NAME     = "mcrouter";
}

#endif  // AREG_APPBASE_NEAPPLICATION_HPP
