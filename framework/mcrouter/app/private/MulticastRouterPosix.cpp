
/************************************************************************
 * \file        mcrouter/private/app/MulticastRouterWin32.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
#include "areg/trace/GETrace.h"
#include <signal.h>

DEF_TRACE_SCOPE(mcrouter_app_MulticastRouterPosix_handleSignalBrokenPipe);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouterPosix_setState);

//////////////////////////////////////////////////////////////////////////
// Global functions, Begin
//////////////////////////////////////////////////////////////////////////

void GServiceMain( int argc, char ** argv );

inline static bool _isEqual( const char * strLeft, const char * strRight )
{
    return (NEString::compareStrings<char, char>(strLeft, strRight, NEString::CountAll, false) == 0);
}

static void handleSignalBrokenPipe(int s)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouterPosix_handleSignalBrokenPipe);
    TRACE_WARN("Caught SIGPIPE signal.");
}

int main(int argc, char* argv[], char* envp[])
{
    int result      = 0;
    MulticastRouter & router = MulticastRouter::getInstance();
    router.setCurrentCommand( MulticastRouter::CMD_Console );

    if ( argc > 1 )
    {
        const char * cmd = argv[1];

        if ( _isEqual( cmd, NEMulticastRouterSettings::ServiceCommandInstall) )
            router.setCurrentCommand( MulticastRouter::CMD_Install );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandUninstall) )
            router.setCurrentCommand( MulticastRouter::CMD_Uninstall );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandConsole) )
            router.setCurrentCommand( MulticastRouter::CMD_Console );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandService) )
            router.setCurrentCommand(MulticastRouter::CMD_Service);
    }

    signal(SIGPIPE, &handleSignalBrokenPipe);

    switch ( MulticastRouter::getInstance().getCurrentCommand() )
    {
    case MulticastRouter::CMD_Install:
        result = MulticastRouter::getInstance().serviceInstall() ? 0 : -2;
        break;

    case MulticastRouter::CMD_Uninstall:
        MulticastRouter::getInstance().serviceUninstall();
        break;

    case MulticastRouter::CMD_Console:
        ::GServiceMain(argc, argv);
        MulticastRouter::getInstance().serviceStop();
        break;

    case MulticastRouter::CMD_Service:
        // result = ::StartServiceCtrlDispatcher(gServiceTable) ? 0 : -1;
        break;

    default:
        ASSERT(false);  // unexpected
        break;
    }

    return result;
}

void GServiceMain( int argc, char ** argv )
{
    MulticastRouter & router = MulticastRouter::getInstance();
    router.setState(NEMulticastRouterSettings::RouterStarting);
    router.serviceMain( static_cast<int>(argc), argv);
    router.setState( NEMulticastRouterSettings::RouterStopped );
}

//////////////////////////////////////////////////////////////////////////
// Global functions, End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MulticastRouter class implementation
//////////////////////////////////////////////////////////////////////////
bool MulticastRouter::_isValid( void ) const
{
    return true;
}

bool MulticastRouter::_registerService( void )
{
    return false;
}


void MulticastRouter::_freeResources( void )
{
    mSvcHandle = NULL;
    mSeMHandle = NULL;
}

bool MulticastRouter::_openService(void)
{
    return true;
}

bool MulticastRouter::_createService(void)
{
    return true;
}

void MulticastRouter::_deleteService( void )
{
}

bool MulticastRouter::setState( NEMulticastRouterSettings::eRouterState newState )
{
    TRACE_SCOPE(mcrouter_app_MulticastRouterPosix_setState);
    TRACE_DBG("Changing Service Router state. Old state [ %s ], new state [ %s ]"
                , NEMulticastRouterSettings::GetString(mRouterState)
                , NEMulticastRouterSettings::GetString(newState));

    bool result = true;

    if ( newState != mRouterState )
    {
        switch ( newState )
        {
        case NEMulticastRouterSettings::RouterStopped:
            break;

        case NEMulticastRouterSettings::RouterStarting:
            break;

        case NEMulticastRouterSettings::RouterStopping:
            break;

        case NEMulticastRouterSettings::RouterRunning:
            break;

        case NEMulticastRouterSettings::RouterContinuing:
            break;

        case NEMulticastRouterSettings::RouterPausing:
            break;

        case NEMulticastRouterSettings::RouterPaused:
            break;

        default:
            ASSERT(false);
        }

        mRouterState = newState;
    }

    return result;
}

#endif  // _POSIX
