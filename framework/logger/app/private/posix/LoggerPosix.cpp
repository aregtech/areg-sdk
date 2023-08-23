/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logger/private/app/LoggerPosix.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Logger, POSIX specific implementation
 ************************************************************************/
#include "logger/app/Logger.hpp"

#ifdef _POSIX

#include "areg/base/NEUtilities.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/base/NEString.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"


//////////////////////////////////////////////////////////////////////////
// Global functions, Begin
//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[], char* envp[])
{
    int result{0};
    Logger & logger = Logger::getInstance();
    const char * temp = static_cast<const char *>(*argv);
    std::pair<const OptionParser::sOptionSetup *, int> opt{ Logger::getOptionSetup( ) };
    if ( logger.parseOptions( argc, &temp, opt.first, opt.second ) == false )
    {
        logger.resetDefaultOptions( );
    }

    switch ( logger.getCurrentOption() )
    {
    case NESystemService::eServiceOption::CMD_Install:
        result = logger.serviceInstall() ? 0 : -2;
        break;

    case NESystemService::eServiceOption::CMD_Uninstall:
        logger.serviceUninstall();
        break;

    case NESystemService::eServiceOption::CMD_Service:
    case NESystemService::eServiceOption::CMD_Console:
        logger.setState(NESystemService::eSystemServiceState::ServiceStarting);
        logger.serviceMain( static_cast<int>(argc), argv);
        logger.setState( NESystemService::eSystemServiceState::ServiceStopped );
        logger.serviceStop();
        break;


    default:
        ASSERT(false);  // unexpected
        break;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Global functions, End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Logger class implementation
//////////////////////////////////////////////////////////////////////////
bool Logger::_osIsValid( void ) const
{
    return true;
}

bool Logger::_osRegisterService( void )
{
    return false;
}


void Logger::_osFreeResources( void )
{
    mSvcHandle = nullptr;
    mSeMHandle = nullptr;
}

bool Logger::_osOpenService(void)
{
    return true;
}

bool Logger::_osCcreateService(void)
{
    return true;
}

void Logger::_osDeleteService( void )
{
}

bool Logger::_osSetState( NESystemService::eSystemServiceState newState )
{
    bool result{ true };

    if ( newState != mSystemServiceState )
    {
        switch ( newState )
        {
        case NESystemService::eSystemServiceState::ServiceStopped:
            break;

        case NESystemService::eSystemServiceState::ServiceStarting:
            break;

        case NESystemService::eSystemServiceState::ServiceStopping:
            break;

        case NESystemService::eSystemServiceState::ServiceRunning:
            break;

        case NESystemService::eSystemServiceState::ServiceContinuing:
            break;

        case NESystemService::eSystemServiceState::ServicePausing:
            break;

        case NESystemService::eSystemServiceState::ServicePaused:
            break;

        default:
            ASSERT(false);
        }

        mSystemServiceState = newState;
    }

    return result;
}

#endif  // _POSIX
