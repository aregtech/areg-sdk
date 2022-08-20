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
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/NEMulticastRouterSettings.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing settings.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/GETrace.h"

#include <string_view>

namespace NEMulticastRouterSettings
{
    /**
     * \brief   NEMulticastRouterSettings::eRouterState
     *          Describes the multi-cast router state.
     **/
    typedef enum class E_RouterState
    {
          RouterStopped     //!< Router is stopped.
        , RouterStarting    //!< Router is in starting process.
        , RouterStopping    //!< Router is in stopping process.
        , RouterRunning     //!< Router is up and running.
        , RouterContinuing  //!< Router was paused and in continuing state.
        , RouterPausing     //!< Router is in pausing state.
        , RouterPaused      //!< Router is paused.
    } eRouterState;

    /**
     * \brief   Returns the human readable string of NEMulticastRouterSettings::eRouterState value
     **/
    inline const char * const GetString( const NEMulticastRouterSettings::eRouterState & RouterState );

    /**
     * \brief   NEMulticastRouterSettings::eServiceCommand
     *          Message routing service commands.
     **/
    typedef enum class E_ServiceCommand
    {
          CMD_Undefined     //!< Command is undefined
        , CMD_Install       //!< Command is to install (register) service in the system
        , CMD_Uninstall     //!< Command is to uninstall (unregister) service in the system
        , CMD_Service       //!< Command is to execute process as a system service (in background)
        , CMD_Console       //!< Command is to execute process as console application.
        , CMD_Verbose       //!< Command is to display the data rate when execute process as console application.
    } eServiceCommand;

    /**
     * \brief   Converts and returns string value of NEMulticastRouterSettings::eServiceCommand type.
     **/
    inline const char * getString( NEMulticastRouterSettings::eServiceCommand cmdService );

    /**
     * \brief   The ASCII name of router as a system service.
     **/
    extern char *       SERVICE_NAME_ASCII          /*= NEApplication::ROUTER_SERVICE_NAME_ASCII*/;
    /**
     * \brief   The UNICODE name of router as a system service.
     **/
    extern wchar_t *    SERVICE_NAME_WIDE           /*= NEApplication::ROUTER_SERVICE_NAME_WIDE*/;
    /**
     * \brief   The default option to run multicast router as a console application.
     **/
    constexpr eServiceCommand   DEFAULT_OPTION      { eServiceCommand::CMD_Console };
    /**
     * \brief   The default option to run multicast router in verbose mode to output send and receive data rate.
     */
    constexpr bool              DEFAULT_VERBOSE     { true };
    /**
     * \brief   Structure that specifies multicast router process option and related eServiceCommand value
     **/
    typedef struct S_ServiceCommand
    {
        const std::string_view  optLong;    //!< The long name of the option passed to process.
        const std::string_view  optShort;   //!< The short name of the option passed to process.
        const eServiceCommand   option;     //!< The related eServiceCommand value.
    } sServiceCommand;

    /**
     * \brief   List of multicast router string options passed in command line and related eServiceCommand.
     **/
    constexpr sServiceCommand   ServiceCommands[]   
    {
          {"--install"  , "-i", eServiceCommand::CMD_Install}   //!< Command to install service. Valid for Windows OS.
        , {"--uninstall", "-u", eServiceCommand::CMD_Uninstall} //!< Command to uninstall service. Valid for Windows OS.
        , {"--service"  , "-s", eServiceCommand::CMD_Service}   //!< Command to run process as a system service process.
        , {"--console"  , "-c", eServiceCommand::CMD_Console}   //!< Command to run process as a console application.
        , {"--verbose"  , "-v", eServiceCommand::CMD_Verbose}   //!< Command to display data rate when run as console application.
        , {""           , ""  , eServiceCommand::CMD_Undefined} //!< Any other command.
    };

    /**
     * \brief   Parses string value and NEMulticastRouterSettings::eServiceCommand
     *          value of the multicast router service option.
     **/
    inline NEMulticastRouterSettings::eServiceCommand parseOption( const char * const option );

    /**
     * \brief   The name of main service router thread.
     **/
    constexpr char      RouterThreadName[]          { "_AREG_SERVICEROUTER_THREAD_" };

} // namespace NEMulticastRouterSettings

//////////////////////////////////////////////////////////////////////////
// NEMulticastRouterSettings namespace inline methods.
//////////////////////////////////////////////////////////////////////////

inline const char * const NEMulticastRouterSettings::GetString( const NEMulticastRouterSettings::eRouterState & RouterState )
{
    switch ( RouterState )
    {
    case NEMulticastRouterSettings::eRouterState::RouterStopped:
        return "NEMulticastRouterSettings::RouterStopped";
    case NEMulticastRouterSettings::eRouterState::RouterStarting:
        return "NEMulticastRouterSettings::RouterStarting";
    case NEMulticastRouterSettings::eRouterState::RouterStopping:
        return "NEMulticastRouterSettings::RouterStopping";
    case NEMulticastRouterSettings::eRouterState::RouterRunning:
        return "NEMulticastRouterSettings::RouterRunning";
    case NEMulticastRouterSettings::eRouterState::RouterContinuing:
        return "NEMulticastRouterSettings::RouterContinuing";
    case NEMulticastRouterSettings::eRouterState::RouterPausing:
        return "NEMulticastRouterSettings::RouterPausing";
    case NEMulticastRouterSettings::eRouterState::RouterPaused:
        return "NEMulticastRouterSettings::RouterPaused";
    default:
        ASSERT(false);
        return "ERR: Undefined NEMulticastRouterSettings::eRouterState value!!!";
    }
}

inline const char * NEMulticastRouterSettings::getString( NEMulticastRouterSettings::eServiceCommand cmdService )
{
    switch ( cmdService )
    {
    case NEMulticastRouterSettings::eServiceCommand::CMD_Undefined:
        return "NEMulticastRouterSettings::CMD_Undefined";
    case NEMulticastRouterSettings::eServiceCommand::CMD_Install:
        return "NEMulticastRouterSettings::CMD_Install";
    case NEMulticastRouterSettings::eServiceCommand::CMD_Uninstall:
        return "NEMulticastRouterSettings::CMD_Uninstall";
    case NEMulticastRouterSettings::eServiceCommand::CMD_Service:
        return "NEMulticastRouterSettings::CMD_Service";
    case NEMulticastRouterSettings::eServiceCommand::CMD_Console:
        return "NEMulticastRouterSettings::CMD_Console";
    case NEMulticastRouterSettings::eServiceCommand::CMD_Verbose:
        return "NEMulticastRouterSettings::CMD_Verbose";
    default:
        ASSERT( false );
        return "ERR: Unexpected NEMulticastRouterSettings::eServiceCommand value!";
    }
}

inline NEMulticastRouterSettings::eServiceCommand NEMulticastRouterSettings::parseOption( const char * const option )
{
    NEMulticastRouterSettings::eServiceCommand result = NEMulticastRouterSettings::DEFAULT_OPTION;

    if ((option != nullptr) && (*option != NEString::EndOfString))
    {
        for ( const sServiceCommand & cmd : NEMulticastRouterSettings::ServiceCommands )
        {
            if ( cmd.option == eServiceCommand::CMD_Undefined )
            {
                break;
            }
            else if ( (cmd.optLong == option) || (cmd.optShort == option) )
            {
                result = cmd.option;
                break;
            }
        }
    }

    return result;
}

#endif  // AREG_MCROUTER_APP_NEMULTICASTROUTERSETTINGS_HPP
