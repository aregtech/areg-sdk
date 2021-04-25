/************************************************************************
 * \file        mcrouter/private/CEMulticastRouterWin32.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Router, Multicast Router Service process
 ************************************************************************/
#include "mcrouter/src/CEMulticastRouter.hpp"

#ifdef WINDOWS

#pragma comment(lib, "areg.lib")
#pragma comment(lib, "advapi32.lib")

#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/CEFile.hpp"
#include "areg/src/base/CEProcess.hpp"
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/CEWideString.hpp"
#include "areg/src/appbase/CEApplication.hpp"
#include "areg/src/appbase/NEApplication.hpp"
#include "areg/src/trace/GETrace.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <shellapi.h>
#include "mcrouter/resources/resource.hpp"

#ifdef UNICODE
    #define ServiceName     NEMulticastRouterSettings::ServiceNameW
#else   // UNICODE
    #define ServiceName     NEMulticastRouterSettings::ServiceNameA
#endif  // UNICODE

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
VOID WINAPI GServiceMain(DWORD argc, LPTSTR * argv);
VOID WINAPI GServiceCtrlHandler(DWORD);

#ifdef UNICODE
    static CEWideString _serviceDescribe(NEApplication::ROUTER_SERVICE_NAME_WIDE);
#else
    static CEString     _serviceDescribe(NEApplication::ROUTER_SERVICE_NAME_ASCII);
#endif // UNICODE

static SERVICE_STATUS           gServiceStatus  = {0};
static SERVICE_STATUS_HANDLE    gStatusHandle   = NULL;
static SERVICE_TABLE_ENTRY      gServiceTable[ ]= 
{
      {_serviceDescribe.UnsafeBuffer(), &::GServiceMain}
    , {NULL, NULL}
};

inline static char ** _convertArguments( TCHAR ** argv, int argc)
{
    char ** argvTemp = argc != 0 ? DEBUG_NEW char * [argc] : NULL;
    if ( argvTemp != NULL )
    {
        for ( int i = 0; i < static_cast<int>(argc); ++ i )
        {
            TCHAR * entry   = argv[i];
            int length      = NEString::getStringLength<TCHAR>(entry);
            char * arg      = DEBUG_NEW char[ length + 1];
            NEString::copyString<char, TCHAR>(arg, length + 1, entry);
            argvTemp[i] = arg;
        }
    }
    return argvTemp;
}

inline static void _deleteArguments( char ** argv, int argc )
{
    if ( argv != NULL )
    {
        for ( int i = 0; i < static_cast<int>(argc); ++ i )
            delete [] argv[i];
        delete [] argv;
    }
}

inline static bool _isEqual( const char * strLeft, const char * strRight )
{
    return (NEString::compareStrings<char, char>(strLeft, strRight, NEString::CountAll, false) == 0);
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int result      = 0;
    char ** argvTemp = _convertArguments(argv, argc);
    CEMulticastRouter & router = CEMulticastRouter::GetMulticastRouter();
    router.SetCurrentCommand( CEMulticastRouter::CMD_Console );

    if ( argc > 1 )
    {
        const char * cmd = argvTemp[1];

        if ( _isEqual( cmd, NEMulticastRouterSettings::ServiceCommandInstall) )
            router.SetCurrentCommand( CEMulticastRouter::CMD_Install );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandUninstall) )
            router.SetCurrentCommand( CEMulticastRouter::CMD_Uninstall );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandConsole) )
            router.SetCurrentCommand( CEMulticastRouter::CMD_Console );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandService) )
            router.SetCurrentCommand(CEMulticastRouter::CMD_Service);
    }

    _deleteArguments(argvTemp, argc);

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
        result = ::StartServiceCtrlDispatcher(gServiceTable) ? 0 : -1;
        break;

    default:
        ASSERT(false);  // unexpected
        break;
    }

	return result;
}

