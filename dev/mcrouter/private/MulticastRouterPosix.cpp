/************************************************************************
 * \file        mcrouter/private/MulticastRouterWin32.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Router, Multicast Router Service process
 ************************************************************************/
#include "mcrouter/MulticastRouter.hpp"

#ifdef _POSIX

#include "areg/base/NEUtilities.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"

DEF_TRACE_SCOPE(areg_mcrouter_WinMain);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_ServiceEntry);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_ServiceInstall);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_ServiceUninstall);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_SetState);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_ServiceStart);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_ServiceStop);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_ServicePause);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_ServiceContinue);

//////////////////////////////////////////////////////////////////////////
// Global functions, Begin
//////////////////////////////////////////////////////////////////////////
void GServiceMain( int argc, char ** argv );

inline static bool _isEqual( const char * strLeft, const char * strRight )
{
    return (NEString::compareStrings<char, char>(strLeft, strRight, NEString::CountAll, false) == 0);
}

int main(int argc, char* argv[], char* envp[])
{
    int result      = 0;
    MulticastRouter & broker = MulticastRouter::getInstance();
    broker.setCurrentCommand( MulticastRouter::CMD_Console );

    if ( argc > 1 )
    {
        const char * cmd = argv[1];

        if ( _isEqual( cmd, NEMulticastRouterSettings::ServiceCommandInstall) )
            broker.setCurrentCommand( MulticastRouter::CMD_Install );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandUninstall) )
            broker.setCurrentCommand( MulticastRouter::CMD_Uninstall );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandConsole) )
            broker.setCurrentCommand( MulticastRouter::CMD_Console );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandService) )
            broker.setCurrentCommand(MulticastRouter::CMD_Service);
    }

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

bool MulticastRouter::serviceInstall(void)
{
    if ( _openService() == false )
        _createService();

    return _isValid();
}

void MulticastRouter::serviceUninstall(void)
{
    _freeResources();
}

void MulticastRouter::serviceMain( int argc, char ** argv )
{
    Application::setWorkingDirectory();
    Application::startTracer(NEApplication::DEFAULT_TRACING_CONFIG_FILE, true);
    Application::startTimerManager();

    do 
    {
        TRACE_SCOPE(areg_mcrouter_MulticastRouter_ServiceEntry);
        TRACE_DBG("Starting service. There are [ %d ] arguments in the list...", argc);

#ifdef  DEBUG
        for ( int i = 0; i < argc; ++ i )
            TRACE_DBG("... Command argument [ %d ]: [ %s ]", i, argv[i]);
#endif  // DEBUG

        mQuitEvent.setEvent();

        if ( mServiceCmd == MulticastRouter::CMD_Console )
        {
            TRACE_DBG("Starting service");
            serviceStart();
        }

        char cmd = '\0';
        while ( mServiceCmd == MulticastRouter::CMD_Console && cmd != 'q')
        {
            scanf("%c", &cmd);
            cmd = cmd == 'Q' ? 'q' : cmd;
        }

        mQuitEvent.lock( mServiceCmd == MulticastRouter::CMD_Service ? IESynchObject::WAIT_INFINITE : IESynchObject::DO_NOT_WAIT );
        TRACE_WARN("Service Stopped and not running anymore");

    } while (false);

    Application::releaseApplication();
}

bool MulticastRouter::serviceStart(void)
{
    TRACE_SCOPE(areg_mcrouter_MulticastRouter_ServiceStart);
    TRACE_DBG("Starting service [ %s ]", NEMulticastRouterSettings::ServiceNameA);
    bool result = false;
    if (  mServiceServer.configureRemoteServicing( NEApplication::DEFAULT_ROUTER_CONFIG_FILE ) && mServiceServer.startRemoteServicing() )
    {
        result = setState(NEMulticastRouterSettings::RouterRunning);
        mQuitEvent.resetEvent();
    }

    return result;
}

void MulticastRouter::servicePause(void)
{
    TRACE_SCOPE(areg_mcrouter_MulticastRouter_ServicePause);
    TRACE_DBG("Pausing Router service");

    setState( NEMulticastRouterSettings::RouterPausing );
    mServiceServer.stopRemoteServicing();
    setState( NEMulticastRouterSettings::RouterPaused );
}

bool MulticastRouter::serviceContinue(void)
{
    TRACE_SCOPE(areg_mcrouter_MulticastRouter_ServiceContinue);
    TRACE_DBG("Continueing Router service");

    bool result = false;
    setState( NEMulticastRouterSettings::RouterContinuing );
    if ( mServiceServer.isRemoteServicingConfigured() && mServiceServer.startRemoteServicing() )
    {
        result = true;
        setState( NEMulticastRouterSettings::RouterRunning );
    }
    else
    {
        TRACE_ERR("Failed to restart remote servicing");
        mQuitEvent.setEvent();
    }
    return result;
}

void MulticastRouter::serviceStop(void)
{
    TRACE_SCOPE(areg_mcrouter_MulticastRouter_ServiceStop);
    TRACE_WARN("Stopping service [ %s ]", NEMulticastRouterSettings::ServiceNameA);
    setState(NEMulticastRouterSettings::RouterStopping);
    mServiceServer.stopRemoteServicing();
    mQuitEvent.setEvent();
}

bool MulticastRouter::setState( NEMulticastRouterSettings::eRouterState newState )
{
    TRACE_SCOPE(areg_mcrouter_MulticastRouter_SetState);
    TRACE_DBG("Changing Service Router state. Old state [ %s ], new state [ %s ]", NEMulticastRouterSettings::GetString(mRouterState), NEMulticastRouterSettings::GetString(newState));

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
