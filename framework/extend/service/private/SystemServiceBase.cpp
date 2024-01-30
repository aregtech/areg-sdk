/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/service/private/SystemServiceBase.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, base class to create system services.
 ************************************************************************/
#include "extend/service/SystemServiceBase.hpp"

#include "areg/trace/GETrace.h"
#include "extend/console/Console.hpp"


//////////////////////////////////////////////////////////////////////////
// Traces.
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE( utilities_service_base_SystemServiceBase_serviceMain );

//////////////////////////////////////////////////////////////////////////
// MulticastRouter class implementation
//////////////////////////////////////////////////////////////////////////
SystemServiceBase::SystemServiceBase( ServiceCommunicatonBase & commBase )
    : mCommunication        ( commBase )
    , mSystemServiceState   ( NESystemService::eSystemServiceState::ServiceStopped )
    , mSystemServiceOption  ( NESystemService::DEFAULT_OPTION )
    , mSvcHandle            ( nullptr )
    , mSeMHandle            ( nullptr )
{
}

void SystemServiceBase::resetDefaultOptions(void)
{
    mSystemServiceOption = NESystemService::DEFAULT_OPTION;
    mCommunication.enableCalculateDataRate(NESystemService::DEFAULT_VERBOSE);
}

bool SystemServiceBase::parseOptions( int argc, const char ** argv, const OptionParser::sOptionSetup * optSetup, int optCount )
{
    bool result{ false };

    if (argc > 1)
    {
        OptionParser parser(optSetup, optCount);
        if (parser.parseCommandLine(static_cast<const char**>(argv), argc))
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

bool SystemServiceBase::parseOptions(int argc, char** argv, const OptionParser::sOptionSetup* optSetup, int optCount)
{
    bool result{ false };

    if (argc > 1)
    {
        OptionParser parser(optSetup, optCount);
        if (parser.parseCommandLine(argv, argc))
        {
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
    bool result{ false };
    bool outHelp{ false };

    for (uint32_t i = 0; i < opts.getSize(); ++i)
    {
        const OptionParser::sOption& opt = opts[i];
        switch (static_cast<NESystemService::eServiceOption>(opt.inCommand))
        {
        case NESystemService::eServiceOption::CMD_Install:
        case NESystemService::eServiceOption::CMD_Uninstall:
        case NESystemService::eServiceOption::CMD_Service:
        case NESystemService::eServiceOption::CMD_Console:
            result = true;
            setCurrentOption(static_cast<NESystemService::eServiceOption>(opt.inCommand));
            break;

        case NESystemService::eServiceOption::CMD_Verbose:
            mCommunication.enableCalculateDataRate(true);
            setCurrentOption(NESystemService::eServiceOption::CMD_Console);
            result = true;
            break;

        case NESystemService::eServiceOption::CMD_Help:
            outHelp = true;
            break;

        default:
            setCurrentOption(NESystemService::eServiceOption::CMD_Undefined);
            outHelp = true;
            break;
        }
    }

    if (outHelp)
    {
        printHelp(true);
        result = false;
    }

    return result;
}

void SystemServiceBase::serviceMain( int argc, char ** argv )
{
    TRACE_SCOPE( utilities_service_base_SystemServiceBase_serviceMain );
    TRACE_DBG( "Starting logger service. There are [ %d ] arguments in the list...", argc );

#ifdef  DEBUG
    for ( int i = 0; i < argc; ++ i )
        TRACE_DBG( "... Command argument [ %d ]: [ %s ]", i, argv[ i ] );
#endif  // DEBUG

    if ( registerService( ) || (mSystemServiceOption == NESystemService::eServiceOption::CMD_Console) )
    {
        TRACE_DBG( "Starting service" );
        serviceStart( );
    }

    if (mSystemServiceOption == NESystemService::eServiceOption::CMD_Service)
    {
        runService();
    }
    else if ( mSystemServiceOption == NESystemService::eServiceOption::CMD_Console )
    {
#if AREG_EXTENDED
        runConsoleInputExtended( );
#else   // !AREG_EXTENDED
        runConsoleInputSimple( );
#endif  // !AREG_EXTENDED
    }

    serviceStop( );
    TRACE_WARN( "Service Stopped and not running anymore" );
}

void SystemServiceBase::sendMessageToTarget(const RemoteMessage& message)
{
    mCommunication.sendMessage(message);
}
