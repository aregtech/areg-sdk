/************************************************************************
 * \file        mcrouter/private/CEMulticastRouterWin32.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Router, Multicast Router Service process
 ************************************************************************/
#include "mcrouter/CEMulticastRouter.hpp"

#ifdef _POSIX

#include "areg/base/NEUtilities.hpp"
#include "areg/base/CEFile.hpp"
#include "areg/base/CEProcess.hpp"
#include "areg/base/CEString.hpp"
#include "areg/appbase/CEApplication.hpp"
#include "areg/appbase/NEApplication.hpp"

DEF_TRACE_SCOPE(areg_mcrouter_WinMain);
DEF_TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_ServiceEntry);
DEF_TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_ServiceInstall);
DEF_TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_ServiceUninstall);
DEF_TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_SetState);
DEF_TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_ServiceStart);
DEF_TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_ServiceStop);
DEF_TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_ServicePause);
DEF_TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_ServiceContinue);

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
    CEMulticastRouter & broker = CEMulticastRouter::GetMulticastRouter();
    broker.SetCurrentCommand( CEMulticastRouter::CMD_Console );

    if ( argc > 1 )
    {
        const char * cmd = argv[1];

        if ( _isEqual( cmd, NEMulticastRouterSettings::ServiceCommandInstall) )
            broker.SetCurrentCommand( CEMulticastRouter::CMD_Install );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandUninstall) )
            broker.SetCurrentCommand( CEMulticastRouter::CMD_Uninstall );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandConsole) )
            broker.SetCurrentCommand( CEMulticastRouter::CMD_Console );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandService) )
            broker.SetCurrentCommand(CEMulticastRouter::CMD_Service);
    }

    switch ( CEMulticastRouter::GetMulticastRouter().GetCurrentCommand() )
    {
    case CEMulticastRouter::CMD_Install:
        result = CEMulticastRouter::GetMulticastRouter().ServiceInstall() ? 0 : -2;
        break;

    case CEMulticastRouter::CMD_Uninstall:
        CEMulticastRouter::GetMulticastRouter().ServiceUninstall();
        break;

    case CEMulticastRouter::CMD_Console:
        ::GServiceMain(argc, argv);
        CEMulticastRouter::GetMulticastRouter().ServiceStop();
        break;

    case CEMulticastRouter::CMD_Service:
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
    CEMulticastRouter & router = CEMulticastRouter::GetMulticastRouter();
    router.SetState(NEMulticastRouterSettings::RouterStarting);
    router.ServiceMain( static_cast<int>(argc), argv);
    router.SetState( NEMulticastRouterSettings::RouterStopped );
}

//////////////////////////////////////////////////////////////////////////
// Global functions, End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEMulticastRouter class implementation
//////////////////////////////////////////////////////////////////////////
bool CEMulticastRouter::_isValid( void ) const
{
    return true;
}

void CEMulticastRouter::_freeResources( void )
{
    mSvcHandle = NULL;
    mSeMHandle = NULL;
}

bool CEMulticastRouter::_openService(void)
{
    return true;
}

bool CEMulticastRouter::_createService(void)
{
    return true;
}

bool CEMulticastRouter::ServiceInstall(void)
{
    if ( _openService() == false )
        _createService();

    return _isValid();
}

void CEMulticastRouter::ServiceUninstall(void)
{
    _freeResources();
}

void CEMulticastRouter::ServiceMain( int argc, char ** argv )
{
    CEApplication::SetupWorkingDirectory();
    CEApplication::StartTracer(NEApplication::DEFAULT_TRACING_CONFIG_FILE, true);
    CEApplication::StartTimerManager();

    do 
    {
        TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_ServiceEntry);
        TRACE_DBG("Starting service. There are [ %d ] arguments in the list...", argc);

#ifdef  DEBUG
        for ( int i = 0; i < argc; ++ i )
            TRACE_DBG("... Command argument [ %d ]: [ %s ]", i, argv[i]);
#endif  // DEBUG

        mQuitEvent.SetEvent();

        if ( mServiceCmd == CEMulticastRouter::CMD_Console )
        {
            TRACE_DBG("Starting service");
            ServiceStart();
        }

        char cmd = '\0';
        while ( mServiceCmd == CEMulticastRouter::CMD_Console && cmd != 'q')
        {
            scanf("%c", &cmd);
            cmd = cmd == 'Q' ? 'q' : cmd;
        }

        mQuitEvent.Lock( mServiceCmd == CEMulticastRouter::CMD_Service ? IESynchObject::WAIT_INFINITE : IESynchObject::DO_NOT_WAIT );
        TRACE_WARN("Service Stopped and not running anymore");

    } while (false);

    CEApplication::Release();
}

bool CEMulticastRouter::ServiceStart(void)
{
    TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_ServiceStart);
    TRACE_DBG("Starting service [ %s ]", NEMulticastRouterSettings::ServiceNameA);
    bool result = false;
    if (  mServiceServer.ServiceConfigure( NEApplication::DEFAULT_ROUTER_CONFIG_FILE ) && mServiceServer.StartRemotingService() )
    {
        result = SetState(NEMulticastRouterSettings::RouterRunning);
        mQuitEvent.ResetEvent();
    }

    return result;
}

void CEMulticastRouter::ServicePause(void)
{
    TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_ServicePause);
    TRACE_DBG("Pausing Router service");

    SetState( NEMulticastRouterSettings::RouterPausing );
    mServiceServer.StopRemotingService();
    SetState( NEMulticastRouterSettings::RouterPaused );
}

bool CEMulticastRouter::ServiceContinue(void)
{
    TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_ServiceContinue);
    TRACE_DBG("Continueing Router service");

    bool result = false;
    SetState( NEMulticastRouterSettings::RouterContinuing );
    if ( mServiceServer.IsServiceConfigured() && mServiceServer.StartRemotingService() )
    {
        result = true;
        SetState( NEMulticastRouterSettings::RouterRunning );
    }
    else
    {
        TRACE_ERR("Failed to restart remote servicing");
        mQuitEvent.SetEvent();
    }
    return result;
}

void CEMulticastRouter::ServiceStop(void)
{
    TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_ServiceStop);
    TRACE_WARN("Stopping service [ %s ]", NEMulticastRouterSettings::ServiceNameA);
    SetState(NEMulticastRouterSettings::RouterStopping);
    mServiceServer.StopRemotingService();
    mQuitEvent.SetEvent();
}

bool CEMulticastRouter::SetState( NEMulticastRouterSettings::eRouterState newState )
{
    TRACE_SCOPE(areg_mcrouter_CEMulticastRouter_SetState);
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
