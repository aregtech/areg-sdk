/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        mcrouter/private/app/MulticastRouterPosix.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Router, Multicast Router Service process
 ************************************************************************/
#include "mcrouter/app/MulticastRouter.hpp"

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

void _posixServiceMain( int argc, char ** argv );

int main(int argc, char* argv[], char* envp[])
{
    int result      = 0;
    MulticastRouter & router = MulticastRouter::getInstance();

    if (router.parseOptions(argc, argv) == false)
    {
        router.resetDefaultOptions();
    }

    switch ( router.getCurrentCommand() )
    {
    case NEMulticastRouterSettings::eServiceCommand::CMD_Install:
        result = router.serviceInstall() ? 0 : -2;
        break;

    case NEMulticastRouterSettings::eServiceCommand::CMD_Uninstall:
        router.serviceUninstall();
        break;

    case NEMulticastRouterSettings::eServiceCommand::CMD_Console:
        ::_posixServiceMain(argc, argv);
        router.serviceStop();
        break;

    case NEMulticastRouterSettings::eServiceCommand::CMD_Service:
        // result = ::StartServiceCtrlDispatcher(_serviceTable) ? 0 : -1;
        break;

    default:
        ASSERT(false);  // unexpected
        break;
    }

    return result;
}

void _posixServiceMain( int argc, char ** argv )
{
    MulticastRouter & router = MulticastRouter::getInstance();
    router.setState(NEMulticastRouterSettings::eRouterState::RouterStarting);
    router.serviceMain( static_cast<int>(argc), argv);
    router.setState( NEMulticastRouterSettings::eRouterState::RouterStopped );
}

//////////////////////////////////////////////////////////////////////////
// Global functions, End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MulticastRouter class implementation
//////////////////////////////////////////////////////////////////////////
bool MulticastRouter::_osIsValid( void ) const
{
    return true;
}

bool MulticastRouter::_osRegisterService( void )
{
    return false;
}


void MulticastRouter::_osFreeResources( void )
{
    mSvcHandle = nullptr;
    mSeMHandle = nullptr;
}

bool MulticastRouter::_osOpenService(void)
{
    return true;
}

bool MulticastRouter::_osCcreateService(void)
{
    return true;
}

void MulticastRouter::_osDeleteService( void )
{
}

bool MulticastRouter::_osSetState( NEMulticastRouterSettings::eRouterState newState )
{
    bool result{ true };

    if ( newState != mRouterState )
    {
        switch ( newState )
        {
        case NEMulticastRouterSettings::eRouterState::RouterStopped:
            break;

        case NEMulticastRouterSettings::eRouterState::RouterStarting:
            break;

        case NEMulticastRouterSettings::eRouterState::RouterStopping:
            break;

        case NEMulticastRouterSettings::eRouterState::RouterRunning:
            break;

        case NEMulticastRouterSettings::eRouterState::RouterContinuing:
            break;

        case NEMulticastRouterSettings::eRouterState::RouterPausing:
            break;

        case NEMulticastRouterSettings::eRouterState::RouterPaused:
            break;

        default:
            ASSERT(false);
        }

        mRouterState = newState;
    }

    return result;
}

#endif  // _POSIX