VOID WINAPI GServiceMain( DWORD argc, LPTSTR * argv )
{
    NEMemory::ZeroData<SERVICE_STATUS>(gServiceStatus);
    gServiceStatus.dwServiceType            = SERVICE_WIN32_OWN_PROCESS;
    gServiceStatus.dwCurrentState           = SERVICE_STOPPED;
    gServiceStatus.dwControlsAccepted       = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP;
    gServiceStatus.dwWin32ExitCode          = NO_ERROR;
    gServiceStatus.dwServiceSpecificExitCode= 0;
    gServiceStatus.dwCheckPoint             = 0;    
    gServiceStatus.dwWaitHint               = 0;

    CEMulticastRouter & router = CEMulticastRouter::GetMulticastRouter();
    router.SetState(NEMulticastRouterSettings::RouterStarting);

    char ** argvTemp = _convertArguments(argv, static_cast<int>(argc));
    router.ServiceMain( static_cast<int>(argc), argvTemp);
    _deleteArguments(argvTemp, static_cast<int>(argc));

    router.SetState( NEMulticastRouterSettings::RouterStopped );
}

VOID WINAPI GServiceCtrlHandler (DWORD CtrlCode)
{
    switch ( CtrlCode )
    {
    case SERVICE_CONTROL_STOP:
        CEMulticastRouter::GetMulticastRouter().ServiceStop();
        break;

    case SERVICE_CONTROL_PAUSE:
        CEMulticastRouter::GetMulticastRouter().ServicePause();
        break;

    case SERVICE_CONTROL_CONTINUE:
        CEMulticastRouter::GetMulticastRouter().ServiceContinue();
        break;

    default:
        break;  // do nothing
    }
}

//////////////////////////////////////////////////////////////////////////
// Global functions, End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEMulticastRouter class implementation
//////////////////////////////////////////////////////////////////////////
bool CEMulticastRouter::_isValid( void ) const
{
    return (mSeMHandle != NULL && mSvcHandle != NULL);
}

void CEMulticastRouter::_freeResources( void )
{
    if ( mSvcHandle != NULL )
        ::CloseServiceHandle( reinterpret_cast<SC_HANDLE>(mSvcHandle) );
    if ( mSeMHandle != NULL )
        ::CloseServiceHandle( reinterpret_cast<SC_HANDLE>(mSeMHandle) );

    mSvcHandle = NULL;
    mSeMHandle = NULL;
}

bool CEMulticastRouter::_openService(void)
{
    if ( mSeMHandle == NULL )
        mSeMHandle = reinterpret_cast<void *>(::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS));
    
    if ( mSeMHandle != NULL )
    {
        if ( mSvcHandle == NULL )
            mSvcHandle = reinterpret_cast<void *>(::OpenService(reinterpret_cast<SC_HANDLE>(mSeMHandle), ServiceName, SERVICE_ALL_ACCESS));
    }
    
    return ( mSvcHandle != NULL );
}

bool CEMulticastRouter::_createService(void)
{
    if ( mSeMHandle == NULL )
        mSeMHandle = reinterpret_cast<void *>(::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS));
    
    if ( mSeMHandle != NULL )
    {
        if ( mSvcHandle == NULL )
        {
            TCHAR szPath[MAX_PATH];
            if( ::GetModuleFileName( NULL, szPath, MAX_PATH ) )
            {

#define DEVELOPMENT_PENDING     1

#if defined(DEVELOPMENT_PENDING) && (DEVELOPMENT_PENDING != 0)
                DWORD startType = SERVICE_DEMAND_START;
#else   // defined(DEVELOPMENT_PENDING) && (DEVELOPMENT_PENDING != 0)
                DWORD startType = SERVICE_AUTO_START;
#endif  // defined(DEVELOPMENT_PENDING) && (DEVELOPMENT_PENDING != 0)

                mSvcHandle = reinterpret_cast<void *>(::CreateService(reinterpret_cast<SC_HANDLE>(mSeMHandle), ServiceName, ServiceName, SERVICE_ALL_ACCESS
                                                                    , SERVICE_WIN32_OWN_PROCESS, startType, SERVICE_ERROR_NORMAL, szPath
                                                                    , NULL, NULL, NULL, NULL, NULL));
                if ( mSvcHandle != NULL )
                {
                    SERVICE_DESCRIPTION description;
                    description.lpDescription = _serviceDescribe.UnsafeBuffer();
                    ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_DESCRIPTION, &description);

                    SERVICE_FAILURE_ACTIONS_FLAG actionFlag = {1};
                    actionFlag.fFailureActionsOnNonCrashFailures = TRUE;
                    ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_FAILURE_ACTIONS_FLAG, &actionFlag);

                    SERVICE_FAILURE_ACTIONS failures = {0};
                    SC_ACTION actions[4];
                    int count = 4;

                    failures.dwResetPeriod  = INFINITE;
                    failures.lpRebootMsg    = NULL;
                    failures.lpCommand      = NULL;
                    failures.cActions       = count;
                    failures.lpsaActions    = actions;

                    // first failure
                    actions[0].Delay        = 500;
                    actions[0].Type         = SC_ACTION_RESTART;

                    // first second
                    actions[1].Delay        = 500;
                    actions[1].Type         = SC_ACTION_RESTART;

                    // third failure
                    actions[2].Delay        = 500;
                    actions[2].Type         = SC_ACTION_RESTART;

                    // fourth failure
                    actions[3].Delay        = 0;
                    actions[3].Type         = SC_ACTION_NONE;

                    actions[count - 1].Delay= 0;
                    actions[count - 1].Type = SC_ACTION_NONE;

                    ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_FAILURE_ACTIONS, &failures);

#if !defined(DEVELOPMENT_PENDING) || (DEVELOPMENT_PENDING == 0)
                    SERVICE_DELAYED_AUTO_START_INFO delay = {1};
                    delay.fDelayedAutostart = TRUE;
                    ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &delay);
