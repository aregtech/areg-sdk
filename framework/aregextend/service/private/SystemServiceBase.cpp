/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/SystemServiceBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, base class to create system services.
 ************************************************************************/
#include "aregextend/service/SystemServiceBase.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/File.hpp"
#include "areg/logging/GELog.h"
#include "aregextend/console/Console.hpp"


//////////////////////////////////////////////////////////////////////////
// Log Scopes
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE( areg_aregextend_service_SystemServiceBase_serviceMain );

//////////////////////////////////////////////////////////////////////////
// MultitargetRouter class implementation
//////////////////////////////////////////////////////////////////////////
SystemServiceBase::SystemServiceBase( ServiceCommunicationBase & commBase )
    : mCommunication        ( commBase )
    , mSystemServiceState   ( NESystemService::ServicePhase::Stopped )
    , mSystemServiceOption  ( NESystemService::DEFAULT_OPTION )
    , mSvcHandle            ( nullptr )
    , mSeMHandle            ( nullptr )
    , mFileConfig           ( NEApplication::DEFAULT_CONFIG_FILE )
{
}

void SystemServiceBase::reset()
{
    mSystemServiceOption = NESystemService::DEFAULT_OPTION;
    mFileConfig = NEApplication::DEFAULT_CONFIG_FILE;
    mCommunication.enable_data_rate(NESystemService::DEFAULT_VERBOSE);
}

bool SystemServiceBase::parse_options( int32_t argc, const char ** argv, const OptionParser::OptionSetup * optSetup, uint32_t optCount )
{
    bool result{ false };

    if (argc > 1)
    {
        OptionParser parser(optSetup, optCount);
        if (parser.parse_command_line(argv, argc > 1 ? static_cast<uint32_t>(argc) : 0))
        {
            result = prepare_options(parser.options());
        }
        else
        {
            print_help(true);
            result = false;
        }
    }
    else if ( argc == 1 )
    {
        reset( );
        result = true;
    }

    return result;
}

bool SystemServiceBase::parse_options(int32_t argc, char** argv, const OptionParser::OptionSetup* optSetup, uint32_t optCount)
{
    bool result{ false };

    if (argc > 1)
    {
        OptionParser parser(optSetup, optCount);
        if (parser.parse_command_line(argv, static_cast<uint32_t>(argc)))
        {
            parser.sort();
            result = prepare_options(parser.options());
        }
        else
        {
            print_help(true);
            result = false;
        }
    }
    else if (argc == 1)
    {
        reset();
        result = true;
    }

    return result;
}

bool SystemServiceBase::prepare_options(const OptionParser::InputOptionList& opts)
{
    bool result{ true };

    for (uint32_t i = 0; i < opts.size(); ++i)
    {
        const OptionParser::InputOption& opt = opts[i];
        result &= dispatch_option(opt);
    }

    return result;
}

bool SystemServiceBase::dispatch_option(const OptionParser::InputOption& opt)
{
    bool result{ false };
    bool outHelp{ false };

    switch (static_cast<NESystemService::ServiceOption>(opt.inCommand))
    {
    case NESystemService::ServiceOption::CMD_Install:  // fall through
    case NESystemService::ServiceOption::CMD_Uninstall:// fall through
    case NESystemService::ServiceOption::CMD_Service:  // fall through
    case NESystemService::ServiceOption::CMD_Console:
        result = true;
        set_current_option(static_cast<NESystemService::ServiceOption>(opt.inCommand));
        break;

    case NESystemService::ServiceOption::CMD_Load:
    {
        String filePath(opt.inString[0]);
        result = File::has_file(filePath);
        if (result)
        {
            mFileConfig = filePath;
        }
    }
    break;

    case NESystemService::ServiceOption::CMD_Verbose:
        mCommunication.enable_data_rate(true);
        set_current_option(NESystemService::ServiceOption::CMD_Console);
        result = true;
        break;

    case NESystemService::ServiceOption::CMD_Help:
        outHelp = true;
        break;

    case NESystemService::ServiceOption::CMD_Undefined:// fall through
    case NESystemService::ServiceOption::CMD_Custom:   // fall through
    default:
        set_current_option(NESystemService::ServiceOption::CMD_Undefined);
        outHelp = true;
        break;
    }

    if (outHelp)
    {
        print_help(true);
        result = false;
    }

    return result;
}

int32_t SystemServiceBase::service_main(NESystemService::ServiceOption optStartup, const char* argument)
{
    int32_t result{ RESULT_SUCCEEDED };
    if (service_initialize(optStartup, argument, nullptr))
    {
        LOG_SCOPE(areg_aregextend_service_SystemServiceBase_serviceMain);
        LOG_DBG( "Starting log collector service, the current option [ %s ]", NESystemService::as_string(optStartup) );
        set_state(NESystemService::ServicePhase::Starting);

        if (register_service())
        {
            LOG_DBG("Registered service, starting service");
            service_start();
        }
        else if (mSystemServiceOption == NESystemService::ServiceOption::CMD_Console)
        {
            LOG_DBG("Starting in console mode, starting service");
            service_start();
        }

        if (mSystemServiceOption == NESystemService::ServiceOption::CMD_Service)
        {
            LOG_DBG("Starts to run service...");
            run_service();
        }
        else if (mSystemServiceOption == NESystemService::ServiceOption::CMD_Console)
        {
            LOG_DBG("Entering console mode...");
#if AREG_EXTENDED
            run_console_input_extended();
#else   // !AREG_EXTENDED
            run_console_input_simple();
#endif  // !AREG_EXTENDED
        }
        else
        {
            LOG_DBG("Unexpected option [ %s ]", NESystemService::as_string(mSystemServiceOption));
        }

        service_stop();
        LOG_WARN("Service Stopped and not running anymore");
    }
    else
    {
        result = RESULT_FAILED_INIT;
    }

    service_release();
    set_state(NESystemService::ServicePhase::Stopped);

    return result;
}

void SystemServiceBase::send_message_to_target(const RemoteMessage& message)
{
    mCommunication.send_message(message);
}

void SystemServiceBase::control_service(SystemServiceBase::ServiceControl control)
{
    switch (control)
    {
    case SystemServiceBase::ServiceControl::ServiceStop:
    case SystemServiceBase::ServiceControl::ServiceShutdown:
        service_stop();
        break;

    case SystemServiceBase::ServiceControl::ServicePause:
        service_pause();
        break;

    case SystemServiceBase::ServiceControl::ServiceContinue:
        service_continue();
        break;

    default:
        ASSERT(false);
        break;
    }
}
