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
    , mSystemServiceState   ( aregext::ServicePhase::Stopped )
    , mSystemServiceOption  ( aregext::DEFAULT_OPTION )
    , mSvcHandle            ( nullptr )
    , mSeMHandle            ( nullptr )
    , mFileConfig           ( areg::DEFAULT_CONFIG_FILE )
{
}

void SystemServiceBase::resetDefaultOptions()
{
    mSystemServiceOption = aregext::DEFAULT_OPTION;
    mFileConfig = areg::DEFAULT_CONFIG_FILE;
    mCommunication.enableCalculateDataRate(aregext::DEFAULT_VERBOSE);
}

bool SystemServiceBase::parseOptions( int32_t argc, const char ** argv, const OptionParser::OptionSetup * optSetup, uint32_t optCount )
{
    bool result{ false };

    if (argc > 1)
    {
        OptionParser parser(optSetup, optCount);
        if (parser.parseCommandLine(argv, argc > 1 ? static_cast<uint32_t>(argc) : 0))
        {
            result = prepareOptions(parser.getOptions());
        }
        else
        {
            printHelp(true);
            result = false;
        }
    }
    else if ( argc == 1 )
    {
        resetDefaultOptions( );
        result = true;
    }

    return result;
}

bool SystemServiceBase::parseOptions(int32_t argc, char** argv, const OptionParser::OptionSetup* optSetup, uint32_t optCount)
{
    bool result{ false };

    if (argc > 1)
    {
        OptionParser parser(optSetup, optCount);
        if (parser.parseCommandLine(argv, static_cast<uint32_t>(argc)))
        {
            parser.sort();
            result = prepareOptions(parser.getOptions());
        }
        else
        {
            printHelp(true);
            result = false;
        }
    }
    else if (argc == 1)
    {
        resetDefaultOptions();
        result = true;
    }

    return result;
}

bool SystemServiceBase::prepareOptions(const OptionParser::InputOptionList& opts)
{
    bool result{ true };

    for (uint32_t i = 0; i < opts.getSize(); ++i)
    {
        const OptionParser::InputOption& opt = opts[i];
        result &= dispatchOption(opt);
    }

    return result;
}

bool SystemServiceBase::dispatchOption(const OptionParser::InputOption& opt)
{
    bool result{ false };
    bool outHelp{ false };

    switch (static_cast<aregext::ServiceOption>(opt.inCommand))
    {
    case aregext::ServiceOption::CMD_Install:  // fall through
    case aregext::ServiceOption::CMD_Uninstall:// fall through
    case aregext::ServiceOption::CMD_Service:  // fall through
    case aregext::ServiceOption::CMD_Console:
        result = true;
        setCurrentOption(static_cast<aregext::ServiceOption>(opt.inCommand));
        break;

    case aregext::ServiceOption::CMD_Load:
    {
        String filePath(opt.inString[0]);
        result = File::existFile(filePath);
        if (result)
        {
            mFileConfig = filePath;
        }
    }
    break;

    case aregext::ServiceOption::CMD_Verbose:
        mCommunication.enableCalculateDataRate(true);
        setCurrentOption(aregext::ServiceOption::CMD_Console);
        result = true;
        break;

    case aregext::ServiceOption::CMD_Help:
        outHelp = true;
        break;

    case aregext::ServiceOption::CMD_Undefined:// fall through
    case aregext::ServiceOption::CMD_Custom:   // fall through
    default:
        setCurrentOption(aregext::ServiceOption::CMD_Undefined);
        outHelp = true;
        break;
    }

    if (outHelp)
    {
        printHelp(true);
        result = false;
    }

    return result;
}

int32_t SystemServiceBase::serviceMain(aregext::ServiceOption optStartup, const char* argument)
{
    int32_t result{ RESULT_SUCCEEDED };
    if (serviceInitialize(optStartup, argument, nullptr))
    {
        LOG_SCOPE(areg_aregextend_service_SystemServiceBase_serviceMain);
        LOG_DBG( "Starting log collector service, the current option [ %s ]", aregext::getString(optStartup) );
        setState(aregext::ServicePhase::Starting);

        if (registerService())
        {
            LOG_DBG("Registered service, starting service");
            serviceStart();
        }
        else if (mSystemServiceOption == aregext::ServiceOption::CMD_Console)
        {
            LOG_DBG("Starting in console mode, starting service");
            serviceStart();
        }

        if (mSystemServiceOption == aregext::ServiceOption::CMD_Service)
        {
            LOG_DBG("Starts to run service...");
            runService();
        }
        else if (mSystemServiceOption == aregext::ServiceOption::CMD_Console)
        {
            LOG_DBG("Entering console mode...");
#if AREG_EXTENDED
            runConsoleInputExtended();
#else   // !AREG_EXTENDED
            runConsoleInputSimple();
#endif  // !AREG_EXTENDED
        }
        else
        {
            LOG_DBG("Unexpected option [ %s ]", aregext::getString(mSystemServiceOption));
        }

        serviceStop();
        LOG_WARN("Service Stopped and not running anymore");
    }
    else
    {
        result = RESULT_FAILED_INIT;
    }

    serviceRelease();
    setState(aregext::ServicePhase::Stopped);

    return result;
}

void SystemServiceBase::sendMessageToTarget(const RemoteMessage& message)
{
    mCommunication.sendMessage(message);
}

void SystemServiceBase::controlService(SystemServiceBase::ServiceControl control)
{
    switch (control)
    {
    case SystemServiceBase::ServiceControl::ServiceStop:
    case SystemServiceBase::ServiceControl::ServiceShutdown:
        serviceStop();
        break;

    case SystemServiceBase::ServiceControl::ServicePause:
        servicePause();
        break;

    case SystemServiceBase::ServiceControl::ServiceContinue:
        serviceContinue();
        break;

    default:
        ASSERT(false);
        break;
    }
}
