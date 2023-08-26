/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
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

int main(int argc, char* argv[], char* envp[])
{
    int result      = 0;
    MulticastRouter & router = MulticastRouter::getInstance();
    const char * temp = static_cast<const char *>(*argv);
    std::pair<const OptionParser::sOptionSetup *, int> opt{ MulticastRouter::getOptionSetup( ) };
    if ( router.parseOptions( argc, &temp, opt.first, opt.second ) == false )
    {
        router.resetDefaultOptions( );
    }

    switch ( router.getCurrentOption() )
    {
    case NESystemService::eServiceOption::CMD_Install:
        result = router.serviceInstall() ? 0 : -2;
        break;

    case NESystemService::eServiceOption::CMD_Uninstall:
        router.serviceUninstall();
        break;

    case NESystemService::eServiceOption::CMD_Service:
    case NESystemService::eServiceOption::CMD_Console:
        router.setState(NESystemService::eSystemServiceState::ServiceStarting);
        router.serviceMain( static_cast<int>(argc), argv);
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

bool MulticastRouter::_osSetState( NESystemService::eSystemServiceState newState )
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
