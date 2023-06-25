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
    int result      = 0;
    Logger & logger = Logger::getInstance();

    if (logger.parseOptions(argc, argv) == false)
    {
        logger.resetDefaultOptions();
    }

    switch ( logger.getCurrentCommand() )
    {
    case NELoggerSettings::eServiceCommand::CMD_Install:
        result = logger.serviceInstall() ? 0 : -2;
        break;

    case NELoggerSettings::eServiceCommand::CMD_Uninstall:
        logger.serviceUninstall();
        break;

    case NELoggerSettings::eServiceCommand::CMD_Service:
    case NELoggerSettings::eServiceCommand::CMD_Console:
        logger.setState(NELoggerSettings::eLoggerState::LoggerStarting);
        logger.serviceMain( static_cast<int>(argc), argv);
        logger.setState( NELoggerSettings::eLoggerState::LoggerStopped );
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

bool Logger::_osSetState( NELoggerSettings::eLoggerState newState )
{
    bool result{ true };

    if ( newState != mLoggerState )
    {
        switch ( newState )
        {
        case NELoggerSettings::eLoggerState::LoggerStopped:
            break;

        case NELoggerSettings::eLoggerState::LoggerStarting:
            break;

        case NELoggerSettings::eLoggerState::LoggerStopping:
            break;

        case NELoggerSettings::eLoggerState::LoggerRunning:
            break;

        case NELoggerSettings::eLoggerState::LoggerContinuing:
            break;

        case NELoggerSettings::eLoggerState::LoggerPausing:
            break;

        case NELoggerSettings::eLoggerState::LoggerPaused:
            break;

        default:
            ASSERT(false);
        }

        mLoggerState = newState;
    }

    return result;
}

#endif  // _POSIX