#endif // !defined(DEVELOPMENT_PENDING) || (DEVELOPMENT_PENDING == 0)
                }
            }
        }
    }
    
    return ( mSvcHandle != NULL );
}

bool CEMulticastRouter::ServiceInstall(void)
{
    if ( _openService() == false )
        _createService();
    
    return _isValid();
}

void CEMulticastRouter::ServiceUninstall(void)
{
    if ( _openService() )
        ::DeleteService( reinterpret_cast<SC_HANDLE>(mSvcHandle) );
    
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
        if ( mServiceCmd == CEMulticastRouter::CMD_Service )
            gStatusHandle = ::RegisterServiceCtrlHandler(ServiceName, GServiceCtrlHandler);

        if ( mServiceCmd == CEMulticastRouter::CMD_Console || gStatusHandle != NULL )
        {
            TRACE_DBG("Starting service");
            ServiceStart();
        }

        char cmd = '\0';
        while ( mServiceCmd == CEMulticastRouter::CMD_Console && cmd != 'q')
        {
            ::scanf_s("%c", &cmd, 1);
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
    gServiceStatus.dwControlsAccepted   = 0;
    gServiceStatus.dwWin32ExitCode      = 0;

    if ( newState != mRouterState )
    {
        switch ( newState )
        {
        case NEMulticastRouterSettings::RouterStopped:
            gServiceStatus.dwCurrentState       = SERVICE_STOPPED;
            gServiceStatus.dwControlsAccepted   = 0;
            gServiceStatus.dwCheckPoint         = 7;
            gServiceStatus.dwWin32ExitCode      = ERROR_SUCCESS;
            break;

        case NEMulticastRouterSettings::RouterStarting:
            gServiceStatus.dwCurrentState       = SERVICE_START_PENDING;
            gServiceStatus.dwCheckPoint         = 1;
            break;

        case NEMulticastRouterSettings::RouterStopping:
            gServiceStatus.dwCurrentState       = SERVICE_STOP_PENDING;
            gServiceStatus.dwCheckPoint         = 6;
            break;

        case NEMulticastRouterSettings::RouterRunning:
            gServiceStatus.dwCurrentState       = SERVICE_RUNNING;
            gServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP;
            gServiceStatus.dwCheckPoint         = 2;
            break;

        case NEMulticastRouterSettings::RouterContinuing:
            gServiceStatus.dwCurrentState       = SERVICE_CONTINUE_PENDING;
            gServiceStatus.dwCheckPoint         = 5;
            break;

        case NEMulticastRouterSettings::RouterPausing:
            gServiceStatus.dwCurrentState       = SERVICE_PAUSE_PENDING;
            gServiceStatus.dwCheckPoint         = 3;
            break;

        case NEMulticastRouterSettings::RouterPaused:
            gServiceStatus.dwCurrentState       = SERVICE_PAUSED;
            gServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP;
            gServiceStatus.dwCheckPoint         = 4;
            break;

        default:
            ASSERT(false);
        }
        mRouterState = newState;
        if ( gStatusHandle != NULL )
            result = ::SetServiceStatus(gStatusHandle, &gServiceStatus) != FALSE;
    }

    return result;
}

#endif // WINDOWS
