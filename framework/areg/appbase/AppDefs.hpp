#ifndef AREG_APPBASE_APPDEFS_HPP
#define AREG_APPBASE_APPDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/appbase/AppDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Application global namespace and configurations
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/Identifier.hpp"
#include <string_view>
#include <vector>

/**
 * \brief   In this namespace contains application default configuration values.
 **/
namespace areg {

    /**
     * \brief   areg::DEFAULT_CONFIG_FILE
     *          The default path of tracing configuration file.
     *          The path is relative to the executable folder.
     **/
    constexpr std::string_view  DEFAULT_CONFIG_FILE             { "./config/areg.init" };

    /**
     * \brief   areg::ROUTER_SERVICE_NAME_ASCII
     *          ASCII version of router service name.
     **/
    extern AREG_API char        ROUTER_SERVICE_NAME_ASCII[];

    /**
     * \brief   areg::ROUTER_SERVICE_NAME_WIDE
     *          Unicode version of router service name.
     **/
    extern AREG_API wchar_t     ROUTER_SERVICE_NAME_WIDE[];

    /**
     * \brief   areg::ROUTER_SERVICE_EXECUTABLE_ASCII
     *          ASCII version of router service executable.
     **/
    extern AREG_API char        ROUTER_SERVICE_EXECUTABLE_ASCII[];

    /**
     * \brief   areg::ROUTER_SERVICE_EXECUTABLE_WIDE
     *          Unicode version of router service executable.
     **/
    extern AREG_API wchar_t     ROUTER_SERVICE_EXECUTABLE_WIDE[];

    /**
     * \brief   areg::LOGGER_SERVICE_NAME_ASCII
     *          ASCII version of log collector service name.
     **/
    extern AREG_API char        LOGGER_SERVICE_NAME_ASCII[];

    /**
     * \brief   areg::LOGGER_SERVICE_NAME_WIDE
     *          Unicode version of log collector service name.
     **/
    extern AREG_API wchar_t     LOGGER_SERVICE_NAME_WIDE[];

    /**
     * \brief   areg::LOGGER_SERVICE_EXECUTABLE_ASCII
     *          ASCII version of log collector service executable.
     **/
    extern AREG_API char        LOGGER_SERVICE_EXECUTABLE_ASCII[];

    /**
     * \brief   areg::LOGGER_SERVICE_EXECUTABLE_WIDE
     *          Unicode version of log collector service executable.
     **/
    extern AREG_API wchar_t     LOGGER_SERVICE_EXECUTABLE_WIDE[];

    /**
     * \brief   areg::DEFAULT_ROUTER_SERVICE_NAME
     *          The default name of Multitarget Router.
     **/
    constexpr std::string_view  DEFAULT_ROUTER_SERVICE_NAME { "mtrouter" };

    /**
     * \brief   areg::DEFAULT_LOGGER_SERVICE_NAME
     *          The default name of Log Collector.
     **/
    constexpr std::string_view  DEFAULT_LOGGER_SERVICE_NAME { "logcollector" };

    /**
     * \brief   areg::DEFAULT_SERVICE_ENABLED
     *          Message router enable / disable default flag. If true, by default it is enabled.
     *          The default values are used if failed to read and parse router configuration file.
     **/
    constexpr  bool             DEFAULT_SERVICE_ENABLED    { true };

    /**
     * \brief   areg::DEFAULT_LOG_ENABLED
     *          Default flag to indicate logging enable / disable status.
     **/
    constexpr bool              DEFAULT_LOG_ENABLED         { false };

    /**
     * \brief   areg::LogTypeIdentifiers
     *          The list of logging type identifiers to convert to string or areg::LogTarget types
     **/
    extern AREG_API const std::vector<areg::Identifier> LogTypeIdentifiers;

    /**
     * \brief   areg::ConnectionIdentifiers
     *          The list of connection type identifiers to convert to string or areg::ConnectionType types
     **/
    extern AREG_API const std::vector<areg::Identifier> ConnectionIdentifiers;

    /**
     * \brief   areg::RemoteServiceIdentifiers
     *          The list of remote servicing type identifiers to convert to string or areg::RemoteServiceKind types
     **/
    extern AREG_API const std::vector<areg::Identifier> RemoteServiceIdentifiers;

    /**
     * \brief   areg::LogScopePriorityIndentifiers
     *          The list of logging priority type identifiers to convert to string or areg::LogPriority types
     **/
    extern AREG_API const std::vector<areg::Identifier> LogScopePriorityIndentifiers;

    /**
     * \brief   areg::AppState
     *          Describes the application states.
     *          -   Initially, the application is in undefined state.
     *          -   The application is in initialization state, when Service Manager did not started yet and it
     *              initializes any module like tracing, timer, etc.
     *          -   Application is in ready state only when Service Manager is started.
     *          -   Application is in release state when it is going to stop Service Manager.
     **/
    enum class AppState : uint8_t
    {
          Stopped       //!< Application state is undefined
        , Initializing  //!< Application is initializing
        , Ready         //!< Application is ready. The application is ready only when Service Manager runs.
        , Releasing     //!< Application is releasing.
        , Failure       //!< Application is failure state and cannot be continued
    };

} // namespace areg
#endif // AREG_APPBASE_APPDEFS_HPP
